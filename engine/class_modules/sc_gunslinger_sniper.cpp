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
  dot_t dot_cull;
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
    buff_t* reactive_shot;
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

  // Mirror names
  struct mirror_t:base_t::mirror_t
  {
    // abilities
    std::string a_ambush;
    std::string a_cover_pulse;
    std::string a_laze_target;
    std::string a_series_of_shots;
    std::string a_shatter_shot;
    std::string a_takedown;
    std::string a_target_acquired;

    // Marksmanship|Sharpshooter
    // t1
    std::string t_cover_screen;
    std::string t_steady_shots;
    std::string t_marksmanship;
    // t2
    std::string t_heavy_shot;
    std::string t_ballistic_dampers;
    std::string t_precision_ambush;
    std::string t_imperial_demarcation;
    // t3
    std::string t_snap_shot;
    std::string t_diversion;
    std::string t_reactive_shot;
    // t4
    std::string t_between_the_eyes;
    std::string t_sector_ranger;
    std::string t_sniper_volley;
    std::string t_snipers_nest;
    // t5
    std::string t_recoil_control;
    std::string t_followthrough;
    std::string t_pillbox_sniper;
    // t6
    std::string t_imperial_assassin;
    std::string t_siege_bunker;
    // t7
    std::string t_rapid_fire;

    // Engineering|Saboteur
    // t1
    std::string t_engineers_tool_belt;
    std::string t_energy_tanks;
    std::string t_gearhead;
    // t2
    std::string t_explosive_engineering;
    std::string t_vital_regulators;
    std::string t_stroke_of_genius;
    std::string t_vitality_serum;
    // t3
    std::string t_cluster_bombs;
    std::string t_efficient_engineering;
    std::string t_interrogation_probe;
    std::string t_inventive_interrogation_techniques;
    // t4
    std::string t_imperial_methodology;
    std::string t_calculated_pursuit;
    std::string t_experimental_explosives;
    // t5
    std::string t_energy_overrides;
    std::string t_emp_discharge;
    std::string t_augmented_shields;
    // t6
    std::string t_deployed_shields;
    std::string t_electrified_railgun;
    // t7
    std::string t_plasma_probe;
  } m;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    base_t( sim, pt == IA_SNIPER ? IA_SNIPER : S_GUNSLINGER, name, rt, buffs, gains, procs, rngs, benefits, talents, m ),
    buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents()
  {
    tree_type[ IA_SNIPER_MARKSMANSHIP ] = TREE_MARKSMANSHIP;
    tree_type[ IA_SNIPER_ENGINEERING  ] = TREE_ENGINEERING;
    tree_type[ IA_LETHALITY           ] = TREE_LETHALITY;

    create_mirror();
    create_talents();
    create_options();
  }

  // Character Definition
  double    _tech_range_accuracy;
  targetdata_t* new_targetdata( player_t& target ) // override
  { return new targetdata_t( *this, target ); }
  ::action_t* create_action( const std::string& name, const std::string& options );
  void      init_talents();
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
  void      create_mirror();
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
  debuff_cluster_bombs        ( new debuff_t ( this, source.m.t_cluster_bombs       , 2 + ( 2 * source.talents.imperial_methodology -> rank()) , from_seconds ( 20 ), from_seconds( 1 ) , 100, false, true /*reverse*/ ) ),
  debuff_electrified_railgun  ( new debuff_t ( this, source.m.t_electrified_railgun , 4, from_seconds ( 5 ), from_seconds( 0 )   , 1 * source.talents.electrified_railgun -> rank() ) ),
  debuff_shatter_shot         ( new debuff_t ( this, source.m.a_shatter_shot        , 1, from_seconds ( 45 ) ) ),
  dot_cull                ( source.m.t_cull                 , &source ),
  dot_electrified_railgun ( source.m.t_electrified_railgun  , &source ),
  dot_interrogation_probe ( source.m.t_interrogation_probe  , &source ),
  dot_plasma_probe        ( source.m.t_plasma_probe         , &source ),
  dot_series_of_shots     ( source.m.a_series_of_shots      , &source )
{
  add( *debuff_cluster_bombs       );
  add( *debuff_electrified_railgun );
  add( *debuff_shatter_shot        );

  add( dot_cull                    );
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
    cooldown -> duration         = from_seconds( ( 15 - p -> talents.rapid_fire -> rank() ) );
    range                        = 35.0;
    dd.standardhealthpercentmin  =
        dd.standardhealthpercentmax  = 0.347;
    dd.power_mod                 = 3.47;
    weapon                       = &( player -> main_hand_weapon );
    weapon_multiplier            = 1.310;

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

  virtual timespan_t execute_time() const
  {
    if ( p() -> buffs.reactive_shot -> check() )
      // TODO: test whether this 1s comes off before or after alacrity. this is after.
      return base_t::execute_time() - from_seconds( 1.0 );
    else
      return base_t::execute_time();
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

    p() -> buffs.reactive_shot -> expire();
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
    // TODO CHECK do razor_rounds and steady_shot benefit the extra attack as well?
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
    base_multiplier            += 0.03 * p -> talents.steady_shots -> rank();
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
    if ( p() -> talents.cluster_bombs -> rank() && result_is_hit() )
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
    dd.standardhealthpercentmax  = 0.239;
    dd.power_mod                 = 2.39;
    weapon                       = &( player -> main_hand_weapon );
    weapon_multiplier            = 0.6;
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
      base_cost                    = 0;
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

// Sniper Volley | Burst Volley =============================================
struct sniper_volley_t : public action_t
{
  typedef action_t base_t;

  sniper_volley_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    check_talent( p -> talents.sniper_volley -> rank() );

    parse_options( options_str );

    cooldown -> duration = from_seconds( 45 );
    use_off_gcd = true;
    trigger_gcd = timespan_t::zero();
  }

  void execute()
  {
    base_t::execute();
    p() -> buffs.sniper_volley -> trigger();
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
    cooldown -> duration         = from_seconds( ( 15 - p -> talents.rapid_fire -> rank() ) );
    channeled                    = true;
    tick_zero                    = true;
    num_ticks                    = 3;
    base_tick_time               = from_seconds( 1 );
    td.standardhealthpercentmin  =
    td.standardhealthpercentmax  = 0.103;
    td.weapon                    = &( player -> main_hand_weapon );
    td.power_mod                 = 1.03;
    td.weapon_multiplier         = -0.313;
    // tick crits. the invisible execute hit shouldn't
    may_crit                     = false;
    base_multiplier             += 0.03 * p -> talents.steady_shots      -> rank();
    base_multiplier             += 0.03 * p -> talents.cut_down          -> rank();
    base_crit                   += 0.02 * p -> talents.between_the_eyes  -> rank();
    crit_multiplier             += 0.1  * p -> talents.imperial_assassin -> rank();
    if ( p -> talents.electrified_railgun -> rank() )
    {
      electrified = new electrified_railgun_t( p, p -> m.t_electrified_railgun );
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

    // TODO confirm this no longer double-grants series of shots...
    //buff_t* sniper_volley = p() -> buffs.sniper_volley;
    //if ( sniper_volley -> up() )
    //{
      ////sniper_volley -> expire();
      //cooldown -> reset();
    //}
  }

};

// Overload Shot | Quick Shot ===============================================

struct overload_shot_t : public agent_smug::overload_shot_t
{
  typedef agent_smug::overload_shot_t base_t;

  overload_shot_t( class_t* p, const std::string& n, const std::string& options_str) :
    base_t( p, n, options_str )
  {
    range = 30;
  }
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
    range            = 35;
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
    base_t::player_buff();
    //if ( p() -> buffs.laze_target -> up() )
      //player_crit += 1; // 100% crit
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
    if ( player -> set_bonus.battlemaster_field_techs -> four_pc() )
      range += 5.0;
    dd.standardhealthpercentmin  =
    dd.standardhealthpercentmax  = 0.284;
    dd.power_mod                 = 2.84;
    weapon                       = &( player -> main_hand_weapon );
    weapon_multiplier            = 0.89;
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
      buffs.reactive_shot -> trigger();
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
    cluster_bombs( new cluster_bombs_t( p, p -> m.t_cluster_bombs ) ),
    debuff_cluster_bombs( cluster_bombs -> targetdata() -> debuff_cluster_bombs )
  {}

  virtual void trigger (::action_t* a, void* /* call_data */)
  {
    if ( ( a -> weapon || a -> td.weapon )
        && a -> result_is_hit()
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
  if ( name == m.a_ambush                 ) return new ambush_t                 ( this, name, options_str ) ;
  if ( name == m.a_cover_pulse            ) return new cover_pulse_t            ( this, name, options_str ) ;
  if ( name == m.t_emp_discharge          ) return new emp_discharge_t          ( this, name, options_str ) ;
  if ( name == m.t_followthrough          ) return new followthrough_t          ( this, name, options_str ) ;
  if ( name == m.t_interrogation_probe    ) return new interrogation_probe_t    ( this, name, options_str ) ;
  if ( name == m.a_laze_target            ) return new laze_target_t            ( this, name, options_str ) ;
  if ( name == m.t_plasma_probe           ) return new plasma_probe_t           ( this, name, options_str ) ;
  if ( name == m.a_series_of_shots        ) return new series_of_shots_t        ( this, name, options_str ) ;
  if ( name == m.t_sniper_volley          ) return new sniper_volley_t          ( this, name, options_str ) ;
  if ( name == m.a_takedown               ) return new takedown_t               ( this, name, options_str ) ;
  if ( name == m.a_target_acquired        ) return new target_acquired_t        ( this, name, options_str ) ;
  if ( name == m.a_shatter_shot           ) return new shatter_shot_t           ( this, name, options_str ) ;

  // extended
  if ( name == m.a_corrosive_grenade      ) return new corrosive_grenade_t      ( this, name, options_str ) ;
  if ( name == m.t_cull                   ) return new cull_t                   ( this, name, options_str ) ;
  if ( name == m.a_explosive_probe        ) return new explosive_probe_t        ( this, name, options_str ) ;
  if ( name == m.a_fragmentation_grenade  ) return new fragmentation_grenade_t  ( this, name, options_str ) ;
  if ( name == m.a_orbital_strike         ) return new orbital_strike_t         ( this, name, options_str ) ;
  if ( name == m.a_overload_shot          ) return new overload_shot_t          ( this, name, options_str ) ;
  if ( name == m.a_snipe                  ) return new snipe_t                  ( this, name, options_str ) ;
  if ( name == m.a_take_cover             ) return new take_cover_t             ( this, name, options_str ) ;

  return base_t::create_action( name, options_str );
}

// class_t::init_talents ========================================

void class_t::init_talents()
{
  base_t::init_talents();

  // Lethality
  // t3
  talents.targeted_demolition                 = find_talent( m.t_targeted_demolition                );
  // t4
  talents.hold_your_ground                    = find_talent( m.t_hold_your_ground                   );
  // t5
  talents.razor_rounds                        = find_talent( m.t_razor_rounds                       );

  // Marksmanship|Sharpshooter
  // t1
  talents.cover_screen                        = find_talent( m.t_cover_screen                       );
  talents.steady_shots                        = find_talent( m.t_steady_shots                       );
  talents.marksmanship                        = find_talent( m.t_marksmanship                       );
  // t2
  talents.heavy_shot                          = find_talent( m.t_heavy_shot                         );
  talents.ballistic_dampers                   = find_talent( m.t_ballistic_dampers                  );
  talents.precision_ambush                    = find_talent( m.t_precision_ambush                   );
  talents.imperial_demarcation                = find_talent( m.t_imperial_demarcation               );
  // t3
  talents.snap_shot                           = find_talent( m.t_snap_shot                          );
  talents.diversion                           = find_talent( m.t_diversion                          );
  talents.reactive_shot                       = find_talent( m.t_reactive_shot                      );
  // t4
  talents.between_the_eyes                    = find_talent( m.t_between_the_eyes                   );
  talents.sector_ranger                       = find_talent( m.t_sector_ranger                      );
  talents.sniper_volley                       = find_talent( m.t_sniper_volley                      );
  talents.snipers_nest                        = find_talent( m.t_snipers_nest                       );
  // t5
  talents.recoil_control                      = find_talent( m.t_recoil_control                     );
  talents.followthrough                       = find_talent( m.t_followthrough                      );
  talents.pillbox_sniper                      = find_talent( m.t_pillbox_sniper                     );
  // t6
  talents.imperial_assassin                   = find_talent( m.t_imperial_assassin                  );
  talents.siege_bunker                        = find_talent( m.t_siege_bunker                       );
  // t7
  talents.rapid_fire                          = find_talent( m.t_rapid_fire                         );

  // Engineering|Saboteur
  // t1
  talents.engineers_tool_belt                 = find_talent( m.t_engineers_tool_belt                );
  talents.energy_tanks                        = find_talent( m.t_energy_tanks                       );
  talents.gearhead                            = find_talent( m.t_gearhead                           );
  // t2
  talents.explosive_engineering               = find_talent( m.t_explosive_engineering              );
  talents.vital_regulators                    = find_talent( m.t_vital_regulators                   );
  talents.stroke_of_genius                    = find_talent( m.t_stroke_of_genius                   );
  talents.vitality_serum                      = find_talent( m.t_vitality_serum                     );
  // t3
  talents.cluster_bombs                       = find_talent( m.t_cluster_bombs                      );
  talents.efficient_engineering               = find_talent( m.t_efficient_engineering              );
  talents.interrogation_probe                 = find_talent( m.t_interrogation_probe                );
  talents.inventive_interrogation_techniques  = find_talent( m.t_inventive_interrogation_techniques );
  // t4
  talents.imperial_methodology                = find_talent( m.t_imperial_methodology               );
  talents.calculated_pursuit                  = find_talent( m.t_calculated_pursuit                 );
  talents.experimental_explosives             = find_talent( m.t_experimental_explosives            );
  // t5
  talents.energy_overrides                    = find_talent( m.t_energy_overrides                   );
  talents.emp_discharge                       = find_talent( m.t_emp_discharge                      );
  talents.augmented_shields                   = find_talent( m.t_augmented_shields                  );
  // t6
  talents.deployed_shields                    = find_talent( m.t_deployed_shields                   );
  talents.electrified_railgun                 = find_talent( m.t_electrified_railgun                );
  // t7
  talents.plasma_probe                        = find_talent( m.t_plasma_probe                       );
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

  buffs.energy_overrides = new buff_t( this , m.t_energy_overrides  , 1 ,  from_seconds( 15  ) );
  buffs.followthrough    = new buff_t( this , m.t_followthrough     , 1 ,  from_seconds( 4.5 ) );
  buffs.laze_target      = new buff_t( this , m.a_laze_target       , 1 ,  from_seconds( 20  ) );
  buffs.snap_shot        = new buff_t( this , m.t_snap_shot         , 1 ,  from_seconds( 10  ), from_seconds( 6 ), 0.5 * talents.snap_shot -> rank() );
  buffs.reactive_shot    = new buff_t( this , m.t_reactive_shot     , 1 ,  from_seconds( 10  ) );
  buffs.sniper_volley    = new buff_t( this , m.t_sniper_volley     , 1 ,  from_seconds( 10  ), from_seconds( 0 ));
  buffs.stroke_of_genius = new buff_t( this , m.t_stroke_of_genius  , 1 ,  from_seconds( 10  ), timespan_t::zero(), 0.5 * talents.stroke_of_genius -> rank() );
  buffs.target_acquired  = new buff_t( this , m.a_target_acquired   , 1 ,  from_seconds( 10  ));
}

// class_t::init_gains ==========================================

void class_t::init_gains()
{
  base_t::init_gains();

  gains.imperial_methodology = get_gain( m.t_imperial_methodology );
  gains.snipers_nest         = get_gain( m.t_snipers_nest         );
  gains.sniper_volley        = get_gain( m.t_sniper_volley        );
  gains.target_acquired      = get_gain( m.a_target_acquired      );
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

  cooldowns.adrenaline_probe    = get_cooldown ( m.a_adrenaline_probe    );
  cooldowns.ambush              = get_cooldown ( m.a_ambush              );
  cooldowns.interrogation_probe = get_cooldown ( m.t_interrogation_probe );
  cooldowns.series_of_shots     = get_cooldown ( m.a_series_of_shots     );
}

// class_t::init_rng ============================================

void class_t::init_rng()
{
  base_t::init_rng();
  rngs.reactive_shot = get_rng( m.t_reactive_shot );

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

    list << sl << m.a_coordination;

    list << "/snapshot_stats";

    list << sl << m.a_take_cover << ",if=buff." << m.a_take_cover << ".down";

    list << sl << m.a_shatter_shot << ",if=buff." << m.a_shatter_shot << ".remains<1.5";

    list << "/use_relics";

    list << "/power_potion";

    list << sl << m.a_adrenaline_probe << ",if=energy<=" << energy_ceil - agent_smug::adrenaline_probe_t::energy_returned_initial();

    list << sl << m.a_target_acquired << ",if=energy<=" << energy_ceil - target_acquired_t::energy_returned();

    list << sl << m.a_laze_target;

    if ( talents.emp_discharge -> rank() )
      list << sl << m.t_emp_discharge << ",if=dot." << m.t_interrogation_probe  << ".remains";

    if ( unsigned rank = talents.energy_overrides -> rank() )
    {
      list << sl << m.t_plasma_probe << ",if=buff." << m.t_energy_overrides << ".up";
      if ( rank == 1 )
        list << "&energy>=" << energy_floor + plasma_probe_t::energy_cost( this ) / 2;
      list << sl << m.a_explosive_probe << ",if=buff." << m.t_energy_overrides << ".up";
      if ( rank == 1 )
        list << "&energy>=" << energy_floor + explosive_probe_t::energy_cost( this ) / 2;
    }

    if ( talents.sniper_volley -> rank() )
      list << sl << m.t_sniper_volley << ",if=cooldown." << m.a_series_of_shots << ".remains";

    if ( talents.imperial_methodology -> rank() )
      list << sl << m.a_explosive_probe << ",if=energy>=" << energy_floor + explosive_probe_t::energy_cost( this );

    if ( talents.followthrough -> rank() )
      list << sl << m.t_followthrough << ",if=buff." << m.t_followthrough << ".up&energy>=" << energy_floor + followthrough_t::energy_cost( this );

    if ( talents.experimental_explosives -> rank() || talents.cull -> rank() )
      list << sl << m.a_orbital_strike << ",if=energy>=" << energy_floor + agent_smug::orbital_strike_t::energy_cost();

    if ( talents.corrosive_grenade -> rank() )
      list << sl << m.a_corrosive_grenade << ",if=!ticking&energy>=" << energy_floor + corrosive_grenade_t::energy_cost();

    if ( talents.corrosive_microbes -> rank() )
      list << sl << m.a_corrosive_dart << ",if=!ticking&energy>=" << energy_floor + agent_smug::corrosive_dart_t::energy_cost();

    if ( talents.cut_down -> rank() )
      list << sl << m.a_series_of_shots << ",if=energy>" << energy_floor + series_of_shots_t::energy_cost();

    if ( set_bonus.rakata_field_techs -> four_pc() )
      list << sl << m.a_takedown << ",if=energy>=" << energy_floor + takedown_t::energy_cost( this );

    if ( talents.weakening_blast -> rank() )
      list << sl << m.t_weakening_blast;

    if ( talents.cull -> rank() )
      list << sl << m.t_cull << ",if=energy>=" << energy_floor + cull_t::energy_cost() <<
              "&(dot." << m.a_corrosive_dart << ".ticking|dot." << m.a_corrosive_dart << "_weak.ticking)"
              "&(dot." << m.a_corrosive_grenade << ".ticking|dot." << m.a_corrosive_grenade << "_weak.ticking)";

    if ( talents.reactive_shot -> rank() )
      list << sl << m.a_ambush << ",if=buff." << m.t_reactive_shot << ".up&energy>=" << energy_floor + ambush_t::energy_cost();

    if ( !talents.cut_down -> rank() )
      list << sl << m.a_series_of_shots << ",if=energy>" << energy_floor + series_of_shots_t::energy_cost();

    if ( talents.plasma_probe -> rank() )
      list << sl << m.t_plasma_probe << ",if=energy>=" << energy_floor + plasma_probe_t::energy_cost( this );

    if ( !talents.experimental_explosives -> rank() && !talents.cull -> rank() )
      list << sl << m.a_orbital_strike << ",if=energy>=" << energy_floor + agent_smug::orbital_strike_t::energy_cost();

    if ( !talents.reactive_shot -> rank() && talents.precision_ambush -> rank() )
      list << sl << m.a_ambush << ",if=energy>=" << energy_floor + ambush_t::energy_cost();

    if ( talents.interrogation_probe -> rank() )
      list << sl << m.t_interrogation_probe << ",if=energy>=" << energy_floor + interrogation_probe_t::energy_cost( this );

    if ( !set_bonus.rakata_field_techs -> four_pc() )
      list << sl << m.a_takedown << ",if=energy>=" << energy_floor + takedown_t::energy_cost( this );

    if ( !talents.imperial_methodology -> rank() )
      list << sl << m.a_explosive_probe << ",if=energy>=" << energy_floor + explosive_probe_t::energy_cost( this );

    if ( !talents.reactive_shot -> rank() && !talents.precision_ambush -> rank() )
      list << sl << m.a_ambush << ",if=energy>=" << energy_floor + ambush_t::energy_cost();

    if ( !talents.cull -> rank() )
      list << sl << m.a_snipe << ",if=energy>=" << energy_floor + snipe_t::energy_cost();

    list << sl << m.a_rifle_shot;

    if ( talents.stroke_of_genius -> rank() )
      list << sl << m.a_cover_pulse;

    action_list_str = list.str();
  }

  if ( talents.cluster_bombs -> rank() )
  {
    register_direct_damage_callback( SCHOOL_ALL_MASK, new cluster_bombs_callback_t( this ) );
    register_tick_damage_callback( SCHOOL_ALL_MASK, new cluster_bombs_callback_t( this ) );
  }

  base_t::init_actions();
}

// class_t::tech_accuracy_chance ================================
double class_t::tech_accuracy_chance() const
{
  return base_t::tech_accuracy_chance() + _tech_range_accuracy
    + ( buffs.target_acquired -> up() ? 0.3 : 0 );
}

// class_t::range_accuracy_chance ===============================
double class_t::range_accuracy_chance() const
{
  return base_t::range_accuracy_chance() + _tech_range_accuracy
    + ( buffs.target_acquired -> up() ? 0.3 : 0 );
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
    eps *= 1.5;
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
    resource_gain( RESOURCE_ENERGY, to_seconds( periodicity ) * energy_regen_per_second() / 3, gains.sniper_volley );
}

// class_t::alacrity ============================================
double class_t::alacrity() const
{
  return base_t::alacrity() + ( buffs.sniper_volley -> up() ? 0.1 : 0 );
}

// class_t::armor_penetration =================================

double class_t::armor_penetration() const
{
  return base_t::armor_penetration() - ( buffs.target_acquired -> up() ? 0.15 : 0 );
}

// class_t::primary_role ========================================
role_type class_t::primary_role() const
{
  return ROLE_DPS;
}

// class_t::create_mirror =======================================

void class_t::create_mirror()
{
  base_t::create_mirror();

  bool sn = type == IA_SNIPER;

  // ABILITY               =    ? SNIPER LABEL           : GUNSLINGER LABEL       ;
  m.a_ambush               = sn ? "ambush"               : "aimed_shot"           ;
  m.a_cover_pulse          = sn ? "cover_pulse"          : "pulse_detonator"      ;
  m.a_laze_target          = sn ? "laze_target"          : "smugglers_luck"       ;
  m.a_series_of_shots      = sn ? "series_of_shots"      : "speed_shot"           ;
  m.a_takedown             = sn ? "takedown"             : "quickdraw"            ;
  m.a_target_acquired      = sn ? "target_acquired"      : "illegal_mods"         ;
  m.a_shatter_shot         = sn ? "shatter_shot"         : "flourish_shot"        ;

  // Marksmanship|Sharpshooter
  // t1
  m.t_cover_screen                        = sn ? "cover_screen"                       : "cover_screen"            ;
  m.t_steady_shots                        = sn ? "steady_shots"                       : "steady_shots"            ;
  m.t_marksmanship                        = sn ? "marksmanship"                       : "sharpshooter"            ;
  // t2
  m.t_heavy_shot                          = sn ? "heavy_shot"                         : "percussive_shot"         ;
  m.t_ballistic_dampers                   = sn ? "ballistic_dampers"                  : "ballistic_dampers"       ;
  m.t_precision_ambush                    = sn ? "precision_ambush"                   : "sharp_aim"               ;
  m.t_imperial_demarcation                = sn ? "imperial_demarcation"               : "trip_shot"               ;
  // t3
  m.t_snap_shot                           = sn ? "snap_shot"                          : "snap_shot"               ;
  m.t_diversion                           = sn ? "diversion"                          : "diversion"               ;
  m.t_reactive_shot                       = sn ? "reactive_shot"                      : "quick_aim"               ;
  // t4
  m.t_between_the_eyes                    = sn ? "between_the_eyes"                   : "slick_shooter"           ;
  m.t_sector_ranger                       = sn ? "sector_ranger"                      : "spacer"                  ;
  m.t_sniper_volley                       = sn ? "sniper_volley"                      : "burst_volley"            ;
  m.t_snipers_nest                        = sn ? "snipers_nest"                       : "foxhole"                 ;
  // t5
  m.t_recoil_control                      = sn ? "recoil_control"                     : "recoil_control"          ;
  m.t_followthrough                       = sn ? "followthrough"                      : "trickshot"               ;
  m.t_pillbox_sniper                      = sn ? "pillbox_sniper"                     : "lay_low"                 ;
  // t6
  m.t_imperial_assassin                   = sn ? "imperial_assassin"                  : "deadeye"                 ;
  m.t_siege_bunker                        = sn ? "siege_bunker"                       : "holed_up"                ;
  // t7
  m.t_rapid_fire                          = sn ? "rapid_fire"                         : "rapid_fire"              ;

  // Engineering|Saboteur
  // t1
  m.t_engineers_tool_belt                 = sn ? "engineers_tool_belt"                : "saboteurs_utility_belt"  ;
  m.t_energy_tanks                        = sn ? "energy_tanks"                       : "bravado"                 ;
  m.t_gearhead                            = sn ? "gearhead"                           : "streetwise"              ;
  // t2
  m.t_explosive_engineering               = sn ? "explosive_engineering"              : "independent_anarchy"     ;
  m.t_vital_regulators                    = sn ? "vital_regulators"                   : "cool_under_pressure"     ;
  m.t_stroke_of_genius                    = sn ? "stroke_of_genius"                   : "pandemonium"             ;
  m.t_vitality_serum                      = sn ? "vitality_serum"                     : "underworld_hardships"    ;
  // t3
  m.t_cluster_bombs                       = sn ? "cluster_bombs"                      : "contingency_charges"     ;
  m.t_efficient_engineering               = sn ? "efficient_engineering"              : "dealers_discount"        ;
  m.t_interrogation_probe                 = sn ? "interrogation_probe"                : "shock_charge"            ;
  m.t_inventive_interrogation_techniques  = sn ? "inventive_interrogation_techniques" : "sapping_charge"          ;
  // t4
  m.t_imperial_methodology                = sn ? "imperial_methodology"               : "insurrection"            ;
  m.t_calculated_pursuit                  = sn ? "calculated_pursuit"                 : "hot_pursuit"             ;
  m.t_experimental_explosives             = sn ? "experimental_explosives"            : "arsonist"                ;
  // t5
  m.t_energy_overrides                    = sn ? "energy_overrides"                   : "seize_the_moment"        ;
  m.t_emp_discharge                       = sn ? "emp_discharge"                      : "sabotage"                ;
  m.t_augmented_shields                   = sn ? "augmented_shields"                  : "hotwired_defenses"       ;
  // t6
  m.t_deployed_shields                    = sn ? "deployed_shields"                   : "riot_screen"             ;
  m.t_electrified_railgun                 = sn ? "electrified_railgun"                : "blazing_speed"           ;
  // t7
  m.t_plasma_probe                        = sn ? "plasma_probe"                       : "incendiary_grenade"      ;

  // Lethality|Dirty Fighting
  // t3
  m.t_targeted_demolition                 = sn ? "targeted_demolition"                : "reopen_wound"            ;
  // t4
  m.t_hold_your_ground                    = sn ? "hold_your_ground"                   : "bombastic"               ;
  // t5
  m.t_razor_rounds                        = sn ? "razor_round"                        : "reopen_wounds"           ;
}

// class_t::create_talents ======================================

void class_t::create_talents()
{
  base_t::create_talents();

  // Marksmanship|Sharpshooter
  const talentinfo_t marksmanship_tree[] = {
    { m.t_cover_screen      , 2 }, { m.t_steady_shots       , 2 }, { m.t_marksmanship     , 3 },
    { m.t_heavy_shot        , 1 }, { m.t_ballistic_dampers  , 2 }, { m.t_precision_ambush , 2 }, { m.t_imperial_demarcation , 2 },
    { m.t_snap_shot         , 2 }, { m.t_diversion          , 1 }, { m.t_reactive_shot    , 2 },
    { m.t_between_the_eyes  , 2 }, { m.t_sector_ranger      , 1 }, { m.t_rapid_fire       , 3 }, { m.t_snipers_nest         , 1 },
    { m.t_recoil_control    , 2 }, { m.t_followthrough      , 1 }, { m.t_pillbox_sniper   , 2 },
    { m.t_imperial_assassin , 3 }, { m.t_siege_bunker       , 2 },
    { m.t_sniper_volley     , 1 }
  };
  init_talent_tree( IA_SNIPER_MARKSMANSHIP, marksmanship_tree );

  // Engineering|Saboteur
  const talentinfo_t engineering_tree[] = {
    { m.t_engineers_tool_belt   , 2 }, { m.t_energy_tanks           , 2 }, { m.t_gearhead                 , 3 },
    { m.t_explosive_engineering , 3 }, { m.t_vital_regulators       , 2 }, { m.t_stroke_of_genius         , 2 }, { m.t_vitality_serum                     , 2 },
    { m.t_cluster_bombs         , 2 }, { m.t_efficient_engineering  , 2 }, { m.t_interrogation_probe      , 1 }, { m.t_inventive_interrogation_techniques , 2 },
    { m.t_imperial_methodology  , 1 }, { m.t_calculated_pursuit     , 2 }, { m.t_experimental_explosives  , 2 },
    { m.t_energy_overrides      , 2 }, { m.t_emp_discharge          , 1 }, { m.t_augmented_shields        , 2 },
    { m.t_deployed_shields      , 2 }, { m.t_electrified_railgun    , 3 },
    { m.t_plasma_probe          , 1 }
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
