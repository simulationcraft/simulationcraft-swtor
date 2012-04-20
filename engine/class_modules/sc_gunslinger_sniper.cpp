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
};

class class_t : public agent_smug::class_t
{
public:
  typedef agent_smug::class_t base_t;

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

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    base_t( sim, pt == IA_SNIPER ? IA_SNIPER : S_GUNSLINGER, name, rt ),
    buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents()
  {
    create_talents();
    create_options();
  }

  // Character Definition
  virtual targetdata_t* new_targetdata( player_t& target ) // override
  { return new targetdata_t( *this, target ); }
  virtual ::action_t* create_action( const std::string& name, const std::string& options );
  virtual void      init_talents();
  virtual void      init_base();
  virtual void      init_benefits();
  virtual void      init_buffs();
  virtual void      init_gains();
  virtual void      init_procs();
  virtual void      init_rng();
  virtual void      init_actions();
  virtual role_type primary_role() const;
          void      create_talents();
};

targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  agent_smug::targetdata_t( source, target )
{}

class action_t : public ::action_t
{
  typedef ::action_t base_t;
public:
  action_t( const std::string& n, class_t* player,
            attack_policy_t policy, resource_type r, school_type s ) :
    base_t( ACTION_ATTACK, n, player, policy, r, s )
  {}

  targetdata_t* targetdata() const
  { return static_cast<targetdata_t*>( base_t::targetdata() ); }

  class_t* p() const
  { return static_cast<class_t*>( player ); }
};

// ==========================================================================
// Gunslinger / Sniper Abilities
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
// Gunslinger / Sniper Character Definition
// ==========================================================================

// class_t::create_action =======================================

::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  if ( type == IA_SNIPER )
  {

  }
  else if ( type == S_GUNSLINGER )
  {

  }

  //if ( name == "apply_charge"           ) return new        apply_charge_t( this, options_str );

  return base_t::create_action( name, options_str );
}

// class_t::init_talents ========================================

void class_t::init_talents()
{
  base_t::init_talents();

  // Darkness|Kinetic Combat
  //talents.thrashing_blades      = find_talent( "Thrashing Blades" );

  // Deception|Infiltration

  // Madness|Balance
}

// class_t::init_base ===========================================

void class_t::init_base()
{
  base_t::init_base();

  default_distance = 3;
  distance = default_distance;

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
  // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

  //bool is_juggernaut = ( type == SITH_MARAUDER );



}

// class_t::init_gains ==========================================

void class_t::init_gains()
{
  base_t::init_gains();

}

// class_t::init_procs ==========================================

void class_t::init_procs()
{
  base_t::init_procs();

}

// class_t::init_rng ============================================

void class_t::init_rng()
{
  base_t::init_rng();

}

// class_t::init_actions ========================================

void class_t::init_actions()
{
  //=======================================================================
  //
  //   Please Mirror all changes between Gunslinger and Sniper!!!
  //
  //=======================================================================

  if ( action_list_str.empty() )
  {
    if ( type == S_GUNSLINGER )
    {
      action_list_str += "stim,type=exotech_resolve";
      action_list_str += "/snapshot_stats";

      switch ( primary_tree() )
      {


      default: break;
      }

      action_list_default = 1;
    }

    // IA_SNIPER
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

  base_t::init_actions();
}

// class_t::primary_role ========================================

role_type class_t::primary_role() const
{
  switch ( base_t::primary_role() )
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

// class_t::create_talents ======================================

void class_t::create_talents()
{
  // See sage_sorcerer_t::create_talents()
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
