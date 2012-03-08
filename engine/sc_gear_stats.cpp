// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// ==========================================================================
// Gear Stats
// ==========================================================================

// gear_stats_t::add_stat ===================================================

void gear_stats_t::add_stat( int    stat,
                             double value )
{
  switch ( stat )
  {
  case STAT_NONE: break;

  case STAT_STRENGTH:  attribute[ ATTR_STRENGTH  ] += value; break;
  case STAT_AIM:       attribute[ ATTR_AIM       ] += value; break;
  case STAT_CUNNING:   attribute[ ATTR_CUNNING   ] += value; break;
  case STAT_WILLPOWER: attribute[ ATTR_WILLPOWER ] += value; break;
  case STAT_ENDURANCE: attribute[ ATTR_ENDURANCE ] += value; break;
  case STAT_PRESENCE:  attribute[ ATTR_PRESENCE  ] += value; break;

  case STAT_HEALTH: resource[ RESOURCE_HEALTH ] += value; break;
  case STAT_MANA:   resource[ RESOURCE_MANA   ] += value; break;
  case STAT_RAGE:   resource[ RESOURCE_RAGE   ] += value; break;
  case STAT_ENERGY: resource[ RESOURCE_ENERGY ] += value; break;
  case STAT_AMMO:   resource[ RESOURCE_AMMO   ] += value; break;

  case STAT_MAX_HEALTH:
  case STAT_MAX_MANA:
  case STAT_MAX_RAGE:
  case STAT_MAX_ENERGY:
  case STAT_MAX_AMMO:
    break;

  case STAT_EXPERTISE_RATING:  expertise_rating  += value; break;
  case STAT_EXPERTISE_RATING2: expertise_rating2 += value; break;

  case STAT_ACCURACY_RATING:      accuracy_rating      += value; break;
  case STAT_ACCURACY_RATING2:     accuracy_rating2     += value; break;
  case STAT_CRIT_RATING:     crit_rating     += value; break;
  case STAT_ALACRITY_RATING: alacrity_rating += value; break;
  case STAT_SURGE_RATING:    surge_rating    += value; break;

  case STAT_WEAPON_DMG:           weapon_dmg           += value; break;
  case STAT_WEAPON_OFFHAND_DMG:   weapon_offhand_dmg   += value; break;

  case STAT_ARMOR:       armor       += value; break;
  case STAT_BONUS_ARMOR: bonus_armor += value; break;

  case STAT_POWER:       power       += value; break;
  case STAT_FORCE_POWER: force_power += value; break;
  case STAT_TECH_POWER:  tech_power  += value; break;

  case STAT_DEFENSE_RATING: defense_rating += value; break;
  case STAT_SHIELD_RATING:  shield_rating  += value; break;
  case STAT_ABSORB_RATING:  absorb_rating  += value; break;

  case STAT_MAX: for ( int i=0; i < ATTRIBUTE_MAX; i++ ) { attribute[ i ] += value; }
    break;

  default: assert( 0 );
  }
}

// gear_stats_t::set_stat ===================================================

void gear_stats_t::set_stat( int    stat,
                             double value )
{
  switch ( stat )
  {
  case STAT_NONE: break;

  case STAT_STRENGTH:  attribute[ ATTR_STRENGTH  ] = value; break;
  case STAT_AIM:       attribute[ ATTR_AIM       ] = value; break;
  case STAT_CUNNING:   attribute[ ATTR_CUNNING   ] = value; break;
  case STAT_WILLPOWER: attribute[ ATTR_WILLPOWER ] = value; break;
  case STAT_ENDURANCE: attribute[ ATTR_ENDURANCE ] = value; break;
  case STAT_PRESENCE:  attribute[ ATTR_PRESENCE ] = value; break;

  case STAT_HEALTH: resource[ RESOURCE_HEALTH ] = value; break;
  case STAT_MANA:   resource[ RESOURCE_MANA   ] = value; break;
  case STAT_RAGE:   resource[ RESOURCE_RAGE   ] = value; break;
  case STAT_ENERGY: resource[ RESOURCE_ENERGY ] = value; break;
  case STAT_AMMO:   resource[ RESOURCE_AMMO   ] = value; break;

  case STAT_MAX_HEALTH:
  case STAT_MAX_MANA:
  case STAT_MAX_RAGE:
  case STAT_MAX_ENERGY:
  case STAT_MAX_AMMO:
    break;

  case STAT_EXPERTISE_RATING:  expertise_rating  = value; break;
  case STAT_EXPERTISE_RATING2: expertise_rating2 = value; break;

  case STAT_ACCURACY_RATING:      accuracy_rating      = value; break;
  case STAT_ACCURACY_RATING2:     accuracy_rating2     = value; break;
  case STAT_CRIT_RATING:     crit_rating     = value; break;
  case STAT_ALACRITY_RATING: alacrity_rating = value; break;
  case STAT_SURGE_RATING:    surge_rating    = value; break;

  case STAT_WEAPON_DMG:         weapon_dmg          = value; break;
  case STAT_WEAPON_OFFHAND_DMG: weapon_offhand_dmg  = value; break;

  case STAT_ARMOR:       armor       = value; break;
  case STAT_BONUS_ARMOR: bonus_armor = value; break;

  case STAT_POWER:       power       = value; break;
  case STAT_FORCE_POWER: force_power = value; break;
  case STAT_TECH_POWER:  tech_power  = value; break;

  case STAT_DEFENSE_RATING: defense_rating = value; break;
  case STAT_SHIELD_RATING:  shield_rating  = value; break;
  case STAT_ABSORB_RATING:  absorb_rating  = value; break;

  case STAT_MAX: for ( int i=0; i < ATTRIBUTE_MAX; i++ ) { attribute[ i ] = value; }
    break;

  default: assert( 0 );
  }
}

// gear_stats_t::get_stat ===================================================

double gear_stats_t::get_stat( int stat ) const
{
  switch ( stat )
  {
  case STAT_NONE: return 0;

  case STAT_STRENGTH:  return attribute[ ATTR_STRENGTH  ];
  case STAT_AIM:       return attribute[ ATTR_AIM       ];
  case STAT_CUNNING:   return attribute[ ATTR_CUNNING   ];
  case STAT_WILLPOWER: return attribute[ ATTR_WILLPOWER ];
  case STAT_ENDURANCE: return attribute[ ATTR_ENDURANCE ];
  case STAT_PRESENCE:  return attribute[ ATTR_PRESENCE  ];

  case STAT_HEALTH: return resource[ RESOURCE_HEALTH ];
  case STAT_MANA:   return resource[ RESOURCE_MANA   ];
  case STAT_RAGE:   return resource[ RESOURCE_RAGE   ];
  case STAT_ENERGY: return resource[ RESOURCE_ENERGY ];
  case STAT_AMMO:   return resource[ RESOURCE_AMMO   ];

  case STAT_MAX_HEALTH:
  case STAT_MAX_MANA:
  case STAT_MAX_RAGE:
  case STAT_MAX_ENERGY:
  case STAT_MAX_AMMO:
    return 0;

  case STAT_EXPERTISE_RATING:  return expertise_rating;
  case STAT_EXPERTISE_RATING2: return expertise_rating2;

  case STAT_ACCURACY_RATING:  return accuracy_rating;
  case STAT_ACCURACY_RATING2: return accuracy_rating2;
  case STAT_CRIT_RATING:      return crit_rating;
  case STAT_ALACRITY_RATING:  return alacrity_rating;
  case STAT_SURGE_RATING:     return surge_rating;

  case STAT_WEAPON_DMG:         return weapon_dmg;
  case STAT_WEAPON_OFFHAND_DMG: return weapon_offhand_dmg;

  case STAT_ARMOR:       return armor;
  case STAT_BONUS_ARMOR: return bonus_armor;

  case STAT_POWER:       return power;
  case STAT_FORCE_POWER: return force_power;
  case STAT_TECH_POWER:  return tech_power;

  case STAT_DEFENSE_RATING: return defense_rating;
  case STAT_SHIELD_RATING:  return shield_rating;
  case STAT_ABSORB_RATING:  return absorb_rating;

  default: assert( 0 );
  }
  return 0;
}

// gear_stats_t::print ======================================================

void gear_stats_t::print( FILE* file )
{
  for ( int i=0; i < STAT_MAX; i++ )
  {
    double value = get_stat( i );

    if ( value != 0 )
    {
      util_t::fprintf( file, " %s=%.*f", util_t::stat_type_abbrev( i ), ( ( ( value - ( int ) value ) > 0 ) ? 3 : 0 ), value );
    }
  }
  util_t::fprintf( file, "\n" );
}

// gear_stats_t::stat_mod ===================================================

double gear_stats_t::stat_mod( int stat )
{
  switch ( stat )
  {
  }
  return 1.0;
}
