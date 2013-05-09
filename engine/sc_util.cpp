// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"
#include "utf8.h"

namespace { // ANONYMOUS ====================================================

// pred_ci ==================================================================

bool pred_ci ( char a, char b )
{
  return std::tolower( a ) == std::tolower( b );
}

} // ANONYMOUS namespace ====================================================

// util_t::str_compare_ci ===================================================

bool util_t::str_compare_ci( const std::string& l,
                             const std::string& r )
{
  if ( l.size() != r.size() || l.size() == 0 )
    return false;

  return std::equal( l.begin(), l.end(), r.begin(), pred_ci );
}

// util_t::str_prefix_ci ====================================================

bool util_t::str_prefix_ci( const std::string& str,
                            const std::string& prefix )
{
  if ( str.size() < prefix.size() )
    return false;

  return std::equal( prefix.begin(), prefix.end(), str.begin(), pred_ci );
}

// util_t::str_in_str_ci ====================================================

bool util_t::str_in_str_ci( const std::string& l,
                            const std::string& r )
{
  return std::search( l.begin(), l.end(), r.begin(), r.end(), pred_ci ) != l.end();
}

// util_t::talent_rank ======================================================

double util_t::talent_rank( int    num,
                            int    max,
                            double increment )
{
  assert( num >= 0 );
  assert( max > 0 );

  if ( num > max ) num = max;

  return num * increment;
}

// util_t::talent_rank ======================================================

int util_t::talent_rank( int num,
                         int max,
                         int increment )
{
  assert( num >= 0 );
  assert( max > 0 );

  if ( num > max ) num = max;

  return num * increment;
}

// util_t::talent_rank ======================================================

double util_t::talent_rank( int    num,
                            int    max,
                            double first,
                            double second, ... )
{
  assert( num >= 0 );
  assert( max > 0 );

  if ( num > max ) num = max;

  if ( num == 1 ) return first;
  if ( num == 2 ) return second;

  va_list vap;
  va_start( vap, second );

  double value=0;

  for ( int i=3; i <= num; i++ )
  {
    value = ( double ) va_arg( vap, double );
  }
  va_end( vap );

  return value;
}

// util_t::talent_rank ======================================================

int util_t::talent_rank( int num,
                         int max,
                         int first,
                         int second, ... )
{
  assert( num >= 0 );
  assert( max > 0 );

  if ( num > max ) num = max;

  if ( num == 1 ) return first;
  if ( num == 2 ) return second;

  va_list vap;
  va_start( vap, second );

  int value=0;

  for ( int i=3; i <= num; i++ )
  {
    value = ( int ) va_arg( vap, int );
  }
  va_end( vap );

  return value;
}

// util_t::ability_rank =====================================================

double util_t::ability_rank( int    player_level,
                             double ability_value,
                             int    ability_level, ... )
{
  va_list vap;
  va_start( vap, ability_level );

  while ( player_level < ability_level )
  {
    ability_value = ( double ) va_arg( vap, double );
    ability_level = ( int ) va_arg( vap, int );
  }

  va_end( vap );

  return ability_value;
}

// util_t::ability_rank =====================================================

int util_t::ability_rank( int player_level,
                          int ability_value,
                          int ability_level, ... )
{
  va_list vap;
  va_start( vap, ability_level );

  while ( player_level < ability_level )
  {
    ability_value = va_arg( vap, int );
    ability_level = va_arg( vap, int );
  }

  va_end( vap );

  return ability_value;
}

#ifdef _MSC_VER
// vsnprintf ================================================================

int vsnprintf_simc( char* buf, size_t size, const char* fmt, va_list ap )
{
  if ( buf && size )
  {
    int rval = _vsnprintf( buf, size, fmt, ap );
    if ( rval < 0 || static_cast<size_t>( rval ) < size )
      return rval;

    buf[ size - 1 ] = '\0';
  }

  return _vscprintf( fmt, ap );
}
#endif

// util_t::role_type_string =================================================

const char* util_t::role_type_string( role_type role )
{
  switch ( role )
  {
  case ROLE_ATTACK: return "attack";
  case ROLE_SPELL:  return "spell";
  case ROLE_HYBRID: return "hybrid";
  case ROLE_DPS:    return "dps";
  case ROLE_TANK:   return "tank";
  case ROLE_HEAL:   return "heal";
  case ROLE_NONE:   return "auto";
  default:          return "unknown";
  }
}

// util_t::parse_role_type ==================================================

role_type util_t::parse_role_type( const std::string& name )
{
  for ( role_type i = ROLE_NONE; i < ROLE_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::role_type_string( i ) ) )
      return i;

  return ROLE_HYBRID;
}

// util_t::race_type_string =================================================

const char* util_t::race_type_string( race_type type )
{
  switch ( type )
  {
  case RACE_NONE:           return "none";
  case RACE_CHISS:          return "chiss";
  case RACE_CYBORG:         return "cyborg";
  case RACE_HUMAN:          return "human";
  case RACE_MIRALUKA:       return "miraluka";
  case RACE_MIRIALAN:       return "mirialan";
  case RACE_RATTATAKI:      return "rattataki";
  case RACE_SITH_PUREBLOOD: return "sith_pureblood";
  case RACE_TWILEK:         return "twilek";
  case RACE_ZABRAK:         return "zabrak";
  default:                  return "unknown";
  }
}

// util_t::parse_race_type ==================================================

race_type util_t::parse_race_type( const std::string& name )
{
  for ( race_type i = RACE_NONE; i < RACE_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::race_type_string( i ) ) )
      return i;

  return RACE_NONE;
}

// util_t::parse_position_type ==============================================

position_type util_t::parse_position_type( const std::string& name )
{
  for ( position_type i = POSITION_NONE; i < POSITION_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::position_type_string( i ) ) )
      return i;

  return POSITION_NONE;
}

// util_t::position_type_string =============================================

const char* util_t::position_type_string( position_type type )
{
  switch ( type )
  {
  case POSITION_NONE:         return "none";
  case POSITION_BACK:         return "back";
  case POSITION_FRONT:        return "front";
  case POSITION_RANGED_BACK:  return "ranged_back";
  case POSITION_RANGED_FRONT: return "ranged_front";
  default:                    return "unknown";
  }
}

// util_t::profession_type_string ===========================================

const char* util_t::profession_type_string( profession_type type )
{
  switch ( type )
  {
  case PROFESSION_NONE:          return "none";
  case PROFESSION_ARMORMECH:     return "armormech";
  case PROFESSION_ARMSTECH:      return "armstech";
  case PROFESSION_ARTIFICE:      return "artifice";
  case PROFESSION_BIOCHEM:       return "biochem";
  case PROFESSION_CYBERTECH:     return "cybertech";
  case PROFESSION_SYNTHWEAVING:  return "synthweaving";
  default:                       return "unknown";
  }
}

// util_t::parse_profession_type ============================================

profession_type util_t::parse_profession_type( const std::string& name )
{
  for ( profession_type i = PROFESSION_NONE; i < PROFESSION_MAX; ++i )
    if ( util_t::str_compare_ci( name, util_t::profession_type_string( i ) ) )
      return i;

  return PROFESSION_UNKNOWN;
}

// util_t::player_type_string ===============================================

const char* util_t::player_type_string( player_type type )
{
  switch ( type )
  {
  case JEDI_SAGE:        return "jedi_sage";
  case JEDI_SHADOW:      return "jedi_shadow";
  case JEDI_SENTINEL:    return "jedi_sentinel";
  case JEDI_GUARDIAN:    return "jedi_guardian";
  case S_SCOUNDREL:      return "scoundrel";
  case S_GUNSLINGER:     return "gunslinger";
  case T_COMMANDO:       return "commando";
  case T_VANGUARD:       return "vanguard";

  case SITH_SORCERER:    return "sith_sorcerer";
  case SITH_ASSASSIN:    return "sith_assassin";
  case SITH_MARAUDER:    return "sith_marauder";
  case SITH_JUGGERNAUT:  return "sith_juggernaut";
  case BH_MERCENARY:     return "mercenary";
  case BH_POWERTECH:     return "powertech";
  case IA_SNIPER:        return "sniper";
  case IA_OPERATIVE:     return "operative";

  case PLAYER_NONE:      return "none";
  case PLAYER_PET:       return "pet";
  case PLAYER_GUARDIAN:  return "guardian";
  case PLAYER_COMPANION: return "companion";
  case ENEMY:            return "enemy";
  case ENEMY_ADD:        return "add";
  default:               return "unknown";
  }
}

// util_t::player_type_string_short =========================================

const char* util_t::player_type_string_short( player_type type )
{
  switch ( type )
  {
  case JEDI_SAGE:        return "sage";
  case JEDI_SHADOW:      return "shadow";
  case JEDI_SENTINEL:    return "sentinel";
  case JEDI_GUARDIAN:    return "guardian";
  case SITH_SORCERER:    return "sorcerer";
  case SITH_ASSASSIN:    return "assassin";
  case SITH_MARAUDER:    return "marauder";
  case SITH_JUGGERNAUT:  return "juggernaut";
  default:               return util_t::player_type_string( type );
  }
}

// util_t::translate_class_str ==============================================

player_type util_t::translate_class_str( const std::string& s )
{
  std::string fmt_s( s );
  util_t::format_name( fmt_s );

  if      ( fmt_s == "jedi_sage"       || fmt_s == "sage"       ) return JEDI_SAGE;
  else if ( fmt_s == "jedi_shadow"     || fmt_s == "shadow"     ) return JEDI_SHADOW;
  else if ( fmt_s == "jedi_sentinel"   || fmt_s == "sentinel"   ) return JEDI_SENTINEL;
  else if ( fmt_s == "jedi_guardian"   || fmt_s == "guardian"   ) return JEDI_GUARDIAN;
  else if ( fmt_s == "commando"                                 ) return T_COMMANDO;
  else if ( fmt_s == "vanguard"                                 ) return T_VANGUARD;
  else if ( fmt_s == "scoundrel"                                ) return S_SCOUNDREL;
  else if ( fmt_s == "gunslinger"                               ) return S_GUNSLINGER;

  else if ( fmt_s == "sith_sorcerer"   || fmt_s == "sorcerer"   ) return SITH_SORCERER;
  else if ( fmt_s == "sith_assassin"   || fmt_s == "assassin"   ) return SITH_ASSASSIN;
  else if ( fmt_s == "sith_marauder"   || fmt_s == "marauder"   ) return SITH_MARAUDER;
  else if ( fmt_s == "sith_juggernaut" || fmt_s == "juggernaut" ) return SITH_JUGGERNAUT;
  else if ( fmt_s == "mercenary"                                ) return BH_MERCENARY;
  else if ( fmt_s == "powertech"                                ) return BH_POWERTECH;
  else if ( fmt_s == "operative"                                ) return IA_OPERATIVE;
  else if ( fmt_s == "sniper"                                   ) return IA_SNIPER;

  else                                                            return PLAYER_NONE;
}

// util_t::parse_player_type ================================================

player_type util_t::parse_player_type( const std::string& name )
{
  for ( player_type i = PLAYER_NONE; i < PLAYER_MAX; ++i )
    if ( util_t::str_compare_ci( name, util_t::player_type_string( i ) ) )
      return i;

  return PLAYER_NONE;
}

// util_t::pet_type_string ==================================================

const char* util_t::pet_type_string( pet_type type )
{
  switch ( type )
  {
  case PET_NONE:  return "none";
  case PET_ENEMY: return "pet_enemy";
  default:        return "unknown";
  }
}

// util_t::parse_pet_type ===================================================

pet_type util_t::parse_pet_type( const std::string& name )
{
  for ( pet_type i= PET_NONE; i < PET_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::pet_type_string( i ) ) )
      return i;

  return PET_NONE;
}

// util_t::attribute_type_string ============================================

const char* util_t::attribute_type_string( attribute_type type )
{
  switch ( type )
  {
  case ATTR_STRENGTH:  return "strength";
  case ATTR_AIM:       return "aim";
  case ATTR_CUNNING:   return "cunning";
  case ATTR_WILLPOWER: return "willpower";
  case ATTR_ENDURANCE: return "endurance";
  case ATTR_PRESENCE:  return "presence";
  default:             return "unknown";
  }
}

// util_t::parse_attribute_type =============================================

attribute_type util_t::parse_attribute_type( const std::string& name )
{
  for ( attribute_type i = ATTRIBUTE_NONE; i < ATTRIBUTE_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::attribute_type_string( i ) ) )
      return i;

  return ATTRIBUTE_NONE;
}

// util_t::dmg_type_string ==================================================

const char* util_t::dmg_type_string( dmg_type type )
{
  switch ( type )
  {
  case ABSORB:         return "absorb";
  case DMG_DIRECT:     return "hit";
  case DMG_OVER_TIME:  return "tick";
  case HEAL_DIRECT:    return "heal";
  case HEAL_OVER_TIME: return "hot";
  default:             return "unknown";
  }
}

// util_t::result_type_string ===============================================

const char* util_t::result_type_string( result_type type )
{
  switch ( type )
  {
  case RESULT_NONE:  return "none";
  case RESULT_MISS:  return "miss";
  case RESULT_AVOID: return "avoid";
  case RESULT_BLOCK: return "block";
  case RESULT_CRIT:  return "crit";
  case RESULT_HIT:   return "hit";
  default:           return "unknown";
  }
}

// util_t::parse_result_type ================================================

result_type util_t::parse_result_type( const std::string& name )
{
  for ( result_type i = RESULT_NONE; i < RESULT_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::result_type_string( i ) ) )
      return i;

  return RESULT_NONE;
}

// util_t::resource_type_string =============================================

const char* util_t::resource_type_string( resource_type type )
{
  switch ( type )
  {
  case RESOURCE_NONE:   return "none";
  case RESOURCE_HEALTH: return "health";
  case RESOURCE_MANA:   return "mana";
  case RESOURCE_RAGE:   return "rage";
  case RESOURCE_ENERGY: return "energy";
  case RESOURCE_HEAT:   return "heat";
  case RESOURCE_AMMO:   return "ammo";
  case RESOURCE_FORCE:  return "force";
  default:              return "unknown";
  }
}

// util_t::parse_resource_type ==============================================

resource_type util_t::parse_resource_type( const std::string& name )
{
  for ( resource_type i=RESOURCE_NONE; i < RESOURCE_MAX; ++i )
    if ( util_t::str_compare_ci( name, util_t::resource_type_string( i ) ) )
      return i;

  return RESOURCE_NONE;
}

// util_t::school_type_string ===============================================

const char* util_t::school_type_string( school_type school )
{
  switch ( school )
  {
  case SCHOOL_KINETIC:   return "kinetic";
  case SCHOOL_INTERNAL:  return "internal";
  case SCHOOL_ELEMENTAL: return "elemental";
  case SCHOOL_ENERGY:    return "energy";
  default:               return "unknown";
  }
}

// util_t::parse_school_type ================================================

school_type util_t::parse_school_type( const std::string& name )
{
  for ( school_type i = SCHOOL_NONE; i < SCHOOL_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::school_type_string( i ) ) )
      return i;

  return SCHOOL_NONE;
}

// util_t::talent_tree ======================================================

talent_tree_type util_t::talent_tree( talent_tab_type tree, player_type ptype )
{
  switch ( ptype )
  {
    case JEDI_SAGE:
      switch ( tree )
      {
      case JEDI_SAGE_SEER:           return TREE_SEER;
      case JEDI_SAGE_TELEKINETICS:   return TREE_TELEKINETICS;
      case JEDI_SAGE_BALANCE:        return TREE_BALANCE;
      default: break;
      }
    break;
    case SITH_SORCERER:
      switch ( tree )
      {
      case SITH_SORCERER_CORRUPTION: return TREE_CORRUPTION;
      case SITH_SORCERER_LIGHTNING:  return TREE_LIGHTNING;
      case SITH_SORCERER_MADNESS:    return TREE_MADNESS;
      default: break;
      }
    break;
    case IA_OPERATIVE:
      switch ( tree )
      {
      case IA_OPERATIVE_MEDICINE:     return TREE_MEDICINE;
      case IA_OPERATIVE_CONCEALMENT:  return TREE_CONCEALMENT;
      case IA_LETHALITY:              return TREE_LETHALITY;
      default: break;
      }
    break;
    case IA_SNIPER:
      switch ( tree )
      {
      case IA_SNIPER_MARKSMANSHIP:    return TREE_MARKSMANSHIP;
      case IA_SNIPER_ENGINEERING:     return TREE_ENGINEERING;
      case IA_LETHALITY:              return TREE_LETHALITY;
      default: break;
      }
    break;
    case BH_MERCENARY:
      switch ( tree )
      {
      case BH_MERCENARY_BODYGUARD:    return TREE_BODYGUARD;
      case BH_MERCENARY_ARSENAL:      return TREE_ARSENAL;
      case BH_MERCENARY_PYROTECH:     return TREE_PYROTECH;
      default: break;
      }
    break;
    default:
      break;
  }

  return TREE_NONE;
}

// util_t::talent_tree_string ===============================================

const char* util_t::talent_tree_string( talent_tree_type tree, bool armory_format )
{
  if ( armory_format )
  {
    switch ( tree )
    {
    // JEDI_SAGE
    case TREE_SEER:           return "seer";
    case TREE_TELEKINETICS:   return "telekinetics";
    case TREE_BALANCE:        return "balance";
    // SITH_SORCERER
    case TREE_CORRUPTION:     return "corruption";
    case TREE_LIGHTNING:      return "lightning";
    case TREE_MADNESS:        return "madness";
    // JEDI_SHADOW
    case TREE_KINETIC_COMBAT: return "kinetic_combat";
    case TREE_INFILTRATION:   return "infiltration";
    // SITH_ASSASSIN
    case TREE_DARKNESS:       return "darkness";
    case TREE_DECEPTION:      return "deception";
    // IA
    case TREE_LETHALITY:      return "lethality";
    // IA_OPERATIVE
    case TREE_MEDICINE:       return "medicine";
    case TREE_CONCEALMENT:    return "concealment";
    // IA_SNIPER
    case TREE_MARKSMANSHIP:   return "marksmanship";
    case TREE_ENGINEERING:    return "engineering";
    // BH_MERCENARY
    case TREE_BODYGUARD:      return "bodyguard";
    case TREE_ARSENAL:        return "arsenal";
    case TREE_PYROTECH:       return "Pyrotech";
    default:                  return "unknown";
    }
  }
  else
  {
    switch ( tree )
    {
    // JEDI_SAGE
    case TREE_SEER:           return "Seer";
    case TREE_TELEKINETICS:   return "Telekinetics";
    case TREE_BALANCE:        return "Balance";
    // SITH_SORCERER
    case TREE_CORRUPTION:     return "Corruption";
    case TREE_LIGHTNING:      return "Lightning";
    case TREE_MADNESS:        return "Madness";
    // JEDI_SHADOW
    case TREE_KINETIC_COMBAT: return "KineticCombat";
    case TREE_INFILTRATION:   return "Infiltration";
    // SITH_ASSASSIN
    case TREE_DARKNESS:       return "Darkness";
    case TREE_DECEPTION:      return "Deception";
    // IA
    case TREE_LETHALITY:      return "Lethality";
    // IA_OPERATIVE
    case TREE_MEDICINE:       return "Medicine";
    case TREE_CONCEALMENT:    return "Concealment";
    // IA_SNIPER
    case TREE_MARKSMANSHIP:   return "Marksmanship";
    case TREE_ENGINEERING:    return "Engineering";
    // BH_MERCENARY
    case TREE_BODYGUARD:      return "Bodyguard";
    case TREE_ARSENAL:        return "Arsenal";
    case TREE_PYROTECH:       return "Pyrotech";
    default:                  return "Unknown";
    }
  }
}

// util_t::parse_talent_tree ================================================

talent_tree_type util_t::parse_talent_tree( const std::string& name )
{
  for ( talent_tree_type i = TREE_NONE; i < TALENT_TREE_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::talent_tree_string( i ) ) )
      return i;

  return TREE_NONE;
}

// util_t::weapon_type_string ===============================================

const char* util_t::weapon_type_string( weapon_type weapon )
{
  switch ( weapon )
  {
  case WEAPON_LIGHTSABER:               return "lightsaber";
  case WEAPON_TRAININGSABER:            return "trainingsaber";
  case WEAPON_VIBROKNIFE:               return "vibroknife";
  case WEAPON_TECHBLADE:                return "techblade";
  case WEAPON_VIBROSWORD:               return "vibrosword";
  case WEAPON_DOUBLE_BLADED_LIGHTSABER: return "doublebladedlightsaber";
  case WEAPON_ELECTROSTAFF:             return "electrostaff";
  case WEAPON_TECHSTAFF:                return "techstaff";
  case WEAPON_ASSAULT_CANNON:           return "assaultcanon";
  case WEAPON_BLASTER_PISTOL:           return "blasterpistol";
  case WEAPON_BLASTER_RIFLE:            return "blasterrifle";
  case WEAPON_SCATTERGUN:               return "scattergun";
  case WEAPON_SNIPER_RIFLE:             return "sniperrifle";
  default:                              return "unknown";
  }
}

// util_t::parse_weapon_type ================================================

weapon_type util_t::parse_weapon_type( const std::string& name )
{
  for ( weapon_type i = WEAPON_NONE; i < WEAPON_MAX; ++i )
    if ( util_t::str_compare_ci( name, util_t::weapon_type_string( i ) ) )
      return i;

  return WEAPON_NONE;
}

// util_t::stim_type_string =================================================

const char* util_t::stim_type_string( stim_type stim )
{
  switch ( stim )
  {
  case stim_type::none:            return "none";
  case stim_type::prototype_nano_infused_resolve: return "prototype_nano_infused_resolve";
  case stim_type::exotech_resolve: return "exotech_resolve";
  case stim_type::rakata_resolve:  return "rakata_resolve";
  case stim_type::exotech_skill:   return "exotech_skill";
  case stim_type::rakata_skill:    return "rakata_skill";
  case stim_type::exotech_reflex:  return "exotech_reflex";
  case stim_type::rakata_reflex:   return "rakata_reflex";
  case stim_type::exotech_might:   return "exotech_might";
  case stim_type::rakata_might:    return "rakata_might";
  default:                         return "unknown";
  }
}

// util_t::parse_stim_t =================================================

stim_type util_t::parse_stim_type( const std::string& name )
{
  for( stim_type i = stim_type::none; ++i < stim_type::max; )
    if ( util_t::str_compare_ci( name, util_t::stim_type_string( i ) ) )
      return i;

  return stim_type::none;
}

// util_t::slot_type_string ================================================

const char* util_t::slot_type_string( slot_type slot )
{
  switch ( slot )
  {
  case SLOT_HEAD:      return "head";
  case SLOT_EAR:       return "ear";
  case SLOT_SHOULDERS: return "shoulders";
  case SLOT_SHIRT:     return "shirt";
  case SLOT_CHEST:     return "chest";
  case SLOT_WAIST:     return "waist";
  case SLOT_LEGS:      return "legs";
  case SLOT_FEET:      return "feet";
  case SLOT_WRISTS:    return "wrists";
  case SLOT_HANDS:     return "hands";
  case SLOT_IMPLANT_1: return "implant1";
  case SLOT_IMPLANT_2: return "implant2";
  case SLOT_RELIC_1:   return "relic1";
  case SLOT_RELIC_2:   return "relic2";
  case SLOT_BACK:      return "back";
  case SLOT_MAIN_HAND: return "main_hand";
  case SLOT_OFF_HAND:  return "off_hand";
  case SLOT_RANGED:    return "ranged";
  case SLOT_TABARD:    return "tabard";
  default:             return "unknown";
  }
}

// util_t::parse_slot_type ==================================================

slot_type util_t::parse_slot_type( const std::string& name )
{
  for ( slot_type i = SLOT_MIN; i < SLOT_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::slot_type_string( i ) ) )
      return i;

  return SLOT_INVALID;
}

// util_t::stat_type_string =================================================

const char* util_t::stat_type_string( stat_type stat )
{
  switch ( stat )
  {
  case STAT_STRENGTH:  return "strength";
  case STAT_AIM:       return "aim";
  case STAT_CUNNING:   return "cunning";
  case STAT_ENDURANCE: return "endurance";
  case STAT_WILLPOWER: return "willpower";
  case STAT_PRESENCE:  return "presence";

  case STAT_HEALTH: return "health";
  case STAT_MANA:   return "mana";
  case STAT_RAGE:   return "rage";
  case STAT_ENERGY: return "energy";
  case STAT_AMMO:  return "ammo";
  case STAT_MAX_HEALTH: return "maximum_health";
  case STAT_MAX_MANA: return "maximum_mana";
  case STAT_MAX_RAGE:   return "maximum_rage";
  case STAT_MAX_ENERGY: return "maximum_energy";
  case STAT_MAX_AMMO:  return "maximum_ammo";

  case STAT_EXPERTISE_RATING:         return "expertise";

  case STAT_ACCURACY_RATING:   return "accuracy_rating";
  case STAT_ACCURACY_RATING2:  return "inverse_accuracy_rating";
  case STAT_CRIT_RATING:  return "crit_rating";
  case STAT_ALACRITY_RATING: return "alacrity_rating";
  case STAT_SURGE_RATING: return "surge_rating";

  case STAT_WEAPON_DMG:   return "weapon_dmg";
  case STAT_WEAPON_OFFHAND_DMG:    return "weapon_offhand_dmg";

  case STAT_ARMOR:             return "armor";
  case STAT_BONUS_ARMOR:       return "bonus_armor";
  case STAT_DEFENSE_RATING:    return "defense_rating";
  case STAT_SHIELD_RATING:     return "shield_rating";
  case STAT_ABSORB_RATING:     return "absorb_rating";

  case STAT_POWER: return "power";
  case STAT_FORCE_POWER: return "forcepower";
  case STAT_TECH_POWER: return "techpower";

  case STAT_ALL: return "all";

  default: return "unknown";
  }
}

// util_t::stat_type_abbrev =================================================

const char* util_t::stat_type_abbrev( stat_type stat )
{
  switch ( stat )
  {
  case STAT_STRENGTH:  return "Str";
  case STAT_AIM:       return "Aim";
  case STAT_CUNNING:   return "Cun";
  case STAT_WILLPOWER: return "Wil";
  case STAT_ENDURANCE: return "End";
  case STAT_PRESENCE:  return "Pre";

  case STAT_HEALTH: return "Health";
  case STAT_MANA:   return "Mana";
  case STAT_RAGE:   return "Rage";
  case STAT_ENERGY: return "Energy";
  case STAT_AMMO:   return "Ammo";
  case STAT_MAX_HEALTH: return "MaxHealth";
  case STAT_MAX_MANA:   return "MaxMana";
  case STAT_MAX_RAGE:   return "MaxRage";
  case STAT_MAX_ENERGY: return "MaxEnergy";
  case STAT_MAX_AMMO:   return "MaxAmmo";

  case STAT_EXPERTISE_RATING:  return "Exp";

  case STAT_ACCURACY_RATING:      return "Acc";
  case STAT_ACCURACY_RATING2:     return "InvAcc";
  case STAT_CRIT_RATING:     return "Crit";
  case STAT_ALACRITY_RATING: return "Alacrity";
  case STAT_SURGE_RATING:    return "Surge";

  case STAT_WEAPON_DMG:   return "Wdmg";
  case STAT_WEAPON_OFFHAND_DMG:   return "WOHdmg";

  case STAT_ARMOR:          return "Armor";
  case STAT_BONUS_ARMOR:    return "BArmor";
  case STAT_DEFENSE_RATING: return "Def";
  case STAT_SHIELD_RATING:  return "Shield";
  case STAT_ABSORB_RATING:  return "Abs";

  case STAT_POWER: return "Power";
  case STAT_FORCE_POWER: return "Force_Power";
  case STAT_TECH_POWER: return "Tech_Power";

  case STAT_ALL: return "All";

  default: return "unknown";
  }
}

// util_t::stat_type_wowhead ================================================

const char* util_t::stat_type_wowhead( stat_type stat )
{
  switch ( stat )
  {
  case STAT_STRENGTH:  return "str";
  case STAT_AIM:       return "aim";
  case STAT_CUNNING:   return "cun";
  case STAT_WILLPOWER: return "wil";
  case STAT_ENDURANCE: return "end";
  case STAT_PRESENCE:  return "pre";

  case STAT_HEALTH: return "health";
  case STAT_MANA:   return "mana";
  case STAT_RAGE:   return "rage";
  case STAT_ENERGY: return "energy";
  case STAT_AMMO:   return "ammo";

  case STAT_EXPERTISE_RATING: return "expertiseRating";

  case STAT_ACCURACY_RATING:      return "accuracyRating";
  case STAT_CRIT_RATING:     return "critRating";
  case STAT_ALACRITY_RATING: return "alacrityRating";
  case STAT_DEFENSE_RATING:    return "defense";

  case STAT_WEAPON_DMG:   return "__dmg";

  case STAT_ARMOR:             return "armor";
  case STAT_BONUS_ARMOR:       return "__armor"; // FIXME! Does wowhead distinguish "bonus" armor?

  case STAT_ALL: return "__all";

  default: return "unknown";
  }
}

// util_t::parse_stat_type ==================================================

stat_type util_t::parse_stat_type( const std::string& name )
{
  for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::stat_type_string( i ) ) )
      return i;

  for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::stat_type_abbrev( i ) ) )
      return i;

  for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::stat_type_wowhead( i ) ) )
      return i;

  if ( name == "rgdcritstrkrtng" ) return STAT_CRIT_RATING;

  // in-case wowhead changes their mind again
  if ( name == "critstrkrtng"   ) return STAT_CRIT_RATING;
  if ( name == "exprtng"        ) return STAT_EXPERTISE_RATING;
  if ( name == "alacrityrtng"   ) return STAT_ALACRITY_RATING;
  if ( name == "accuracy"       ) return STAT_ACCURACY_RATING;

  return STAT_NONE;
}

// util_t::parse_origin =====================================================

bool util_t::parse_origin( std::string& region_str,
                           std::string& server_str,
                           std::string& name_str,
                           const std::string& origin_str )
{
  if ( ( origin_str.find( ".battle."    ) == std::string::npos ) &&
       ( origin_str.find( ".wowarmory." ) == std::string::npos ) )
    return false;

  std::vector<std::string> tokens;
  int num_tokens = util_t::string_split( tokens, origin_str, "/:.?&=" );

  for ( int i=0; i < num_tokens; i++ )
  {
    std::string& t = tokens[ i ];

    if ( t == "http" )
    {
      if ( ( i+1 ) >= num_tokens ) return false;
      region_str = tokens[ ++i ];
    }
    else if ( t == "r" ) // old armory
    {
      if ( ( i+1 ) >= num_tokens ) return false;
      server_str = tokens[ ++i ];
    }
    else if ( t == "n" || t == "cn" ) // old armory
    {
      if ( ( i+1 ) >= num_tokens ) return false;
      name_str = tokens[ ++i ];
    }
    else if ( t == "character" ) // new battle.net
    {
      if ( ( i+2 ) >= num_tokens ) return false;
      server_str = tokens[ ++i ];
      name_str   = tokens[ ++i ];
    }
  }

  if ( region_str.empty() ) return false;
  if ( server_str.empty() ) return false;
  if (   name_str.empty() ) return false;

  return true;
}

// util_t::quality_type_string ==============================================

const char* util_t::quality_type_string( quality_type quality )
{
  switch ( quality )
  {
  case QUALITY_NONE:      return "none";
  case QUALITY_CHEAP:     return "cheap";
  case QUALITY_STANDARD:  return "standard";
  case QUALITY_PREMIUM:   return "premium";
  case QUALITY_PROTOTYPE: return "prototype";
  case QUALITY_CUSTOM:    return "custom";
  case QUALITY_ARTIFACT:  return "artifact";
  case QUALITY_LEGENDARY: return "legendary";
  case QUALITY_LEGACY:    return "legacy";
  default:                return "unknown";
  }
}

// util_t::parse_quality_type ===============================================

quality_type util_t::parse_quality_type( const std::string& quality )
{
  for ( quality_type i = QUALITY_NONE; i < QUALITY_MAX; ++i )
    if ( str_compare_ci( quality, quality_type_string( i ) ) )
      return i;
  return QUALITY_NONE;
}

// util_t::string_split =====================================================

void util_t::string_split_( std::vector<std::string>& results,
                            const std::string&        str,
                            const char*               delim,
                            bool                      allow_quotes )
{
  std::string buffer = str;
  std::string::size_type cut_pt, start = 0;

  std::string not_in_quote = delim;
  if ( allow_quotes )
    not_in_quote += '"';

  static const std::string in_quote = "\"";
  const std::string* search = &not_in_quote;

  while ( ( cut_pt = buffer.find_first_of( *search, start ) ) != buffer.npos )
  {
    if ( allow_quotes && ( buffer[ cut_pt ] == '"' ) )
    {
      buffer.erase( cut_pt, 1 );
      start = cut_pt;
      search = ( search == &not_in_quote ) ? &in_quote : &not_in_quote;
    }
    else if ( search == &not_in_quote )
    {
      if ( cut_pt > 0 )
        results.push_back( buffer.substr( 0, cut_pt ) );
      buffer.erase( 0, cut_pt + 1 );
      start = 0;
    }
  }

  if ( buffer.length() > 0 )
    results.push_back( buffer );

  /*
    std::string buffer = str;
    std::string::size_type cut_pt;

    while ( ( cut_pt = buffer.find_first_of( delim ) ) != buffer.npos )
    {
      if ( cut_pt > 0 )
      {
        results.push_back( buffer.substr( 0, cut_pt ) );
      }
      buffer = buffer.substr( cut_pt + 1 );
    }
    if ( buffer.length() > 0 )
    {
      results.push_back( buffer );
    }
  */
}

// util_t::string_split =====================================================

int util_t::string_split( const std::string& str,
                          const char*        delim,
                          const char*        format, ... )
{
  std::vector<std::string>    str_splits;
  std::vector<std::string> format_splits;

  int    str_size = util_t::string_split(    str_splits, str,    delim );
  int format_size = util_t::string_split( format_splits, format, " "   );

  if ( str_size == format_size )
  {
    va_list vap;
    va_start( vap, format );

    for ( int i=0; i < str_size; i++ )
    {
      std::string& f = format_splits[ i ];
      std::string& s = str_splits[ i ];

      if      ( f == "i" ) *( va_arg( vap, int*         ) ) = atoi( s.c_str() );
      else if ( f == "f" ) *( va_arg( vap, double*      ) ) = atof( s.c_str() );
      else if ( f == "d" ) *( va_arg( vap, double*      ) ) = atof( s.c_str() );
      else if ( f == "S" ) *( va_arg( vap, std::string* ) ) = s;
      else assert( false );
    }

    va_end( vap );
  }

  return str_size;
}

// util_t::string_strip_quotes ==============================================

void util_t::string_strip_quotes( std::string& str )
{
  std::string::size_type pos = str.find( '"' );
  if ( pos == str.npos ) return;

  std::string::iterator dst = str.begin() + pos, src = dst;
  while ( ++src != str.end() )
  {
    if ( *src != '"' )
      *dst++ = *src;
  }

  str.resize( dst - str.begin() );
}

// util_t::replace_all ======================================================

void util_t::replace_all_( std::string& s, const char* from, char to )
{
  std::string::size_type pos = s.find( from );
  if ( pos != s.npos )
  {
    std::size_t len = std::strlen( from );
    do
      s.replace( pos, len, 1, to );
    while ( ( pos = s.find( from, pos ) ) != s.npos );
  }
}

void util_t::replace_all_( std::string& s, char from, const char* to )
{
  std::string::size_type pos;
  if ( ( pos = s.find( from ) ) != s.npos )
  {
    std::size_t len = std::strlen( to );
    do
    {
      s.replace( pos, 1, to, len );
      pos += len;
    }
    while ( ( pos = s.find( from, pos ) ) != s.npos );
  }
}

// util_t::to_string ========================================================

std::string util_t::to_string( double f, int precision )
{
  std::ostringstream ss;
  ss << std::fixed << std::setprecision( precision ) << f;
  return ss.str();
}

// util_t::to_string ========================================================

std::string util_t::to_string( double f )
{
  if ( std::abs( f - static_cast<int>( f ) ) < 0.001 )
    return to_string( static_cast<int>( f ) );
  else
    return to_string( f, 3 );
}

// util_t::parse_date =======================================================

int64_t util_t::parse_date( const std::string& month_day_year )
{
  std::vector<std::string> splits;
  int num_splits = util_t::string_split( splits, month_day_year, " _,;-/ \t\n\r" );
  if ( num_splits != 3 ) return 0;

  std::string& month = splits[ 0 ];
  std::string& day   = splits[ 1 ];
  std::string& year  = splits[ 2 ];

  std::transform( month.begin(), month.end(), month.begin(), ( int( * )( int ) ) std::tolower );

  if ( month.find( "jan" ) != std::string::npos ) month = "01";
  if ( month.find( "feb" ) != std::string::npos ) month = "02";
  if ( month.find( "mar" ) != std::string::npos ) month = "03";
  if ( month.find( "apr" ) != std::string::npos ) month = "04";
  if ( month.find( "may" ) != std::string::npos ) month = "05";
  if ( month.find( "jun" ) != std::string::npos ) month = "06";
  if ( month.find( "jul" ) != std::string::npos ) month = "07";
  if ( month.find( "aug" ) != std::string::npos ) month = "08";
  if ( month.find( "sep" ) != std::string::npos ) month = "09";
  if ( month.find( "oct" ) != std::string::npos ) month = "10";
  if ( month.find( "nov" ) != std::string::npos ) month = "11";
  if ( month.find( "dec" ) != std::string::npos ) month = "12";

  if ( month.size() == 1 ) month.insert( month.begin(), '0'  );
  if ( day  .size() == 1 ) day  .insert( day  .begin(), '0'  );
  if ( year .size() == 2 ) year .insert( year .begin(), 2, '0' );

  if ( day.size()   != 2 ) return 0;
  if ( month.size() != 2 ) return 0;
  if ( year.size()  != 4 ) return 0;

  std::string buffer = year + month + day;

  return atoi( buffer.c_str() );
}

// util_t::vfprintf_helper ==================================================

int util_t::vfprintf_helper( FILE *stream, const char *format, va_list args )
{
  std::string p_locale = setlocale( LC_CTYPE, NULL );
  setlocale( LC_CTYPE, "" );

  int retcode = ::vfprintf( stream, format, args );

  setlocale( LC_CTYPE, p_locale.c_str() );

  return retcode;
}

// util_t::fprintf ==========================================================

int util_t::fprintf( FILE *stream, const char *format,  ... )
{
  va_list fmtargs;
  va_start( fmtargs, format );

  int retcode = vfprintf_helper( stream, format, fmtargs );

  va_end( fmtargs );

  return retcode;
}

// util_t::printf ===========================================================

int util_t::printf( const char *format,  ... )
{
  va_list fmtargs;
  va_start( fmtargs, format );

  int retcode = vfprintf_helper( stdout, format, fmtargs );

  va_end( fmtargs );

  return retcode;
}

// util_t::schkprintf_report ================================================

void util_t::schkprintf_report( const char* file, const char* function,
                                int line, size_t size, int rval )
{
  std::cerr << "schkprintf: buffer too small (" << rval << ">="
            << size << ") at file \"" << file << "\" line "
            << line << ", in function \"" << function << '"'
            << std::endl;
  abort();
}

#if !defined(__GNUC__)
// util_t::schkprintf =======================================================

int util_t::schkprintf( const char* file, const char* function, int line,
                        char* buf, size_t size, const char* fmt, ... )
{
  va_list args;
  va_start( args, fmt );
  int rval = vsnprintf( buf, size, fmt, args );
  va_end( args );

  if ( unlikely( rval >= static_cast<int>( size ) ) )
    schkprintf_report( file, function, line, size, rval );

  return rval;
}
#endif // !defined(__GNUC__)

// util_t::str_to_utf8_ =====================================================

void util_t::str_to_utf8_( std::string& str )
{
  std::string::iterator p = utf8::find_invalid( str.begin(), str.end() );
  if ( p == str.end() ) return;

  std::string temp( str.begin(), p );
  for ( std::string::iterator e = str.end(); p != e; ++p )
    utf8::append( static_cast<unsigned char>( *p ), std::back_inserter( temp ) );

  str.swap( temp );
}

// util_t::str_to_latin1_ ===================================================

void util_t::str_to_latin1_( std::string& str )
{
  if ( str.empty() ) return;
  if ( ! utf8::is_valid( str.begin(), str.end() ) ) return;


  std::string temp;
  std::string::iterator i = str.begin(), e = str.end();

  while ( i != e )
    temp += ( unsigned char ) utf8::next( i, e );

  str.swap( temp );
}

// util_t::urlencode_ =======================================================

void util_t::urlencode_( std::string& str )
{
  std::string::size_type l = str.length();
  if ( ! l ) return;

  std::string temp;

  for ( std::string::size_type i = 0; i < l; ++i )
  {
    unsigned char c = str[ i ];

    if ( c > 0x7F || c == ' ' || c == '\'' )
      temp += ( boost::format( "%%%02X" ) % c ).str();
    else if ( c == '+' )
      temp += "%20";
    else if ( c < 0x20 )
      continue;
    else
      temp += c;
  }

  str.swap( temp );
}

// util_t::urldecode_ =======================================================

void util_t::urldecode_( std::string& str )
{
  std::string::size_type l = str.length();
  if ( ! l ) return;

  std::string temp;

  for ( std::string::size_type i = 0; i < l; ++i )
  {
    unsigned char c = ( unsigned char ) str[ i ];

    if ( c == '%' && i + 2 < l )
    {
      long v = strtol( str.substr( i + 1, 2 ).c_str(), 0, 16 );
      if ( v ) temp += ( unsigned char ) v;
      i += 2;
    }
    else if ( c == '+' )
      temp += ' ';
    else
      temp += c;
  }

  str.swap( temp );
}

// util_t::format_text ======================================================

void util_t::format_text_( std::string& name, bool input_is_utf8 )
{
  if ( name.empty() ) return;
  bool is_utf8 = utf8::is_valid( name.begin(), name.end() );

  if ( is_utf8 && ! input_is_utf8 )
    util_t::str_to_latin1( name );
  else if ( ! is_utf8 && input_is_utf8 )
    util_t::str_to_utf8( name );
}

// util_t::html_special_char_decode_ ========================================

void util_t::html_special_char_decode_( std::string& str )
{
  std::string::size_type pos = 0;

  while ( ( pos = str.find( "&", pos ) ) != std::string::npos )
  {
    if ( str[ pos+1 ] == '#' )
    {
      std::string::size_type end = str.find( ';', pos + 2 );
      char encoded = ( char ) atoi( str.substr( pos + 2, end ).c_str() );
      str.erase( pos, end - pos + 1 );
      str.insert( pos, 1, encoded );
    }
    else if ( 0 == str.compare( pos, 6, "&quot;" ) )
    {
      str.erase( pos, 6 );
      str.insert( pos, "\"" );
    }
    else if ( 0 == str.compare( pos, 5, "&amp;" ) )
    {
      str.erase( pos, 5 );
      str.insert( pos, "&" );
    }
    else if ( 0 == str.compare( pos, 4, "&lt;" ) )
    {
      str.erase( pos, 4 );
      str.insert( pos, "<" );
    }
    else if ( 0 == str.compare( pos, 4, "&gt;" ) )
    {
      str.erase( pos, 4 );
      str.insert( pos, ">" );
    }
  }
}

// util_t::floor ============================================================

double util_t::floor( double X, unsigned int decplaces )
{
  switch ( decplaces )
  {
  case 0: return ::floor( X );
  case 1: return ::floor( X * 10.0 ) * 0.1;
  case 2: return ::floor( X * 100.0 ) * 0.01;
  case 3: return ::floor( X * 1000.0 ) * 0.001;
  case 4: return ::floor( X * 10000.0 ) * 0.0001;
  case 5: return ::floor( X * 100000.0 ) * 0.00001;
  default:
    double mult = 1000000.0;
    double div = SC_EPSILON;
    for ( unsigned int i = 6; i < decplaces; i++ )
    {
      mult *= 10.0;
      div *= 0.1;
    }
    return ::floor( X * mult ) * div;
  }
}

// util_t::ceil =============================================================

double util_t::ceil( double X, unsigned int decplaces )
{
  switch ( decplaces )
  {
  case 0: return ::ceil( X );
  case 1: return ::ceil( X * 10.0 ) * 0.1;
  case 2: return ::ceil( X * 100.0 ) * 0.01;
  case 3: return ::ceil( X * 1000.0 ) * 0.001;
  case 4: return ::ceil( X * 10000.0 ) * 0.0001;
  case 5: return ::ceil( X * 100000.0 ) * 0.00001;
  default:
    double mult = 1000000.0;
    double div = SC_EPSILON;
    for ( unsigned int i = 6; i < decplaces; i++ )
    {
      mult *= 10.0;
      div *= 0.1;
    }
    return ::ceil( X * mult ) * div;
  }
}

// util_t::round ============================================================

double util_t::round( double X, unsigned int decplaces )
{
  switch ( decplaces )
  {
  case 0: return ::floor( X + 0.5 );
  case 1: return ::floor( X * 10.0 + 0.5 ) * 0.1;
  case 2: return ::floor( X * 100.0 + 0.5 ) * 0.01;
  case 3: return ::floor( X * 1000.0 + 0.5 ) * 0.001;
  case 4: return ::floor( X * 10000.0 + 0.5 ) * 0.0001;
  case 5: return ::floor( X * 100000.0 + 0.5 ) * 0.00001;
  default:
    double mult = 1000000.0;
    double div = SC_EPSILON;
    for ( unsigned int i = 6; i < decplaces; i++ )
    {
      mult *= 10.0;
      div *= 0.1;
    }
    return ::floor( X * mult + 0.5 ) * div;
  }
}

// util_t::tolower_ =========================================================

void util_t::tolower_( std::string& str )
{
  for ( std::string::size_type i = 0, n = str.length(); i < n; ++i )
    str[i] = ::tolower( str[i] );
}

//-------------------------------
// std::STRING   utils
//-------------------------------

// utility functions
std::string tolower( const std::string& src )
{
  std::string result;
  result.reserve( src.size() );
  for ( char c : src )
    result += std::tolower( c );
  return result;
}

#if 0 // UNUSED
void replace_char( std::string& str, char old_c, char new_c  )
{
  for ( std::string::size_type i = 0, n = str.length(); i < n; ++i )
  {
    if ( str[ i ] == old_c )
      str[ i ] = new_c;
  }
}

void replace_str( std::string& src, const std::string& old_str, const std::string& new_str  )
{
  if ( old_str.empty() ) return;

  std::string::size_type p = 0;
  while ( ( p = src.find( old_str, p ) ) != std::string::npos )
  {
    src.replace( p, p + old_str.length(), new_str );
    p += new_str.length();
  }
}

bool str_to_float( std::string src, double& dest )
{
  bool was_sign=false;
  bool was_digit=false;
  bool was_dot=false;
  bool res=true;
  //check each char
  for ( std::size_t p=0; res&&( p<src.length() ); p++ )
  {
    char ch=src[p];
    if ( ch==' ' ) continue;
    if ( ( ( ch=='+' )||( ch=='-' ) )&& !( was_sign||was_digit ) )
    {
      was_sign=true;
      continue;
    }
    if ( ( ch=='.' )&& was_digit && !was_dot )
    {
      was_dot=true;
      continue;
    }
    if ( ( ch>='0' )&&( ch<='9' ) )
    {
      was_digit=true;
      continue;
    }
    //if none of above, fail
    res=false;
  }
  //check if we had at least one digit
  if ( !was_digit ) res=false;
  //return result
  dest=atof( src.c_str() );
  return res;
}
#endif

int base36_t::decode( char c ) const
{
  auto p = boost::find( encoding, c );
  if ( p != std::end( encoding ) )
    return p - std::begin( encoding );

  throw bad_char( c );
}

// util_t::format_name_ ========================================================

void util_t::format_name_( std::string& name )
{
  if ( name.empty() ) return;

  util_t::str_to_utf8( name );

  std::string buffer;

  for ( size_t i = 0, size = name.size(); i < size; i++ )
  {
    char c = name[ i ];

    if ( unlikely( c & 0x80 ) )
      continue;

    else if ( std::isalpha( c ) )
      c = std::tolower( c );

    else if ( c == ' ' || c == '-' )
      c = '_';

    else if ( ( c == '_' || c == '+' ) && i == 0 )
      continue;

    else if ( c != '_' &&
              c != '+' &&
              c != '.' &&
              c != '%' &&
              ! std::isdigit( c ) )
      continue;

    buffer += c;
  }

  name.swap( buffer );
}
