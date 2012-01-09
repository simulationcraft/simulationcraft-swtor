// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"
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

// util_t::dup ==============================================================

char* util_t::dup( const char *value )
{
  std::size_t n = strlen( value ) + 1;
  void *p = malloc( n );
  if ( p ) memcpy( p, value, n );
  return static_cast<char*>( p );
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

const char* util_t::role_type_string( int role )
{
  switch ( role )
  {
  case ROLE_ATTACK:    return "attack";
  case ROLE_SPELL:     return "spell";
  case ROLE_HYBRID:    return "hybrid";
  case ROLE_DPS:       return "dps";
  case ROLE_TANK:      return "tank";
  case ROLE_HEAL:      return "heal";
  case ROLE_NONE:      return "auto";
  }
  return "unknown";
}

// util_t::parse_role_type ==================================================

role_type util_t::parse_role_type( const std::string& name )
{
  for ( int i=( int ) ROLE_NONE; i < ( int )ROLE_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::role_type_string( i ) ) )
      return ( role_type ) i;

  return ROLE_HYBRID;
}

// util_t::race_type_string =================================================

const char* util_t::race_type_string( int type )
{
  switch ( type )
  {
  case RACE_NONE:        return "none";
  case RACE_HUMAN:       return "human";
  case RACE_MIRIALAN:    return "mirialan";
  case RACE_MIRALUKA:    return "miraluka";
  case RACE_TWILEK:      return "twilek";
  case RACE_ZABRAK:      return "zabrak";
  case RACE_RATTATAKI:   return "rattataki";
  case RACE_PUREBLOOD:   return "pureblood";
  }
  return "unknown";
}

// util_t::parse_race_type ==================================================

race_type util_t::parse_race_type( const std::string& name )
{
  for ( int i=( int ) RACE_NONE; i < ( int )RACE_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::race_type_string( i ) ) )
      return ( race_type ) i;

  return RACE_NONE;
}

// util_t::parse_position_type ==============================================

position_type util_t::parse_position_type( const std::string& name )
{
  for ( int i = ( int ) POSITION_NONE; i < ( int ) POSITION_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::position_type_string( i ) ) )
      return ( position_type ) i;

  return POSITION_NONE;
}

// util_t::position_type_string =============================================

const char* util_t::position_type_string( int type )
{
  switch ( type )
  {
  case POSITION_NONE:         return "none";
  case POSITION_BACK:         return "back";
  case POSITION_FRONT:        return "front";
  case POSITION_RANGED_BACK:  return "ranged_back";
  case POSITION_RANGED_FRONT: return "ranged_front";
  }

  return "unknown";
}

// util_t::profession_type_string ===========================================

const char* util_t::profession_type_string( int type )
{
  switch ( type )
  {
  case PROFESSION_NONE:     return "none";
  }
  return "unknown";
}

// util_t::parse_profession_type ============================================

int util_t::parse_profession_type( const std::string& name )
{
  for ( int i=0; i < PROFESSION_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::profession_type_string( i ) ) )
      return i;

  return PROFESSION_NONE;
}

// util_t::translate_profession_id ==========================================

profession_type util_t::translate_profession_id( int skill_id )
{
  switch ( skill_id )
  {
  }
  return PROFESSION_NONE;
}

// util_t::player_type_string ===============================================

const char* util_t::player_type_string( int type )
{
  switch ( type )
  {
  case PLAYER_NONE:     return "none";
  case SITH_WARRIOR:    return "sith_warrior";
  case TROOPER:         return "trooper";
  case BOUNTY_HUNTER:   return "bounty_hunter";
  case IMPERIAL_AGENT:  return "imperial_agent";
  case SMUGGLER:        return "smuggler";
  case SITH_INQUISITOR: return "sith_inquisitor";
  case JEDI_KNIGHT:     return "jedi_knight";
  case PLAYER_PET:      return "pet";
  case PLAYER_GUARDIAN: return "guardian";
  case ENEMY:           return "enemy";
  case ENEMY_ADD:       return "add";
  case JEDI_SAGE:       return "jedi_sage";
  }
  return "unknown";
}

// util_t::translate_class_str ==============================================

int util_t::translate_class_str( std::string& s )
{
  std::string fmt_s = armory_t::format( s );
  if      ( fmt_s == "sith_warrior"    ) return SITH_WARRIOR;
  else if ( fmt_s == "trooper"         ) return TROOPER;
  else if ( fmt_s == "bounty_hunter"   ) return BOUNTY_HUNTER;
  else if ( fmt_s == "imperial_agent"  ) return IMPERIAL_AGENT;
  else if ( fmt_s == "smuggler"        ) return SMUGGLER;
  else if ( fmt_s == "sith_inquisitor" ) return SITH_INQUISITOR;
  else if ( fmt_s == "jedi_knight"     ) return JEDI_KNIGHT;
  else if ( fmt_s == "jedi_sage"       ) return JEDI_SAGE;
  return PLAYER_NONE;
}

// util_t::parse_player_type ================================================

player_type util_t::parse_player_type( const std::string& name )
{
  for ( int i=( int ) PLAYER_NONE; i < ( int ) PLAYER_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::player_type_string( i ) ) )
      return ( player_type ) i;

  return PLAYER_NONE;
}

// util_t::pet_type_string ==================================================

const char* util_t::pet_type_string( int type )
{
  switch ( type )
  {
  case PET_NONE:                return "none";
  case PET_ENEMY:               return "pet_enemy";
  }
  return "unknown";
}

// util_t::parse_pet_type ===================================================

pet_type_t util_t::parse_pet_type( const std::string& name )
{
  for ( int i=( int )PET_NONE; i < ( int )PET_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::pet_type_string( i ) ) )
      return ( pet_type_t )i;

  return PET_NONE;
}

// util_t::attribute_type_string ============================================

const char* util_t::attribute_type_string( int type )
{
  switch ( type )
  {
  case ATTR_STRENGTH:  return "strength";
  case ATTR_AGILITY:   return "agility";
  case ATTR_STAMINA:   return "stamina";
  case ATTR_INTELLECT: return "intellect";
  case ATTR_SPIRIT:    return "spirit";
  }
  return "unknown";
}

// util_t::parse_attribute_type =============================================

int util_t::parse_attribute_type( const std::string& name )
{
  for ( int i=0; i < ATTRIBUTE_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::attribute_type_string( i ) ) )
      return i;

  return ATTRIBUTE_NONE;
}

// util_t::dmg_type_string ==================================================

const char* util_t::dmg_type_string( int type )
{
  switch ( type )
  {
  case DMG_DIRECT:    return "hit";
  case DMG_OVER_TIME: return "tick";
  }
  return "unknown";
}

// util_t::gem_type_string ==================================================

const char* util_t::gem_type_string( int type )
{
  switch ( type )
  {
  case GEM_META:      return "meta";
  case GEM_PRISMATIC: return "prismatic";
  case GEM_RED:       return "red";
  case GEM_YELLOW:    return "yellow";
  case GEM_BLUE:      return "blue";
  case GEM_ORANGE:    return "orange";
  case GEM_GREEN:     return "green";
  case GEM_PURPLE:    return "purple";
  case GEM_COGWHEEL:  return "cogwheel";
  }
  return "unknown";
}

// util_t::parse_gem_type ===================================================

int util_t::parse_gem_type( const std::string& name )
{
  for ( int i=0; i < GEM_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::gem_type_string( i ) ) )
      return i;

  return GEM_NONE;
}

// util_t::meta_gem_type_string =============================================

const char* util_t::meta_gem_type_string( int type )
{
  switch ( type )
  {
    case META_GEM_NONE:         return "none";
  }
  return "unknown";
}

// util_t::parse_meta_gem_type ==============================================

int util_t::parse_meta_gem_type( const std::string& name )
{
  for ( int i=0; i < META_GEM_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::meta_gem_type_string( i ) ) )
      return i;

  return META_GEM_NONE;
}

// util_t::result_type_string ===============================================

const char* util_t::result_type_string( int type )
{
  switch ( type )
  {
  case RESULT_NONE:   return "none";
  case RESULT_MISS:   return "miss";
  case RESULT_RESIST: return "resist";
  case RESULT_DODGE:  return "dodge";
  case RESULT_PARRY:  return "parry";
  case RESULT_BLOCK:  return "block";
  case RESULT_GLANCE: return "glance";
  case RESULT_CRIT:   return "crit";
  case RESULT_CRIT_BLOCK: return "crit-block";
  case RESULT_HIT:    return "hit";
  }
  return "unknown";
}

// util_t::parse_result_type ================================================

int util_t::parse_result_type( const std::string& name )
{
  for ( int i=0; i < RESULT_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::result_type_string( i ) ) )
      return i;

  return RESULT_NONE;
}

// util_t::resource_type_string =============================================

const char* util_t::resource_type_string( int type )
{
  switch ( type )
  {
  case RESOURCE_NONE:         return "none";
  case RESOURCE_HEALTH:       return "health";
  case RESOURCE_MANA:         return "mana";
  case RESOURCE_RAGE:         return "rage";
  case RESOURCE_ENERGY:       return "energy";
  case RESOURCE_AMMO:         return "ammo";
  case RESOURCE_FORCE:        return "force";
  }
  return "unknown";
}

// util_t::parse_resource_type ==============================================

int util_t::parse_resource_type( const std::string& name )
{
  for ( int i=0; i < RESOURCE_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::resource_type_string( i ) ) )
      return i;

  return RESOURCE_NONE;
}

// util_t::school_type_component ============================================

int util_t::school_type_component( int s_type, int c_type )
{
  uint32_t s_mask = spell_id_t::get_school_mask( ( school_type ) s_type );
  uint32_t c_mask = spell_id_t::get_school_mask( ( school_type ) c_type );

  return s_mask & c_mask;
}

// util_t::school_type_string ===============================================

const char* util_t::school_type_string( int school )
{
  switch ( school )
  {
  case SCHOOL_ARCANE:           return "arcane";
  case SCHOOL_BLEED:            return "bleed";
  case SCHOOL_CHAOS:            return "chaos";
  case SCHOOL_FIRE:             return "fire";
  case SCHOOL_FROST:            return "frost";
  case SCHOOL_FROSTFIRE:        return "frostfire";
  case SCHOOL_HOLY:             return "holy";
  case SCHOOL_NATURE:           return "nature";
  case SCHOOL_PHYSICAL:         return "physical";
  case SCHOOL_SHADOW:           return "shadow";
  case SCHOOL_HOLYSTRIKE:       return "holystrike";
  case SCHOOL_FLAMESTRIKE:      return "flamestrike";
  case SCHOOL_HOLYFIRE:         return "holyfire";
  case SCHOOL_STORMSTRIKE:      return "stormstrike";
  case SCHOOL_HOLYSTORM:        return "holystorm";
  case SCHOOL_FIRESTORM:        return "firestorm";
  case SCHOOL_FROSTSTRIKE:      return "froststrike";
  case SCHOOL_HOLYFROST:        return "holyfrost";
  case SCHOOL_FROSTSTORM:       return "froststorm";
  case SCHOOL_SHADOWSTRIKE:     return "shadowstrike";
  case SCHOOL_SHADOWLIGHT:      return "shadowlight";
  case SCHOOL_SHADOWFLAME:      return "shadowflame";
  case SCHOOL_SHADOWSTORM:      return "shadowstorm";
  case SCHOOL_SHADOWFROST:      return "shadowfrost";
  case SCHOOL_SPELLSTRIKE:      return "spellstrike";
  case SCHOOL_DIVINE:           return "divine";
  case SCHOOL_SPELLFIRE:        return "spellfire";
  case SCHOOL_SPELLSTORM:       return "spellstorm";
  case SCHOOL_SPELLFROST:       return "spellfrost";
  case SCHOOL_SPELLSHADOW:      return "spellshadow";
  case SCHOOL_ELEMENTAL:        return "elemental";
  case SCHOOL_CHROMATIC:        return "chromatic";
  case SCHOOL_MAGIC:            return "magic";
  case SCHOOL_DRAIN:            return "drain";
  }
  return "unknown";
}

// util_t::parse_school_type ================================================

school_type util_t::parse_school_type( const std::string& name )
{
  for ( int i=SCHOOL_NONE; i < SCHOOL_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::school_type_string( i ) ) )
      return ( school_type )( i );

  return SCHOOL_NONE;
}

// util_t::talent_tree ======================================================

int util_t::talent_tree( int tree, player_type ptype )
{
  switch ( ptype )
  {
    case JEDI_SAGE:
      switch ( tree )
      {
      case JEDI_SAGE_SEER:          return TREE_SEER;
      case JEDI_SAGE_TELEKINETICS:  return TREE_TELEKINETICS;
      case JEDI_SAGE_BALANCE:       return TREE_BALANCE;
      }
    default:
      break;
  }

  return TREE_NONE;
}

// util_t::talent_tree_string ===============================================

const char* util_t::talent_tree_string( int tree, bool armory_format )
{
  if ( armory_format )
  {
    switch ( tree )
    {
    case TREE_SEER:         return "seer";
    case TREE_TELEKINETICS: return "telekinetics";
    case TREE_BALANCE:      return "balance";
    default: return "Unknown";
    }
  }
  else
  {
    switch ( tree )
    {
    case TREE_SEER:         return "Seer";
    case TREE_TELEKINETICS: return "Telekinetics";
    case TREE_BALANCE:      return "Balance";
    default: return "Unknown";
    }
  }
}

// util_t::parse_talent_tree ================================================

int util_t::parse_talent_tree( const std::string& name )
{
  for ( int i=0; i < TALENT_TREE_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::talent_tree_string( i ) ) )
      return i;

  return TREE_NONE;
}

// util_t::weapon_type_string ===============================================

const char* util_t::weapon_type_string( int weapon )
{
  switch ( weapon )
  {
  case WEAPON_NONE:     return "none";
  case WEAPON_DAGGER:   return "dagger";
  case WEAPON_FIST:     return "fist";
  case WEAPON_BEAST:    return "beast";
  case WEAPON_SWORD:    return "sword";
  case WEAPON_MACE:     return "mace";
  case WEAPON_AXE:      return "axe";
  case WEAPON_BEAST_2H: return "beast2h";
  case WEAPON_SWORD_2H: return "sword2h";
  case WEAPON_MACE_2H:  return "mace2h";
  case WEAPON_AXE_2H:   return "axe2h";
  case WEAPON_STAFF:    return "staff";
  case WEAPON_POLEARM:  return "polearm";
  case WEAPON_BOW:      return "bow";
  case WEAPON_CROSSBOW: return "crossbow";
  case WEAPON_GUN:      return "gun";
  case WEAPON_THROWN:   return "thrown";
  }
  return "unknown";
}

// util_t::weapon_subclass_string ===========================================

const char* util_t::weapon_subclass_string( int subclass )
{
  switch ( subclass )
  {
  case ITEM_SUBCLASS_WEAPON_AXE:      return "Axe";
  case ITEM_SUBCLASS_WEAPON_AXE2:     return "Axe";
  case ITEM_SUBCLASS_WEAPON_BOW:      return "Bow";
  case ITEM_SUBCLASS_WEAPON_GUN:      return "Gun";
  case ITEM_SUBCLASS_WEAPON_MACE:     return "Mace";
  case ITEM_SUBCLASS_WEAPON_MACE2:    return "Mace";
  case ITEM_SUBCLASS_WEAPON_POLEARM:  return "Polearm";
  case ITEM_SUBCLASS_WEAPON_SWORD:    return "Sword";
  case ITEM_SUBCLASS_WEAPON_SWORD2:   return "Sword";
  case ITEM_SUBCLASS_WEAPON_STAFF:    return "Staff";
  case ITEM_SUBCLASS_WEAPON_FIST:     return "Fist Weapon";
  case ITEM_SUBCLASS_WEAPON_DAGGER:   return "Dagger";
  case ITEM_SUBCLASS_WEAPON_THROWN:   return "Thrown";
  case ITEM_SUBCLASS_WEAPON_CROSSBOW: return "Crossbow";
  case ITEM_SUBCLASS_WEAPON_WAND:     return "Wand";
  }
  return "Unknown";
}

// util_t::weapon_class_string ==============================================

const char* util_t::weapon_class_string( int class_ )
{
  switch ( class_ )
  {
  case INVTYPE_WEAPON:
    return "One Hand";
  case INVTYPE_2HWEAPON:
    return "Two-Hand";
  case INVTYPE_WEAPONMAINHAND:
    return "Main Hand";
  case INVTYPE_WEAPONOFFHAND:
    return "Off Hand";
  }
  return 0;
}

// util_t::set_item_type_string =============================================

const char* util_t::set_item_type_string( int item_set )
{
  switch ( item_set )
  {
    // Melee sets
  case 925:   // Death Knight T11
  case 927:   // Druid T11
  case 932:   // Paladin T11
  case 939:   // Shaman T11
  case 942:   // Warrior T11
  case 1000:  // Death Knight T12
  case 1002:  // Druid T12
  case 1011:  // Paladin T12
  case 1015:  // Shaman T12
  case 1017:  // Warrior T12
    return "Melee";

    // Tank sets
  case 926:   // Death Knight T11
  case 934:   // Paladin T11
  case 943:   // Warrior T11
  case 1001:  // Death Knight T12
  case 1013:  // Paladin T12
  case 1018:  // Warrior T12
    return "Tank";

    // Healer sets
  case 928:   // Druid T11
  case 933:   // Paladin T11
  case 935:   // Priest T11
  case 938:   // Shaman T11
  case 1003:  // Druid T12
  case 1010:  // Priest T12
  case 1012:  // Paladin T12
  case 1014:  // Shaman T12
    return "Healer";

    // DPS Caster sets
  case 929:   // Druid T11
  case 936:   // Priest T11
  case 940:   // Shaman T11
  case 1004:  // Druid T12
  case 1009:  // Priest T12
  case 1016:  // Shaman T12
    return "Caster";
  }
  return 0;
}

// util_t::parse_weapon_type ================================================

int util_t::parse_weapon_type( const std::string& name )
{
  for ( int i=0; i < WEAPON_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::weapon_type_string( i ) ) )
      return i;

  return WEAPON_NONE;
}

// util_t::flask_type_string ================================================

const char* util_t::flask_type_string( int flask )
{
  switch ( flask )
  {
  case FLASK_NONE:               return "none";
  }
  return "unknown";
}

// util_t::parse_flask_type =================================================

int util_t::parse_flask_type( const std::string& name )
{
  for ( int i=0; i < FLASK_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::flask_type_string( i ) ) )
      return i;

  return FLASK_NONE;
}

// util_t::food_type_string =================================================

const char* util_t::food_type_string( int food )
{
  switch ( food )
  {
  case FOOD_NONE:                     return "none";
  }
  return "unknown";
}

// util_t::parse_food_type ==================================================

int util_t::parse_food_type( const std::string& name )
{
  for ( int i=0; i < FOOD_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::food_type_string( i ) ) )
      return i;

  return FOOD_NONE;
}

// util_t::set_bonus_string =================================================

const char* util_t::set_bonus_string( set_type type )
{
  switch ( type )
  {
  case SET_T11_2PC_CASTER: return "tier11_2pc_caster";
  case SET_T11_4PC_CASTER: return "tier11_4pc_caster";
  case SET_T11_2PC_MELEE:  return "tier11_2pc_melee";
  case SET_T11_4PC_MELEE:  return "tier11_4pc_melee";
  case SET_T11_2PC_TANK:   return "tier11_2pc_tank";
  case SET_T11_4PC_TANK:   return "tier11_4pc_tank";
  case SET_T12_2PC_CASTER: return "tier12_2pc_caster";
  case SET_T12_4PC_CASTER: return "tier12_4pc_caster";
  case SET_T12_2PC_MELEE:  return "tier12_2pc_melee";
  case SET_T12_4PC_MELEE:  return "tier12_4pc_melee";
  case SET_T12_2PC_TANK:   return "tier12_2pc_tank";
  case SET_T12_4PC_TANK:   return "tier12_4pc_tank";
  case SET_T13_2PC_CASTER: return "tier13_2pc_caster";
  case SET_T13_4PC_CASTER: return "tier13_4pc_caster";
  case SET_T13_2PC_MELEE:  return "tier13_2pc_melee";
  case SET_T13_4PC_MELEE:  return "tier13_4pc_melee";
  case SET_T13_2PC_TANK:   return "tier13_2pc_tank";
  case SET_T13_4PC_TANK:   return "tier13_4pc_tank";
  case SET_T14_2PC_CASTER: return "tier14_2pc_caster";
  case SET_T14_4PC_CASTER: return "tier14_4pc_caster";
  case SET_T14_2PC_MELEE:  return "tier14_2pc_melee";
  case SET_T14_4PC_MELEE:  return "tier14_4pc_melee";
  case SET_T14_2PC_TANK:   return "tier14_2pc_tank";
  case SET_T14_4PC_TANK:   return "tier14_4pc_tank";
  case SET_PVP_2PC_CASTER: return "pvp_2pc_caster";
  case SET_PVP_4PC_CASTER: return "pvp_4pc_caster";
  case SET_PVP_2PC_MELEE:  return "pvp_2pc_melee";
  case SET_PVP_4PC_MELEE:  return "pvp_4pc_melee";
  case SET_PVP_2PC_TANK:   return "pvp_2pc_tank";
  case SET_PVP_4PC_TANK:   return "pvp_4pc_tank";
  default:
    break;
  }
  return "unknown";
}

// util_t::parse_set_bonus ==================================================

set_type util_t::parse_set_bonus( const std::string& name )
{
  for ( int i=0; i < SET_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::set_bonus_string( ( set_type ) i ) ) )
      return ( set_type ) i;

  return SET_NONE;
}

// util_t::slot_type_string =================================================

const char* util_t::slot_type_string( int slot )
{
  switch ( slot )
  {
  case SLOT_HEAD:      return "head";
  case SLOT_NECK:      return "neck";
  case SLOT_SHOULDERS: return "shoulders";
  case SLOT_SHIRT:     return "shirt";
  case SLOT_CHEST:     return "chest";
  case SLOT_WAIST:     return "waist";
  case SLOT_LEGS:      return "legs";
  case SLOT_FEET:      return "feet";
  case SLOT_WRISTS:    return "wrists";
  case SLOT_HANDS:     return "hands";
  case SLOT_FINGER_1:  return "finger1";
  case SLOT_FINGER_2:  return "finger2";
  case SLOT_TRINKET_1: return "trinket1";
  case SLOT_TRINKET_2: return "trinket2";
  case SLOT_BACK:      return "back";
  case SLOT_MAIN_HAND: return "main_hand";
  case SLOT_OFF_HAND:  return "off_hand";
  case SLOT_RANGED:    return "ranged";
  case SLOT_TABARD:    return "tabard";
  }
  return "unknown";
}

// util_t::armor_type_string ================================================

const char* util_t::armor_type_string( player_type ptype, int slot_type )
{
  return NULL;
}

// util_t::parse_slot_type ==================================================

int util_t::parse_slot_type( const std::string& name )
{
  for ( int i=0; i < SLOT_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::slot_type_string( i ) ) )
      return i;

  return SLOT_NONE;
}

// util_t::stat_type_string =================================================

const char* util_t::stat_type_string( int stat )
{
  switch ( stat )
  {
  case STAT_STRENGTH:  return "strength";
  case STAT_AGILITY:   return "agility";
  case STAT_STAMINA:   return "stamina";
  case STAT_INTELLECT: return "intellect";
  case STAT_SPIRIT:    return "spirit";
  case STAT_WILLPOWER: return "willpower";

  case STAT_HEALTH: return "health";
  case STAT_MANA:   return "mana";
  case STAT_MAX_MANA: return "maximum_mana";
  case STAT_RAGE:   return "rage";
  case STAT_ENERGY: return "energy";
  case STAT_AMMO:  return "ammo";

  case STAT_SPELL_POWER:       return "spell_power";
  case STAT_SPELL_PENETRATION: return "spell_penetration";
  case STAT_MP5:               return "mp5";

  case STAT_ATTACK_POWER:             return "attack_power";
  case STAT_EXPERTISE_RATING:         return "expertise_rating";
  case STAT_EXPERTISE_RATING2:        return "inverse_expertise_rating";

  case STAT_HIT_RATING:   return "hit_rating";
  case STAT_HIT_RATING2:  return "inverse_hit_rating";
  case STAT_CRIT_RATING:  return "crit_rating";
  case STAT_HASTE_RATING: return "haste_rating";

  case STAT_WEAPON_DPS:   return "weapon_dps";
  case STAT_WEAPON_SPEED: return "weapon_speed";

  case STAT_WEAPON_OFFHAND_DPS:    return "weapon_offhand_dps";
  case STAT_WEAPON_OFFHAND_SPEED:  return "weapon_offhand_speed";

  case STAT_ARMOR:             return "armor";
  case STAT_BONUS_ARMOR:       return "bonus_armor";
  case STAT_RESILIENCE_RATING: return "resilience_rating";
  case STAT_DODGE_RATING:      return "dodge_rating";
  case STAT_PARRY_RATING:      return "parry_rating";

  case STAT_BLOCK_RATING: return "block_rating";

  case STAT_MASTERY_RATING: return "mastery_rating";

  case STAT_POWER: return "power";
  case STAT_FORCE_POWER: return "forcepower";

  case STAT_MAX: return "all";
  }
  return "unknown";
}

// util_t::stat_type_abbrev =================================================

const char* util_t::stat_type_abbrev( int stat )
{
  switch ( stat )
  {
  case STAT_STRENGTH:  return "Str";
  case STAT_AGILITY:   return "Agi";
  case STAT_STAMINA:   return "Sta";
  case STAT_INTELLECT: return "Int";
  case STAT_SPIRIT:    return "Spi";
  case STAT_WILLPOWER: return "Willpower";

  case STAT_HEALTH: return "Health";
  case STAT_MANA:   return "Mana";
  case STAT_RAGE:   return "Rage";
  case STAT_ENERGY: return "Energy";
  case STAT_AMMO:   return "Ammo";

  case STAT_SPELL_POWER:       return "SP";
  case STAT_SPELL_PENETRATION: return "SPen";
  case STAT_MP5:               return "MP5";

  case STAT_ATTACK_POWER:             return "AP";
  case STAT_EXPERTISE_RATING:         return "Exp";
  case STAT_EXPERTISE_RATING2:        return "InvExp";

  case STAT_HIT_RATING:   return "Hit";
  case STAT_HIT_RATING2:  return "InvHit";
  case STAT_CRIT_RATING:  return "Crit";
  case STAT_HASTE_RATING: return "Haste";

  case STAT_WEAPON_DPS:   return "Wdps";
  case STAT_WEAPON_SPEED: return "Wspeed";

  case STAT_WEAPON_OFFHAND_DPS:    return "WOHdps";
  case STAT_WEAPON_OFFHAND_SPEED:  return "WOHspeed";

  case STAT_ARMOR:             return "Armor";
  case STAT_BONUS_ARMOR:       return "BArmor";
  case STAT_RESILIENCE_RATING: return "Resil";
  case STAT_DODGE_RATING:      return "Dodge";
  case STAT_PARRY_RATING:      return "Parry";

  case STAT_BLOCK_RATING: return "BlockR";

  case STAT_MASTERY_RATING: return "Mastery";

  case STAT_POWER: return "Power";
  case STAT_FORCE_POWER: return "Force_Power";

  case STAT_MAX: return "All";
  }
  return "unknown";
}

// util_t::stat_type_wowhead ================================================

const char* util_t::stat_type_wowhead( int stat )
{
  switch ( stat )
  {
  case STAT_STRENGTH:  return "str";
  case STAT_AGILITY:   return "agi";
  case STAT_STAMINA:   return "sta";
  case STAT_INTELLECT: return "int";
  case STAT_SPIRIT:    return "spr";

  case STAT_HEALTH: return "health";
  case STAT_MANA:   return "mana";
  case STAT_RAGE:   return "rage";
  case STAT_ENERGY: return "energy";
  case STAT_AMMO:   return "ammo";

  case STAT_SPELL_POWER:       return "spellPower";
  case STAT_SPELL_PENETRATION: return "spellPen";

  case STAT_ATTACK_POWER:             return "attackPower";
  case STAT_EXPERTISE_RATING:         return "expertiseRating";

  case STAT_HIT_RATING:   return "hitRating";
  case STAT_CRIT_RATING:  return "critRating";
  case STAT_HASTE_RATING: return "hasteRating";

  case STAT_WEAPON_DPS:   return "__dps";
  case STAT_WEAPON_SPEED: return "__speed";

  case STAT_ARMOR:             return "armor";
  case STAT_BONUS_ARMOR:       return "__armor"; // FIXME! Does wowhead distinguish "bonus" armor?
  case STAT_RESILIENCE_RATING: return "resilRating";
  case STAT_DODGE_RATING:      return "dodgeRating";
  case STAT_PARRY_RATING:      return "parryRating";

  case STAT_MASTERY_RATING: return "masteryRating";

  case STAT_MAX: return "__all";
  }
  return "unknown";
}

// util_t::parse_stat_type ==================================================

stat_type util_t::parse_stat_type( const std::string& name )
{
  for ( int i=0; i <= STAT_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::stat_type_string( i ) ) )
      return ( stat_type ) i;

  for ( int i=0; i <= STAT_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::stat_type_abbrev( i ) ) )
      return ( stat_type ) i;

  for ( int i=0; i <= STAT_MAX; i++ )
    if ( util_t::str_compare_ci( name, util_t::stat_type_wowhead( i ) ) )
      return ( stat_type ) i;

  if ( name == "rgdcritstrkrtng" ) return STAT_CRIT_RATING;

  // in-case wowhead changes their mind again
  if ( name == "atkpwr"         ) return STAT_ATTACK_POWER;
  if ( name == "critstrkrtng"   ) return STAT_CRIT_RATING;
  if ( name == "dodgertng"      ) return STAT_DODGE_RATING;
  if ( name == "exprtng"        ) return STAT_EXPERTISE_RATING;
  if ( name == "hastertng"      ) return STAT_HASTE_RATING;
  if ( name == "hitrtng"        ) return STAT_HIT_RATING;
  if ( name == "mastrtng"       ) return STAT_MASTERY_RATING;
  if ( name == "parryrtng"      ) return STAT_PARRY_RATING;
  if ( name == "resiliencertng" ) return STAT_RESILIENCE_RATING;
  if ( name == "splpwr"         ) return STAT_SPELL_POWER;
  if ( name == "splpen"         ) return STAT_SPELL_PENETRATION;
  if ( name == "spi"            ) return STAT_SPIRIT;
  if ( util_t::str_compare_ci( name, "__wpds"   ) ) return STAT_WEAPON_DPS;
  if ( util_t::str_compare_ci( name, "__wspeed" ) ) return STAT_WEAPON_SPEED;

  return STAT_NONE;
}

// util_t::parse_reforge_type ===============================================

stat_type util_t::parse_reforge_type( const std::string& name )
{
  stat_type s = util_t::parse_stat_type( name );

  switch ( s )
  {
  case STAT_EXPERTISE_RATING:
  case STAT_HIT_RATING:
  case STAT_CRIT_RATING:
  case STAT_HASTE_RATING:
  case STAT_MASTERY_RATING:
  case STAT_SPIRIT:
  case STAT_DODGE_RATING:
  case STAT_PARRY_RATING:
    return s;
  default:
    return STAT_NONE;
  }
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

// util_t::class_id_mask ====================================================

int util_t::class_id_mask( int type )
{
  int cid = class_id( type );
  if ( cid <= 0 ) return 0;
  return 1 << ( cid - 1 );
}

// util_t::class_id =========================================================

int util_t::class_id( int type )
{
  return 0;
}

// util_t::race_id ==========================================================

unsigned util_t::race_id( int r )
{
  return 0;
}

// util_t::race_mask ========================================================

unsigned util_t::race_mask( int r )
{
  uint32_t id = race_id( r );

  if ( id > 0 )
    return ( 1 << ( id - 1 ) );

  return 0x00;
}

// util_t::pet_class_type ===================================================

player_type util_t::pet_class_type( int c )
{
  return PLAYER_PET;
}

// util_t::pet_mask =========================================================

unsigned util_t::pet_mask( int p )
{
  return 0x0;
}

// util_t::pet_id ===========================================================

unsigned util_t::pet_id( int p )
{
  uint32_t mask = pet_mask( p );

  switch ( mask )
  {
  case 0x1: return 1;
  case 0x2: return 2;
  case 0x4: return 3;
  }

  return 0;
}

// util_t::class_id_string ==================================================

const char* util_t::class_id_string( int type )
{
  return "0";
}

// util_t::translate_class_id ===============================================

int util_t::translate_class_id( int cid )
{
  return PLAYER_NONE;
}

// util_t::translate_race_id ================================================

race_type util_t::translate_race_id( int rid )
{
  return RACE_NONE;
}

// util_t::translate_item_mod ===============================================

stat_type util_t::translate_item_mod( int item_mod )
{
  switch ( item_mod )
  {
  case ITEM_MOD_AGILITY:             return STAT_AGILITY;
  case ITEM_MOD_STRENGTH:            return STAT_STRENGTH;
  case ITEM_MOD_INTELLECT:           return STAT_INTELLECT;
  case ITEM_MOD_SPIRIT:              return STAT_SPIRIT;
  case ITEM_MOD_STAMINA:             return STAT_STAMINA;
  case ITEM_MOD_DODGE_RATING:        return STAT_DODGE_RATING;
  case ITEM_MOD_PARRY_RATING:        return STAT_PARRY_RATING;
  case ITEM_MOD_BLOCK_RATING:        return STAT_BLOCK_RATING;
  case ITEM_MOD_HIT_RATING:          return STAT_HIT_RATING;
  case ITEM_MOD_CRIT_RATING:         return STAT_CRIT_RATING;
  case ITEM_MOD_HASTE_RATING:        return STAT_HASTE_RATING;
  case ITEM_MOD_EXPERTISE_RATING:    return STAT_EXPERTISE_RATING;
  case ITEM_MOD_ATTACK_POWER:        return STAT_ATTACK_POWER;
  case ITEM_MOD_RANGED_ATTACK_POWER: return STAT_ATTACK_POWER;
  case ITEM_MOD_SPELL_POWER:         return STAT_SPELL_POWER;
  case ITEM_MOD_MASTERY_RATING:      return STAT_MASTERY_RATING;
  case ITEM_MOD_EXTRA_ARMOR:         return STAT_BONUS_ARMOR;
  case ITEM_MOD_RESILIENCE_RATING:   return STAT_RESILIENCE_RATING;
  }

  return STAT_NONE;
}

// util_t::translate_weapon_subclass ========================================

weapon_type util_t::translate_weapon_subclass( item_subclass_weapon id )
{
  switch ( id )
  {
  case ITEM_SUBCLASS_WEAPON_AXE:          return WEAPON_AXE;
  case ITEM_SUBCLASS_WEAPON_AXE2:         return WEAPON_AXE_2H;
  case ITEM_SUBCLASS_WEAPON_BOW:          return WEAPON_BOW;
  case ITEM_SUBCLASS_WEAPON_GUN:          return WEAPON_GUN;
  case ITEM_SUBCLASS_WEAPON_MACE:         return WEAPON_MACE;
  case ITEM_SUBCLASS_WEAPON_MACE2:        return WEAPON_MACE_2H;
  case ITEM_SUBCLASS_WEAPON_POLEARM:      return WEAPON_POLEARM;
  case ITEM_SUBCLASS_WEAPON_SWORD:        return WEAPON_SWORD;
  case ITEM_SUBCLASS_WEAPON_SWORD2:       return WEAPON_SWORD_2H;
  case ITEM_SUBCLASS_WEAPON_STAFF:        return WEAPON_STAFF;
  case ITEM_SUBCLASS_WEAPON_FIST:         return WEAPON_FIST;
  case ITEM_SUBCLASS_WEAPON_DAGGER:       return WEAPON_DAGGER;
  case ITEM_SUBCLASS_WEAPON_THROWN:       return WEAPON_THROWN;
  case ITEM_SUBCLASS_WEAPON_CROSSBOW:     return WEAPON_CROSSBOW;
  case ITEM_SUBCLASS_WEAPON_WAND:         return WEAPON_WAND;
  default: return WEAPON_NONE;
  }

  return WEAPON_NONE;
}

// util_t::translate_invtype ================================================

slot_type util_t::translate_invtype( int inv_type )
{
  switch ( inv_type )
  {
  case INVTYPE_2HWEAPON:
  case INVTYPE_WEAPON:
  case INVTYPE_WEAPONMAINHAND:
    return SLOT_MAIN_HAND;
  case INVTYPE_WEAPONOFFHAND:
  case INVTYPE_SHIELD:
  case INVTYPE_HOLDABLE:
    return SLOT_OFF_HAND;
  case INVTYPE_THROWN:
  case INVTYPE_RELIC:
  case INVTYPE_RANGED:
  case INVTYPE_RANGEDRIGHT:
    return SLOT_RANGED;
  case INVTYPE_CHEST:
  case INVTYPE_ROBE:
    return SLOT_CHEST;
  case INVTYPE_CLOAK:
    return SLOT_BACK;
  case INVTYPE_FEET:
    return SLOT_FEET;
  case INVTYPE_FINGER:
    return SLOT_FINGER_1;
  case INVTYPE_HANDS:
    return SLOT_HANDS;
  case INVTYPE_HEAD:
    return SLOT_HEAD;
  case INVTYPE_LEGS:
    return SLOT_LEGS;
  case INVTYPE_NECK:
    return SLOT_NECK;
  case INVTYPE_SHOULDERS:
    return SLOT_SHOULDERS;
  case INVTYPE_TABARD:
    return SLOT_TABARD;
  case INVTYPE_TRINKET:
    return SLOT_TRINKET_1;
  case INVTYPE_WAIST:
    return SLOT_WAIST;
  case INVTYPE_WRISTS:
    return SLOT_WRISTS;
  }

  return SLOT_NONE;
}

// util_t::socket_gem_match =================================================

bool util_t::socket_gem_match( int socket,
                               int gem )
{
  if ( socket == GEM_NONE || gem == GEM_PRISMATIC ) return true;

  if ( socket == GEM_COGWHEEL && gem == GEM_COGWHEEL ) return true;

  if ( socket == GEM_META ) return ( gem == GEM_META );

  if ( socket == GEM_RED    ) return ( gem == GEM_RED    || gem == GEM_ORANGE || gem == GEM_PURPLE );
  if ( socket == GEM_YELLOW ) return ( gem == GEM_YELLOW || gem == GEM_ORANGE || gem == GEM_GREEN  );
  if ( socket == GEM_BLUE   ) return ( gem == GEM_BLUE   || gem == GEM_PURPLE || gem == GEM_GREEN  );

  return false;
}

// util_t::item_quality_string ==============================================

const char* util_t::item_quality_string( int quality )
{
  switch( quality )
  {
  case 1:   return "common";
  case 2:   return "uncommon";
  case 3:   return "rare";
  case 4:   return "epic";
  case 5:   return "legendary";
  default:  return "poor";
  }
}

// util_t::parse_item_quality ===============================================

int util_t::parse_item_quality( const std::string& quality )
{
  int i = 6;

  while ( --i > 0 )
    if ( str_compare_ci( quality, item_quality_string( i ) ) )
      break;

  return i;
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
      const char*  s =    str_splits[ i ].c_str();

      if      ( f == "i" ) *( ( int* )         va_arg( vap, int*    ) ) = atoi( s );
      else if ( f == "f" ) *( ( double* )      va_arg( vap, double* ) ) = atof( s );
      else if ( f == "d" ) *( ( double* )      va_arg( vap, double* ) ) = atof( s );
      else if ( f == "S" ) *( ( std::string* ) va_arg( vap, std::string* ) ) = s;
      else assert( 0 );
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
  char buffer[ 64 ];
  snprintf( buffer, sizeof( buffer ), "%.*f", precision, f );
  return std::string( buffer );
}

// util_t::to_string ========================================================

std::string util_t::to_string( double f )
{
  if ( std::abs( f - static_cast<int>( f ) ) < 0.001 )
    return to_string( static_cast<int>( f ) );
  else
    return to_string( f, 3 );
}

// util_t::milliseconds =====================================================

int64_t util_t::milliseconds()
{
  return 1000 * clock() / CLOCKS_PER_SEC;
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
  char *p_locale = util_t::dup( setlocale( LC_CTYPE, NULL ) );
  setlocale( LC_CTYPE, "" );

  int retcode = ::vfprintf( stream, format, args );

  setlocale( LC_CTYPE, p_locale );
  free( p_locale );

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

// util_t::snprintf =========================================================

int util_t::snprintf( char* buf, size_t size, const char* fmt, ... )
{
  va_list ap;
  va_start( ap, fmt );
  int rval = ::vsnprintf( buf, size, fmt, ap );
  va_end( ap );
  if ( rval >= 0 )
    assert( static_cast<size_t>( rval ) < size );
  return rval;
}


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
    {
      char enc_str[4];
      snprintf( enc_str, sizeof( enc_str ), "%%%02X", c );
      temp += enc_str;
    }
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
      long c = strtol( str.substr( i + 1, 2 ).c_str(), 0, 16 );
      if ( c ) temp += ( unsigned char ) c;
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
  std::string dest;
  for ( std::string::size_type i = 0, n = src.length(); i < n; ++i )
    dest.push_back( tolower( src[ i ] ) );
  return dest;
}

#if 0 // UNUSED
std::string trim( const std::string& src )
{
  std::string dest;

  std::string::size_type begin = src.find_first_not_of( ' ' );
  if ( begin != src.npos )
  {
    std::string::size_type end = src.find_last_not_of( ' ' );
    dest.assign( src, begin, end - begin );
  }

  return dest;
}

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
