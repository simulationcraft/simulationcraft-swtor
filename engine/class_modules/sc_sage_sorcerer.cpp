// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "../simulationcraft.hpp"

struct sage_sorcerer_targetdata_t : public targetdata_t
{
  dot_t dot_telekinetic_throw;
  dot_t dot_mind_crush;
  dot_t dot_weaken_mind;
  dot_t dot_sever_force;

  dot_t dot_healing_trance;
  dot_t dot_salvation;
  dot_t dot_rejuvenate;

  sage_sorcerer_targetdata_t( player_t& source, player_t& target )
    : targetdata_t( source, target ),
      dot_telekinetic_throw( "telekinetic_throw", &source ),
      dot_mind_crush( "mind_crush", &source ),
      dot_weaken_mind( "weaken_mind", &source ),
      dot_sever_force( "sever_force", &source ),
      dot_healing_trance( "healing_trance", &source ),
      dot_salvation( "salvation", &source ),
      dot_rejuvenate( "rejuvenate", &source)
  {
    add( dot_telekinetic_throw );
    alias( dot_telekinetic_throw, "force_lightning" );

    add( dot_mind_crush );
    alias( dot_mind_crush, "mind_crush_dot" );
    alias( dot_mind_crush, "crushing_darkness" );
    alias( dot_mind_crush, "crushing_darkness_dot" );

    add( dot_weaken_mind );
    alias( dot_weaken_mind, "affliction" );

    add( dot_sever_force );
    alias( dot_sever_force, "creeping_terror" );

    add( dot_healing_trance );
    alias( dot_healing_trance, "innervate" );

    add( dot_salvation );
    alias( dot_salvation, "revivification" );

    add( dot_rejuvenate );
    alias( dot_rejuvenate, "resurgence" );
  }
};

// ==========================================================================
// Jedi Sage
// ==========================================================================

struct sage_sorcerer_t : public player_t
{
  // Buffs
  struct buffs_t
  {
    buff_t* concentration;
    buff_t* psychic_projection;
    buff_t* tidal_force;
    buff_t* telekinetic_effusion;
    buff_t* tremors;
    buff_t* presence_of_mind;
    buff_t* force_suppression;
    buff_t* mental_alacrity;
    buff_t* force_potency;
    buff_t* psychic_projection_dd;
    buff_t* rakata_force_masters_4pc;
    buff_t* noble_sacrifice;
    buff_t* resplendence;
    buff_t* conveyance;
  } buffs;

  // Gains
  struct gains_t
  {
    gain_t* concentration;
    gain_t* focused_insight;
    gain_t* psychic_barrier;
    gain_t* noble_sacrifice_power;
    gain_t* noble_sacrifice_health;
    gain_t* noble_sacrifice_power_regen_lost;
  } gains;

  // Procs
  struct procs_t
  {
    //proc_t* procs_<procname>;
  } procs;

  // rngs
  struct rngs_t
  {
    rng_t* psychic_barrier;
    rng_t* upheaval;
    rng_t* tm;
    rng_t* psychic_projection_dd;
  } rngs;

  struct benefits_t
  {
    benefit_t* turbulence;
    benefit_t* fs_weaken_mind;
    benefit_t* fs_mind_crush;
    benefit_t* fs_sever_force;
  } benefits;

  struct cooldowns_t
  {
    cooldown_t* telekinetic_wave;
  } cooldowns;

  double psychic_projection_dd_chance;

  // Talents
  struct talents_t
  {
    // Seer|Corruption
    talent_t* immutable_force;
    talent_t* penetrating_light;
    talent_t* wisdom;
    talent_t* foresight;
    talent_t* pain_bearer;
    talent_t* psychic_suffusion;
    talent_t* conveyance;
    talent_t* rejuvenate;
    talent_t* valiance;
    talent_t* preservation;
    talent_t* mend_wounds;
    talent_t* force_shelter;
    talent_t* egress;
    talent_t* confound;
    talent_t* healing_trance;
    talent_t* serenity;
    talent_t* resplendence;
    talent_t* clairvoyance;
    talent_t* salvation;

    // Telekinetics|Lightning
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

    // Balance|Madness
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

  } talents;

  sage_sorcerer_t( sim_t* sim, player_type pt, const std::string& name, race_type r = RACE_NONE ) :
    player_t( sim, pt == SITH_SORCERER ? SITH_SORCERER : JEDI_SAGE, name, ( r == RACE_NONE ) ? RACE_HUMAN : r ),
    psychic_projection_dd_chance( 1.0 )
  {
    if ( pt == SITH_SORCERER )
    {
      tree_type[ SITH_SORCERER_CORRUPTION ] = TREE_CORRUPTION;
      tree_type[ SITH_SORCERER_LIGHTNING ]  = TREE_LIGHTNING;
      tree_type[ SITH_SORCERER_MADNESS ]    = TREE_MADNESS;
      cooldowns.telekinetic_wave = get_cooldown( "chain_lightning" );
    }
    else
    {
      tree_type[ JEDI_SAGE_SEER ]         = TREE_SEER;
      tree_type[ JEDI_SAGE_TELEKINETICS ] = TREE_TELEKINETICS;
      tree_type[ JEDI_SAGE_BALANCE ]      = TREE_BALANCE;
      cooldowns.telekinetic_wave = get_cooldown( "telekinetic_wave" );
    }

    primary_attribute   = ATTR_WILLPOWER;
    secondary_attribute = ATTR_STRENGTH;

    create_talents();
    create_options();
  }

  // Character Definition
  virtual targetdata_t* new_targetdata( player_t& target ) // override
  { return new sage_sorcerer_targetdata_t( *this, target ); }

  virtual void      init_talents();
  virtual void      init_base();
  virtual void      init_benefits();
  virtual void      init_buffs();
  virtual void      init_gains();
  virtual void      init_procs();
  virtual void      init_rng();
  virtual void      init_actions();

  virtual void      init_scaling()
  {
    player_t::init_scaling();

    scales_with[ STAT_STRENGTH    ] = false;
    scales_with[ STAT_FORCE_POWER ] = true;
  }

  virtual int       primary_resource() const;
  virtual int       primary_role() const;

  virtual void      regen( timespan_t periodicity );

  virtual double    force_bonus_multiplier() const;
  virtual double    force_healing_bonus_multiplier() const;
  virtual double    alacrity() const;

  virtual double default_accuracy_chance() const
  { return player_t::default_accuracy_chance() + talents.clairvoyance -> rank() * 0.01; }

  virtual double force_crit_chance() const
  { return player_t::force_crit_chance() + talents.penetrating_light -> rank() * 0.01; }

  virtual double force_healing_crit_chance() const
  { return player_t::force_healing_crit_chance() + talents.penetrating_light -> rank() * 0.01 + talents.serenity -> rank() * 0.01; }

  virtual double    composite_player_heal_multiplier( const school_type school ) const
  { return player_t::composite_player_heal_multiplier( school ) + talents.wisdom -> rank() * 0.01; }

  double school_damage_reduction( school_type school ) const
  { return player_t::school_damage_reduction( school ) + talents.serenity -> rank() * 0.01; }

  player_t::heal_info_t assess_heal( double amount,  const school_type school,int  dmg_type, int result, action_t* action )
  {
    amount *= 1.0 + talents.pain_bearer -> rank() * 0.04;

    return player_t::assess_heal( amount, school, dmg_type, result, action );
  }

  virtual action_t* create_action( const std::string& name, const std::string& options );
  virtual void      create_talents();
  virtual void      create_options();

  void trigger_tidal_force( double pc )
  {
    if ( talents.tidal_force -> rank() && buffs.tidal_force -> trigger( 1, 0, pc ) )
    {
      cooldowns.telekinetic_wave -> reset();
    }
  }

  virtual bool report_attack_type( action_t::policy_t policy )
  {
    return policy == action_t::melee_policy ||
           policy == action_t::force_policy ||
           ( primary_role() == ROLE_HEAL && policy == action_t::force_heal_policy );
  }
};

namespace { // ANONYMOUS NAMESPACE ==========================================

class sage_sorcerer_action_t : public action_t
{
public:
  sage_sorcerer_action_t( const std::string& n, sage_sorcerer_t* player,
                          attack_policy_t policy, resource_type r, school_type s ) :
    action_t( ACTION_ATTACK, n.c_str(), player, policy, r, s )
  {}

  sage_sorcerer_targetdata_t* targetdata() const
  { return static_cast<sage_sorcerer_targetdata_t*>( action_t::targetdata() ); }

  sage_sorcerer_t* p() const
  { return static_cast<sage_sorcerer_t*>( player ); }

  sage_sorcerer_t* cast() const
  { return p(); }
};

// ==========================================================================
// Jedi Sage Abilities
// ==========================================================================

struct sage_sorcerer_spell_t : public sage_sorcerer_action_t
{
  bool influenced_by_inner_strength;

  sage_sorcerer_spell_t( const std::string& n, sage_sorcerer_t* p, school_type s=SCHOOL_KINETIC ) :
    sage_sorcerer_action_t( n, p, force_policy, RESOURCE_FORCE, s ),
    influenced_by_inner_strength( true )
  {
    may_crit   = true;
    tick_may_crit = true;
  }

  virtual void init()
  {
    action_t::init();

    sage_sorcerer_t* p = cast();

    if ( td.base_min > 0 && ! channeled )
      crit_bonus += p -> talents.mental_scarring -> rank() * 0.1;
  }

  virtual timespan_t execute_time() const
  {
    timespan_t et = action_t::execute_time();

    sage_sorcerer_t* p = cast();

    if ( base_execute_time > timespan_t::zero && p -> buffs.presence_of_mind -> up() )
      et = timespan_t::zero;

    return et;
  }

  virtual void execute()
  {
    action_t::execute();

    sage_sorcerer_t* p = cast();

    if ( base_execute_time > timespan_t::zero )
      p -> buffs.presence_of_mind -> expire();

    if ( dd.base_min > 0 || channeled )
      p -> buffs.force_potency -> decrement();
  }

  virtual void player_buff()
  {
    action_t::player_buff();

    sage_sorcerer_t* p = cast();

    if ( base_execute_time > timespan_t::zero && p -> buffs.presence_of_mind -> up() )
      dd.player_multiplier *= 1.20;

    if ( ( dd.base_min > 0 || channeled ) && p -> buffs.force_potency -> up() )
      player_crit += 0.60;
  }

  virtual void target_debuff( player_t* t, int dmg_type )
  {
    action_t::target_debuff( t, dmg_type );

    sage_sorcerer_t* p = cast();

    // This method is in target_debuff so that it is checked before every dot tick

    // Assume channeled spells don't profit
    if ( p -> talents.force_suppression -> rank() > 0 && td.base_min > 0 && !channeled )
      if ( p -> buffs.force_suppression -> up() )
        td.target_multiplier *= 1.20;
  }

  virtual void assess_damage( player_t* t, double dmg_amount, int dmg_type, int dmg_result )
  {
    action_t::assess_damage( t, dmg_amount, dmg_type, dmg_result );

    sage_sorcerer_t* p = cast();

    // Procs from all critical damage including dot ticks. Source:  17/01/2012 http://sithwarrior.com/forums/Thread-Sorcerer-Sage-Mechanics-and-Quirks

    if ( dmg_result == RESULT_CRIT && p -> talents.telekinetic_effusion -> rank() > 0 )
    {
      p -> buffs.telekinetic_effusion -> trigger( 2 );
    }
  }

  virtual double cost() const
  {
    double c = action_t::cost();

    sage_sorcerer_t* p = cast();

    if ( p -> talents.inner_strength -> rank() > 0 && influenced_by_inner_strength )
    {
      c *= 1.0 - p -> talents.inner_strength -> rank() * 0.03;
      c = ceil( c ); // 17/01/2012 According to http://sithwarrior.com/forums/Thread-Sorcerer-Sage-Mechanics-and-Quirks
    }

    if ( p -> buffs.telekinetic_effusion -> check() > 0 )
    {
      c *= 0.5;
      c = ceil( c ); // FIXME: floor or ceil?
    }

    return c;
  }

  virtual void consume_resource()
  {
    action_t::consume_resource();

    sage_sorcerer_t* p = cast();

    p -> buffs.telekinetic_effusion -> up();
  }

  virtual void tick( dot_t* d )
  {
    action_t::tick( d );

    sage_sorcerer_t* p = cast();

    if ( tick_dmg > 0 && !channeled )
    {
      p -> buffs.force_suppression -> decrement();

      if ( p -> talents.focused_insight -> rank() && result == RESULT_CRIT )
      {
        double hp = p -> resource_max[ RESOURCE_HEALTH ] * p -> talents.focused_insight -> rank() * 0.005;
        p -> resource_gain( RESOURCE_HEALTH, hp, p -> gains.focused_insight );
      }
    }
  }
};

struct force_valor_t : public sage_sorcerer_spell_t
{
  force_valor_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_spell_t( n, p )
  {
    parse_options( 0, options_str );
    base_cost = 0.0;
    harmful = false;
  }

  virtual void execute()
  {
    sage_sorcerer_spell_t::execute();

    for ( player_t* p = sim -> player_list; p; p = p -> next )
    {
      if ( p -> ooc_buffs() )
        p -> buffs.force_valor -> trigger();
    }
  }

  virtual bool ready()
  {
    if ( player -> buffs.force_valor -> check() )
      return false;

    return sage_sorcerer_spell_t::ready();
  }
};

struct noble_sacrifice_t : public sage_sorcerer_spell_t
{
  noble_sacrifice_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_spell_t( n, p )
  {
    parse_options( 0, options_str );
    base_cost = 0.0;
    harmful = false;
  }

  virtual void execute()
  {
    sage_sorcerer_spell_t::execute();

    sage_sorcerer_t* p = cast();

    double health_loss = p -> buffs.resplendence -> up() ? 0 : p -> resource_max[ RESOURCE_HEALTH ] * ( 0.15 - p -> talents.valiance -> rank() * 0.01 );
    p -> resource_loss( RESOURCE_HEALTH, health_loss, p -> gains.noble_sacrifice_health );
    p -> resource_gain( RESOURCE_FORCE, p -> resource_max[ RESOURCE_FORCE ] * 0.08 , p -> gains.noble_sacrifice_power );

    if ( ! p -> buffs.resplendence -> check() )
      p -> buffs.noble_sacrifice -> trigger();
  }
};

struct project_t : public sage_sorcerer_spell_t
{
  project_t* upheaval;

  project_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str, bool is_upheaval = false ) :
    sage_sorcerer_spell_t( n + std::string( is_upheaval ? "_upheaval" : "" ), p ),
    upheaval( 0 )
  {
    rank_level_list = { 1, 4, 7, 11, 14, 17, 23, 34, 47, 50 };

    parse_options( 0, options_str );

    range = 30.0;

    if ( p -> type == JEDI_SAGE )
      travel_speed = 40.0; // 0.5s travel time for range=20, 0.75s for range=30

    if ( is_upheaval )
    {
      dd.standardhealthpercentmin = 0.058;
      dd.standardhealthpercentmax = 0.098;
      dd.power_mod = 0.78;
      background = true;
    }
    else
    {
      dd.standardhealthpercentmin = 0.136;
      dd.standardhealthpercentmax = 0.176;
      dd.power_mod = 1.56;

      base_cost = 45.0;

      cooldown -> duration = timespan_t::from_seconds( 6.0 );

      if ( p -> talents.upheaval -> rank() > 0 )
      {
        upheaval = new project_t( p, n, options_str, true );
        add_child( upheaval );
      }
    }
  }

  virtual void execute()
  {
    sage_sorcerer_spell_t::execute();

    if ( upheaval )
    {
      sage_sorcerer_t* p = cast();

      if ( p -> rngs.upheaval -> roll( p -> talents.upheaval -> rank() * 0.15 ) )
        upheaval -> execute();
    }
  }
};

struct telekinetic_throw_t : public sage_sorcerer_spell_t
{
  bool is_buffed_by_psychic_projection;

  telekinetic_throw_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_spell_t( n, p ),
    is_buffed_by_psychic_projection( false )
  {
    rank_level_list = { 2, 5, 8, 11, 14, 19, 27, 39, 50 };

    parse_options( 0, options_str );

    id = p -> type == SITH_SORCERER ? 188186 : 189258;

    td.standardhealthpercentmin = td.standardhealthpercentmax = .079;
    td.power_mod = 0.79;

    base_cost = 30.0;
    if ( player -> set_bonus.rakata_force_masters -> two_pc() )
      base_cost -= 2.0;

    range = 30.0;
    num_ticks = 3;
    base_tick_time = timespan_t::from_seconds( 1.0 );
    may_crit = false;
    channeled = true;
    tick_zero = true;

    if ( p -> talents.telekinetic_balance -> rank() > 0 )
      cooldown -> duration = timespan_t::zero;
    else
      cooldown -> duration = timespan_t::from_seconds( 6.0 );

    base_crit += p -> talents.critical_kinesis -> rank() * 0.05;

    base_multiplier *= 1.0 + p -> talents.empowered_throw -> rank() * 0.04;
  }

  virtual void execute()
  {
    sage_sorcerer_t* p = cast();

    if ( p -> buffs.psychic_projection -> up()
         || ( p -> buffs.psychic_projection_dd -> up() && p -> rngs.psychic_projection_dd -> roll ( p -> psychic_projection_dd_chance ) )
         )
    {
      is_buffed_by_psychic_projection = true;
      if ( p -> bugs && p -> buffs.psychic_projection -> check() )
      {
        p -> buffs.psychic_projection_dd -> trigger();
      }
    }
    else
      is_buffed_by_psychic_projection = false;

    sage_sorcerer_spell_t::execute();
  }

  virtual timespan_t tick_time() const
  {
    timespan_t tt = sage_sorcerer_spell_t::tick_time();

    if ( is_buffed_by_psychic_projection )
      tt *= 0.5;

    return tt;
  }

  virtual void last_tick( dot_t* d )
  {
    sage_sorcerer_spell_t::last_tick( d );

    sage_sorcerer_t* p = cast();

    if ( is_buffed_by_psychic_projection )
      p -> buffs.psychic_projection -> expire();
  }

  virtual void tick( dot_t* d )
  {
    sage_sorcerer_spell_t::tick( d );

    if ( tick_dmg > 0 )
    {
      sage_sorcerer_t* p = cast();

      if ( p -> talents.psychic_barrier -> rank() > 0 && p -> rngs.psychic_barrier -> roll( p -> talents.psychic_barrier -> rank() * ( 1 / 3.0 ) ) )
      {
        double f = p -> resource_max[ RESOURCE_FORCE ] * 0.01;
        p -> resource_gain( RESOURCE_FORCE, f , p -> gains.psychic_barrier );
      }
      p -> buffs.presence_of_mind -> trigger();
    }
  }
};

struct disturbance_t : public sage_sorcerer_spell_t
{
  sage_sorcerer_spell_t* tm;

  disturbance_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str, bool is_tm = false ) :
    sage_sorcerer_spell_t( n + std::string( is_tm ? "_tm" : "" ), p ),
    tm( 0 )
  {
    rank_level_list = { 10, 13, 16, 25, 36, 45, 50 };

    parse_options( 0, options_str );

    dd.standardhealthpercentmin = .112;
    dd.standardhealthpercentmax = .152;
    dd.power_mod = 1.32;

    base_execute_time = timespan_t::from_seconds( 1.5 );

    base_cost = 30.0;
    if ( player -> set_bonus.rakata_force_masters -> two_pc() )
      base_cost -= 2.0;
    range = 30.0;

    base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02;

    base_crit += p -> talents.critical_kinesis -> rank() * 0.05;

    if ( is_tm )
    {
      dd.standardhealthpercentmin = .03;
      dd.standardhealthpercentmax = .05;
      dd.power_mod = 0.4;
      base_cost = 0.0;
      background = true;
    }
    else if ( p -> talents.telekinetic_momentum -> rank() > 0 )
    {
      tm = new disturbance_t( p, n, options_str, true );
      add_child( tm );
    }
  }

  virtual void impact( player_t* t, int impact_result, double travel_dmg )
  {
    sage_sorcerer_spell_t::impact( t, impact_result, travel_dmg );

    if ( result_is_hit( impact_result ) )
    {
      sage_sorcerer_t* p = cast();

      p -> buffs.concentration -> trigger();

      // Does the TM version also proc Tidal Force?
      p -> trigger_tidal_force( 0.3 );
    }
  }

  virtual void execute()
  {
    sage_sorcerer_spell_t::execute();

    if ( tm )
    {
      sage_sorcerer_t* p = cast();

      if ( p -> rngs.tm -> roll( p -> talents.telekinetic_momentum -> rank() * 0.10 ) )
      {
        tm -> execute();
        p -> buffs.tremors -> trigger( 1 );
      }
    }
  }
};

struct mind_crush_t : public sage_sorcerer_spell_t
{
  struct mind_crush_dot_t : public sage_sorcerer_spell_t
  {
    mind_crush_dot_t( sage_sorcerer_t* p, const std::string& n ) :
      sage_sorcerer_spell_t( n, p )
    {
      rank_level_list = { 14, 19, 30, 41, 50 };

      td.standardhealthpercentmin = td.standardhealthpercentmax = .0295;
      td.power_mod = 0.295;

      base_tick_time = timespan_t::from_seconds( 1.0 );
      num_ticks = 6 + p -> talents.assertion -> rank() * 1;
      range = 30.0;
      influenced_by_inner_strength = false;
      background = true;
      may_crit = false;

      base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02;
    }

    virtual void target_debuff( player_t* t, int dmg_type )
    {
      sage_sorcerer_spell_t::target_debuff( t, dmg_type );

      sage_sorcerer_t* p = cast();

      if ( p -> talents.force_suppression -> rank() > 0 )
        p -> benefits.fs_mind_crush -> update( p -> buffs.force_suppression -> check() > 0 );
    }
  };

  mind_crush_dot_t* dot_spell;

  mind_crush_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_spell_t( n, p ),
    dot_spell( new mind_crush_dot_t( p, n + "_dot" ) )
  {
    static const int ranks[] = { 14, 19, 30, 41, 50 };
    rank_level_list = util_t::array_to_vector( ranks );

    parse_options( 0, options_str );

    dd.standardhealthpercentmin = .103;
    dd.standardhealthpercentmax = .143;
    dd.power_mod = 1.23;

    base_execute_time = timespan_t::from_seconds( 2.0 );
    base_cost = 40.0;
    range = 30.0;
    cooldown -> duration = timespan_t::from_seconds( 15.0 );
    if ( player -> set_bonus.battlemaster_force_masters -> two_pc() )
      cooldown -> duration -= timespan_t::from_seconds( 1.5 );
    influenced_by_inner_strength = false;

    base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02;

    add_child( dot_spell );
  }

  virtual void execute()
  {
    sage_sorcerer_spell_t::execute();
    dot_spell -> execute();
  }
};

struct weaken_mind_t : public sage_sorcerer_spell_t
{
  weaken_mind_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_spell_t( n, p, SCHOOL_INTERNAL )
  {
    rank_level_list = { 16, 22, 33, 44, 50 };

    parse_options( 0, options_str );

    td.standardhealthpercentmin = td.standardhealthpercentmax = .031;
    td.power_mod = 0.31;

    base_tick_time = timespan_t::from_seconds( 3.0 );
    num_ticks = 5 + p -> talents.disturb_mind -> rank();
    base_cost = 35.0;
    range = 30.0;
    may_crit = false;
    crit_bonus += p -> talents.reverberation -> rank() * 0.1;
    base_multiplier *= 1.0 + p -> talents.drain_thoughts -> rank() * 0.075;
    influenced_by_inner_strength = false;
  }

  virtual void tick( dot_t* d )
  {
    sage_sorcerer_spell_t::tick( d );

    sage_sorcerer_t* p = cast();

    if ( result == RESULT_CRIT && p -> talents.psychic_projection -> rank() > 0 )
    {
      p -> buffs.psychic_projection -> trigger( p -> buffs.psychic_projection -> max_stack );
    }

    p -> buffs.rakata_force_masters_4pc -> trigger();
  }

  virtual void target_debuff( player_t* t, int dmg_type )
  {
    sage_sorcerer_spell_t::target_debuff( t, dmg_type );

    sage_sorcerer_t* p = cast();

    if ( p -> talents.force_suppression -> rank() > 0 )
      p -> benefits.fs_weaken_mind -> update( p -> buffs.force_suppression -> check() > 0 );
  }
};

struct turbulence_t : public sage_sorcerer_spell_t
{
  sage_sorcerer_spell_t* tm;

  turbulence_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_spell_t( n, p, SCHOOL_INTERNAL )
  {
    check_talent( p -> talents.turbulence -> rank() );

    parse_options( 0, options_str );

    dd.standardhealthpercentmin = .138;
    dd.standardhealthpercentmax = .178;
    dd.power_mod = 1.58;

    base_execute_time = timespan_t::from_seconds( 2.0 );
    base_cost = 45.0;
    range = 30.0;
    crit_bonus += p -> talents.reverberation -> rank() * 0.1;

    base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02;
    cooldown -> duration = timespan_t::from_seconds( 9.0 );
  }

  virtual void target_debuff( player_t *t, int dmg_type )
  {
    sage_sorcerer_spell_t::target_debuff( t, dmg_type );

    assert( t == target );
    sage_sorcerer_targetdata_t& td = dynamic_cast<sage_sorcerer_targetdata_t&>( *targetdata() );

    bool increase_crit_chance = td.dot_weaken_mind.ticking != 0;

    cast() -> benefits.turbulence -> update( increase_crit_chance );

    if ( increase_crit_chance )
      target_crit += 1.0;
  }
};

struct force_in_balance_t : public sage_sorcerer_spell_t
{
  force_in_balance_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_spell_t( n, p, SCHOOL_INTERNAL )
  {
    check_talent( p -> talents.force_in_balance -> rank() );

    parse_options( 0, options_str );

    dd.standardhealthpercentmin = .167;
    dd.standardhealthpercentmax = .207;
    dd.power_mod = 1.87;

    ability_lag = timespan_t::from_seconds( 0.2 );
    base_cost = 50.0;
    range = 30.0;
    aoe = 2;

    cooldown -> duration = timespan_t::from_seconds( 15.0 );

    crit_bonus += p -> talents.mental_scarring -> rank() * 0.1;
    base_multiplier *= 1.0 + p -> talents.psychic_suffusion -> rank() * 0.05;
  }

  virtual void execute()
  {
    sage_sorcerer_spell_t::execute();

    sage_sorcerer_t* p = cast();

    // ToDo: Move buff to targetdata and buff trigger to impact

    p -> buffs.force_suppression -> trigger( 10 );
  }
};

struct sever_force_t : public sage_sorcerer_spell_t
{
  sever_force_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_spell_t( n, p, SCHOOL_INTERNAL )
  {
    check_talent( p -> talents.sever_force -> rank() );

    parse_options( 0, options_str );

    td.standardhealthpercentmin = td.standardhealthpercentmax = .031;
    td.power_mod = 0.311;

    base_tick_time = timespan_t::from_seconds( 3.0 );
    num_ticks = 6;
    base_cost = 20.0;
    range = 30.0;
    may_crit = false;
    cooldown -> duration = timespan_t::from_seconds( 9.0 );
    tick_zero = true;
    influenced_by_inner_strength = false;
  }

  virtual void target_debuff( player_t* t, int dmg_type )
  {
    sage_sorcerer_spell_t::target_debuff( t, dmg_type );

    sage_sorcerer_t* p = cast();

    if ( p -> talents.force_suppression -> rank() > 0 )
      p -> benefits.fs_sever_force -> update( p -> buffs.force_suppression -> check() > 0 );
  }
};

struct mental_alacrity_t : public sage_sorcerer_spell_t
{
  mental_alacrity_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_spell_t( n, p )
  {
    check_talent( p -> talents.mental_alacrity -> rank() );

    parse_options( 0, options_str );
    cooldown -> duration = timespan_t::from_seconds( 120.0 );
    harmful = false;

    // TODO: Does it trigger a gcd?
  }

  virtual void execute()
  {
    sage_sorcerer_spell_t::execute();

    sage_sorcerer_t* p = cast();

    p -> buffs.mental_alacrity -> trigger();
  }
};

struct telekinetic_wave_t : public sage_sorcerer_spell_t
{
  sage_sorcerer_spell_t* tm;

  telekinetic_wave_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str, bool is_tm = false ) :
    sage_sorcerer_spell_t( n + std::string( is_tm ? "_tm" : "" ), p ),
    tm( 0 )
  {
    check_talent( p -> talents.telekinetic_wave -> rank() );

    parse_options( 0, options_str );

    if ( is_tm )
    {
      dd.standardhealthpercentmin = .041;
      dd.standardhealthpercentmax = .081;
      dd.power_mod = .61;

      base_cost = 0.0;
      background = true;
    }
    else
    {
      dd.standardhealthpercentmin = .182;
      dd.standardhealthpercentmax = .222;
      dd.power_mod = 2.02;

      base_cost = 50.0;
      cooldown -> duration = timespan_t::from_seconds( 6.0 );
      base_execute_time = timespan_t::from_seconds( 3.0 );

      if ( p -> talents.telekinetic_momentum -> rank() > 0 )
      {
        tm = new telekinetic_wave_t( p, n, options_str, true );
        add_child( tm );
      }
    }

    range = 30.0;
    aoe = 4;

    base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02 + p -> talents.psychic_suffusion -> rank() * 0.05;
    crit_bonus += p -> talents.reverberation -> rank() * 0.1;
  }

  virtual void execute()
  {
    sage_sorcerer_spell_t::execute();

    if ( tm )
    {
      sage_sorcerer_t* p = cast();

      if ( p -> rngs.tm -> roll( p -> talents.telekinetic_momentum -> rank() * 0.10 ) )
      {
        tm -> execute();
        p -> buffs.tremors -> trigger( 1 );
      }
    }
  }

  virtual timespan_t execute_time() const
  {
    timespan_t et = sage_sorcerer_spell_t::execute_time();

    sage_sorcerer_t* p = cast();

    if ( p -> buffs.tidal_force -> up() )
      et = timespan_t::zero;

    return et;
  }

  virtual void update_ready()
  {
    sage_sorcerer_spell_t::update_ready();

    sage_sorcerer_t* p = cast();

    p -> buffs.tidal_force -> expire();
  }
};

struct force_potency_t : public sage_sorcerer_spell_t
{
  force_potency_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_spell_t( n, p )
  {
    parse_options( 0, options_str );
    cooldown -> duration = timespan_t::from_seconds( 90.0 );
    harmful = false;

    trigger_gcd = timespan_t::zero;
  }

  virtual void execute()
  {
    sage_sorcerer_spell_t::execute();

    sage_sorcerer_t* p = cast();

    p -> buffs.force_potency -> trigger( 2 );
  }
};


struct sage_sorcerer_heal_t : public heal_t
{
  bool influenced_by_inner_strength;

  sage_sorcerer_heal_t( const std::string& n, sage_sorcerer_t* p, int r=RESOURCE_NONE, const school_type s=SCHOOL_KINETIC ) :
    heal_t( n.c_str(), p, force_heal_policy, r, s ),
    influenced_by_inner_strength( true )
  {
    may_crit   = true;
    tick_may_crit = true;
  }

  sage_sorcerer_targetdata_t* targetdata() const
  { return static_cast<sage_sorcerer_targetdata_t*>( action_t::targetdata() ); }

  sage_sorcerer_t* p() const
  { return static_cast<sage_sorcerer_t*>( player ); }


  virtual void init()
  {
    heal_t::init();

    if ( td.base_min > 0 && !channeled )
      crit_bonus += p() -> talents.mental_scarring -> rank() * 0.1;
  }

  virtual double cost() const
  {
    double c = heal_t::cost();

    if ( p() -> talents.inner_strength -> rank() > 0 && influenced_by_inner_strength )
    {
      c *= 1.0 - p() -> talents.inner_strength -> rank() * 0.03;
      c = ceil( c ); // 17/01/2012 According to http://sithwarrior.com/forums/Thread-Sorcerer-Sage-Mechanics-and-Quirks
    }

    if ( p() -> buffs.telekinetic_effusion -> check() > 0 )
    {
      c *= 0.5;
      c = floor( c ); // FIXME: floor or ceil?
    }

    return c;
  }

  virtual void consume_resource()
  {
    heal_t::consume_resource();

    p() -> buffs.telekinetic_effusion -> up();
  }
};

struct deliverance_t : public sage_sorcerer_heal_t
{
  deliverance_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_heal_t( n, p, RESOURCE_FORCE, SCHOOL_INTERNAL )
  {
    parse_options( 0, options_str );

    dd.standardhealthpercentmin = .16;
    dd.standardhealthpercentmax = .18;
    dd.power_mod = 3.41;

    base_cost = 55.0;
    base_execute_time = timespan_t::from_seconds( 3.0 );
    base_execute_time -= timespan_t::from_seconds( p -> talents.immutable_force -> rank() * 0.25 );

    range = 30.0;
  }

  virtual timespan_t execution_time()
  {
    timespan_t et = sage_sorcerer_heal_t::execute_time();

    // FIXME: check if -1.0s becomes before or after Alacrity
    if ( p() -> buffs.conveyance -> up() )
      et -= timespan_t::from_seconds( 1.0 );

    return et;
  }

  virtual void execute()
  {
    sage_sorcerer_heal_t::execute();

    p() -> buffs.conveyance -> expire();
  }
};

struct benevolence_t : public sage_sorcerer_heal_t
{
  benevolence_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_heal_t( n, p, RESOURCE_FORCE, SCHOOL_INTERNAL )
  {
    parse_options( 0, options_str );

    dd.standardhealthpercentmin = .078;
    dd.standardhealthpercentmax = .098;
    dd.power_mod = 1.75;

    base_cost = 50.0;
    base_execute_time = timespan_t::from_seconds( 1.5 );

    range = 30.0;
  }

  virtual double cost() const
  {
    double c = sage_sorcerer_heal_t::cost();

    if ( p() -> buffs.conveyance -> check() )
    {
      c *= 0.5;
      c = ceil( c );
    }

    return c;
  }

  virtual void consume_resource()
  {
    sage_sorcerer_heal_t::consume_resource();

    p() -> buffs.conveyance -> up(); // uptime tracking
  }

  virtual void execute()
  {
    sage_sorcerer_heal_t::execute();

    p() -> buffs.conveyance -> expire();
  }
};

struct healing_trance_t : public sage_sorcerer_heal_t
{
  healing_trance_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_heal_t( n, p, RESOURCE_FORCE, SCHOOL_INTERNAL )
  {
    parse_options( 0, options_str );

    dd.standardhealthpercentmin = dd.standardhealthpercentmax = .0515;
    dd.power_mod = 1.03;

    td.standardhealthpercentmin = td.standardhealthpercentmax = .0515;
    td.power_mod = 1.03;

    base_cost = 40.0;
    base_execute_time = timespan_t::from_seconds( 3.0 );
    cooldown -> duration = timespan_t::from_seconds( 9.0 );

    range = 30.0;
  }

  virtual void impact( player_t* t, int impact_result, double travel_dmg )
  {
    sage_sorcerer_heal_t::impact( t, impact_result, travel_dmg );

    if ( impact_result == RESULT_CRIT )
    {
      p() -> buffs.resplendence -> trigger();
    }
  }

  virtual void player_buff()
  {
    sage_sorcerer_heal_t::player_buff();

    if ( p() -> buffs.conveyance -> up() )
      player_crit += 0.25;
  }

  virtual void execute()
  {
    sage_sorcerer_heal_t::execute();

    p() -> buffs.conveyance -> expire();
  }
};

struct rejuvenate_t : public sage_sorcerer_heal_t
{
  rejuvenate_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_heal_t( n, p, RESOURCE_FORCE, SCHOOL_INTERNAL )
  {
    parse_options( 0, options_str );

    dd.standardhealthpercentmin = .035;
    dd.standardhealthpercentmax = .055;
    dd.power_mod = 0.91;

    td.standardhealthpercentmin = td.standardhealthpercentmax = .016;
    td.power_mod = 0.33;

    base_cost = 30.0;
    cooldown -> duration = timespan_t::from_seconds( 6.0 );

    num_ticks = 3 + p -> talents.force_shelter -> rank();
    base_tick_time = timespan_t::from_seconds( 3.0 );

    range = 30.0;
  }

  virtual void execute()
  {
    sage_sorcerer_heal_t::execute();

    // FIXME: check assumption
    // Assuming conveyance is only trigger once on rejuvenate execution, not on tick

    p() -> buffs.conveyance -> trigger();
  }
};

struct salvation_t : public sage_sorcerer_heal_t
{
  struct salvation_tick_spell_t : public sage_sorcerer_heal_t
  {
    salvation_tick_spell_t( sage_sorcerer_t* p, const std::string& n ) :
      sage_sorcerer_heal_t( n + "tick_spell", p, RESOURCE_FORCE, SCHOOL_INTERNAL )
    {
      dd.standardhealthpercentmin = dd.standardhealthpercentmax = .019;
      dd.power_mod = 0.376;

      background = true;
      dual        = true;
      direct_tick = true;

      range = 30.0;

      base_multiplier *= 1.0 + p -> talents.psychic_suffusion -> rank() * 0.05;

      stats = player -> get_stats( n.c_str(), this );
    }
  };

  salvation_tick_spell_t* tick_spell;

  salvation_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_heal_t( n, p, RESOURCE_FORCE, SCHOOL_INTERNAL ),
    tick_spell( 0 )
  {
    parse_options( 0, options_str );
    harmful = false;

    num_ticks = 10;
    tick_zero = true;
    base_tick_time = timespan_t::from_seconds( 1.0 );

    base_cost = 100.0;
    base_execute_time = timespan_t::from_seconds( 2.0 );
    cooldown -> duration = timespan_t::from_seconds( 15.0 );

    range = 30.0;

    tick_spell = new salvation_tick_spell_t( p, n );
  }

  virtual void tick( dot_t* d )
  {
    sage_sorcerer_heal_t::tick( d );

    tick_spell -> execute();
  }

  virtual double cost() const
  {
    double c = sage_sorcerer_heal_t::cost();

    if ( p() -> buffs.conveyance -> check() )
    {
      c *= 0.7;
      c = ceil( c );
    }

    return c;
  }

  virtual void consume_resource()
  {
    sage_sorcerer_heal_t::consume_resource();

    p() -> buffs.conveyance -> up(); // uptime tracking
  }

  virtual void execute()
  {
    sage_sorcerer_heal_t::execute();

    p() -> buffs.conveyance -> expire();
  }
};

struct sage_sorcerer_absorb_t : public absorb_t
{
  sage_sorcerer_absorb_t( const std::string& n, sage_sorcerer_t* p, int r=RESOURCE_NONE, const school_type s=SCHOOL_KINETIC ) :
    absorb_t( n.c_str(), p, force_heal_policy, r, s )
  {
  }

  sage_sorcerer_targetdata_t* targetdata() const
  { return static_cast<sage_sorcerer_targetdata_t*>( action_t::targetdata() ); }

  sage_sorcerer_t* p() const
  { return static_cast<sage_sorcerer_t*>( player ); }


  // FIXME: check
  virtual void consume_resource()
  {
    absorb_t::consume_resource();

    p() -> buffs.telekinetic_effusion -> up();
  }
};

struct force_armor_t : public sage_sorcerer_absorb_t
{
  force_armor_t( sage_sorcerer_t* p, const std::string& n, const std::string& options_str ) :
    sage_sorcerer_absorb_t( n, p, RESOURCE_FORCE, SCHOOL_INTERNAL )
  {
    parse_options( 0, options_str );

    dd.standardhealthpercentmin = .164;
    dd.standardhealthpercentmax = .164;
    dd.power_mod = 3.27;

    base_cost = 65.0;
    base_cost -= p -> talents.preservation -> rank() * 15.0;
    cooldown -> duration = timespan_t::from_seconds( 4.5 );
    cooldown -> duration -= timespan_t::from_seconds( p -> talents.preservation -> rank() * 1.5 );

    range = 30.0;

    base_multiplier *= p -> talents.telekinetic_defense -> rank() * 0.10;
  }

  virtual void execute()
  {
    sage_sorcerer_absorb_t::execute();

    p() -> buffs.conveyance -> expire();
  }
};

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// sage_sorcerer Character Definition
// ==========================================================================

// sage_sorcerer_t::create_action ====================================================

action_t* sage_sorcerer_t::create_action( const std::string& name,
                                 const std::string& options_str )
{
  if ( type == JEDI_SAGE )
  {
    if ( name == "force_valor"        ) return new       force_valor_t( this, "force_valor", options_str );
    if ( name == "project"            ) return new           project_t( this, "project", options_str );
    if ( name == "telekinetic_throw"  ) return new telekinetic_throw_t( this, "telekinetic_throw", options_str );
    if ( name == "disturbance"        ) return new       disturbance_t( this, "disturbance", options_str );
    if ( name == "mind_crush"         ) return new        mind_crush_t( this, "mind_crush", options_str );
    if ( name == "weaken_mind"        ) return new       weaken_mind_t( this, "weaken_mind", options_str );
    if ( name == "turbulence"         ) return new        turbulence_t( this, "turbulence", options_str );
    if ( name == "force_in_balance"   ) return new  force_in_balance_t( this, "force_in_balance", options_str );
    if ( name == "sever_force"        ) return new       sever_force_t( this, "sever_force", options_str );
    if ( name == "mental_alacrity"    ) return new   mental_alacrity_t( this, "mental_alacrity", options_str );
    if ( name == "telekinetic_wave"   ) return new  telekinetic_wave_t( this, "telekinetic_wave", options_str );
    if ( name == "force_potency"      ) return new     force_potency_t( this, "force_potency", options_str );
    if ( name == "noble_sacrifice"    ) return new   noble_sacrifice_t( this, "noble_sacrifice", options_str );

    // Heal
    if ( name == "deliverance"        ) return new       deliverance_t( this, "deliverance", options_str );
    if ( name == "benevolence"        ) return new       benevolence_t( this, "benevolence", options_str );
    if ( name == "healing_trance"     ) return new    healing_trance_t( this, "healing_trance", options_str );
    if ( name == "rejuvenate"         ) return new        rejuvenate_t( this, "rejuvenate", options_str );
    if ( name == "salvation"          ) return new         salvation_t( this, "salvation", options_str );
  }
  else if ( type == SITH_SORCERER )
  {
    if ( name == "mark_of_power"      ) return new       force_valor_t( this, "mark_of_power", options_str );
    if ( name == "shock"              ) return new           project_t( this, "shock", options_str );
    if ( name == "force_lightning"    ) return new telekinetic_throw_t( this, "force_lightning", options_str );
    if ( name == "lightning_strike"   ) return new       disturbance_t( this, "lightning_strike", options_str );
    if ( name == "crushing_darkness"  ) return new        mind_crush_t( this, "crushing_darkness", options_str );
    if ( name == "affliction"         ) return new       weaken_mind_t( this, "affliction", options_str );
    if ( name == "thundering_blast"   ) return new        turbulence_t( this, "thundering_blast", options_str );
    if ( name == "death_field"        ) return new  force_in_balance_t( this, "death_field", options_str );
    if ( name == "creeping_terror"    ) return new       sever_force_t( this, "creeping_terror", options_str );
    if ( name == "polarity_shift"     ) return new   mental_alacrity_t( this, "polarity_shift", options_str );
    if ( name == "chain_lightning"    ) return new  telekinetic_wave_t( this, "chain_lightning", options_str );
    if ( name == "recklessness"       ) return new     force_potency_t( this, "recklessness", options_str );
    if ( name == "dark_infusion"      ) return new       deliverance_t( this, "dark_infusion", options_str );
    if ( name == "consumption"        ) return new   noble_sacrifice_t( this, "consumption", options_str );

    // Heal
    if ( name == "dark_infusion"      ) return new       deliverance_t( this, "dark_infusion", options_str );
    if ( name == "dark_heal"          ) return new       benevolence_t( this, "dark_heal", options_str );
    if ( name == "innervate"          ) return new    healing_trance_t( this, "innervate", options_str );
    if ( name == "resurgence"         ) return new        rejuvenate_t( this, "resurgence", options_str );
    if ( name == "revivification"     ) return new         salvation_t( this, "revivification", options_str );
  }

  return player_t::create_action( name, options_str );
}

// sage_sorcerer_t::init_talents =====================================================

void sage_sorcerer_t::init_talents()
{
  player_t::init_talents();

  // Seer|Corruption
  talents.immutable_force       = find_talent( "Immutable Force" );
  talents.penetrating_light     = find_talent( "Penetrating Light" );
  talents.wisdom                = find_talent( "Wisdom" );
  talents.foresight             = find_talent( "Foresight" );
  talents.pain_bearer           = find_talent( "Pain Bearer" );
  talents.psychic_suffusion     = find_talent( "Psychic Suffusion" );
  talents.conveyance            = find_talent( "Conveyance" );
  talents.rejuvenate            = find_talent( "Rejuvenate" );
  talents.valiance              = find_talent( "Valiance" );
  talents.preservation          = find_talent( "Preservation" ); // add
  talents.mend_wounds           = find_talent( "Mend Wounds" );
  talents.force_shelter         = find_talent( "Force Shelter" ); // add buff
  talents.egress                = find_talent( "Egress" ); // add
  talents.confound              = find_talent( "Confound" ); // add
  talents.healing_trance        = find_talent( "Healing Trance" );
  talents.serenity              = find_talent( "Serenity" );
  talents.resplendence          = find_talent( "Resplendence" );
  talents.clairvoyance          = find_talent( "Clairvoyance" );
  talents.salvation             = find_talent( "Salvation" );

  // Telekinetics|Lightning
  talents.inner_strength        = find_talent( "Inner Strength" );
  talents.mental_longevity      = find_talent( "Mental Longevity" );
  talents.clamoring_force       = find_talent( "Clamoring Force" );
  talents.minds_eye             = find_talent( "Minds Eye" );
  talents.disturb_mind          = find_talent( "Disturb Mind" );
  talents.concentration         = find_talent( "Concentration" );
  talents.telekinetic_defense   = find_talent( "Telekinetic Defense" );
  talents.blockout              = find_talent( "Blockout" );
  talents.telekinetic_wave      = find_talent( "Telekinetic Wave" );
  talents.psychic_projection    = find_talent( "Psychic Projection" );
  talents.force_wake            = find_talent( "Force Wake" );
  talents.tidal_force           = find_talent( "Tidal Force" );
  talents.telekinetic_effusion  = find_talent( "Telekinetic Effusion" );
  talents.kinetic_collapse      = find_talent( "Kinetic Collapse" );
  talents.tremors               = find_talent( "Tremors" );
  talents.telekinetic_momentum  = find_talent( "Telekinetic Momentum" );
  talents.mental_alacrity       = find_talent( "Mental Alacrity" );
  talents.reverberation         = find_talent( "Reverberation" );
  talents.turbulence            = find_talent( "Turbulence" );

  // Balance|Madness
  talents.empowered_throw       = find_talent( "Empowered Throw" );
  talents.jedi_resistance       = find_talent( "Jedi Resistance" );
  talents.will_of_the_jedi      = find_talent( "Will of the Jedi" );
  talents.pinning_resolve       = find_talent( "Pinning Resolve" );
  talents.upheaval              = find_talent( "Upheaval" );
  talents.focused_insight       = find_talent( "Focused Insight" );
  talents.critical_kinesis      = find_talent( "Critical Kinesis" );
  talents.force_in_balance      = find_talent( "Force in Balance" );
  talents.psychic_barrier       = find_talent( "Psychic Barrier" );
  talents.telekinetic_balance   = find_talent( "Telekinetic Balance" );
  talents.containment           = find_talent( "Containment" );
  talents.mind_ward             = find_talent( "Mind Ward" );
  talents.presence_of_mind      = find_talent( "Presence of Mind" );
  talents.force_suppression     = find_talent( "Force Suppression" );
  talents.drain_thoughts        = find_talent( "Drain Thoughts" );
  talents.assertion             = find_talent( "Assertion" );
  talents.mental_scarring       = find_talent( "Mental Scarring" );
  talents.psychic_absorption    = find_talent( "Psychic Absorption" );
  talents.sever_force           = find_talent( "Sever Force" );
}

// sage_sorcerer_t::init_base ========================================================

void sage_sorcerer_t::init_base()
{
  player_t::init_base();

  distance = default_distance = 30;

  base_force_regen_per_second = 8.0;
  resource_base[  RESOURCE_FORCE  ] += 500 + talents.mental_longevity -> rank() * 50;

  attribute_multiplier_initial[ ATTR_WILLPOWER ] += talents.will_of_the_jedi -> rank() * 0.03;
}

// sage_sorcerer_t::init_benefits =======================================================

void sage_sorcerer_t::init_benefits()
{
  player_t::init_benefits();

  if ( type == SITH_SORCERER )
  {
    benefits.turbulence = get_benefit( "Thundering Blast automatic crit" );
    benefits.fs_weaken_mind = get_benefit( "Affliction ticks with Deathmark");
    benefits.fs_mind_crush = get_benefit( "Crushing Darkness ticks with Deathmark");
    benefits.fs_sever_force = get_benefit( "Creeping Terror ticks with Deathmark");
  }
  else
  {
    benefits.turbulence = get_benefit( "Turbulence automatic crit" );
    benefits.fs_weaken_mind = get_benefit( "Weaken Mind ticks with Force Suppression");
    benefits.fs_mind_crush = get_benefit( "Mind Crush ticks with Force Suppression");
    benefits.fs_sever_force = get_benefit( "Sever Force ticks with Force Suppression");
  }
}

// sage_sorcerer_t::init_buffs =======================================================

void sage_sorcerer_t::init_buffs()
{
  player_t::init_buffs();

  // buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rngs.type=rngs.CYCLIC, activated=true )
  // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rngs.type=rngs.CYCLIC, activated=true )
  // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rngs.type=rngs.CYCLIC, activated=true )

  bool is_sage = ( type == JEDI_SAGE );

  buffs.concentration = new buff_t( this, is_sage ? "concentration" : "subversion", 3, timespan_t::from_seconds( 10.0 ), timespan_t::zero, 0.5 * talents.concentration -> rank() );
  buffs.psychic_projection = new buff_t( this, is_sage ? "psychic_projection" : "lightning_barrage", 1, timespan_t::zero, timespan_t::from_seconds( 10.0 ), 0.5 * talents.psychic_projection -> rank() );
  buffs.tidal_force = new buff_t( this, is_sage ? "tidal_force" : "lightning_storm", 1, timespan_t::zero, timespan_t::from_seconds( 10.0 ) );
  buffs.telekinetic_effusion = new buff_t( this, is_sage ? "telekinetic_effusion" : "lightning_effusion", 2, timespan_t::zero, timespan_t::zero, 0.5 * talents.telekinetic_effusion -> rank() );
  buffs.tremors = new buff_t( this, is_sage ? "tremors" : "conduction", 3, timespan_t::from_seconds( 30.0 ) );
  buffs.presence_of_mind = new buff_t( this, is_sage ? "presence_of_mind" : "wrath", 1, timespan_t::zero, timespan_t::zero, talents.presence_of_mind -> rank() * 0.3 );
  buffs.force_suppression = new buff_t( this, is_sage ? "force_suppression" : "deathmark", 10, timespan_t::from_seconds( 30.0 ), timespan_t::zero, talents.force_suppression -> rank() );
  buffs.mental_alacrity = new buff_t( this, is_sage ? "mental_alacrity" : "polarity_shift", 1, timespan_t::from_seconds( 10.0 ) );
  buffs.force_potency = new buff_t( this, is_sage ? "force_potency" : "recklessness", 2, timespan_t::from_seconds( 20.0 ) );
  buffs.psychic_projection_dd = new buff_t( this, is_sage ? "psychic_projection_dd" : "lightning_barrage_dd", 1, timespan_t::from_seconds( 2.0 ), timespan_t::zero );
  buffs.rakata_force_masters_4pc = new buff_t( this, "rakata_force_masters_4pc", 1, timespan_t::from_seconds( 15.0 ), timespan_t::from_seconds( 20.0 ), set_bonus.rakata_force_masters -> four_pc() ? 0.10 : 0.0 );
  buffs.noble_sacrifice = new buff_t( this, "noble_sacrifice", 4, timespan_t::from_seconds( 10.0 ) );
  buffs.resplendence = new buff_t( this, is_sage ? "resplendence" : "force_surge", 1 , timespan_t::zero, timespan_t::zero, talents.resplendence -> rank() / 2.0 );
  buffs.conveyance = new buff_t( this, is_sage ? "conveyace" : "force_bending", 1, timespan_t::zero, timespan_t::zero, talents.conveyance -> rank() / 2.0 );
}

// sage_sorcerer_t::init_gains =======================================================

void sage_sorcerer_t::init_gains()
{
  player_t::init_gains();

  bool is_sage = ( type == JEDI_SAGE );

  gains.concentration   = get_gain( is_sage ? "concentration" : "subversion" );
  gains.focused_insight = get_gain( is_sage ? "focused_insight" : "parasitism" );
  gains.psychic_barrier = get_gain( is_sage ? "psychic_barrier" : "sith_efficacy" );
  gains.noble_sacrifice_health = get_gain( is_sage ? "noble_sacrifice_health" : "consumption_health" );
  gains.noble_sacrifice_power = get_gain( is_sage ? "noble_sacrifice_power" : "consumption_power" );
  gains.noble_sacrifice_power_regen_lost = get_gain( is_sage ? "noble_sacrifice_power_regen_lost" : "consumption_power_regen_lost" );
}

// sage_sorcerer_t::init_procs =======================================================

void sage_sorcerer_t::init_procs()
{
  player_t::init_procs();
}

// sage_sorcerer_t::init_rngs =========================================================

void sage_sorcerer_t::init_rng()
{
  player_t::init_rng();

  rngs.psychic_barrier = get_rng( "psychic_barrier" );
  rngs.upheaval = get_rng( "upheaval" );
  rngs.tm = get_rng( "telekinetic_momentum" );
  rngs.psychic_projection_dd = get_rng( type == JEDI_SAGE ? "psychic_projection_dd" : "lightning_barrage_dd" );
}

// sage_sorcerer_t::init_actions =====================================================

void sage_sorcerer_t::init_actions()
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
      action_list_str += "stim,type=exotech_resolve";
      action_list_str += "/force_valor";
      action_list_str += "/snapshot_stats";

      switch ( primary_tree() )
      {
      case TREE_BALANCE:

      if ( talents.psychic_projection -> rank() )
        action_list_str += "/telekinetic_throw,if=buff.psychic_projection_dd.up";

      action_list_str += "/power_potion";
      action_list_str += "/force_potency";
      action_list_str += "/use_relics";
      action_list_str += "/weaken_mind,if=!ticking";

      if ( talents.force_in_balance -> rank() > 0 )
        action_list_str += "/force_in_balance,if=force>100";

      if ( talents.presence_of_mind -> rank() )
        action_list_str += "/mind_crush,if=buff.presence_of_mind.react";

      if ( talents.telekinetic_wave -> rank() && talents.presence_of_mind -> rank() )
        action_list_str += "/telekinetic_wave,if=buff.presence_of_mind.react&force>76+6*target.health_pct&cooldown.mind_crush.remains>4";

      if ( talents.sever_force -> rank() > 0 )
        action_list_str += "/sever_force,if=!ticking";

      if ( ! talents.presence_of_mind -> rank() )
        action_list_str += "/mind_crush";

      action_list_str += "/telekinetic_throw";
      action_list_str += "/project,moving=1,if=force>120";

      break;


      case TREE_TELEKINETICS:

      action_list_str += "/power_potion";
      action_list_str += "/use_relics";
      action_list_str += "/weaken_mind,if=!ticking";

      if ( talents.turbulence -> rank() > 0 )
        action_list_str += "/turbulence,if=dot.weaken_mind.remains>cast_time";

      action_list_str += "/force_potency,if=buff.psychic_projection.react|buff.tidal_force.react";

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
      action_list_str += "stim,type=exotech_resolve";
      action_list_str += "/mark_of_power";
      action_list_str += "/snapshot_stats";

      switch ( primary_tree() )
      {
      case TREE_MADNESS:

      if ( talents.psychic_projection -> rank() )
        action_list_str += "/force_lightning,if=buff.lightning_barrage_dd.up";

      action_list_str += "/power_potion";
      action_list_str += "/recklessness";
      action_list_str += "/use_relics";
      action_list_str += "/affliction,if=!ticking";

      if ( talents.force_in_balance -> rank() > 0 )
        action_list_str += "/death_field,if=force>100";

      if ( talents.presence_of_mind -> rank() )
        action_list_str += "/crushing_darkness,if=buff.wrath.react";

      if ( talents.telekinetic_wave -> rank() && talents.presence_of_mind -> rank() )
        action_list_str += "/chain_lightning,if=buff.wrath.react&force>76+6*target.health_pct&cooldown.crushing_darkness.remains>4";

      if ( talents.sever_force -> rank() > 0 )
        action_list_str += "/creeping_terror,if=!ticking";

      if ( ! talents.presence_of_mind -> rank() )
        action_list_str += "/crushing_darkness";

      action_list_str += "/force_lightning";
      action_list_str += "/shock,moving=1,if=force>120";

      break;


      case TREE_LIGHTNING:

      action_list_str += "/power_potion";
      action_list_str += "/use_relics";
      action_list_str += "/affliction,if=!ticking";

      if ( talents.turbulence -> rank() > 0 )
        action_list_str += "/thundering_blast,if=dot.affliction.remains>cast_time";

      action_list_str += "/recklessness,if=buff.lightning_barrage.react|buff.lightning_storm.react";

      if ( talents.psychic_projection -> rank() > 0)
        action_list_str += "/force_lightning,if=buff.lightning_barrage.react";

      if ( talents.telekinetic_wave -> rank() > 0 && talents.tidal_force -> rank() > 0 )
        action_list_str += "/chain_lightning,if=buff.lightning_storm.react";

      action_list_str += "/polarity_shift,moving=0";
      action_list_str += "/crushing_darkness";
      action_list_str += "/lightning_strike";
      action_list_str += "/shock,moving=1";

      break;

      case TREE_CORRUPTION:

     // action_list_str += "/revivification,if=force>300";
      action_list_str += "/innervate,if=!ticking";
      action_list_str += "/resurgence,if=!ticking";
      action_list_str += "/dark_infusion";

      break;

      default: break;
      }

      action_list_default = 1;
    }
  }

  player_t::init_actions();
}

// sage_sorcerer_t::primary_resource ==================================================

int sage_sorcerer_t::primary_resource() const
{
  return RESOURCE_FORCE;
}

// sage_sorcerer_t::primary_role ==================================================

int sage_sorcerer_t::primary_role() const
{
  switch ( player_t::primary_role() )
  {
  case ROLE_HEAL:
    return ROLE_HEAL;
  case ROLE_DPS:
  case ROLE_SPELL:
    return ROLE_SPELL;
  default:
    if ( primary_tree() == TREE_SEER || primary_tree() == TREE_CORRUPTION )
      return ROLE_HEAL;
    return ROLE_SPELL;
  }
}

// sage_sorcerer_t::regen ==================================================

void sage_sorcerer_t::regen( timespan_t periodicity )
{
  player_t::regen( periodicity );

  if ( buffs.concentration -> up() )
  {
    double force_regen = periodicity.total_seconds() * force_regen_per_second() * buffs.concentration -> check() * 0.10;
    resource_gain( RESOURCE_FORCE, force_regen, gains.concentration );
  }
  if ( buffs.noble_sacrifice -> up() )
  {
    double force_regen = periodicity.total_seconds() * force_regen_per_second() * buffs.noble_sacrifice -> check() * 0.25;
    resource_loss( RESOURCE_FORCE, force_regen, gains.noble_sacrifice_power_regen_lost );
  }
}

// sage_sorcerer_t::force_bonus_multiplier ================================

double sage_sorcerer_t::force_bonus_multiplier() const
{
  return player_t::force_bonus_multiplier() +
      buffs.tremors -> stack() * 0.01;
}

double sage_sorcerer_t::force_healing_bonus_multiplier() const
{
  return player_t::force_healing_bonus_multiplier() +
      talents.clairvoyance -> rank() * 0.02;
}

// sage_sorcerer_t::alacrity =============================================

double sage_sorcerer_t::alacrity() const
{
  double sh = player_t::alacrity();

  sh -= buffs.mental_alacrity -> stack() * 0.20;

  sh -= buffs.rakata_force_masters_4pc -> up() * 0.05;

  return sh;
}

// sage_sorcerer_t::create_talents ==================================================

void sage_sorcerer_t::create_talents()
{
  static const struct
  {
    const char* name;
    int maxpoints;
  } talent_descriptions[] = {
    // Seer|Corruption
    { "Immutable Force", 2 }, { "Penetrating Light", 3 }, { "Wisdom", 2 }, { "Foresight", 3 },
    { "Pain Bearer", 2 }, { "Psychic Suffusion", 2 }, { "Conveyance", 2 }, { "Rejuvenate", 1 },
    { "Valiance", 2 }, { "Preservation", 2 }, { "Mend Wounds", 1 }, { "Force Shelter", 2 },
    { "Egress", 2 }, { "Confound", 2 }, { "Healing Trance", 1 }, { "Serenity", 2 },
    { "Resplendence", 2 }, { "Clairvoyance", 3 }, { "Salvation", 1 },
    { 0, 0 },

    // Telekinetics|Lightning
    { "Inner Strength", 3 }, { "Mental Longevity", 2 }, { "Clamoring Force", 3 }, { "Minds Eye", 1 },
    { "Disturb Mind", 2 }, { "Concentration", 2 }, { "Telekinetic Defense",  2 }, { "Blockout", 2 },
    { "Telekinetic Wave", 1 }, { "Psychic Projection", 2 }, { "Force Wake", 2 },
    { "Tidal Force", 1 }, { "Telekinetic Effusion", 2 }, { "Kinetic Collapse", 2 }, { "Tremors", 1 },
    { "Telekinetic Momentum", 3 }, { "Mental Alacrity", 1 }, { "Reverberation", 5 }, { "Turbulence", 1 },
    { 0, 0 },

     // Balance|Madness
    { "Empowered Throw", 3 }, { "Jedi Resistance", 2 }, { "Will of the Jedi", 2 },
    { "Pinning Resolve", 2 }, { "Upheaval", 3 }, { "Focused Insight", 2 }, { "Critical Kinesis", 2 },
    { "Force in Balance", 1 }, { "Psychic Barrier", 3 }, { "Telekinetic Balance", 1 },
    { "Containment", 2 }, { "Mind Ward", 2 }, { "Presence of Mind", 1 }, { "Force Suppression", 1 },
    { "Drain Thoughts", 2 }, { "Assertion", 2 }, { "Mental Scarring", 3 }, { "Psychic Absorption", 2 },
    { "Sever Force", 1 },
    { 0, 0 },
  };

  unsigned i = 0;
  for ( unsigned tree = 0; tree < 3; ++tree )
  {
    for(; talent_descriptions[ i ].name != 0; ++i )
    {
      talent_trees[ tree ].push_back( new talent_t( this, talent_descriptions[ i ].name, tree,
                                                    talent_descriptions[ i ].maxpoints ) );
    }
    ++i;
  }

  player_t::create_talents();
}

// sage_sorcerer_t::create_options =================================================

void sage_sorcerer_t::create_options()
{
  player_t::create_options();

  option_t sage_sorcerer_options[] =
  {
    { "psychic_projection_dd_chance",       OPT_FLT, &( psychic_projection_dd_chance      ) },
    { "lightning_barrage_dd_chance",        OPT_FLT, &( psychic_projection_dd_chance      ) },
    { NULL, OPT_UNKNOWN, NULL }
  };

  option_t::copy( options, sage_sorcerer_options );
}

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_jedi_sage  ===================================================

player_t* player_t::create_jedi_sage( sim_t* sim, const std::string& name, race_type r )
{
  return new sage_sorcerer_t( sim, JEDI_SAGE, name, r );
}

// player_t::create_sith_sorcerer  ===================================================

player_t* player_t::create_sith_sorcerer( sim_t* sim, const std::string& name, race_type r )
{
  return new sage_sorcerer_t( sim, SITH_SORCERER, name, r );
}

// player_t::sage_sorcerer_init ======================================================

void player_t::sage_sorcerer_init( sim_t* sim )
{
  for ( unsigned int i = 0; i < sim -> actor_list.size(); i++ )
  {
    player_t* p = sim -> actor_list[i];
    p -> buffs.force_valor = new buff_t( p, "force_valor_mark_of_power", 1 );
  }
}

// player_t::sage_sorcerer_combat_begin ==============================================

void player_t::sage_sorcerer_combat_begin( sim_t* sim )
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
