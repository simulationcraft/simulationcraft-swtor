// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// stat_proc_callback =======================================================

struct stat_proc_callback_t : public action_callback_t
{
  std::string name_str;
  stat_type stat;
  double amount;
  timespan_t tick;
  stat_buff_t* buff;

  stat_proc_callback_t( const std::string& n, player_t* p, stat_type s, int max_stacks, double a,
                        double proc_chance, timespan_t duration, timespan_t cooldown,
                        timespan_t t=timespan_t::zero(), bool reverse=false, rng_type rt=RNG_DEFAULT, bool activated=true ) :
    action_callback_t( p ), name_str( n ), stat( s ), amount( a ), tick( t )
  {
    if ( max_stacks == 0 ) max_stacks = 1;
    if ( proc_chance == 0 ) proc_chance = 1;
    if ( rt == RNG_DEFAULT ) rt = RNG_DISTRIBUTED;

    buff = new stat_buff_t( p, n, stat, amount, max_stacks, duration, cooldown, proc_chance, false, reverse, rt );
    buff -> activated = activated;
  }

  virtual void activate()
  {
    action_callback_t::activate();
    if ( buff -> reverse ) buff -> trigger( buff -> max_stack );
  }

  virtual void deactivate()
  {
    action_callback_t::deactivate();
    buff -> expire();
  }

  virtual void trigger( action_t* a, void* /* call_data */ )
  {
    if ( buff -> trigger( a ) )
    {
      if ( tick > timespan_t::zero() ) // The buff stacks over time.
      {
        struct tick_stack_t : public event_t
        {
          stat_proc_callback_t* callback;

          tick_stack_t( player_t* p, stat_proc_callback_t* cb ) :
            event_t( p, callback -> buff -> name() ), callback( cb )
          { sim -> add_event( this, callback -> tick ); }

          virtual void execute()
          {
            stat_buff_t* b = callback -> buff;
            if ( b -> current_stack > 0 &&
                 b -> current_stack < b -> max_stack )
            {
              b -> bump();
              new ( sim ) tick_stack_t( player, callback );
            }
          }
        };

        new ( a -> player -> sim ) tick_stack_t( a -> player, this );
      }
    }
  }
};

// cost_reduction_proc_callback =============================================

struct cost_reduction_proc_callback_t : public action_callback_t
{
  std::string name_str;
  school_type school;
  double amount;
  cost_reduction_buff_t* buff;

  cost_reduction_proc_callback_t( const std::string& n, player_t* p, school_type s, int max_stacks, double a,
                                  double proc_chance, timespan_t duration, timespan_t cooldown,
                                  bool refreshes=false, bool reverse=false, rng_type rt=RNG_DEFAULT, bool activated=true ) :
    action_callback_t( p ), name_str( n ), school( s ), amount( a )
  {
    if ( max_stacks == 0 ) max_stacks = 1;
    if ( proc_chance == 0 ) proc_chance = 1;
    if ( rt == RNG_DEFAULT ) rt = RNG_DISTRIBUTED;

    buff = new cost_reduction_buff_t( p, n, school, amount, max_stacks, duration, cooldown, proc_chance, refreshes, false, reverse, rt );
    buff -> activated = activated;
  }

  virtual void activate()
  {
    action_callback_t::activate();
    if ( buff -> reverse ) buff -> trigger( buff -> max_stack );
  }

  virtual void deactivate()
  {
    action_callback_t::deactivate();
    buff -> expire();
  }

  virtual void trigger( action_t* a, void* /* call_data */ )
  {
    buff -> trigger( a );
  }
};

// discharge_proc_callback ==================================================

struct discharge_proc_callback_t : public action_callback_t
{
  std::string name_str;
  int stacks, max_stacks;
  double proc_chance;
  cooldown_t* cooldown;
  action_t* discharge_action;
  proc_t* proc;
  rng_t* rng;

  discharge_proc_callback_t( const std::string& n, player_t* p, int ms,
                             const school_type school, const attack_policy_t policy, double amount, double scaling,
                             double pc, timespan_t cd, bool no_crit, bool no_buffs, bool no_debuffs, rng_type rt=RNG_DEFAULT ) :
    action_callback_t( p ), name_str( n ), stacks( 0 ), max_stacks( ms ),
    proc_chance( pc ), cooldown( 0 ), discharge_action( 0 ), proc( 0 ), rng( 0 )
  {
    if ( rt == RNG_DEFAULT ) rt = RNG_DISTRIBUTED;

    struct discharge_spell_t : public action_t
    {
      discharge_spell_t( const std::string& n, player_t* p, double amount, double scaling, const school_type s, attack_policy_t pc, bool no_crit, bool nb, bool nd ) :
        action_t( ACTION_ATTACK, n, p, pc!=NULL ? pc : force_policy, RESOURCE_NONE, s )
      {
        may_crit = ! no_crit;
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero();
        dd.base_min = dd.base_max = amount;
        dd.power_mod = scaling;
        background  = true;
        no_buffs = nb;
        no_debuffs = nd;
        init();
      }
    };

    struct discharge_attack_t : public action_t
    {
      discharge_attack_t( const std::string& n, player_t* p, double amount, double scaling, const school_type s, attack_policy_t pc, bool no_crit, bool nb, bool nd ) :
        action_t( ACTION_ATTACK, n, p, pc!=NULL ? pc : melee_policy, RESOURCE_NONE, s )
      {
        may_crit = ! no_crit;
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero();
        dd.base_min = amount;
        dd.base_max = amount;
        dd.power_mod = scaling;
        background  = true;
        no_buffs = nb;
        no_debuffs = nd;
        init();
      }
    };

    if ( amount > 0 )
    {
      cooldown = p -> get_cooldown( "damage_discharge_" + util_t::to_string( cd ) + "cd" );
      discharge_action = new discharge_spell_t( name_str, p, amount, scaling, school, policy, no_crit, no_buffs, no_debuffs );
    }
    else
    {
      cooldown = p -> get_cooldown( name_str );
      discharge_action = new discharge_attack_t( name_str, p, -amount, scaling, school, policy, no_crit, no_buffs, no_debuffs );
    }

    cooldown -> duration = cd;

    proc = p -> get_proc( name_str );
    rng  = p -> get_rng ( name_str, rt );
  }

  virtual void reset() { stacks=0; }

  virtual void deactivate() { action_callback_t::deactivate(); stacks=0; }

  virtual void trigger( action_t* a, void* /* call_data */ )
  {
    if ( cooldown -> remains() != timespan_t::zero() )
      return;

    if ( ! allow_self_procs && ( a == discharge_action ) ) return;

    if ( a -> discharge_proc ) return;

    if ( proc_chance )
    {
      if ( proc_chance < 0 )
      {
        if ( ! rng -> roll( a -> ppm_proc_chance( -proc_chance ) ) )
          return;
      }
      else
      {
        if ( ! rng -> roll( proc_chance ) )
          return;
      }
    }
    else
      return;

    cooldown -> start();

    if ( ++stacks < max_stacks )
    {
      listener -> aura_gain( name_str, stacks );
    }
    else
    {
      stacks = 0;
      if ( listener -> sim -> debug ) log_t::output( listener -> sim, "%s procs %s", a -> name(), discharge_action -> name() );
      discharge_action -> execute();
      proc -> occur();
    }
  }
};

// chance_discharge_proc_callback ===========================================

struct chance_discharge_proc_callback_t : public action_callback_t
{
  std::string name_str;
  int stacks, max_stacks;
  double proc_chance;
  cooldown_t* cooldown;
  action_t* discharge_action;
  proc_t* proc;
  rng_t* rng;

  chance_discharge_proc_callback_t( const std::string& n, player_t* p, int ms,
                                    const school_type school, attack_policy_t policy, double amount, double scaling,
                                    double pc, timespan_t cd, bool no_crit, bool no_buffs, bool no_debuffs, rng_type rt=RNG_DEFAULT ) :
    action_callback_t( p ), name_str( n ), stacks( 0 ), max_stacks( ms ),
    proc_chance( pc )
  {
    if ( rt == RNG_DEFAULT ) rt = RNG_DISTRIBUTED;

    struct discharge_spell_t : public action_t
    {
      discharge_spell_t( const std::string& n, player_t* p, double amount, double scaling, const school_type s, attack_policy_t pc, bool no_crit, bool nb, bool nd ) :
        action_t( ACTION_ATTACK, n, p, pc!=NULL ? pc : force_policy, RESOURCE_NONE, s )
      {
        may_crit = ! no_crit;
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero();
        dd.base_min = amount;
        dd.base_max = amount;
        dd.power_mod = scaling;
        background  = true;
        no_buffs = nb;
        no_debuffs = nd;
        init();
      }
    };

    struct discharge_attack_t : public action_t
    {
      discharge_attack_t( const std::string& n, player_t* p, double amount, double scaling, const school_type s, attack_policy_t pc, bool no_crit, bool nb, bool nd ) :
        action_t( ACTION_ATTACK, n, p, pc!=NULL ? pc : melee_policy, RESOURCE_NONE, s )
      {
        may_crit = ! no_crit;
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero();
        dd.base_min = amount;
        dd.base_max = amount;
        dd.power_mod = scaling;
        background  = true;
        no_buffs = nb;
        no_debuffs = nd;
        init();
      }
    };

    if ( amount > 0 )
    {
      cooldown = p -> get_cooldown( "damage_discharge_" + util_t::to_string( cd ) + "cd" );
      discharge_action = new discharge_spell_t( name_str, p, amount, scaling, school, policy, no_crit, no_buffs, no_debuffs );
    }
    else
    {
      cooldown = p -> get_cooldown( name_str );
      discharge_action = new discharge_attack_t( name_str, p, -amount, scaling, school, policy, no_crit, no_buffs, no_debuffs );
    }

    cooldown -> duration = cd;

    proc = p -> get_proc( name_str );
    rng  = p -> get_rng ( name_str, rt );
  }

  virtual void reset() { stacks=0; }

  virtual void deactivate() { action_callback_t::deactivate(); stacks=0; }

  virtual void trigger( action_t* a, void* /* call_data */ )
  {
    /* Always adds a stack if not on cooldown. The proc chance is the chance to discharge */
    if ( cooldown -> remains() > timespan_t::zero() )
      return;

    if ( ! allow_self_procs && ( a == discharge_action ) ) return;

    cooldown -> start();

    if ( ++stacks < max_stacks )
    {
      listener -> aura_gain( name_str, stacks );
      if ( proc_chance )
      {
        if ( proc_chance < 0 )
        {
          if ( ! rng -> roll( a -> ppm_proc_chance( -proc_chance ) ) )
            return;
        }
        else
        {
          if ( ! rng -> roll( proc_chance ) )
            return;
        }
      }
      discharge_action -> dd.base_multiplier = stacks;
      discharge_action -> execute();
      stacks = 0;
      proc -> occur();
    }
    else
    {
      discharge_action -> dd.base_multiplier = stacks;
      discharge_action -> execute();
      stacks = 0;
      proc -> occur();
    }
  }
};

// stat_discharge_proc_callback =============================================

struct stat_discharge_proc_callback_t : public action_callback_t
{
  std::string name_str;
  stat_buff_t* buff;
  action_t* discharge_action;

  stat_discharge_proc_callback_t( const std::string& n, player_t* p,
                                  stat_type stat, int max_stacks, double stat_amount,
                                  const school_type school, attack_policy_t policy, double discharge_amount, double discharge_scaling,
                                  double proc_chance, timespan_t duration, timespan_t cooldown, bool no_crit, bool no_buffs, bool no_debuffs, bool activated=true ) :
    action_callback_t( p ), name_str( n )
  {
    if ( max_stacks == 0 ) max_stacks = 1;
    if ( proc_chance == 0 ) proc_chance = 1;

    buff = new stat_buff_t( p, n, stat, stat_amount, max_stacks, duration, cooldown, proc_chance );
    buff -> activated = activated;

    struct discharge_spell_t : public action_t
    {
      discharge_spell_t( const std::string& n, player_t* p, double amount, double scaling, const school_type s, attack_policy_t pc, bool no_crit, bool nb, bool nd ) :
        action_t( ACTION_ATTACK, n, p, pc!=NULL ? pc : force_policy, RESOURCE_NONE, s )
      {
        may_crit = ! no_crit;
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero();
        dd.base_min = amount;
        dd.base_max = amount;
        dd.power_mod = scaling;
        background  = true;
        no_buffs = nb;
        no_debuffs = nd;
        init();
      }
    };

    struct discharge_attack_t : public action_t
    {
      bool no_buffs;

      discharge_attack_t( const std::string& n, player_t* p, double amount, double scaling, const school_type s, attack_policy_t pc, bool no_crit, bool nb, bool nd ) :
        action_t( ACTION_ATTACK, n, p, pc!=NULL ? pc : melee_policy, RESOURCE_NONE, s )
      {
        may_crit = ! no_crit;
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero();
        dd.base_min = amount;
        dd.base_max = amount;
        dd.power_mod = scaling;
        background  = true;
        no_buffs = nb;
        no_debuffs = nd;
        init();
      }
    };

    if ( discharge_amount > 0 )
    {
      discharge_action = new discharge_spell_t( name_str, p, discharge_amount, discharge_scaling, school, policy, no_crit, no_buffs, no_debuffs );
    }
    else
    {
      discharge_action = new discharge_attack_t( name_str, p, -discharge_amount, discharge_scaling, school, policy, no_crit, no_buffs, no_debuffs );
    }
  }

  virtual void deactivate()
  {
    action_callback_t::deactivate();
    buff -> expire();
  }

  virtual void trigger( action_t* a, void* /* call_data */ )
  {
    if ( buff -> trigger( a ) )
    {
      if ( ! allow_self_procs && ( a == discharge_action ) ) return;
      discharge_action -> execute();
    }
  }
};



// ==========================================================================
// unique_gear_t::init
// ==========================================================================

void unique_gear_t::init( player_t* p )
{
  if ( p -> is_pet() ) return;

  int num_items = ( int ) p -> items.size();

  for ( int i=0; i < num_items; i++ )
  {
    item_t& item = p -> items[ i ];

    if ( item.equip.stat && item.equip.school )
    {
      register_stat_discharge_proc( item, item.equip );
    }
    else if ( item.equip.stat )
    {
      register_stat_proc( item, item.equip );
    }
    else if ( item.equip.cost_reduction && item.equip.school )
    {
      register_cost_reduction_proc( item, item.equip );
    }
    else if ( item.equip.school && item.equip.proc_chance && item.equip.chance_to_discharge )
    {
      register_chance_discharge_proc( item, item.equip );
    }
    else if ( item.equip.school )
    {
      register_discharge_proc( item, item.equip );
    }

  }
}

// ==========================================================================
// unique_gear_t::register_stat_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_stat_proc( int                type,
                                                      int64_t            mask,
                                                      const std::string& name,
                                                      player_t*          player,
                                                      stat_type          stat,
                                                      int                max_stacks,
                                                      double             amount,
                                                      double             proc_chance,
                                                      timespan_t         duration,
                                                      timespan_t         cooldown,
                                                      timespan_t         tick,
                                                      bool               reverse,
                                                      rng_type           rt )
{
  action_callback_t* cb = new stat_proc_callback_t( name, player, stat, max_stacks, amount, proc_chance, duration, cooldown, tick, reverse, rt, type == PROC_NONE );

  if ( type == PROC_DAMAGE || type == PROC_DAMAGE_HEAL )
  {
    player -> register_tick_damage_callback( mask, cb );
    player -> register_direct_damage_callback( mask, cb );
  }
  if ( type == PROC_HEAL || type == PROC_DAMAGE_HEAL )
  {
    player -> register_tick_heal_callback( mask, cb );
    player -> register_direct_heal_callback( mask, cb );
  }
  else if ( type == PROC_TICK_DAMAGE )
  {
    player -> register_tick_damage_callback( mask, cb );
  }
  else if ( type == PROC_DIRECT_DAMAGE )
  {
    player -> register_direct_damage_callback( mask, cb );
  }
  else if ( type == PROC_ATTACK )
  {
    player -> register_attack_callback( mask, cb );
  }
  else if ( type == PROC_SPELL )
  {
    player -> register_spell_callback( mask, cb );
  }
  else if ( type == PROC_TICK )
  {
    player -> register_tick_callback( mask, cb );
  }
  else if ( type == PROC_HARMFUL_SPELL )
  {
    player -> register_harmful_spell_callback( mask, cb );
  }
  else if ( type == PROC_HEAL_SPELL )
  {
    player -> register_heal_callback( mask, cb );
  }

  return cb;
}

// ==========================================================================
// unique_gear_t::register_cost_reduction_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_cost_reduction_proc( int                type,
                                                                int64_t            mask,
                                                                const std::string& name,
                                                                player_t*          player,
                                                                school_type        school,
                                                                int                max_stacks,
                                                                double             amount,
                                                                double             proc_chance,
                                                                timespan_t         duration,
                                                                timespan_t         cooldown,
                                                                bool               refreshes,
                                                                bool               reverse,
                                                                rng_type           rt )
{
  action_callback_t* cb = new cost_reduction_proc_callback_t( name, player, school, max_stacks, amount, proc_chance, duration, cooldown, refreshes, reverse, rt, type == PROC_NONE );

  if ( type == PROC_DAMAGE || type == PROC_DAMAGE_HEAL )
  {
    player -> register_tick_damage_callback( mask, cb );
    player -> register_direct_damage_callback( mask, cb );
  }
  if ( type == PROC_HEAL || type == PROC_DAMAGE_HEAL )
  {
    player -> register_tick_heal_callback( mask, cb );
    player -> register_direct_heal_callback( mask, cb );
  }
  else if ( type == PROC_TICK_DAMAGE )
  {
    player -> register_tick_damage_callback( mask, cb );
  }
  else if ( type == PROC_DIRECT_DAMAGE )
  {
    player -> register_direct_damage_callback( mask, cb );
  }
  else if ( type == PROC_TICK )
  {
    player -> register_tick_callback( mask, cb );
  }
  else if ( type == PROC_ATTACK )
  {
    player -> register_attack_callback( mask, cb );
  }
  else if ( type == PROC_SPELL )
  {
    player -> register_spell_callback( mask, cb );
  }
  else if ( type == PROC_HARMFUL_SPELL )
  {
    player -> register_harmful_spell_callback( mask, cb );
  }
  else if ( type == PROC_HEAL_SPELL )
  {
    player -> register_heal_callback( mask, cb );
  }

  return cb;
}

// ==========================================================================
// unique_gear_t::register_discharge_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_discharge_proc( int                type,
                                                           int64_t            mask,
                                                           const std::string& name,
                                                           player_t*          player,
                                                           int                max_stacks,
                                                           const school_type  school,
                                                           attack_policy_t    policy,
                                                           double             amount,
                                                           double             scaling,
                                                           double             proc_chance,
                                                           timespan_t         cooldown,
                                                           bool               no_crit,
                                                           bool               no_buffs,
                                                           bool               no_debuffs,
                                                           rng_type           rt )
{
  action_callback_t* cb = new discharge_proc_callback_t( name, player, max_stacks, school, policy, amount, scaling, proc_chance, cooldown,
                                                         no_crit, no_buffs, no_debuffs, rt );

  if ( type == PROC_DAMAGE || type == PROC_DAMAGE_HEAL )
  {
    player -> register_tick_damage_callback( mask, cb );
    player -> register_direct_damage_callback( mask, cb );
  }
  if ( type == PROC_HEAL || type == PROC_DAMAGE_HEAL )
  {
    player -> register_tick_heal_callback( mask, cb );
    player -> register_direct_heal_callback( mask, cb );
  }
  else if ( type == PROC_TICK_DAMAGE )
  {
    player -> register_tick_damage_callback( mask, cb );
  }
  else if ( type == PROC_DIRECT_DAMAGE )
  {
    player -> register_direct_damage_callback( mask, cb );
  }
  else if ( type == PROC_TICK )
  {
    player -> register_tick_callback( mask, cb );
  }
  else if ( type == PROC_ATTACK )
  {
    player -> register_attack_callback( mask, cb );
  }
  else if ( type == PROC_SPELL )
  {
    player -> register_spell_callback( mask, cb );
  }
  else if ( type == PROC_SPELL_AND_TICK )
  {
    player -> register_spell_callback( mask, cb );
    player -> register_tick_callback( mask, cb );
  }
  else if ( type == PROC_HARMFUL_SPELL )
  {
    player -> register_harmful_spell_callback( mask, cb );
  }
  else if ( type == PROC_HEAL_SPELL )
  {
    player -> register_heal_callback( mask, cb );
  }

  return cb;
}

// ==========================================================================
// unique_gear_t::register_chance_discharge_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_chance_discharge_proc( int                type,
                                                                  int64_t            mask,
                                                                  const std::string& name,
                                                                  player_t*          player,
                                                                  int                max_stacks,
                                                                  const school_type  school,
                                                                  attack_policy_t    policy,
                                                                  double             amount,
                                                                  double             scaling,
                                                                  double             proc_chance,
                                                                  timespan_t         cooldown,
                                                                  bool               no_crit,
                                                                  bool               no_buffs,
                                                                  bool               no_debuffs,
                                                                  rng_type           rt )
{
  action_callback_t* cb = new chance_discharge_proc_callback_t( name, player, max_stacks, school, policy, amount, scaling, proc_chance, cooldown,
                                                                no_crit, no_buffs, no_debuffs, rt );

  if ( type == PROC_DAMAGE || type == PROC_DAMAGE_HEAL )
  {
    player -> register_tick_damage_callback( mask, cb );
    player -> register_direct_damage_callback( mask, cb );
  }
  if ( type == PROC_HEAL  || type == PROC_DAMAGE_HEAL )
  {
    player -> register_tick_heal_callback( mask, cb );
    player -> register_direct_heal_callback( mask, cb );
  }
  else if ( type == PROC_TICK_DAMAGE )
  {
    player -> register_tick_damage_callback( mask, cb );
  }
  else if ( type == PROC_DIRECT_DAMAGE )
  {
    player -> register_direct_damage_callback( mask, cb );
  }
  else if ( type == PROC_TICK )
  {
    player -> register_tick_callback( mask, cb );
  }
  else if ( type == PROC_ATTACK )
  {
    player -> register_attack_callback( mask, cb );
  }
  else if ( type == PROC_SPELL )
  {
    player -> register_spell_callback( mask, cb );
  }
  else if ( type == PROC_SPELL_AND_TICK )
  {
    player -> register_spell_callback( mask, cb );
    player -> register_tick_callback( mask, cb );
  }
  else if ( type == PROC_HARMFUL_SPELL )
  {
    player -> register_harmful_spell_callback( mask, cb );
  }
  else if ( type == PROC_HEAL_SPELL )
  {
    player -> register_heal_callback( mask, cb );
  }

  return cb;
}

// ==========================================================================
// unique_gear_t::register_stat_discharge_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_stat_discharge_proc( int                type,
                                                                int64_t            mask,
                                                                const std::string& name,
                                                                player_t*          player,
                                                                stat_type          stat,
                                                                int                max_stacks,
                                                                double             stat_amount,
                                                                school_type        school,
                                                                attack_policy_t    policy,
                                                                double             min_dmg,
                                                                double             max_dmg,
                                                                double             proc_chance,
                                                                timespan_t         duration,
                                                                timespan_t         cooldown,
                                                                bool               no_crit,
                                                                bool               no_buffs,
                                                                bool               no_debuffs )
{
  action_callback_t* cb = new stat_discharge_proc_callback_t( name, player, stat, max_stacks, stat_amount, school, policy, min_dmg, max_dmg, proc_chance,
                                                              duration, cooldown, no_crit, no_buffs, no_debuffs, type == PROC_NONE );

  if ( type == PROC_DAMAGE || type == PROC_DAMAGE_HEAL )
  {
    player -> register_tick_damage_callback( mask, cb );
    player -> register_direct_damage_callback( mask, cb );
  }
  if ( type == PROC_HEAL  || type == PROC_DAMAGE_HEAL )
  {
    player -> register_tick_heal_callback( mask, cb );
    player -> register_direct_heal_callback( mask, cb );
  }
  else if ( type == PROC_TICK_DAMAGE )
  {
    player -> register_tick_damage_callback( mask, cb );
  }
  else if ( type == PROC_DIRECT_DAMAGE )
  {
    player -> register_direct_damage_callback( mask, cb );
  }
  else if ( type == PROC_TICK )
  {
    player -> register_tick_callback( mask, cb );
  }
  else if ( type == PROC_ATTACK )
  {
    player -> register_attack_callback( mask, cb );
  }
  else if ( type == PROC_SPELL )
  {
    player -> register_spell_callback( mask, cb );
  }
  else if ( type == PROC_HARMFUL_SPELL )
  {
    player -> register_harmful_spell_callback( mask, cb );
  }
  else if ( type == PROC_HEAL_SPELL )
  {
    player -> register_heal_callback( mask, cb );
  }

  return cb;
}

// ==========================================================================
// unique_gear_t::register_stat_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_stat_proc( item_t& i,
                                                      item_t::special_effect_t& e )
{
  std::string name = e.name_str;
  if ( name.empty() ) name = i.name();

  return register_stat_proc( e.trigger_type, e.trigger_mask, name,
                             i.player, e.stat, e.max_stacks, e.stat_amount,
                             e.proc_chance, e.duration, e.cooldown, e.tick,
                             e.reverse, RNG_DEFAULT );
}

// ==========================================================================
// unique_gear_t::register_cost_reduction_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_cost_reduction_proc( item_t& i,
                                                                item_t::special_effect_t& e )
{
  std::string name = e.name_str;
  if ( name.empty() ) name = i.name();

  return register_cost_reduction_proc( e.trigger_type, e.trigger_mask, name,
                                       i.player, e.school, e.max_stacks, e.discharge_amount,
                                       e.proc_chance, e.duration, e.cooldown, ! e.no_refresh, e.reverse, RNG_DEFAULT );
}

// ==========================================================================
// unique_gear_t::register_discharge_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_discharge_proc( item_t& i,
                                                           item_t::special_effect_t& e )
{
  std::string name = e.name_str;
  if ( name.empty() ) name = i.name();

  return register_discharge_proc( e.trigger_type, e.trigger_mask, name, i.player,
                                  e.max_stacks, e.school, e.policy, e.discharge_amount, e.discharge_scaling,
                                  e.proc_chance, e.cooldown, e.no_crit, e.no_player_benefits, e.no_debuffs );
}

// ==========================================================================
// unique_gear_t::register_chance_discharge_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_chance_discharge_proc( item_t& i,
                                                                  item_t::special_effect_t& e )
{
  std::string name = e.name_str;
  if ( name.empty() ) name = i.name();

  return register_chance_discharge_proc( e.trigger_type, e.trigger_mask, name, i.player,
                                         e.max_stacks, e.school, e.policy, e.discharge_amount, e.discharge_scaling,
                                         e.proc_chance, e.cooldown, e.no_crit, e.no_player_benefits, e.no_debuffs );
}

// ==========================================================================
// unique_gear_t::register_stat_discharge_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_stat_discharge_proc( item_t& i,
                                                                item_t::special_effect_t& e )
{
  std::string name = e.name_str;
  if ( name.empty() ) name = i.name();

  return register_stat_discharge_proc( e.trigger_type, e.trigger_mask, name, i.player,
                                       e.stat, e.max_stacks, e.stat_amount,
                                       e.school, e.policy, e.discharge_amount, e.discharge_scaling,
                                       e.proc_chance, e.duration, e.cooldown, e.no_crit, e.no_player_benefits, e.no_debuffs );
}

// ==========================================================================
// unique_gear_t::get_equip_encoding
// ==========================================================================

bool unique_gear_t::get_equip_encoding( std::string&       encoding,
                                        const std::string& name,
                                        const bool         /* heroic */,
                                        const bool         /* lfr */,
                                        const bool         /* ptr */,
                                        const std::string& /*  id i*/ )
{
  const char* e = nullptr;

  // SWTOR Equip Relics

  // Old Relics
  if      ( name == "relentless_winds"                              ) e = "ondamage_224Kinetic_30%_4.5cd";
  else if ( name == "plasma_burst_device"                           ) e = "ondamage_168Elemental_30%_4.5cd";
  else if ( name == "energy_surge_device"                           ) e = "ondamage_224Energy_30%_4.5cd";
  else if ( name == "dark_energy_surge"                             ) e = "ondamage_168Internal_30%_4.5cd";
  else if ( name == "lightning_pillar"                              ) e = "ondamage_208Energy_30%_4.5cd";
  else if ( name == "gravity_wave"                                  ) e = "ondamage_208Kinetic_30%_4.5cd";
  else if ( name == "corrosive_injection_kit"                       ) e = "ondamage_156Internal_30%_4.5cd";
  else if ( name == "burning_force_relic"                           ) e = "ondamage_156Elemental_30%_4.5cd";
  else if ( name == "relic_of_kinetic_force"                        ) e = "ondamage_170Kinetic_30%_4.5cd";
  else if ( name == "plasma_injection_nanobots"                     ) e = "ondamage_128Elemental_30%_4.5cd";
  else if ( name == "volatile_energy_module"                        ) e = "ondamage_170Energy_30%_4.5c";
  else if ( name == "relic_of_vital_rupturing"                      ) e = "ondamage_128Internal_30%_4.5cd";
  else if ( name == "battlemaster_relic_of_dark_radiance"           ) e = "ondamage_168internal_30%_4.5cd";
  else if ( name == "battlemaster_relic_of_elemental_transcendence" ) e = "ondamage_168elemental_30%_4.5cd";
  else if ( name == "battlemaster_relic_of_the_cerulean_nova"       ) e = "ondamage_224energy_30%_4.5cd";
  else if ( name == "battlemaster_relic_of_the_kinetic_tempest"     ) e = "ondamage_224kinetic_30%_4.5cd";
  else if ( name == "campaign_relic_of_dark_radiance"               ) e = "ondamage_184internal_30%_4.5cd";
  else if ( name == "campaign_relic_of_elemental_transcendence"     ) e = "ondamage_184elemental_30%_4.5cd";
  else if ( name == "campaign_relic_of_the_cerulean_nova"           ) e = "ondamage_246energy_30%_4.5cd";
  else if ( name == "campaign_relic_of_the_kinetic_tempest"         ) e = "ondamage_246kinetic_30%_4.5cd";
  else if ( name == "denova_relic_of_dark_radiance"                 ) e = "ondamage_199internal_30%_4.5cd";
  else if ( name == "denova_relic_of_elemental_transcendence"       ) e = "ondamage_199elemental_30%_4.5cd";
  else if ( name == "denova_relic_of_the_cerulean_nova"             ) e = "ondamage_256energy_30%_4.5cd";
  else if ( name == "denova_relic_of_the_kinetic_tempest"           ) e = "ondamage_256kinetic_30%_4.5cd";
  else if ( name == "war_hero_relic_of_dark_radiance"               ) e = "ondamage_184internal_30%_4.5cd";
  else if ( name == "war_hero_relic_of_elemental_transcendence"     ) e = "ondamage_184elemental_30%_4.5cd";
  else if ( name == "war_hero_relic_of_the_cerulean_nova"           ) e = "ondamage_246energy_30%_4.5cd";
  else if ( name == "war_hero_relic_of_the_kinetic_tempest"         ) e = "ondamage_246kinetic_30%_4.5cd";
  else if ( name == "elite_war_hero_relic_of_dark_radiance"         ) e = "ondamage_199internal_30%_4.5cd";
  else if ( name == "elite_war_hero_relic_of_elemental_transcendence") e= "ondamage_199elemental_30%_4.5cd";
  else if ( name == "elite_war_hero_relic_of_the_cerulean_nova"     ) e = "ondamage_265energy_30%_4.5cd";
  else if ( name == "elite_war_hero_relic_of_the_kinetic_tempest"   ) e = "ondamage_265kinetic_30%_4.5cd";
  // XXX still misspelled in torhead and/or MrRobot (Old Relics)
  else if ( name == "battlemaster_relic_of_elemental_transcendance" ) e = "ondamage_168elemental_30%_4.5cd";
  else if ( name == "war_hero_relic_of_elemental_transcendance"     ) e = "ondamage_184elemental_30%_4.5cd";
  else if ( name == "elite_war_hero_relic_of_elemental_transcendance") e= "ondamage_199elemental_30%_4.5cd";

  // Current Relics
  else if ( name == "dread_guard_relic_of_dark_radiance"            ) e = "ondamage_210internal_tech_30%_4.5cd";
  else if ( name == "dread_guard_relic_of_elemental_transcendence"  ) e = "ondamage_210elemental_force_30%_4.5cd";
  else if ( name == "dread_guard_relic_of_the_cerulean_nova"        ) e = "ondamage_280energy_force_30%_4.5cd";
  else if ( name == "dread_guard_relic_of_the_kinetic_tempest"      ) e = "ondamage_280kinetic_tech_30%_4.5cd";

  else if ( name == "arkanian_relic_of__serendipitous_assault"      ) e = "ondamage_510Power_30%_20cd_6dur";
  else if ( name == "arkanian_relic_of_dark_radiance"               ) e = "ondamage_241internal_force_30%_4.5cd";
  else if ( name == "arkanian_relic_of_elemental_transcendence"     ) e = "ondamage_241elemental_tech_30%_4.5cd";
  else if ( name == "arkanian_relic_of_the_cerulean_nova"           ) e = "ondamage_322energy_tech_30%_4.5cd";
  else if ( name == "arkanian_relic_of_the_kinetic_tempest"         ) e = "ondamage_322kinetic_force_30%_4.5cd";

  else if ( name == "underworld_relic_of__serendipitous_assault"    ) e = "ondamage_550Power_30%_20cd_6dur";
  else if ( name == "underworld_relic_of_dark_radiance"             ) e = "ondamage_264internal_force_30%_4.5cd";  // Both Force in-game... :(
  else if ( name == "underworld_relic_of_elemental_transcendence"   ) e = "ondamage_264elemental_force_30%_4.5cd"; // Both Force in-game... :(
  else if ( name == "underworld_relic_of_the_cerulean_nova"         ) e = "ondamage_352energy_tech_30%_4.5cd";     // Only one Tech relic...
  else if ( name == "underworld_relic_of_the_kinetic_tempest"       ) e = "ondamage_352kinetic_force_30%_4.5cd";

  else if ( name == "kell_dragon_relic_of__serendipitous_assault"   ) e = "ondamage_595Power_30%_20cd_6dur";
  else if ( name == "kell_dragon_relic_of_dark_radiance"            ) e = "ondamage_287internal_force_30%_4.5cd";  // Both Force in-game... :(
  else if ( name == "kell_dragon_relic_of_elemental_transcendence"  ) e = "ondamage_287elemental_force_30%_4.5cd"; // Both Force in-game... :(
  else if ( name == "kell_dragon_relic_of_the_cerulean_nova"        ) e = "ondamage_382energy_tech_30%_4.5cd";     // Only Tech relic...
  else if ( name == "kell_dragon_relic_of_the_kinetic_tempest"      ) e = "ondamage_382kinetic_force_30%_4.5cd";

  else if ( name == "partisan_relic_of__serendipitous_assault"      ) e = "ondamage_410Power_30%_20cd_6dur";
  else if ( name == "partisan_relic_of_dark_radiance"               ) e = "ondamage_190internal_force_30%_4.5cd";
  else if ( name == "partisan_relic_of_elemental_transcendence"     ) e = "ondamage_190elemental_tech_30%_4.5cd";
  else if ( name == "partisan_relic_of_the_cerulean_nova"           ) e = "ondamage_254energy_tech_30%_4.5cd";
  else if ( name == "partisan_relic_of_the_kinetic_tempest"         ) e = "ondamage_254kinetic_force_30%_4.5cd";

  else if ( name == "conqueror_relic_of__serendipitous_assault"     ) e = "ondamage_435Power_30%_20cd_6dur";
  else if ( name == "conqueror_relic_of_dark_radiance"              ) e = "ondamage_202internal_force_30%_4.5cd";
  else if ( name == "conqueror_relic_of_elemental_transcendence"    ) e = "ondamage_202elemental_tech_30%_4.5cd";
  else if ( name == "conqueror_relic_of_the_cerulean_nova"          ) e = "ondamage_269energy_tech_30%_4.5cd";
  else if ( name == "conqueror_relic_of_the_kinetic_tempest"        ) e = "ondamage_269kinetic_force_30%_4.5cd";

  if ( ! e ) return false;

  encoding = e;
  util_t::tolower( encoding );

  return true;
}

// ==========================================================================
// unique_gear_t::get_use_encoding
// ==========================================================================

bool unique_gear_t::get_use_encoding( std::string&       encoding,
                                      const std::string& name,
                                      const bool         /* heroic */,
                                      const bool         /* lfr */,
                                      const bool         /* ptr */,
                                      const std::string& /* id */ )
{
  const char* e = nullptr;

  // SWTOR Use Relics
  if      ( name == "rakata_relic_of_boundless_ages"               ) e = "290Power_120cd_30dur";
  else if ( name == "rakata_relic_of_forbidden_secrets"            ) e = "175Crit_175Surge_120cd_30dur";
  else if ( name == "rakata_relic_of_the_primeval_fatesealer"      ) e = "290Alacrity_120cd_30dur";
  else if ( name == "columi_relic_of_boundless_ages"               ) e = "270Power_120cd_30dur";
  else if ( name == "columi_relic_of_forbidden_secrets"            ) e = "165Crit_165Surge_120cd_30dur";
  else if ( name == "columi_relic_of_primeval_fatesealer"          ) e = "270Alacrity_120cd_30dur";
  else if ( name == "relic_of_boundless_ages"                      ) e = "225Power_120cd_30dur";
  else if ( name == "relic_of_forbidden_secrets"                   ) e = "135Crit_135Surge_120cd_30dur";
  else if ( name == "relic_of_primeval_fatesealer"                 ) e = "225Alacrity_120cd_30dur";
  else if ( name == "campaign_relic_of_boundless_ages"             ) e = "315Power_120cd_30dur";
  else if ( name == "campaign_relic_of_imperiling_serenity"        ) e = "315Defense_120cd_30dur";
  else if ( name == "campaign_relic_of_the_primeval_fatesealer"    ) e = "315Alacrity_120cd_30dur";
  else if ( name == "campaign_relic_of_the_shrouded_crusader"      ) e = "190shield_190abs_120cd_30dur";
  else if ( name == "dread_guard_relic_of_boundless_ages"          ) e = "350Power_120cd_30dur";
  else if ( name == "dread_guard_relic_of_imperiling_serenity"     ) e = "350Defense_120cd_30dur";
  else if ( name == "dread_guard_relic_of_the_primeval_fatesealer" ) e = "350Alacrity_120cd_30dur";
  else if ( name == "dread_guard_relic_of_the_shrouded_crusader"   ) e = "215shield_215abs_120cd_30dur";
  else if ( name == "arkanian_relic_of_boundless_ages"             ) e = "395Power_120cd_30dur";
  else if ( name == "arkanian_relic_of_the_primeval_fatesealer"    ) e = "395Alacrity_120cd_30dur";
  else if ( name == "underworld_relic_of_boundless_ages"           ) e = "425Power_120cd_30dur";
  else if ( name == "underworld_relic_of_the_primeval_fatesealer"  ) e = "425Alacrity_120cd_30dur";
  else if ( name == "kell_dragon_relic_of_boundless_ages"          ) e = "460Power_120cd_30dur";
  else if ( name == "kell_dragon_relic_of_the_primeval_fatesealer" ) e = "460Alacrity_120cd_30dur";

  if ( ! e ) return false;

  encoding = e;
  util_t::tolower( encoding );

  return true;
}
