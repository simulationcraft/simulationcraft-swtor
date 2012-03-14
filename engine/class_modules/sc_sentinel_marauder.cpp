// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "../simulationcraft.hpp"

struct sentinel_marauder_targetdata_t : public targetdata_t
{
  sentinel_marauder_targetdata_t( player_t& source, player_t& target )
    : targetdata_t( source, target )
  {}
};


// ==========================================================================
// Jedi Shadow | Sith Assassin
// ==========================================================================

struct sentinel_marauder_t : public player_t
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

    sentinel_marauder_t( sim_t* sim, player_type pt, const std::string& name, race_type r = RACE_NONE ) :
        player_t( sim, pt == SITH_MARAUDER ? SITH_MARAUDER : JEDI_SENTINEL, name, ( r == RACE_NONE ) ? RACE_HUMAN : r )
    {


      primary_attribute   = ATTR_STRENGTH;
      secondary_attribute = ATTR_WILLPOWER;

      create_talents();
      create_options();
    }

    // Character Definition
    virtual targetdata_t* new_targetdata( player_t& target ) // override
    { return new sentinel_marauder_targetdata_t( *this, target ); }

    virtual action_t* create_action( const std::string& name, const std::string& options );
    virtual void      init_talents();
    virtual void      init_base();
    virtual void      init_benefits();
    virtual void      init_buffs();
    virtual void      init_gains();
    virtual void      init_procs();
    virtual void      init_rng();
    virtual void      init_actions();
    virtual int       primary_resource() const;
    virtual int       primary_role() const;
            void      create_talents();

    virtual void init_scaling()
    {
      player_t::init_scaling();
      scales_with[ STAT_FORCE_POWER ] = true;
    }
};

namespace { // ANONYMOUS NAMESPACE ==========================================

class sentinel_marauder_action_t : public action_t
{
public:
  sentinel_marauder_action_t( const std::string& n, sentinel_marauder_t* player,
                          attack_policy_t policy, resource_type r, school_type s ) :
    action_t( ACTION_ATTACK, n.c_str(), player, policy, r, s )
  {}

  sentinel_marauder_targetdata_t* targetdata() const
  { return static_cast<sentinel_marauder_targetdata_t*>( action_t::targetdata() ); }

  sentinel_marauder_t* p() const
  { return static_cast<sentinel_marauder_t*>( player ); }
};

// ==========================================================================
// Sentinel / Marauder Abilities
// ==========================================================================

struct sentinel_marauder_attack_t : public sentinel_marauder_action_t
{
    sentinel_marauder_attack_t( const std::string& n, sentinel_marauder_t* p, school_type s=SCHOOL_KINETIC ) :
      sentinel_marauder_action_t( n, p, melee_policy, RESOURCE_NONE, s )
    {
        may_crit   = true;
    }

};

struct sentinel_marauder_spell_t : public sentinel_marauder_action_t
{
    sentinel_marauder_spell_t( const std::string& n, sentinel_marauder_t* p, school_type s=SCHOOL_KINETIC ) :
      sentinel_marauder_action_t( n, p, force_policy, RESOURCE_NONE, s )
    {
        may_crit   = true;
        tick_may_crit = true;
    }

};


} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// sentinel_marauder Character Definition
// ==========================================================================

// sentinel_marauder_t::create_action ====================================================

action_t* sentinel_marauder_t::create_action( const std::string& name,
                                            const std::string& options_str )
{
    if ( type == SITH_MARAUDER )
    {

    }
    else if ( type == JEDI_SENTINEL )
    {

    }

    //if ( name == "apply_charge"           ) return new        apply_charge_t( this, options_str );

    return player_t::create_action( name, options_str );
}

// sentinel_marauder_t::init_talents =====================================================

void sentinel_marauder_t::init_talents()
{
    player_t::init_talents();

    // Darkness|Kinetic Combat
    //talents.thrashing_blades      = find_talent( "Thrashing Blades" );

    // Deception|Infiltration

    // Madness|Balance
}

// sentinel_marauder_t::init_base ========================================================

void sentinel_marauder_t::init_base()
{
    player_t::init_base();

    default_distance = 3;
    distance = default_distance;

}

// sentinel_marauder_t::init_benefits =======================================================

void sentinel_marauder_t::init_benefits()
{
    player_t::init_benefits();

}

// sentinel_marauder_t::init_buffs =======================================================

void sentinel_marauder_t::init_buffs()
{
    player_t::init_buffs();

    // buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
    // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
    // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

    //bool is_juggernaut = ( type == SITH_MARAUDER );



}

// sentinel_marauder_t::init_gains =======================================================

void sentinel_marauder_t::init_gains()
{
    player_t::init_gains();

}

// sentinel_marauder_t::init_procs =======================================================

void sentinel_marauder_t::init_procs()
{
    player_t::init_procs();

}

// sentinel_marauder_t::init_rng =========================================================

void sentinel_marauder_t::init_rng()
{
    player_t::init_rng();

}

// sentinel_marauder_t::init_actions =====================================================

void sentinel_marauder_t::init_actions()
{
    //======================================================================================
    //
    //   Please Mirror all changes between Jedi Shadow and Sith Assassin!!!
    //
    //======================================================================================

    if ( action_list_str.empty() )
    {
        if ( type == JEDI_SENTINEL )
        {
            action_list_str += "stim,type=exotech_resolve";
            action_list_str += "/snapshot_stats";

            switch ( primary_tree() )
            {


            default: break;
            }

            action_list_default = 1;
        }

        // Sith ASSASSIN
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

// sentinel_marauder_t::primary_resource ==================================================

int sentinel_marauder_t::primary_resource() const
{
    return RESOURCE_FORCE;
}

// sentinel_marauder_t::primary_role ==================================================

int sentinel_marauder_t::primary_role() const
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

// sentinel_marauder_t::create_talents ==================================================

void sentinel_marauder_t::create_talents()
{
  // See sage_sorcerer_t::create_talents()
}

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_jedi_shadow  ============================================

player_t* player_t::create_jedi_sentinel( sim_t* sim, const std::string& name, race_type r )
{
    return new sentinel_marauder_t( sim, JEDI_SENTINEL, name, r );
}

// player_t::create_SITH_MARAUDER  ==========================================

player_t* player_t::create_sith_marauder( sim_t* sim, const std::string& name, race_type r )
{
    return new sentinel_marauder_t( sim, SITH_MARAUDER, name, r );
}

// player_t::sentinel_marauder_init ===========================================

void player_t::sentinel_marauder_init( sim_t* /* sim */ )
{

}

// player_t::sentinel_marauder_combat_begin ===================================

void player_t::sentinel_marauder_combat_begin( sim_t* /* sim */ )
{

}
