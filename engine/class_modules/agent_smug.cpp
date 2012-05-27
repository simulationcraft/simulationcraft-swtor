// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "agent_smug.hpp"

namespace agent_smug { // ===================================================

targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  ::targetdata_t( source, target )
{
  dot_adrenaline_probe       = dot_t      ( source.abilities.adrenaline_probe, &source               );

  add( dot_adrenaline_probe       ) ;
}

// ==========================================================================
// Smuggler / Agent Abilities
// ==========================================================================

// Adrenaline Probe | Cool Head =============================================

struct adrenaline_probe_t : public action_t
{
  adrenaline_probe_t( class_t* p, const std::string& n, const std::string& options_str ) :
    action_t(n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE)
  {
    parse_options( options_str );

    cooldown -> duration = from_seconds( 120 - 15 * p -> talents.lethal_purpose -> rank() );
    use_off_gcd = true;
    trigger_gcd = timespan_t::zero();

    num_ticks = 2;
    base_tick_time = from_seconds( 1.5 );
  }

  // the combat log isn't in sync with the game here.
  // the combat log shows after 1.5 seconds a tick of 8 and 34, and then another tick of 8 1.5s later.
  // what happens in game is you instantly get 34, and then two ticks of 8.
  void execute()
  {
    action_t::execute();
    class_t* p = cast();

    p -> buffs.adrenaline_probe -> trigger();
    p -> resource_gain( RESOURCE_ENERGY, 34, p -> gains.adrenaline_probe );
  }

  void tick(dot_t* d)
  {
    action_t::tick(d);
    class_t* p = cast();

    p -> resource_gain( RESOURCE_ENERGY, 8, p -> gains.adrenaline_probe );
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
    // todo variable to 35 for sniper
    range = 30.0;

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
// Smuggler / Agent Character Definition
// ==========================================================================

// class_t::create_action =======================================

::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  if ( name == abilities.adrenaline_probe ) return new adrenaline_probe_t ( this, name, options_str ) ;
  if ( name == abilities.rifle_shot ) return new rifle_shot_t ( this, name, options_str ) ;

  return base_t::create_action( name, options_str );
}

// class_t::init_abilities =======================================

void class_t::init_abilities()
{
  //=======================================================================
  //
  //   Please Mirror all changes between Smuggler and Agent!!!
  //
  //=======================================================================
  if ( type == IA_SNIPER || type == IA_OPERATIVE )
  {
    abilities.adrenaline_probe = "adrenaline_probe";
    abilities.coordination = "coordination";
    abilities.corrosive_dart = "corrosive_dart";
    abilities.explosive_probe = "explosive_probe";
    abilities.fragmentation_grenade = "fragmentation_grenade";
    abilities.orbital_strike = "orbital_strike";
    abilities.overload_shot = "overload_shot";
    abilities.rifle_shot = "rifle_shot";
    abilities.shiv = "shiv";
  }
  else
  {
    abilities.adrenaline_probe = "cool_head";
    abilities.coordination = "lucky_shots";
    abilities.corrosive_dart = "vital_shot";
    abilities.explosive_probe = "sabotage_charge";
    abilities.fragmentation_grenade = "thermal_grenade";
    abilities.orbital_strike = "xs_freighter_flyby";
    abilities.overload_shot = "quick_shot";
    abilities.rifle_shot = "flurry_of_bolts";
    abilities.shiv = "blaster_whip";
  }
}

// class_t::init_talents =======================================

void class_t::init_talents()
{
  base_t::init_talents();

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

// class_t::init_buffs ===========================================

void class_t::init_buffs()
{
  base_t::init_buffs();

  buffs.adrenaline_probe   = new buff_t( this , abilities.adrenaline_probe   , 1 ,  from_seconds(  3 ) );
}

// class_t::init_gains ========================================

void class_t::init_gains()
{
  base_t::init_gains();

  energy_gains.minimum = get_gain( "min"  );
  energy_gains.low     = get_gain( "low"  );
  energy_gains.medium  = get_gain( "med"  );
  energy_gains.high    = get_gain( "high" );

  gains.adrenaline_probe = get_gain( abilities.adrenaline_probe );
}

// class_t::create_talents =======================================

void class_t::create_talents()
{
  // Lethality|Dirty Fighting
  static const talentinfo_t lethality_tree[] = {
    { "Deadly Directive", 2 }, { "Lethality", 3 }, { "Razor Edge", 2 },
    { "Slip Away", 2 }, { "Flash Powder", 2 }, { "Corrosive Microbes", 2 }, { "Lethal Injectors", 1 },
    { "Corrosive Grenade", 1 }, { "Combat Stims", 2 }, { "Cut Down", 2 },
    { "Lethal Purpose", 2 }, { "Adhesive Corrosives", 2 }, { "Escape Plan", 2 }, { "Lethal Dose", 3 },
    { "Cull", 1 }, { "License to Kill", 2 }, { "Counterstrike", 2 },
    { "Devouring Microbes", 3 }, { "Lingering Toxins", 2 }, { "Weakening Blast", 1 },
  };
  init_talent_tree( IA_LETHALITY, lethality_tree );
}

} // namespace agent_smug ===================================================
