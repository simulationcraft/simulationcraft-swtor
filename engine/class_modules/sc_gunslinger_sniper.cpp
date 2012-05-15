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

    // Lethality|Dirty Fighting
    // t1
    talent_t* deadly_directive;
    talent_t* lethality;
    talent_t* razor_edge;
    // t2
    talent_t* slip_away;
    talent_t* flash_powder;
    talent_t* corrosive_microbes;
    talent_t* lethal_injectors;
    // t3
    talent_t* corrosive_grenade;
    talent_t* combat_stims;
    talent_t* cut_down;
    // t4
    talent_t* lethal_purpose;
    talent_t* adhesive_corrosives;
    talent_t* escape_plan;
    talent_t* lethal_dose;
    // t5
    talent_t* cull;
    talent_t* license_to_kill;
    talent_t* counterstrike;
    // t6
    talent_t* devouring_microbes;
    talent_t* lingering_toxins;
    // t7
    talent_t* weakening_blast;
  } talents;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    base_t( sim, pt == IA_SNIPER ? IA_SNIPER : S_GUNSLINGER, name, rt ),
    buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents()
  {
    tree_type[ IA_SNIPER_MARKSMANSHIP ] = TREE_MARKSMANSHIP;
    tree_type[ IA_SNIPER_ENGINEERING  ] = TREE_ENGINEERING;
    tree_type[ IA_SNIPER_LETHALITY    ] = TREE_LETH;

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
  attack_t( const std::string& n, class_t* p, attack_policy_t policy, school_type s ) :
    action_t( n, p, policy, RESOURCE_ENERGY, s )
  {
    harmful  = true;
    may_crit = true;
  }
};

struct tech_attack_t : public attack_t
{
  tech_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_KINETIC ) :
    attack_t( n, p, tech_policy, s )
  {
  }
};

struct range_attack_t : public attack_t
{
  range_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_ENERGY ) :
    attack_t( n, p, range_policy, s )
  {
  }

};

// Rifle Shot | Flurry Of Bolts =========================================================

struct rifle_shot_t : public range_attack_t
{
  rifle_shot_t* second_strike;

  rifle_shot_t( class_t* p, const std::string& n, const std::string& options_str,
                bool is_consequent_strike = false ) :
    range_attack_t( n, p ), second_strike( 0 )
  {
    parse_options( options_str );

    base_cost = 0;
    range = 35.0;

    weapon = &( player->main_hand_weapon );
    weapon_multiplier = -0.5;
    dd.power_mod = 0.5;

    // Is a Basic attack
    base_accuracy -= 0.10;

    if ( is_consequent_strike )
    {
      background = dual = true;
      trigger_gcd = timespan_t::zero();
      base_execute_time = from_seconds( 0.75 );
    }
    else
      second_strike = new rifle_shot_t( p, n, options_str, true );
  }

  virtual void execute()
  {
    range_attack_t::execute();
    if ( second_strike )
        second_strike->schedule_execute();
  }
};

// ==========================================================================
// Gunslinger / Sniper Character Definition
// ==========================================================================

// class_t::create_action =======================================

::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  bool sniper = ( type == IA_SNIPER );

  const char* rifle_shot            = sniper ? "rifle_shot"            : "flurry_of_bolts"    ;

  if ( name == rifle_shot            ) return new rifle_shot_t            ( this, name, options_str ) ;

  return base_t::create_action( name, options_str );
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

  // Lethality|Dirty Fighting
  // t1
  talents.deadly_directive                    = find_talent( "Deadly Directive"                   );
  talents.lethality                           = find_talent( "Lethality"                          );
  talents.razor_edge                          = find_talent( "Razor Edge"                         );
  // t2
  talents.slip_away                           = find_talent( "Slip Away"                          );
  talents.flash_powder                        = find_talent( "Flash Powder"                       );
  talents.corrosive_microbes                  = find_talent( "Corrosive Microbes"                 );
  talents.lethal_injectors                    = find_talent( "Lethal Injectors"                   );
  // t3
  talents.corrosive_grenade                   = find_talent( "Corrosive Grenade"                  );
  talents.combat_stims                        = find_talent( "Combat Stims"                       );
  talents.cut_down                            = find_talent( "Cut Down"                           );
  // t4
  talents.lethal_purpose                      = find_talent( "Lethal Purpose"                     );
  talents.adhesive_corrosives                 = find_talent( "Adhesive Corrosives"                );
  talents.escape_plan                         = find_talent( "Escape Plan"                        );
  talents.lethal_dose                         = find_talent( "Lethal Dose"                        );
  // t5
  talents.cull                                = find_talent( "Cull"                               );
  talents.license_to_kill                     = find_talent( "License to Kill"                    );
  talents.counterstrike                       = find_talent( "Counterstrike"                      );
  // t6
  talents.devouring_microbes                  = find_talent( "Devouring Microbes"                 );
  talents.lingering_toxins                    = find_talent( "Lingering Toxins"                   );
  // t7
  talents.weakening_blast                     = find_talent( "Weakening Blast"                    );
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
  bool sniper = ( type == IA_SNIPER );
  const std::string rifle_shot            = sniper ? "rifle_shot"            : "flurry_of_bolts"    ;
  const std::string sl = "/";

  if ( action_list_str.empty() )
  {
    action_list_str += sl + rifle_shot;
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
  // Marksmanship|Sharpshooter
  static const talentinfo_t marksmanship_tree[] = {
    { "Cover Screen", 2 }, { "Steady Shots", 2 }, { "Marksmanship", 3 },
    { "Heavy Shot", 1 }, { "Ballistic Dampers", 2 }, { "Precision Ambush", 2}, { "Imperial Demarcation", 2 },
    { "Snap Shot", 2 }, { "Diversion", 1 }, { "Reactive Shot", 2 },
    { "Between the Eyes", 2 }, { "Sector Ranger", 1 }, { "Sniper Volley", 3 }, { "Sniper's Nest", 1},
    { "Recoil Control", 2 }, { "Followthrough", 1 }, { "Pillbox Sniper", 2 },
    { "Imperial Assassin", 3 }, { "Siege Bunker", 2},
    { "Rapid Fire", 1 }
  };
  init_talent_tree( IA_SNIPER_MARKSMANSHIP, marksmanship_tree );

  // Engineering|Saboteur
  static const talentinfo_t engineering_tree[] = {
    { "Engineer's Tool Belt", 2 }, { "Energy Tanks", 2 }, { "Gearhead", 3 },
    { "Explosive Engineering", 3 }, { "Vital Regulators", 2 }, { "Stroke of Genius", 2 }, { "Vitality Serum", 2},
    { "Cluster Bombs", 2 }, { "Efficient Engineering", 2 }, { "Interrogation Probe", 1 }, { "Inventive Interrogation Techniques", 2},
    { "Imperial Methodology", 1 }, { "Calculated Pursuit", 2 }, { "Experimental Explosives", 2 },
    { "Energy Overrides", 2 }, { "EMP Discharge", 1 }, { "Augmented Shields", 2},
    { "Deployed Shields", 2 }, { "Electrified Railgun", 3},
    { "Plasma Probe", 1 }
  };
  init_talent_tree( IA_SNIPER_ENGINEERING, engineering_tree );

  // Lethality|Dirty Fighting
  static const talentinfo_t lethality_tree[] = {
    { "Deadly Directive", 2 }, { "Lethality", 3 }, { "Razor Edge", 2 },
    { "Slip Away", 2 }, { "Flash Powder", 2 }, { "Corrosive Microbes", 2 }, { "Lethal Injectors", 1 },
    { "Corrosive Grenade", 1 }, { "Combat Stims", 2 }, { "Cut Down", 2 },
    { "Lethal Purpose", 2 }, { "Adhesive Corrosives", 2 }, { "Escape Plan", 2 }, { "Lethal Dose", 3 },
    { "Cull", 1 }, { "License to Kill", 2 }, { "Counterstrike", 2 },
    { "Devouring Microbes", 3 }, { "Lingering Toxins", 2 }, { "Weakening Blast", 1 },
  };
  init_talent_tree( IA_SNIPER_LETHALITY, lethality_tree );
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
