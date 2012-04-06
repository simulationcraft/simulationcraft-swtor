// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// ==========================================================================
// Weapon
// ==========================================================================

// weapon_t::group ==========================================================

int weapon_t::group() const
{
  switch ( type )
  {

  case WEAPON_LIGHTSABER:
  case WEAPON_TRAININGSABER:
  case WEAPON_VIBROKNIFE:
  case WEAPON_TECHBLADE:
  case WEAPON_VIBROSWORD:
    return WEAPON_1H;

  case WEAPON_DOUBLE_BLADED_LIGHTSABER:
  case WEAPON_ELECTROSTAFF:
  case WEAPON_TECHSTAFF:
    return WEAPON_2H;

  case WEAPON_ASSAULT_CANNON:
  case WEAPON_BLASTER_PISTOL:
  case WEAPON_BLASTER_RIFLE:
  case WEAPON_SCATTERGUN:
  case WEAPON_SNIPER_RIFLE:
    return WEAPON_RANGED;
  }
  return WEAPON_NONE;
}
