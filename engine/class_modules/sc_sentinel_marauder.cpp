// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "../simulationcraft.hpp"
#include "warr_knight.hpp"

// ==========================================================================
// Jedi Sentinel | Sith Marauder
// ==========================================================================

namespace { // ANONYMOUS ====================================================

namespace sentinel_marauder {

class class_t;

enum form_type
{
  FORM_NONE = 0, JUYO_FORM, SHII_CHO_FORM, ATARU_FORM
};

struct sentinel_marauder_targetdata_t : public targetdata_t
{
  dot_t rupture;
  dot_t deadly_saber;

  sentinel_marauder_targetdata_t( player_t& source, player_t& target )
    : targetdata_t( source, target ) {}
};


struct class_t : public warr_knight::class_t
{
    typedef warr_knight::class_t base_t;
    // Buffs
    struct buffs_t
    {
      buff_t* juyo_form;
      buff_t* annihlator;
      buff_t* fury;
      buff_t* deadly_saber;
      buff_t* beserk;
      buff_t* bloodthirst;
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
      rng_t* enraged_charge;
      rng_t* pulverize;
      rng_t* empowerment;
    } rngs;

    // Benefits
    struct benefits_t
    {

    } benefits;

    // Cooldowns
    struct cooldowns_t
    {
      cooldown_t* deadly_saber;
      cooldown_t* force_charge;
      cooldown_t* rupture;
      cooldown_t* annihilate;
      cooldown_t* ravage;
      cooldown_t* battering_assault;
      cooldown_t* bloodthirst;
      cooldown_t* vicious_throw;
      cooldown_t* frenzy;
      cooldown_t* force_choke;
      cooldown_t* smash;
    } cooldowns;

    // Talents
    struct talents_t
    {
        // Annihilation|Watchman
        // t1
        talent_t* cloak_of_annihilation;
        talent_t* short_fuse;
        talent_t* enraged_slash;
        //t2
        talent_t* juyo_mastery;
        talent_t* seeping_wound;
        talent_t* hungering;
        //t3
        talent_t* bleedout;
        talent_t* deadly_saber;
        talent_t* blurred_speed;
        //t4
        talent_t* enraged_charge;
        talent_t* subjugation;
        talent_t* deep_wound;
        talent_t* close_quarters;
        //t5
        talent_t* phantom;
        talent_t* pulverize;
        //t6
        talent_t* empowerment;
        talent_t* hemorrhage;
        //t7
        talent_t* annihilate;

        // Carnage|Combat
        //t1
        talent_t* cloak_of_carnage;
        talent_t* dual_wield_mastery;
        talent_t* defensive_forms;
        //t2
        talent_t* narrowed_hatred;
        talent_t* defensive_roll;
        talent_t* stagger;
        //t3
        talent_t* execute;
        talent_t* ataru_form;
        talent_t* ataru_mastery;
        //t4
        talent_t* blood_frenzy;
        talent_t* towering_rage;
        talent_t* enraged_assault;
        talent_t* displacement;
        //t5
        talent_t* unbound;
        talent_t* gore;
        talent_t* rattling_voice;
        //t6
        talent_t* overwhelm;
        talent_t* sever;
        //t7
        talent_t* massacre;

        // Rage|Focus
        //t1
        talent_t* ravager;
        talent_t* malice;
        talent_t* decimate;
        //t2
        talent_t* payback;
        talent_t* overpower;
        talent_t* enraged_scream;
        talent_t* brutality;
        //t3
        talent_t* saber_strength;
        talent_t* obliterate;
        talent_t* strangulate;
        //t4
        talent_t* dominate;
        talent_t* shockwave;
        talent_t* berserker;
        //t5
        talent_t* gravity;
        talent_t* interceptor;
        talent_t* shii_cho_mastery;
        //t6
        talent_t* dark_resonance;
        talent_t* undying;
        //t7
        talent_t* force_crush;

    } talents;

    struct actives_t
    {
      form_type form;
    } actives;

    class_t( sim_t* sim, player_type pt, const std::string& name, race_type r = RACE_NONE ) :
      base_t( sim, pt == SITH_MARAUDER ? SITH_MARAUDER : JEDI_SENTINEL, name, ( r == RACE_NONE ) ? RACE_HUMAN : r ),
      buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents(), actives()
    {

      tree_type[ SITH_MARAUDER_ANNIHILATION ] = TREE_ANNIHILATION;
      tree_type[ SITH_MARAUDER_CARNAGE ] = TREE_CARNAGE;
      tree_type[ SITH_MARAUDER_RAGE ] = TREE_RAGE;

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
    virtual resource_type primary_resource() const;
    virtual role_type primary_role() const;
            void      create_talents();

    virtual void init_scaling()
    {
      player_t::init_scaling();
      scales_with[ STAT_FORCE_POWER ] = true;
    }
};


class action_t : public ::action_t
{
  typedef ::action_t base_t;
public:
  action_t( const std::string& n, class_t* player,
                          attack_policy_t policy, resource_type r, school_type s ) :
    base_t( ACTION_ATTACK, n, player, policy, r, s )
  {}

  sentinel_marauder_targetdata_t* targetdata() const
  { return static_cast<sentinel_marauder_targetdata_t*>( action_t::targetdata() ); }

  class_t* p() const
  { return static_cast<class_t*>( player ); }
};

// ==========================================================================
// Sentinel / Marauder Abilities
// ==========================================================================

struct attack_t : public action_t
{
    attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_KINETIC ) :
      action_t( n, p, melee_policy, RESOURCE_NONE, s )
    {
        may_crit   = true;
    }

};

struct spell_t : public action_t
{
    spell_t( const std::string& n, class_t* p, school_type s=SCHOOL_KINETIC ) :
      action_t( n, p, force_policy, RESOURCE_NONE, s )
    {
        may_crit   = true;
        tick_may_crit = true;
    }

};

// ==========================================================================
// sentinel_marauder Character Definition
// ==========================================================================

// class_t::create_action ====================================================

::action_t* class_t::create_action( const std::string& name,
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

// class_t::init_talents =====================================================

void class_t::init_talents()
{
    player_t::init_talents();

    // Darkness|Kinetic Combat
    //talents.thrashing_blades      = find_talent( "Thrashing Blades" );

    // Deception|Infiltration

    // Madness|Balance
}

// class_t::init_base ========================================================

void class_t::init_base()
{
    player_t::init_base();

    default_distance = 3;
    distance = default_distance;

}

// class_t::init_benefits =======================================================

void class_t::init_benefits()
{
    player_t::init_benefits();

}

// class_t::init_buffs =======================================================

void class_t::init_buffs()
{
    player_t::init_buffs();

    // buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
    // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
    // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

    //bool is_juggernaut = ( type == SITH_MARAUDER );



}

// class_t::init_gains =======================================================

void class_t::init_gains()
{
    player_t::init_gains();

}

// class_t::init_procs =======================================================

void class_t::init_procs()
{
    player_t::init_procs();

}

// class_t::init_rng =========================================================

void class_t::init_rng()
{
    player_t::init_rng();

}

// class_t::init_actions =====================================================

void class_t::init_actions()
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
            action_list_str += "stim,type=exotech_might";
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
            action_list_str += "stim,type=exotech_might";
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

// class_t::primary_resource ==================================================

resource_type class_t::primary_resource() const
{ return RESOURCE_RAGE; }

// class_t::primary_role ==================================================

role_type class_t::primary_role() const
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

// class_t::create_talents ==================================================

void class_t::create_talents()
{
  // See sage_sorcerer_t::create_talents()
}

} // namespace sentinel_marauder ============================================

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_jedi_sentinel  ============================================

player_t* player_t::create_jedi_sentinel( sim_t* sim, const std::string& name, race_type r )
{
    return new sentinel_marauder::class_t( sim, JEDI_SENTINEL, name, r );
}

// player_t::create_SITH_MARAUDER  ==========================================

player_t* player_t::create_sith_marauder( sim_t* sim, const std::string& name, race_type r )
{
    return new sentinel_marauder::class_t( sim, SITH_MARAUDER, name, r );
}

// player_t::sentinel_marauder_init ===========================================

void player_t::sentinel_marauder_init( sim_t* /* sim */ )
{

}

// player_t::sentinel_marauder_combat_begin ===================================

void player_t::sentinel_marauder_combat_begin( sim_t* /* sim */ )
{

}
