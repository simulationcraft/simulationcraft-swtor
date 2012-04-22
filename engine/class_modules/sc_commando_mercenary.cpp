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
        // Tree 1


        // Tree 2


        // Tree 3

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
    virtual resource_type primary_resource() const;
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
      if ( name == "apply_charge"           ) return new        apply_charge_t( this, options_str );
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
  talents.xxx                       = find_talent( "xxx"  );

  // Bodyguard

  // t1
  talents.improved vents = find_talent( "Improved Vents" );
  talents.med tech = find_talent( "Med Tech" );
  talents.hired muscle = find_talent( "Hired Muscle" );
  // t2
  talents.empowered scans = find_talent( "Empowered Scans" );
  talents.surgical precision system = find_talent( "Surgical Precision System" );
  talents.supercharged gas = find_talent( "Supercharged Gas" );
  talents.critical reaction = find_talent( "Critical Reaction" );
  // t3
  talents.heat damping = find_talent( "Heat Damping" );
  talents.kolto residue = find_talent( "Kolto Residue" );
  talents.kolto missile = find_talent( "Kolto Missile" );
  talents.power shield = find_talent( "Power Shield" );
  // t4
  talents.powered insulators = find_talent( "Powered Insulators" );
  talents.critical efficiency = find_talent( "Critical Efficiency" );
  talents.protective field = find_talent( "Protective Field" );
  // t5
  talents.reactive armor = find_talent( "Reactive Armor" );
  talents.proactive medicine = find_talent( "Proactive Medicine" );
  talents.kolto shell = find_talent( "Kolto Shell" );
  talents.cure mind = find_talent( "Cure Mind" );
  // t6
  talents.warden = find_talent( "Warden" );
  // t7
  talents.emergency scan = find_talent( "Emergency Scan" );

  // Arsenal

  // t1
  talents.mandalorian iron warheads = find_talent( "Mandalorian Iron Warheads" );
  talents.integrated systems = find_talent( "Integrated Systems" );
  talents.ironsights = find_talent( "Ironsights" );
  // t2
  talents.stabilizers = find_talent( "Stabilizers" );
  talents.muzzle fluting = find_talent( "Muzzle Fluting" );
  talents.upgraded arsenal = find_talent( "Upgraded Arsenal" );
  talents.custom enviro suit = find_talent( "Custom Enviro Suit" );
  // t3
  talents.power barrier = find_talent( "Power Barrier" );
  talents.afterburners = find_talent( "Afterburners" );
  talents.tracer missile = find_talent( "Tracer Missile" );
  talents.target tracking = find_talent( "Target Tracking" );
  // t4
  talents.jet escape = find_talent( "Jet Escape" );
  talents.light em up = find_talent( "Light Em Up" );
  talents.terminal velocity = find_talent( "Terminal Velocity" );
  // t5
  talents.pinning fire = find_talent( "Pinning Fire" );
  talents.riddle = find_talent( "Riddle" );
  talents.tracer lock = find_talent( "Tracer Lock" );
  talents.kolto vents = find_talent( "Kolto Vents" );
  // t6
  talents.barrage = find_talent( "Barrage" );
  talents.power overrides = find_talent( "Power Overrides" );
  // t7
  talents.heatseeker missiles = find_talent( "Heatseeker Missiles" );

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
  talents.energy_rebounder_= find_talent( "Energy Rebounder" );
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

// commando_mercenary_t::primary_resource ==================================================

resource_type commando_mercenary_t::primary_resource() const
{ return RESOURCE_AIM; }

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
  // See sage_sorcerer_t::create_talents()
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
