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

struct sith_sorcerer_targetdata_t : public targetdata_t
{
  dot_t* dots_telekinetic_throw;
  dot_t* dots_crushing_darkness;
  dot_t* dots_affliction;
  dot_t* dots_creeping_terror;

  sith_sorcerer_targetdata_t( player_t* source, player_t* target )
    : targetdata_t( source, target )
  {
  }
};

void register_sith_sorcerer_targetdata( sim_t*  sim  )
{
  player_type t = SITH_SORCERER;
  typedef sith_sorcerer_targetdata_t type;

  REGISTER_DOT( telekinetic_throw );
  REGISTER_DOT( crushing_darkness );
  REGISTER_DOT( affliction );
  REGISTER_DOT( creeping_terror );
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
  buff_t* buffs_mental_alacrity;
  buff_t* buffs_force_potency;

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

  benefit_t* benefits_turbulence;
  benefit_t* benefits_fs_weaken_mind;
  benefit_t* benefits_fs_mind_crush;
  benefit_t* benefits_fs_sever_force;

  cooldown_t* cooldowns_telekinetic_wave;

  // Talents
  struct talents_t
  {
    // TREE_SEER
    talent_t* penetrating_light;
    talent_t* psychic_suffusion;

    // TREE_TELEKINETICS
    talent_t* inner_strength;
    talent_t* mental_longevity;
    talent_t* clamoring_force;
    talent_t* minds_eye;
    talent_t* disturb_mind;
    talent_t* concentration;
    talent_t* telekinetic_defense;
    talent_t* blockout;
    talent_t* telekinetic_wave;
    talent_t* psychic_projection;
    talent_t* force_wake;
    talent_t* tidal_force;
    talent_t* telekinetic_effusion;
    talent_t* kinetic_collapse;
    talent_t* tremors;
    talent_t* telekinetic_momentum;
    talent_t* mental_alacrity;
    talent_t* reverberation;
    talent_t* turbulence;

    // TREE_BALANCE
    talent_t* empowered_throw;
    talent_t* jedi_resistance;
    talent_t* will_of_the_jedi;
    talent_t* pinning_resolve;
    talent_t* upheaval;
    talent_t* focused_insight;
    talent_t* critical_kinesis;
    talent_t* force_in_balance;
    talent_t* psychic_barrier;
    talent_t* telekinetic_balance;
    talent_t* containment;
    talent_t* mind_ward;
    talent_t* presence_of_mind;
    talent_t* force_suppression;
    talent_t* drain_thoughts;
    talent_t* assertion;
    talent_t* mental_scarring;
    talent_t* psychic_absorption;
    talent_t* sever_force;

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
      cooldowns_telekinetic_wave = get_cooldown( "chain_lightning" );
    }
    else
    {
      tree_type[ JEDI_SAGE_SEER ]         = TREE_SEER;
      tree_type[ JEDI_SAGE_TELEKINETICS ] = TREE_TELEKINETICS;
      tree_type[ JEDI_SAGE_BALANCE ]      = TREE_BALANCE;
      cooldowns_telekinetic_wave = get_cooldown( "telekinetic_wave" );
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
  virtual void      init_benefits();
  virtual void      init_buffs();
  virtual void      init_gains();
  virtual void      init_procs();
  virtual void      init_rng();
  virtual void      init_actions();
  virtual int       primary_role() const;
  virtual void      regen( timespan_t periodicity );
  virtual double    composite_force_damage_bonus() const;
  virtual double    composite_spell_alacrity() const;
  virtual void      create_talents();
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
  bool influenced_by_inner_strength;

  jedi_consular_spell_t( const char* n, jedi_consular_t* p, int r=RESOURCE_NONE, const school_type s=SCHOOL_HOLY, int t=TREE_NONE ) :
    spell_t( n, p, r, s, t ),
    influenced_by_inner_strength( true )
  {
    _init_jedi_consular_spell_t();
  }

  virtual void init()
  {
    spell_t::init();

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( base_td > 0 && !channeled )
        crit_bonus += p -> talents.mental_scarring -> rank() * 0.1;
    }
  }

  void _init_jedi_consular_spell_t()
  {
    may_crit   = true;
    tick_may_crit = true;
  }

  virtual void assess_damage( player_t* t, double dmg_amount, int dmg_type, int dmg_result )
  {
    spell_t::assess_damage( t, dmg_amount, dmg_type, dmg_result );

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      // Procs from all critical damage including dot ticks. Source:  17/01/2012 http://sithwarrior.com/forums/Thread-Sorcerer-Sage-Mechanics-and-Quirks

      if ( dmg_result == RESULT_CRIT && p -> talents.telekinetic_effusion -> rank() > 0 )
      {
        p -> buffs_telekinetic_effusion -> trigger( 2 );
      }
    }
  }

  virtual timespan_t execute_time() const
  {
    timespan_t et = spell_t::execute_time();

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( base_execute_time > timespan_t::zero && p -> buffs_presence_of_mind -> up() )
        et = timespan_t::zero;
    }

    return et;
  }

  virtual void player_buff()
  {
    spell_t::player_buff();

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( base_execute_time > timespan_t::zero && p -> buffs_presence_of_mind -> up() )
        player_dd_multiplier *= 1.20;

      if ( p -> buffs_force_potency -> up() && base_dd_min > 0 )
        player_crit += 0.60;
    }
  }

  virtual void target_debuff( player_t* t, int dmg_type )
  {
    spell_t::target_debuff( t, dmg_type );

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      // This method is in target_debuff so that it is checked before every dot tick

      // Assume channeled spells don't profit
      if ( p -> talents.force_suppression -> rank() > 0 && base_td > 0 && !channeled )
        if ( p -> buffs_force_suppression -> up() )
          target_td_multiplier *= 1.20;
    }
  }

  virtual double cost() const
  {
    double c = spell_t::cost();

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();
      if ( p -> talents.inner_strength -> rank() > 0 && influenced_by_inner_strength )
      {
        c *= 1.0 - p -> talents.inner_strength -> rank() * 0.03;
        c = ceil( c ); // 17/01/2012 According to http://sithwarrior.com/forums/Thread-Sorcerer-Sage-Mechanics-and-Quirks
      }

      if ( p -> buffs_telekinetic_effusion -> check() > 0 )
      {
        c *= 0.5;
        c = floor( c ); // FIXME: floor or ceil?
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

      if ( base_execute_time > timespan_t::zero )
        p -> buffs_presence_of_mind -> expire();

      if ( base_dd_min > 0 )
        p -> buffs_force_potency -> decrement();
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
  force_valor_t( jedi_consular_t* p, const std::string& n, const std::string& options_str ) :
    jedi_consular_spell_t( n.c_str(), p, RESOURCE_FORCE )
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


  project_t( jedi_consular_t* p, const std::string& n, const std::string& options_str, bool is_upheaval = false ) :
    jedi_consular_spell_t( ( n + std::string( is_upheaval ? "_upheaval" : "" ) ).c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC ),
    upheaval( 0 )
  {
    parse_options( 0, options_str );
    base_dd_min = 219.0; base_dd_max = 283.4;
    base_cost = 45.0;
    range = 30.0;
    direct_power_mod = 1.85;

    cooldown -> duration = timespan_t::from_seconds( 6.0 );

    if ( p -> type == JEDI_SAGE )
      travel_speed = 40.0; // 0.5s travel time for range=20, 0.75s for range=30

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( !is_upheaval && p -> talents.upheaval -> rank() > 0 )
      {
        upheaval = new project_t( p, n, options_str, true );
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

        if ( p -> rng_upheaval -> roll( p -> talents.upheaval -> rank() * 0.15 ) )
          upheaval -> execute();
      }
    }
  }
};

struct telekinetic_throw_t : public jedi_consular_spell_t
{
  bool is_buffed_by_psychic_projection;

  telekinetic_throw_t( jedi_consular_t* p, const std::string& n, const std::string& options_str ) :
    jedi_consular_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC ),
    is_buffed_by_psychic_projection( false )
  {
    parse_options( 0, options_str );
    base_td = 127.2;
    base_cost = 30.0;
    range = 30.0;
    tick_power_mod = 0.79;
    num_ticks = 3;
    base_tick_time = timespan_t::from_seconds( 1.0 );
    may_crit = false;
    channeled = true;
    tick_zero = true;

    cooldown -> duration = timespan_t::from_seconds( 6.0 );

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      base_crit += p -> talents.critical_kinesis -> rank() * 0.05;

      base_multiplier *= 1.0 + p -> talents.empowered_throw -> rank() * 0.04;

      if ( p -> talents.telekinetic_balance -> rank() > 0 )
        cooldown -> duration = timespan_t::zero;
    }
  }

  virtual void execute()
  {
    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( p -> buffs_psychic_projection -> up() )
        is_buffed_by_psychic_projection = true;
      else
        is_buffed_by_psychic_projection = false;
    }

    jedi_consular_spell_t::execute();
  }

  virtual timespan_t tick_time() const
  {
    timespan_t tt = jedi_consular_spell_t::tick_time();

    if ( is_buffed_by_psychic_projection )
      tt *= 0.5;

    return tt;
  }

  virtual void last_tick( dot_t* d )
  {
    jedi_consular_spell_t::last_tick( d );

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( is_buffed_by_psychic_projection )
        p -> buffs_psychic_projection -> expire();
    }
  }

  virtual void tick( dot_t* d )
  {
    jedi_consular_spell_t::tick( d );

    if ( player -> is_jedi_sage() )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( tick_dmg > 0 )
      {
        if ( p -> talents.psychic_barrier -> rank() > 0 && p -> rng_psychic_barrier -> roll( p -> talents.psychic_barrier -> rank() * ( 1 / 3.0 ) ) )
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
    jedi_consular_spell_t::tick( d );

    jedi_sage_t* p = player -> cast_jedi_sage();

    if ( tick_dmg > 0 && p -> talents.focused_insight -> rank() > 0 )
    {
      double hp = p -> resource_max[ RESOURCE_HEALTH ] * p -> talents.focused_insight -> rank() * 0.005;
      p -> resource_gain( RESOURCE_HEALTH, hp , p -> gains_focused_insight );
    }
  }
};

void trigger_tidal_force( action_t* a, double pc = 0 )
{
  jedi_sage_t* p = a -> player -> cast_jedi_sage();

  if ( p -> talents.tidal_force -> rank() == 0 )
    return;

  if ( p -> buffs_tidal_force -> trigger( 1, 0, pc ) )
  {
    p -> cooldowns_telekinetic_wave -> reset();
  }
}

struct disturbance_t : public jedi_sage_spell_t
{
  jedi_consular_spell_t* tm;

  disturbance_t( jedi_sage_t* p, const std::string& n, const std::string& options_str, bool is_tm = false ) :
    jedi_sage_spell_t( ( n + std::string( is_tm ? "_tm" : "" ) ).c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC ),
    tm( 0 )
  {
    parse_options( 0, options_str );
    base_dd_min = 180.3; base_dd_max = 244.7;
    base_execute_time = timespan_t::from_seconds( 1.5 );
    base_cost = 30.0;
    range = 30.0;
    direct_power_mod = 1.32;

    base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02;

    base_crit += p -> talents.critical_kinesis -> rank() * 0.05;

    if ( !is_tm )
    {
      if ( p -> talents.telekinetic_momentum -> rank() > 0 )
      {
        tm = new disturbance_t( p, n, options_str, true );
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

      // Does the TM version also proc Tidal Force?
      trigger_tidal_force( this, 0.3 );
    }
  }

  virtual void execute()
  {
    jedi_consular_spell_t::execute();

    jedi_sage_t* p = player -> cast_jedi_sage();

    if ( tm )
    {
      if ( p -> rng_tm -> roll( p -> talents.telekinetic_momentum -> rank() * 0.10 ) )
      {
        tm -> execute();
        p -> buffs_tremors -> trigger( 1 );
      }
    }
  }
};

struct mind_crush_dot_t : public jedi_sage_spell_t
{
  mind_crush_dot_t( jedi_sage_t* p, const std::string& n ) :
    jedi_sage_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC )
  {
    base_td = 47.5;
    base_tick_time = timespan_t::from_seconds( 1.0 );
    num_ticks = 6 + p -> talents.assertion -> rank() * 1;
    range = 30.0;
    tick_power_mod = 0.295;
    influenced_by_inner_strength = false;
    background = true;
    may_crit = false;

    base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02;
  }

  virtual void target_debuff( player_t* t, int dmg_type )
  {
    jedi_sage_spell_t::target_debuff( t, dmg_type );

    jedi_sage_t* p = player -> cast_jedi_sage();

    if ( p -> talents.force_suppression -> rank() > 0 )
      p -> benefits_fs_mind_crush -> update( p -> buffs_force_suppression -> check() > 0 );
  }
};

struct mind_crush_t : public jedi_sage_spell_t
{
  jedi_sage_spell_t* dot_spell;

  mind_crush_t( jedi_sage_t* p, const std::string& n, const std::string& options_str ) :
    jedi_sage_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC ),
    dot_spell( 0 )
  {
    parse_options( 0, options_str );
    base_dd_min = 165.83; base_dd_max = 230.23;
    base_execute_time = timespan_t::from_seconds( 2.0 );
    base_cost = 40.0;
    range = 30.0;
    direct_power_mod = 1.23;
    cooldown -> duration = timespan_t::from_seconds( 15.0 );
    influenced_by_inner_strength = false;

    base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02;

    dot_spell = new mind_crush_dot_t( p, ( n + "_dot").c_str() );

    assert( dot_spell );

    add_child( dot_spell );
  }

  virtual void execute()
  {
    jedi_sage_spell_t::execute();

    dot_spell -> execute();
  }
};

struct weaken_mind_t : public jedi_sage_spell_t
{
  weaken_mind_t( jedi_sage_t* p, const std::string& n, const std::string& options_str ) :
    jedi_sage_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL )
  {
    parse_options( 0, options_str );
    base_td = 49.91;
    base_tick_time = timespan_t::from_seconds( 3.0 );
    num_ticks = 5 + p -> talents.disturb_mind -> rank();
    base_cost = 35.0;
    range = 30.0;
    tick_power_mod = 0.31;
    may_crit = false;
    crit_bonus += p -> talents.reverberation -> rank() * 0.1;
    base_multiplier *= 1.0 + p -> talents.drain_thoughts -> rank() * 0.075;
    influenced_by_inner_strength = false;
  }

  virtual void tick( dot_t* d )
  {
    jedi_sage_spell_t::tick( d );

    jedi_sage_t* p = player -> cast_jedi_sage();

    if ( result == RESULT_CRIT && p -> talents.psychic_projection -> rank() > 0 )
    {
      p -> buffs_psychic_projection -> trigger();
    }
  }

  virtual void target_debuff( player_t* t, int dmg_type )
  {
    jedi_sage_spell_t::target_debuff( t, dmg_type );

    jedi_sage_t* p = player -> cast_jedi_sage();

    if ( p -> talents.force_suppression -> rank() > 0 )
      p -> benefits_fs_weaken_mind -> update( p -> buffs_force_suppression -> check() > 0 );
  }
};

struct turbulence_t : public jedi_sage_spell_t
{
  jedi_consular_spell_t* tm;

  turbulence_t( jedi_sage_t* p, const std::string& n, const std::string& options_str ) :
    jedi_sage_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL )
  {
    check_talent( p -> talents.turbulence -> rank() );

    parse_options( 0, options_str );
    base_dd_min = 222.2; base_dd_max = 286.6;
    base_execute_time = timespan_t::from_seconds( 2.0 );
    base_cost = 45.0;
    range = 30.0;
    direct_power_mod = 1.58;
    crit_bonus += p -> talents.reverberation -> rank() * 0.1;

    base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02;
    cooldown -> duration = timespan_t::from_seconds( 9.0 );
  }

  virtual void calculate_result()
  {
    jedi_sage_spell_t::calculate_result();

    bool t = false;
    if ( player -> type == JEDI_SAGE )
    {
      jedi_sage_targetdata_t* td = targetdata() -> cast_jedi_sage();

      if ( td -> dots_weaken_mind -> ticking )
      {
        t = true;
        result = RESULT_CRIT;
      }
    }
    else if ( player -> type == SITH_SORCERER )
    {
      sith_sorcerer_targetdata_t* td = targetdata() -> cast_sith_sorcerer();

      if ( td -> dots_affliction -> ticking )
      {
        t = true;
        result = RESULT_CRIT;
      }
    }
    else
      assert( 0 );

    player -> cast_jedi_sage() -> benefits_turbulence -> update( t );
  }
};

struct force_in_balance_t : public jedi_sage_spell_t
{
  force_in_balance_t( jedi_sage_t* p, const std::string& n, const std::string& options_str ) :
    jedi_sage_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL )
  {
    check_talent( p -> talents.force_in_balance -> rank() );

    parse_options( 0, options_str );
    base_dd_min = 268.9; base_dd_max = 333.3;
    ability_lag = timespan_t::from_seconds( 0.2 );
    base_cost = 50.0;
    range = 30.0;
    direct_power_mod = 1.87;

    cooldown -> duration = timespan_t::from_seconds( 15.0 );

    crit_bonus += p -> talents.mental_scarring -> rank() * 0.1;
    base_multiplier *= 1.0 + p -> talents.psychic_suffusion -> rank() * 0.05;
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
  sever_force_t( jedi_sage_t* p, const std::string& n, const std::string& options_str ) :
    jedi_sage_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL )
  {
    check_talent( p -> talents.sever_force -> rank() );

    parse_options( 0, options_str );
    base_td = 50;
    base_tick_time = timespan_t::from_seconds( 3.0 );
    num_ticks = 6;
    base_cost = 20.0;
    range = 30.0;
    tick_power_mod = 0.311;
    may_crit = false;
    cooldown -> duration = timespan_t::from_seconds( 9.0 );
    tick_zero = true;
    influenced_by_inner_strength = false;
  }

  virtual void target_debuff( player_t* t, int dmg_type )
  {
    jedi_sage_spell_t::target_debuff( t, dmg_type );

    jedi_sage_t* p = player -> cast_jedi_sage();

    if ( p -> talents.force_suppression -> rank() > 0 )
      p -> benefits_fs_sever_force -> update( p -> buffs_force_suppression -> check() > 0 );
  }
};

struct mental_alacrity_t : public jedi_sage_spell_t
{
  mental_alacrity_t( jedi_sage_t* p, const std::string& n, const std::string& options_str ) :
    jedi_sage_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL )
  {
    check_talent( p -> talents.mental_alacrity -> rank() );

    parse_options( 0, options_str );
    base_cost = 30.0;
    cooldown -> duration = timespan_t::from_seconds( 120.0 );
    harmful = false;

    // TODO: Does it trigger a gcd?
  }

  virtual void execute()
  {
    jedi_sage_spell_t::execute();

    jedi_sage_t* p = player -> cast_jedi_sage();

    p -> buffs_mental_alacrity -> trigger();
  }
};

struct telekinetic_wave_t : public jedi_sage_spell_t
{
  jedi_consular_spell_t* tm;

  telekinetic_wave_t( jedi_sage_t* p, const std::string& n, const std::string& options_str, bool is_tm = false ) :
    jedi_sage_spell_t( ( n + std::string( is_tm ? "_tm" : "" ) ).c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC ),
    tm( 0 )
  {
    check_talent( p -> talents.telekinetic_wave -> rank() );

    parse_options( 0, options_str );
    base_cost = 50.0;
    cooldown -> duration = timespan_t::from_seconds( 6.0 );
    base_dd_min = 293.02; base_dd_max = 357.42;
    base_execute_time = timespan_t::from_seconds( 3.0 );
    range = 30.0;
    direct_power_mod = 2.02;

    base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02 + p -> talents.psychic_suffusion -> rank() * 0.05;
    crit_bonus += p -> talents.reverberation -> rank() * 0.1;

    if ( !is_tm )
    {
      if ( p -> talents.telekinetic_momentum -> rank() > 0 )
      {
        tm = new telekinetic_wave_t( p, n, options_str, true );
        tm -> base_multiplier *= 0.30;
        tm -> base_cost = 0.0;
        tm -> background = true;
        add_child( tm );
      }
    }
  }

  virtual void execute()
  {
    jedi_consular_spell_t::execute();

    if ( tm )
    {
      jedi_sage_t* p = player -> cast_jedi_sage();

      if ( p -> rng_tm -> roll( p -> talents.telekinetic_momentum -> rank() * 0.10 ) )
      {
        tm -> execute();
        p -> buffs_tremors -> trigger( 1 );
      }
    }
  }

  virtual timespan_t execute_time() const
  {
    timespan_t et = jedi_consular_spell_t::execute_time();

    jedi_sage_t* p = player -> cast_jedi_sage();

    if ( p -> buffs_tidal_force -> up() )
      et = timespan_t::zero;

    return et;
  }

  virtual void update_ready()
  {
    jedi_consular_spell_t::update_ready();

    jedi_sage_t* p = player -> cast_jedi_sage();

    p -> buffs_tidal_force -> expire();
  }
};

struct force_potency_t : public jedi_sage_spell_t
{
  force_potency_t( jedi_sage_t* p, const std::string& n, const std::string& options_str ) :
    jedi_sage_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL )
  {
    check_talent( p -> talents.mental_alacrity -> rank() );

    parse_options( 0, options_str );
    cooldown -> duration = timespan_t::from_seconds( 90.0 );
    harmful = false;

    trigger_gcd = timespan_t::zero;
  }

  virtual void execute()
  {
    jedi_sage_spell_t::execute();

    jedi_sage_t* p = player -> cast_jedi_sage();

    p -> buffs_force_potency -> trigger( 2 );
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
  if ( type == JEDI_SAGE )
  {
    if ( name == "force_valor"       ) return new        force_valor_t( this, "force_valor", options_str );
    if ( name == "project"           ) return new            project_t( this, "project", options_str );
    if ( name == "telekinetic_throw" ) return new  telekinetic_throw_t( this, "telekinetic_throw", options_str );
  }
  else if ( type == SITH_SORCERER )
  {
    if ( name == "mark_of_power"   ) return new        force_valor_t( this, "mark_of_power", options_str );
    if ( name == "shock"           ) return new            project_t( this, "shock", options_str );
    if ( name == "force_lightning" ) return new  telekinetic_throw_t( this, "force_lightning", options_str );
  }

  return player_t::create_action( name, options_str );
}

// jedi_consular_t::init_base ========================================================

void jedi_consular_t::init_base()
{
  player_t::init_base();


  default_distance = 10;
  distance = default_distance;

  base_gcd = timespan_t::from_seconds( 1.5 );

  resource_base[  RESOURCE_FORCE  ] = 100;

  base_force_regen_per_second = 8.0;


  attribute_base[ ATTR_WILLPOWER ] = 250;

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
  if ( type == JEDI_SAGE )
  {
    if ( name == "disturbance"        ) return new       disturbance_t( this, "disturbance", options_str );
    if ( name == "mind_crush"         ) return new        mind_crush_t( this, "mind_crush", options_str );
    if ( name == "weaken_mind"        ) return new       weaken_mind_t( this, "weaken_mind", options_str );
    if ( name == "turbulence"         ) return new        turbulence_t( this, "turbulence", options_str );
    if ( name == "force_in_balance"   ) return new  force_in_balance_t( this, "force_in_balance", options_str );
    if ( name == "sever_force"        ) return new       sever_force_t( this, "sever_force", options_str );
    if ( name == "mental_alacrity"    ) return new   mental_alacrity_t( this, "mental_alacrity", options_str );
    if ( name == "telekinetic_wave"   ) return new  telekinetic_wave_t( this, "telekinetic_wave", options_str );
    if ( name == "force_potency"      ) return new     force_potency_t( this, "force_potency", options_str );
  }
  else if ( type == SITH_SORCERER )
  {
    if ( name == "lightning_strike"   ) return new       disturbance_t( this, "lightning_strike", options_str );
    if ( name == "crushing_darkness"  ) return new        mind_crush_t( this, "crushing_darkness", options_str );
    if ( name == "affliction"         ) return new       weaken_mind_t( this, "affliction", options_str );
    if ( name == "thundering_blast"   ) return new        turbulence_t( this, "thundering_blast", options_str );
    if ( name == "death_field"        ) return new  force_in_balance_t( this, "death_field", options_str );
    if ( name == "creeping_terror"    ) return new       sever_force_t( this, "creeping_terror", options_str );
    if ( name == "polarity_shift"     ) return new   mental_alacrity_t( this, "polarity_shift", options_str );
    if ( name == "chain_lightning"    ) return new  telekinetic_wave_t( this, "chain_lightning", options_str );
    if ( name == "recklessness"       ) return new     force_potency_t( this, "recklessness", options_str );
  }

  return jedi_consular_t::create_action( name, options_str );
}

// jedi_sage_t::init_talents =====================================================

void jedi_sage_t::init_talents()
{
  jedi_consular_t::init_talents();

  talents.penetrating_light = find_talent( "Penetrating Light" );
  talents.psychic_suffusion = find_talent( "Psychic Suffusion" );


  talents.inner_strength = find_talent( "Inner Strength" );
  talents.mental_longevity = find_talent( "Mental Longevity" );
  talents.clamoring_force = find_talent( "Clamoring Force" );
  talents.minds_eye = find_talent( "Minds Eye" );
  talents.disturb_mind = find_talent( "Disturb Mind" );
  talents.concentration = find_talent( "Concentration" );
  talents.telekinetic_defense = find_talent( "Telekinetic Defense" );
  talents.blockout = find_talent( "Blockout" );
  talents.telekinetic_wave = find_talent( "Telekinetic Wave" );
  talents.psychic_projection = find_talent( "Psychic Projection" );
  talents.force_wake = find_talent( "Force Wake" );
  talents.tidal_force = find_talent( "Tidal Force" );
  talents.telekinetic_effusion = find_talent( "Telekinetic Effusion" );
  talents.kinetic_collapse = find_talent( "Kinetic Collapse" );
  talents.tremors = find_talent( "Tremors" );
  talents.telekinetic_momentum = find_talent( "Telekinetic Momentum" );
  talents.mental_alacrity = find_talent( "Mental Alacrity" );
  talents.reverberation = find_talent( "Reverberation" );
  talents.turbulence = find_talent( "Turbulence" );


  // TREE_BALANCE
  talents.empowered_throw = find_talent( "Empowered Throw" );
  talents.jedi_resistance = find_talent( "Jedi Resistance" );
  talents.will_of_the_jedi = find_talent( "Will of the Jedi" );
  talents.pinning_resolve = find_talent( "Pinning Resolve" );
  talents.upheaval = find_talent( "Upheaval" );
  talents.focused_insight = find_talent( "Focused Insight" );
  talents.critical_kinesis = find_talent( "Critical Kinesis" );
  talents.force_in_balance = find_talent( "Force in Balance" );
  talents.psychic_barrier = find_talent( "Psychic Barrier" );
  talents.telekinetic_balance = find_talent( "Telekinetic Balance" );
  talents.containment = find_talent( "Containment" );
  talents.mind_ward = find_talent( "Mind Ward" );
  talents.presence_of_mind = find_talent( "Presence of Mind" );
  talents.force_suppression = find_talent( "Force Suppression" );
  talents.drain_thoughts = find_talent( "Drain Thoughts" );
  talents.assertion = find_talent( "Assertion" );
  talents.mental_scarring = find_talent( "Mental Scarring" );
  talents.psychic_absorption = find_talent( "Psychic Absorption" );
  talents.sever_force = find_talent( "Sever Force" );


}

// jedi_sage_t::init_base ========================================================

void jedi_sage_t::init_base()
{
  jedi_consular_t::init_base();


  default_distance = 30;
  distance = default_distance;


  resource_base[  RESOURCE_FORCE  ] += 400 + talents.mental_longevity -> rank() * 50;

  attribute_multiplier_initial[ ATTR_WILLPOWER ] += talents.will_of_the_jedi -> rank() * 0.03;

  base_spell_crit += talents.penetrating_light -> rank() * 0.01;

  // FIXME: Add defensive constants
  //diminished_kfactor    = 0;
  //diminished_dodge_capi = 0;
  //diminished_parry_capi = 0;
}
// jedi_sage_t::init_benefits =======================================================

void jedi_sage_t::init_benefits()
{
  jedi_consular_t::init_benefits();

  if ( type == SITH_SORCERER )
  {
    benefits_turbulence = get_benefit( "Thundering Blast automatic crit" );
    benefits_fs_weaken_mind = get_benefit( "Affliction ticks with Deathmark");
    benefits_fs_mind_crush = get_benefit( "Crushing Darkness ticks with Deathmark");
    benefits_fs_sever_force = get_benefit( "Creeping Terror ticks with Deathmarkc");
  }
  else
  {
    benefits_turbulence = get_benefit( "Turbulence automatic crit" );
    benefits_fs_weaken_mind = get_benefit( "Weaken Mind ticks with Force Suppression");
    benefits_fs_mind_crush = get_benefit( "Mind Crush ticks with Force Suppression");
    benefits_fs_sever_force = get_benefit( "Sever Force ticks with Force Suppression");
  }
}

// jedi_sage_t::init_buffs =======================================================

void jedi_sage_t::init_buffs()
{
  jedi_consular_t::init_buffs();

  // buff_t( player, name, max_stack, duration, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

  bool is_sage = ( type == JEDI_SAGE );

  buffs_concentration = new buff_t( this, is_sage ? "concentration" : "subversion", 3, timespan_t::from_seconds( 10.0 ), timespan_t::zero, 0.5 * talents.concentration -> rank() );
  buffs_psychic_projection = new buff_t( this, is_sage ? "psychic_projection" : "lightning_barrage", 1, timespan_t::zero, timespan_t::from_seconds( 10.0 ), 0.5 * talents.psychic_projection -> rank() );
  buffs_tidal_force = new buff_t( this, is_sage ? "tidal_force" : "lightning_storm", 1, timespan_t::zero, timespan_t::from_seconds( 10.0 ) );
  buffs_telekinetic_effusion = new buff_t( this, is_sage ? "telekinetic_effusion" : "lightning_effusion", 2, timespan_t::zero, timespan_t::zero, 0.5 * talents.telekinetic_effusion -> rank() );
  buffs_tremors = new buff_t( this, is_sage ? "tremors" : "conduction", 3, timespan_t::from_seconds( 30.0 ) );
  buffs_presence_of_mind = new buff_t( this, is_sage ? "presence_of_mind" : "wrath", 1, timespan_t::zero, timespan_t::zero, talents.presence_of_mind -> rank() * 0.3 );
  buffs_force_suppression = new buff_t( this, is_sage ? "force_suppression" : "deathmark", 10, timespan_t::from_seconds( 30.0 ), timespan_t::zero, talents.force_suppression -> rank() );
  buffs_mental_alacrity = new buff_t( this, is_sage ? "mental_alacrity" : "polarity_shift", 1, timespan_t::from_seconds( 10.0 ) );
  buffs_force_potency = new buff_t( this, is_sage ? "force_potency" : "recklessness", 2, timespan_t::from_seconds( 20.0 ) );
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

//======================================================================================
//
//   Please Mirror all changes between Jedi Sage and Sith Sorcerer!!!
//
//======================================================================================

  if ( action_list_str.empty() )
  {
    if ( type == JEDI_SAGE )
    {
      switch ( primary_tree() )
      {
      case TREE_BALANCE:

      action_list_str += "stim,type=rakata_resolve";

      action_list_str += "/power_potion,if=time>10";

      action_list_str += "/force_valor";

      action_list_str += "/snapshot_stats";

      action_list_str += "/weaken_mind,if=!ticking";

      if ( talents.force_in_balance -> rank() > 0 && talents.force_suppression -> rank() > 0 )
        action_list_str += "/force_in_balance";

      if ( talents.sever_force -> rank() > 0 )
        action_list_str += "/sever_force,if=!ticking";

      action_list_str += "/mind_crush";

      action_list_str += "/telekinetic_throw";

      action_list_str += "/disturbance";

      break;


      case TREE_TELEKINETICS:

      action_list_str += "stim,type=rakata_resolve";

      action_list_str += "/power_potion,if=time>10";

      action_list_str += "/force_valor";

      action_list_str += "/snapshot_stats";

      action_list_str += "/weaken_mind,if=!ticking";

      if ( talents.turbulence -> rank() > 0 )
        action_list_str += "/turbulence,if=dot.weaken_mind.remains>cast_time";

      if ( talents.psychic_projection -> rank() > 0)
        action_list_str += "/telekinetic_throw,if=buff.psychic_projection.react";

      if ( talents.telekinetic_wave -> rank() > 0 && talents.tidal_force -> rank() > 0 )
        action_list_str += "/telekinetic_wave,if=buff.tidal_force.react";

      action_list_str += "/mental_alacrity,moving=0";

      action_list_str += "/mind_crush";

      action_list_str += "/disturbance";

      action_list_str += "/project,moving=1";

      break;

      default: break;
      }

      action_list_default = 1;
    }
    // Sith Sorcerer
    else
    {
      switch ( primary_tree() )
      {
      case TREE_MADNESS:

      action_list_str += "stim,type=rakata_resolve";

      action_list_str += "/power_potion,if=time>10";

      action_list_str += "/mark_of_power";

      action_list_str += "/snapshot_stats";

      action_list_str += "/affliction,if=!ticking";

      if ( talents.force_in_balance -> rank() > 0 && talents.force_suppression -> rank() > 0 )
        action_list_str += "/death_field";

      if ( talents.sever_force -> rank() > 0 )
        action_list_str += "/creeping_terror,if=!ticking";

      action_list_str += "/crushing_darkness";

      action_list_str += "/force_lightning";

      action_list_str += "/lightning_strike";
      break;


      case TREE_LIGHTNING:

      action_list_str += "stim,type=rakata_resolve";

      action_list_str += "/power_potion,if=time>10";

      action_list_str += "/mark_of_power";

      action_list_str += "/snapshot_stats";

      action_list_str += "/affliction,if=!ticking";

      if ( talents.turbulence -> rank() > 0 )
        action_list_str += "/thundering_blast,if=dot.affliction.remains>cast_time";

      if ( talents.psychic_projection -> rank() > 0)
        action_list_str += "/force_lightning,if=buff.lightning_barrage.react";

      if ( talents.telekinetic_wave -> rank() > 0 && talents.tidal_force -> rank() > 0 )
        action_list_str += "/chain_lightning,if=buff.lightning_storm.react";

      action_list_str += "/polarity_shift,moving=0";

      action_list_str += "/crushing_darkness";

      action_list_str += "/lightning_strike";

      action_list_str += "/shock,moving=1";

      break;

      default: break;
      }

      action_list_default = 1;
    }
  }

  jedi_consular_t::init_actions();
}

// jedi_sage_t::primary_role ==================================================

int jedi_sage_t::primary_role() const
{
  return ROLE_SPELL;
}

// jedi_sage_t::regen ==================================================

void jedi_sage_t::regen( timespan_t periodicity )
{
  jedi_consular_t::regen( periodicity );

  if ( buffs_concentration -> check() > 0 )
  {
    double force_regen = periodicity.total_seconds() * force_regen_per_second() * buffs_concentration -> check() * 0.10;

    resource_gain( RESOURCE_FORCE, force_regen, gains_concentration );
  }
}

// jedi_sage_t::composite_spell_power ==================================================

double jedi_sage_t::composite_force_damage_bonus() const
{
  double sp = jedi_consular_t::composite_force_damage_bonus();

  sp *= 1.0 + buffs_tremors -> stack() * 0.01;

  return sp;
}

// jedi_sage_t::composite_spell_alacrity ==================================================

double jedi_sage_t::composite_spell_alacrity() const
{
  double sh = jedi_consular_t::composite_spell_alacrity();

  sh -= buffs_mental_alacrity -> stack() * 0.20;

  return sh;
}

// jedi_sage_t::create_talents ==================================================

void jedi_sage_t::create_talents()
{

  talent_trees[ 0 ].push_back(  new talent_t( this, "Immutable Force", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Penetrating Light", 0, 3 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Wisdom", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Foresight", 0, 3 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Pain Bearer", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Psychic Suffusion", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Conveyance", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Rejuvenate", 0, 1 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Valiance", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Preservation", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Mend Wounds", 0, 1 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Force Shelter", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Egress", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Confound", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Healing Trance", 0, 1 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Serenity", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Resplendence", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Clairvoyance", 0, 3 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Salvation", 0, 1 ) );


  talent_trees[ 1 ].push_back(  new talent_t( this, "Inner Strength", 1, 3 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Mental Longevity", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Clamoring Force", 1, 3 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Minds Eye", 1, 1 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Disturb Mind", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Concentration", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Telekinetic Defense", 1,  2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Blockout", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Telekinetic Wave", 1, 1 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Psychic Projection", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Force Wake", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Tidal Force", 1, 1 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Telekinetic Effusion", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Kinetic Collapse", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Tremors", 1, 1 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Telekinetic Momentum", 1, 3 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Mental Alacrity", 1, 1 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Reverberation", 1, 5 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Turbulence", 1, 1 ) );

   // TREE BALANCE
  talent_trees[ 2 ].push_back(  new talent_t( this, "Empowered Throw", 2, 3 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Jedi Resistance", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Will of the Jedi", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Pinning Resolve", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Upheaval", 2, 3 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Focused Insight", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Critical Kinesis", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Force in Balance", 2, 1 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Psychic Barrier", 2, 3 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Telekinetic Balance", 2, 1 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Containment", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Mind Ward", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Presence of Mind", 2, 1 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Force Suppression", 2, 1 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Drain Thoughts", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Assertion", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Mental Scarring", 2, 3 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Psychic Absorption", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Sever Force", 2, 1 ) );


  jedi_consular_t::create_talents();
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
    p -> buffs.force_valor = new buff_t( p, "force_valor_mark_of_power", 1 );
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

