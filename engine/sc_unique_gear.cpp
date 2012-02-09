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

    struct discharge_spell_t : public spell_t
    {
      discharge_spell_t( const char* n, player_t* p, double amount, double scaling, const school_type s, bool no_crit, bool nb, bool nd ) :
        spell_t( n, p, RESOURCE_NONE, ( s == SCHOOL_DRAIN ) ? SCHOOL_SHADOW : s )
      {
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero;
        base_dd_min = amount;
        base_dd_max = amount;
        direct_power_mod = scaling;
        may_crit = ( s != SCHOOL_DRAIN ) && ! no_crit;
        background  = true;
        no_buffs = nb;
        no_debuffs = nd;
        init();
      }
    };

    struct discharge_attack_t : public attack_t
    {
      discharge_attack_t( const char* n, player_t* p, double amount, double scaling, const school_type s, bool no_crit, bool nb, bool nd ) :
        attack_t( n, p, RESOURCE_NONE, ( s == SCHOOL_DRAIN ) ? SCHOOL_SHADOW : s )
      {
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero;
        base_dd_min = amount;
        base_dd_max = amount;
        direct_power_mod = scaling;
        may_crit = ( s != SCHOOL_DRAIN ) && ! no_crit;
        may_dodge = ( s == SCHOOL_PHYSICAL && ! no_crit );
        may_parry = ( s == SCHOOL_PHYSICAL && ! no_crit && ( p -> position == POSITION_FRONT || p -> position == POSITION_RANGED_FRONT ) );
        may_glance = false;
        background  = true;
        no_buffs = nb;
        no_debuffs = nd;
        init();
      }
    };

    cooldown = p -> get_cooldown( name_str );
    cooldown -> duration = cd;

    if ( amount > 0 )
    {
      discharge_action = new discharge_spell_t( name_str.c_str(), p, amount, scaling, school, no_crit, no_buffs, no_debuffs );
    }
    else
    {
      discharge_action = new discharge_attack_t( name_str.c_str(), p, -amount, scaling, school, no_crit, no_buffs, no_debuffs );
    }

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

    struct discharge_spell_t : public spell_t
    {
      discharge_spell_t( const char* n, player_t* p, double amount, double scaling, const school_type s, bool no_crit, bool nb, bool nd ) :
        spell_t( n, p, RESOURCE_NONE, ( s == SCHOOL_DRAIN ) ? SCHOOL_SHADOW : s )
      {
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero;
        base_dd_min = amount;
        base_dd_max = amount;
        direct_power_mod = scaling;
        may_crit = ( s != SCHOOL_DRAIN ) && ! no_crit;
        background  = true;
        no_buffs = nb;
        no_debuffs = nd;
        init();
      }
    };

    struct discharge_attack_t : public attack_t
    {
      discharge_attack_t( const char* n, player_t* p, double amount, double scaling, const school_type s, bool no_crit, bool nb, bool nd ) :
        attack_t( n, p, RESOURCE_NONE, ( s == SCHOOL_DRAIN ) ? SCHOOL_SHADOW : s )
      {
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero;
        base_dd_min = amount;
        base_dd_max = amount;
        direct_power_mod = scaling;
        may_crit = ( s != SCHOOL_DRAIN ) && ! no_crit;
        may_dodge = ( s == SCHOOL_PHYSICAL && ! no_crit );
        may_parry = ( s == SCHOOL_PHYSICAL && ! no_crit && ( p -> position == POSITION_FRONT || p -> position == POSITION_RANGED_FRONT ) );
        may_glance = false;
        background  = true;
        no_buffs = nb;
        no_debuffs = nd;
        init();
      }
    };

    cooldown = p -> get_cooldown( name_str );
    cooldown -> duration = cd;

    if ( amount > 0 )
    {
      discharge_action = new discharge_spell_t( name_str.c_str(), p, amount, scaling, school, no_crit, no_buffs, no_debuffs );
    }
    else
    {
      discharge_action = new discharge_attack_t( name_str.c_str(), p, -amount, scaling, school, no_crit, no_buffs, no_debuffs );
    }

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
      discharge_action -> base_dd_multiplier = stacks;
      discharge_action -> execute();
      stacks = 0;
      proc -> occur();
    }
    else
    {
      discharge_action -> base_dd_multiplier = stacks;
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

    struct discharge_spell_t : public spell_t
    {
      discharge_spell_t( const char* n, player_t* p, double amount, double scaling, const school_type s, bool no_crit, bool nb, bool nd ) :
        spell_t( n, p, RESOURCE_NONE, ( s == SCHOOL_DRAIN ) ? SCHOOL_SHADOW : s )
      {
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero;
        base_dd_min = amount;
        base_dd_max = amount;
        direct_power_mod = scaling;
        may_crit = ( s != SCHOOL_DRAIN ) && ! no_crit;
        background  = true;
        no_buffs = nb;
        no_debuffs = nd;
        init();
      }
    };

    struct discharge_attack_t : public attack_t
    {
      bool no_buffs;

      discharge_attack_t( const char* n, player_t* p, double amount, double scaling, const school_type s, bool no_crit, bool nb, bool nd ) :
        attack_t( n, p, RESOURCE_NONE, ( s == SCHOOL_DRAIN ) ? SCHOOL_SHADOW : s )
      {
        discharge_proc = true;
        item_proc = true;
        trigger_gcd = timespan_t::zero;
        base_dd_min = amount;
        base_dd_max = amount;
        direct_power_mod = scaling;
        may_crit = ( s != SCHOOL_DRAIN ) && ! no_crit;
        may_dodge = ( s == SCHOOL_PHYSICAL && ! no_crit );
        may_parry = ( s == SCHOOL_PHYSICAL && ! no_crit && ( p -> position == POSITION_FRONT || p -> position == POSITION_RANGED_FRONT ) );
        may_glance = false;
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
                                        const bool         heroic,
                                        const bool         lfr,
                                        const bool         /* ptr */,
                                        const std::string& /* id */ )
{
  std::string e;

  // Stat Procs
  if      ( name == "abyssal_rune"                        ) e = "OnSpellCast_590SP_25%_10Dur_45Cd";
  else if ( name == "anhuurs_hymnal"                      ) e = ( heroic ? "OnSpellCast_1710SP_10%_10Dur_50Cd" : "OnSpellCast_1512SP_10%_10Dur_50Cd" );
  else if ( name == "ashen_band_of_endless_destruction"   ) e = "OnSpellHit_285SP_10%_10Dur_60Cd";
  else if ( name == "ashen_band_of_unmatched_destruction" ) e = "OnSpellHit_285SP_10%_10Dur_60Cd";
  else if ( name == "ashen_band_of_endless_vengeance"     ) e = "OnAttackHit_480AP_1PPM_10Dur_60Cd";
  else if ( name == "ashen_band_of_unmatched_vengeance"   ) e = "OnAttackHit_480AP_1PPM_10Dur_60Cd";
  else if ( name == "ashen_band_of_endless_might"         ) e = "OnAttackHit_480AP_1PPM_10Dur_60Cd";
  else if ( name == "ashen_band_of_unmatched_might"       ) e = "OnAttackHit_480AP_1PPM_10Dur_60Cd";
  else if ( name == "banner_of_victory"                   ) e = "OnAttackHit_1008AP_20%_10Dur_50Cd";
  else if ( name == "bell_of_enraging_resonance"          ) e = ( heroic ? "OnSpellCast_2178SP_30%_20Dur_100Cd" : "OnSpellCast_1926SP_30%_20Dur_100Cd" );
  else if ( name == "black_magic"                         ) e = "OnSpellHit_250Haste_35%_10Dur_35Cd";
  else if ( name == "blood_of_the_old_god"                ) e = "OnAttackCrit_1284AP_10%_10Dur_50Cd";
  else if ( name == "chuchus_tiny_box_of_horrors"         ) e = "OnAttackHit_258Crit_15%_10Dur_45Cd";
  else if ( name == "comets_trail"                        ) e = "OnAttackHit_726Haste_10%_10Dur_45Cd";
  else if ( name == "corens_chromium_coaster"             ) e = "OnAttackCrit_1000AP_10%_10Dur_50Cd";
  else if ( name == "corens_chilled_chromium_coaster"     ) e = "OnAttackCrit_4000AP_10%_10Dur_50Cd";
  else if ( name == "crushing_weight"                     ) e = ( heroic ? "OnAttackHit_2178Haste_10%_15Dur_75Cd" : "OnAttackHit_1926Haste_10%_15Dur_75Cd" );
  else if ( name == "dark_matter"                         ) e = "OnAttackHit_612Crit_15%_10Dur_45Cd";
  else if ( name == "darkmoon_card_crusade"               ) e = "OnDamage_8SP_10Stack_10Dur";
  else if ( name == "dwyers_caber"                        ) e = "OnDamage_1020Crit_15%_20Dur_50Cd";
  else if ( name == "dying_curse"                         ) e = "OnSpellCast_765SP_15%_10Dur_45Cd";
  else if ( name == "elemental_focus_stone"               ) e = "OnSpellCast_522Haste_10%_10Dur_45Cd";
  else if ( name == "embrace_of_the_spider"               ) e = "OnSpellCast_505Haste_10%_10Dur_45Cd";
  else if ( name == "essence_of_the_cyclone"              ) e = ( heroic ? "OnAttackHit_2178Crit_10%_10Dur_50Cd" : "OnAttackHit_1926Crit_10%_10Dur_50Cd" );
  else if ( name == "eye_of_magtheridon"                  ) e = "OnSpellMiss_170SP_10Dur";
  else if ( name == "eye_of_the_broodmother"              ) e = "OnSpellDamageHeal_25SP_5Stack_10Dur";
  else if ( name == "flare_of_the_heavens"                ) e = "OnSpellCast_850SP_10%_10Dur_45Cd";
  else if ( name == "fluid_death"                         ) e = "OnAttackHit_38Agi_10Stack_15Dur";
  else if ( name == "forge_ember"                         ) e = "OnSpellHit_512SP_10%_10Dur_45Cd";
  else if ( name == "fury_of_the_five_flights"            ) e = "OnAttackHit_16AP_20Stack_10Dur";
  else if ( name == "gale_of_shadows"                     ) e = ( heroic ? "OnSpellTickDamage_17SP_20Stack_15Dur" : "OnSpellTickDamage_15SP_20Stack_15Dur" );
  else if ( name == "grace_of_the_herald"                 ) e = ( heroic ? "OnAttackHit_1710Crit_10%_10Dur_75Cd" : "OnAttackHit_924Crit_10%_10Dur_75Cd" );
  else if ( name == "grim_toll"                           ) e = "OnAttackHit_612Crit_15%_10Dur_45Cd";
  else if ( name == "harrisons_insignia_of_panache"       ) e = "OnAttackHit_918Mastery_10%_20Dur_95Cd"; // TO-DO: Confirm ICD
  else if ( name == "heart_of_rage"                       ) e = ( heroic ? "OnAttackHit_2178Str_10%_20Dur_100Cd" : "OnAttackHit_1926Str_10%_20Dur_100Cd" ); // TO-DO: Confirm ICD.
  else if ( name == "heart_of_solace"                     ) e = ( heroic ? "OnAttackHit_1710Str_10%_20Dur_100Cd" : "OnAttackHit_1512Str_10%_20Dur_100Cd" );
  else if ( name == "heart_of_the_vile"                   ) e = "OnAttackHit_924Crit_10%_10Dur_75Cd";
  else if ( name == "heartsong"                           ) e = "OnSpellCast_200Spi_25%_15Dur_20Cd";
  else if ( name == "herkuml_war_token"                   ) e = "OnAttackHit_17AP_20Stack_10Dur";
  else if ( name == "illustration_of_the_dragon_soul"     ) e = "OnSpellCast_20SP_10Stack_10Dur";
  else if ( name == "key_to_the_endless_chamber"          ) e = ( heroic ? "OnAttackHit_1710Agi_10%_15Dur_75Cd" : "OnAttackHit_1290Agi_10%_15Dur_75Cd" );
  else if ( name == "left_eye_of_rajh"                    ) e = ( heroic ? "OnAttackCrit_1710Agi_50%_10Dur_50Cd" : "OnAttackCrit_1512Agi_50%_10Dur_50Cd" );
  else if ( name == "license_to_slay"                     ) e = "OnAttackHit_38Str_10Stack_15Dur";
  else if ( name == "mark_of_defiance"                    ) e = "OnSpellHit_150Mana_15%_15Cd";
  else if ( name == "mirror_of_truth"                     ) e = "OnAttackCrit_1000AP_10%_10Dur_50Cd";
  else if ( name == "mithril_pocketwatch"                 ) e = "OnSpellCast_590SP_10%_10Dur_45Cd";
  else if ( name == "mithril_stopwatch"                   ) e = "OnSpellCast_2040SP_10%_10Dur_45Cd"; // FIXME: Confirm ICD
  else if ( name == "mjolnir_runestone"                   ) e = "OnAttackHit_665Haste_15%_10Dur_45Cd";
  else if ( name == "muradins_spyglass"                   ) e = ( heroic ? "OnSpellDamage_20SP_10Stack_10Dur" : "OnSpellDamage_18SP_10Stack_10Dur" );
  else if ( name == "necromantic_focus"                   ) e = ( heroic ? "OnSpellTickDamage_44Mastery_10Stack_10Dur" : "OnSpellTickDamage_39Mastery_10Stack_10Dur" );
  else if ( name == "needleencrusted_scorpion"            ) e = "OnAttackCrit_678crit_10%_10Dur_50Cd";
  else if ( name == "pandoras_plea"                       ) e = "OnSpellCast_751SP_10%_10Dur_45Cd";
  else if ( name == "petrified_pickled_egg"               ) e = "OnHeal_2040Haste_10%_10Dur_50Cd"; // FIXME: Confirm ICD
  else if ( name == "porcelain_crab"                      ) e = ( heroic ? "OnAttackHit_1710Mastery_10%_20Dur_95Cd" : "OnAttackHit_918Mastery_10%_20Dur_95Cd" ); // TO-DO: Confirm ICD.
  else if ( name == "prestors_talisman_of_machination"    ) e = ( heroic ? "OnAttackHit_2178Haste_10%_15Dur_75Cd" : "OnAttackHit_1926Haste_10%_15Dur_75Cd" ); // TO-DO: Confirm ICD.
  else if ( name == "purified_lunar_dust"                 ) e = "OnSpellCast_304MP5_10%_15Dur_45Cd";
  else if ( name == "pyrite_infuser"                      ) e = "OnAttackCrit_1234AP_10%_10Dur_50Cd";
  else if ( name == "quagmirrans_eye"                     ) e = "OnSpellCast_320Haste_10%_6Dur_45Cd";
  else if ( name == "right_eye_of_rajh"                   ) e = ( heroic ? "OnAttackCrit_1710Str_50%_10Dur_50Cd" : "OnAttackCrit_1512Str_50%_10Dur_50Cd" );
  else if ( name == "schnotzzs_medallion_of_command"      ) e = "OnAttackHit_918Mastery_10%_20Dur_95Cd"; // TO-DO: Confirm ICD.
  else if ( name == "sextant_of_unstable_currents"        ) e = "OnSpellCrit_190SP_20%_15Dur_45Cd";
  else if ( name == "shiffars_nexus_horn"                 ) e = "OnSpellCrit_225SP_20%_10Dur_45Cd";
  else if ( name == "stonemothers_kiss"                   ) e = "OnSpellCast_1164Crit_10%_20Dur_75Cd";
  else if ( name == "stump_of_time"                       ) e = "OnSpellCast_1926SP_10%_15Dur_75Cd";
  else if ( name == "sundial_of_the_exiled"               ) e = "OnSpellCast_590SP_10%_10Dur_45Cd";
  else if ( name == "talisman_of_sinister_order"          ) e = "OnSpellCast_918Mastery_10%_20Dur_95Cd"; // TO-DO: Confirm ICD.
  else if ( name == "tendrils_of_burrowing_dark"          ) e = ( heroic ? "OnSpellCast_1710SP_10%_15Dur_75Cd" : "OnSpellCast_1290SP_10%_15Dur_75Cd" ); // TO-DO: Confirm ICD
  else if ( name == "the_hungerer"                        ) e = ( heroic ? "OnAttackHit_1730Haste_100%_15Dur_60Cd" : "OnAttackHit_1532Haste_100%_15Dur_60Cd" );
  else if ( name == "theralions_mirror"                   ) e = ( heroic ? "OnHarmfulSpellCast_2178Mastery_10%_20Dur_100Cd" : "OnHarmfulSpellCast_1926Mastery_10%_20Dur_100Cd" );
  else if ( name == "tias_grace"                          ) e = ( heroic ? "OnAttackHit_34Agi_10Stack_15Dur" : "OnAttackHit_34Agi_10Stack_15Dur" );
  else if ( name == "unheeded_warning"                    ) e = "OnAttackHit_1926AP_10%_10Dur_50Cd";
  else if ( name == "vessel_of_acceleration"              ) e = ( heroic ? "OnAttackCrit_93Crit_5Stack_20Dur" : "OnAttackCrit_82Crit_5Stack_20Dur" );
  else if ( name == "witching_hourglass"                  ) e = ( heroic ? "OnSpellCast_1710Haste_10%_15Dur_75Cd" : "OnSpellCast_918Haste_10%_15Dur_75Cd" );
  else if ( name == "wrath_of_cenarius"                   ) e = "OnSpellHit_132SP_5%_10Dur";
  else if ( name == "fall_of_mortality"                   ) e = ( heroic ? "OnHealCast_2178Spi_15Dur_75Cd" : "OnHealCast_1926Spi_15Dur_75Cd" );
  else if ( name == "darkmoon_card_tsunami"               ) e = "OnHeal_80Spi_5Stack_20Dur";
  else if ( name == "phylactery_of_the_nameless_lich"     ) e = ( heroic ? "OnSpellTickDamage_1206SP_30%_20Dur_100Cd" : "OnSpellTickDamage_1073SP_30%_20Dur_100Cd" );
  else if ( name == "whispering_fanged_skull"             ) e = ( heroic ? "OnAttackHit_1250AP_35%_15Dur_45Cd" : "OnAttackHit_1110AP_35%_15Dur_45Cd" );
  else if ( name == "charred_twilight_scale"              ) e = ( heroic ? "OnSpellCast_861SP_10%_15Dur_45Cd" : "OnSpellCast_763SP_10%_15Dur_45Cd" );
  else if ( name == "sharpened_twilight_scale"            ) e = ( heroic ? "OnAttackHit_1472AP_35%_15Dur_45Cd" : "OnAttackHit_1304AP_35%_15Dur_45Cd" );

  // 4.3
  else if ( name == "will_of_unbinding"                   ) e = ( heroic ? "OnHarmfulSpellCast_99Int_100%_10Dur_10Stack" : lfr ? "OnHarmfulSpellCast_78Int_100%_10Dur_10Stack" : "OnHarmfulSpellCast_88Int_100%_10Dur_10Stack" );
  else if ( name == "eye_of_unmaking"                     ) e = ( heroic ? "OnAttackHit_99Str_100%_10Dur_10Stack" : lfr ? "OnAttackHit_78Str_100%_10Dur_10Stack" : "OnAttackHit_88Str_100%_10Dur_10Stack" );
  else if ( name == "wrath_of_unchaining"                 ) e = ( heroic ? "OnAttackHit_99Agi_100%_10Dur_10Stack" : lfr ? "OnAttackHit_78Agi_100%_10Dur_10Stack" : "OnAttackHit_88Agi_100%_10Dur_10Stack" );
  else if ( name == "heart_of_unliving"                   ) e = ( heroic ? "OnHealCast_99Spi_100%_10Dur_10Stack" : lfr ? "OnHealCast_78Spi_100%_10Dur_10Stack" : "OnHealCast_88Spi_100%_10Dur_10Stack" );
  else if ( name == "resolve_of_undying"                  ) e = ( heroic ? "OnAttackHit_99Dodge_100%_10Dur_10Stack" : lfr ? "OnAttackHit_99Dodge_100%_10Dur_10Stack" : "OnAttackHit_88Dodge_100%_10Dur_10Stack" );
  else if ( name == "creche_of_the_final_dragon"          ) e = ( heroic ? "OnDamage_3278Crit_15%_20Dur_115Cd" : lfr ? "OnDamage_2573Crit_15%_20Dur_115Cd" : "OnDamage_2904Crit_15%_20Dur_115Cd" ); // FIXME: ICD needs testing, assuming same as insignia of corrupted mind
  else if ( name == "starcatcher_compass"                 ) e = ( heroic ? "OnDamage_3278Haste_15%_20Dur_115Cd" : lfr ? "OnDamage_2573Haste_15%_20Dur_115Cd" : "OnDamage_2904Haste_15%_20Dur_115Cd" ); // FIXME: ICD needs testing, assuming same as insignia of corrupted mind
  else if ( name == "seal_of_the_seven_signs"             ) e = ( heroic ? "OnHeal_3278Haste_15%_20Dur_115Cd" : lfr ? "OnHeal_2573Haste_15%_20Dur_115Cd" : "OnHeal_2904Haste_15%_20Dur_115Cd" ); // FIXME: ICD needs testing, assuming same as insignia of corrupted mind
  else if ( name == "soulshifter_vortex"                  ) e = ( heroic ? "OnDamage_3278Mastery_15%_20Dur_115Cd" : lfr ? "OnDamage_2573Mastery_15%_20Dur_115Cd" : "OnDamage_2904Mastery_15%_20Dur_115Cd" ); // FIXME: ICD needs testing, assuming same as insignia of corrupted mind
  else if ( name == "insignia_of_the_corrupted_mind"      ) e = ( heroic ? "OnDamage_3278Haste_15%_20Dur_115Cd" : lfr ? "OnDamage_2573Haste_15%_20Dur_115Cd" : "OnDamage_2904Haste_15%_20Dur_115Cd" ); // ICD confirmed on PTR 2011/11/14
  else if ( name == "foul_gift_of_the_demon_lord"         ) e = "OnSpellCast_1149Mastery_15%_20Dur_45Cd"; // TO-DO: Confirm ICD. Min seen 50.112sec
  else if ( name == "arrow_of_time"                       ) e = "OnAttackHit_1149Haste_20%_20Dur_45Cd";   // TO-DO: Confirm ICD
  else if ( name == "rosary_of_light"                     ) e = "OnAttackHit_1149Crit_15%_20Dur_45Cd";   // TO-DO: Confirm ICD. Min seen 50.934sec.
  else if ( name == "varothens_brooch"                    ) e = "OnAttackHit_1149Mastery_20%_20Dur_45Cd";   // TO-DO: Confirm ICD. Min seen 50.192sec.

  // Stat Procs with Tick Increases
  else if ( name == "dislodged_foreign_object"            ) e = ( heroic ? "OnSpellCast_121SP_10Stack_10%_20Dur_45Cd_2Tick" : "OnSpellCast_105SP_10Stack_10%_20Dur_45Cd_2Tick" );

  // Discharge Procs
  else if ( name == "bandits_insignia"                    ) e = "OnAttackHit_1880Arcane_15%_45Cd";
  else if ( name == "darkmoon_card_hurricane"             ) e = "OnAttackHit_-7000Nature_1PPM_nocrit_nobuffs";
  else if ( name == "darkmoon_card_volcano"               ) e = "OnSpellDamage_1200+10Fire_1600Int_30%_12Dur_45Cd";
  else if ( name == "extract_of_necromantic_power"        ) e = "OnSpellTickDamage_1050Shadow_10%_15Cd";
  else if ( name == "lightning_capacitor"                 ) e = "OnSpellCrit_750Nature_3Stack_2.5Cd";
  else if ( name == "timbals_crystal"                     ) e = "OnSpellTickDamage_380Shadow_10%_15Cd";
  else if ( name == "thunder_capacitor"                   ) e = "OnSpellCrit_1276Nature_4Stack_2.5Cd";
  else if ( name == "bryntroll_the_bone_arbiter"          ) e = ( heroic ? "OnAttackHit_2538Drain_11%" : "OnAttackHit_2250Drain_11%" );
  else if ( name == "cunning_of_the_cruel"                ) e = ( heroic ? "OnSpellDamage_4774.8+42.32Shadow_45%_9Cd" : lfr ? "OnSpellDamage_3747.6+33.24Shadow_45%_9Cd" : "OnSpellDamage_4230+37.52Shadow_45%_9Cd" );
  else if ( name == "vial_of_shadows"                     ) e = ( heroic ? "OnAttackHit_-6770+40.55Physical_45%_9Cd" : lfr ? "OnAttackHit_-5313+31.81Physical_45%_9Cd" : "OnAttackHit_-5997+35.92Physical_45%_9Cd" ); // ICD, base damage, and ap coeff determined experimentally on heroic version. Assuming dbc has wrong base damage. Normal and LFR assumed to be changed by the same %
  else if ( name == "reign_of_the_unliving"               ) e = ( heroic ? "OnSpellDirectCrit_2117Fire_3Stack_2.0Cd" : "OnSpellDirectCrit_1882Fire_3Stack_2.0Cd" );
  else if ( name == "reign_of_the_dead"                   ) e = ( heroic ? "OnSpellDirectCrit_2117Fire_3Stack_2.0Cd" : "OnSpellDirectCrit_1882Fire_3Stack_2.0Cd" );
  else if ( name == "solace_of_the_defeated"              ) e = ( heroic ? "OnSpellCast_18MP5_8Stack_10Dur" : "OnSpellCast_16MP5_8Stack_10Dur" );
  else if ( name == "solace_of_the_fallen"                ) e = ( heroic ? "OnSpellCast_18MP5_8Stack_10Dur" : "OnSpellCast_16MP5_8Stack_10Dur" );

  // Variable Stack Discharge Procs
  else if ( name == "variable_pulse_lightning_capacitor"  ) e = ( heroic ? "OnSpellCrit_3300.7Nature_15%_10Stack_2.5Cd_chance" : "OnSpellCrit_2926.3Nature_15%_10Stack_2.5Cd_chance" );

  // Enchants
  else if ( name == "lightweave_old"                      ) e = "OnSpellCast_295SP_35%_15Dur_60Cd";
  else if ( name == "lightweave_embroidery_old"           ) e = "OnSpellCast_295SP_35%_15Dur_60Cd";
  else if ( name == "lightweave" ||
            name == "lightweave_embroidery"               ) e = "OnSpellDamageHeal_580Int_25%_15Dur_64Cd";
  else if ( name == "darkglow_embroidery_old"             ) e = "OnSpellCast_400Mana_35%_15Dur_60Cd";
  else if ( name == "darkglow_embroidery"                 ) e = "OnSpellCast_800Mana_30%_15Dur_45Cd";       // TO-DO: Confirm ICD.
  else if ( name == "swordguard_embroidery_old"           ) e = "OnAttackHit_400AP_20%_15Dur_60Cd";
  else if ( name == "swordguard_embroidery"               ) e = "OnAttackHit_1000AP_15%_15Dur_55Cd";
  else if ( name == "flintlockes_woodchucker"             ) e = "OnAttackHit_1100Physical_300Agi_10%_10Dur_40Cd_nocrit"; // TO-DO: Confirm ICD.

  // DK Runeforges
  else if ( name == "rune_of_cinderglacier"               ) e = "custom";
  else if ( name == "rune_of_razorice"                    ) e = "custom";
  else if ( name == "rune_of_the_fallen_crusader"         ) e = "custom";

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
                                      const bool         heroic,
                                      const bool         lfr,
                                      const bool         /* ptr */,
                                      const std::string& /* id */ )
{
  std::string e;

  // Simple
  if      ( name == "ancient_petrified_seed"       ) e = ( heroic ? "1441Agi_15Dur_60Cd"  : "1277Agi_15Dur_60Cd" );
  else if ( name == "brawlers_trophy"              ) e = "1700Dodge_20Dur_120Cd";
  else if ( name == "core_of_ripeness"             ) e = "1926Spi_20Dur_120Cd";
  else if ( name == "electrospark_heartstarter"    ) e = "567Int_20Dur_120Cd";
  else if ( name == "energy_siphon"                ) e = "408SP_20Dur_120Cd";
  else if ( name == "ephemeral_snowflake"          ) e = "464Haste_20Dur_120Cd";
  else if ( name == "essence_of_the_eternal_flame" ) e = ( heroic ? "1441Str_15Dur_60Cd" : "1277Str_15Dur_60Cd" );
  else if ( name == "fiery_quintessence"           ) e = ( heroic ? "1297Int_25Dur_90Cd"  : "1149Int_25Dur_90Cd" );
  else if ( name == "figurine__demon_panther"      ) e = "1425Agi_20Dur_120Cd";
  else if ( name == "figurine__dream_owl"          ) e = "1425Spi_20Dur_120Cd";
  else if ( name == "figurine__jeweled_serpent"    ) e = "1425Sp_20Dur_120Cd";
  else if ( name == "figurine__king_of_boars"      ) e = "1425Str_20Dur_120Cd";
  else if ( name == "impatience_of_youth"          ) e = "1605Str_20Dur_120Cd";
  else if ( name == "jaws_of_defeat"               ) e = ( heroic ? "OnSpellCast_125HolyStorm_CostRd_10Stack_20Dur_120Cd" : "OnSpellCast_110HolyStorm_CostRd_10Stack_20Dur_120Cd" );
  else if ( name == "living_flame"                 ) e = "505SP_20Dur_120Cd";
  else if ( name == "maghias_misguided_quill"      ) e = "716SP_20Dur_120Cd";
  else if ( name == "magnetite_mirror"             ) e = ( heroic ? "1425Str_15Dur_90Cd" : "1075Str_15Dur_90Cd" );
  else if ( name == "mark_of_khardros"             ) e = ( heroic ? "1425Mastery_15Dur_90Cd" : "1260Mastery_15Dur_90Cd" );
  else if ( name == "mark_of_norgannon"            ) e = "491Haste_20Dur_120Cd";
  else if ( name == "mark_of_supremacy"            ) e = "1024AP_20Dur_120Cd";
  else if ( name == "mark_of_the_firelord"         ) e = ( heroic ? "1441Int_15Dur_60Cd"  : "1277Int_15Dur_60Cd" );
  else if ( name == "moonwell_chalice"             ) e = "1700Mastery_20Dur_120Cd";
  else if ( name == "moonwell_phial"               ) e = "1700Dodge_20Dur_120Cd";
  else if ( name == "might_of_the_ocean"           ) e = ( heroic ? "1425Str_15Dur_90Cd" : "765Str_15Dur_90Cd" );
  else if ( name == "platinum_disks_of_battle"     ) e = "752AP_20Dur_120Cd";
  else if ( name == "platinum_disks_of_sorcery"    ) e = "440SP_20Dur_120Cd";
  else if ( name == "platinum_disks_of_swiftness"  ) e = "375Haste_20Dur_120Cd";
  else if ( name == "rickets_magnetic_fireball"    ) e = "1700Crit_20Dur_120Cd"; // FIXME: "Your attacks may occasionally attract small celestial objects."
  else if ( name == "rune_of_zeth"                 ) e = ( heroic ? "1441Int_15Dur_60Cd" : "1277Int_15Dur_60Cd" );
  else if ( name == "scale_of_fates"               ) e = "432Haste_20Dur_120Cd";
  else if ( name == "sea_star"                     ) e = ( heroic ? "1425Sp_20Dur_120Cd" : "765Sp_20Dur_120Cd" );
  else if ( name == "shard_of_the_crystal_heart"   ) e = "512Haste_20Dur_120Cd";
  else if ( name == "shard_of_woe"                 ) e = "1935Haste_10Dur_60Cd";
  else if ( name == "skardyns_grace"               ) e = ( heroic ? "1425Mastery_20Dur_120Cd" : "1260Mastery_20Dur_120Cd" );
  else if ( name == "sliver_of_pure_ice"           ) e = "1625Mana_120Cd";
  else if ( name == "soul_casket"                  ) e = "1926Sp_20Dur_120Cd";
  else if ( name == "souls_anguish"                ) e = "765Str_15Dur_90Cd";
  else if ( name == "spirit_world_glass"           ) e = "336Spi_20Dur_120Cd";
  else if ( name == "talisman_of_resurgence"       ) e = "599SP_20Dur_120Cd";
  else if ( name == "unsolvable_riddle"            ) e = "1605Agi_20Dur_120Cd";
  else if ( name == "wrathstone"                   ) e = "856AP_20Dur_120Cd";
  // 4.3
  else if ( name == "bottled_wishes"               ) e = ( heroic ? "2585SP_15Dur_90Cd" : lfr ? "2029SP_15Dur_90Cd" : "2290SP_15Dur_90Cd" );
  else if ( name == "kiroptyric_sigil"             ) e = ( heroic ? "2585Agi_15Dur_90Cd" : lfr ? "2029Agi_15Dur_90Cd" : "2290Agi_15Dur_90Cd" );
  else if ( name == "rotting_skull"                ) e = ( heroic ? "2585Str_15Dur_90Cd" : lfr ? "2029Str_15Dur_90Cd" : "2290Str_15Dur_90Cd" );
  else if ( name == "fire_of_the_deep"             ) e = ( heroic ? "2585Dodge_15Dur_90Cd" : lfr ? "2029Dodge_15Dur_90Cd" : "2290Dodge_15Dur_90Cd" );
  else if ( name == "reflection_of_the_light"      ) e = ( heroic ? "2585SP_15Dur_90Cd" : lfr ? "2029SP_15Dur_90Cd" : "2290SP_15Dur_90Cd" );

  // Hybrid
  else if ( name == "fetish_of_volatile_power"   ) e = ( heroic ? "OnSpellCast_64Haste_8Stack_20Dur_120Cd" : "OnSpellCast_57Haste_8Stack_20Dur_120Cd" );
  else if ( name == "talisman_of_volatile_power" ) e = ( heroic ? "OnSpellCast_64Haste_8Stack_20Dur_120Cd" : "OnSpellCast_57Haste_8Stack_20Dur_120Cd" );
  else if ( name == "vengeance_of_the_forsaken"  ) e = ( heroic ? "OnAttackHit_250AP_5Stack_20Dur_120Cd" : "OnAttackHit_215AP_5Stack_20Dur_120Cd" );
  else if ( name == "victors_call"               ) e = ( heroic ? "OnAttackHit_250AP_5Stack_20Dur_120Cd" : "OnAttackHit_215AP_5Stack_20Dur_120Cd" );
  else if ( name == "nevermelting_ice_crystal"   ) e = "OnSpellCrit_184Crit_5Stack_20Dur_180Cd_reverse";

  // Engineering Tinkers
  else if ( name == "pyrorocket"                   ) e = "1165Fire_45Cd";  // temporary for backwards compatibility
  else if ( name == "hand_mounted_pyro_rocket"     ) e = "1165Fire_45Cd";
  else if ( name == "hyperspeed_accelerators"      ) e = "240Haste_12Dur_60Cd";
  else if ( name == "tazik_shocker"                ) e = "4800Nature_120Cd";
  else if ( name == "quickflip_deflection_plates"  ) e = "1500Armor_12Dur_60Cd";

  if ( e.empty() ) return false;

  util_t::tolower( e );

  encoding = e;

  return true;
}
