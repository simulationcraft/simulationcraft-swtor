// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// ==========================================================================
// Weapon
// ==========================================================================

// weapon_t::group ==========================================================

int weapon_t::group() const
{
  switch ( type )
  {
  case WEAPON_DAGGER:
  case WEAPON_SMALL:
    return WEAPON_SMALL;

  case WEAPON_SWORD:
  case WEAPON_MACE:
  case WEAPON_AXE:
  case WEAPON_FIST:
  case WEAPON_1H:
    return WEAPON_1H;

  case WEAPON_SWORD_2H:
  case WEAPON_MACE_2H:
  case WEAPON_AXE_2H:
  case WEAPON_STAFF:
  case WEAPON_POLEARM:
  case WEAPON_2H:
    return WEAPON_2H;

  case WEAPON_BOW:
  case WEAPON_CROSSBOW:
  case WEAPON_GUN:
  case WEAPON_WAND:
  case WEAPON_THROWN:
  case WEAPON_RANGED:
    return WEAPON_RANGED;
  }
  return WEAPON_NONE;
}
