// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "../simulationcraft.hpp"
#include "cons_inq.hpp"

// ==========================================================================
// Jedi Sage | Sith Sorcerer
// ==========================================================================

namespace { // ANONYMOUS NAMESPACE ==========================================

namespace sage_sorcerer { // ================================================

class class_t;

class targetdata_t : public cons_inq::targetdata_t
{
public:
  dot_t dot_weaken_mind;

  dot_t dot_healing_trance;
  dot_t dot_salvation;
  dot_t dot_rejuvenate;

  targetdata_t( class_t& source, player_t& target );
};

class class_t : public cons_inq::class_t
{
  typedef cons_inq::class_t base_t;
public:
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
    buff_t* underworld_force_masters_4pc;
    buff_t* noble_sacrifice;
    buff_t* resplendence;
    buff_t* conveyance;
    buff_t* telekinetic_focal_point;
  } buffs;

  // Gains
  struct gains_t
  {
    gain_t* concentration;
    gain_t* rippling_force;
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
    rng_t* mm;
    rng_t* tremors;
    rng_t* psychic_projection_dd;
    rng_t* rippling_force;
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
    cooldown_t* mental_alacrity;
  } cooldowns;

  // Talents
  struct talents_t
  {
    // Seer|Corruption
    talent_t* force_gift;
    talent_t* penetrating_light;
    talent_t* psychic_suffusion;
    talent_t* foresight;
    talent_t* pain_bearer;
    talent_t* immutable_force;
    talent_t* humility;
    talent_t* conveyance;
    talent_t* rejuvenate;
    talent_t* preservation;
    talent_t* valiance;
    talent_t* mend_wounds;
    talent_t* force_shelter;
    talent_t* egress;
    talent_t* confound;
    talent_t* healing_trance;
    talent_t* life_ward;
    talent_t* serenity;
    talent_t* resplendence;
    talent_t* clairvoyance;
    talent_t* master_speed;
    talent_t* force_warden;
    talent_t* amnesty;
    talent_t* salvation;

    // Telekinetics|Lightning
    talent_t* upheaval;
    talent_t* mental_longevity;
    talent_t* inner_strength;
    talent_t* minds_eye;
    talent_t* clamoring_force;
    talent_t* concentration;
    talent_t* telekinetic_defense;
    talent_t* psychic_projection;
    talent_t* telekinetic_wave;
    talent_t* telekinetic_effusion;
    talent_t* cascading_force;
    talent_t* tidal_force;
    talent_t* kinetic_collapse;
    talent_t* tremors;
    talent_t* telekinetic_momentum;
    talent_t* blockout;
    talent_t* force_wake;
    talent_t* flowing_force;
    talent_t* reverberation;
    talent_t* force_haste;
    talent_t* mental_momentum;
    talent_t* turbulence;

    // Balance|Madness
    talent_t* empowered_throw;
    talent_t* jedi_resistance;
    talent_t* will_of_the_jedi;
    talent_t* pinning_resolve;
    talent_t* drain_thoughts;
    talent_t* focused_insight;
    talent_t* critical_kinesis;
    talent_t* force_in_balance;
    talent_t* psychic_barrier;
    talent_t* telekinetic_balance;
    talent_t* containment;
    talent_t* mind_ward;
    talent_t* presence_of_mind;
    talent_t* force_suppression;
    talent_t* rippling_force;
    talent_t* mind_warp;
    talent_t* mental_scarring;
    talent_t* psychic_absorption;
    talent_t* telekinetic_focal_point;
    talent_t* metaphysical_alacrity;
    talent_t* mental_defense;
    talent_t* sever_force;
  } talents;

  int disable_double_dip;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    base_t( sim, pt == SITH_SORCERER ? SITH_SORCERER : JEDI_SAGE, name, rt ),
    buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents(),
    disable_double_dip( false )
  {
    if ( pt == SITH_SORCERER )
    {
      tree_type[ SITH_SORCERER_CORRUPTION ] = TREE_CORRUPTION;
      tree_type[ SITH_SORCERER_LIGHTNING ]  = TREE_LIGHTNING;
      tree_type[ SITH_SORCERER_MADNESS ]    = TREE_MADNESS;
      cooldowns.telekinetic_wave = get_cooldown( "chain_lightning" );
      cooldowns.mental_alacrity = get_cooldown( "polarity_shift" );
    }
    else
    {
      tree_type[ JEDI_SAGE_SEER ]         = TREE_SEER;
      tree_type[ JEDI_SAGE_TELEKINETICS ] = TREE_TELEKINETICS;
      tree_type[ JEDI_SAGE_BALANCE ]      = TREE_BALANCE;
      cooldowns.telekinetic_wave = get_cooldown( "telekinetic_wave" );
      cooldowns.mental_alacrity = get_cooldown( "mental_alacrity" );
    }

    create_talents();
    create_options();
  }

  // Character Definition
  virtual targetdata_t* new_targetdata( player_t& target ) // override
  { return new targetdata_t( *this, target ); }

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
    base_t::init_scaling();
    scales_with[ STAT_STRENGTH ] = false;
  }

  virtual role_type primary_role() const;

  virtual double    force_regen_per_second() const; // override
  virtual void      regen( timespan_t periodicity );

  virtual double    force_bonus_multiplier() const;
  virtual double    force_healing_bonus_multiplier() const;
  virtual double    alacrity() const;

  virtual double force_crit_chance() const
  { return base_t::force_crit_chance() + talents.penetrating_light -> rank() * 0.01; }

  virtual double force_healing_crit_chance() const
  { return base_t::force_healing_crit_chance() + talents.penetrating_light -> rank() * 0.01 + talents.serenity -> rank() * 0.01; }

  virtual double composite_player_heal_multiplier( school_type school ) const
  { return base_t::composite_player_heal_multiplier( school ); }

  double school_damage_reduction( school_type school ) const
  { return base_t::school_damage_reduction( school ) + talents.serenity -> rank() * 0.01; }

  heal_info_t assess_heal( double      amount,
                           school_type school,
                           dmg_type    dmg_type,
                           result_type result,
                           ::action_t* action )
  {
    amount *= 1.0 + talents.pain_bearer -> rank() * 0.04;

    return base_t::assess_heal( amount, school, dmg_type, result, action );
  }

  virtual ::action_t* create_action( const std::string& name, const std::string& options );
  void create_talents();
  virtual void create_options();

  void trigger_tidal_force( double pc )
  {
    if ( talents.tidal_force -> rank() && buffs.tidal_force -> trigger( 1, 0, pc ) )
      cooldowns.telekinetic_wave -> reset();
  }
  
  void trigger_tremors(){
      cooldowns.mental_alacrity -> reduce(from_seconds( 1.0 ));
  }

  virtual bool report_attack_type( ::action_t::policy_t policy )
  {
    return policy == action_t::melee_policy ||
           policy == action_t::force_policy ||
           ( primary_role() == ROLE_HEAL && policy == action_t::force_heal_policy );
  }
};

targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  cons_inq::targetdata_t( source, target ),
  dot_weaken_mind( "weaken_mind", &source ),
  dot_healing_trance( "healing_trance", &source ),
  dot_salvation( "salvation", &source ),
  dot_rejuvenate( "rejuvenate", &source )
{
  add( dot_weaken_mind );
  alias( dot_weaken_mind, "affliction" );

  add( dot_healing_trance );
  alias( dot_healing_trance, "innervate" );

  add( dot_salvation );
  alias( dot_salvation, "revivification" );

  add( dot_rejuvenate );
  alias( dot_rejuvenate, "resurgence" );
}

// ==========================================================================
// Jedi Sage Abilities
// ==========================================================================

class action_t : public ::action_t
{
  typedef ::action_t base_t;
public:
  action_t( const std::string& n, class_t* player,
            attack_policy_t policy, resource_type r, school_type s ) :
    base_t( ACTION_ATTACK, n, player, policy, r, s )
  {}

  targetdata_t* targetdata() const
  { return static_cast<targetdata_t*>( base_t::targetdata() ); }

  class_t* p() const
  { return static_cast<class_t*>( player ); }

  class_t* cast() const
  { return p(); }
};

struct attack_t : public action_t
{
  attack_t( const std::string& n, class_t* player ) :
    action_t( n, player, melee_policy, RESOURCE_FORCE, SCHOOL_KINETIC )
  {
    may_crit = true;
  }
};

typedef cons_inq::saber_strike_t<attack_t> saber_strike_t;

struct spell_t : public action_t
{
  bool influenced_by_inner_strength;

  spell_t( const std::string& n, class_t* p, school_type s=SCHOOL_KINETIC ) :
    action_t( n, p, force_policy, RESOURCE_FORCE, s ),
    influenced_by_inner_strength( true )
  {
    may_crit   = true;
    tick_may_crit = true;
  }

  virtual void init()
  {
    action_t::init();

    if ( td.base_min > 0 && ! channeled ){
      crit_bonus += p() -> talents.mental_scarring -> rank() * 0.1;
      base_multiplier *= 1.0 + p() -> talents.drain_thoughts -> rank() * 0.03;
    }
  }

  virtual void execute()
  {
    action_t::execute();

    if ( dd.base_min > 0 )
      p() -> buffs.force_potency -> decrement();
  }

  virtual void player_buff()
  {
    action_t::player_buff();

    if ( ( dd.base_min > 0 || channeled ) && p() -> buffs.force_potency -> up() )
      player_crit += 0.60;
  }

  virtual void target_debuff( player_t* t, dmg_type dmg_type )
  {
    action_t::target_debuff( t, dmg_type );

    class_t* p = cast();

    // This method is in target_debuff so that it is checked before every dot tick

    // Assume channeled spells don't profit
    if ( p -> talents.force_suppression -> rank() > 0 && td.base_min > 0 && !channeled )
      if ( p -> buffs.force_suppression -> up() )
        td.target_multiplier *= 1.20;
  }

  virtual void assess_damage( player_t* t, double dmg_amount, dmg_type dmg_type, result_type dmg_result )
  {
    action_t::assess_damage( t, dmg_amount, dmg_type, dmg_result );
    class_t* p = cast();

    // Procs from all critical damage including dot ticks. Source:  17/01/2012 http://sithwarrior.com/forums/Thread-Sorcerer-Sage-Mechanics-and-Quirks

    if ( dmg_result == RESULT_CRIT && p -> talents.telekinetic_effusion -> rank() > 0 )
    {
      p -> buffs.telekinetic_effusion -> trigger( 2 );
    }

  }

  virtual double cost() const
  {
    double c = action_t::cost();

    class_t* p = cast();

    if ( p -> talents.inner_strength -> rank() > 0 && influenced_by_inner_strength )
    {
      c *= 1.0 - p -> talents.inner_strength -> rank() * 0.03;
      c = ceil( c ); // 17/01/2012 According to http://sithwarrior.com/forums/Thread-Sorcerer-Sage-Mechanics-and-Quirks
    }

    if ( p -> buffs.telekinetic_effusion -> check() > 0 )
    {
      c *= 0.25;
      c = ceil( c ); // FIXME: floor or ceil?
    }

    return c;
  }

  virtual void consume_resource()
  {
    action_t::consume_resource();

    class_t* p = cast();
    double c = action_t::cost();

    if(c > 0){
        p -> buffs.telekinetic_effusion -> up();
        p -> buffs.telekinetic_effusion -> decrement();
    }
  }

  virtual void tick( dot_t* d )
  {
    action_t::tick( d );

    class_t* p = cast();

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

  virtual void last_tick( dot_t* d )
  {
    action_t::last_tick( d );
    if ( channeled )
      p() -> buffs.force_potency -> decrement();
  }
};

struct force_valor_t : public spell_t
{
  force_valor_t( class_t* p, const std::string& n, const std::string& options_str ) :
    spell_t( n, p )
  {
    parse_options( options_str );
    base_cost = 0.0;
    harmful = false;
  }

  virtual void execute()
  {
    spell_t::execute();

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

    return spell_t::ready();
  }
};

struct noble_sacrifice_t : public spell_t
{
  noble_sacrifice_t( class_t* p, const std::string& n, const std::string& options_str ) :
    spell_t( n, p )
  {
    parse_options( options_str );
    base_cost = 0.0;
    harmful = false;
  }

  virtual void execute()
  {
    spell_t::execute();

    class_t* p = cast();

    double health_loss = p -> resource_max[ RESOURCE_HEALTH ]
                         * ( 0.15 - p -> talents.valiance -> rank() * 0.02 );

    p -> resource_loss( RESOURCE_HEALTH, health_loss, p -> gains.noble_sacrifice_health );
    p -> resource_gain( RESOURCE_FORCE, p -> resource_max[ RESOURCE_FORCE ] * 0.08 , p -> gains.noble_sacrifice_power );

    if ( ! p -> buffs.resplendence -> up() )
      p -> buffs.noble_sacrifice -> trigger();
  }
};

struct project_t : public spell_t
{
  project_t* upheaval;

  project_t( class_t* p, const std::string& n, const std::string& options_str, bool is_upheaval = false ) :
    spell_t( n + std::string( is_upheaval ? "_upheaval" : "" ), p ),
    upheaval()
  {
    rank_level_list = { 1, 4, 7, 11, 14, 17, 23, 34, 47, 50, 53, 55};

    parse_options( options_str );

    range = 30.0;
    if ( player -> set_bonus.battlemaster_force_masters -> four_pc() )
      range += 5.0;

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

      cooldown -> duration = from_seconds( 6.0 );

      if ( p -> talents.upheaval -> rank() > 0 )
      {
        upheaval = new project_t( p, n, options_str, true );
        add_child( upheaval );
      }
    }

    base_multiplier *= 1.0 + p -> talents.empowered_throw -> rank() * 0.02;
  }

  virtual void execute()
  {
    spell_t::execute();

    if ( upheaval )
    {
      class_t* p = cast();

      if ( p -> rngs.upheaval -> roll( p -> talents.upheaval -> rank() * 0.15 ) )
        upheaval -> execute();
    }
  }
};

struct rippling_force_d_t : public spell_t
{
  rippling_force_d_t( class_t* p, const std::string& n) :
    spell_t( n, p, SCHOOL_KINETIC )
  {

    td.standardhealthpercentmin = td.standardhealthpercentmax = .027;
    td.power_mod = 0.27;

    base_tick_time = from_seconds( 1 );
    num_ticks = 1;
    base_cost = 0;
    may_crit = false;
    background = true;
  }

  virtual void execute(){
      spell_t::execute();
      class_t* p = cast();

      double f = 2;
      p -> resource_gain( RESOURCE_FORCE, f , p -> gains.rippling_force );

  }
};

//This is only so I can add a child for FL specifically and not get 'parent already exists' error
struct rippling_force_tt_t : public rippling_force_d_t
{
  rippling_force_tt_t( class_t* p, const std::string& n) :
    rippling_force_d_t( p, n)
    {
    }
};


struct telekinetic_throw_t : public spell_t
{
  bool is_buffed_by_psychic_projection;
  rippling_force_tt_t* rf;

  telekinetic_throw_t( class_t* p, const std::string& n, const std::string& options_str ) :
    spell_t( n, p ),
    is_buffed_by_psychic_projection( false ),
    rf( new rippling_force_tt_t(p, "rippling_force_tt") )
  {
    rank_level_list = { 2, 5, 8, 11, 14, 19, 27, 39, 50, 55 };

    parse_options( options_str );

    id = p -> type == SITH_SORCERER ? 188186 : 189258;

    td.standardhealthpercentmin = td.standardhealthpercentmax = .079;
    td.power_mod = 0.79;

    base_cost = 30.0;
    if ( player -> set_bonus.rakata_force_masters -> two_pc() )
        base_cost -= 2.0;
    if ( player -> set_bonus.underworld_force_masters -> two_pc() )
        base_cost -= 2.0;

    range = 30.0;
    num_ticks = 3;
    base_tick_time = from_seconds( 1.0 );
    may_crit = false;
    channeled = true;
    tick_zero = true;

    if ( p -> talents.telekinetic_balance -> rank() > 0 )
      cooldown -> duration = timespan_t::zero();
    else
      cooldown -> duration = from_seconds( 6.0 );

    base_crit += p -> talents.critical_kinesis -> rank() * 0.03;
    base_multiplier *= 1.0 + p -> talents.empowered_throw -> rank() * 0.02;
    add_child( rf );
  }

  virtual void execute()
  {
    class_t* p = cast();

    if ( p -> buffs.psychic_projection -> up() || p -> buffs.psychic_projection_dd -> up() )
    {
      is_buffed_by_psychic_projection = true;
      if ( p -> bugs && p -> buffs.psychic_projection_dd -> check() )
      {
        num_ticks = 1;
      }
      if ( p -> bugs && ! p -> disable_double_dip && p -> buffs.psychic_projection -> check() )
      {
        p -> buffs.psychic_projection_dd -> trigger();
      }
    }
    else
      is_buffed_by_psychic_projection = false;

    spell_t::execute();

    num_ticks = 3;
  }

  virtual timespan_t tick_time() const
  {
    timespan_t tt = spell_t::tick_time();

    if ( is_buffed_by_psychic_projection )
      tt *= 0.5;

    return tt;
  }

  virtual void last_tick( dot_t* d )
  {
    spell_t::last_tick( d );

    class_t* p = cast();

    if ( is_buffed_by_psychic_projection )
      p -> buffs.psychic_projection -> expire();
  }

  virtual void tick( dot_t* d )
  {
    spell_t::tick( d );

    if ( tick_dmg > 0 )
    {
      class_t* p = cast();
      if ( p -> rngs.rippling_force -> roll( p -> talents.rippling_force -> rank() * 0.10 ) ){
          rf -> execute();
      }

      if ( p -> talents.psychic_barrier -> rank() > 0 && p -> rngs.psychic_barrier -> roll( p -> talents.psychic_barrier -> rank() * ( 1 / 3.0 ) ) )
      {
        double f = p -> resource_max[ RESOURCE_FORCE ] * 0.01;
        p -> resource_gain( RESOURCE_FORCE, f , p -> gains.psychic_barrier );
      }
      p -> buffs.presence_of_mind -> trigger();
      p -> buffs.telekinetic_focal_point -> trigger(1, 0, 0.1 * p -> talents.telekinetic_focal_point -> rank());
    }
  }
};

struct disturbance_t : public spell_t
{
  spell_t* tm;
  bool is_tm;
  rippling_force_d_t* rf;

  disturbance_t( class_t* p, const std::string& n, const std::string& options_str, bool is_tm_in = false ) :
    spell_t( n + std::string( is_tm_in ? "_tm" : "" ), p ),
    tm( 0 ),
    is_tm ( is_tm_in)
  {
    rank_level_list = { 10, 13, 16, 25, 36, 45, 50, 53, 55 };

    parse_options( options_str );

    dd.standardhealthpercentmin = .14;
    dd.standardhealthpercentmax = .16;
    dd.power_mod = 1.5;

    base_execute_time = from_seconds( 1.5 );

    base_cost = 40.0;
    if ( player -> set_bonus.rakata_force_masters -> two_pc() )
        base_cost -= 2.0;
    if ( player -> set_bonus.underworld_force_masters -> two_pc() )
        base_cost -= 2.0;
    range = 30.0;
    crit_bonus += p -> talents.reverberation -> rank() * 0.25;

    base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02;
    base_crit += p -> talents.critical_kinesis -> rank() * 0.03;

    if ( is_tm_in )
    {
      dd.standardhealthpercentmin = .035;
      dd.standardhealthpercentmax = .055;
      dd.power_mod = 0.45;
      base_cost = 0.0;
      background = true;
    }
    else if ( p -> talents.telekinetic_momentum -> rank() > 0 )
    {
      tm = new disturbance_t( p, n, options_str, true );
      add_child( tm );
    }

    //Only add rf if we're not on a proc'd version
    if (!is_tm_in){
        rf = new rippling_force_d_t(p, "rippling_force_d"); 
        add_child( rf );
    }
  }

  virtual timespan_t execute_time() const
  {
    if ( p() -> buffs.presence_of_mind -> up() )
      return timespan_t::zero();

    return spell_t::execute_time();
  }

  virtual void player_buff() // override
  {
    spell_t::player_buff();

    if ( p() -> buffs.presence_of_mind -> up() )
      dd.player_multiplier *= 1.35;
  }

  virtual void impact( player_t* t, result_type impact_result, double travel_dmg )
  {
    spell_t::impact( t, impact_result, travel_dmg );

    if ( result_is_hit( impact_result ) )
    {
      class_t* p = cast();

      p -> buffs.concentration -> trigger();
      p -> buffs.telekinetic_focal_point -> trigger(1, 0, 0.5 * p -> talents.telekinetic_focal_point -> rank());

      // Does the TM version also proc Tidal Force? We'll assume that it does
      // not for now.
      if ( !is_tm )
        p -> trigger_tidal_force( 0.6 );
    }
  }

  virtual void execute()
  {
    spell_t::execute();

    class_t* p = cast();

    //This naming is confusing
    //tm != NULL on NON-TM proc IFF we're spec'd into telekinetic_momentum
    if ( tm )
    {
      double tm_trigger_pct_per_stack = 0.10;
      if(p-> buffs.mental_alacrity -> up() && p -> talents.flowing_force -> rank())
          tm_trigger_pct_per_stack *= 2;
      if ( p -> rngs.tm -> roll( p -> talents.telekinetic_momentum -> rank() * tm_trigger_pct_per_stack ) )
      {
        tm -> execute();
        p -> buffs.tremors -> trigger( 1 );
        if ( p -> rngs.tremors -> roll( p -> talents.tremors -> rank() * 0.50 ) )
            p -> trigger_tremors();
      }
    }

    if ( p -> rngs.rippling_force -> roll( p -> talents.rippling_force -> rank() * 0.30 ) && !is_tm){
        rf -> execute();
    }

    // TESTME: Should PoM really affect the tm proc as implemented here?
    p -> buffs.presence_of_mind -> expire();
  }
};

struct mind_crush_t : public spell_t
{
  struct mind_crush_dot_t : public spell_t
  {
    bool is_buffed_by_presence_of_mind;

    mind_crush_dot_t( class_t* p, const std::string& n ) :
      spell_t( n, p ),
      is_buffed_by_presence_of_mind( false )
    {
      rank_level_list = { 14, 19, 28, 38, 50, 52, 55};

      td.standardhealthpercentmin = td.standardhealthpercentmax = .0295;
      td.power_mod = 0.295;

      base_tick_time = from_seconds( 1.0 );
      num_ticks = 6 + p -> talents.mind_warp -> rank() * 1;
      range = 30.0;
      influenced_by_inner_strength = false;
      background = true;
      may_crit = false;

      base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02; 
    }

    virtual void player_buff() // override
    {
      spell_t::player_buff();

      // 1.2 PTS: Mind Crush's periodic damage now benefits from the 35%
      // damage bonus when it is used to consume the Presence of Mind buff.
      if ( is_buffed_by_presence_of_mind )
        td.player_multiplier *= 1.35;
    }

    virtual void target_debuff( player_t* t, dmg_type dmg_type )
    {
      spell_t::target_debuff( t, dmg_type );

      class_t* p = cast();

      if ( p -> talents.force_suppression -> rank() > 0 )
        p -> benefits.fs_mind_crush -> update( p -> buffs.force_suppression -> check() > 0 );
    }

    virtual void tick( dot_t* d )
    {
      spell_t::tick( d );
      class_t* p = cast();
      //Is the second tick calculated differently?
      if ( p -> rngs.mm -> roll( p -> talents.mental_momentum -> rank() * 0.10 ) )
          spell_t::tick( d );
    }
  };

  mind_crush_dot_t* dot_spell;

  mind_crush_t( class_t* p, const std::string& n, const std::string& options_str ) :
    spell_t( n, p ),
    dot_spell( new mind_crush_dot_t( p, n + "_dot" ) )
  {
    static const int ranks[] = { 14, 19, 28, 38, 50, 52, 55};
    rank_level_list = util_t::array_to_vector( ranks );

    parse_options( options_str );

    dd.standardhealthpercentmin = .103;
    dd.standardhealthpercentmax = .143;
    dd.power_mod = 1.23;

    base_execute_time = from_seconds( 2.0 );
    base_cost = 40.0;
    range = 30.0;
    cooldown -> duration = from_seconds( 15.0 );
    if ( player -> set_bonus.battlemaster_force_masters -> two_pc() )
      cooldown -> duration -= from_seconds( 1.5 );
    influenced_by_inner_strength = true;

    base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02;

    add_child( dot_spell );
  }

  virtual timespan_t execute_time() const
  {
    if ( p() -> buffs.presence_of_mind -> up() )
      return timespan_t::zero();

    return spell_t::execute_time();
  }

  virtual void player_buff() // override
  {
    spell_t::player_buff();

    bool p_o_m = p() -> buffs.presence_of_mind -> up();
    dot_spell -> is_buffed_by_presence_of_mind = p_o_m;
    if( p_o_m )
      dd.player_multiplier *= 1.35;
  }

  virtual void execute()
  {
    spell_t::execute();
    dot_spell -> execute();
    p() -> buffs.presence_of_mind -> expire();
  }
};

struct weaken_mind_t : public spell_t
{
  weaken_mind_t( class_t* p, const std::string& n, const std::string& options_str ) :
    spell_t( n, p, SCHOOL_INTERNAL )
  {
    rank_level_list = { 16, 22, 33, 44, 50, 53, 55 };

    parse_options( options_str );

    td.standardhealthpercentmin = td.standardhealthpercentmax = .036;
    td.power_mod = 0.36;

    base_tick_time = from_seconds( 3.0 );
    num_ticks = 6;
    base_cost = 30.0;
    range = 30.0;
    may_crit = false;

    influenced_by_inner_strength = true;

    base_multiplier += p -> talents.empowered_throw -> rank() * 0.02;
  }

  virtual void tick( dot_t* d )
  {
    spell_t::tick( d );

    class_t* p = cast();

    if ( result == RESULT_CRIT && p -> talents.psychic_projection -> rank() > 0 )
    {
      p -> buffs.psychic_projection -> trigger( p -> buffs.psychic_projection -> max_stack );
    }

    p -> buffs.rakata_force_masters_4pc -> trigger();
    p -> buffs.underworld_force_masters_4pc -> trigger();
  }

  virtual void target_debuff( player_t* t, dmg_type dmg_type )
  {
    spell_t::target_debuff( t, dmg_type );

    class_t* p = cast();

    if ( p -> talents.force_suppression -> rank() > 0 )
      p -> benefits.fs_weaken_mind -> update( p -> buffs.force_suppression -> check() > 0 );
  }
};

struct turbulence_t : public spell_t
{
  spell_t* mm;
  bool is_mm;

  turbulence_t( class_t* p, const std::string& n, const std::string& options_str, bool is_mm_in = false ) :
    spell_t( n + std::string( is_mm_in ? "_mm" : ""), p, SCHOOL_INTERNAL ),
    mm(0),
    is_mm (is_mm_in)
  {
    check_talent( p -> talents.turbulence -> rank() );

    parse_options( options_str );

    dd.standardhealthpercentmin = .149;
    dd.standardhealthpercentmax = .209;
    dd.power_mod = 1.79;

    base_execute_time = from_seconds( 2.0 );
    base_cost = 50.0;
    range = 30.0;
    crit_bonus += p -> talents.reverberation -> rank() * 0.25;

    base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02;
    cooldown -> duration = from_seconds( 9.0 );

    if ( is_mm_in )
    {
      dd.standardhealthpercentmin = .037;
      dd.standardhealthpercentmax = .057;
      dd.power_mod = 0.474;
      base_cost = 0.0;
      background = true;
    }
    else if ( p -> talents.mental_momentum -> rank() > 0 )
    {
      mm = new turbulence_t( p, n, options_str, true );
      add_child( mm );
    }
  }

  virtual void impact( player_t* t, result_type impact_result, double travel_dmg )
  {
    spell_t::impact( t, impact_result, travel_dmg );

    if ( result_is_hit( impact_result ) )
    {
      class_t* p = cast();

      // Does the MM version also proc Tidal Force? We'll assume that it does
      // not for now.
      if ( !is_mm )
        p -> trigger_tidal_force( 0.6 );
    }
  }

  virtual void target_debuff( player_t *t, dmg_type dmg_type )
  {
    spell_t::target_debuff( t, dmg_type );

    assert( t == target );
    targetdata_t& td = *targetdata();

    bool increase_crit_chance = td.dot_weaken_mind.ticking != 0;

    cast() -> benefits.turbulence -> update( increase_crit_chance );

    if ( increase_crit_chance )
      target_crit += 1.0;
  }

  virtual void execute()
  {
    spell_t::execute();

    class_t* p = cast();

    if ( mm )
    {
      if ( p -> rngs.mm -> roll( p -> talents.mental_momentum -> rank() * 0.10 ) )
      {
        mm -> execute();
        p -> buffs.tremors -> trigger( 1 );
        if ( p -> rngs.tremors -> roll( p -> talents.tremors -> rank() * 0.50 ) )
            p -> trigger_tremors();
      }
    }

    // TESTME: Should PoM really affect the mm proc as implemented here?
    p -> buffs.presence_of_mind -> expire();
  }
};

struct force_in_balance_t : public spell_t
{
  force_in_balance_t( class_t* p, const std::string& n, const std::string& options_str ) :
    spell_t( n, p, SCHOOL_INTERNAL )
  {
    check_talent( p -> talents.force_in_balance -> rank() );

    parse_options( options_str );

    dd.standardhealthpercentmin = .167;
    dd.standardhealthpercentmax = .207;
    dd.power_mod = 1.87;

    ability_lag = from_seconds( 0.2 );
    base_cost = 50.0;
    range = 30.0;
    aoe = 2;

    cooldown -> duration = from_seconds( 15.0 );

    crit_bonus += p -> talents.mental_scarring -> rank() * 0.1;
    base_multiplier *= 1.0;
  }

  virtual void execute()
  {
    spell_t::execute();

    class_t* p = cast();

    // ToDo: Move buff to targetdata and buff trigger to impact

    p -> buffs.force_suppression -> trigger( 15 );
  }
};

struct sever_force_t : public spell_t
{
  sever_force_t( class_t* p, const std::string& n, const std::string& options_str ) :
    spell_t( n, p, SCHOOL_INTERNAL )
  {
    check_talent( p -> talents.sever_force -> rank() );

    parse_options( options_str );

    td.standardhealthpercentmin = td.standardhealthpercentmax = .036;
    td.power_mod = 0.36;

    base_tick_time = from_seconds( 3.0 );
    num_ticks = 6;
    base_cost = 20.0;
    range = 30.0;
    may_crit = false;
    cooldown -> duration = from_seconds( 9.0 );
    tick_zero = true;
    influenced_by_inner_strength = true;

    base_multiplier += p -> talents.empowered_throw -> rank() * 0.02;
  }

  virtual void target_debuff( player_t* t, dmg_type dmg_type )
  {
    spell_t::target_debuff( t, dmg_type );

    class_t* p = cast();

    if ( p -> talents.force_suppression -> rank() > 0 )
      p -> benefits.fs_sever_force -> update( p -> buffs.force_suppression -> check() > 0 );
  }
};

struct mental_alacrity_t : public spell_t
{
  mental_alacrity_t( class_t* p, const std::string& n, const std::string& options_str ) :
    spell_t( n, p )
  {
    parse_options( options_str );

    cooldown -> duration = from_seconds( 120 );
    cooldown -> duration -= from_seconds( p -> talents.flowing_force -> rank() * 15 );
    if ( player -> set_bonus.underworld_force_masters -> two_pc())
        cooldown -> duration -= from_seconds(15);
    trigger_gcd = timespan_t::zero();
    harmful = false;
  }

  virtual void execute()
  {
    spell_t::execute();

    p() -> buffs.mental_alacrity -> trigger();
  }
};

struct telekinetic_wave_t : public spell_t
{
  spell_t* tm;

  telekinetic_wave_t( class_t* p, const std::string& n, const std::string& options_str, bool is_tm = false ) :
    spell_t( n + std::string( is_tm ? "_tm" : "" ), p ),
    tm( nullptr )
  {
    check_talent( p -> talents.telekinetic_wave -> rank() );

    parse_options( options_str );

    if ( is_tm )
    {
      dd.standardhealthpercentmin = .057;
      dd.standardhealthpercentmax = .077;
      dd.power_mod = .667;

      base_cost = 0.0;
      background = true;
    }
    else
    {
      dd.standardhealthpercentmin = .201;
      dd.standardhealthpercentmax = .241;
      dd.power_mod = 2.21;

      base_cost = 60.0;
      cooldown -> duration = from_seconds( 6.0 );
      base_execute_time = from_seconds( 3.0 );

      if ( p -> talents.telekinetic_momentum -> rank() > 0 )
      {
        tm = new telekinetic_wave_t( p, n, options_str, true );
        add_child( tm );
      }
    }

    range = 30.0;
    aoe = 4;

    base_multiplier *= 1.0 + p -> talents.clamoring_force -> rank() * 0.02 + p -> talents.psychic_suffusion -> rank() * 0.05;
    crit_bonus += p -> talents.reverberation -> rank() * 0.25;
  }

  virtual void execute()
  {
    spell_t::execute();

    class_t* p = cast();

    if ( tm )
    {
      double tm_trigger_pct_per_stack = 0.10;
      if(p -> buffs.mental_alacrity -> up() && p -> talents.flowing_force -> rank())
          tm_trigger_pct_per_stack *= 2;
      if ( p -> rngs.tm -> roll( p -> talents.telekinetic_momentum -> rank() * tm_trigger_pct_per_stack ) )
      {
        tm -> execute();
        p -> buffs.tremors -> trigger( 1 );
        if ( p -> rngs.tremors -> roll( p -> talents.tremors -> rank() * 0.50 ) )
            p -> trigger_tremors();
      }
    }
  }

  virtual double cost() const
  {
      spell_t::cost();

      class_t* p = cast();

      double c = action_t::cost();

      if(p -> buffs.tidal_force -> up())
          c = 0;

      return c;
  }

  virtual timespan_t execute_time() const
  {
    timespan_t et = spell_t::execute_time();

    class_t* p = cast();

    if ( p -> buffs.tidal_force -> up() )
      et = timespan_t::zero();

    return et;
  }

  virtual void update_ready()
  {
    spell_t::update_ready();

    class_t* p = cast();

    p -> buffs.tidal_force -> expire();
  }
};

struct force_potency_t : public spell_t
{
  force_potency_t( class_t* p, const std::string& n, const std::string& options_str ) :
    spell_t( n, p )
  {
    parse_options( options_str );
    cooldown -> duration = from_seconds( 90.0 );
    if ( p -> set_bonus.battlemaster_stalkers -> four_pc() )
      cooldown -> duration -= from_seconds( 15.0 );
    harmful = false;

    trigger_gcd = timespan_t::zero();
  }

  virtual void execute()
  {
    spell_t::execute();

    class_t* p = cast();

    p -> buffs.force_potency -> trigger ( p -> buffs.force_potency -> max_stack );
  }
};

class heal_t : public ::heal_t
{
  typedef ::heal_t base_t;
public:
  bool influenced_by_inner_strength;

  heal_t( const std::string& n, class_t* p, const school_type s=SCHOOL_KINETIC ) :
    base_t( n, p, force_heal_policy, RESOURCE_FORCE, s ),
    influenced_by_inner_strength( true )
  {
    may_crit   = true;
    tick_may_crit = true;
  }

  targetdata_t* targetdata() const
  { return static_cast<targetdata_t*>( base_t::targetdata() ); }

  class_t* p() const
  { return static_cast<class_t*>( player ); }

  virtual void init()
  {
    base_t::init();

    if ( td.base_min > 0 && !channeled )
      crit_bonus += p() -> talents.mental_scarring -> rank() * 0.1;
  }

  virtual double cost() const
  {
    double c = base_t::cost();

    if ( p() -> talents.inner_strength -> rank() > 0 && influenced_by_inner_strength )
    {
      c *= 1.0 - p() -> talents.inner_strength -> rank() * 0.03;
      c = ceil( c ); // 17/01/2012 According to http://sithwarrior.com/forums/Thread-Sorcerer-Sage-Mechanics-and-Quirks
    }

    if ( p() -> buffs.telekinetic_effusion -> check() > 0 )
    {
      c *= 0.25;
      c = floor( c ); // FIXME: floor or ceil?
    }

    return c;
  }

  virtual void consume_resource()
  {
    base_t::consume_resource();

    double c = action_t::cost();

    if(c > 0){
        p() -> buffs.telekinetic_effusion -> up();
        p() -> buffs.telekinetic_effusion -> decrement();
    }
  }
};

struct deliverance_t : public heal_t
{
  deliverance_t( class_t* p, const std::string& n, const std::string& options_str ) :
    heal_t( n, p, SCHOOL_INTERNAL )
  {
    parse_options( options_str );

    dd.standardhealthpercentmin = .16;
    dd.standardhealthpercentmax = .18;
    dd.power_mod = 3.41;

    base_cost = 55.0;
    base_execute_time = from_seconds( 3.0 );
    base_execute_time -= from_seconds( p -> talents.immutable_force -> rank() * 0.25 );

    range = 30.0;
  }

  virtual timespan_t execution_time()
  {
    timespan_t et = heal_t::execute_time();

    // FIXME: check if -1.0s becomes before or after Alacrity
    if ( p() -> buffs.conveyance -> up() )
      et -= from_seconds( 1.0 );

    return et;
  }

  virtual void execute()
  {
    heal_t::execute();

    p() -> buffs.conveyance -> expire();
  }
};

struct benevolence_t : public heal_t
{
  benevolence_t( class_t* p, const std::string& n, const std::string& options_str ) :
    heal_t( n, p, SCHOOL_INTERNAL )
  {
    parse_options( options_str );

    dd.standardhealthpercentmin = .086;
    dd.standardhealthpercentmax = .126;
    dd.power_mod = 2.13;

    base_cost = 50.0;
    base_execute_time = from_seconds( 1.5 );

    range = 30.0;
  }

  virtual double cost() const
  {
    double c = heal_t::cost();

    if ( p() -> buffs.conveyance -> check() )
    {
      c *= 0.5;
      c = ceil( c );
    }

    return c;
  }

  virtual void consume_resource()
  {
    heal_t::consume_resource();

    p() -> buffs.conveyance -> up(); // uptime tracking
  }

  virtual void execute()
  {
    heal_t::execute();

    p() -> buffs.conveyance -> expire();
  }
};

struct healing_trance_t : public heal_t
{
  healing_trance_t( class_t* p, const std::string& n, const std::string& options_str ) :
    heal_t( n, p, SCHOOL_INTERNAL )
  {
    parse_options( options_str );

    td.standardhealthpercentmin = td.standardhealthpercentmax = .056;
    td.power_mod = 1.12;

    tick_zero = true;
    channeled = true;

    base_cost = 40.0;
    num_ticks = 3;
    base_tick_time = from_seconds( 1.0 );
    cooldown -> duration = from_seconds( 9.0 );
    if ( player -> set_bonus.rakata_force_mystics -> two_pc() )
      cooldown -> duration -= from_seconds( 1.5 );

    range = 30.0;
  }

  virtual void tick( dot_t* d )
  {
    heal_t::tick( d );

    if ( result == RESULT_CRIT )
    {
      p() -> buffs.resplendence -> trigger();
    }
  }

  virtual void player_buff()
  {
    heal_t::player_buff();

    // FIXME: probably needs to be saved for whole channel of the spell
    if ( p() -> buffs.conveyance -> up() )
      player_crit += 0.25;
  }

  virtual void execute()
  {
    heal_t::execute();

    p() -> buffs.conveyance -> expire();
  }
};

struct rejuvenate_t : public heal_t
{
  rejuvenate_t( class_t* p, const std::string& n, const std::string& options_str ) :
    heal_t( n, p, SCHOOL_INTERNAL )
  {
    parse_options( options_str );

    dd.standardhealthpercentmin = .035;
    dd.standardhealthpercentmax = .055;
    dd.power_mod = 0.91;

    td.standardhealthpercentmin = td.standardhealthpercentmax = .02;
    td.power_mod = 0.393;

    base_cost = 30.0;
    cooldown -> duration = from_seconds( 6.0 );

    num_ticks = 3 + p -> talents.force_shelter -> rank();
    base_tick_time = from_seconds( 3.0 );

    range = 30.0;
  }

  virtual void execute()
  {
    heal_t::execute();

    p() -> buffs.conveyance -> trigger();
  }
};

struct salvation_t : public heal_t
{
  struct salvation_tick_spell_t : public heal_t
  {
    salvation_tick_spell_t( class_t* p, const std::string& n ) :
      heal_t( n + "tick_spell", p, SCHOOL_INTERNAL )
    {
      dd.standardhealthpercentmin = dd.standardhealthpercentmax = .019;
      dd.power_mod = 0.376;

      background = true;
      dual        = true;
      direct_tick = true;

      range = 30.0;

      base_multiplier *= 1.0 + p -> talents.psychic_suffusion -> rank() * 0.05;

      stats = player -> get_stats( n, this );
    }
  };

  salvation_tick_spell_t* tick_spell;

  salvation_t( class_t* p, const std::string& n, const std::string& options_str ) :
    heal_t( n, p, SCHOOL_INTERNAL ),
    tick_spell( 0 )
  {
    parse_options( options_str );
    harmful = false;

    num_ticks = 10;
    tick_zero = true;
    base_tick_time = from_seconds( 1.0 );

    base_cost = 100.0;
    base_execute_time = from_seconds( 2.0 );
    cooldown -> duration = from_seconds( 15.0 );

    range = 30.0;

    tick_spell = new salvation_tick_spell_t( p, n );
  }

  virtual void tick( dot_t* d )
  {
    heal_t::tick( d );

    tick_spell -> execute();
  }

  virtual double cost() const
  {
    double c = heal_t::cost();

    if ( p() -> buffs.conveyance -> check() )
    {
      c *= 0.7;
      c = ceil( c );
    }

    return c;
  }

  virtual void consume_resource()
  {
    heal_t::consume_resource();

    p() -> buffs.conveyance -> up(); // uptime tracking
  }

  virtual void execute()
  {
    heal_t::execute();

    p() -> buffs.conveyance -> expire();
  }
};

class absorb_t : public ::absorb_t
{
  typedef ::absorb_t base_t;
public:
  absorb_t( const std::string& n, class_t* p, school_type s=SCHOOL_KINETIC ) :
    base_t( n, p, force_heal_policy, RESOURCE_FORCE, s )
  {}

  targetdata_t* targetdata() const
  { return static_cast<targetdata_t*>( base_t::targetdata() ); }

  class_t* p() const
  { return static_cast<class_t*>( player ); }

  virtual void consume_resource()
  {
    base_t::consume_resource();

    double c = action_t::cost();

    if(c > 0){
        p() -> buffs.telekinetic_effusion -> up();
        p() -> buffs.telekinetic_effusion -> decrement();
    }
  }
};

struct force_armor_t : public absorb_t
{
  force_armor_t( class_t* p, const std::string& n, const std::string& options_str ) :
    absorb_t( n, p, SCHOOL_INTERNAL )
  {
    parse_options( options_str );

    dd.standardhealthpercentmin = .164;
    dd.standardhealthpercentmax = .164;
    dd.power_mod = 3.27;

    base_cost = 65.0;
    base_cost -= p -> talents.preservation -> rank() * 15.0;
    cooldown -> duration = from_seconds( 4.5 );
    cooldown -> duration -= from_seconds( p -> talents.preservation -> rank() * 1.5 );

    range = 30.0;

    base_multiplier *= p -> talents.telekinetic_defense -> rank() * 0.10;
  }

  virtual void execute()
  {
    absorb_t::execute();

    p() -> buffs.conveyance -> expire();
  }
};

// ==========================================================================
// sage_sorcerer Character Definition
// ==========================================================================

// class_t::create_action ===================================================

::action_t* class_t::create_action( const std::string& name,
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

  if ( name == "saber_strike"         ) return new      saber_strike_t( this, options_str );

  return base_t::create_action( name, options_str );
}

// class_t::init_talents ============================================

void class_t::init_talents()
{
  base_t::init_talents();

  // Seer|Corruption
  talents.force_gift            = find_talent( "Force Gift" );
  talents.penetrating_light     = find_talent( "Penetrating Light" );
  talents.psychic_suffusion     = find_talent( "Psychic Suffusion" );
  talents.foresight             = find_talent( "Foresight" );
  talents.pain_bearer           = find_talent( "Pain Bearer" );
  talents.immutable_force       = find_talent( "Immutable Force" );
  talents.humility              = find_talent( "Humility" );
  talents.conveyance            = find_talent( "Conveyance" );
  talents.rejuvenate            = find_talent( "Rejuvenate" );
  talents.preservation          = find_talent( "Preservation" ); // add
  talents.valiance              = find_talent( "Valiance" );
  talents.mend_wounds           = find_talent( "Mend Wounds" );
  talents.force_shelter         = find_talent( "Force Shelter" ); // add buff
  talents.egress                = find_talent( "Egress" ); // add
  talents.confound              = find_talent( "Confound" ); // add
  talents.healing_trance        = find_talent( "Healing Trance" );
  talents.life_ward             = find_talent( "Life Ward" );
  talents.serenity              = find_talent( "Serenity" );
  talents.resplendence          = find_talent( "Resplendence" );
  talents.clairvoyance          = find_talent( "Clairvoyance" );
  talents.master_speed          = find_talent( "Master Speed" );
  talents.force_warden          = find_talent( "Force Warden" );
  talents.amnesty               = find_talent( "Amnesty");
  talents.salvation             = find_talent( "Salvation" );

  // Telekinetics|Lightning
  talents.upheaval              = find_talent( "Upheaval" );
  talents.mental_longevity      = find_talent( "Mental Longevity" );
  talents.inner_strength        = find_talent( "Inner Strength" );
  talents.minds_eye             = find_talent( "Mind's Eye" );
  talents.clamoring_force       = find_talent( "Clamoring Force" );
  talents.concentration         = find_talent( "Concentration" );
  talents.telekinetic_defense   = find_talent( "Telekinetic Defense" );
  talents.psychic_projection    = find_talent( "Psychic Projection" );
  talents.telekinetic_wave      = find_talent( "Telekinetic Wave" );
  talents.telekinetic_effusion  = find_talent( "Telekinetic Effusion" );
  talents.cascading_force       = find_talent( "Cascading Force" );
  talents.tidal_force           = find_talent( "Tidal Force" );
  talents.kinetic_collapse      = find_talent( "Kinetic Collapse" );
  talents.tremors               = find_talent( "Tremors" );
  talents.telekinetic_momentum  = find_talent( "Telekinetic Momentum" );
  talents.blockout              = find_talent( "Blockout" );
  talents.force_wake            = find_talent( "Force Wake" );
  talents.flowing_force         = find_talent( "Flowing Force" );
  talents.reverberation         = find_talent( "Reverberation" );
  talents.force_haste           = find_talent( "Force Haste" );
  talents.mental_momentum       = find_talent( "Mental Momentum" );
  talents.turbulence            = find_talent( "Turbulence" );

  // Balance|Madness
  talents.empowered_throw       = find_talent( "Empowered Throw" );
  talents.jedi_resistance       = find_talent( "Jedi Resistance" );
  talents.will_of_the_jedi      = find_talent( "Will of the Jedi" );
  talents.pinning_resolve       = find_talent( "Pinning Resolve" );
  talents.drain_thoughts        = find_talent( "Drain Thoughts" );
  talents.focused_insight       = find_talent( "Focused Insight" );
  talents.critical_kinesis      = find_talent( "Critical Kinesis" );
  talents.force_in_balance      = find_talent( "Force in Balance" );
  talents.psychic_barrier       = find_talent( "Psychic Barrier" );
  talents.telekinetic_balance   = find_talent( "Telekinetic Balance" );
  talents.containment           = find_talent( "Containment" );
  talents.mind_ward             = find_talent( "Mind Ward" );
  talents.presence_of_mind      = find_talent( "Presence of Mind" );
  talents.force_suppression     = find_talent( "Force Suppression" );
  talents.rippling_force        = find_talent( "Rippling Force" );
  talents.mind_warp             = find_talent( "Mind Warp" );
  talents.mental_scarring       = find_talent( "Mental Scarring" );
  talents.psychic_absorption    = find_talent( "Psychic Absorption" );
  talents.telekinetic_focal_point = find_talent( "Telekinetic Focal Point" );
  talents.metaphysical_alacrity = find_talent( "Metaphysical Alacrity" );
  talents.mental_defense        = find_talent( "Mental Defense" );
  talents.sever_force           = find_talent( "Sever Force" );

  set_base_accuracy( get_base_accuracy() + 0.01 * talents.inner_strength -> rank() );
}

// class_t::init_base ===============================================

void class_t::init_base()
{
  base_t::init_base();

  distance = default_distance = 30;

  resource_base[ RESOURCE_FORCE ] += 400 + talents.mental_longevity -> rank() * 50;
  attribute_multiplier_initial[ ATTR_WILLPOWER ] += talents.will_of_the_jedi -> rank() * 0.03;
}

// class_t::init_benefits ===========================================

void class_t::init_benefits()
{
  base_t::init_benefits();

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

// class_t::init_buffs ==============================================

void class_t::init_buffs()
{
  base_t::init_buffs();

  // buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rngs.type=rngs.CYCLIC, activated=true )
  // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rngs.type=rngs.CYCLIC, activated=true )
  // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rngs.type=rngs.CYCLIC, activated=true )

  bool is_sage = ( type == JEDI_SAGE );

  buffs.concentration = new buff_t( this, is_sage ? "concentration" : "subversion", 3, from_seconds( 10.0 ), timespan_t::zero(), 0.5 * talents.concentration -> rank() );
  buffs.telekinetic_focal_point = new buff_t( this, is_sage ? "telekinetic_focal_point" : "focal_lightning", 4, from_seconds( 15.0 ), timespan_t::zero()); 
  buffs.psychic_projection = new buff_t( this, is_sage ? "psychic_projection" : "lightning_barrage", 1, from_seconds( 10 ), from_seconds( 10.0 ), 0.5 * talents.psychic_projection -> rank() );
  buffs.tidal_force = new buff_t( this, is_sage ? "tidal_force" : "lightning_storm", 1, from_seconds( 30 ), from_seconds( 10.0 ) );
  buffs.telekinetic_effusion = new buff_t( this, is_sage ? "telekinetic_effusion" : "lightning_effusion", 2, from_seconds( 30 ), timespan_t::zero(), 0.5 * talents.telekinetic_effusion -> rank() );
  buffs.tremors = new buff_t( this, is_sage ? "tremors" : "conduction", 3, from_seconds( 30.0 ) );
  buffs.presence_of_mind = new buff_t( this, is_sage ? "presence_of_mind" : "wrath", 1, from_seconds( 30 ), timespan_t::zero(), talents.presence_of_mind -> rank() * 0.3 );
  buffs.force_suppression = new buff_t( this, is_sage ? "force_suppression" : "deathmark", 10, from_seconds( 30.0 ), timespan_t::zero(), talents.force_suppression -> rank() );
  buffs.mental_alacrity = new buff_t( this, is_sage ? "mental_alacrity" : "polarity_shift", 1, talents.flowing_force -> rank() ? from_seconds( 15.0 ) : from_seconds( 10.0 ));
  buffs.force_potency = new buff_t( this, is_sage ? "force_potency" : "recklessness",
                                     set_bonus.battlemaster_stalkers -> four_pc() ? 3 : 2

                                    , from_seconds( 20.0 ) );
  buffs.psychic_projection_dd = new buff_t( this, is_sage ? "psychic_projection_dd" : "lightning_barrage_dd", 1, from_seconds( 2.0 ), timespan_t::zero() );
  buffs.rakata_force_masters_4pc = new buff_t( this, "rakata_force_masters_4pc", 1, from_seconds( 15.0 ), from_seconds( 20.0 ), set_bonus.rakata_force_masters -> four_pc() ? 0.10 : 0.0 );
  buffs.underworld_force_masters_4pc = new buff_t( this, "underworld_force_masters_4pc", 1, from_seconds( 15.0 ), from_seconds( 20.0 ), set_bonus.underworld_force_masters -> four_pc() ? 0.30 : 0.0 );
  buffs.noble_sacrifice = new buff_t( this, "noble_sacrifice", 4, from_seconds( 10.0 ) );
  buffs.resplendence = new buff_t( this, is_sage ? "resplendence" : "force_surge", 1 , from_seconds( 10 ), timespan_t::zero(), talents.resplendence -> rank() / 2.0 );
  buffs.conveyance = new buff_t( this, is_sage ? "conveyace" : "force_bending", 1, from_seconds( 10 ), timespan_t::zero(), talents.conveyance -> rank() / 2.0 );
}

// class_t::init_gains ==============================================

void class_t::init_gains()
{
  base_t::init_gains();

  bool is_sage = ( type == JEDI_SAGE );

  gains.concentration   = get_gain( is_sage ? "concentration" : "subversion" );
  gains.focused_insight = get_gain( is_sage ? "focused_insight" : "parasitism" );
  gains.psychic_barrier = get_gain( is_sage ? "psychic_barrier" : "sith_efficacy" );
  gains.noble_sacrifice_health = get_gain( is_sage ? "noble_sacrifice_health" : "consumption_health" );
  gains.noble_sacrifice_power = get_gain( is_sage ? "noble_sacrifice_power" : "consumption_power" );
  gains.noble_sacrifice_power_regen_lost = get_gain( is_sage ? "noble_sacrifice_power_regen_lost" : "consumption_power_regen_lost" );
  gains.rippling_force = get_gain ( is_sage ? "rippling_force" : "lightning_burns" );
}

// class_t::init_procs ==============================================

void class_t::init_procs()
{
  base_t::init_procs();
}

// class_t::init_rngs ===============================================

void class_t::init_rng()
{
  base_t::init_rng();

  rngs.psychic_barrier = get_rng( "psychic_barrier" );
  rngs.upheaval = get_rng( "upheaval" );
  rngs.tm = get_rng( "telekinetic_momentum" );
  rngs.mm = get_rng( "mental_momentum" );
  rngs.tremors = get_rng( "tremors" );
  rngs.psychic_projection_dd = get_rng( type == JEDI_SAGE ? "psychic_projection_dd" : "lightning_barrage_dd" );
  rngs.rippling_force = get_rng( "rippling_force" );
}

// class_t::init_actions ============================================

void class_t::init_actions()
{
  //=========================================================================
  //
  //   Please Mirror all changes between Empire and Republic.
  //
  //=========================================================================

  if ( action_list_str.empty() )
  {
    if ( type == JEDI_SAGE )
    {
      action_list_str += "stim,type=prototype_nano_infused_resolve";
      action_list_str += "/force_valor";
      action_list_str += "/snapshot_stats";

      switch ( primary_tree() )
      {
      case TREE_BALANCE:

      if ( talents.psychic_projection -> rank() )
        action_list_str += "/telekinetic_throw,if=buff.psychic_projection_dd.up";

      action_list_str += "/weaken_mind,if=!ticking";
      action_list_str += "/power_potion";
      action_list_str += "/use_relics";

      action_list_str += "/mental_alacrity";
      action_list_str += "/force_potency,if=cooldown.force_in_balance.remains<2.7";
      action_list_str +="/force_in_balance,if=buff.force_potency.up";
      

      if ( talents.force_in_balance -> rank() > 0 )
        action_list_str += "/force_in_balance,if=force>80";

      if ( talents.sever_force -> rank() > 0 )
        action_list_str += "/sever_force,if=!ticking";

      action_list_str +="/telekinetic_throw,if=buff.force_potency.up";

      if ( talents.presence_of_mind -> rank() ){
        action_list_str += "/mind_crush,if=buff.presence_of_mind.react";
        action_list_str += "/disturbance,if=buff.presence_of_mind.react";
      }

      if ( ! talents.presence_of_mind -> rank() )
        action_list_str += "/mind_crush";

      action_list_str += "/telekinetic_throw";
      action_list_str += "/project,moving=1,if=force>120";

      break;


      case TREE_TELEKINETICS:

      action_list_str += "/weaken_mind,if=!ticking";
      action_list_str += "/power_potion";
      action_list_str += "/use_relics";
      action_list_str += "/mental_alacrity";

      if ( talents.turbulence -> rank() > 0 )
        action_list_str += "/turbulence,if=dot.weaken_mind.remains>cast_time";

      action_list_str += "/force_potency,if=buff.psychic_projection.react|buff.tidal_force.react";

      if ( talents.psychic_projection -> rank() > 0)
        action_list_str += "/telekinetic_throw,if=buff.psychic_projection.react";

      if ( talents.telekinetic_wave -> rank() > 0 && talents.tidal_force -> rank() > 0 )
        action_list_str += "/telekinetic_wave,if=buff.tidal_force.react";

      action_list_str += "/disturbance,if=buff.mental_alacrity.react";
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
      action_list_str += "stim,type=prototype_nano_infused_resolve";
      action_list_str += "/mark_of_power";
      action_list_str += "/snapshot_stats";

      switch ( primary_tree() )
      {
      case TREE_MADNESS:

      if ( talents.psychic_projection -> rank() )
        action_list_str += "/force_lightning,if=buff.lightning_barrage_dd.up";

      action_list_str += "/affliction,if=!ticking";
      action_list_str += "/power_potion";
      action_list_str += "/use_relics";

      action_list_str += "/polarity_shift";
      action_list_str += "/recklessness,if=cooldown.death_field.remains<2.7";
      action_list_str +="/death_field,if=buff.recklessness.up";
      

      if ( talents.force_in_balance -> rank() > 0 )
        action_list_str += "/death_field,if=force>80";

      if ( talents.sever_force -> rank() > 0 )
        action_list_str += "/creeping_terror,if=!ticking";

      action_list_str +="/force_lightning,if=buff.recklessness.up";

      if ( talents.presence_of_mind -> rank() ){
        action_list_str += "/crushing_darkness,if=buff.wrath.react";
        action_list_str += "/lightning_strike,if=buff.wrath.react";
      }


      if ( ! talents.presence_of_mind -> rank() )
        action_list_str += "/crushing_darkness";

      action_list_str += "/force_lightning";
      action_list_str += "/shock,moving=1,if=force>120";

      break;


      case TREE_LIGHTNING:

      action_list_str += "/affliction,if=!ticking";
      action_list_str += "/power_potion";
      action_list_str += "/use_relics";
      action_list_str += "/polarity_shift";

      if ( talents.turbulence -> rank() > 0 )
        action_list_str += "/thundering_blast,if=dot.affliction.remains>cast_time";

      action_list_str += "/recklessness,if=buff.lightning_barrage.react|buff.lightning_storm.react";

      if ( talents.psychic_projection -> rank() > 0)
        action_list_str += "/force_lightning,if=buff.lightning_barrage.react";

      if ( talents.telekinetic_wave -> rank() > 0 && talents.tidal_force -> rank() > 0 )
        action_list_str += "/chain_lightning,if=buff.lightning_storm.react";

      action_list_str += "/lightning_strike,if=buff.polarity_shift.react";
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

  base_t::init_actions();
}

// class_t::primary_role ============================================

role_type class_t::primary_role() const
{
  switch ( base_t::primary_role() )
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

// class_t::force_regen_per_second ==================================

double class_t::force_regen_per_second() const
{
  double regen = base_t::force_regen_per_second();
  //alacrity here is the modifier for cast time, i.e. something like 93%, in that case we want regen to go up by 7%
  regen += base_regen_per_second * ( buffs.concentration -> check() * 0.10 -
                                     buffs.noble_sacrifice -> check() * 0.25  + (alacrity()-1));
  return regen;
}

// class_t::regen ===================================================

void class_t::regen( timespan_t periodicity )
{
  double force_regen = to_seconds( periodicity ) * base_regen_per_second;

  if ( buffs.concentration -> up() )
    resource_gain( RESOURCE_FORCE, force_regen * buffs.concentration -> check() * 0.10, gains.concentration );

  if ( buffs.noble_sacrifice -> up() )
    resource_loss( RESOURCE_FORCE, force_regen * buffs.noble_sacrifice -> check() * 0.25, gains.noble_sacrifice_power_regen_lost );

  resource_gain( RESOURCE_FORCE, force_regen * (alacrity()-1));
  base_t::regen( periodicity );
}

// class_t::force_bonus_multiplier ==================================

double class_t::force_bonus_multiplier() const
{
  return base_t::force_bonus_multiplier() +
      buffs.tremors -> stack() * 0.01;
}

double class_t::force_healing_bonus_multiplier() const
{
  return base_t::force_healing_bonus_multiplier() +
      talents.clairvoyance -> rank() * 0.02;
}

// class_t::alacrity ================================================

double class_t::alacrity() const
{
  double sh = base_t::alacrity();

  sh += buffs.mental_alacrity -> stack() * 0.20;

  sh += talents.force_gift -> rank() * 0.01;

  sh += buffs.telekinetic_focal_point -> stack() * 0.01;

  return sh;
}

// class_t::create_talents ==========================================

void class_t::create_talents()
{
  // Seer|Corruption
  static const talentinfo_t seer_tree[] = {
      { "Force Gift", 2}, { "Penetrating Light", 3}, { "Psychic Suffusion", 2},
      { "Foresight", 1}, { "Pain Bearer", 2}, { "Immutable Force", 2}, { "Humility", 1},
      { "Conveyance", 2}, { "Rejuvenate", 1}, { "Preservation", 2}, { "Valiance", 2},
      { "Mend Wounds", 1}, { "Force Shelter", 2}, { "Egress", 2},
      { "Confound", 2}, { "Healing Trance", 1}, { "Life Ward", 2}, { "Serenity", 2},
      { "Resplendence", 2}, { "Clairvoyance", 3},
      { "Master Speed", 1}, { "Force Warden", 3}, { "Amnesty", 1}, { "Salvation", 1}
  };
  init_talent_tree( JEDI_SAGE_SEER, seer_tree );

  // Telekinetics|Lightning
  static const talentinfo_t telekinetics_tree[] = {
      { "Upheaval", 3}, { "Mental Longevity", 2}, { "Inner Strength", 3},
      { "Mind's Eye", 1}, { "Clamoring Force", 2}, { "Concentration", 2}, { "Telekinetic Defense", 2},
      { "Psychic Projection", 2}, { "Telekinetic Wave", 1}, { "Telekinetic Effusion", 2},
      { "Cascading Force", 2}, { "Tidal Force", 1}, { "Kinetic Collapse", 2},
      { "Tremors", 2}, { "Telekinetic Momentum", 3}, { "Blockout", 2},
      { "Force Wake", 2}, { "Flowing Force", 1}, { "Reverberation", 2},
      { "Force Haste", 2}, { "Mental Momentum", 3},
      { "Turbulence", 1}
  };
  init_talent_tree( JEDI_SAGE_TELEKINETICS, telekinetics_tree );

   // Balance|Madness
  static const talentinfo_t balance_tree[] = {
      { "Empowered Throw", 3}, { "Jedi Resistance", 2}, { "Will of the Jedi", 2},
      { "Pinning Resolve", 2}, { "Drain Thoughts", 3}, { "Focused Insight", 2}, { "Critical Kinesis", 2},
      { "Force in Balance", 1}, { "Psychic Barrier", 3}, { "Telekinetic Balance", 1},
      { "Containment", 2}, { "Mind Ward", 2}, { "Presence of Mind", 1},
      { "Force Suppression", 1}, { "Rippling Force", 2}, { "Mind Warp", 2},
      { "Mental Scarring", 3}, { "Psychic Absorption", 2},
      { "Telekinetic Focal Point", 2}, { "Metaphysical Alacrity", 1}, { "Mental Defense", 2},
      { "Sever Force", 1}
  };
  init_talent_tree( JEDI_SAGE_BALANCE, balance_tree );
}

// class_t::create_options ==========================================

void class_t::create_options()
{
  base_t::create_options();

  option_t sage_sorcerer_options[] =
  {
    { "disable_double_dip", OPT_BOOL, &( disable_double_dip ) },
    { NULL, OPT_UNKNOWN, NULL }
  };

  option_t::copy( options, sage_sorcerer_options );
}

} // namespace sage_sorcerer ================================================

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

using sage_sorcerer::class_t;

// player_t::create_jedi_sage  ==============================================

player_t* player_t::create_jedi_sage( sim_t* sim, const std::string& name, race_type r )
{
  return new class_t( sim, JEDI_SAGE, name, r );
}

// player_t::create_sith_sorcerer  ==========================================

player_t* player_t::create_sith_sorcerer( sim_t* sim, const std::string& name, race_type r )
{
  return new class_t( sim, SITH_SORCERER, name, r );
}

// player_t::sage_sorcerer_init =============================================

void player_t::sage_sorcerer_init( sim_t* sim )
{
  for ( size_t i = 0; i < sim -> actor_list.size(); i++ )
  {
    player_t* p = sim -> actor_list[i];
    p -> buffs.force_valor = new buff_t( p, "force_valor_mark_of_power", 1 );
    p -> buffs.force_valor -> constant = true;
  }
}

// player_t::sage_sorcerer_combat_begin =====================================

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
