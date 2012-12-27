// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "bount_troop.hpp"

namespace commando_mercenary { // ===========================================

class class_t;

struct targetdata_t : public bount_troop::targetdata_t
{

  dot_t dot_radiation_burns;
  dot_t dot_rapid_shots;
  dot_t dot_rapid_shots_offhand;
  dot_t dot_unload;
  dot_t dot_unload_offhand;
  dot_t dot_vent_heat;

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
      buff_t* thermal_sensor_override;
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
      talent_t* peacekeeper;
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
      talent_t* tracer_lock;
      talent_t* terminal_velocity;
      // t5
      talent_t* pinning_fire;
      talent_t* riddle;
      talent_t* light_em_up;
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
    virtual void      init_rng();
    virtual void      init_actions();
    virtual role_type primary_role() const;
    virtual void      regen( timespan_t periodicity );
    virtual double    alacrity() const;
    virtual double    armor_penetration() const;
    virtual void      reset();
};

targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  bount_troop::targetdata_t ( source                , target  ),
  dot_radiation_burns       ( "radiation_burns"     , &source ),
  dot_rapid_shots           ( "rapid_shots"         , &source ),
  dot_rapid_shots_offhand   ( "rapid_shots_offhand" , &source ),
  dot_unload                ( "unload"              , &source ),
  dot_unload_offhand        ( "unload_offhand"      , &source ),
  dot_vent_heat             ( "vent_heat"           , &source ),
  debuff_heat_signature( new buff_t( this, "heat_signature", 5, from_seconds( 15 ) ) )
{
  add ( dot_radiation_burns     );
  add ( dot_rapid_shots         );
  add ( dot_rapid_shots_offhand );
  add ( dot_unload              );
  add ( dot_unload_offhand      );
  add ( dot_vent_heat           );
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
// class_t::fusion_missile ================================================================
struct fusion_missile_t : public missile_attack_t
{
  struct radiation_burns_t : public attack_t
  {
    radiation_burns_t( class_t* p, const std::string& n ) :
      attack_t( n, p, tech_policy, SCHOOL_ELEMENTAL )
    {
      td.power_mod                = 0.28;
      td.standardhealthpercentmin = td.standardhealthpercentmax = 0.028;
      num_ticks                   = 6;
      base_tick_time              = from_seconds( 1 );
      background                  = true;
      base_accuracy               = 999;
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
    // travel_speed                 = 18.4; // XXX guess. how to convert theirs to ours?

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
    // rank_level_list = { ... 50 }

    check_talent( p -> talents.heatseeker_missiles -> rank() );

    parse_options( options_str );

    base_cost                    = 16;
    cooldown -> duration         = from_seconds( 15 );
    range                        = 30.0;

    dd.power_mod                 = 0; // set in execute
    dd.standardhealthpercentmin  = 0; // set in execute
    dd.standardhealthpercentmax  = 0; // set in execute

    crit_bonus                  += 0.15 * p -> talents.target_tracking -> rank();
  }

  virtual void execute()
  {
    // TEST:
    // assuming damage is calculated on cast, not impact.
    // could be
    // 1: damage on cast, so target debuffs during travel time won't be included
    // 2: damage on impact, so player changes during travel time will be included
    // 3: player stats on cast, target damage on impact
    // 3 would be the purest, but for simplicity going with 1. Unsure which the game does.
    double mod = dd.power_mod;
    switch ( targetdata() -> debuff_heat_signature -> stack() )
    {
      case 5:
        dd.power_mod                 = 2.6;
        dd.standardhealthpercentmin  = 0.21;
        dd.standardhealthpercentmax  = 0.31;
        break;
      case 4:
        dd.power_mod                 = 2.5;
        dd.standardhealthpercentmin  = 0.202;
        dd.standardhealthpercentmax  = 0.298;
        break;
      case 3:
        dd.power_mod                 = 2.39;
        dd.standardhealthpercentmin  = 0.193;
        dd.standardhealthpercentmax  = 0.285;
        break;
      case 2:
        dd.power_mod                 = 2.288;
        dd.standardhealthpercentmin  = 0.185;
        dd.standardhealthpercentmax  = 0.273;
        break;
      case 1:
        dd.power_mod                 = 2.184;
        dd.standardhealthpercentmin  = 0.176;
        dd.standardhealthpercentmax  = 0.26;
        break;
      default:
        dd.power_mod                 = 2.08;
        dd.standardhealthpercentmin  = 0.168;
        dd.standardhealthpercentmax  = 0.248;
        break;
    }
    if ( mod != dd.power_mod )
      set_base_min_max();
    base_t::execute();
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
    rank_level_list = { 10, 13, 16, 22, 33, 45, 50 };

    parse_options( options_str );

    base_cost                    = 16;
    range                        = 30.0;
    base_execute_time            = from_seconds( 2 - 0.5 * p -> talents.muzzle_fluting -> rank() );
    weapon_multiplier            = 0.15;
    weapon                       = &( player -> main_hand_weapon );
    dd.power_mod                 = 1.71;
    dd.standardhealthpercentmin  = 
    dd.standardhealthpercentmax  = 0.171;
    base_crit                   += ( p -> set_bonus.rakata_eliminators -> two_pc() ? 0.15 : 0 );

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

  virtual void player_buff()
  {
    attack_t::player_buff();
    if ( unsigned rank = p() -> talents.mandalorian_iron_warheads -> rank() )
      player_multiplier += 0.03 * rank;
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
// TODO 120s cooldown. next ability with a cast time is instant.

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

    base_cost                    = 16;
    base_execute_time            = from_seconds( 2 - 0.5 * p -> talents.muzzle_fluting -> rank() );
    range                        = 30.0;
    travel_speed                 = 18.4; // XXX guess. how to convert theirs to ours?
    dd.power_mod                 = 1.71;
    dd.standardhealthpercentmin  = 0.131;
    dd.standardhealthpercentmax  = 0.211;
    base_crit                   += ( p -> set_bonus.rakata_eliminators -> two_pc() ? 0.15 : 0 );
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
      targetdata() -> debuff_heat_signature
        -> trigger( p() -> talents.light_em_up -> rank() ? 2 : 1 );
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
    rank_level_list = { 9, 12, 15, 20, 31, 42, 50 };

    parse_options( options_str );

    base_cost                    =  16 - ( p -> set_bonus.rakata_eliminators -> four_pc() ? 8 : 0 );
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

    if ( p -> talents.tracer_lock -> rank() )
      player_multiplier += 0.06 * p -> buffs.tracer_lock -> stack();

    if ( unsigned rank = p -> talents.advanced_targeting -> rank () )
      player_armor_penetration -= 0.1 * rank;

    if ( p -> set_bonus.battlemaster_eliminators -> four_pc() )
      player_crit += 0.15;

  }

  virtual double cost() const
  {
    double c = attack_t::cost();
    class_t* p = cast();
    if ( p -> talents.upgraded_arsenal -> rank() && p -> buffs.high_velocity_gas_cylinder -> check() )
      c -= 8;

    return c;
  }

  virtual void execute()
  {
    attack_t::execute();
    p() -> buffs.tracer_lock -> expire();
  }

  bool ignore_thermal_sensor_override()
  {
    // XXX TODO i think this is only meant to be true if we have both  4pc and upgraded_arsenal and HVGS up
    return true;
  }
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
    base_tick_time              = from_seconds( 0.3 );
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

    cooldown -> duration = from_seconds( 120 - ( 15 * p -> talents.power_overrides -> rank() ) );
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
    rank_level_list = { 3, 6, 9, 12, 15, 21, 32, 44, 50 };

    parse_options( options_str );

    base_cost                   = 16;
    range                       = 30.0;
    cooldown -> duration        = from_seconds( 15.0 );
    channeled                   = true;
    num_ticks                   = 3;
    base_tick_time              = from_seconds( 1 );
    base_multiplier            += 0.33 * p -> talents.riddle -> rank();
    crit_bonus                 +=  0.15 * p -> talents.target_tracking -> rank();
    td.standardhealthpercentmin =
      td.standardhealthpercentmax = 0.105;
    td.weapon                   = &( player -> main_hand_weapon );
    td.power_mod                = 1.05;
    td.weapon_multiplier        = -0.3;

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
    if ( p() -> buffs.barrage -> up() )
      player_multiplier += 0.25;

    if ( unsigned rank = p() -> talents.advanced_targeting -> rank () )
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
    if ( p() -> buffs.barrage -> up() )
      benefit_from_barrage = true;
    else
      benefit_from_barrage = false; // to be safe

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
    base_tick_time       = from_seconds( 3 );
    trigger_gcd          = timespan_t::zero();
  }

  virtual void execute()
  {
    action_t::execute();
    class_t* p = cast();

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
// class_t::hired_protection reduced the cooldown of revive by 100%

// class_t::create_action =================================================================

::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
    if ( type == BH_MERCENARY )
    {
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
  return buffs.critical_reaction -> up() ? base_t::alacrity() - 0.05 : base_t::alacrity();
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
  talents.peacekeeper                    = find_talent( "Peacekeeper" );
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
  set_base_alacrity( get_base_alacrity()   +  0.02 * talents.system_calibrations -> rank() );
  set_base_crit( get_base_crit()           +  0.01 *  talents.hired_muscle       -> rank() );
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

  buffs.tracer_lock             = new buff_t( this, "tracer_lock",             5, from_seconds( 15 ) );
  buffs.thermal_sensor_override = new buff_t( this, "thermal_sensor_override", 1, from_seconds( 15 ) );

  buffs.high_velocity_gas_cylinder = new buff_t( this, "high_velocity_gas_cylinder", 1 );

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

// class_t::init_rng ======================================================================

void class_t::init_rng()
{
  base_t::init_rng();
  bool is_bh = ( type == BH_MERCENARY );
  const char* terminal_velocity = is_bh ? "terminal_velocity" : "penetrate_armor" ;


  rngs.terminal_velocity = get_rng( terminal_velocity );
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
            action_list_str += "stim,type=exotech_reflex";
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
            action_list_str += "stim,type=exotech_reflex";
            action_list_str += "/snapshot_stats";
            // ARSENAL
            action_list_str += "/high_velocity_gas_cylinder,if=!buff.high_velocity_gas_cylinder.up";
            action_list_str += "/vent_heat,if=heat<=40";
            action_list_str += "/use_relics";
            action_list_str += "/power_potion";
            if ( set_bonus.rakata_eliminators -> four_pc() )
              action_list_str += "/rail_shot,if=heat<83&buff.high_velocity_gas_cylinder.up";
              action_list_str += "/rail_shot,if=heat>77&!buff.high_velocity_gas_cylinder.up";
            action_list_str += "/thermal_sensor_override,if=heat<83";
            action_list_str += "/fusion_missile,if=buff.thermal_sensor_override.up";
            if ( talents.heatseeker_missiles)
              action_list_str += "/heatseeker_missiles,if=heat>77|cooldown.vent_heat.remains<10";
            action_list_str += "/unload,if=heat>77|cooldown.vent_heat.remains<10";
            if ( talents.tracer_missile )
              action_list_str += "/tracer_missile,if=heat>77|cooldown.vent_heat.remains<10|(heat>=70&cooldown.heatseeker_missiles.remains>1.5&cooldown.unload.remains>1.5)";
            else
              action_list_str += "/power_shot,if=heat>75";
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
  if ( unsigned rank = talents.terminal_velocity -> rank() )
  {
    if ( next_terminal_velocity < sim -> current_time )
    {
      next_terminal_velocity += from_seconds( 6 );
      if ( rngs.terminal_velocity -> roll( rank * 0.5 ) )
      {
        resource_gain( RESOURCE_HEAT, 8, gains.terminal_velocity );
        procs.terminal_velocity -> occur();
      }
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
    { "Empowered Scans"   , 2 }, { "Surgical Precision System", 2 }, { "Supercharged Gas", 1 }, { "Critical Reaction", 2 },
    // t3
    { "Heat Damping"      , 1 }, { "Kolto Residue"            , 2 }, { "Kolto Missile"   , 1 }, { "Power Shield"     , 1 },
    // t4
    { "Powered Insulators", 2 }, { "Critical Efficiency"      , 3 }, { "Protective Field", 2 },
    // t5
    { "Reactive Armor"    , 1 }, { "Proactive Medicine"       , 2 }, { "Kolto Shell"     , 1 }, { "Cure Mind"        , 1 },
    // t6
    { "Warden"            , 3 }, { "Peacekeeper"            , 2 },
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
    { "Jet Escape"               , 2 }, { "Tracer Lock"       , 1 }, { "Terminal Velocity", 2 },
    // t5
    { "Pinning Fire"             , 2 }, { "Riddle"            , 1 }, { "Light 'Em Up"     , 1 }, { "Kolto Vents"       , 1 },
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
