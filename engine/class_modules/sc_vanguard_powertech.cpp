// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "../simulationcraft.hpp"

// ==========================================================================
// Vanguard / Powertech
// ==========================================================================

namespace { // ANONYMOUS ====================================================

struct vanguard_powertech_targetdata_t : public targetdata_t
{
  vanguard_powertech_targetdata_t( player_t& source, player_t& target )
    : targetdata_t( source, target )
  {}
};

struct vanguard_powertech_t : public player_t
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

    vanguard_powertech_t( sim_t* sim, player_type pt, const std::string& name, race_type r = RACE_NONE ) :
        player_t( sim, pt == BH_POWERTECH ? BH_POWERTECH : T_VANGUARD, name, ( r == RACE_NONE ) ? RACE_HUMAN : r )
    {


      primary_attribute   = ATTR_STRENGTH;
      secondary_attribute = ATTR_WILLPOWER;

      create_talents();
      create_options();
    }

    // Character Definition
    virtual targetdata_t* new_targetdata( player_t& target ) // override
    { return new vanguard_powertech_targetdata_t( *this, target ); }

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
            void      create_talents();

    virtual void init_scaling()
    {
      player_t::init_scaling();
      scales_with[ STAT_FORCE_POWER ] = true;
    }
};

namespace { // ANONYMOUS NAMESPACE ==========================================

class vanguard_powertech_action_t : public action_t
{
public:
  vanguard_powertech_action_t( const std::string& n, vanguard_powertech_t* player,
                          attack_policy_t policy, resource_type r, school_type s ) :
    action_t( ACTION_ATTACK, n.c_str(), player, policy, r, s )
  {}

  vanguard_powertech_targetdata_t* targetdata() const
  { return static_cast<vanguard_powertech_targetdata_t*>( action_t::targetdata() ); }

  vanguard_powertech_t* p() const
  { return static_cast<vanguard_powertech_t*>( player ); }
};

// ==========================================================================
// Vanguard / Powertech Abilities
// ==========================================================================

struct vanguard_powertech_attack_t : public vanguard_powertech_action_t
{
    vanguard_powertech_attack_t( const std::string& n, vanguard_powertech_t* p, school_type s=SCHOOL_KINETIC ) :
      vanguard_powertech_action_t( n, p, melee_policy, RESOURCE_NONE, s )
    {
        may_crit   = true;
    }

};

struct vanguard_powertech_spell_t : public vanguard_powertech_action_t
{
    vanguard_powertech_spell_t( const std::string& n, vanguard_powertech_t* p, school_type s=SCHOOL_KINETIC ) :
      vanguard_powertech_action_t( n, p, force_policy, RESOURCE_NONE, s )
    {
        may_crit   = true;
        tick_may_crit = true;
    }

};




} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// Vanguard / Powertech Character Definition
// ==========================================================================

// vanguard_powertech_t::create_action ====================================================

action_t* vanguard_powertech_t::create_action( const std::string& name,
                                            const std::string& options_str )
{
    if ( type == BH_POWERTECH )
    {

    }
    else if ( type == T_VANGUARD )
    {

    }

    //if ( name == "apply_charge"           ) return new        apply_charge_t( this, options_str );

    return player_t::create_action( name, options_str );
}

// vanguard_powertech_t::init_talents =====================================================

void vanguard_powertech_t::init_talents()
{
    player_t::init_talents();

    // Darkness|Kinetic Combat
    //talents.thrashing_blades      = find_talent( "Thrashing Blades" );

    // Deception|Infiltration

    // Madness|Balance
}

// vanguard_powertech_t::init_base ========================================================

void vanguard_powertech_t::init_base()
{
    player_t::init_base();

    default_distance = 3;
    distance = default_distance;

}

// vanguard_powertech_t::init_benefits =======================================================

void vanguard_powertech_t::init_benefits()
{
    player_t::init_benefits();

}

// vanguard_powertech_t::init_buffs =======================================================

void vanguard_powertech_t::init_buffs()
{
    player_t::init_buffs();

    // buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
    // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
    // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

    //bool is_powertech = ( type == BH_POWERTECH );



}

// vanguard_powertech_t::init_gains =======================================================

void vanguard_powertech_t::init_gains()
{
    player_t::init_gains();

}

// vanguard_powertech_t::init_procs =======================================================

void vanguard_powertech_t::init_procs()
{
    player_t::init_procs();

}

// vanguard_powertech_t::init_rng =========================================================

void vanguard_powertech_t::init_rng()
{
    player_t::init_rng();

}

// vanguard_powertech_t::init_actions =====================================================

void vanguard_powertech_t::init_actions()
{
    //======================================================================================
    //
    //   Please Mirror all changes between Jedi Shadow and Sith Assassin!!!
    //
    //======================================================================================

    if ( action_list_str.empty() )
    {
        if ( type == T_VANGUARD )
        {
            action_list_str += "stim,type=exotech_resolve";
            action_list_str += "/snapshot_stats";

            switch ( primary_tree() )
            {


            default: break;
            }

            action_list_default = 1;
        }

        // BH_POWERTECH
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

// vanguard_powertech_t::primary_resource ===================================

resource_type vanguard_powertech_t::primary_resource() const
{ return RESOURCE_FORCE; }

// vanguard_powertech_t::primary_role =======================================

role_type vanguard_powertech_t::primary_role() const
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

// vanguard_powertech_t::create_talents =====================================

void vanguard_powertech_t::create_talents()
{
  // See sage_sorcerer_t::create_talents()
}

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_vanguard ================================================

player_t* player_t::create_vanguard( sim_t* sim, const std::string& name, race_type r )
{
    return new vanguard_powertech_t( sim, T_VANGUARD, name, r );
}

// player_t::create_powertech ===============================================

player_t* player_t::create_powertech( sim_t* sim, const std::string& name, race_type r )
{
    return new vanguard_powertech_t( sim, BH_POWERTECH, name, r );
}

// player_t::vanguard_powertech_init ========================================

void player_t::vanguard_powertech_init( sim_t* /* sim */ )
{

}

// player_t::vanguard_powertech_combat_begin ================================

void player_t::vanguard_powertech_combat_begin( sim_t* /* sim */ )
{

}
