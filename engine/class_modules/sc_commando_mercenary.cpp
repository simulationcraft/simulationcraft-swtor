// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "../simulationcraft.hpp"

// ==========================================================================
// Commando | Mercenary
// ==========================================================================

namespace { // ANONYMOUS ====================================================

struct commando_mercenary_targetdata_t : public targetdata_t
{
  commando_mercenary_targetdata_t( player_t& source, player_t& target )
    : targetdata_t( source, target )
  {}
};

struct commando_mercenary_t : public player_t
{
    // Buffs
    struct buffs_t
    {

    } buffs;

    // Gains
    struct gains_t
    {

    } gains;

    // Procs
    struct procs_t
    {

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

    commando_mercenary_t( sim_t* sim, player_type pt, const std::string& name, race_type r = RACE_NONE ) :
      player_t( sim, pt == BH_MERCENARY ? BH_MERCENARY : T_COMMANDO, name, ( r == RACE_NONE ) ? RACE_HUMAN : r ),
      buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents()
    {


      primary_attribute   = ATTR_AIM;
      secondary_attribute = ATTR_CUNNING;

      create_talents();
      create_options();
    }

    // Character Definition
    virtual targetdata_t* new_targetdata( player_t& target ) // override
    { return new commando_mercenary_targetdata_t( *this, target ); }

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

    virtual void init_scaling()
    {
      player_t::init_scaling();

      scales_with[ STAT_TECH_POWER ] = true;
    }
};

namespace { // ANONYMOUS NAMESPACE ==========================================

class commando_mercenary_action_t : public action_t
{
public:
  commando_mercenary_action_t( const std::string& n, commando_mercenary_t* player,
                          attack_policy_t policy, resource_type r, school_type s ) :
    action_t( ACTION_ATTACK, n, player, policy, r, s )
  {}

  commando_mercenary_targetdata_t* targetdata() const
  { return static_cast<commando_mercenary_targetdata_t*>( action_t::targetdata() ); }

  commando_mercenary_t* p() const
  { return static_cast<commando_mercenary_t*>( player ); }
};

// ==========================================================================
// Commando / Mercenary Abilities
// ==========================================================================

struct commando_mercenary_attack_t : public commando_mercenary_action_t
{
    commando_mercenary_attack_t( const std::string& n, commando_mercenary_t* p, school_type s=SCHOOL_KINETIC ) :
      commando_mercenary_action_t( n, p, melee_policy, RESOURCE_NONE, s )
    {
        may_crit   = true;
    }

};

struct commando_mercenary_spell_t : public commando_mercenary_action_t
{
    commando_mercenary_spell_t( const std::string& n, commando_mercenary_t* p, school_type s=SCHOOL_KINETIC ) :
      commando_mercenary_action_t( n, p, force_policy, RESOURCE_NONE, s )
    {
        may_crit   = true;
        tick_may_crit = true;
    }

};

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// commando_mercenary Character Definition
// ==========================================================================

// commando_mercenary_t::create_action ====================================================

action_t* commando_mercenary_t::create_action( const std::string& name,
                                            const std::string& options_str )
{
    if ( type == BH_MERCENARY )
    {
      //if ( name == "apply_charge"           ) return new        apply_charge_t( this, options_str );
    }
    else if ( type == T_COMMANDO )
    {

    }

    return player_t::create_action( name, options_str );
}

// commando_mercenary_t::init_talents =====================================================

void commando_mercenary_t::init_talents()
{
  player_t::init_talents();

  // Bodyguard
  // t1
  talents.improved_vents = find_talent( "Improved Vents" );
  talents.med_tech = find_talent( "Med Tech" );
  talents.hired_muscle = find_talent( "Hired Muscle" );
  // t2
  talents.empowered_scans = find_talent( "Empowered Scans" );
  talents.surgical_precision_system = find_talent( "Surgical Precision System" );
  talents.supercharged_gas = find_talent( "Supercharged Gas" );
  talents.critical_reaction = find_talent( "Critical Reaction" );
  // t3
  talents.heat_damping = find_talent( "Heat Damping" );
  talents.kolto_residue = find_talent( "Kolto Residue" );
  talents.kolto_missile = find_talent( "Kolto Missile" );
  talents.power_shield = find_talent( "Power Shield" );
  // t4
  talents.powered_insulators = find_talent( "Powered Insulators" );
  talents.critical_efficiency = find_talent( "Critical Efficiency" );
  talents.protective_field = find_talent( "Protective Field" );
  // t5
  talents.reactive_armor = find_talent( "Reactive Armor" );
  talents.proactive_medicine = find_talent( "Proactive Medicine" );
  talents.kolto_shell = find_talent( "Kolto Shell" );
  talents.cure_mind = find_talent( "Cure Mind" );
  // t6
  talents.warden = find_talent( "Warden" );
  // t7
  talents.emergency_scan = find_talent( "Emergency Scan" );

  // Arsenal
  // t1
  talents.mandalorian_iron_warheads = find_talent( "Mandalorian Iron Warheads" );
  talents.integrated_systems = find_talent( "Integrated Systems" );
  talents.ironsights = find_talent( "Ironsights" );
  // t2
  talents.stabilizers = find_talent( "Stabilizers" );
  talents.muzzle_fluting = find_talent( "Muzzle Fluting" );
  talents.upgraded_arsenal = find_talent( "Upgraded Arsenal" );
  talents.custom_enviro_suit = find_talent( "Custom Enviro Suit" );
  // t3
  talents.power_barrier = find_talent( "Power Barrier" );
  talents.afterburners = find_talent( "Afterburners" );
  talents.tracer_missile = find_talent( "Tracer Missile" );
  talents.target_tracking = find_talent( "Target Tracking" );
  // t4
  talents.jet_escape = find_talent( "Jet Escape" );
  talents.light_em_up = find_talent( "Light 'Em Up" );
  talents.terminal_velocity = find_talent( "Terminal Velocity" );
  // t5
  talents.pinning_fire = find_talent( "Pinning Fire" );
  talents.riddle = find_talent( "Riddle" );
  talents.tracer_lock = find_talent( "Tracer Lock" );
  talents.kolto_vents = find_talent( "Kolto Vents" );
  // t6
  talents.barrage = find_talent( "Barrage" );
  talents.power_overrides = find_talent( "Power Overrides" );
  // t7
  talents.heatseeker_missiles = find_talent( "Heatseeker Missiles" );

  // pyrotech
  // t1
  talents.advanced_targeting = find_talent( "Advanced Targeting" );
  talents.system_calibrations = find_talent( "System Calibrations" );
  talents.integrated_cardio_package = find_talent( "Integrated Cardio Package" );
  // t2
  talents.superheated_gas = find_talent( "Superheated Gas" );
  talents.sweltering_heat = find_talent( "Sweltering Heat" );
  talents.gyroscopic_alignment_jets = find_talent( "Gyroscopic Alignment Jets" );
  // t3
  talents.degauss = find_talent( "Degauss" );
  talents.superheated_rail = find_talent( "Superheated Rail" );
  talents.incendiary_missile = find_talent( "Incendiary Missile" );
  talents.infrared_sensors = find_talent( "Infrared Sensors" );
  // t4
  talents.rapid_venting = find_talent( "Rapid Venting" );
  talents.prototype_particle_accelerator = find_talent( "Prototype Particle Accelerator" );
  // t5
  talents.energy_rebounder = find_talent( "Energy Rebounder" );
  talents.rain_of_fire = find_talent( "Rain of Fire" );
  talents.firebug = find_talent( "Firebug" );
  // t6
  talents.automated_defenses = find_talent( "Automated Defenses" );
  talents.burnout = find_talent( "Burnout" );
  // t7
  talents.thermal_detonator = find_talent( "Thermal Detonator" );



}

// commando_mercenary_t::init_base ========================================================

void commando_mercenary_t::init_base()
{
    player_t::init_base();

    default_distance = 20;
    distance = default_distance;

}

// commando_mercenary_t::init_benefits =======================================================

void commando_mercenary_t::init_benefits()
{
    player_t::init_benefits();

}

// commando_mercenary_t::init_buffs =======================================================

void commando_mercenary_t::init_buffs()
{
    player_t::init_buffs();

    // buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
    // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
    // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

    //bool is_mercenary = ( type == BH_MERCENARY );



}

// commando_mercenary_t::init_gains =======================================================

void commando_mercenary_t::init_gains()
{
    player_t::init_gains();

}

// commando_mercenary_t::init_procs =======================================================

void commando_mercenary_t::init_procs()
{
    player_t::init_procs();

}

// commando_mercenary_t::init_rng =========================================================

void commando_mercenary_t::init_rng()
{
    player_t::init_rng();

}

// commando_mercenary_t::init_actions =====================================================

void commando_mercenary_t::init_actions()
{
    //======================================================================================
    //
    //   Please Mirror all changes between Commando and Mercenary!!!
    //
    //======================================================================================

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

            switch ( primary_tree() )
            {

            default: break;
            }

            action_list_default = 1;
        }
    }

    player_t::init_actions();
}

// commando_mercenary_t::primary_role ==================================================

role_type commando_mercenary_t::primary_role() const
{
  switch ( player_t::primary_role() )
  {
  case ROLE_TANK:
    return ROLE_TANK;
  case ROLE_DPS:
  case ROLE_HYBRID:
    return ROLE_HYBRID;
  default:
    break;
  }

  return ROLE_HYBRID;
}

// commando_mercenary_t::create_talents ==================================================

void commando_mercenary_t::create_talents()
{
  static const talentinfo_t bodyguard_tree[] = {
    // t1
    { "Improved Vents", 2 }, { "Med Tech", 2 }, { "Hired Muscle", 3 },
    // t2
    { "Empowered Scans", 2 }, { "Surgical Precision System", 2 }, { "Supercharged Gas", 1 }, { "Critical Reaction", 2 },
    // t3
    { "Heat Damping", 1 }, { "Kolto Residue", 2 }, { "Kolto Missile", 1 }, { "Power Shield", 1 },
    // t4
    { "Powered Insulators", 2 }, { "Critical Efficiency", 3 }, { "Protective Field", 2 },
    // t5
    { "Reactive Armor", 1 }, { "Proactive Medicine", 2 }, { "Kolto Shell", 1 }, { "Cure Mind", 1 },
    // t6
    { "Warden", 5 },
    // t7
    { "Emergency Scan", 1 }
  };
  init_talent_tree( BH_MERCENARY_BODYGUARD, bodyguard_tree );

  static const talentinfo_t arsenal_tree[] = {
    // t1
    { "Mandalorian Iron Warheads", 2 }, { "Integrated Systems", 2 }, { "Ironsights", 3 },
    // t2
    { "Stabilizers", 3 }, { "Muzzle Fluting", 1 }, { "Upgraded Arsenal", 1 }, { "Custom Enviro Suit", 2 },
    // t3
    { "Power Barrier", 2 }, { "Afterburners", 2 }, { "Tracer Missile", 1 }, { "Target Tracking", 2 },
    // t4
    { "jet Escape", 2 }, { "Light 'Em Up", 1 }, { "Terminal Velocity", 2 },
    // t5
    { "Pinning Fire", 2 }, { "Riddle", 1 }, { "Tracer Lock", 1 }, { "Kolto Vents", 1 },
    // t6
    { "Barrage", 3 }, { "Power Overrides", 2 },
    // t7
    { "Heatseeker Missiles", 1 }
  };
  init_talent_tree( BH_MERCENARY_ARSENAL, arsenal_tree );

  static const talentinfo_t pyrotech_tree[] = {
    // t1
    { "Advanced Targeting", 3 }, { "System Calibrations", 2 }, { "Integrated Cardio Package", 3 },
    // t2
    { "Superheated Gas", 3 }, { "Sweltering Heat", 2 }, { "Gyroscopic Alignment Jets", 2 },
    // t3
    { "Degauss", 2 }, { "Superheated Rail", 2 }, { "Incendiary Missile", 1 }, { "Infrared Sensors", 2 },
    // t4
    { "Rapid Venting", 2 }, { "Prototype Particle Accelerator", 3 },
    // t5
    { "Energy Rebounder", 2 }, { "Rain of Fire", 3 }, { "Firebug", 2 },
    // t6
    { "Automated Defenses", 2 }, { "Burnout", 3 },
    // t7
    { "Thermal Detonator", 1 }
  };
  init_talent_tree( BH_MERCENARY_PYROTECH, pyrotech_tree );
}

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_jedi_shadow  ============================================

player_t* player_t::create_commando( sim_t* sim, const std::string& name, race_type r )
{
    return new commando_mercenary_t( sim, T_COMMANDO, name, r );
}

// player_t::create_BH_MERCENARY  ==========================================

player_t* player_t::create_mercenary( sim_t* sim, const std::string& name, race_type r )
{
    return new commando_mercenary_t( sim, BH_MERCENARY, name, r );
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
