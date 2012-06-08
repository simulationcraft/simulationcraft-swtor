// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "agent_smug.hpp"

// ==========================================================================
// Gunslinger | Sniper
// ==========================================================================

namespace { // ANONYMOUS ====================================================


namespace gunslinger_sniper { // ============================================

class class_t;

class targetdata_t : public agent_smug::targetdata_t
{
public:
  targetdata_t( class_t& source, player_t& target );

  debuff_t* debuff_cluster_bombs;
  debuff_t* debuff_electrified_railgun;
  debuff_t* debuff_shatter_shot;
  dot_t dot_electrified_railgun;
  dot_t dot_interrogation_probe;
  dot_t dot_plasma_probe;
  dot_t dot_series_of_shots;
};

class class_t : public agent_smug::class_t
{
public:


  typedef agent_smug::class_t base_t;

  // Buffs
  struct buffs_t:base_t::buffs_t
  {
    buff_t* energy_overrides;
    buff_t* followthrough;
    buff_t* laze_target;
    buff_t* rapid_fire;
    buff_t* snap_shot;
    buff_t* sniper_volley;
    buff_t* stroke_of_genius;
    buff_t* target_acquired;
  } buffs;

  // Gains
  struct gains_t:base_t::gains_t
  {
    gain_t* imperial_methodology;
    gain_t* snipers_nest;
    gain_t* sniper_volley;
    gain_t* target_acquired;
  } gains;

  // Procs
  struct procs_t:base_t::procs_t
  {
  } procs;

  // RNGs
  struct rngs_t:base_t::rngs_t
  {
    rng_t* reactive_shot;
  } rngs;

  // Benefits
  struct benefits_t:base_t::benefits_t
  {
  } benefits;

  // Cooldowns
  struct cooldowns_t
  {
    cooldown_t* ambush;
    cooldown_t* adrenaline_probe;
    cooldown_t* interrogation_probe;
    cooldown_t* series_of_shots;
  } cooldowns;

  // Talents
  struct talents_t:base_t::talents_t
  {
    // Marksmanship|Sharpshooter
    // t1
    talent_t* cover_screen;
    talent_t* steady_shots;
    talent_t* marksmanship;
    // t2
    talent_t* heavy_shot;
    talent_t* ballistic_dampers;
    talent_t* precision_ambush;
    talent_t* imperial_demarcation;
    // t3
    talent_t* snap_shot;
    talent_t* diversion;
    talent_t* reactive_shot;
    // t4
    talent_t* between_the_eyes;
    talent_t* sector_ranger;
    talent_t* sniper_volley;
    talent_t* snipers_nest;
    // t5
    talent_t* recoil_control;
    talent_t* followthrough;
    talent_t* pillbox_sniper;
    // t6
    talent_t* imperial_assassin;
    talent_t* siege_bunker;
    // t7
    talent_t* rapid_fire;

    // Engineering|Saboteur
    // t1
    talent_t* engineers_tool_belt;
    talent_t* energy_tanks;
    talent_t* gearhead;
    // t2
    talent_t* explosive_engineering;
    talent_t* vital_regulators;
    talent_t* stroke_of_genius;
    talent_t* vitality_serum;
    // t3
    talent_t* cluster_bombs;
    talent_t* efficient_engineering;
    talent_t* interrogation_probe;
    talent_t* inventive_interrogation_techniques;
    // t4
    talent_t* imperial_methodology;
    talent_t* calculated_pursuit;
    talent_t* experimental_explosives;
    // t5
    talent_t* energy_overrides;
    talent_t* emp_discharge;
    talent_t* augmented_shields;
    // t6
    talent_t* deployed_shields;
    talent_t* electrified_railgun;
    // t7
    talent_t* plasma_probe;
  } talents;

  // Abilities
  struct abilities_t:base_t::abilities_t
  {
    std::string ambush;
    std::string cover_pulse;
    std::string emp_discharge;
    std::string followthrough;
    std::string interrogation_probe;
    std::string laze_target;
    std::string plasma_probe;
    std::string rapid_fire;
    std::string series_of_shots;
    std::string takedown;
    std::string target_acquired;
    std::string shatter_shot;

    // buffs
    std::string cluster_bombs;
    std::string energy_overrides;
    std::string snap_shot;
    std::string sniper_volley;
    std::string stroke_of_genius;
    // rngs from talent
    std::string reactive_shot;
    // gains from talent
    std::string imperial_methodology;
    std::string snipers_nest;
    // talent dot
    std::string electrified_railgun;
  } abilities;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    base_t( sim, pt == IA_SNIPER ? IA_SNIPER : S_GUNSLINGER, name, rt, buffs, gains, procs, rngs, benefits, talents, abilities ),
    buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents()
  {
    tree_type[ IA_SNIPER_MARKSMANSHIP ] = TREE_MARKSMANSHIP;
    tree_type[ IA_SNIPER_ENGINEERING  ] = TREE_ENGINEERING;
    tree_type[ IA_SNIPER_LETHALITY    ] = TREE_LETHALITY;

    create_talents();
    create_options();
  }

  // Character Definition
  double    _tech_range_accuracy;
  targetdata_t* new_targetdata( player_t& target ) // override
  { return new targetdata_t( *this, target ); }
  ::action_t* create_action( const std::string& name, const std::string& options );
  void      init_talents();
  void      init_abilities();
  void      init_base();
  void      init_benefits();
  void      init_buffs();
  void      init_gains();
  void      init_procs();
  void      init_cooldowns();
  void      init_rng();
  void      init_actions();
  role_type primary_role() const;
  double    tech_accuracy_chance() const;
  double    range_accuracy_chance() const;
  void      create_talents();

  double    energy_regen_per_second() const;
  void      regen( timespan_t periodicity );
  int       talented_energy() const;
  double    alacrity() const;
  double    armor_penetration() const;

  void      _trigger_reactive_shot();
};

targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  agent_smug::targetdata_t( source, target ),
  debuff_cluster_bombs        ( new debuff_t ( this, source.abilities.cluster_bombs, 2 + source.talents.imperial_methodology -> rank(), from_seconds ( 0 ), from_seconds( 1.5 ), 100, false, true /*reverse*/ ) ),
  debuff_electrified_railgun  ( new debuff_t ( this, source.abilities.electrified_railgun, 4, from_seconds ( 5 ), from_seconds( 0 ), 1 * source.talents.electrified_railgun -> rank() ) ),
  debuff_shatter_shot         ( new debuff_t ( this, source.abilities.shatter_shot, 1, from_seconds ( 45 ) ) ),
  dot_electrified_railgun ( source.abilities.electrified_railgun, &source ),
  dot_interrogation_probe ( source.abilities.interrogation_probe, &source ),
  dot_plasma_probe        ( source.abilities.plasma_probe       , &source ),
  dot_series_of_shots     ( source.abilities.series_of_shots    , &source )
{
  add( *debuff_cluster_bombs       );
  add( *debuff_electrified_railgun );
  add( *debuff_shatter_shot        );

  add( dot_electrified_railgun     );
  add( dot_interrogation_probe     );
  add( dot_plasma_probe            );
  add( dot_series_of_shots         );
}

struct action_t : public agent_smug::action_t
{
  typedef agent_smug::action_t base_t;

  action_t( const std::string& n, class_t* player, attack_policy_t policy, resource_type r, school_type s ) :
    base_t( n, player, policy, r, s )
  {}

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( base_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }
};

struct attack_t : public agent_smug::attack_t
{
  typedef agent_smug::attack_t base_t;

  attack_t( const std::string& n, class_t* p, attack_policy_t policy, school_type s ) :
    base_t( n, p, policy, s )
  {}

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( base_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }
};

struct tech_attack_t : public agent_smug::tech_attack_t
{
  typedef agent_smug::tech_attack_t base_t;

  tech_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_KINETIC ) :
    base_t( n, p, s )
  {}

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( base_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }
};

struct range_attack_t : public agent_smug::range_attack_t
{
  typedef agent_smug::range_attack_t base_t;

  range_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_ENERGY ) :
    base_t( n, p, s )
  {}

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( base_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }
};

struct poison_attack_t : public agent_smug::poison_attack_t
{
  typedef agent_smug::poison_attack_t base_t;

  poison_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_INTERNAL ) :
    base_t( n, p, s )
  {}

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( base_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }
};

// ==========================================================================
// Gunslinger / Sniper Abilities
// ==========================================================================

// Ambush | Aimed Shot ======================================================
struct ambush_t : public range_attack_t
{
  typedef range_attack_t base_t;
  ambush_t* offhand_attack;

  static int energy_cost() { return 15; }

  ambush_t( class_t* p, const std::string& n, const std::string& options_str,
            bool is_offhand=false ) :
    base_t( n, p ),
    offhand_attack( NULL )
  {
    // todo: get this list right
    rank_level_list = { 8, 12, 16, 22, 31, 40, 50 };

    parse_options( options_str );

    base_cost                    = energy_cost();
    base_execute_time            = from_seconds( 2.5 );
    cooldown -> duration         = from_seconds( 15 );
    range                        = 35.0;
    dd.standardhealthpercentmin  =
        dd.standardhealthpercentmax  = 0.329;
    dd.power_mod                 = 3.29;
    weapon                       = &( player -> main_hand_weapon );
    weapon_multiplier            = 1.2;

    if ( is_offhand )
    {
      base_cost                  = 0;
      background                 = true;
      dual                       = true;
      base_execute_time          = timespan_t::zero();
      trigger_gcd                = timespan_t::zero();
      weapon                     = &( player -> off_hand_weapon );
      rank_level_list            = { 0 };
      dd.power_mod               = 0;
    }

    else if ( player -> type == S_GUNSLINGER && &( player -> off_hand_weapon ) )
    {
      offhand_attack             = new ambush_t( p, n+"_offhand", options_str, true );
      add_child( offhand_attack );
    }
  }

  virtual void player_buff()
  {
    base_t::player_buff();
    if ( p() -> talents.precision_ambush -> rank() )
      player_armor_penetration -= 0.1;
  }

  virtual void execute()
  {
    base_t::execute();
    if ( offhand_attack )
      offhand_attack -> schedule_execute();
    else if ( p() -> talents.followthrough -> rank() )
      p() -> buffs.followthrough -> trigger();
  }
};

// Cover Pulse | Pulse Detonator ============================================
struct cover_pulse_t : public action_t
{
  typedef action_t base_t;

  cover_pulse_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    parse_options( options_str );

    cooldown -> duration = from_seconds( 30 - 2.5 * p -> talents.hold_your_ground -> rank() );
    use_off_gcd = true;
    trigger_gcd = timespan_t::zero();
  }

  void execute()
  {
    base_t::execute();

    if ( p() -> talents.stroke_of_genius -> rank() )
      p() -> buffs.stroke_of_genius -> trigger();
  }
};

// Cull | Wounding Shot =====================================================

struct cull_extra_t : public agent_smug::cull_extra_t
{
  typedef agent_smug::cull_extra_t base_t;

  cull_extra_t( class_t* p, const std::string& n ) :
    base_t( p, n )
  {
    range                       = 30;
    dd.standardhealthpercentmin =
    dd.standardhealthpercentmax =  0.037;
    dd.power_mod                =  0.37;
  }
};

struct cull_t : public agent_smug::cull_t
{
  typedef agent_smug::cull_t base_t;

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( agent_smug::cull_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }

  static int energy_cost() { return base_t::energy_cost(); }

  cull_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( p, n, options_str )
  {
    // TODO does RR benefit the extra attack as well?
    range                       = 30; // TODO check or 35?
    cooldown -> duration        = from_seconds( 9 );
    base_crit                   = 0.04 * p -> talents.razor_rounds -> rank();
    td.weapon                   =  &( player->main_hand_weapon );
    td.weapon_multiplier        = -0.64;
    td.standardhealthpercentmin =
    td.standardhealthpercentmax = 0.053;
    td.power_mod                = 0.533;
    channeled                   = true;
    num_ticks                   = 3;
    base_tick_time              = from_seconds(1);
  }

  agent_smug::cull_extra_t* get_extra_strike()
  {
    return new cull_extra_t( p(), name_str + "_extra" );
  }
};

// Explosive Probe | Sabotage Charge ========================================

struct explosive_probe_t : public agent_smug::explosive_probe_t
{
  typedef agent_smug::explosive_probe_t base_t;

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>(base_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }

  static int energy_cost( class_t* p ) { return base_t::energy_cost() - 2 * p -> talents.efficient_engineering -> rank(); }

  explosive_probe_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( p, n, options_str )
  {
    base_cost        = energy_cost( p );
    base_multiplier += 0.05 * p -> talents.explosive_engineering -> rank();
  }

  // TODO: explosive probe "attaches" to the target and detonates on damage
  // (tooltip says damage. game data has text that says blaster damage)
  // this is not implemented yet.

  virtual double cost() const
  {
    return p() -> buffs.energy_overrides -> up()
        ? base_t::cost() * ( 1 - 0.5 * p() -> talents.energy_overrides -> rank() )
        : base_t::cost();
  }

  virtual void execute()
  {
    base_t::execute();

    p() -> buffs.energy_overrides -> expire();
    if ( p() -> talents.cluster_bombs && result_is_hit() )
    {
      buff_t* cluster_bombs = targetdata() -> debuff_cluster_bombs;
      if ( cluster_bombs -> up() )
        // REVIEW for normal buffs would just use trigger, but this is in reverse. so trigger decrements
        // once it is up. how to best get it up to max stacks?
        cluster_bombs -> refresh(cluster_bombs -> max_stack);
      else
        cluster_bombs -> start(cluster_bombs -> max_stack);
    }
  }
};

// EMP Discharge | Sabotage =================================================
struct emp_discharge_t : public tech_attack_t
{
  typedef tech_attack_t base_t;

  emp_discharge_t( class_t* p, const std::string& n, const std::string& options_str) :
    base_t( n, p, SCHOOL_ENERGY )
  {
    check_talent( p -> talents.emp_discharge -> rank() );

    // rank_level_list = { ... ,50 };

    parse_options( options_str );

    base_cost                   = 0;
    cooldown -> duration        = from_seconds( 60 );
    range                       = 60.0;
    dd.standardhealthpercentmin = 0.07;
    dd.standardhealthpercentmax = 0.13;
    dd.power_mod                = 1;
    use_off_gcd                 = true;
    trigger_gcd                 = timespan_t::zero();
  }

  virtual bool ready()
  {
    return targetdata() -> dot_interrogation_probe.ticking ? base_t::ready() : false;
  }

  virtual void execute()
  {
    // REVIEW: or reset()?
    targetdata() -> dot_interrogation_probe.cancel();
    p() -> cooldowns.interrogation_probe -> reset();
    p() -> cooldowns.adrenaline_probe -> reset();
    if ( p() -> talents.energy_overrides -> rank() )
      p() -> buffs.energy_overrides -> trigger();

    base_t::execute();
  }
};


// Followthrough | Trickshot ================================================
struct followthrough_t : public range_attack_t
{
  typedef range_attack_t base_t;

  static int energy_cost( class_t* p ) { return 10 - 2 * p -> talents.recoil_control -> rank(); }

  followthrough_t( class_t* p, const std::string& n, const std::string& options_str) :
    base_t( n, p )
  {
    check_talent( p -> talents.followthrough -> rank() );

    // rank_level_list = { ... ,50 };

    parse_options( options_str );

    base_cost                    = energy_cost( p );
    cooldown -> duration         = from_seconds( 9 - 1.5 * p -> talents.recoil_control -> rank() );
    range                        = 35.0;
    dd.standardhealthpercentmin  =
    dd.standardhealthpercentmax  = 0.227;
    dd.power_mod                 = 2.27;
    weapon                       = &( player -> main_hand_weapon );
    weapon_multiplier            = 0.51;
    base_crit                   += 0.02 * p -> talents.between_the_eyes -> rank();
    crit_multiplier             += 0.1  * p -> talents.imperial_assassin -> rank();
  }

  virtual bool ready()
  {
    return p() -> buffs.followthrough -> up() ? base_t::ready() : false;
  }
};

// Interrogation Probe | Shock Charge =======================================
struct interrogation_probe_t : public tech_attack_t
{
  typedef tech_attack_t base_t;

  static int energy_cost( class_t* p ) { return 20 - 2 * p -> talents.efficient_engineering -> rank(); }

  interrogation_probe_t( class_t* p, const std::string& n, const std::string& options_str) :
    base_t( n, p, SCHOOL_ENERGY )
  {
    check_talent( p -> talents.interrogation_probe -> rank() );

    // rank_level_list = { ... ,50 };

    parse_options( options_str );

    base_cost                    = energy_cost( p );
    cooldown -> duration         = from_seconds( 18 );
    range                        = 35.0;
    td.standardhealthpercentmin  =
    td.standardhealthpercentmax  = 0.055;
    td.power_mod                 = 0.55;
    tick_zero                    = true;
    num_ticks                    = 6;
    base_tick_time               = from_seconds( 3.0 );
    base_crit                   += 0.04 * p -> talents.lethal_dose -> rank();

  }

  // not implemented: cooldown resets if ends prematurely (target dies, dispelled).
};

// Laze Target | Smugglers Luck =============================================
struct laze_target_t : public action_t
{
  typedef action_t base_t;

  laze_target_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    parse_options( options_str );

    cooldown -> duration = from_seconds( 60 );
    // TODO find out if this triggers GCD, and if it can be used off GCD
    use_off_gcd = true;
    trigger_gcd = timespan_t::zero();
  }

  void execute()
  {
    base_t::execute();
    p() -> buffs.laze_target -> trigger();
  }
};


// Plasma Probe | Incendiary Grenade ========================================
struct plasma_probe_t : public tech_attack_t
{
  typedef tech_attack_t base_t;

  static int energy_cost( class_t* p ) { return 20 - 2 * p -> talents.efficient_engineering -> rank(); }

  plasma_probe_t* weaker_attack;

  plasma_probe_t( class_t* p, const std::string& n, const std::string& options_str,
     bool is_weaker_attack=false ) :
    base_t( n, p, SCHOOL_ELEMENTAL ),
    weaker_attack( 0 )
  {
    check_talent( p -> talents.plasma_probe -> rank() );

    // rank_level_list = { ... ,50 };
    parse_options( options_str );

    base_cost                    = energy_cost( p );
    cooldown -> duration         = from_seconds( 18 );
    range                        = 35.0;
    td.standardhealthpercentmin  =
    td.standardhealthpercentmax  = 0.044;
    td.power_mod                 = 0.44;
    tick_zero                    = true;
    num_ticks                    = 3;
    base_tick_time               = from_seconds( 1.0 );
    base_multiplier             += 0.05 * p -> talents.explosive_engineering -> rank();
    base_crit                   += 0.04 * p -> talents.lethal_dose -> rank();
    if ( is_weaker_attack)
    {
      base_accuracy                = 999;
      background                   = true;
      trigger_gcd                  = timespan_t::zero();
      td.standardhealthpercentmax  = 0.02;
      td.power_mod                 = 0.2;
      tick_zero                    = false;
      num_ticks                    = 5;
      base_tick_time               = from_seconds( 3.0 );
    }
    else
      weaker_attack = new plasma_probe_t(p, n, options_str, true);
  }

  virtual double cost() const
  {
    return p() -> buffs.energy_overrides -> up()
      ? base_t::cost() * ( 1 - 0.5 * p() -> talents.energy_overrides -> rank() )
      : base_t::cost();
  }

  virtual void execute()
  {
    base_t::execute();
    p() -> buffs.energy_overrides -> expire();
  }

  virtual void last_tick(dot_t* d)
  {
    base_t::last_tick( d );
    if ( weaker_attack )
      weaker_attack -> execute();
  }
};

// Rapid Fire | Rapid Fire ==================================================
struct rapid_fire_t : public action_t
{
  typedef action_t base_t;

  rapid_fire_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    check_talent( p -> talents.rapid_fire -> rank() );

    parse_options( options_str );

    cooldown -> duration = from_seconds( 90 );
    use_off_gcd = true;
    trigger_gcd = timespan_t::zero();
  }

  void execute()
  {
    base_t::execute();

    p() -> buffs.rapid_fire -> trigger();
    p() -> cooldowns.series_of_shots -> reset();
  }
};


// Orbital Strike | XS Freighter Flyby ======================================
struct orbital_strike_t : public agent_smug::orbital_strike_t
{
  typedef agent_smug::orbital_strike_t base_t;

  orbital_strike_t( class_t* p, const std::string& n, const std::string& options_str) :
      base_t( p, n, options_str )
  {
    cooldown -> duration -= from_seconds( 7.5  * p -> talents.pillbox_sniper          -> rank() );
    cooldown -> duration -= from_seconds( 1    * p -> talents.sector_ranger           -> rank() );
    base_multiplier      +=               0.05 * p -> talents.explosive_engineering   -> rank();
    crit_multiplier      +=               0.15 * p -> talents.experimental_explosives -> rank();
  }
};

// Series of Shots | Speed Shot =============================================
struct series_of_shots_t : public tech_attack_t
{
  struct electrified_railgun_t : public tech_attack_t
  {
    typedef tech_attack_t base_t;

    electrified_railgun_t( class_t* p, const std::string& n ) :
      base_t( n, p, SCHOOL_ELEMENTAL )
    {
      background                  = true;
      trigger_gcd                 = timespan_t::zero();
      td.standardhealthpercentmin =
        td.standardhealthpercentmax = 0.005;
      td.power_mod                = 0.05;
      // ticks crit, invisible hit not
      may_crit                    = false;
      tick_zero                   = true;
      num_ticks                   = 3;
      base_tick_time              = from_seconds( 1.1 ); // using 1.1 to avoid tick overlaps while series of shots is going
    }

    virtual void player_buff()
    {
      base_t::player_buff();
      int stack = targetdata() -> debuff_electrified_railgun -> stack();
      assert( stack );
      player_multiplier *= stack;
    }

    virtual void execute()
    {
      targetdata() -> debuff_electrified_railgun -> trigger();
      base_t::execute();
    }
  };

  typedef tech_attack_t base_t;
  electrified_railgun_t* electrified;

  static int energy_cost() { return 20; }

  series_of_shots_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( n, p, SCHOOL_ENERGY ),
    electrified( 0 )
  {
    // rank_level_list = { ... , 50 };

    parse_options( options_str );

    base_cost                    = energy_cost();
    range                        = 35.0;
    cooldown -> duration         = from_seconds( 15.0 );
    channeled                    = true;
    tick_zero                    = true;
    num_ticks                    = 3;
    base_tick_time               = from_seconds( 1 );
    td.standardhealthpercentmin  =
    td.standardhealthpercentmax  = 0.098;
    td.weapon                    = &( player -> main_hand_weapon );
    td.power_mod                 = 0.98;
    td.weapon_multiplier         = -0.348;
    // tick crits. the invisible execute hit shouldn't
    may_crit                     = false;
    base_multiplier             += 0.03 * p -> talents.steady_shots      -> rank();
    base_multiplier             += 0.03 * p -> talents.cut_down          -> rank();
    base_crit                   += 0.02 * p -> talents.between_the_eyes  -> rank();
    crit_multiplier             += 0.1  * p -> talents.imperial_assassin -> rank();
    if ( p -> talents.electrified_railgun -> rank() )
    {
      electrified = new electrified_railgun_t( p, p -> abilities.electrified_railgun );
      add_child(electrified);
    }
  }

  virtual void tick( dot_t* d )
  {
    base_t::tick( d );

    if ( electrified )
      electrified -> schedule_execute();

    if ( result == RESULT_CRIT )
      p() -> _trigger_reactive_shot();
  }

  virtual void execute()
  {
    base_t::execute();

    buff_t* rapid_fire = p() -> buffs.rapid_fire;
    if ( rapid_fire -> up() )
    {
      rapid_fire -> expire();
      cooldown -> reset();
    }
  }

  // TODO talent: electrified raligun has 1/3 2/3 3/3 chance on damage to
  // deal elemental damage over 4 seconds, stacking up to 4 times.
  // need to determine mechanics of the stack.
};

// Snipe | Charged Burst ====================================================
struct snipe_t : public agent_smug::snipe_t
{
  typedef agent_smug::snipe_t base_t;

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( base_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }

  snipe_t( class_t* p, const std::string& n, const std::string& options_str) :
    base_t( p, n, options_str )
  {
    base_multiplier += 0.03 * p -> talents.steady_shots     -> rank();
    base_crit       += 0.02 * p -> talents.between_the_eyes -> rank();
  }

  virtual timespan_t execute_time() const
  {
    if ( p() -> buffs.snap_shot -> check() || p() -> buffs.stroke_of_genius-> check() )
      return timespan_t::zero();

    return base_t::execute_time();
  }

  virtual void player_buff()
  {
    if ( p() -> buffs.laze_target -> up() )
      base_crit += 1; // 100% crit
  }

  virtual void execute()
  {
    base_t::execute();

    // TODO test behavior if both snap_shot and stroke_of_genius are up, or can be up, etc
    if ( p() -> buffs.snap_shot -> up() )
      p() -> buffs.snap_shot -> expire();
    if ( p() -> buffs.stroke_of_genius -> up() )
      p() -> buffs.stroke_of_genius -> expire();

    if ( p() -> buffs.laze_target -> up() )
      p() -> buffs.laze_target -> expire();

    if ( p() -> talents.followthrough -> rank() )
      p() -> buffs.followthrough -> trigger();

    if ( result == RESULT_CRIT )
      p() -> _trigger_reactive_shot();
  }
};

// Corrosive Grenade | Shrap Bomb ===========================================
struct corrosive_grenade_t : public agent_smug::corrosive_grenade_t
{
  typedef agent_smug::corrosive_grenade_t base_t;

  corrosive_grenade_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( p, n, options_str )
  {
    base_multiplier      +=               0.05    * p -> talents.explosive_engineering   -> rank();
    base_multiplier      +=               0.04    * p -> talents.targeted_demolition     -> rank();
    //crit_multiplier      +=               0.15    * p -> talents.experimental_explosives -> rank();
    // explosive engineering also benefits corrosive grenade, but not experimental explosives
    // http://www.swtor.com/community/showthread.php?p=2268473
  }
};

// Fragmentation Grenade | Thermal Grenade ==================================
struct fragmentation_grenade_t : public agent_smug::fragmentation_grenade_t
{
  typedef agent_smug::fragmentation_grenade_t base_t;

  fragmentation_grenade_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( p, n, options_str )
  {
    cooldown -> duration  = from_seconds( 6 - 1.5 * p -> talents.engineers_tool_belt     -> rank() );
    base_multiplier      +=               0.05    * p -> talents.explosive_engineering   -> rank();
    base_multiplier      +=               0.04    * p -> talents.targeted_demolition     -> rank();
    crit_multiplier      +=               0.15    * p -> talents.experimental_explosives -> rank();
  }
};

// TODO steady_shots also affects Cull, but we still need to move that from
// scoundrel to agent


// Take Cover | Take Cover ==================================================
struct take_cover_t : public agent_smug::take_cover_t
{
  typedef agent_smug::take_cover_t base_t;

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( base_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }

  take_cover_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( p, n, options_str )
  {}

  virtual void execute()
  {
    base_t::execute();
    if ( p() -> talents.snap_shot -> rank() && p() -> buffs.cover -> up() )
      p() -> buffs.snap_shot -> trigger();
  }
};

// Takedown | Quickdraw =====================================================
struct takedown_t : public range_attack_t
{
  typedef range_attack_t base_t;

  static int energy_cost( class_t* p ) { return 15 - ( p -> set_bonus.rakata_field_techs -> four_pc() ? 7 : 0 ); }

  takedown_t( class_t* p, const std::string& n, const std::string& options_str) :
    base_t( n, p )
  {
    // rank_level_list = { ... ,50 };

    parse_options( options_str );

    base_cost                    = energy_cost( p );
    cooldown -> duration         = from_seconds( 12 );
    range                        = 35.0;
    dd.standardhealthpercentmin  =
    dd.standardhealthpercentmax  = 0.27;
    dd.power_mod                 = 2.7;
    weapon                       = &( player -> main_hand_weapon );
    weapon_multiplier            = 0.8;
    crit_multiplier             += 0.1  * p -> talents.imperial_assassin -> rank();
  }

  virtual bool ready()
  {
    return target->health_percentage() >= 30 ? false : base_t::ready();
  }
};

// Target Acquired | Illegal Mods ===========================================
struct target_acquired_t : public action_t
{
  typedef action_t base_t;

  static int energy_returned() { return 10; }

  target_acquired_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    parse_options( options_str );

    cooldown -> duration = from_seconds( 120 );
    use_off_gcd = true;
    trigger_gcd = timespan_t::zero();
  }

  void execute()
  {
    base_t::execute();
    p() -> buffs.target_acquired -> trigger();
    if ( p() -> set_bonus.rakata_field_techs -> two_pc() )
      p() -> resource_gain( RESOURCE_ENERGY, energy_returned(), p() -> gains.target_acquired );
  }
};

// Shatter Shot | Flourish Shot =============================================
struct shatter_shot_t : public range_attack_t
{
  typedef range_attack_t base_t;

  shatter_shot_t( class_t* p, const std::string& n, const std::string& options_str) :
    base_t( n, p )
  {
    // rank_level_list = { ... ,50 };

    parse_options( options_str );

    base_cost                    = 10;
    cooldown -> duration         = from_seconds( 4.5 );
    range                        = 30.0;
    dd.standardhealthpercentmin  =
      dd.standardhealthpercentmax  = 0.056;
    dd.power_mod                 = 0.56;
    weapon                       = &( player -> main_hand_weapon );
    weapon_multiplier            = -0.62;
  }

  virtual void execute()
  {
    base_t::execute();
    targetdata_t *t = targetdata();
    t -> debuff_shatter_shot ->trigger();
  }
};

// ==========================================================================
// Gunslinger / Sniper Utility
// ==========================================================================
void class_t::_trigger_reactive_shot()
{
  unsigned rank = talents.reactive_shot -> rank();
    if ( rank && rngs.reactive_shot -> roll ( rank * 0.5 ))
      cooldowns.ambush -> reduce( from_seconds( 1 ) );
}

// ==========================================================================
// Gunslinger / Sniper Callbacks
// ==========================================================================

class action_callback_t : public ::action_callback_t
{
public:
  action_callback_t( class_t* player ) :
    ::action_callback_t( player )
  {}

  class_t* p() const { return static_cast<class_t*>( listener ); }
  class_t* cast() const { return p(); }
};

struct sniper_volley_callback_t : public action_callback_t
{
  sniper_volley_callback_t( class_t* p ) :
    action_callback_t( p )
  {}

  virtual void trigger (::action_t* a, void* /* call_data */)
  {
    // TODO test what constitutes "blaster fire" assuming any weapon attack
    if ( a -> weapon || a -> td.weapon )
      p() -> buffs.sniper_volley -> trigger();
  }
};

struct cluster_bombs_callback_t : public action_callback_t
{
  struct cluster_bombs_t : public tech_attack_t
  {
    typedef tech_attack_t base_t;

    cluster_bombs_t( class_t* p, const std::string& n) :
      base_t( n, p )
    {
      // rank_level_list = { ... ,50 };

      // unlimited? it will always hit
      range                        = 99.0;
      // always hit?
      base_accuracy                = 999;
      dd.standardhealthpercentmin  =
      dd.standardhealthpercentmax  = 0.047;
      dd.power_mod                 = 0.47;
      background                   = true;
      trigger_gcd                  = timespan_t::zero();
    }
  };

  cluster_bombs_t* cluster_bombs;
  buff_t* debuff_cluster_bombs;

  cluster_bombs_callback_t( class_t* p ) :
    action_callback_t( p ),
    cluster_bombs( new cluster_bombs_t( p, p -> abilities.cluster_bombs ) ),
    debuff_cluster_bombs( cluster_bombs -> targetdata() -> debuff_cluster_bombs )
  {}

  virtual void trigger (::action_t* a, void* /* call_data */)
  {
    if ( ( a -> weapon || a -> td.weapon )
        && debuff_cluster_bombs -> up()
        && debuff_cluster_bombs -> trigger() )
    {
      cluster_bombs -> execute();
      if ( p() -> talents.imperial_methodology -> rank() )
        p() -> resource_gain( RESOURCE_ENERGY, 5, p() -> gains.imperial_methodology );
    }
  }
};


// ==========================================================================
// Gunslinger / Sniper Character Definition
// ==========================================================================

// class_t::create_action =======================================
::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  if ( name == abilities.ambush                ) return new ambush_t                ( this, name, options_str ) ;
  if ( name == abilities.cover_pulse           ) return new cover_pulse_t           ( this, name, options_str ) ;
  if ( name == abilities.emp_discharge         ) return new emp_discharge_t         ( this, name, options_str ) ;
  if ( name == abilities.followthrough         ) return new followthrough_t         ( this, name, options_str ) ;
  if ( name == abilities.interrogation_probe   ) return new interrogation_probe_t   ( this, name, options_str ) ;
  if ( name == abilities.laze_target           ) return new laze_target_t           ( this, name, options_str ) ;
  if ( name == abilities.plasma_probe          ) return new plasma_probe_t          ( this, name, options_str ) ;
  if ( name == abilities.rapid_fire            ) return new rapid_fire_t            ( this, name, options_str ) ;
  if ( name == abilities.series_of_shots       ) return new series_of_shots_t       ( this, name, options_str ) ;
  if ( name == abilities.takedown              ) return new takedown_t              ( this, name, options_str ) ;
  if ( name == abilities.target_acquired       ) return new target_acquired_t       ( this, name, options_str ) ;
  if ( name == abilities.shatter_shot          ) return new shatter_shot_t          ( this, name, options_str ) ;

  // extended
  if ( name == abilities.corrosive_grenade     ) return new corrosive_grenade_t     ( this, name, options_str ) ;
  if ( name == abilities.cull                  ) return new cull_t                  ( this, name, options_str ) ;
  if ( name == abilities.explosive_probe       ) return new explosive_probe_t       ( this, name, options_str ) ;
  if ( name == abilities.fragmentation_grenade ) return new fragmentation_grenade_t ( this, name, options_str ) ;
  if ( name == abilities.orbital_strike        ) return new orbital_strike_t        ( this, name, options_str ) ;
  if ( name == abilities.snipe                 ) return new snipe_t                 ( this, name, options_str ) ;
  if ( name == abilities.take_cover            ) return new take_cover_t            ( this, name, options_str ) ;

  return base_t::create_action( name, options_str );
}

// class_t::init_abilities =======================================

void class_t::init_abilities()
{
  base_t::init_abilities();

  bool sn = type == IA_SNIPER;

  //                             =    ? SNIPER LABEL           : GUNSLINGER LABEL       ;
  abilities.ambush               = sn ? "ambush"               : "aimed_shot"           ;
  abilities.cover_pulse          = sn ? "cover_pulse"          : "pulse_detonator"      ;
  abilities.emp_discharge        = sn ? "emp_discharge"        : "sabotage"             ;
  abilities.followthrough        = sn ? "followthrough"        : "trickshot"            ;
  abilities.interrogation_probe  = sn ? "interrogation_probe"  : "shock_charge"         ;
  abilities.laze_target          = sn ? "laze_target"          : "smugglers_luck"       ;
  abilities.plasma_probe         = sn ? "plasma_probe"         : "incendiary_grenade"   ;
  abilities.rapid_fire           = sn ? "rapid_fire"           : "rapid_fire"           ;
  abilities.series_of_shots      = sn ? "series_of_shots"      : "speed_shot"           ;
  abilities.takedown             = sn ? "takedown"             : "quickdraw"            ;
  abilities.target_acquired      = sn ? "target_acquired"      : "illegal_mods"         ;
  abilities.shatter_shot         = sn ? "shatter_shot"         : "flourish_shot"        ;

  // buffs
  abilities.energy_overrides     = sn ? "energy_overrides"     : "seize_the_moment"     ;
  abilities.cluster_bombs        = sn ? "cluster_bombs"        : "contingency_charges"  ;
  abilities.snap_shot            = sn ? "snap_shot"            : "snap_shot"            ;
  abilities.sniper_volley        = sn ? "sniper_volley"        : "burst_volley"         ;
  abilities.stroke_of_genius     = sn ? "stroke_of_genius"     : "pandemonium"          ;
  // rngs
  abilities.reactive_shot        = sn ? "reactive_shot"        : "quick_aim"            ;
  // gains
  abilities.imperial_methodology = sn ? "imperial_methodology" : "imperial_methodology" ;
  abilities.snipers_nest         = sn ? "snipers_nest"         : "fox_hole"             ;
  // talent dot
  abilities.electrified_railgun  = sn ? "electrified_railgun"  : "blazing_speed"        ;
}

// class_t::init_talents ========================================

void class_t::init_talents()
{
  base_t::init_talents();

  // Marksmanship|Sharpshooter
  // t1
  talents.cover_screen                        = find_talent( "Cover Screen"                       );
  talents.steady_shots                        = find_talent( "Steady Shots"                       );
  talents.marksmanship                        = find_talent( "Marksmanship"                       );
  // t2
  talents.heavy_shot                          = find_talent( "Heavy Shot"                         );
  talents.ballistic_dampers                   = find_talent( "Ballistic Dampers"                  );
  talents.precision_ambush                    = find_talent( "Precision Ambush"                   );
  talents.imperial_demarcation                = find_talent( "Imperial Demarcation"               );
  // t3
  talents.snap_shot                           = find_talent( "Snap Shot"                          );
  talents.diversion                           = find_talent( "Diversion"                          );
  talents.reactive_shot                       = find_talent( "Reactive Shot"                      );
  // t4
  talents.between_the_eyes                    = find_talent( "Between the Eyes"                   );
  talents.sector_ranger                       = find_talent( "Sector Ranger"                      );
  talents.sniper_volley                       = find_talent( "Sniper Volley"                      );
  talents.snipers_nest                        = find_talent( "Sniper's Nest"                      );
  // t5
  talents.recoil_control                      = find_talent( "Recoil Control"                     );
  talents.followthrough                       = find_talent( "Followthrough"                      );
  talents.pillbox_sniper                      = find_talent( "Pillbox Sniper"                     );
  // t6
  talents.imperial_assassin                   = find_talent( "Imperial Assassin"                  );
  talents.siege_bunker                        = find_talent( "Siege Bunker"                       );
  // t7
  talents.rapid_fire                          = find_talent( "Rapid Fire"                         );

  // Engineering|Saboteur
  // t1
  talents.engineers_tool_belt                 = find_talent( "Engineer's Tool Belt"               );
  talents.energy_tanks                        = find_talent( "Energy Tanks"                       );
  talents.gearhead                            = find_talent( "Gearhead"                           );
  // t2
  talents.explosive_engineering               = find_talent( "Explosive Engineering"              );
  talents.vital_regulators                    = find_talent( "Vital Regulators"                   );
  talents.stroke_of_genius                    = find_talent( "Stroke of Genius"                   );
  talents.vitality_serum                      = find_talent( "Vitality Serum"                     );
  // t3
  talents.cluster_bombs                       = find_talent( "Cluster Bombs"                      );
  talents.efficient_engineering               = find_talent( "Efficient Engineering"              );
  talents.interrogation_probe                 = find_talent( "Interrogation Probe"                );
  talents.inventive_interrogation_techniques  = find_talent( "Inventive Interrogation Techniques" );
  // t4
  talents.imperial_methodology                = find_talent( "Imperial Methodology"               );
  talents.calculated_pursuit                  = find_talent( "Calculated Pursuit"                 );
  talents.experimental_explosives             = find_talent( "Experimental Explosives"            );
  // t5
  talents.energy_overrides                    = find_talent( "Energy Overrides"                   );
  talents.emp_discharge                       = find_talent( "EMP Discharge"                      );
  talents.augmented_shields                   = find_talent( "Augmented Shields"                  );
  // t6
  talents.deployed_shields                    = find_talent( "Deployed Shields"                   );
  talents.electrified_railgun                 = find_talent( "Electrified Railgun"                );
  // t7
  talents.plasma_probe                        = find_talent( "Plasma Probe"                       );
}

// class_t::init_base ===========================================

void class_t::init_base()
{
  base_t::init_base();

  default_distance = 3;
  distance = default_distance;
  attribute_multiplier_initial[ ATTR_CUNNING ] += 0.03 * talents.gearhead -> rank();

  if ( unsigned rank = talents.energy_tanks -> rank() )
    resource_base[ RESOURCE_ENERGY ] += 5 * rank;

  _tech_range_accuracy = 0.01 * talents.marksmanship -> rank();
}

// class_t::init_benefits =======================================

void class_t::init_benefits()
{
  base_t::init_benefits();
}

// class_t::init_buffs ==========================================

void class_t::init_buffs()
{
  base_t::init_buffs();

// buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

  buffs.energy_overrides = new buff_t( this , abilities.energy_overrides , 1 ,  from_seconds( 15  ) );
  buffs.followthrough    = new buff_t( this , abilities.followthrough    , 1 ,  from_seconds( 4.5 ) );
  buffs.laze_target      = new buff_t( this , abilities.laze_target      , 1 ,  from_seconds( 20  ) );
  buffs.snap_shot        = new buff_t( this , abilities.snap_shot        , 1 ,  from_seconds( 10  ), from_seconds( 6 ), 0.5 * talents.snap_shot -> rank() );
  buffs.rapid_fire       = new buff_t( this , abilities.rapid_fire       , 1 ,  from_seconds( 10  ) );
  buffs.sniper_volley    = new buff_t( this , abilities.sniper_volley    , 1 ,  from_seconds( 10  ), from_seconds( 30 ), 0.05 * talents.sniper_volley -> rank() );
  buffs.stroke_of_genius = new buff_t( this , abilities.stroke_of_genius , 1 ,  from_seconds( 10  ), timespan_t::zero(), 0.5 * talents.stroke_of_genius -> rank() );
  buffs.target_acquired  = new buff_t( this , abilities.target_acquired  , 1 ,  from_seconds( 10  ));
}

// class_t::init_gains ==========================================

void class_t::init_gains()
{
  base_t::init_gains();

  gains.imperial_methodology = get_gain( abilities.imperial_methodology );
  gains.snipers_nest         = get_gain( abilities.snipers_nest         );
  gains.sniper_volley        = get_gain( abilities.sniper_volley        );
  gains.target_acquired      = get_gain( abilities.target_acquired      );
}

// class_t::init_procs ==========================================

void class_t::init_procs()
{
  base_t::init_procs();

}

// class_t::init_cooldowns ======================================

void class_t::init_cooldowns()
{
  base_t::init_cooldowns();

  cooldowns.adrenaline_probe    = get_cooldown ( abilities.adrenaline_probe    );
  cooldowns.ambush              = get_cooldown ( abilities.ambush              );
  cooldowns.interrogation_probe = get_cooldown ( abilities.interrogation_probe );
  cooldowns.series_of_shots     = get_cooldown ( abilities.series_of_shots     );
}

// class_t::init_rng ============================================

void class_t::init_rng()
{
  base_t::init_rng();
  rngs.reactive_shot = get_rng( abilities.reactive_shot );

}

// class_t::init_actions ========================================

void class_t::init_actions()
{
  const std::string sl = "/";
  const int energy_ceil = talented_energy();
  const int energy_floor = talented_energy() * 6 / 10;

  if ( action_list_str.empty() )
  {
    action_list_default = true;

    std::stringstream list;

    list << "/stim,type=exotech_skill";

    list << sl << abilities.coordination;

    list << "/snapshot_stats";

    list << sl << abilities.take_cover << ",if=buff." << abilities.cover << ".down";

    list << sl << abilities.shatter_shot << ",if=buff." << abilities.shatter_shot << ".remains<1.5";

    list << "/use_relics/power_potion";

    if ( talents.plasma_probe -> rank() )
      list << sl << abilities.plasma_probe << ",if=energy>=" << energy_floor + plasma_probe_t::energy_cost( this );

    if ( unsigned rank = talents.energy_overrides -> rank() )
    {
      list << sl << abilities.plasma_probe << ",if=buff." << abilities.energy_overrides << ".up";
      if ( rank == 1 )
        list << "&energy>=" << energy_floor + plasma_probe_t::energy_cost( this ) / 2;
      list << sl << abilities.explosive_probe << ",if=buff." << abilities.energy_overrides << ".up";
      if ( rank == 1 )
        list << "&energy>=" << energy_floor + explosive_probe_t::energy_cost( this ) / 2;
    }

    if ( talents.emp_discharge -> rank() )
      list << sl << abilities.emp_discharge << ",if=dot.interrogation_probe.remains<1.6";

    list << sl << abilities.adrenaline_probe << ",if=energy<=" << energy_ceil - agent_smug::adrenaline_probe_t::energy_returned_initial();

    list << sl << abilities.target_acquired << ",if=energy<=" << energy_ceil - target_acquired_t::energy_returned();

    if ( talents.interrogation_probe -> rank() )
      list << sl << abilities.interrogation_probe << ",if=energy>=" << energy_floor + interrogation_probe_t::energy_cost( this );

    list << sl << abilities.orbital_strike << ",if=energy>=" << energy_floor + agent_smug::orbital_strike_t::energy_cost();

    if ( talents.corrosive_grenade -> rank() )
      list << sl << abilities.corrosive_grenade << ",if=!ticking";

    list << sl << abilities.takedown << ",if=energy>=" << energy_floor + takedown_t::energy_cost( this );

    if ( talents.cull -> rank() )
      list << sl << abilities.cull << ",if=energy>=" << energy_floor + cull_t::energy_cost() <<
          "&(dot." << abilities.corrosive_dart << ".ticking|dot." << abilities.corrosive_dart << "_weak.ticking)"
          "&(dot." << abilities.corrosive_grenade << ".ticking|dot." << abilities.corrosive_grenade << "_weak.ticking)";

    if ( talents.weakening_blast -> rank() )
      list << sl << abilities.weakening_blast;

    list << sl << abilities.series_of_shots << ",if=energy>" << energy_floor + series_of_shots_t::energy_cost();

    if ( talents.rapid_fire -> rank() )
      list << sl << abilities.rapid_fire << ",if=cooldown." << abilities.series_of_shots << ".remains";

    if ( talents.followthrough -> rank() )
      list << sl << abilities.followthrough << ",if=buff." << abilities.followthrough << ".up&energy>=" << energy_floor + followthrough_t::energy_cost( this );

    list << sl << abilities.explosive_probe << ",if=energy>=" << energy_floor + explosive_probe_t::energy_cost( this );

    list << sl << abilities.ambush << ",if=energy>=" << energy_floor + ambush_t::energy_cost();

    list << sl << abilities.corrosive_dart << ",if=!ticking&energy>=" << energy_floor + agent_smug::corrosive_dart_t::energy_cost();

    if ( talents.stroke_of_genius -> rank() )
      list << sl << abilities.cover_pulse;

    list << sl << abilities.laze_target;

    if ( talents.followthrough -> rank() )
      list << sl << abilities.snipe << ",if=energy>" << energy_floor + snipe_t::energy_cost();

    list << sl << abilities.rifle_shot;

    action_list_str = list.str();
  }

  if ( talents.sniper_volley -> rank() )
  {
    register_attack_callback( RESULT_HIT_MASK, new sniper_volley_callback_t( this ) );
    register_tick_callback  ( RESULT_HIT_MASK, new sniper_volley_callback_t( this ) );
  }

  if ( talents.cluster_bombs -> rank() )
  {
    register_attack_callback( RESULT_HIT_MASK, new cluster_bombs_callback_t( this ) );
    register_tick_callback  ( RESULT_HIT_MASK, new cluster_bombs_callback_t( this ) );
  }

  base_t::init_actions();
}

// class_t::tech_accuracy_chance ================================
double class_t::tech_accuracy_chance() const
{
  return base_t::tech_accuracy_chance() + _tech_range_accuracy
    + ( (sim -> ptr && buffs.target_acquired -> up()) ? 0.3 : 0 );
}

// class_t::range_accuracy_chance ===============================
double class_t::range_accuracy_chance() const
{
  return base_t::range_accuracy_chance() + _tech_range_accuracy
    + ( (sim -> ptr && buffs.target_acquired -> up()) ? 0.3 : 0 );
}

// class_t::talented_energy =====================================
int class_t::talented_energy() const
{
  return base_t::talented_energy() + 5 * talents.energy_tanks -> rank();
}

// class_t::energy_regen_per_second =============================
// MIRROR CHANGES WITH regen BELOW
double class_t::energy_regen_per_second() const
{
  double eps = base_t::energy_regen_per_second();
  if ( talents.snipers_nest -> rank() && buffs.cover -> up() )
    eps += 1;
  if ( buffs.sniper_volley -> up() )
    eps *= 2;
  return eps;
}


// class_t::regen ===============================================
// MIRROR CHANGES WITH energy_regen_per_second ABOVE
void class_t::regen( timespan_t periodicity )
{
  base_t::regen( periodicity );
  if ( talents.snipers_nest -> rank() && buffs.cover -> up() )
    resource_gain( RESOURCE_ENERGY, to_seconds( periodicity ) * 1, gains.snipers_nest );

  if ( buffs.sniper_volley -> up() )
    // using ERPS instead of directly checking gains as a kind of double check that ERPS is maintained
    resource_gain( RESOURCE_ENERGY, to_seconds( periodicity ) * energy_regen_per_second() / 2, gains.sniper_volley );
}

// class_t::alacrity ============================================
double class_t::alacrity() const
{
  // TODO confirm how these 10% and 20% work/stack
  // are they a flat 20%, or maybe 20% increase to alacrity rating?
  return base_t::alacrity() - ( buffs.sniper_volley -> up() ? 0.1 : 0 ) -
    ( sim -> ptr                   ? 0   :
     buffs.target_acquired -> up() ? 0.2 : 0 );
}

// class_t::armor_penetration =================================

double class_t::armor_penetration() const
{
  double arpen = base_t::armor_penetration();

  if ( sim -> ptr && buffs.target_acquired -> up() )
    arpen *= ( 1 - 0.15 );

  return arpen;
}

// class_t::primary_role ========================================
role_type class_t::primary_role() const
{
  return ROLE_DPS;
}

// class_t::create_talents ======================================
void class_t::create_talents()
{
  base_t::create_talents();

  // Marksmanship|Sharpshooter
  static const talentinfo_t marksmanship_tree[] = {
     { "Cover Screen"      , 2 }, { "Steady Shots"      , 2 }, { "Marksmanship"     , 3 },
     { "Heavy Shot"        , 1 }, { "Ballistic Dampers" , 2 }, { "Precision Ambush" , 2 }, { "Imperial Demarcation" , 2 },
     { "Snap Shot"         , 2 }, { "Diversion"         , 1 }, { "Reactive Shot"    , 2 },
     { "Between the Eyes"  , 2 }, { "Sector Ranger"     , 1 }, { "Sniper Volley"    , 3 }, { "Sniper's Nest"        , 1 },
     { "Recoil Control"    , 2 }, { "Followthrough"     , 1 }, { "Pillbox Sniper"   , 2 },
     { "Imperial Assassin" , 3 }, { "Siege Bunker"      , 2 },
     { "Rapid Fire"        , 1 }
  };
  init_talent_tree( IA_SNIPER_MARKSMANSHIP, marksmanship_tree );

  // Engineering|Saboteur
  static const talentinfo_t engineering_tree[] = {
     { "Engineer's Tool Belt"  , 2 }, { "Energy Tanks"          , 2 }, { "Gearhead"                , 3 },
     { "Explosive Engineering" , 3 }, { "Vital Regulators"      , 2 }, { "Stroke of Genius"        , 2 }, { "Vitality Serum"                     , 2 },
     { "Cluster Bombs"         , 2 }, { "Efficient Engineering" , 2 }, { "Interrogation Probe"     , 1 }, { "Inventive Interrogation Techniques" , 2 },
     { "Imperial Methodology"  , 1 }, { "Calculated Pursuit"    , 2 }, { "Experimental Explosives" , 2 },
     { "Energy Overrides"      , 2 }, { "EMP Discharge"         , 1 }, { "Augmented Shields"       , 2 },
     { "Deployed Shields"      , 2 }, { "Electrified Railgun"   , 3 },
     { "Plasma Probe"          , 1 }
  };
  init_talent_tree( IA_SNIPER_ENGINEERING, engineering_tree );
}

} // namespace gunslinger_sniper ============================================

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_gunslinger  =============================================

player_t* player_t::create_gunslinger( sim_t* sim, const std::string& name, race_type r )
{
  return new gunslinger_sniper::class_t( sim, S_GUNSLINGER, name, r );
}

// player_t::create_sniper  =================================================

player_t* player_t::create_sniper( sim_t* sim, const std::string& name, race_type r )
{
  return new gunslinger_sniper::class_t( sim, IA_SNIPER, name, r );
}

// player_t::gunslinger_sniper_init =========================================

void player_t::gunslinger_sniper_init( sim_t* /* sim */ )
{
}

// player_t::gunslinger_sniper_combat_begin =================================

void player_t::gunslinger_sniper_combat_begin( sim_t* /* sim */ )
{
}
