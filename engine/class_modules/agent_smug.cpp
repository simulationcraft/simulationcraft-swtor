// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "agent_smug.hpp"

namespace agent_smug { // ===================================================

void class_t::init_talents(talents_t& talents)
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
