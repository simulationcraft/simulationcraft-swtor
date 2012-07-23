// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "warr_knight.hpp"

namespace warr_knight { // ===================================================

targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  ::targetdata_t( source, target )
{
}


// ==========================================================================
// Warrior | Knight Character Definitions
// ==========================================================================

// class_t::create_action =======================================

::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  //if ( name == m.a_adrenaline_probe      ) return new adrenaline_probe_t      ( this, name, options_str ) ;

  return base_t::create_action( name, options_str );
}

// class_t::init_talents =======================================

void class_t::init_talents()
{
  base_t::init_talents();

  // Lethality|Dirty Fighting
  // t1
  //talents.deadly_directive     = find_talent( m.t_deadly_directive    );
  // t2
  // t3
  // t4
  // t5
  // t6
  // t7
}

// class_t::init_benefits =====================================

void class_t::init_benefits()
{
  base_t::init_benefits();
}

// class_t::init_buffs ===========================================

void class_t::init_buffs()
{
  base_t::init_buffs();
}

// class_t::init_procs ========================================

void class_t::init_procs()
{
  base_t::init_procs();
}

// class_t::init_rng ==========================================

void class_t::init_rng()
{
  base_t::init_rng();
}

// class_t::init_actions ========================================

void class_t::init_actions()
{
  base_t::init_actions();
}

// class_t::create_mirror =======================================

void class_t::create_mirror()
{
  //bool ag = type == IA_SNIPER || type == IA_OPERATIVE;

  // ABILITY                  =    ? AGENT LABEL              : SMUGGLER LABEL          ;
  //m.a_adrenaline_probe        = ag ? "adrenaline_probe"       : "cool_head"             ;

  // Shared Talents
  // t1
  //m.t_deadly_directive          = ag ? "deadly_directive"     : "black_market_mods"     ;
}

// class_t::create_talents =======================================

void class_t::create_talents()
{
}

} // namespace agent_smug ===================================================
