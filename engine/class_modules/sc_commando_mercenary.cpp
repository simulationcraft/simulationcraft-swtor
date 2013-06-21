// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "bount_troop.hpp"

namespace commando_mercenary { // ===========================================

class class_t;

struct targetdata_t : public bount_troop::targetdata_t
{

  dot_t dot_electro_net;
  dot_t dot_radiation_burns;
  dot_t dot_rapid_shots;
  dot_t dot_rapid_shots_offhand;
  dot_t dot_unload;
  dot_t dot_unload_offhand;
  dot_t dot_vent_heat;

  buff_t* debuff_electro_net;
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
//      buff_t* power_barrier;
      buff_t* thermal_sensor_override;
      buff_t* tracer_lock;
      buff_t* improved_vents;

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

    } rngs;

    // Benefits
    struct benefits_t
    {

    } benefits;

    // Cooldowns
    struct cooldowns_t
    {
      cooldown_t* unload;
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
      talent_t* heat_damping;
      talent_t* critical_reaction;
      // t3
      talent_t* kolto_pods;
      talent_t* kolto_residue;
      talent_t* supercharged_gas;
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
      talent_t* peacekeeper;
      // t7
      talent_t* kolto_jets;
      talent_t* bodyguard;
      // t8
      talent_t* emergency_scan;

      // Arsenal
      // t1
      talent_t* mandalorian_iron_warheads;
      talent_t* stabilizers;
      talent_t* ironsights;
      // t2
      talent_t* power_barrier;
      talent_t* upgraded_arsenal;
      talent_t* custom_enviro_suit;
      // t3
      talent_t* afterburners;
      talent_t* tracer_missile;
      talent_t* target_tracking;
      // t4
      talent_t* jet_escape;
      talent_t* tracer_lock;
      talent_t* terminal_velocity;
      // t5
      talent_t* pinning_fire;
      talent_t* riddle;
      talent_t* light_em_up;
      talent_t* torque_boosters;
      // t6
      talent_t* barrage;
      talent_t* power_overrides;
      talent_t* decoy;
      // t7
      talent_t* energy_rebounder;
      talent_t* power_launcher;
      // t8
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
      talent_t* pyro_shield;
      talent_t* superheated_rail;
      talent_t* incendiary_missile;
      talent_t* infrared_sensors;
      // t4
      talent_t* suit_foe;
      talent_t* prototype_particle_accelerator;
      talent_t* volatile_warhead;
      // t5
      talent_t* firebug;
      talent_t* rain_of_fire;
      // t6
      talent_t* degauss;
      talent_t* burnout;
      // t7
      talent_t* rapid_venting;
      talent_t* power_barrels;
      talent_t* jet_rebounder;
      // t8
      talent_t* thermal_detonator;

    } talents;

    timespan_t next_terminal_velocity;

    class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
      base_t( sim, pt == BH_MERCENARY ? BH_MERCENARY : T_COMMANDO, name, rt ),
      buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents()
    {
      if ( type == BH_MERCENARY )
      {
        tree_type[ BH_MERCENARY_BODYGUARD   ] = TREE_BODYGUARD;
        tree_type[ BH_MERCENARY_ARSENAL     ] = TREE_ARSENAL;
        tree_type[ BH_MERCENARY_PYROTECH    ] = TREE_PYROTECH;
      }
      else if ( type == T_COMMANDO )
      {
        tree_type[ BH_MERCENARY_BODYGUARD   ] = TREE_BODYGUARD;
        tree_type[ BH_MERCENARY_ARSENAL     ] = TREE_ARSENAL;
        tree_type[ BH_MERCENARY_PYROTECH    ] = TREE_PYROTECH;
      }


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
    virtual void      init_cooldowns();
    virtual void      init_actions();
    virtual role_type primary_role() const;
    virtual void      regen( timespan_t periodicity );
    virtual double    alacrity() const;
    virtual double    armor_penetration() const;
    virtual void      reset();
};

targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  bount_troop::targetdata_t ( source                , target  ),
  dot_electro_net           ( "electro_net"         , &source ),
  dot_radiation_burns       ( "radiation_burns"     , &source ),
  dot_rapid_shots           ( "rapid_shots"         , &source ),
  dot_rapid_shots_offhand   ( "rapid_shots_offhand" , &source ),
  dot_unload                ( "unload"              , &source ),
  dot_unload_offhand        ( "unload_offhand"      , &source ),
  dot_vent_heat             ( "vent_heat"           , &source ),
  debuff_electro_net    ( new buff_t( this, "electro_net",    5, from_seconds( 10 ) ) ),
  debuff_heat_signature ( new buff_t( this, "heat_signature", 1, from_seconds( 45 ) ) )
{
  add ( dot_electro_net         );
  add ( dot_radiation_burns     );
  add ( dot_rapid_shots         );
  add ( dot_rapid_shots_offhand );
  add ( dot_unload              );
  add ( dot_unload_offhand      );
  add ( dot_vent_heat           );
  add ( *debuff_electro_net     );
  add ( *debuff_heat_signature  );
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

  virtual double cost() const
  {
    double c = base_t::cost();
    class_t* p = cast();
    if (  c > 0.0 && p -> buffs.thermal_sensor_override -> up() )
      c = 0;

    return c;
  }

  bool ignore_thermal_sensor_override()
  {
    return false;
  }

  virtual void consume_resource()
  {
    base_t::consume_resource();
    class_t* p = cast();
    if ( !ignore_thermal_sensor_override() && base_cost && p -> buffs.thermal_sensor_override -> up() )
      p -> buffs.thermal_sensor_override -> expire();
    return;
  }
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

  // these should be callbacks
  virtual void execute()
  {
    action_t::execute();

    class_t* p = cast();
    if ( result == RESULT_CRIT && direct_dmg > 0 && p -> talents.critical_reaction -> rank() )
      p -> buffs.critical_reaction->trigger();
  }

  virtual void tick( dot_t* d )
  {
    action_t::tick( d );

    class_t* p = cast();
    if ( result == RESULT_CRIT && tick_dmg > 0 && p -> talents.critical_reaction -> rank() )
      p -> buffs.critical_reaction->trigger();
  }
};

struct missile_attack_t : public attack_t
{
    missile_attack_t( class_t* p, const std::string& n ) :
      attack_t( n, p, tech_policy, SCHOOL_KINETIC)
  {
    base_multiplier += 0.03 * p -> talents.mandalorian_iron_warheads -> rank();
  }
};

// MERC ABILITIES
// class_t::chaff_flare ===================================================================
// class_t::combat_support_cylinder =======================================================
// class_t::concussion_missile ============================================================
// class_t::cure ==========================================================================
// class_t::disabling_shot ================================================================
// class_t::emergency_scan ================================================================
// class_t::electro_net ================================================================
struct electro_net_t : public attack_t
{
  electro_net_t( class_t* p, const std::string& n, const std::string& options_str) :
    attack_t( n, p, tech_policy, SCHOOL_ENERGY )
  {
//    rank_level_list = { ... 55 };

    parse_options( options_str );

    base_cost                    = 8;
    range                        = 30.0;
    td.power_mod                 = 0.29;
    td.standardhealthpercentmin  = 0.029;
    td.standardhealthpercentmax  = 0.029;
    num_ticks                    = 10;
    tick_zero                    = true;
    base_tick_time               = from_seconds( 1 );
    cooldown -> duration         = from_seconds( 90 );

    base_multiplier += 0.02 * p -> talents.power_launcher -> rank();
  }

  virtual void execute()
  {
    targetdata() -> debuff_electro_net -> expire();
    attack_t::execute();
  }

  virtual void tick( dot_t* d )
  {
    attack_t::tick( d );
    targetdata() -> debuff_electro_net -> trigger();  // Triggers after the dot ticks because the first tick doesn't benefit from the increased damage.
  }

  virtual void player_buff()
  {
    attack_t::player_buff();
    player_multiplier += 0.02 * targetdata() -> debuff_electro_net -> stack() ;
  }
};

// class_t::fusion_missile ================================================================
struct fusion_missile_t : public missile_attack_t
{
  struct radiation_burns_t : public attack_t
  {
    radiation_burns_t( class_t* p, const std::string& n ) :
      attack_t( n, p, tech_policy, SCHOOL_ELEMENTAL )
    {
      // TODO: Check valuse as the tool-tip values are around 1% off assuming (Without Mand Iron Warheads).
      td.power_mod                = 0.28;
      td.standardhealthpercentmin = td.standardhealthpercentmax = 0.028;

      num_ticks                   = 6;
      base_tick_time              = from_seconds( 1 );
      background                  = true;
//      base_accuracy               = 999;  // Not required???
    }
  };

  radiation_burns_t* radiation_burns;

  fusion_missile_t( class_t* p, const std::string& n, const std::string& options_str ) :
    missile_attack_t( p, n ),
    radiation_burns( new radiation_burns_t( p, "radiation_burns" ) )
  {

    parse_options( options_str );

    base_cost                    = 33;
    base_execute_time            = from_seconds( 1.5 );
    cooldown -> duration         = from_seconds( 30 );
    range                        = 30.0;
    travel_speed                 = 2 * 9;

    // TODO: Check valuse as the tool-tip values are around 1% off assuming Mand Iron Warheads is NOT affecting the values as suggested by the bug line below. All other missile tool-tips are spot on.
    dd.power_mod                 = 1.4;
    dd.standardhealthpercentmin  = 0.1;
    dd.standardhealthpercentmax  = 0.16;

    if ( p -> bugs )
      // revert bonus from missile_attack_t tested in 1.3 by freehugs
      base_multiplier -= 0.03 * p -> talents.mandalorian_iron_warheads -> rank();

    aoe                          = 3;

    add_child(radiation_burns);
  }

  virtual void impact( player_t* t, result_type impact_result, double travel_dmg )
  {
    missile_attack_t::impact( t, impact_result, travel_dmg );
    if ( result_is_hit( impact_result ) )
      radiation_burns -> execute();
  }
};

// class_t::healing_scan ==================================================================
// class_t::heatseeker_missiles ===========================================================
struct heatseeker_missiles_t : public missile_attack_t
{
  typedef missile_attack_t base_t;
  heatseeker_missiles_t( class_t* p, const std::string& n, const std::string& options_str) :
    base_t( p, n )
  {
    // TODO
    // rank_level_list = { ... 55 }

    check_talent( p -> talents.heatseeker_missiles -> rank() );

    parse_options( options_str );

    base_cost                    = 16;
    cooldown -> duration         = from_seconds( 15 );
    range                        = 30.0;
    travel_speed                 = 2 * 9;

    dd.power_mod                 = 2.77;  // No longer set in execute - Looks like it's now just a standard 25% damage increase from the effect details.
    dd.standardhealthpercentmin  = 0.257; // No longer set in execute - See above
    dd.standardhealthpercentmax  = 0.297; // No longer set in execute - See above

    crit_bonus                  += 0.15 * p -> talents.target_tracking -> rank();
    base_multiplier             += 0.02 * p -> talents.power_launcher -> rank();
  }

  virtual void player_buff()
  {
    missile_attack_t::player_buff();

    // TEST:
    // assuming damage is calculated on cast, not impact.
    // could be
    // 1: damage on cast, so target debuffs during travel time won't be included
    // 2: damage on impact, so player changes during travel time will be included
    // 3: player stats on cast, target damage on impact
    // 3 would be the purest, but for simplicity going with 1. Unsure which the game does.
    if ( targetdata() -> debuff_heat_signature -> check() )
      player_multiplier += 0.25;
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
struct power_shot_t : public attack_t
{
  power_shot_t* offhand_attack;

  power_shot_t( class_t* p, const std::string& n, const std::string& options_str,
      bool is_offhand = false ) :
    attack_t( n, p, range_policy, SCHOOL_ENERGY ),
    offhand_attack( 0 )
  {
    rank_level_list = { 10, 13, 16, 22, 33, 45, 50, 55 };

    parse_options( options_str );

    base_cost                    = 19;
    range                        = 30.0;
    base_execute_time            = from_seconds( 1.5 );
    weapon_multiplier            = 0.12;
    weapon                       = &( player -> main_hand_weapon );
    dd.power_mod                 = 1.67;  // Estimated since new val not in Torhead (was 1.71). Changind the ability level caused too much of a diffecence.
    dd.standardhealthpercentmin  =        // As above (was 0.171). Tooltip min value was off by 1.8%, now spot on.
    dd.standardhealthpercentmax  = 0.167; // As above (was 0.171). Tooltip max value was off by 1.5%, now spot on.

    base_crit                   += (( p -> set_bonus.rakata_eliminators -> two_pc() || p -> set_bonus.underworld_eliminators -> two_pc() ) ? 0.15 : 0 );
    base_multiplier             += 0.03 * p -> talents.mandalorian_iron_warheads -> rank();   // No need for this to be in player_buff()

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
    else
    {
      offhand_attack             = new power_shot_t( p, n+"_offhand", options_str, true );
      add_child( offhand_attack );
    }
  }

  virtual void execute()
  {
    attack_t::execute();
    class_t& p = *cast();

    if ( offhand_attack )
    {
      offhand_attack -> schedule_execute();

      if ( p.talents.barrage -> rank() && p.buffs.barrage -> trigger() )
        p.cooldowns.unload -> reset( sim->reaction_time ); // p.total_reaction_time() seems to have way too much variance
    }
  }
};

// class_t::power_surge ===================================================================
// TODO 120s cooldown reduced by Power Overrides. Next ability with a cast time is instant. 50%/100% for a second instant ability depending on

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
    // rank_level_list = { ... 55 }
    check_talent( p -> talents.tracer_missile -> rank() );

    parse_options( options_str );

    base_cost                    = 19;
    base_execute_time            = from_seconds( 1.5 );
    range                        = 30.0;
    travel_speed                 = 2.5 * 9; // XXX guess. how to convert theirs to ours? FIXED? It takes 1.33 seconds to travel the full 30m so multiply their value of 2.5 by 9 (actually 10 because of animation delay?)
    dd.power_mod                 = 1.75;
    dd.standardhealthpercentmin  = 0.125;
    dd.standardhealthpercentmax  = 0.225;

    base_crit                   += (( p -> set_bonus.rakata_eliminators -> two_pc() || p -> set_bonus.underworld_eliminators -> two_pc() ) ? 0.15 : 0 );
    base_multiplier             += 0.02 * p -> talents.power_launcher -> rank();
  }

  virtual double cost() const
  {
    double c = missile_attack_t::cost();
    class_t* p = cast();
    int arsenal_rank = p -> talents.upgraded_arsenal -> rank();
    if ( arsenal_rank && p -> buffs.high_velocity_gas_cylinder -> check() )
      c -= 1 * arsenal_rank;

    return c;
  }

  virtual void execute()
  {
    missile_attack_t::execute();

    class_t& p = *cast();

    if ( p.talents.tracer_lock -> rank() )
      p.buffs.tracer_lock -> trigger( ( p.talents.light_em_up -> rank() ) ? 2 : 1 );

    if ( p.talents.barrage -> rank() && p.buffs.barrage -> trigger() )
      p.cooldowns.unload -> reset( sim->reaction_time ); // p.total_reaction_time() seems to have way too much variance
  }

  virtual void impact( player_t* t, result_type impact_result, double travel_dmg )
  {
    missile_attack_t::impact( t, impact_result, travel_dmg );
    if ( result_is_hit( impact_result ) )
      targetdata() -> debuff_heat_signature -> trigger();
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
    attack_t( n, p, range_policy, SCHOOL_ENERGY )
  {
    rank_level_list = { 9, 12, 15, 20, 31, 42, 50, 55 };

    parse_options( options_str );

    base_cost                    =  16 - ( p -> set_bonus.rakata_eliminators -> four_pc() ? 8 : 0 );
    cooldown -> duration         =  from_seconds( 15 );
    range                        =  30.0;
    dd.power_mod                 =  1.9;
    dd.standardhealthpercentmin  =  
    dd.standardhealthpercentmax  =  0.19;
    weapon                       =  &( player -> main_hand_weapon );
    weapon_multiplier            =  0.27;

    base_multiplier             += 0.02 * p -> talents.power_launcher -> rank();
  }

  virtual void player_buff()
  {
    attack_t::player_buff();
    class_t* p = cast();

    if ( p -> talents.tracer_lock -> rank() )
      player_multiplier += 0.06 * p -> buffs.tracer_lock -> stack();

    if ( unsigned rank = p -> talents.advanced_targeting -> rank () )
      player_armor_penetration -= 0.1 * rank;

    if ( p -> set_bonus.battlemaster_eliminators -> four_pc() )
      player_crit += 0.15;

    if ( p -> set_bonus.underworld_eliminators -> four_pc() )
      player_multiplier += 0.08;

  }

  virtual double cost() const
  {
    double c = attack_t::cost();
    class_t* p = cast();
    int arsenal_rank = p -> talents.upgraded_arsenal -> rank();
    if ( arsenal_rank && p -> buffs.high_velocity_gas_cylinder -> check() )
      c -= 2 * arsenal_rank;

    return c;
  }

  virtual void execute()
  {
    attack_t::execute();
    p() -> buffs.tracer_lock -> expire();
  }

//  bool ignore_thermal_sensor_override()
//  {
//    // XXX TODO: This will never be free now since you can only reduce Rail Shots cost to 2 heat.
//    return true;
//  }
};

// class_t::rapid_shots ===================================================================
struct rapid_shots_t : public attack_t
{
  rapid_shots_t* offhand_attack;

  rapid_shots_t( class_t* p, const std::string& n, const std::string& options_str,
      bool is_offhand = false ) :
    attack_t( n, p, range_policy, SCHOOL_ENERGY ), offhand_attack( 0 )
  {
    // 0 grants no bonus from level.
    rank_level_list = { 0 };

    parse_options( options_str );

    range                       = 30.0;
    base_accuracy              -= 0.10;
    num_ticks                   = 5;
    base_tick_time              = from_seconds( 0.1 );     // Checked in combat log for varying alacrity values.
    // For each alacrity value all 10 ticks occur on average within 400ms of each other making it 500ms adding on the initial tick delay.
    // Having it set to 0.3 also caused the last tick to be cut off if another rapid shots was cast directly after with any alacrity. More if alacrity was high enough.
    // This is because it is not 'channeled' or 'hasted_ticks' causing tick times to stay the same regardless of alacrity while the GCD is reduced.
    td.weapon                   = &( player -> main_hand_weapon );
    td.weapon_multiplier        = -0.8;
    td.power_mod                = 0.2;

    if ( is_offhand )
    {
      background                = true;
      dual                      = true;
      trigger_gcd               = timespan_t::zero();
      td.weapon                 = &( player -> off_hand_weapon );
      td.power_mod              = 0;
    }
    else
    {
      offhand_attack            = new rapid_shots_t( p, n+"_offhand", options_str, true );
      add_child( offhand_attack );
    }
  }

  virtual void execute()
  {
    attack_t::execute();
    if ( offhand_attack )
      offhand_attack->schedule_execute();
  }
};

// class_t::rocket_punch ==================================================================
// class_t::shoulder_slam =================================================================
// class_t::stealth_scan ==================================================================

// class_t::thermal_sensor_override =======================================================
struct thermal_sensor_override_t : public action_t
{
  thermal_sensor_override_t( class_t* p, const std::string& n, const std::string& options_str ) :
    action_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    parse_options( options_str );

    cooldown -> duration = from_seconds( 120 );
    trigger_gcd = timespan_t::zero();
  }

  virtual void execute()
  {
    action_t::execute();
    p() -> buffs.thermal_sensor_override -> trigger();
  }
};

// class_t::unload ========================================================================
struct unload_t : public attack_t
{
  unload_t* offhand_attack;
  bool benefit_from_barrage;

  unload_t( class_t* p, const std::string& n, const std::string& options_str,
      bool is_offhand = false ) :
    attack_t( n, p, range_policy, SCHOOL_ENERGY ), offhand_attack( 0 ),
    benefit_from_barrage(false)
  {
    rank_level_list = { 3, 6, 9, 12, 15, 21, 32, 44, 50, 55 };

    parse_options( options_str );

    base_cost                   = 16;
    range                       = 30.0;
    cooldown -> duration        = from_seconds( 15.0 );
    channeled                   = true;
    num_ticks                   = 3;
    base_tick_time              = from_seconds( 1 );
    td.standardhealthpercentmin =
    td.standardhealthpercentmax = 0.11;
    td.weapon                   = &( player -> main_hand_weapon );
    td.power_mod                = 1.1;
    td.weapon_multiplier        = -0.3;

    base_multiplier            += 0.33 * p -> talents.riddle -> rank();
    crit_bonus                 += 0.15 * p -> talents.target_tracking -> rank();

    if ( is_offhand )
    {
      background                = true;
      dual                      = true;
      // Setting channeled=true allows the OH dot to be interrupted the same way as the MH dot for higher priority attacks or low player skill.
      // This seems better than leaving it tick in all situations but still not ideal:
      // Low player skill interrupts on MH and OH will not be in synch and I doubt other cancel/interrupt mechanics will affect OH.
      // TODO: Modify dot_event to allow cancelling a dot to cancel related dots via action object.
      // Add a vitrual empty func to action/attack called on interrupting a dot that can be overridden here to cancel offhand dots? If not overidden functionality remains unchanged.
//      channeled                 = false;
      base_cost                 = 0;
      trigger_gcd               = timespan_t::zero();
      td.weapon                 = &( player -> off_hand_weapon );
      td.power_mod              = 0;
      rank_level_list           = { 0 };
    }
    else
    {
      offhand_attack            = new unload_t( p, n+"_offhand", options_str, true );
      add_child( offhand_attack );
    }
  }

  virtual void player_buff()
  {
    attack_t::player_buff();
    class_t* p = cast();

    if ( p -> buffs.barrage -> up() )
      player_multiplier += 0.25;

    if ( unsigned rank = p -> talents.advanced_targeting -> rank () )
      player_armor_penetration -= 0.1 * rank;

  }

  virtual void last_tick(dot_t* d)
  {
    attack_t::last_tick( d );
    if ( ! offhand_attack )
      p() -> buffs.barrage -> expire();
  }

  virtual void execute()
  {
    benefit_from_barrage = p() -> buffs.barrage -> check();   // Up used in player buff

    attack_t::execute();

    if ( offhand_attack )
      offhand_attack->schedule_execute();
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
    base_tick_time       = from_seconds( 1.5 );
    trigger_gcd          = timespan_t::zero();
  }

  virtual void execute()
  {
    action_t::execute();
    class_t* p = cast();

    p -> resource_loss( RESOURCE_HEAT, 34, p -> gains.vent_heat );
    if ( unsigned rank = p -> talents.improved_vents -> rank() )
    {
      p -> buffs.improved_vents -> trigger();
      p -> resource_loss( RESOURCE_HEAT, 8 * rank, p -> gains.improved_vents );
    }
  }

  virtual void tick(dot_t* d)
  {
    action_t::tick(d);
    class_t* p = cast();

    p -> resource_loss( RESOURCE_HEAT, 8, p -> gains.vent_heat );
  }
};

// ==========================================================================
// commando_mercenary Character Definition
// ==========================================================================

// TODO passives
// class_t::hired_protection reduced the cooldown of revive by 100%

// class_t::create_action =================================================================

::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
    if ( type == BH_MERCENARY )
    {
      if ( name == "electro_net"                ) return new electro_net_t                ( this, name, options_str );
      if ( name == "fusion_missile"             ) return new fusion_missile_t             ( this, name, options_str );
      if ( name == "heatseeker_missiles"        ) return new heatseeker_missiles_t        ( this, name, options_str );
      if ( name == "high_velocity_gas_cylinder" ) return new high_velocity_gas_cylinder_t ( this, name, options_str );
      if ( name == "power_shot"                 ) return new power_shot_t                 ( this, name, options_str );
      if ( name == "rail_shot"                  ) return new rail_shot_t                  ( this, name, options_str );
      if ( name == "rapid_shots"                ) return new rapid_shots_t                ( this, name, options_str );
      if ( name == "thermal_sensor_override"    ) return new thermal_sensor_override_t    ( this, name, options_str );
      if ( name == "tracer_missile"             ) return new tracer_missile_t             ( this, name, options_str );
      if ( name == "vent_heat"                  ) return new vent_heat_t                  ( this, name, options_str );
      if ( name == "unload"                     ) return new unload_t                     ( this, name, options_str );
    }
    else if ( type == T_COMMANDO )
    {

    }

    return base_t::create_action( name, options_str );
}

// class_t::recalculate_alacrity =============================================================

double class_t::alacrity() const
{
  return  base_t::alacrity()
          + (buffs.high_velocity_gas_cylinder -> up() ? 0.03 : 0)
          + (buffs.critical_reaction -> up()          ? 0.03 : 0)
          + (buffs.improved_vents -> up()             ? 0.1  : 0);
}

// class_t::armor_penetration ================================================================

double class_t::armor_penetration() const
{
  return buffs.high_velocity_gas_cylinder -> up() ? base_t::armor_penetration() - 0.35 : base_t::armor_penetration();
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
  talents.heat_damping                   = find_talent( "Heat Damping" );
  talents.critical_reaction              = find_talent( "Critical Reaction" );
  // t3
  talents.kolto_pods                     = find_talent( "Kolto Pods" );
  talents.kolto_residue                  = find_talent( "Kolto Residue" );
  talents.supercharged_gas               = find_talent( "Supercharged Gas" );
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
  talents.peacekeeper                    = find_talent( "Peacekeeper" );
  // t7
  talents.kolto_jets                     = find_talent( "Kolto Jets" );
  talents.bodyguard                      = find_talent( "Bodyguard" );
  // t8
  talents.emergency_scan                 = find_talent( "Emergency Scan" );

  // Arsenal
  // t1
  talents.mandalorian_iron_warheads      = find_talent( "Mandalorian Iron Warheads" );
  talents.stabilizers                    = find_talent( "Stabilizers" );
  talents.ironsights                     = find_talent( "Ironsights" );
  // t2
  talents.power_barrier                  = find_talent( "Power Barrier" );
  talents.upgraded_arsenal               = find_talent( "Upgraded Arsenal" );
  talents.custom_enviro_suit             = find_talent( "Custom Enviro Suit" );
  // t3
  talents.afterburners                   = find_talent( "Afterburners" );
  talents.tracer_missile                 = find_talent( "Tracer Missile" );
  talents.target_tracking                = find_talent( "Target Tracking" );
  // t4
  talents.jet_escape                     = find_talent( "Jet Escape" );
  talents.tracer_lock                    = find_talent( "Tracer Lock" );
  talents.terminal_velocity              = find_talent( "Terminal Velocity" );
  // t5
  talents.pinning_fire                   = find_talent( "Pinning Fire" );
  talents.riddle                         = find_talent( "Riddle" );
  talents.light_em_up                    = find_talent( "Light 'Em Up" );
  talents.torque_boosters                = find_talent( "Torque Boosters" );
  // t6
  talents.barrage                        = find_talent( "Barrage" );
  talents.power_overrides                = find_talent( "Power Overrides" );
  talents.decoy                          = find_talent( "Decoy" );
  // t7
  talents.energy_rebounder               = find_talent( "Energy Rebounder" );
  talents.power_launcher                 = find_talent( "Power Launcher" );
  // t8
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
  talents.pyro_shield                    = find_talent( "Pyro Shield" );
  talents.superheated_rail               = find_talent( "Superheated Rail" );
  talents.incendiary_missile             = find_talent( "Incendiary Missile" );
  talents.infrared_sensors               = find_talent( "Infrared Sensors" );
  // t4
  talents.suit_foe                       = find_talent( "Suit FOE" );
  talents.prototype_particle_accelerator = find_talent( "Prototype Particle Accelerator" );
  talents.volatile_warhead               = find_talent( "Volatile Warhead" );
  // t5
  talents.firebug                        = find_talent( "Firebug" );
  talents.rain_of_fire                   = find_talent( "Rain of Fire" );
  // t6
  talents.degauss                        = find_talent( "Degauss" );
  talents.burnout                        = find_talent( "Burnout" );
  // t7
  talents.rapid_venting                  = find_talent( "Rapid Venting" );
  talents.power_barrels                  = find_talent( "Power Barrels" );
  talents.jet_rebounder                  = find_talent( "Jet Rebounder" );
  // t8
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
  set_base_alacrity( get_base_alacrity()   +  0.01 * talents.system_calibrations -> rank() );
  set_base_crit( get_base_crit()           +  0.01 * talents.hired_muscle        -> rank() );
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
  buffs.barrage                    = new buff_t( this, "barrage",                    1, from_seconds( 15 ), from_seconds( 6 ), (talents.barrage -> rank() * 0.15) );
  buffs.critical_reaction          = new buff_t( this, "critical_reaction",          1, from_seconds( 6 ),  from_seconds( 0 ), (talents.critical_reaction -> rank() * 0.5) );
  buffs.tracer_lock                = new buff_t( this, "tracer_lock",                5, from_seconds( 15 ) );
  buffs.thermal_sensor_override    = new buff_t( this, "thermal_sensor_override",    1, from_seconds( 15 ) );
  buffs.high_velocity_gas_cylinder = new buff_t( this, "high_velocity_gas_cylinder", 1 );
  buffs.improved_vents             = new buff_t( this, "improved_vents",             1, from_seconds( 6 ) );

}

// class_t::init_gains ====================================================================

void class_t::init_gains()
{
  base_t::init_gains();
  bool is_bh = ( type == BH_MERCENARY );
  const char* improved_vents    = is_bh ? "improved_vents"    : "cell_capacitor"  ;
  const char* terminal_velocity = is_bh ? "terminal_velocity" : "penetrate_armor" ;
  const char* vent_heat         = is_bh ? "vent_heat"         : "recharge_cells"  ;

  gains.improved_vents    = get_gain( improved_vents    );
  gains.terminal_velocity = get_gain( terminal_velocity );
  gains.vent_heat         = get_gain( vent_heat         );
}

// class_t::init_procs ====================================================================

void class_t::init_procs()
{
  base_t::init_procs();
  bool is_bh = ( type == BH_MERCENARY );
  const char* terminal_velocity = is_bh ? "Terminal Velocity" : "Penetrate Armor" ;

  procs.terminal_velocity = get_proc( terminal_velocity );
}

// class_t::init_cooldowns ================================================================

void class_t::init_cooldowns()
{
  base_t::init_cooldowns();

  bool is_bh = ( type == BH_MERCENARY );
  const char* unload = is_bh ? "unload"    : "???" ;
  cooldowns.unload = get_cooldown( unload );

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
      action_list_str += "stim,type=prototype_nano_infused_reflex";
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
      std::string heatseeker_condition = talents.heatseeker_missiles ? "&cooldown.heatseeker_missiles.remains>cast_time" : "";

      action_list_str += "stim,type=prototype_nano_infused_reflex";
      // ARSENAL
      action_list_str += "/high_velocity_gas_cylinder,if=!buff.high_velocity_gas_cylinder.up";
      action_list_str += "/snapshot_stats";     // After HVGS since it now adds a 3% alacritl buff as well.
      action_list_str += "/sequence,name=debuff:tracer_missile:tracer_missile";  // Only one is required for debuff but two is higher DPS. Maybe to avoid sitting on 0 heat after electro net?
      action_list_str += "/vent_heat,if=heat>=55";
      action_list_str += "/use_relics";
      action_list_str += "/power_potion";
      action_list_str += "/thermal_sensor_override,if=heat>25";
      action_list_str += "/fusion_missile,if=buff.thermal_sensor_override.up";
      if ( set_bonus.rakata_eliminators -> four_pc() )
        action_list_str += "/rail_shot";
      if ( talents.heatseeker_missiles)
        action_list_str += "/heatseeker_missiles,if=heat<40-cost|cooldown.vent_heat.remains<10";
      action_list_str += "/electro_net,if=heat<40-cost|cooldown.vent_heat.remains<10";
      if ( !set_bonus.rakata_eliminators -> four_pc() )
        action_list_str += "/rail_shot,if=buff.tracer_lock.stack>4&heat<40-cost|cooldown.vent_heat.remains<10";
      action_list_str += "/unload,if=heat<40-cost|cooldown.vent_heat.remains<10";
      if ( talents.tracer_missile )
        action_list_str += "/tracer_missile,if=heat<21|cooldown.vent_heat.remains<10|(heat<31" + heatseeker_condition + "&cooldown.unload.remains)";
      else
        action_list_str += "/power_shot,if=heat<21|cooldown.vent_heat.remains<10|(heat<31" + heatseeker_condition + "&cooldown.unload.remains)";
      action_list_str += "/rapid_shots";

      switch ( primary_tree() )
      {
      case TREE_ARSENAL:
        break;
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

// class_t::regen ===================================================

void class_t::regen( timespan_t periodicity )
{
  base_t::regen( periodicity );
  unsigned rank = talents.terminal_velocity -> rank();

  if (( rank ) && ( buffs.high_velocity_gas_cylinder -> check() ))
  {
    if ( next_terminal_velocity < sim -> current_time )
    {
      next_terminal_velocity += from_seconds( 6 );
      resource_loss( RESOURCE_HEAT, 4 * rank, gains.terminal_velocity );
      procs.terminal_velocity -> occur();
    }
  }
}

// class_t::create_talents ===============================================================

void class_t::create_talents()
{
  static const talentinfo_t bodyguard_tree[] = {
    // t1
    { "Improved Vents"    , 2 }, { "Med Tech"                 , 2 }, { "Hired Muscle"    , 3 },
    // t2
    { "Empowered Scans"   , 2 }, { "Surgical Precision System", 2 }, { "Heat Damping"    , 1 }, { "Critical Reaction", 2 },
    // t3
    { "Kolto Pods"        , 1 }, { "Kolto Residue"            , 2 }, { "Supercharged Gas", 1 }, { "Power Shield"     , 1 },
    // t4
    { "Powered Insulators", 2 }, { "Critical Efficiency"      , 3 }, { "Protective Field", 2 },
    // t5
    { "Reactive Armor"    , 1 }, { "Proactive Medicine"       , 2 }, { "Kolto Shell"     , 1 }, { "Cure Mind"        , 1 },
    // t6
    { "Warden"            , 3 }, { "Peacekeeper"              , 2 },
    // t7
    { "Kolto Jets"        , 2 }, { "Bodyguard"                , 3 },
    // t8
    { "Emergency Scan"    , 1 }
  };
  init_talent_tree( BH_MERCENARY_BODYGUARD, bodyguard_tree );

  static const talentinfo_t arsenal_tree[] = {
    // t1
    { "Mandalorian Iron Warheads", 2 }, { "Stabilizers"       , 2 }, { "Ironsights"        , 3 },
    // t2
    { "Power Barrier"            , 2 }, { "Upgraded Arsenal"  , 3 }, { "Custom Enviro Suit", 2 },
    // t3
    { "Afterburners"             , 2 }, { "Tracer Missile"    , 1 }, { "Target Tracking"   , 2 },
    // t4
    { "Jet Escape"               , 2 }, { "Tracer Lock"       , 1 }, { "Terminal Velocity" , 2 },
    // t5
    { "Pinning Fire"             , 2 }, { "Riddle"            , 1 }, { "Light 'Em Up"      , 1 }, { "Torque Boosters"  , 1 },
    // t6
    { "Barrage"                  , 3 }, { "Power Overrides"   , 2 }, { "Decoy"             , 2 },
    // t7
    { "Energy Rebounder"         , 2 }, { "Power Launcher"   , 3 },
    // t8
    { "Heatseeker Missiles"      , 1 }
  };
  init_talent_tree( BH_MERCENARY_ARSENAL, arsenal_tree );

  static const talentinfo_t pyrotech_tree[] = {
    // t1
    { "Advanced Targeting", 3 }, { "System Calibrations"           , 2 }, { "Integrated Cardio Package", 3 },
    // t2
    { "Superheated Gas"   , 3 }, { "Sweltering Heat"               , 2 }, { "Gyroscopic Alignment Jets", 2 },
    // t3
    { "Pyro Shield"       , 2 }, { "Superheated Rail"              , 2 }, { "Incendiary Missile"       , 1 }, { "Infrared Sensors"   , 2 },
    // t4
    { "Suit FOE"          , 2 }, { "Prototype Particle Accelerator", 1 }, { "Volatile Warhead"         , 2 },
    // t5
    { "Firebug"           , 2 }, { "Rain of Fire"                  , 3 },
    // t6
    { "Degauss"           , 2 }, { "Burnout"                       , 3 },
    // t7
    { "Rapid Venting"     , 2 }, { "Power Barrels"                 , 1 }, { "Jet Rebounder"  , 2 },
    // t8
    { "Thermal Detonator" , 1 }
  };
  init_talent_tree( BH_MERCENARY_PYROTECH, pyrotech_tree );
}

void class_t::reset()
{
  // tried using rngs.terminal_velocity -> range(0,6) but it alwyas gives 3. meh.
  next_terminal_velocity = sim -> current_time + from_seconds( rand() % 5 );
  base_t::reset();
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
