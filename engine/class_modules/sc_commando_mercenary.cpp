// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "bount_troop.hpp"

namespace commando_mercenary { // ===========================================

class class_t;

struct targetdata_t : public bount_troop::targetdata_t
{

  dot_t dot_rapid_shots;
  dot_t dot_unload;
  dot_t dot_vent_heat;

  // TODO this applies arpen to target of 4% per stack
  buff_t* debuff_heat_signature;

  targetdata_t( class_t& source, player_t& target );
};

// ==========================================================================
// Commando | Mercenary
// ==========================================================================

struct class_t : public bount_troop::class_t
{
    typedef bount_troop::class_t base_t;
    // Buffs
    struct buffs_t
    {
      buff_t* barrage;
      buff_t* critical_reaction;
      buff_t* high_velocity_gas_cylinder;
      buff_t* power_barrier;
      buff_t* tracer_lock;

    } buffs;

    // Gains
    struct gains_t
    {
      gain_t* improved_vents;
      gain_t* terminal_velocity;
      gain_t* vent_heat;

    } gains;

    // Procs
    struct procs_t
    {
      proc_t* terminal_velocity;
    } procs;

    // RNGs
    struct rngs_t
    {
      rng_t* terminal_velocity;
    } rngs;

    // Benefits
    struct benefits_t
    {

    } benefits;

    // Cooldowns
    struct cooldowns_t
    {

    } cooldowns;

    // Talents
    struct talents_t
    {
      // Bodyguard
      // t1
      talent_t* improved_vents;
      talent_t* med_tech;
      talent_t* hired_muscle;
      // t2
      talent_t* empowered_scans;
      talent_t* surgical_precision_system;
      talent_t* supercharged_gas;
      talent_t* critical_reaction;
      // t3
      talent_t* heat_damping;
      talent_t* kolto_residue;
      talent_t* kolto_missile;
      talent_t* power_shield;
      // t4
      talent_t* powered_insulators;
      talent_t* critical_efficiency;
      talent_t* protective_field;
      // t5
      talent_t* reactive_armor;
      talent_t* proactive_medicine;
      talent_t* kolto_shell;
      talent_t* cure_mind;
      // t6
      talent_t* warden;
      // t7
      talent_t* emergency_scan;

      // Arsenal
      // t1
      talent_t* mandalorian_iron_warheads;
      talent_t* integrated_systems;
      talent_t* ironsights;
      // t2
      talent_t* stabilizers;
      talent_t* muzzle_fluting;
      talent_t* upgraded_arsenal;
      talent_t* custom_enviro_suit;
      // t3
      talent_t* power_barrier;
      talent_t* afterburners;
      talent_t* tracer_missile;
      talent_t* target_tracking;
      // t4
      talent_t* jet_escape;
      talent_t* light_em_up;
      talent_t* terminal_velocity;
      // t5
      talent_t* pinning_fire;
      talent_t* riddle;
      talent_t* tracer_lock;
      talent_t* kolto_vents;
      // t6
      talent_t* barrage;
      talent_t* power_overrides;
      // t7
      talent_t* heatseeker_missiles;

      // Pyrotech
      // t1
      talent_t* advanced_targeting;
      talent_t* system_calibrations;
      talent_t* integrated_cardio_package;
      // t2
      talent_t* superheated_gas;
      talent_t* sweltering_heat;
      talent_t* gyroscopic_alignment_jets;
      // t3
      talent_t* degauss;
      talent_t* superheated_rail;
      talent_t* incendiary_missile;
      talent_t* infrared_sensors;
      // t4
      talent_t* rapid_venting;
      talent_t* prototype_particle_accelerator;
      // t5
      talent_t* energy_rebounder;
      talent_t* rain_of_fire;
      talent_t* firebug;
      // t6
      talent_t* automated_defenses;
      talent_t* burnout;
      // t7
      talent_t* thermal_detonator;

    } talents;

    class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
      base_t( sim, pt == BH_MERCENARY ? BH_MERCENARY : T_COMMANDO, name, rt ),
      buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents()
    {

      tree_type[ BH_MERCENARY_BODYGUARD   ] = TREE_BODYGUARD;
      tree_type[ BH_MERCENARY_ARSENAL     ] = TREE_ARSENAL;
      tree_type[ BH_MERCENARY_PYROTECH    ] = TREE_PYROTECH;

      create_talents();
      create_options();
    }

    // Character Definition
    virtual targetdata_t* new_targetdata( player_t& target ) // override
    { return new targetdata_t( *this, target ); }

            void      create_talents();
    virtual action_t* create_action( const std::string& name, const std::string& options );
    virtual void      init_talents();
    virtual void      init_base();
    virtual void      init_benefits();
    virtual void      init_buffs();
    virtual void      init_gains();
    virtual void      init_procs();
    virtual void      init_rng();
    virtual void      init_actions();
    virtual role_type primary_role() const;
    virtual double    alacrity() const;
    virtual double    armor_penetration() const;

};

targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  bount_troop::targetdata_t( source, target ),
  dot_rapid_shots ( "rapid_shots" , &source ), 
  dot_unload      ( "unload"      , &source ), 
  dot_vent_heat   ( "vent_heat"   , &source ), 
  debuff_heat_signature( new buff_t( this, "heat_signature", 5, from_seconds( 15 ) ) )
{
  add ( dot_rapid_shots        ) ;
  add ( dot_unload             ) ;
  add ( dot_vent_heat          ) ;
  add ( *debuff_heat_signature ) ;
}

class action_t : public ::action_t
{
  typedef ::action_t base_t;
public:
  action_t( const std::string& n, class_t* player,
            attack_policy_t policy, resource_type r, school_type s ) :
    base_t( ACTION_ATTACK, n, player, policy, r, s )
  {
    harmful = false;
  }

  targetdata_t* targetdata() const
  { return static_cast<targetdata_t*>( base_t::targetdata() ); }

  class_t* p() const
  { return static_cast<class_t*>( player ); }
  class_t* cast() const { return p(); }
};

// ==========================================================================
// Commando / Mercenary Abilities
// ==========================================================================

// 4 types of attack: melee, ranged, force, tech. denoted by policy.
// 4 types of damage: kinetic, energy, internal, elemental.
// all combinations possible, but usually patterend.


// action hierarchy
// action - attack

struct attack_t : public action_t
{
    attack_t( const std::string& n, class_t* p, attack_policy_t policy, school_type s ) :
      action_t( n, p, policy, RESOURCE_HEAT, s )
    {
      harmful       = true;
      may_crit      = true;
      tick_may_crit = true;
    }

  virtual void execute()
  {
    action_t::execute();

    class_t* p = cast();
    if ( result == RESULT_CRIT && p -> talents.critical_reaction -> rank() )
      p -> buffs.critical_reaction->trigger();
  }
};

struct terminal_velocity_attack_t : public attack_t
{
    terminal_velocity_attack_t( class_t* p, const std::string& n, attack_policy_t policy, school_type s ) :
      attack_t( n, p, policy, s)
  {
  }

  virtual void execute()
  {
    attack_t::execute();
    terminal_velocity();
  }

  virtual void tick( dot_t* d )
  {
    attack_t::tick( d );
    terminal_velocity();
  }


  virtual void terminal_velocity()
  {
    class_t* p = cast();
    if (
        result == RESULT_CRIT
        && p -> rngs.terminal_velocity -> roll ( p -> talents.terminal_velocity -> rank() * 0.5 )
        && ( sim -> current_time - p -> procs.terminal_velocity -> last_proc ) > from_seconds( 3.0 )
       )
    {
      p -> resource_gain( RESOURCE_HEAT, 8, p -> gains.terminal_velocity );
      p -> procs.terminal_velocity -> occur();
    }

  }
};

struct missile_attack_t : public terminal_velocity_attack_t
{
    missile_attack_t( class_t* p, const std::string& n ) :
      terminal_velocity_attack_t( p, n, range_policy, SCHOOL_KINETIC)
  {
  }

  virtual void player_buff()
  {
    terminal_velocity_attack_t::player_buff();
    if ( unsigned rank = p() -> talents.mandalorian_iron_warheads -> rank() )
      player_multiplier += 0.03 * rank;
  }
};

// MERC ABILITIES
// class_t::chaff_flare ===================================================================
// class_t::combat_support_cylinder =======================================================
// class_t::concussion_missile ============================================================
// class_t::cure ==========================================================================
// class_t::emergency_scan ================================================================
// class_t::fusion_missile ================================================================
// class_t::healing_scan ==================================================================
// class_t::heatseeker_missiles ===========================================================
struct heatseeker_missiles_t : public missile_attack_t
{
  heatseeker_missiles_t( class_t* p, const std::string& n, const std::string& options_str) :
    missile_attack_t( p, n )
  {
    // TODO
    // rank_level_list = { ... 50 }

    parse_options( options_str );

    base_cost                    = 16;
    cooldown -> duration         = from_seconds( 15 );
    range                        = 30.0;

    dd.power_mod                 = 2.08;
    dd.standardhealthpercentmin  = 0.168;
    dd.standardhealthpercentmax  = 0.248;

    // TODO TEST: crit_bonus, crit_multiplier, or crit_bonus_multiplier? assuming additive to surge
    crit_bonus                  += 0.15 * p -> talents.target_tracking -> rank();
  }

  virtual void target_debuff( player_t* tgt, dmg_type type )
  {
    missile_attack_t::target_debuff( tgt, type );
    if ( unsigned stacks = p() -> buffs.tracer_lock -> stack() )
      target_multiplier += 0.05 * stacks;
  }

  virtual void execute()
  {
    p() -> buffs.tracer_lock -> expire();
    missile_attack_t::execute();
  }
};

// class_t::high_velocity_gas_cylinder ====================================================
struct high_velocity_gas_cylinder_t : public action_t
{
  high_velocity_gas_cylinder_t( class_t* p, const std::string& n, const std::string& options_str ) :
    action_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    parse_options( options_str );
    base_execute_time = from_seconds( 1.5 );
  }

  virtual void execute()
  {
    action_t::execute();
    class_t* p = cast();

    p -> buffs.high_velocity_gas_cylinder -> trigger();
    // TODO when implemented p -> buffs.combustible_gas_cylinder -> expire();
  }
};

// class_t::incendiary_missile ============================================================
// class_t::jet_boost =====================================================================
// class_t::kolto_missile =================================================================
// class_t::kolto_shell ===================================================================
// class_t::onboard_aed ===================================================================

// class_t::power_shot ====================================================================
// Charges up both blasters and fires off two powerful shots. Requires two blasters.
struct power_shot_t : public attack_t
{
  power_shot_t( class_t* p, const std::string& n, const std::string& options_str) :
    // TODO check unsure of policy and school
    attack_t( n, p, range_policy, SCHOOL_ENERGY )
  {
    // TODO
    // rank_level_list = { ... 50 }

    parse_options( options_str );

    base_cost = 16;
    range = 30.0;

    // TODO CHECK torhead says slots->[SecondaryRanged] but what about MH hit?
    // i think there's a lot missing from this ability on torhead. moving on...
    weapon = &( player -> off_hand_weapon );
    weapon_multiplier = 0.15;
  }

  virtual void execute()
  {
    attack_t::execute();

    class_t& p = *cast();

    if ( result_is_hit() && p.talents.barrage -> rank() )
        p.buffs.barrage -> trigger();

    // TODO
    // 2nd shot
  }
};

// class_t::power_surge ===================================================================
// class_t::rapid_scan ====================================================================
// class_t::supercharged_gas ==============================================================
// class_t::sweeping_blasters =============================================================
// class_t::tracer_missile ================================================================
struct tracer_missile_t : public missile_attack_t
{
  tracer_missile_t( class_t* p, const std::string& n, const std::string& options_str) :
    missile_attack_t( p, n )
  {
    // TODO
    // rank_level_list = { ... 50 }
    check_talent( p -> talents.tracer_missile -> rank() );

    parse_options( options_str );

    base_cost                   = 16;
    base_execute_time           = from_seconds( 2 - 0.5 * p -> talents.muzzle_fluting -> rank() );
    range                       = 30.0;
    // TODO uncomment.
    //travel_speed                = 18.4; // XXX guess. how to convert theirs to ours?
    dd.power_mod                = 1.71;
    dd.standardhealthpercentmin = 0.131;
    dd.standardhealthpercentmax = 0.211;

  }

  // FIXME is this the right place to check if a person has an ability?
  // Or should that go in the action list
  virtual bool ready()
  {
    if ( ! p() -> talents.tracer_missile -> rank() )
      return false;

    return missile_attack_t::ready();
  }

  virtual void execute()
  {
    missile_attack_t::execute();

    if ( result_is_hit() )
    {
      // XXX REVIEW can't this thing be put in action_t or something? it's C voodoo to me
      class_t& p = *cast();

      // BUG: debuff is ignoring travel time and incorrectly applying instantly
      targetdata() -> debuff_heat_signature -> trigger( p.talents.light_em_up -> rank() ? 2 : 1 );

      if ( p.talents.tracer_lock -> rank() )
        p.buffs.tracer_lock -> trigger();

      if ( p.talents.barrage -> rank() )
        p.buffs.barrage -> trigger();
    }
  }
};

// BH ABILITIES TODO move into bount_troop.hpp?

// class_t::combustible_gas_cylinder ======================================================
// class_t::death_from_above ==============================================================
// class_t::determination =================================================================
// class_t::electro_dart ==================================================================
// class_t::energy_shield =================================================================
// class_t::explosive_dart ================================================================
// class_t::flame_thrower =================================================================
// class_t::heroic_moment =================================================================
// class_t::kolto_overload ================================================================
// class_t::missile_blast =================================================================
// class_t::rail_shot =====================================================================
struct rail_shot_t : public attack_t
{
  rail_shot_t( class_t* p, const std::string& n, const std::string& options_str) :
    // ENERGY or tech?
    attack_t( n, p, range_policy, SCHOOL_ENERGY )
  {
    parse_options( options_str );

    base_cost                    =  16;
    cooldown -> duration         =  from_seconds( 15 );
    range                        =  30.0;
    dd.power_mod                 =  1.9;
    dd.standardhealthpercentmin  =  
    dd.standardhealthpercentmax  =  0.19;

    weapon                       =  &( player -> main_hand_weapon );
    weapon_multiplier            =  0.27;
  }

  virtual void player_buff()
  {
    attack_t::player_buff();
    class_t* p = cast();
    if ( p -> talents.upgraded_arsenal -> rank() && p -> buffs.high_velocity_gas_cylinder -> up() )
      base_cost = 8;
    else
      base_cost = 16;
  }
};

// class_t::rapid_shots ===================================================================
struct rapid_shots_t : public attack_t
{
  // TODO implement offhand shot
  rapid_shots_t( class_t* p, const std::string& n, const std::string& options_str) :
    // TODO check policy and school
    attack_t( n, p, range_policy, SCHOOL_ENERGY )
  {
    // TODO
    // rank_level_list = { ... 50 }
    parse_options( options_str );

    // TODO REVIEW
    // hitting way too hard. hits 5 times for each weapon for about 1/5 the damage. Can't work out how to translate the torhead data into these fields.
    range                       = 30.0;
    td.weapon                   = &( player -> main_hand_weapon );
    td.weapon_multiplier        = -0.8;
    td.power_mod                = 1;
    base_accuracy               -= 0.10;
    num_ticks                   = 5;
    base_tick_time              = from_seconds( 0.3 );
  }
};

// class_t::rocket_punch ==================================================================
// class_t::shoulder_slam =================================================================
// class_t::stealth_scan ==================================================================
// class_t::thermal_sensor_override =======================================================
// class_t::unload ========================================================================
struct unload_t : public terminal_velocity_attack_t
{
  // TODO offhand attack
  unload_t( class_t* p, const std::string& n, const std::string& options_str) :
    // TODO test range_policy and school energy?
    terminal_velocity_attack_t( p, n, range_policy, SCHOOL_ENERGY )
  {
    // TODO
    // rank_level_list = { ... 50 }

    parse_options( options_str );

    base_cost                   = 16;
    cooldown -> duration        = from_seconds( 15.0 );
    range                       = 30.0;
    channeled                   = true;
    td.standardhealthpercentmin = 
    td.standardhealthpercentmax = 0.0263;
    td.power_mod                = 0.263;
    // XXX REVIEW
    // added td.weapon & td.weapon_multiplier to sc_action_t instead of making it reference weapon
    // since direct_damage will work with just weapon set and did not want to break that behavior
    td.weapon                   = &( player -> main_hand_weapon );
    td.weapon_multiplier        = 0.82;
    num_ticks                   = 3;
    base_tick_time              = from_seconds( 1 );
    base_multiplier             = 1 + 0.33 * p -> talents.riddle -> rank();
  }

  // TODO CHECK barrage buff resets Unload cooldown? tracer_missile, or in unload::ready()
  virtual bool ready()
  {
    if ( p() -> buffs.barrage -> up() )
      reset();

    return terminal_velocity_attack_t::ready();
  }

  virtual void player_buff()
  {
    terminal_velocity_attack_t::player_buff();
    if ( p() -> buffs.barrage -> up() )
      player_multiplier += 0.25;
  }

  virtual void last_tick(dot_t* d)
  {
    terminal_velocity_attack_t::last_tick( d );
    p() -> buffs.barrage -> expire();
  }
};

// class_t::vent_heat =====================================================================
struct vent_heat_t : public action_t
{
  vent_heat_t( class_t* p, const std::string& n, const std::string& options_str ) :
    action_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {

    parse_options( options_str );

    cooldown -> duration = from_seconds( 120.0 );
    num_ticks            = 2;
    base_tick_time       = from_seconds( 3 );
    trigger_gcd          = timespan_t::zero();
  }

  virtual void execute()
  {
    action_t::execute();
    class_t* p = cast();

    // TODO check immediately gives 34 + 16 with talents
    p -> resource_gain( RESOURCE_HEAT, 34, p -> gains.vent_heat );
    if ( unsigned rank = p -> talents.improved_vents -> rank() )
      p -> resource_gain( RESOURCE_HEAT, 8 * rank, p -> gains.improved_vents );
  }

  virtual void tick(dot_t* d)
  {
    action_t::tick(d);
    class_t* p = cast();

    p -> resource_gain( RESOURCE_HEAT, 8, p -> gains.vent_heat );
  }
};

// ==========================================================================
// commando_mercenary Character Definition
// ==========================================================================

// TODO passives
// class_t::riddle increases unload damage by 33% passive?
// class_t::hired_protection reduced the cooldown of revive by 100%

// class_t::create_action =================================================================

::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
    if ( type == BH_MERCENARY )
    {
      if ( name == "heatseeker_missiles"        ) return new heatseeker_missiles_t        ( this, name, options_str );
      if ( name == "high_velocity_gas_cylinder" ) return new high_velocity_gas_cylinder_t ( this, name, options_str );
      if ( name == "power_shot"                 ) return new power_shot_t                 ( this, name, options_str );
      if ( name == "rail_shot"                  ) return new rail_shot_t                  ( this, name, options_str );
      if ( name == "rapid_shots"                ) return new rapid_shots_t                ( this, name, options_str );
      if ( name == "tracer_missile"             ) return new tracer_missile_t             ( this, name, options_str );
      if ( name == "unload"                     ) return new unload_t                     ( this, name, options_str );
      if ( name == "vent_heat"                  ) return new vent_heat_t                  ( this, name, options_str );
    }
    else if ( type == T_COMMANDO )
    {

    }

    return base_t::create_action( name, options_str );
}

// class_t::recalculate_alacrity =============================================================

double class_t::alacrity() const
{
  double sh = base_t::alacrity();

  if ( buffs.critical_reaction -> up() )
    sh -= 0.05;

  return sh;
}
// class_t::armor_penetration ================================================================

double class_t::armor_penetration() const
{
  double arpen = base_t::armor_penetration();

  if ( buffs.high_velocity_gas_cylinder -> up() )
    arpen += 0.35;

  return arpen;
}



// class_t::init_talents ==================================================================

void class_t::init_talents()
{
  base_t::init_talents();

  // Bodyguard
  // t1
  talents.improved_vents                 = find_talent( "Improved Vents" );
  talents.med_tech                       = find_talent( "Med Tech" );
  talents.hired_muscle                   = find_talent( "Hired Muscle" );
  // t2
  talents.empowered_scans                = find_talent( "Empowered Scans" );
  talents.surgical_precision_system      = find_talent( "Surgical Precision System" );
  talents.supercharged_gas               = find_talent( "Supercharged Gas" );
  talents.critical_reaction              = find_talent( "Critical Reaction" );
  // t3
  talents.heat_damping                   = find_talent( "Heat Damping" );
  talents.kolto_residue                  = find_talent( "Kolto Residue" );
  talents.kolto_missile                  = find_talent( "Kolto Missile" );
  talents.power_shield                   = find_talent( "Power Shield" );
  // t4
  talents.powered_insulators             = find_talent( "Powered Insulators" );
  talents.critical_efficiency            = find_talent( "Critical Efficiency" );
  talents.protective_field               = find_talent( "Protective Field" );
  // t5
  talents.reactive_armor                 = find_talent( "Reactive Armor" );
  talents.proactive_medicine             = find_talent( "Proactive Medicine" );
  talents.kolto_shell                    = find_talent( "Kolto Shell" );
  talents.cure_mind                      = find_talent( "Cure Mind" );
  // t6
  talents.warden                         = find_talent( "Warden" );
  // t7
  talents.emergency_scan                 = find_talent( "Emergency Scan" );

  // Arsenal
  // t1
  talents.mandalorian_iron_warheads      = find_talent( "Mandalorian Iron Warheads" );
  talents.integrated_systems             = find_talent( "Integrated Systems" );
  talents.ironsights                     = find_talent( "Ironsights" );
  // t2
  talents.stabilizers                    = find_talent( "Stabilizers" );
  talents.muzzle_fluting                 = find_talent( "Muzzle Fluting" );
  talents.upgraded_arsenal               = find_talent( "Upgraded Arsenal" );
  talents.custom_enviro_suit             = find_talent( "Custom Enviro Suit" );
  // t3
  talents.power_barrier                  = find_talent( "Power Barrier" );
  talents.afterburners                   = find_talent( "Afterburners" );
  talents.tracer_missile                 = find_talent( "Tracer Missile" );
  talents.target_tracking                = find_talent( "Target Tracking" );
  // t4
  talents.jet_escape                     = find_talent( "Jet Escape" );
  talents.light_em_up                    = find_talent( "Light 'Em Up" );
  talents.terminal_velocity              = find_talent( "Terminal Velocity" );
  // t5
  talents.pinning_fire                   = find_talent( "Pinning Fire" );
  talents.riddle                         = find_talent( "Riddle" );
  talents.tracer_lock                    = find_talent( "Tracer Lock" );
  talents.kolto_vents                    = find_talent( "Kolto Vents" );
  // t6
  talents.barrage                        = find_talent( "Barrage" );
  talents.power_overrides                = find_talent( "Power Overrides" );
  // t7
  talents.heatseeker_missiles            = find_talent( "Heatseeker Missiles" );

  // pyrotech
  // t1
  talents.advanced_targeting             = find_talent( "Advanced Targeting" );
  talents.system_calibrations            = find_talent( "System Calibrations" );
  talents.integrated_cardio_package      = find_talent( "Integrated Cardio Package" );
  // t2
  talents.superheated_gas                = find_talent( "Superheated Gas" );
  talents.sweltering_heat                = find_talent( "Sweltering Heat" );
  talents.gyroscopic_alignment_jets      = find_talent( "Gyroscopic Alignment Jets" );
  // t3
  talents.degauss                        = find_talent( "Degauss" );
  talents.superheated_rail               = find_talent( "Superheated Rail" );
  talents.incendiary_missile             = find_talent( "Incendiary Missile" );
  talents.infrared_sensors               = find_talent( "Infrared Sensors" );
  // t4
  talents.rapid_venting                  = find_talent( "Rapid Venting" );
  talents.prototype_particle_accelerator = find_talent( "Prototype Particle Accelerator" );
  // t5
  talents.energy_rebounder               = find_talent( "Energy Rebounder" );
  talents.rain_of_fire                   = find_talent( "Rain of Fire" );
  talents.firebug                        = find_talent( "Firebug" );
  // t6
  talents.automated_defenses             = find_talent( "Automated Defenses" );
  talents.burnout                        = find_talent( "Burnout" );
  // t7
  talents.thermal_detonator              = find_talent( "Thermal Detonator" );
}

// class_t::init_base =====================================================================

void class_t::init_base()
{
  base_t::init_base();

  default_distance = 20;
  distance = default_distance;

  attribute_multiplier_initial[ ATTR_AIM ] += 0.03 * talents.ironsights          -> rank();
  set_base_accuracy( get_base_accuracy()   +  0.01 * talents.advanced_targeting  -> rank() );
  set_base_alacrity( get_base_alacrity()   +  0.02 * talents.system_calibrations -> rank() );
  set_base_crit( get_base_crit()           +  0.02 *  talents.hired_muscle       -> rank() );
}

// class_t::init_benefits =================================================================

void class_t::init_benefits()
{
    base_t::init_benefits();
}

// class_t::init_buffs ====================================================================

void class_t::init_buffs()
{
  base_t::init_buffs();

  // buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  buffs.barrage           = new buff_t( this, "barrage",               1, from_seconds( 15 ), from_seconds( 6 ), (talents.barrage -> rank() * 0.15) );
  buffs.critical_reaction = new buff_t( this, "critical_reaction",     1, from_seconds( 6 ),  from_seconds( 0 ), (talents.critical_reaction -> rank() * 0.5) );

  buffs.tracer_lock = new buff_t( this, "tracer_lock", 5, from_seconds( 15 ) );

  buffs.high_velocity_gas_cylinder = new buff_t( this, "high_velocity_gas_cylinder", 1 );

}

// class_t::init_gains ====================================================================

void class_t::init_gains()
{
    base_t::init_gains();

    gains.improved_vents    = get_gain( "improved_vents" );
    gains.terminal_velocity = get_gain( "terminal_velocity" );
    gains.vent_heat         = get_gain( "vent_heat" );
}

// class_t::init_procs ====================================================================

void class_t::init_procs()
{
    base_t::init_procs();

    procs.terminal_velocity = get_proc( "Terminal Velocity" );
}

// class_t::init_rng ======================================================================

void class_t::init_rng()
{
  base_t::init_rng();

  rngs.terminal_velocity = get_rng( "terminal_velocity" );
}

// class_t::init_actions ==================================================================

void class_t::init_actions()
{
    //=====================================================================================
    //
    //   Please Mirror all changes between Empire/Republic
    //
    //=====================================================================================

    if ( action_list_str.empty() )
    {
        // Commando
        if ( type == T_COMMANDO )
        {
            action_list_str += "stim,type=exotech_resolve";
            action_list_str += "/snapshot_stats";

            switch ( primary_tree() )
            {


            default: break;
            }

            action_list_default = 1;
        }

        // Mercenary
        else
        {
            action_list_str += "stim,type=exotech_resolve";
            action_list_str += "/snapshot_stats";

            // testing

            // TODO check for arsenal
            action_list_str += "/high_velocity_gas_cylinder,if=!buff.high_velocity_gas_cylinder.up";
            // TODO check for talent improved vents
            action_list_str += "/vent_heat,if=heat<=65";
            // TODO check for barrage talent
            action_list_str += "/unload,if=heat>76&buff.barrage.up";
            // TODO check for talent
            action_list_str += "/heatseeker_missiles,if=heat>76&buff.tracer_lock.stack>=5";
            action_list_str += "/tracer_missile,if=heat>76";
            action_list_str += "/rail_shot,if=heat>76";
            // TODO check for talent tracer missile
            action_list_str += "/tracer_missile,if=heat>76";
            action_list_str += "/rapid_shots";

            switch ( primary_tree() )
            {

            default: break;
            }

            action_list_default = 1;
        }
    }

    base_t::init_actions();
}

// class_t::primary_role ===============================================================

role_type class_t::primary_role() const
{
  switch ( base_t::primary_role() )
  {
  case ROLE_TANK:
    return ROLE_TANK;
  case ROLE_DPS:
    return ROLE_DPS;
  case ROLE_HYBRID:
    return ROLE_HYBRID;
  default:
     if ( primary_tree() == TREE_BODYGUARD )
        return ROLE_HEAL;
     if ( primary_tree() == TREE_ARSENAL )
       return ROLE_DPS;
     if ( primary_tree() == TREE_PYROTECH )
       return ROLE_DPS;
    break;
  }

  return ROLE_HYBRID;
}

// class_t::create_talents ===============================================================

void class_t::create_talents()
{
  static const talentinfo_t bodyguard_tree[] = {
    // t1
    { "Improved Vents"    , 2 }, { "Med Tech"                 , 2 }, { "Hired Muscle"    , 3 },
    // t2
    { "Empowered Scans"   , 2 }, { "Surgical Precision System", 2 }, { "Supercharged Gas", 1 }, { "Critical Reaction", 2 },
    // t3
    { "Heat Damping"      , 1 }, { "Kolto Residue"            , 2 }, { "Kolto Missile"   , 1 }, { "Power Shield"     , 1 },
    // t4
    { "Powered Insulators", 2 }, { "Critical Efficiency"      , 3 }, { "Protective Field", 2 },
    // t5
    { "Reactive Armor"    , 1 }, { "Proactive Medicine"       , 2 }, { "Kolto Shell"     , 1 }, { "Cure Mind"        , 1 },
    // t6
    { "Warden"            , 5 },
    // t7
    { "Emergency Scan"    , 1 }
  };
  init_talent_tree( BH_MERCENARY_BODYGUARD, bodyguard_tree );

  static const talentinfo_t arsenal_tree[] = {
    // t1
    { "Mandalorian Iron Warheads", 2 }, { "Integrated Systems", 2 }, { "Ironsights"       , 3 },
    // t2
    { "Stabilizers"              , 3 }, { "Muzzle Fluting"    , 1 }, { "Upgraded Arsenal" , 1 }, { "Custom Enviro Suit", 2 },
    // t3
    { "Power Barrier"            , 2 }, { "Afterburners"      , 2 }, { "Tracer Missile"   , 1 }, { "Target Tracking"   , 2 },
    // t4
    { "Jet Escape"               , 2 }, { "Light 'Em Up"      , 1 }, { "Terminal Velocity", 2 },
    // t5
    { "Pinning Fire"             , 2 }, { "Riddle"            , 1 }, { "Tracer Lock"      , 1 }, { "Kolto Vents"       , 1 },
    // t6
    { "Barrage"                  , 3 }, { "Power Overrides"   , 2 },
    // t7
    { "Heatseeker Missiles"      , 1 }
  };
  init_talent_tree( BH_MERCENARY_ARSENAL, arsenal_tree );

  static const talentinfo_t pyrotech_tree[] = {
    // t1
    { "Advanced Targeting", 3 }, { "System Calibrations"           , 2 }, { "Integrated Cardio Package", 3 },
    // t2
    { "Superheated Gas"   , 3 }, { "Sweltering Heat"               , 2 }, { "Gyroscopic Alignment Jets", 2 },
    // t3
    { "Degauss"           , 2 }, { "Superheated Rail"              , 2 }, { "Incendiary Missile"       , 1 }, { "Infrared Sensors", 2 },
    // t4
    { "Rapid Venting"     , 2 }, { "Prototype Particle Accelerator", 3 },
    // t5
    { "Energy Rebounder"  , 2 }, { "Rain of Fire"                  , 3 }, { "Firebug"                  , 2 },
    // t6
    { "Automated Defenses", 2 }, { "Burnout"                       , 3 },
    // t7
    { "Thermal Detonator" , 1 }
  };
  init_talent_tree( BH_MERCENARY_PYROTECH, pyrotech_tree );
}

} // namespace commando_mercenary============================================

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_commando  ===============================================

player_t* player_t::create_commando( sim_t* sim, const std::string& name, race_type r )
{
    return new commando_mercenary::class_t( sim, T_COMMANDO, name, r );
}

// player_t::create_BH_MERCENARY  ==========================================

player_t* player_t::create_mercenary( sim_t* sim, const std::string& name, race_type r )
{
    return new commando_mercenary::class_t( sim, BH_MERCENARY, name, r );
}

// player_t::commando_mercenary_init ===========================================

void player_t::commando_mercenary_init( sim_t* sim )
{
  for ( player_t* p : sim -> actor_list )
  {
    p -> buffs.fortification_hunters_boon = new buff_t( p, "fortification_hunters_boon", 1 );
    p -> buffs.fortification_hunters_boon -> constant = true;
  }
}

// player_t::commando_mercenary_combat_begin ===================================

void player_t::commando_mercenary_combat_begin( sim_t* sim )
{
  for ( player_t* p = sim -> player_list; p; p = p -> next )
  {
    if ( p -> ooc_buffs() )
    {
      if ( sim -> overrides.fortification_hunters_boon )
        p -> buffs.fortification_hunters_boon -> override();
    }
  }
}
