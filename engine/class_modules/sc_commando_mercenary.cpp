// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "bount_troop.hpp"

namespace commando_mercenary { // ===========================================

class class_t;

struct targetdata_t : public bount_troop::targetdata_t
{

  dot_t dot_combustible_gas_burn;
  dot_t dot_death_from_above;
  dot_t dot_explosive_dart;
  dot_t dot_electro_net;
  dot_t dot_flame_thrower;
  dot_t dot_incendiary_missile_burn;
  dot_t dot_radiation_burns;
  dot_t dot_rapid_shots;
  dot_t dot_rapid_shots_offhand;
  dot_t dot_sweeping_blasters;
  dot_t dot_sweeping_blasters_offhand;
  dot_t dot_thermal_detonator;
  dot_t dot_thermal_detonator_burn;
  dot_t dot_unload;
  dot_t dot_unload_offhand;
  dot_t dot_vent_heat;

  buff_t* debuff_electro_net;
  buff_t* debuff_heat_signature;

  targetdata_t( class_t& source, player_t& target );
  bool burning();
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
      buff_t* combustible_gas_cylinder;
      buff_t* critical_reaction;
      buff_t* high_velocity_gas_cylinder;
//      buff_t* power_barrier;
      buff_t* improved_vents;
      buff_t* prototype_particle_accelerator;
      buff_t* thermal_sensor_override;
      buff_t* tracer_lock;

    } buffs;

    // Gains
    struct gains_t
    {
      gain_t* improved_vents;
      gain_t* superheated_rail;
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
      rng_t* combustible_gas_burn;
      rng_t* superheated_rail;
    } rngs;

    // Benefits
    struct benefits_t
    {

    } benefits;

    // Cooldowns
    struct cooldowns_t
    {
      cooldown_t* unload;
      cooldown_t* rail_shot;
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
    virtual void      init_rng();
    virtual void      init_cooldowns();
    virtual void      init_actions();
    virtual role_type primary_role() const;
    virtual void      regen( timespan_t periodicity );
    virtual double    alacrity() const;
    virtual double    range_crit_chance() const;
    virtual double    tech_crit_chance() const;
    virtual double    armor_penetration() const;
    virtual void      reset();
};

targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  bount_troop::targetdata_t     ( source                     , target  ),
  dot_combustible_gas_burn      ( "combustible_gas_burn"     , &source ),
  dot_death_from_above          ( "death_from_above"         , &source ),
  dot_explosive_dart            ( "explosive_dart"           , &source ),
  dot_electro_net               ( "electro_net"              , &source ),
  dot_flame_thrower             ( "flame_thrower"            , &source ),
  dot_incendiary_missile_burn   ( "incendiary_missile_burn"  , &source ),
  dot_radiation_burns           ( "radiation_burns"          , &source ),
  dot_rapid_shots               ( "rapid_shots"              , &source ),
  dot_rapid_shots_offhand       ( "rapid_shots_offhand"      , &source ),
  dot_sweeping_blasters         ( "sweeping_blasters"        , &source ),
  dot_sweeping_blasters_offhand ( "sweeping_blasters_offhand", &source ),
  dot_thermal_detonator         ( "thermal_detonator"        , &source ),
  dot_thermal_detonator_burn    ( "thermal_detonator_burn"   , &source ),
  dot_unload                    ( "unload"                   , &source ),
  dot_unload_offhand            ( "unload_offhand"           , &source ),
  dot_vent_heat                 ( "vent_heat"                , &source ),
  debuff_electro_net    ( new buff_t( this, "electro_net",    5, from_seconds( 10 ) ) ),
  debuff_heat_signature ( new buff_t( this, "heat_signature", 1, from_seconds( 45 ) ) )
{
  add ( dot_combustible_gas_burn      );
  add ( dot_death_from_above          );
  add ( dot_explosive_dart            );
  add ( dot_electro_net               );
  add ( dot_flame_thrower             );
  add ( dot_incendiary_missile_burn   );
  add ( dot_radiation_burns           );
  add ( dot_rapid_shots               );
  add ( dot_rapid_shots_offhand       );
  add ( dot_sweeping_blasters         );
  add ( dot_sweeping_blasters_offhand );
  add ( dot_thermal_detonator         );
  add ( dot_thermal_detonator_burn    );
  add ( dot_unload                    );
  add ( dot_unload_offhand            );
  add ( dot_vent_heat                 );
  add ( *debuff_electro_net           );
  add ( *debuff_heat_signature        );
}

bool targetdata_t::burning()
{
  return ( dot_combustible_gas_burn.ticking
        || dot_incendiary_missile_burn.ticking
        || dot_thermal_detonator_burn.ticking
        || dot_radiation_burns.ticking
        || dot_flame_thrower.ticking );
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

struct elemental_burn_attack_t : public attack_t
{
  elemental_burn_attack_t( class_t* p, const std::string& n ) :
    attack_t( n, p, tech_policy, SCHOOL_ELEMENTAL)
  {
  }

  virtual void target_debuff( player_t* t, dmg_type dmg_type )
  {
    attack_t::target_debuff( t, dmg_type );
    class_t* p = cast();

    int rank;
    if (( rank = p -> talents.burnout -> rank() > 0 ) && ( t -> health_percentage() < 30 ))
      player_multiplier += 0.1 * rank ;
  }
};

class action_callback_t : public ::action_callback_t
{
public:
  action_callback_t( class_t* player ) :
    ::action_callback_t( player )
  {}

  class_t* p() const { return static_cast<class_t*>( listener ); }
  class_t* cast() const { return p(); }
};

// MERC ABILITIES
// class_t::chaff_flare ===================================================================
// class_t::combat_support_cylinder =======================================================
// class_t::combustible_gas_cylinder ======================================================
struct combustible_gas_cylinder_t : public action_t
{
  combustible_gas_cylinder_t( class_t* p, const std::string& n, const std::string& options_str ) :
    action_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    parse_options( options_str );
    base_execute_time = from_seconds( 1.5 );
  }

  virtual void execute()
  {
    action_t::execute();
    class_t* p = cast();

    p -> buffs.combustible_gas_cylinder -> trigger();
    p -> buffs.high_velocity_gas_cylinder -> expire();
    // TODO if implemented p -> buffs.combat_support_cylinder -> expire();
  }
};

struct combustible_gas_burn_callback_t : public action_callback_t
{
  struct combustible_gas_burn_t : public elemental_burn_attack_t
  {
    combustible_gas_burn_t( class_t* p, const std::string& n ) :
      elemental_burn_attack_t( p, n )
    {
  //    rank_level_list = { ... 55 };

      td.power_mod                = 0.1715;
      td.standardhealthpercentmin =
      td.standardhealthpercentmax = 0.01715;

      num_ticks                   = 2;
      tick_zero                   = true;
      base_tick_time              = from_seconds( 3 );
      background                  = true;

      crit_bonus                 += 0.15 * p -> talents.firebug -> rank();
      base_multiplier            += 0.10 * p -> talents.superheated_gas -> rank();

      // TODO: Does alacrity affect this??
    }

    virtual void execute()
    {
      class_t* p = cast();

      if ( ! p -> buffs.combustible_gas_cylinder -> up() )   // TODO: Change to Check()?
        return;

      elemental_burn_attack_t::execute();
    }

    virtual double proc_chance()
    {
      class_t* p = cast();
      return 0.25 + 0.03 * p -> talents.superheated_gas -> rank();
    }

    virtual bool procced()
    {
      class_t* p = cast();
      return p -> rngs.combustible_gas_burn -> roll( proc_chance() );
    }
  };

  combustible_gas_burn_t* combustible_gas_burn;

  combustible_gas_burn_callback_t( class_t* p ) :
    action_callback_t( p ),
    combustible_gas_burn( new combustible_gas_burn_t( p, "combustible_gas_burn" ) )
  {}

  virtual void trigger (::action_t* a, void* /* call_data */)
  {
    class_t* pl = p();
    targetdata_t* td = static_cast<targetdata_t*> ( a -> targetdata() );
    if ( ( a -> weapon || a -> td.weapon )
        && a -> result_is_hit()
        && pl -> buffs.combustible_gas_cylinder -> up()
        && td -> dot_combustible_gas_burn.ticking == false
        && combustible_gas_burn -> procced() )
    {
      combustible_gas_burn -> execute();
    }
  }
};

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
    num_ticks                    = 9;
    tick_zero                    = true;
    base_tick_time               = from_seconds( 1 );
    cooldown -> duration         = from_seconds( 90 );

    base_multiplier             += 0.02 * p -> talents.power_launcher -> rank();
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

// class_t::flame_thrower ================================================================
struct flame_thrower_t : public elemental_burn_attack_t        // Is it considered an elemental burn for the Burnout talent?
{
  flame_thrower_t( class_t* p, const std::string& n, const std::string& options_str ) :
    elemental_burn_attack_t( p, n )
  {
    // rank_level_list = { 55 };

    parse_options( options_str );

    base_cost                   = 24;   // 25
    range                       = 10.0;
    channeled                   = true;
    num_ticks                   = 3;
    tick_zero                   = true;
    base_tick_time              = from_seconds( 1 );
    cooldown -> duration        = from_seconds( 18 );
    aoe                         = 99;

    td.standardhealthpercentmin =
    td.standardhealthpercentmax = 0.07075; // 0.072
    td.power_mod                = 0.71;  // 0.72
  }
};

// class_t::fusion_missile ================================================================
struct fusion_missile_t : public missile_attack_t
{
  struct radiation_burns_t : public elemental_burn_attack_t
  {
    radiation_burns_t( class_t* p, const std::string& n ) :
      elemental_burn_attack_t( p, n )
    {
      rank_level_list = { 54 };  // Tested in game

      td.power_mod                = 0.28;
      td.standardhealthpercentmin = td.standardhealthpercentmax = 0.028;

      num_ticks                   = 6;
      base_tick_time              = from_seconds( 1 );
      background                  = true;
    }
  };

  radiation_burns_t* radiation_burns;

  fusion_missile_t( class_t* p, const std::string& n, const std::string& options_str ) :
    missile_attack_t( p, n ),
    radiation_burns( new radiation_burns_t( p, "radiation_burns" ) )
  {
    rank_level_list = { 54 };  // Tested in game

    parse_options( options_str );

    base_cost                    = 33;
    base_execute_time            = from_seconds( 1.5 );
    cooldown -> duration         = from_seconds( 30 );
    range                        = 30.0;
    travel_speed                 = 2 * 9;

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
      radiation_burns -> schedule_execute();
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

    dd.power_mod                 = 2.2;  //2.77;  // No longer set in execute - Looks like it's now just a standard 25% damage increase from the effect details.
    dd.standardhealthpercentmin  = 0.18; //0.257; // No longer set in execute - See above
    dd.standardhealthpercentmax  = 0.24; //0.297; // No longer set in execute - See above

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
    dd.power_mod                 = 2.2;  //2.77;  // No longer set in execute - Looks like it's now just a standard 25% damage increase from the effect details.
    dd.standardhealthpercentmin  = 0.18; //0.257; // No longer set in execute - See above
    dd.standardhealthpercentmax  = 0.24; //0.297; // No longer set in execute - See above

    if ( targetdata() -> debuff_heat_signature -> check() )
        dd.power_mod                 = 2.77;  // No longer set in execute - Looks like it's now just a standard 25% damage increase from the effect details.
        dd.standardhealthpercentmin  = 0.257; // No longer set in execute - See above
        dd.standardhealthpercentmax  = 0.297; // No longer set in execute - See above

//      player_multiplier += 0.25;
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
    p -> buffs.combustible_gas_cylinder -> expire();
    // TODO when implemented p -> buffs.combat_support_cylinder -> expire();
  }
};

// class_t::incendiary_missile ============================================================
struct incendiary_missile_t : public missile_attack_t
{
  struct incendiary_missile_burn_t : public elemental_burn_attack_t
  {
    incendiary_missile_burn_t( class_t* p, const std::string& n ) :
      elemental_burn_attack_t( p, n )
    {
      td.power_mod                = 0.28;
      td.standardhealthpercentmin =
      td.standardhealthpercentmax = 0.028;

      num_ticks                   = 6;
      base_tick_time              = from_seconds( 3 );
      background                  = true;

      crit_bonus                 += 0.15 * p -> talents.firebug -> rank();
    }
  };

  incendiary_missile_burn_t* incendiary_missile_burn;

  incendiary_missile_t( class_t* p, const std::string& n, const std::string& options_str ) :
    missile_attack_t( p, n ),
    incendiary_missile_burn( new incendiary_missile_burn_t( p, "incendiary_missile_burn" ) )
  {
    check_talent( p -> talents.incendiary_missile -> rank() );

    parse_options( options_str );

    school                       = SCHOOL_ELEMENTAL;

    base_cost                    = 16;
    range                        = 30.0;
    travel_speed                 = 2 * 9;

    dd.power_mod                 = 0.53;
    dd.standardhealthpercentmin  = 0.033;
    dd.standardhealthpercentmax  = 0.073;

    crit_bonus                  += 0.15 * p -> talents.firebug -> rank();

    add_child( incendiary_missile_burn );
  }

  virtual void impact( player_t* t, result_type impact_result, double travel_dmg )
  {
    missile_attack_t::impact( t, impact_result, travel_dmg );
    if ( result_is_hit( impact_result ) ) //&& ( !targetdata() -> dot_incendiary_missile_burn.ticking ) )
      incendiary_missile_burn -> schedule_execute();  // Resets when re-applied. If chain cast it won't tick.
  }
};

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

    base_cost                    = 19 - 3 * p -> talents.power_barrels -> rank();
    range                        = 30.0;
    base_execute_time            = from_seconds( 1.5 );
    weapon_multiplier            = 0.12;
    weapon                       = &( player -> main_hand_weapon );
    dd.power_mod                 = 1.67;  // Estimated since new val not in Torhead (was 1.71). Changind the ability level caused too much of a diffecence.
    dd.standardhealthpercentmin  =        // As above (was 0.171). Tooltip min value was off by 1.8%, now spot on.
    dd.standardhealthpercentmax  = 0.167; // As above (was 0.171). Tooltip max value was off by 1.5%, now spot on.

    base_crit                   += (( p -> set_bonus.rakata_eliminators -> two_pc() || p -> set_bonus.underworld_eliminators -> two_pc() ) ? 0.15 : 0 );
    crit_bonus                  += 0.15 * p -> talents.firebug -> rank();
    base_multiplier             += 0.03 * p -> talents.mandalorian_iron_warheads -> rank() + 0.06 * p -> talents.power_barrels -> rank();   // No need for this to be in player_buff()

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
    class_t* p = cast();
    targetdata_t* td = targetdata();

    unsigned rof_rank = p -> talents.rain_of_fire -> rank();
    if ( rof_rank && td -> burning() )
      player_multiplier += 0.03 * rof_rank;
  }

  virtual void execute()
  {
    attack_t::execute();
    class_t* p = cast();

    if ( offhand_attack )
      offhand_attack -> schedule_execute();

    if ( p -> talents.barrage -> rank() && p -> buffs.barrage -> trigger() )
      p -> cooldowns.unload -> reset( sim -> reaction_time ); // p.total_reaction_time() seems to have way too much variance

    if ( p -> buffs.combustible_gas_cylinder -> up()
         && p -> talents.prototype_particle_accelerator -> rank()
         && p -> buffs.prototype_particle_accelerator -> trigger( 1, 0, 0.45 ) )
        p -> cooldowns.rail_shot -> reset(); // This can be queued after a powershot incase of a proc without worring about hitting Power Shot again straight after interrupting it like Unload/Barrage.
//      p -> cooldowns.rail_shot -> reset( sim -> reaction_time ); // p.total_reaction_time() seems to have way too much variance
  }
};

// class_t::power_surge ===================================================================
// TODO 120s cooldown reduced by Power Overrides. Next ability with a cast time is instant. 50%/100% for a second instant ability depending on

// class_t::rapid_scan ====================================================================
// class_t::supercharged_gas ==============================================================
// class_t::sweeping_blasters =============================================================
struct sweeping_blasters_t : public attack_t
{
  sweeping_blasters_t* offhand_attack;

  sweeping_blasters_t( class_t* p, const std::string& n, const std::string& options_str,
      bool is_offhand = false ) :
    attack_t( n, p, range_policy, SCHOOL_ENERGY ), offhand_attack( 0 )
  {
    // rank_level_list = { 55 };

    parse_options( options_str );

    base_cost                   = 33;
    range                       = 30.0;
    channeled                   = true;
    num_ticks                   = 3;
    base_tick_time              = from_seconds( 1 );
    aoe                         = 5;

    td.weapon                   = &( player -> main_hand_weapon );
    td.standardhealthpercentmin =
    td.standardhealthpercentmax = 0.0817;  // Calculated to match in-game values for 3 power data points since it is not listed in Torhead.
    td.power_mod                = 0.813;   // Calculated to match in-game values for 3 power data points since it is not listed in Torhead.
    td.weapon_multiplier        = -0.468;  // This value is in Torhead.

    if ( is_offhand )
    {
      background                = true;
      dual                      = true;
//      channeled                 = false;  // See Unload for why this is commendte out.
      base_cost                 = 0;
      trigger_gcd               = timespan_t::zero();
      td.weapon                 = &( player -> off_hand_weapon );
      td.power_mod              = 0;
      rank_level_list           = { 0 };
    }
    else
    {
      offhand_attack            = new sweeping_blasters_t( p, n+"_offhand", options_str, true );
      add_child( offhand_attack );
    }
  }

  virtual void player_buff()
  {
    attack_t::player_buff();
    class_t* p = cast();
    targetdata_t* td = targetdata();

    unsigned rof_rank = p -> talents.rain_of_fire -> rank();
    if ( rof_rank && td -> burning() )
      player_multiplier += 0.03 * rof_rank;
  }

  virtual void execute()
  {
    attack_t::execute();

    if ( offhand_attack )
      offhand_attack->schedule_execute();
  }
};

// class_t::thermal_detonator ================================================================
struct thermal_detonator_t : public attack_t
{
  struct thermal_detonator_burn_t : public elemental_burn_attack_t
  {
    thermal_detonator_burn_t( class_t* p, const std::string& n ) :
      elemental_burn_attack_t( p, n )
    {
      td.power_mod                = 0.3;
      td.standardhealthpercentmin =
      td.standardhealthpercentmax = 0.03;

      num_ticks                   = 4;
      base_tick_time              = from_seconds( 3 );
      background                  = true;

      crit_bonus                 += 0.15 * p -> talents.firebug -> rank();
      base_multiplier            += 0.06 * p -> talents.power_barrels -> rank();   // TODO: Test this
    }
  };

  thermal_detonator_burn_t* thermal_detonator_burn;

  thermal_detonator_t( class_t* p, const std::string& n, const std::string& options_str) :
    attack_t( n, p, tech_policy, SCHOOL_KINETIC ),
    thermal_detonator_burn( new thermal_detonator_burn_t( p, "thermal_detonator_burn" ) )
  {
    check_talent( p -> talents.thermal_detonator -> rank() );

    parse_options( options_str );

    base_cost                    = 16;
    range                        = 30.0;
    num_ticks                    = 1;
    base_tick_time               = from_seconds( 3.4 );
    cooldown -> duration         = from_seconds( 15 );
    travel_speed                 = 3 * 9;

    td.power_mod                 = 2.06;
    td.standardhealthpercentmin  = 0.186;
    td.standardhealthpercentmax  = 0.226;

    crit_bonus                  += 0.15 * p -> talents.firebug -> rank();
    base_multiplier             += 0.06 * p -> talents.power_barrels -> rank();

    add_child( thermal_detonator_burn );
  }

  virtual void tick( dot_t* d )
  {
    attack_t::tick( d );
    thermal_detonator_burn -> schedule_execute();
  }
};

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
struct death_from_above_t : public missile_attack_t
{
  death_from_above_t( class_t* p, const std::string& n, const std::string& options_str ) :
    missile_attack_t( p, n )
  {
    // rank_level_list = { 55 };

    parse_options( options_str );

    base_cost                   = 25;
    range                       = 30.0;
    cooldown -> duration        = from_seconds( 60 );
//    travel_speed                = 6 * 9;
    channeled                   = true;
    num_ticks                   = 6;
    base_tick_time              = from_seconds( 0.5 );
    aoe                         = -1;  // Unlimited I think.

    td.standardhealthpercentmin = 0.064;
    td.standardhealthpercentmax = 0.094;
    td.power_mod                = 0.79;

    if ( p -> bugs ) // Death from above benefist from the 6% missile dammage bonus on the coefficient (power_mod) only. Tested by steve.mellross in 2.2.3.
    {
      // Revert overall bonus
      base_multiplier -= 0.03 * p -> talents.mandalorian_iron_warheads -> rank();
      // Add bonus directly to coefficient.
      td.power_mod *= 1 + 0.03 * p -> talents.mandalorian_iron_warheads -> rank();
    }

  }
};

// class_t::determination =================================================================
// class_t::electro_dart ==================================================================
// class_t::energy_shield =================================================================
// class_t::explosive_dart ================================================================
struct explosive_dart_t : public attack_t
{
  explosive_dart_t( class_t* p, const std::string& n, const std::string& options_str) :
    attack_t( n, p, tech_policy, SCHOOL_KINETIC )
  {
    rank_level_list = { 54 };

    parse_options( options_str );

    base_cost                    = 16;
    range                        = 30.0;
    num_ticks                    = 1;
    base_tick_time               = from_seconds( 3.4 );
    cooldown -> duration         = from_seconds( 15 );
    travel_speed                 = 3 * 9;

    aoe                          = 3;

    td.power_mod                 = 1.61;
    td.standardhealthpercentmin  = 0.141;
    td.standardhealthpercentmax  = 0.181;
  }
};

// class_t::flame_thrower =================================================================
// class_t::heroic_moment =================================================================
// class_t::kolto_overload ================================================================
// class_t::missile_blast =================================================================
// class_t::rail_shot =====================================================================
struct rail_shot_t : public attack_t
{
  rail_shot_t* offhand_attack;
  int tracer_lock_stacks;

  rail_shot_t( class_t* p, const std::string& n, const std::string& options_str, bool is_offhand = false ) :
      attack_t( n, p, range_policy, SCHOOL_ENERGY ), offhand_attack( 0 ), tracer_lock_stacks( 0 )
  {
    rank_level_list = { 9, 12, 15, 20, 31, 42, 50, 55 };

    parse_options( options_str );

    base_cost                    =  16 - ( p -> set_bonus.rakata_eliminators -> four_pc() ? 8 : 0 );
    cooldown -> duration         =  from_seconds( 15 );
    range                        =  30.0;
    dd.power_mod                 =  2.02;   // 1.9
    dd.standardhealthpercentmin  =  
    dd.standardhealthpercentmax  =  0.202;  // 0.19
    weapon                       =  &( player -> main_hand_weapon );
    weapon_multiplier            =  0.35;   // 0.27

    crit_bonus                  += 0.15 * p -> talents.firebug -> rank();
    base_crit                   += (p -> set_bonus.battlemaster_eliminators -> four_pc() ? 0.15 : 0 );
    base_multiplier             += (p -> set_bonus.underworld_eliminators   -> four_pc() ? 0.08 : 0 );
    base_multiplier             += 0.02 * p -> talents.power_launcher -> rank()      + 0.06 * p -> talents.power_barrels -> rank();
    base_armor_penetration      -= 0.10 * p -> talents.advanced_targeting -> rank()  + 0.15 * p -> talents.superheated_rail -> rank();

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
      offhand_attack             = new rail_shot_t( p, n+"_offhand", options_str, true );
      add_child( offhand_attack );
    }
  }

  virtual void player_buff()
  {
    attack_t::player_buff();
    class_t* p = cast();
    targetdata_t* td = targetdata();

    if ( p -> talents.tracer_lock -> rank() )
      player_multiplier += 0.06 * tracer_lock_stacks;

    unsigned rof_rank = p -> talents.rain_of_fire -> rank();
    if ( rof_rank && td -> burning() )
      player_multiplier += 0.03 * rof_rank;
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
    class_t* p = cast();
    targetdata_t* td = targetdata();

    if ( offhand_attack )
    {
      tracer_lock_stacks = p -> buffs.tracer_lock -> stack();
      offhand_attack -> tracer_lock_stacks = tracer_lock_stacks;
      p -> buffs.tracer_lock -> expire();
    }

    attack_t::execute();

    if ( offhand_attack )
    {
      offhand_attack -> schedule_execute();

      unsigned rof_rank = p -> talents.rain_of_fire -> rank();
      if ( rof_rank && td -> burning() )
      {
        if ( td -> burning() && p -> rngs.superheated_rail -> roll( 0.5 * rof_rank ) )
        {
          p -> resource_loss( RESOURCE_HEAT, 8, p -> gains.superheated_rail );
          td -> dot_combustible_gas_burn.refresh_duration();  // Will do nothing if not active
        }
      }
    }
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

  virtual void player_buff()
  {
    attack_t::player_buff();
    class_t* p = cast();
    targetdata_t* td = targetdata();

    unsigned rof_rank = p -> talents.rain_of_fire -> rank();
    if ( rof_rank && td -> burning() )
      player_multiplier += 0.03 * rof_rank;
  }

  virtual void execute()
  {
    attack_t::execute();
    if ( offhand_attack )
      offhand_attack->schedule_execute();
  }
};

// class_t::rocket_punch ==================================================================
struct rocket_punch_t : public attack_t
{
  rocket_punch_t( class_t* p, const std::string& n, const std::string& options_str) :
    attack_t( n, p, tech_policy, SCHOOL_KINETIC )
  {
    // rank_level_list = { ... 55 };

    parse_options( options_str );

    base_cost                    = 16;
    range                        = 4;
    cooldown -> duration         = from_seconds( 9 );

    dd.power_mod                 =  1.82;
    dd.standardhealthpercentmin  =  0.17;
    dd.standardhealthpercentmax  =  0.194;
  }
};

// class_t::stealth_scan ==================================================================

// class_t::thermal_sensor_override =======================================================
struct thermal_sensor_override_t : public action_t
{
  thermal_sensor_override_t( class_t* p, const std::string& n, const std::string& options_str ) :
    action_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    parse_options( options_str );

    cooldown -> duration = from_seconds( 120 - 15 * p -> talents.rapid_venting -> rank() );
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
    cooldown -> duration        = from_seconds( 15.0 - 3 * p -> talents.power_barrels -> rank() );
    channeled                   = true;
    num_ticks                   = 3;
    base_tick_time              = from_seconds( 1 );
    td.standardhealthpercentmin =
    td.standardhealthpercentmax = 0.11;
    td.weapon                   = &( player -> main_hand_weapon );
    td.power_mod                = 1.1;
    td.weapon_multiplier        = -0.3;

    crit_bonus                 += 0.15 * p -> talents.target_tracking -> rank();
    base_multiplier            += 0.33 * p -> talents.riddle -> rank() + 0.06 * p -> talents.power_barrels -> rank();
    base_armor_penetration     -= 0.10 * p -> talents.advanced_targeting -> rank ();

    if ( is_offhand )
    {
      background                = true;
      dual                      = true;
      // Setting channeled=true allows the OH dot to be interrupted the same way as the MH dot for higher priority attacks or low player skill.
      // This seems better than leaving it tick in all situations but still not ideal:
      // Low player skill interrupts on MH and OH will not be in synch and I doubt other cancel/interrupt mechanics will affect OH.
      // TODO: Modify dot_event to allow cancelling a dot to cancel related dots via action object.
      // Add a vitrual empty func to action/attack called on interrupting a dot that can be overridden here to cancel offhand dots? If not overidden functionality remains unchanged.
      channeled                 = false;
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
    targetdata_t* td = targetdata();

    if ( p -> buffs.barrage -> up() )
      player_multiplier += 0.25;

    unsigned rof_rank = p -> talents.rain_of_fire -> rank();
    if ( rof_rank && td -> burning() )
      player_multiplier += 0.03 * rof_rank;
  }

  virtual void last_tick(dot_t* d)
  {
    attack_t::last_tick( d );
    if ( ! offhand_attack )
      p() -> buffs.barrage -> expire();
  }

  virtual void execute()
  {
    class_t* p = cast();

    benefit_from_barrage = p -> buffs.barrage -> check();   // Up used in player buff

    attack_t::execute();

    if ( offhand_attack )
      offhand_attack -> schedule_execute();

    if ( p -> buffs.combustible_gas_cylinder -> up()
         && p -> talents.prototype_particle_accelerator -> rank()
         && p -> buffs.prototype_particle_accelerator -> trigger( 1, 0, 0.75 ) )
      p -> cooldowns.rail_shot -> reset(); // This can be queued after a powershot incase of a proc without worring about hitting Power Shot again straight after interrupting it like Unload/Barrage.
//      p -> cooldowns.rail_shot -> reset( sim -> reaction_time ); // p.total_reaction_time() seems to have way too much variance
  }

};

// class_t::vent_heat =====================================================================
struct vent_heat_t : public action_t
{
  vent_heat_t( class_t* p, const std::string& n, const std::string& options_str ) :
    action_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {

    parse_options( options_str );

    cooldown -> duration = from_seconds( 120.0 - 15 * p -> talents.rapid_venting -> rank() );
    num_ticks            = 2;
    base_tick_time       = from_seconds( 1.5 );
    trigger_gcd          = timespan_t::zero();
  }

  virtual void execute()
  {
    action_t::execute();
    class_t* p = cast();

    p -> resource_loss( RESOURCE_HEAT, 34, p -> gains.vent_heat );
    if ( unsigned iv_rank = p -> talents.improved_vents -> rank() )
    {
      p -> buffs.improved_vents -> trigger();
      p -> resource_loss( RESOURCE_HEAT, 8 * iv_rank, p -> gains.improved_vents );
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
      if ( name == "combustible_gas_cylinder"   ) return new combustible_gas_cylinder_t   ( this, name, options_str );
      if ( name == "death_from_above"           ) return new death_from_above_t           ( this, name, options_str );
      if ( name == "electro_net"                ) return new electro_net_t                ( this, name, options_str );
      if ( name == "explosive_dart"             ) return new explosive_dart_t             ( this, name, options_str );
      if ( name == "flame_thrower"              ) return new flame_thrower_t              ( this, name, options_str );
      if ( name == "fusion_missile"             ) return new fusion_missile_t             ( this, name, options_str );
      if ( name == "heatseeker_missiles"        ) return new heatseeker_missiles_t        ( this, name, options_str );
      if ( name == "high_velocity_gas_cylinder" ) return new high_velocity_gas_cylinder_t ( this, name, options_str );
      if ( name == "incendiary_missile"         ) return new incendiary_missile_t         ( this, name, options_str );
      if ( name == "power_shot"                 ) return new power_shot_t                 ( this, name, options_str );
      if ( name == "rail_shot"                  ) return new rail_shot_t                  ( this, name, options_str );
      if ( name == "rapid_shots"                ) return new rapid_shots_t                ( this, name, options_str );
      if ( name == "rocket_punch"               ) return new rocket_punch_t               ( this, name, options_str );
      if ( name == "sweeping_blasters"          ) return new sweeping_blasters_t          ( this, name, options_str );
      if ( name == "thermal_detonator"          ) return new thermal_detonator_t          ( this, name, options_str );
      if ( name == "thermal_sensor_override"    ) return new thermal_sensor_override_t    ( this, name, options_str );
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
  return  base_t::alacrity()
          + (buffs.high_velocity_gas_cylinder -> up() ? 0.03 : 0)
          + (buffs.critical_reaction -> up()          ? 0.03 : 0)
          + (buffs.improved_vents -> up()             ? 0.1  : 0)
          + talents.rapid_venting -> rank() * 0.01;
}

// class_t::range_crit_chance ================================================================

double class_t::range_crit_chance() const
{
  return base_t::range_crit_chance()
         + ( buffs.combustible_gas_cylinder -> up()   ? 0.02 * talents.upgraded_arsenal -> rank() : 0 );
}

// class_t::tech_crit_chance ================================================================

double class_t::tech_crit_chance() const
{
  return base_t::range_crit_chance() + 0.01 * talents.burnout -> rank();
}

// class_t::armor_penetration ================================================================

double class_t::armor_penetration() const
{
  return base_t::armor_penetration() - (buffs.high_velocity_gas_cylinder -> up() ? 0.35 : 0);
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

  default_distance = 10;   // Changed from 20 to 10 because of Flame Thrower
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
  buffs.barrage                        = new buff_t( this, "barrage",                        1, from_seconds( 15 ), from_seconds( 6 ), (talents.barrage -> rank() * 0.15) );
  buffs.combustible_gas_cylinder       = new buff_t( this, "combustible_gas_cylinder",       1 );
  buffs.critical_reaction              = new buff_t( this, "critical_reaction",              1, from_seconds( 6 ),  from_seconds( 0 ), (talents.critical_reaction -> rank() * 0.5) );
  buffs.high_velocity_gas_cylinder     = new buff_t( this, "high_velocity_gas_cylinder",     1 );
  buffs.improved_vents                 = new buff_t( this, "improved_vents",                 1, from_seconds( 6 ) );
  buffs.prototype_particle_accelerator = new buff_t( this, "prototype_particle_accelerator", 1, from_seconds( 10 ), from_seconds( 6 ), (1) );
  buffs.thermal_sensor_override        = new buff_t( this, "thermal_sensor_override",        1, from_seconds( 15 ) );
  buffs.tracer_lock                    = new buff_t( this, "tracer_lock",                    5, from_seconds( 15 ) );
//
}

// class_t::init_gains ====================================================================

void class_t::init_gains()
{
  base_t::init_gains();
  bool is_bh = ( type == BH_MERCENARY );
  const char* improved_vents    = is_bh ? "improved_vents"    : "cell_capacitor"  ;
  const char* superheated_rail  = is_bh ? "superheated_rail"  : "high_friction_bolts";
  const char* terminal_velocity = is_bh ? "terminal_velocity" : "penetrate_armor" ;
  const char* vent_heat         = is_bh ? "vent_heat"         : "recharge_cells"  ;

  gains.improved_vents    = get_gain( improved_vents    );
  gains.superheated_rail  = get_gain( superheated_rail  );
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

// class_t::init_rng ============================================

void class_t::init_rng()
{
  base_t::init_rng();
  rngs.combustible_gas_burn = get_rng( "combustible_gas_burn" );
  rngs.superheated_rail = get_rng( "superheated_rail" );
}
// class_t::init_cooldowns ================================================================

void class_t::init_cooldowns()
{
  base_t::init_cooldowns();

  bool is_bh = ( type == BH_MERCENARY );

  const char* unload = is_bh ? "unload"    : "???" ;
  const char* rail_shot = is_bh ? "rail_shot"    : "???" ;

  cooldowns.unload = get_cooldown( unload );
  cooldowns.rail_shot = get_cooldown( rail_shot );
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
      action_list_str += "/death_from_above,if=heat<40-cost|cooldown.vent_heat.remains<10";
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

  register_direct_damage_callback( SCHOOL_ALL_MASK, new combustible_gas_burn_callback_t( this ) );
  register_tick_damage_callback( SCHOOL_ALL_MASK, new combustible_gas_burn_callback_t( this ) );

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

  unsigned tv_rank = talents.terminal_velocity -> rank();
  if ( tv_rank && buffs.high_velocity_gas_cylinder -> check() )
  {
    if ( next_terminal_velocity < sim -> current_time )
    {
      next_terminal_velocity += from_seconds( 6 );
      resource_loss( RESOURCE_HEAT, 4 * tv_rank, gains.terminal_velocity );
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
