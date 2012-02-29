// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// stat_proc_callback =======================================================

struct stat_proc_callback_t : public action_callback_t
{
  std::string name_str;
  int stat;
  double amount;
  timespan_t tick;
  stat_buff_t* buff;

  stat_proc_callback_t( const std::string& n, player_t* p, int s, int max_stacks, double a,
                        double proc_chance, timespan_t duration, timespan_t cooldown,
                        timespan_t t=timespan_t::zero, bool reverse=false, int rng_type=RNG_DEFAULT, bool activated=true ) :
    action_callback_t( p -> sim, p ),
    name_str( n ), stat( s ), amount( a ), tick( t )
  {
    if ( max_stacks == 0 ) max_stacks = 1;
    if ( proc_chance == 0 ) proc_chance = 1;
    if ( rng_type == RNG_DEFAULT ) rng_type = RNG_DISTRIBUTED;

    buff = new stat_buff_t( p, n, stat, amount, max_stacks, duration, cooldown, proc_chance, false, reverse, rng_type );
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
      if ( tick > timespan_t::zero ) // The buff stacks over time.
      {
        struct tick_stack_t : public event_t
        {
          stat_proc_callback_t* callback;
          tick_stack_t( sim_t* sim, player_t* p, stat_proc_callback_t* cb ) : event_t( sim, p ), callback( cb )
          {
            name = callback -> buff -> name();
            sim -> add_event( this, callback -> tick );
          }
          virtual void execute()
          {
            stat_buff_t* b = callback -> buff;
            if ( b -> current_stack > 0 &&
                 b -> current_stack < b -> max_stack )
            {
              b -> bump();
              new ( sim ) tick_stack_t( sim, player, callback );
            }
          }
        };

        new ( sim ) tick_stack_t( sim, a -> player, this );
      }
    }
  }
};

// cost_reduction_proc_callback =============================================

struct cost_reduction_proc_callback_t : public action_callback_t
{
  std::string name_str;
  int school;
  double amount;
  cost_reduction_buff_t* buff;

  cost_reduction_proc_callback_t( const std::string& n, player_t* p, int s, int max_stacks, double a,
                                  double proc_chance, timespan_t duration, timespan_t cooldown,
                                  bool refreshes=false, bool reverse=false, int rng_type=RNG_DEFAULT, bool activated=true ) :
    action_callback_t( p -> sim, p ),
    name_str( n ), school( s ), amount( a )
  {
    if ( max_stacks == 0 ) max_stacks = 1;
    if ( proc_chance == 0 ) proc_chance = 1;
    if ( rng_type == RNG_DEFAULT ) rng_type = RNG_DISTRIBUTED;

    buff = new cost_reduction_buff_t( p, n, school, amount, max_stacks, duration, cooldown, proc_chance, refreshes, false, reverse, rng_type );
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
                             const school_type school, double amount, double scaling,
                             double pc, timespan_t cd, bool no_crit, bool no_buffs, bool no_debuffs, int rng_type=RNG_DEFAULT ) :
    action_callback_t( p -> sim, p ),
    name_str( n ), stacks( 0 ), max_stacks( ms ), proc_chance( pc ), cooldown( 0 ), discharge_action( 0 ), proc( 0 ), rng( 0 )
  {
    if ( rng_type == RNG_DEFAULT ) rng_type = RNG_DISTRIBUTED;

    struct discharge_spell_t : public action_t
    {
      discharge_spell_t( const char* n, player_t* p, double amount, double scaling, const school_type s, bool no_crit, bool nb, bool nd ) :
        action_t( ACTION_ATTACK, n, p, force_policy, RESOURCE_NONE, s )
      {
        may_crit = ! no_crit;
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero;
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
      discharge_attack_t( const char* n, player_t* p, double amount, double scaling, const school_type s, bool no_crit, bool nb, bool nd ) :
        action_t( ACTION_ATTACK, n, p, melee_policy, RESOURCE_NONE, s )
      {
        may_crit = ! no_crit;
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero;
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
      cooldown = p -> get_cooldown( "damage_discharge_" + util_t::to_string( cd.total_seconds() ) + "cd" );
      discharge_action = new discharge_spell_t( name_str.c_str(), p, amount, scaling, school, no_crit, no_buffs, no_debuffs );
    }
    else
    {
      cooldown = p -> get_cooldown( name_str );
      discharge_action = new discharge_attack_t( name_str.c_str(), p, -amount, scaling, school, no_crit, no_buffs, no_debuffs );
    }

    cooldown -> duration = cd;

    proc = p -> get_proc( name_str.c_str() );
    rng  = p -> get_rng ( name_str.c_str(), rng_type );  // default is CYCLIC since discharge should not have duration
  }

  virtual void reset() { stacks=0; }

  virtual void deactivate() { action_callback_t::deactivate(); stacks=0; }

  virtual void trigger( action_t* a, void* /* call_data */ )
  {
    if ( cooldown -> remains() > timespan_t::zero )
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
      listener -> aura_gain( name_str.c_str(), stacks );
    }
    else
    {
      stacks = 0;
      if ( sim -> debug ) log_t::output( sim, "%s procs %s", a -> name(), discharge_action -> name() );
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
                                    const school_type school, double amount, double scaling,
                                    double pc, timespan_t cd, bool no_crit, bool no_buffs, bool no_debuffs, int rng_type=RNG_DEFAULT ) :
    action_callback_t( p -> sim, p ),
    name_str( n ), stacks( 0 ), max_stacks( ms ), proc_chance( pc )
  {
    if ( rng_type == RNG_DEFAULT ) rng_type = RNG_DISTRIBUTED;

    struct discharge_spell_t : public action_t
    {
      discharge_spell_t( const char* n, player_t* p, double amount, double scaling, const school_type s, bool no_crit, bool nb, bool nd ) :
        action_t( ACTION_ATTACK, n, p, force_policy, RESOURCE_NONE, s )
      {
        may_crit = ! no_crit;
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero;
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
      discharge_attack_t( const char* n, player_t* p, double amount, double scaling, const school_type s, bool no_crit, bool nb, bool nd ) :
        action_t( ACTION_ATTACK, n, p, melee_policy, RESOURCE_NONE, s )
      {
        may_crit = ! no_crit;
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero;
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
      cooldown = p -> get_cooldown( "damage_discharge_" + util_t::to_string( cd.total_seconds() ) + "cd" );
      discharge_action = new discharge_spell_t( name_str.c_str(), p, amount, scaling, school, no_crit, no_buffs, no_debuffs );
    }
    else
    {
      cooldown = p -> get_cooldown( name_str );
      discharge_action = new discharge_attack_t( name_str.c_str(), p, -amount, scaling, school, no_crit, no_buffs, no_debuffs );
    }

    cooldown -> duration = cd;

    proc = p -> get_proc( name_str.c_str() );
    rng  = p -> get_rng ( name_str.c_str(), rng_type );  // default is CYCLIC since discharge should not have duration
  }

  virtual void reset() { stacks=0; }

  virtual void deactivate() { action_callback_t::deactivate(); stacks=0; }

  virtual void trigger( action_t* a, void* /* call_data */ )
  {
    /* Always adds a stack if not on cooldown. The proc chance is the chance to discharge */
    if ( cooldown -> remains() > timespan_t::zero )
      return;

    if ( ! allow_self_procs && ( a == discharge_action ) ) return;

    cooldown -> start();

    if ( ++stacks < max_stacks )
    {
      listener -> aura_gain( name_str.c_str(), stacks );
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
                                  int stat, int max_stacks, double stat_amount,
                                  const school_type school, double discharge_amount, double discharge_scaling,
                                  double proc_chance, timespan_t duration, timespan_t cooldown, bool no_crit, bool no_buffs, bool no_debuffs, bool activated=true ) :
    action_callback_t( p -> sim, p ), name_str( n )
  {
    if ( max_stacks == 0 ) max_stacks = 1;
    if ( proc_chance == 0 ) proc_chance = 1;

    buff = new stat_buff_t( p, n, stat, stat_amount, max_stacks, duration, cooldown, proc_chance );
    buff -> activated = activated;

    struct discharge_spell_t : public action_t
    {
      discharge_spell_t( const char* n, player_t* p, double amount, double scaling, const school_type s, bool no_crit, bool nb, bool nd ) :
        action_t( ACTION_ATTACK, n, p, force_policy, RESOURCE_NONE, s )
      {
        may_crit = ! no_crit;
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero;
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

      discharge_attack_t( const char* n, player_t* p, double amount, double scaling, const school_type s, bool no_crit, bool nb, bool nd ) :
        action_t( ACTION_ATTACK, n, p, melee_policy, RESOURCE_NONE, s )
      {
        may_crit = ! no_crit;
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero;
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
      discharge_action = new discharge_spell_t( name_str.c_str(), p, discharge_amount, discharge_scaling, school, no_crit, no_buffs, no_debuffs );
    }
    else
    {
      discharge_action = new discharge_attack_t( name_str.c_str(), p, -discharge_amount, discharge_scaling, school, no_crit, no_buffs, no_debuffs );
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
                                                      int                stat,
                                                      int                max_stacks,
                                                      double             amount,
                                                      double             proc_chance,
                                                      timespan_t         duration,
                                                      timespan_t         cooldown,
                                                      timespan_t         tick,
                                                      bool               reverse,
                                                      int                rng_type )
{
  action_callback_t* cb = new stat_proc_callback_t( name, player, stat, max_stacks, amount, proc_chance, duration, cooldown, tick, reverse, rng_type, type == PROC_NONE );

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
                                                                int                school,
                                                                int                max_stacks,
                                                                double             amount,
                                                                double             proc_chance,
                                                                timespan_t         duration,
                                                                timespan_t         cooldown,
                                                                bool               refreshes,
                                                                bool               reverse,
                                                                int                rng_type )
{
  action_callback_t* cb = new cost_reduction_proc_callback_t( name, player, school, max_stacks, amount, proc_chance, duration, cooldown, refreshes, reverse, rng_type, type == PROC_NONE );

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
                                                           double             amount,
                                                           double             scaling,
                                                           double             proc_chance,
                                                           timespan_t         cooldown,
                                                           bool               no_crit,
                                                           bool               no_buffs,
                                                           bool               no_debuffs,
                                                           int                rng_type )
{
  action_callback_t* cb = new discharge_proc_callback_t( name, player, max_stacks, school, amount, scaling, proc_chance, cooldown,
                                                         no_crit, no_buffs, no_debuffs, rng_type );

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
                                                                  double             amount,
                                                                  double             scaling,
                                                                  double             proc_chance,
                                                                  timespan_t         cooldown,
                                                                  bool               no_crit,
                                                                  bool               no_buffs,
                                                                  bool               no_debuffs,
                                                                  int                rng_type )
{
  action_callback_t* cb = new chance_discharge_proc_callback_t( name, player, max_stacks, school, amount, scaling, proc_chance, cooldown,
                                                                no_crit, no_buffs, no_debuffs, rng_type );

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
                                                                int                max_stacks,
                                                                int                stat,
                                                                double             stat_amount,
                                                                const school_type  school,
                                                                double             min_dmg,
                                                                double             max_dmg,
                                                                double             proc_chance,
                                                                timespan_t         duration,
                                                                timespan_t         cooldown,
                                                                bool               no_crit,
                                                                bool               no_buffs,
                                                                bool               no_debuffs )
{
  action_callback_t* cb = new stat_discharge_proc_callback_t( name, player, stat, max_stacks, stat_amount, school, min_dmg, max_dmg, proc_chance,
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
  const char* name = e.name_str.empty() ? i.name() : e.name_str.c_str();

  return register_stat_proc( e.trigger_type, e.trigger_mask, name, i.player,
                             e.stat, e.max_stacks, e.stat_amount,
                             e.proc_chance, e.duration, e.cooldown, e.tick,
                             e.reverse, RNG_DEFAULT );
}

// ==========================================================================
// unique_gear_t::register_cost_reduction_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_cost_reduction_proc( item_t& i,
                                                                item_t::special_effect_t& e )
{
  const char* name = e.name_str.empty() ? i.name() : e.name_str.c_str();

  return register_cost_reduction_proc( e.trigger_type, e.trigger_mask, name, i.player,
                                       e.school, e.max_stacks, e.discharge_amount,
                                       e.proc_chance, e.duration, e.cooldown, ! e.no_refresh, e.reverse, RNG_DEFAULT );
}

// ==========================================================================
// unique_gear_t::register_discharge_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_discharge_proc( item_t& i,
                                                           item_t::special_effect_t& e )
{
  const char* name = e.name_str.empty() ? i.name() : e.name_str.c_str();

  return register_discharge_proc( e.trigger_type, e.trigger_mask, name, i.player,
                                  e.max_stacks, e.school, e.discharge_amount, e.discharge_scaling,
                                  e.proc_chance, e.cooldown, e.no_crit, e.no_player_benefits, e.no_debuffs );
}

// ==========================================================================
// unique_gear_t::register_chance_discharge_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_chance_discharge_proc( item_t& i,
                                                                  item_t::special_effect_t& e )
{
  const char* name = e.name_str.empty() ? i.name() : e.name_str.c_str();

  return register_chance_discharge_proc( e.trigger_type, e.trigger_mask, name, i.player,
                                         e.max_stacks, e.school, e.discharge_amount, e.discharge_scaling,
                                         e.proc_chance, e.cooldown, e.no_crit, e.no_player_benefits, e.no_debuffs );
}

// ==========================================================================
// unique_gear_t::register_stat_discharge_proc
// ==========================================================================

action_callback_t* unique_gear_t::register_stat_discharge_proc( item_t& i,
                                                                item_t::special_effect_t& e )
{
  const char* name = e.name_str.empty() ? i.name() : e.name_str.c_str();

  return register_stat_discharge_proc( e.trigger_type, e.trigger_mask, name, i.player,
                                       e.max_stacks, e.stat, e.stat_amount,
                                       e.school, e.discharge_amount, e.discharge_scaling,
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
                                        const std::string& /* id */ )
{
  std::string e;

  // SWTOR Equip Relics
       if ( name == "relentless_winds"                    ) e = "ondamage_224Kinetic_30%_4.5cd";
  else if ( name == "plasma_burst_device"                 ) e = "ondamage_168Elemental_30%_4.5cd";
  else if ( name == "energy_surge_device"                 ) e = "ondamage_224Energy_30%_4.5cd";
  else if ( name == "dark_energy_surge"                   ) e = "ondamage_168Internal_30%_4.5cd";
  else if ( name == "lightning_pillar"                    ) e = "ondamage_208Energy_30%_4.5cd";
  else if ( name == "gravity_wave"                        ) e = "Ondamage_208Kinetic_30%_4.5cd";
  else if ( name == "corrosive_injection_kit"             ) e = "ondamage_156Internal_30%_4.5cd";
  else if ( name == "burning_force_relic"                 ) e = "ondamage_170Kinetic_30%_4.5cd";
  else if ( name == "relic_of_kinetic_force"              ) e = "ondamage_224Kinetic_30%_4.5cd";
  else if ( name == "plasma_injection_nanobots"           ) e = "ondamage_128Elemental_30%_4.5cd";
  else if ( name == "volatile_energy_module"              ) e = "ondamage_170Energy_30%_4.5c";
  else if ( name == "relic_of_vital_rupturing"            ) e = "ondamage_128Internal_30%_4.5cd";

  if ( e.empty() ) return false;

  util_t::tolower( e );

  encoding = e;

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
  std::string e;

  // SWTOR Use Relics
       if ( name == "rakata_relic_of_the_primeval_fatesealer" ) e = "380Alacrity_120cd_20dur";
  else if ( name == "rakata_relic_of_forbidden_secrets"       ) e = "235Crit_235Surge_120cd_20dur";
  else if ( name == "rakata_relic_of_boundless_ages"          ) e = "380Power_120cd_20dur";
  else if ( name == "columi_relic_of_primeval_fatesealer"     ) e = "355Alacrity_120cd_20dur";
  else if ( name == "columi_relic_of_forbidden_secrets"       ) e = "220Crit_220Surge_120cd_20dur";
  else if ( name == "columi_relic_of_boundless_ages"          ) e = "355Power_120cd_20dur";
  else if ( name == "relic_of_primeval_fatesealer"            ) e = "300Alacrity_120cd_20dur";
  else if ( name == "relic_of_forbidden_secrets"              ) e = "185Crit_185Surge_120cd_20dur";
  else if ( name == "relic_of_boundless_ages"                 ) e = "300Power_120cd_20dur";

  if ( e.empty() ) return false;

  util_t::tolower( e );

  encoding = e;

  return true;
}
