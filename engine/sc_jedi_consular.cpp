// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

struct jedi_sage_targetdata_t : public targetdata_t
{
  dot_t* dots_telekinetic_throw;
  dot_t* dots_mind_crush;
  dot_t* dots_weaken_mind;
  dot_t* dots_sever_force;

  jedi_sage_targetdata_t( player_t* source, player_t* target )
    : targetdata_t( source, target )
  {
  }
};

void register_jedi_sage_targetdata( sim_t*  sim  )
{
  player_type t = JEDI_SAGE;
  typedef jedi_sage_targetdata_t type;

  REGISTER_DOT( telekinetic_throw );
  REGISTER_DOT( mind_crush );
  REGISTER_DOT( weaken_mind );
  REGISTER_DOT( sever_force );
}

// ==========================================================================
// Jediconsular
// ==========================================================================

struct jedi_consular_t : public player_t
{
protected:
  jedi_consular_t( sim_t* sim, player_type pt, const std::string& name, race_type r ) :
    player_t( sim, JEDI_CONSULAR, pt, name, ( r == RACE_NONE ) ? RACE_HUMAN : r )
  {
    create_options();
  }
public:

  // Character Definition
  virtual action_t* create_action( const std::string& name, const std::string& options );
  virtual void      init_base();
  virtual void      init_buffs();
  virtual void      init_resources( bool force=false );
  virtual int       primary_resource() const;
  virtual int       primary_role() const;
};


// ==========================================================================
// Jedi Sage
// ==========================================================================

struct jedi_sage_t : public jedi_consular_t
{

  // Buffs
  buff_t* buffs_concentration;
  buff_t* buffs_psychic_projection;
  buff_t* buffs_tidal_force;
  buff_t* buffs_telekinetic_effusion;
  buff_t* buffs_tremors;
  buff_t* buffs_presence_of_mind;
  buff_t* buffs_force_suppression;

  // Gains
  gain_t* gains_concentration;
  gain_t* gains_focused_insight;
  gain_t* gains_psychic_barrier;

  // Procs
  //proc_t* procs_<procname>;

  // RNG
  rng_t* rng_psychic_barrier;
  rng_t* rng_upheaval;
  rng_t* rng_tm;


  // Talents
  struct talents_t
  {
    // TREE_SEER

    // TREE_TELEKINETICS
    int inner_strength;
    int mental_longevity;
    int clamoring_force;
    int minds_eye;
    int disturb_mind;
    int concentration;
    int telekinetic_defense;
    int blockout;
    int telekinetic_wave;
    int psychic_projection;
    int force_wake;
    int tidal_force;
    int telekinetic_effusion;
    int kinetic_collapse;
    int tremors;
    int telekinetic_momentum;
    int mental_alacrity;
    int reverberation;
    int turbulence;

    // TREE_BALANCE
    int empowered_throw;
    int jedi_resistance;
    int will_of_the_jedi;
    int pinning_resolve;
    int upheaval;
    int focused_insight;
    int critical_kinsesis;
    int force_in_balance;
    int psychic_barrier;
    int telekinetic_balance;
    int containment;
    int mind_ward;
    int presence_of_mind;
    int force_suppression;
    int drain_thoughts;
    int assertion;
    int mental_scarring;
    int psychic_absorption;
    int sever_force;

    talents_t() { memset( ( void* ) this, 0x0, sizeof( talents_t ) ); }
  };
  talents_t talents;

  jedi_sage_t( sim_t* sim, player_type pt, const std::string& name, race_type r = RACE_NONE ) :
    jedi_consular_t( sim, pt == SITH_SORCERER ? SITH_SORCERER : JEDI_SAGE, name, ( r == RACE_NONE ) ? RACE_HUMAN : r )
  {
    if ( pt == SITH_SORCERER )
    {
      tree_type[ SITH_SORCERER_CORRUPTION ] = TREE_CORRUPTION;
      tree_type[ SITH_SORCERER_LIGHTNING ]  = TREE_LIGHTNING;
      tree_type[ SITH_SORCERER_MADNESS ]    = TREE_MADNESS;
    }
    else
    {
      tree_type[ JEDI_SAGE_SEER ]         = TREE_SEER;
      tree_type[ JEDI_SAGE_TELEKINETICS ] = TREE_TELEKINETICS;
      tree_type[ JEDI_SAGE_BALANCE ]      = TREE_BALANCE;
    }

    create_talents();
    create_glyphs();
    create_options();
  }

  // Character Definition
  virtual targetdata_t* new_targetdata( player_t* source, player_t* target ) {return new jedi_sage_targetdata_t( source, target );}
  virtual action_t* create_action( const std::string& name, const std::string& options );
  virtual void      init_talents();
  virtual void      init_base();
  virtual void      init_buffs();
  virtual void      init_gains();
  virtual void      init_procs();
  virtual void      init_rng();
  virtual void      init_actions();
  virtual int       primary_role() const;
  virtual void      regen( const double periodicity );
  virtual double    composite_spell_power( const school_type school ) const;
};

namespace { // ANONYMOUS NAMESPACE ==========================================


// ==========================================================================
// jedi_consular Abilities
// ==========================================================================

struct jedi_consular_attack_t : public attack_t
{
  jedi_consular_attack_t( const char* n, jedi_consular_t* p, int r=RESOURCE_NONE, const school_type s=SCHOOL_HOLY, int t=TREE_NONE ) :
    attack_t( n, p, r, s, t )
  {
    _init_jedi_consular_attack_t();
  }

  void _init_jedi_consular_attack_t()
  {
    may_crit   = true;
    may_glance = false;
  }

  virtual bool ready();
};

struct jedi_consular_spell_t : public spell_t
{
  jedi_consular_spell_t( const char* n, jedi_consular_t* p, int r=RESOURCE_NONE, const school_type s=SCHOOL_HOLY, int t=TREE_NONE ) :
    spell_t( n, p, r, s, t )
  {
    _init_jedi_consular_spell_t();
  }

  virtual void init()
  {
    spell_t::init();

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( base_td > 0 )
        crit_bonus += p -> talents.mental_scarring * 0.1;
    }
  }

  void _init_jedi_consular_spell_t()
  {
    may_crit   = true;
    tick_may_crit = true;

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      base_td_multiplier *= 1.0 + p -> talents.mental_scarring * 0.10;
    }
  }

  virtual void impact( player_t* t, int impact_result, double travel_dmg )
  {
    spell_t::impact( t, impact_result, travel_dmg );

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( impact_result == RESULT_CRIT && p -> talents.telekinetic_effusion > 0 )
      {
        p -> buffs_telekinetic_effusion -> trigger( 2 );
      }
    }
  }

  virtual double execute_time() const
  {
    double et = spell_t::execute_time();

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( base_execute_time > 0 && p -> buffs_presence_of_mind -> up() )
        et = 0;
    }

    return et;
  }

  virtual void player_buff()
  {
    spell_t::player_buff();

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( base_execute_time > 0 && p -> buffs_presence_of_mind -> up() )
        player_multiplier *= 1.20;

      // FIXME: test whether channeled spells profit from it or not ( they clearly don't consume the buff, tested January 2012 )
      if ( p -> talents.force_suppression > 0 && base_td > 0 )
        if ( p -> buffs_force_suppression -> up() )
          player_td_multiplier *= 1.20;
    }
  }

  virtual double cost() const
  {
    double c = spell_t::cost();

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();
      if ( p -> talents.inner_strength > 0 )
      {
        c *= 1.0 - p -> talents.inner_strength * 0.03;
        c = floor( c );
      }

      if ( p -> buffs_telekinetic_effusion -> check() > 0 )
      {
        c *= 0.5;
        c = floor( c );
      }
    }

    return c;
  }

  virtual void consume_resource()
  {
    spell_t::consume_resource();

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      p -> buffs_telekinetic_effusion -> up();
    }
  }

  virtual void execute()
  {
    spell_t::execute();

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( base_execute_time > 0 )
        p -> buffs_presence_of_mind -> expire();
    }
  }

  virtual void tick( dot_t* d)
  {
    spell_t::tick( d );

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( tick_dmg > 0 && !channeled )
        p -> buffs_force_suppression -> decrement();
    }
  }
};

struct force_valor_t : public jedi_consular_spell_t
{
  force_valor_t( jedi_consular_t* p, const std::string& options_str ) :
    jedi_consular_spell_t( "force_valor", p, RESOURCE_FORCE )
  {
    parse_options( 0, options_str );
    base_cost = 0.0;
    harmful = false;
  }

  virtual void execute()
  {
    jedi_consular_spell_t::execute();

    for ( player_t* p = sim -> player_list; p; p = p -> next )
    {
      if ( p -> ooc_buffs() )
        p -> buffs.force_valor -> override();
    }
  }

  virtual bool ready()
  {
    if ( player -> buffs.force_valor -> check() )
      return false;

    return jedi_consular_spell_t::ready();
  }
};

struct project_t : public jedi_consular_spell_t
{
  jedi_consular_spell_t* upheaval;

  project_t( jedi_consular_t* p, const std::string& options_str, bool is_upheaval = false ) :
    jedi_consular_spell_t( ( std::string( p -> type == SITH_SORCERER ? "shock" : "project") + std::string( is_upheaval ? "_upheaval" : "" ) ).c_str(), p, RESOURCE_FORCE, SCHOOL_PHYSICAL ),
    upheaval( 0 )
  {
    parse_options( 0, options_str );
    base_dd_min = 219.0; base_dd_max = 283.4;
    base_cost = 45.0;
    range = 30.0;
    direct_power_mod = 1.85;

    cooldown -> duration = 6.0;

    if ( player -> is_jedi_sage() && !is_upheaval )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( p -> talents.upheaval > 0 )
      {
        upheaval = new project_t( p, options_str, true );
        upheaval -> base_multiplier *= 0.50;
        upheaval -> base_cost = 0.0;
        upheaval -> background = true;
        add_child( upheaval );
      }
    }
  }

  virtual void execute()
  {
    jedi_consular_spell_t::execute();

    if ( upheaval )
    {
      if ( player -> is_jedi_sage() )
      {
        jedi_sage_t* p = player -> cast_jedi_sage();

        if ( p -> rng_upheaval -> roll( p -> talents.upheaval * 0.15 ) )
          upheaval -> execute();
      }
    }
  }
};

struct telekinetic_throw_t : public jedi_consular_spell_t
{
  telekinetic_throw_t( jedi_consular_t* p, const std::string& options_str ) :
    jedi_consular_spell_t( p -> type == SITH_SORCERER ? "force_lightning" : "telekinetic_throw", p, RESOURCE_FORCE, SCHOOL_PHYSICAL )
  {
    parse_options( 0, options_str );
    base_td = 508.8;
    base_cost = 30.0;
    range = 30.0;
    tick_power_mod = 0.79;

    num_ticks = 3;

    base_tick_time = 1.0;
    may_crit = false;
    channeled = true;

    cooldown -> duration = 6.0;

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      base_multiplier *= 1.0 + p -> talents.empowered_throw * 0.04;

      base_crit += p -> talents.critical_kinsesis * 0.05;

      if ( p -> talents.telekinetic_balance > 0 )
        cooldown -> duration = 0;

      crit_bonus += p -> talents.reverberation * 0.1;
    }
  }

  virtual double tick_time() const
  {
    double tt = jedi_consular_spell_t::tick_time();

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( p -> buffs_psychic_projection -> check() )
        tt *= 0.5;
    }

    return tt;
  }

  virtual void tick( dot_t* d )
  {
    jedi_consular_spell_t::tick( d );

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( tick_dmg > 0 && p -> talents.psychic_barrier > 0 )
      {
        if ( p -> rng_psychic_barrier -> roll( p -> talents.psychic_barrier * ( 1 / 3.0 ) ) )
        {
          double f = p -> resource_max[ RESOURCE_FORCE ] * 0.01;
          p -> resource_gain( RESOURCE_FORCE, f , p -> gains_psychic_barrier );
        }
        p -> buffs_presence_of_mind -> trigger();
      }

    }
  }
};

// ==========================================================================
// Jedi Sage Abilities
// ==========================================================================

struct jedi_sage_attack_t : public jedi_consular_attack_t
{
  jedi_sage_attack_t( const char* n, jedi_sage_t* p, int r=RESOURCE_NONE, const school_type s=SCHOOL_HOLY, int t=TREE_NONE ) :
    jedi_consular_attack_t( n, p, r, s, t )
  {
    _init_jedi_sage_attack_t();
  }

  void _init_jedi_sage_attack_t()
  {
    may_crit   = true;
    may_glance = false;
  }

  virtual bool ready();
};

struct jedi_sage_spell_t : public jedi_consular_spell_t
{
  jedi_sage_spell_t( const char* n, jedi_sage_t* p, int r=RESOURCE_NONE, const school_type s=SCHOOL_HOLY, int t=TREE_NONE ) :
    jedi_consular_spell_t( n, p, r, s, t )
  {
    _init_jedi_sage_spell_t();
  }

  void _init_jedi_sage_spell_t()
  {
    may_crit   = true;
    tick_may_crit = true;
  }

  virtual void tick( dot_t* d )
  {
    spell_t::tick( d );

    jedi_sage_t* p = player -> cast_jedi_sage();

    if ( tick_dmg > 0 && p -> talents.focused_insight > 0 )
    {
      double hp = p -> resource_max[ RESOURCE_HEALTH ] * p -> talents.focused_insight * 0.005;
      p -> resource_gain( RESOURCE_HEALTH, hp , p -> gains_focused_insight );
    }
  }
};

void trigger_tidal_force( action_t* a, double pc = 0 )
{
  jedi_sage_t* p = a -> player -> cast_jedi_sage();

  if ( p -> talents.tidal_force == 0 )
    return;

  if ( p -> buffs_tidal_force -> trigger( 1, 0, pc ) )
  {
    // telekinetic wave cd -> reset();
  }
}

struct disturbance_t : public jedi_sage_spell_t
{
  jedi_consular_spell_t* tm;

  disturbance_t( jedi_sage_t* p, const std::string& options_str, bool is_tm = false ) :
    jedi_sage_spell_t( ( std::string( p -> type == SITH_SORCERER ? "lightning_strike" : "disturbance") + std::string( is_tm ? "_tm" : "" ) ).c_str(), p, RESOURCE_FORCE, SCHOOL_PHYSICAL ),
    tm( 0 )
  {
    parse_options( 0, options_str );
    base_dd_min = 180.3; base_dd_max = 244.7;
    base_execute_time = 1.5;
    base_cost = 30.0;
    range = 30.0;
    direct_power_mod = 1.32;

    base_multiplier *= 1.0 + p -> talents.clamoring_force * 0.02;

    base_crit += p -> talents.critical_kinsesis * 0.05;

    if ( !is_tm )
    {
      if ( p -> talents.telekinetic_momentum > 0 )
      {
        tm = new disturbance_t( p, options_str, true );
        tm -> base_multiplier *= 0.30;
        tm -> base_cost = 0.0;
        tm -> background = true;
        add_child( tm );
      }
    }
  }

  virtual void impact( player_t* t, int impact_result, double travel_dmg )
  {
    jedi_sage_spell_t::impact( t, impact_result, travel_dmg );

    if ( result_is_hit( impact_result ) )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      p -> buffs_concentration -> trigger();

      trigger_tidal_force( this, 0.3 );
    }
  }

  virtual void execute()
  {
    jedi_consular_spell_t::execute();

    if ( tm )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( p -> rng_tm -> roll( p -> talents.telekinetic_momentum * 0.10 ) )
        tm -> execute();
    }
  }
};

struct mind_crush_t : public jedi_sage_spell_t
{
  mind_crush_t( jedi_sage_t* p, const std::string& options_str ) :
    jedi_sage_spell_t( p -> type == SITH_SORCERER ? "crushing_darkness" : "mind_crush", p, RESOURCE_FORCE, SCHOOL_PHYSICAL )
  {
    parse_options( 0, options_str );
    base_dd_min = 545.8; base_dd_max = 610.2;
    base_td = 47.5;
    base_tick_time = 1.0;
    base_execute_time = 2.0;
    num_ticks = 6 + p -> talents.assertion * 1;
    base_cost = 40.0;
    range = 30.0;
    direct_power_mod = 1.23;
    tick_power_mod = 0.295;

    cooldown -> duration = 15.0;

    base_multiplier *= 1.0 + p -> talents.clamoring_force * 0.02;
  }
};

struct weaken_mind_t : public jedi_sage_spell_t
{
  weaken_mind_t( jedi_sage_t* p, const std::string& options_str ) :
    jedi_sage_spell_t( p -> type == SITH_SORCERER ? "affliction" : "weaken_mind", p, RESOURCE_FORCE )
  {
    parse_options( 0, options_str );
    base_td = 299.5;
    base_tick_time = 3.0;
    num_ticks = 6 + p -> talents.disturb_mind;
    base_cost = 35.0;
    range = 30.0;
    tick_power_mod = 0.31;
    may_crit = false;
    crit_bonus += p -> talents.reverberation * 0.1;
    base_multiplier *= 1.0 + p -> talents.drain_thoughts * 0.075;
  }

  virtual void tick( dot_t* d )
  {
    jedi_sage_spell_t::tick( d );

    jedi_sage_t* p = player -> cast_jedi_sage();

    if ( result == RESULT_CRIT && p -> talents.psychic_projection > 0 )
    {
      p -> buffs_psychic_projection -> trigger();
    }
  }
};

struct turbulence_t : public jedi_sage_spell_t
{
  jedi_consular_spell_t* tm;

  turbulence_t( jedi_sage_t* p, const std::string& options_str, bool is_tm = false ) :
    jedi_sage_spell_t( ( std::string( p -> type == SITH_SORCERER ? "thundering_blast" : "turbulence") + std::string( is_tm ? "_tm" : "" ) ).c_str(), p, RESOURCE_FORCE ),
    tm( 0 )
  {
    check_talent( p -> talents.turbulence );

    parse_options( 0, options_str );
    base_dd_min = 222.2; base_dd_max = 286.6;
    base_execute_time = 2.0;
    base_cost = 45.0;
    range = 30.0;
    direct_power_mod = 1.58;
    crit_bonus += p -> talents.reverberation * 0.1;

    base_multiplier *= 1.0 + p -> talents.clamoring_force * 0.02;

    if ( !is_tm )
    {
      if ( p -> talents.telekinetic_momentum > 0 )
      {
        tm = new turbulence_t( p, options_str, true );
        tm -> base_multiplier *= 0.30;
        tm -> base_cost = 0.0;
        tm -> background = true;
        add_child( tm );
      }
    }
  }

  virtual void calculate_result()
  {
    jedi_sage_spell_t::calculate_result();

    jedi_sage_targetdata_t* td = targetdata() -> cast_jedi_sage();

    if ( td -> dots_weaken_mind -> ticking )
      result = RESULT_CRIT;
  }

  virtual void execute()
  {
    jedi_consular_spell_t::execute();

    if ( tm )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( p -> rng_tm -> roll( p -> talents.telekinetic_momentum * 0.10 ) )
        tm -> execute();
    }
  }
};

struct force_in_balance_t : public jedi_sage_spell_t
{
  force_in_balance_t( jedi_sage_t* p, const std::string& options_str ) :
    jedi_sage_spell_t( p -> type == SITH_SORCERER ? "death_field" : "force_in_balance", p, RESOURCE_FORCE )
  {
    check_talent( p -> talents.force_in_balance );

    parse_options( 0, options_str );
    base_dd_min = 268.9; base_dd_max = 333.3;
    ability_lag=0.2;
    base_cost = 50.0;
    range = 30.0;
    direct_power_mod = 1.87;

    cooldown -> duration = 15.0;

    crit_bonus += p -> talents.mental_scarring * 0.1;
  }

  virtual void calculate_result()
  {
    jedi_sage_spell_t::calculate_result();

    jedi_sage_t* p = player -> cast_jedi_sage();

    p -> buffs_force_suppression -> trigger( 10 );
  }
};

struct sever_force_t : public jedi_sage_spell_t
{
  sever_force_t( jedi_sage_t* p, const std::string& options_str ) :
    jedi_sage_spell_t( p -> type == SITH_SORCERER ? "creeping_terror" : "sever_force", p, RESOURCE_FORCE )
  {
    check_talent( p -> talents.sever_force );

    parse_options( 0, options_str );
    base_td = 349.4;
    base_tick_time = 3.0;
    num_ticks = 6;
    base_cost = 20.0;
    range = 30.0;
    tick_power_mod = 0.311;
    may_crit = false;
  }
};

} // ANONYMOUS NAMESPACE ====================================================


// ==========================================================================
// jediconsular Character Definition
// ==========================================================================

// jedi_consular_t::create_action ====================================================

action_t* jedi_consular_t::create_action( const std::string& name,
                                 const std::string& options_str )
{
  if ( name == "force_valor"       ) return new        force_valor_t( this, options_str );
  if ( name == "project"           ) return new            project_t( this, options_str );
  if ( name == "telekinetic_throw" ) return new  telekinetic_throw_t( this, options_str );

  return player_t::create_action( name, options_str );
}

// jedi_consular_t::init_base ========================================================

void jedi_consular_t::init_base()
{
  player_t::init_base();


  default_distance = 10;
  distance = default_distance;

  base_gcd = 1.5;

  resource_base[  RESOURCE_FORCE  ] = 100;

  base_force_regen_per_second = 8.0;

  // FIXME: Add defensive constants
  //diminished_kfactor    = 0;
  //diminished_dodge_capi = 0;
  //diminished_parry_capi = 0;
}

// jedi_consular_t::init_buffs =======================================================

void jedi_consular_t::init_buffs()
{
  player_t::init_buffs();

  // buff_t( player, name, max_stack, duration, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

}

// jedi_consular_t::reset ==================================================

void jedi_consular_t::init_resources( bool force )
{
  player_t::init_resources( force);
}

// jedi_consular_t::primary_role ==================================================

int jedi_consular_t::primary_resource() const
{
  return RESOURCE_FORCE;
}

// jedi_consular_t::primary_role ==================================================

int jedi_consular_t::primary_role() const
{
  return ROLE_HYBRID;
}

// ==========================================================================
// jedi_sage Character Definition
// ==========================================================================

// jedi_sage_t::create_action ====================================================

action_t* jedi_sage_t::create_action( const std::string& name,
                                 const std::string& options_str )
{
  if ( name == "disturbance"       ) return new       disturbance_t( this, options_str );
  if ( name == "mind_crush"        ) return new        mind_crush_t( this, options_str );
  if ( name == "weaken_mind"       ) return new       weaken_mind_t( this, options_str );
  if ( name == "turbulence"        ) return new        turbulence_t( this, options_str );
  if ( name == "force_in_balance"  ) return new  force_in_balance_t( this, options_str );
  if ( name == "sever_force"       ) return new       sever_force_t( this, options_str );

  return jedi_consular_t::create_action( name, options_str );
}

// jedi_sage_t::init_talents =====================================================

void jedi_sage_t::init_talents()
{
  jedi_consular_t::init_talents();

  // set talent ranks here for now

  // Telekinetics Spec
/*
  talents.inner_strength = 3;
  talents.mental_longevity = 2;
  talents.clamoring_force = 3;
  talents.minds_eye = 0;
  talents.disturb_mind = 2;
  talents.concentration = 2;
  talents.telekinetic_wave = 1;
  talents.psychic_projection = 2;
  talents.tidal_force = 1;
  talents.telekinetic_effusion = 2;
  talents.tremors = 1;
  talents.telekinetic_momentum = 3;
  talents.mental_alacrity = 1;
  talents.reverberation = 5;
  talents.turbulence = 1;

  talents.empowered_throw = 3;
  talents.will_of_the_jedi = 2;
  talents.upheaval = 3;
  talents.critical_kinsesis = 2;
*/

  // Balance Spec

  talents.empowered_throw = 3;
  talents.will_of_the_jedi = 2;
  talents.upheaval = 3;
  talents.focused_insight = 2;
  talents.critical_kinsesis = 2;
  talents.force_in_balance = 1;
  talents.psychic_barrier = 3;
  talents.telekinetic_balance = 1;
  talents.presence_of_mind = 1;
  talents.force_suppression = 1;
  talents.drain_thoughts = 2;
  talents.assertion = 2;
  talents.mental_scarring = 3;
  talents.sever_force = 1;

  talents.inner_strength = 3;
  talents.mental_longevity = 2;
  talents.clamoring_force = 3;
  talents.minds_eye = 0;
  talents.disturb_mind = 2;

}

// jedi_sage_t::init_base ========================================================

void jedi_sage_t::init_base()
{
  jedi_consular_t::init_base();


  default_distance = 30;
  distance = default_distance;


  resource_base[  RESOURCE_FORCE  ] += 400 + talents.mental_longevity * 50;

  attribute_multiplier_initial[ ATTR_WILLPOWER ] *= 1.0 + talents.will_of_the_jedi * 0.03;

  // FIXME: Add defensive constants
  //diminished_kfactor    = 0;
  //diminished_dodge_capi = 0;
  //diminished_parry_capi = 0;
}

// jedi_sage_t::init_buffs =======================================================

void jedi_sage_t::init_buffs()
{
  jedi_consular_t::init_buffs();

  // buff_t( player, name, max_stack, duration, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

  buffs_concentration = new buff_t( this, "concentration", 3, 10.0, 0.5 * talents.concentration );
  buffs_psychic_projection = new buff_t( this, "psychic_projection", 1, 0.0, 0.5 * talents.psychic_projection, 10.0 );
  buffs_tidal_force = new buff_t( this, "tidal_force", 1, 0.0, 0, 10.0 );
  buffs_telekinetic_effusion = new buff_t( this, "telekinetic_effusion", 2, 0.0, 0.5 * talents.telekinetic_effusion );
  buffs_tremors = new buff_t( this, "tremors", 3, 30.0 );
  buffs_presence_of_mind = new buff_t( this, "presence_of_mind", 1, 0.0, talents.presence_of_mind * 0.3 );
  buffs_force_suppression = new buff_t( this, "force_suppression", 10, 30.0, talents.force_suppression );
}

// jedi_sage_t::init_gains =======================================================

void jedi_sage_t::init_gains()
{
  jedi_consular_t::init_gains();

  gains_concentration   = get_gain( "concentration"   );
  gains_focused_insight = get_gain( "focused_insight" );
  gains_psychic_barrier = get_gain( "psychic_barrier" );
}

// jedi_sage_t::init_procs =======================================================

void jedi_sage_t::init_procs()
{
  jedi_consular_t::init_procs();
}

// jedi_sage_t::init_rng =========================================================

void jedi_sage_t::init_rng()
{
  jedi_consular_t::init_rng();

  rng_psychic_barrier = get_rng( "psychic_barrier" );
  rng_upheaval = get_rng( "upheaval" );
  rng_tm = get_rng( "telekinetic_momentum" );
}

// jedi_sage_t::init_actions =====================================================

void jedi_sage_t::init_actions()
{
  if ( action_list_str.empty() )
  {
    //switch ( primary_tree() )
    //{
    //case TREE_SEER:

    action_list_str += "stim,type=rakata_resolve";

    action_list_str += "/power_potion,if=time>10";

    action_list_str += "/force_valor";

    action_list_str += "/snapshot_stats";

    action_list_str += "/mind_crush";

    action_list_str += "/weaken_mind,if=!ticking";

    if ( talents.force_in_balance > 0 && talents.force_suppression > 0 )
      action_list_str += "/force_in_balance,if=!dot.mind_crush.ticking";

    if ( talents.sever_force > 0 )
      action_list_str += "/sever_force,if=!ticking";

    //action_list_str += "/project";

    action_list_str += "/telekinetic_throw";

    action_list_str += "/disturbance";
    //break;


    //case TREE_TELEKINETICS:
/*
    action_list_str += "stim,type=rakata_resolve";

    action_list_str += "/power_potion,if=time>10";

    action_list_str += "/force_valor";

    action_list_str += "/snapshot_stats";

    action_list_str += "/mind_crush";

    action_list_str += "/weaken_mind,if=!ticking";

    action_list_str += "/project";

    if ( talents.psychic_projection > 0)
    {
      action_list_str += "/telekinetic_throw,if=buff.psychic_projection";

      if ( talents.psychic_projection == 1 )
        action_list_str += ".react";
      else
        action_list_str += ".up";
    }

    if ( talents.turbulence > 0 )
    {
      action_list_str += "/disturbance,if=buff.concentration.down|buff.concentration.remains<4";

      action_list_str += "/turbulence";
    }
*/
    //break;

    //default: break;
    //}

    action_list_default = 1;
  }

  jedi_consular_t::init_actions();
}

// jedi_sage_t::primary_role ==================================================

int jedi_sage_t::primary_role() const
{
  return ROLE_SPELL;
}

// jedi_sage_t::regen ==================================================

void jedi_sage_t::regen( const double periodicity )
{
  jedi_consular_t::regen( periodicity );

  if ( buffs_concentration -> check() > 0 )
  {
    double force_regen = periodicity * force_regen_per_second() * buffs_concentration -> check() * 0.10;

    resource_gain( RESOURCE_FORCE, force_regen, gains_concentration );
  }
}

// jedi_sage_t::composite_spell_power ==================================================

double jedi_sage_t::composite_spell_power( const school_type school ) const
{
  double sp = jedi_consular_t::composite_spell_power( school );

  sp *= 1.0 + buffs_tremors -> stack() * 0.01;

  return sp;
}

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_jedi_sage  ===================================================

player_t* player_t::create_jedi_sage( sim_t* sim, const std::string& name, race_type r )
{
  return new jedi_sage_t( sim, JEDI_SAGE, name, r );
}

// player_t::create_sith_sorcerer  ===================================================

player_t* player_t::create_sith_sorcerer( sim_t* sim, const std::string& name, race_type r )
{
  return new jedi_sage_t( sim, SITH_SORCERER, name, r );
}

// player_t::jedi_sage_init ======================================================

void player_t::jedi_sage_init( sim_t* sim )
{
  for ( unsigned int i = 0; i < sim -> actor_list.size(); i++ )
  {
    player_t* p = sim -> actor_list[i];
    p -> buffs.force_valor = new buff_t( p, "force_valor", 1 );
  }
}

// player_t::jedi_sage_combat_begin ==============================================

void player_t::jedi_sage_combat_begin( sim_t* sim )
{
  for ( player_t* p = sim -> player_list; p; p = p -> next )
  {
    if ( p -> ooc_buffs() )
    {
      if ( sim -> overrides.force_valor )
        p -> buffs.force_valor -> override();
    }
  }
}

