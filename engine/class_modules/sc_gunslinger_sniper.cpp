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
  struct buffs_t:base_t::buffs_t
  {

  } buffs;

  // Gains
  struct gains_t:base_t::gains_t
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
  struct benefits_t:base_t::benefits_t
  {

  } benefits;

  // Cooldowns
  struct cooldowns_t
  {

  } cooldowns;

  // Talents
  struct talents_t:base_t::talents_t
  {
    // Marksmanship|Sharpshooter
    // t1
    talent_t* cover_screen;
    talent_t* steady_shots;
    talent_t* marksmanship;
    // t2
    talent_t* heavy_shot;
    talent_t* ballistic_dampers;
    talent_t* precision_ambush;
    talent_t* imperial_demarcation;
    // t3
    talent_t* snap_shot;
    talent_t* diversion;
    talent_t* reactive_shot;
    // t4
    talent_t* between_the_eyes;
    talent_t* sector_ranger;
    talent_t* sniper_volley;
    talent_t* snipers_nest;
    // t5
    talent_t* recoil_control;
    talent_t* followthrough;
    talent_t* pillbox_sniper;
    // t6
    talent_t* imperial_assassin;
    talent_t* siege_bunker;
    // t7
    talent_t* rapid_fie;

    // Engineering|Saboteur
    // t1
    talent_t* engineers_tool_belt;
    talent_t* energy_tanks;
    talent_t* gearhead;
    // t2
    talent_t* explosive_engineering;
    talent_t* vital_regulators;
    talent_t* stroke_of_genius;
    talent_t* vitality_serum;
    // t3
    talent_t* cluster_bombs;
    talent_t* efficient_engineering;
    talent_t* interrogation_probe;
    talent_t* inventive_interrogation_techniques;
    // t4
    talent_t* imperial_methodology;
    talent_t* calculated_pursuit;
    talent_t* experimental_explosives;
    // t5
    talent_t* energy_overrides;
    talent_t* emp_discharge;
    talent_t* augmented_shields;
    // t6
    talent_t* deployed_shields;
    talent_t* electrified_railgun;
    // t7
    talent_t* plasma_probe;
  } talents;

  // Abilities
  struct abilities_t:base_t::abilities_t
  {
    std::string ambush;
  } abilities;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    // TODO CHECK WTF IA_SNIPER ? IA_SNIPER : S_GUNSLINGER? IA_SNIPER is a constant it will always be true..?
    base_t( sim, pt == IA_SNIPER ? IA_SNIPER : S_GUNSLINGER, name, rt, buffs, gains, benefits, talents, abilities ),
    buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents()
  {
    tree_type[ IA_SNIPER_MARKSMANSHIP ] = TREE_MARKSMANSHIP;
    tree_type[ IA_SNIPER_ENGINEERING  ] = TREE_ENGINEERING;
    tree_type[ IA_LETHALITY           ] = TREE_LETHALITY;

    create_talents();
    create_options();
  }

  // Character Definition
  targetdata_t* new_targetdata( player_t& target ) // override
  { return new targetdata_t( *this, target ); }
  ::action_t* create_action( const std::string& name, const std::string& options );
  void      init_talents();
  void      init_abilities();
  void      init_base();
  void      init_benefits();
  void      init_buffs();
  void      init_gains();
  void      init_procs();
  void      init_rng();
  void      init_actions();
  role_type primary_role() const;
  void      create_talents();
};

targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  agent_smug::targetdata_t( source, target )
{}

// ==========================================================================
// Gunslinger / Sniper Abilities
// ==========================================================================

// Ambush | Aimed Shot ======================================================
struct ambush_t : public agent_smug::range_attack_t
{
  typedef agent_smug::range_attack_t base_t;
  ambush_t* offhand_attack;

  ambush_t( class_t* p, const std::string& n, const std::string& options_str,
     bool is_offhand=false ) :
    range_attack_t( n, p )
  {
    // todo: get this list right
    rank_level_list = { 8, 12, 16, 22, 31, 40, 50 };

    parse_options( options_str );

    base_cost                    = 15;
    base_execute_time            = from_seconds( 2.5 );
    cooldown -> duration         = from_seconds( 15 );
    range                        = 35.0;
    dd.standardhealthpercentmin  =
    dd.standardhealthpercentmax  = 0.329;
    dd.power_mod                 = 3.29;
    weapon                       = &( player -> main_hand_weapon );
    weapon_multiplier            = 1.2;

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

    else if ( player -> type == S_GUNSLINGER && &( player -> off_hand_weapon ) )
    {
      offhand_attack             = new ambush_t( p, n+"_offhand", options_str, true );
      add_child( offhand_attack );
    }
  }

  virtual void execute()
  {
    base_t::execute();
    if ( offhand_attack )
      offhand_attack -> schedule_execute();
  }
};

// ==========================================================================
// Gunslinger / Sniper Character Definition
// ==========================================================================

// class_t::create_action =======================================
::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  if ( name == abilities.ambush ) return new ambush_t( this, name, options_str );
  return base_t::create_action( name, options_str );
}

// class_t::init_abilities =======================================

void class_t::init_abilities()
{
  base_t::init_abilities();

  bool sn = type == IA_SNIPER;

  //               =    ? SNIPER LABEL : GUNSLINGER LABEL ;
  abilities.ambush = sn ? "ambush"     : "aimed_shot"     ;

}

// class_t::init_talents ========================================

void class_t::init_talents()
{
  base_t::init_talents();

  // Marksmanship|Sharpshooter
  // t1
  talents.cover_screen                        = find_talent( "Cover Screen"                       );
  talents.steady_shots                        = find_talent( "Steady Shots"                       );
  talents.marksmanship                        = find_talent( "Marksmanship"                       );
  // t2
  talents.heavy_shot                          = find_talent( "Heavy Shot"                         );
  talents.ballistic_dampers                   = find_talent( "Ballistic Dampers"                  );
  talents.precision_ambush                    = find_talent( "Precision Ambush"                   );
  talents.imperial_demarcation                = find_talent( "Imperial Demarcation"               );
  // t3
  talents.snap_shot                           = find_talent( "Snap Shot"                          );
  talents.diversion                           = find_talent( "Diversion"                          );
  talents.reactive_shot                       = find_talent( "Reactive Shot"                      );
  // t4
  talents.between_the_eyes                    = find_talent( "Between the Eyes"                   );
  talents.sector_ranger                       = find_talent( "Sector Ranger"                      );
  talents.sniper_volley                       = find_talent( "Sniper Volley"                      );
  talents.snipers_nest                        = find_talent( "Sniper's Nest"                      );
  // t5
  talents.recoil_control                      = find_talent( "Recoil Control"                     );
  talents.followthrough                       = find_talent( "Followthrough"                      );
  talents.pillbox_sniper                      = find_talent( "Pillbox Sniper"                     );
  // t6
  talents.imperial_assassin                   = find_talent( "Imperial Assassin"                  );
  talents.siege_bunker                        = find_talent( "Siege Bunker"                       );
  // t7
  talents.rapid_fie                           = find_talent( "Rapid Fire"                         );

  // Engineering|Saboteur
  // t1
  talents.engineers_tool_belt                 = find_talent( "Engineer's Tool Belt"               );
  talents.energy_tanks                        = find_talent( "Energy Tanks"                       );
  talents.gearhead                            = find_talent( "Gearhead"                           );
  // t2
  talents.explosive_engineering               = find_talent( "Explosive Engineering"              );
  talents.vital_regulators                    = find_talent( "Vital Regulators"                   );
  talents.stroke_of_genius                    = find_talent( "Stroke of Genius"                   );
  talents.vitality_serum                      = find_talent( "Vitality Serum"                     );
  // t3
  talents.cluster_bombs                       = find_talent( "Cluster Bombs"                      );
  talents.efficient_engineering               = find_talent( "Efficient Engineering"              );
  talents.interrogation_probe                 = find_talent( "Interrogation Probe"                );
  talents.inventive_interrogation_techniques  = find_talent( "Inventive Interrogation Techniques" );
  // t4
  talents.imperial_methodology                = find_talent( "Imperial Methodology"               );
  talents.calculated_pursuit                  = find_talent( "Calculated Pursuit"                 );
  talents.experimental_explosives             = find_talent( "Experimental Explosives"            );
  // t5
  talents.energy_overrides                    = find_talent( "Energy Overrides"                   );
  talents.emp_discharge                       = find_talent( "EMP Discharge"                      );
  talents.augmented_shields                   = find_talent( "Augmented Shields"                  );
  // t6
  talents.deployed_shields                    = find_talent( "Deployed Shields"                   );
  talents.electrified_railgun                 = find_talent( "Electrified Railgun"                );
  // t7
  talents.plasma_probe                        = find_talent( "Plasma Probe"                       );
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
  const std::string sl = "/";

  if ( action_list_str.empty() )
  {
    action_list_default = true;
    action_list_str += sl + abilities.ambush + ",if=energy>75";
    // guessing priority and optimal energy
    action_list_str += sl + abilities.orbital_strike + ",if=energy>65";
    // guessing priority and optimal energy
    action_list_str += sl + abilities.snipe + ",if=energy>75";
    action_list_str += sl + abilities.rifle_shot;
  }

  base_t::init_actions();
}

// class_t::primary_role ========================================

role_type class_t::primary_role() const
{
  return ROLE_DPS;
}

// class_t::create_talents ======================================

void class_t::create_talents()
{
  base_t::create_talents();

  // Marksmanship|Sharpshooter
  static const talentinfo_t marksmanship_tree[] = {
     { "Cover Screen"      , 2 }, { "Steady Shots"      , 2 }, { "Marksmanship"     , 3 },
     { "Heavy Shot"        , 1 }, { "Ballistic Dampers" , 2 }, { "Precision Ambush" , 2 }, { "Imperial Demarcation" , 2 },
     { "Snap Shot"         , 2 }, { "Diversion"         , 1 }, { "Reactive Shot"    , 2 },
     { "Between the Eyes"  , 2 }, { "Sector Ranger"     , 1 }, { "Sniper Volley"    , 3 }, { "Sniper's Nest"        , 1 },
     { "Recoil Control"    , 2 }, { "Followthrough"     , 1 }, { "Pillbox Sniper"   , 2 },
     { "Imperial Assassin" , 3 }, { "Siege Bunker"      , 2 },
     { "Rapid Fire"        , 1 }
  };
  init_talent_tree( IA_SNIPER_MARKSMANSHIP, marksmanship_tree );

  // Engineering|Saboteur
  static const talentinfo_t engineering_tree[] = {
     { "Engineer's Tool Belt"  , 2 }, { "Energy Tanks"          , 2 }, { "Gearhead"                , 3 },
     { "Explosive Engineering" , 3 }, { "Vital Regulators"      , 2 }, { "Stroke of Genius"        , 2 }, { "Vitality Serum"                     , 2 },
     { "Cluster Bombs"         , 2 }, { "Efficient Engineering" , 2 }, { "Interrogation Probe"     , 1 }, { "Inventive Interrogation Techniques" , 2 },
     { "Imperial Methodology"  , 1 }, { "Calculated Pursuit"    , 2 }, { "Experimental Explosives" , 2 },
     { "Energy Overrides"      , 2 }, { "EMP Discharge"         , 1 }, { "Augmented Shields"       , 2 },
     { "Deployed Shields"      , 2 }, { "Electrified Railgun"   , 3 },
     { "Plasma Probe"          , 1 }
  };
  init_talent_tree( IA_SNIPER_ENGINEERING, engineering_tree );
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
