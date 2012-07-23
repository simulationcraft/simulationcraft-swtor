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

  // Rage|Focus
  //t1
  talents.ravager                     = find_talent( "Ravager" );
  talents.malice                      = find_talent( "Malice");
  talents.decimate                    = find_talent( "Decimate" );
  //t2
  talents.payback                     = find_talent( "Payback" );
  talents.overpower                   = find_talent( "Overpower" );
  talents.enraged_scream              = find_talent( "Enraged Scream" );
  talents.brutality                   = find_talent( "Brutality" );
  //t3
  talents.saber_strength              = find_talent( "Saber Strength" );
  talents.obliterate                  = find_talent( "Obliterate" );
  talents.strangulate                 = find_talent( "Strangulate" );
  //t4
  talents.dominate                    = find_talent( "Dominate" );
  talents.shockwave                   = find_talent( "Shockwave" );
  talents.berserker                   = find_talent( "Berserker" );
  //t5
  talents.gravity                     = find_talent( "Gravity" );
  talents.interceptor                 = find_talent( "Interceptor" );
  talents.shii_cho_mastery            = find_talent( "Shii Cho Mastery" );
  //t6
  talents.dark_resonance              = find_talent( "Dark Resonance" );
  talents.undying                     = find_talent( "Undying" );
  //t7
  talents.force_crush                 = find_talent( "Force Crush" );
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
  static const talentinfo_t rage_tree[] = {
    // t1
    { "Ravager"                 , 3 }, { "Malice"             , 3 }, { "Decimate"         , 2 },
    // t2
    { "Payback"                 , 2 }, { "Overpower"          , 2 }, { "Enraged Scream"   , 1 }, { "Brutality"        , 2 },
    // t3
    { "Saber Strength"          , 2 }, { "Obliterate"         , 1 }, { "Strangulate"      , 2 },
    // t4
    { "Dominate"                , 3 }, { "Shockwave"          , 2 }, { "Berserker"        , 2 },
    // t5
    { "Gravity"                 , 1 }, { "Interceptor"        , 2 }, { "Shii Cho Mastery" , 2 },
    // t6
    { "Dark Resonance"          , 3 }, { "Undying"            , 2 },
    // t7
    { "Force Crush"             , 1 }
  };
  init_talent_tree( SITH_MARAUDER_RAGE, rage_tree );
}

} // namespace agent_smug ===================================================
