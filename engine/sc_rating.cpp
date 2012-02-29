// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// ==========================================================================
// Rating
// ==========================================================================

namespace { // ANONYMOUS ====================================================

// Source: http://sithwarrior.com/forums/Thread-SWTOR-formula-list?pid=8298#pid8298
// Which he supposedly datamined with Nodeviewer.
const int base_health_table[] = {
  /*  0 */    0,
  /*  1 */  130,
  /*  2 */  140,
  /*  3 */  150,
  /*  4 */  160,
  /*  5 */  170,
  /*  6 */  185,
  /*  7 */  200,
  /*  8 */  220,
  /*  9 */  240,
  /* 10 */  260,
  /* 11 */  285,
  /* 12 */  310,
  /* 13 */  335,
  /* 14 */  365,
  /* 15 */  395,
  /* 16 */  430,
  /* 17 */  465,
  /* 18 */  500,
  /* 19 */  540,
  /* 20 */  580,
  /* 21 */  620,
  /* 22 */  665,
  /* 23 */  710,
  /* 24 */  755,
  /* 25 */  805,
  /* 26 */  855,
  /* 27 */  905,
  /* 28 */  960,
  /* 29 */ 1015,
  /* 30 */ 1070,
  /* 31 */ 1125,
  /* 32 */ 1185,
  /* 33 */ 1245,
  /* 34 */ 1305,
  /* 35 */ 1370,
  /* 36 */ 1435,
  /* 37 */ 1500,
  /* 38 */ 1570,
  /* 39 */ 1640,
  /* 40 */ 1710,
  /* 41 */ 1780,
  /* 42 */ 1855,
  /* 43 */ 1930,
  /* 44 */ 2005,
  /* 45 */ 2085,
  /* 46 */ 2165,
  /* 47 */ 2245,
  /* 48 */ 2330,
  /* 49 */ 2415,
  /* 50 */ 2500,
};

struct standard_health_t
{ int damage, healing; };

// Source: http://sithwarrior.com/forums/Thread-SWTOR-formula-list?pid=3547#pid3547
const standard_health_t standard_health_table[] = {
  /*  0 */ {    0,    0 },
  /*  1 */ {  180,  375 },
  /*  2 */ {  210,  430 },
  /*  3 */ {  240,  505 },
  /*  4 */ {  270,  585 },
  /*  5 */ {  305,  675 },
  /*  6 */ {  340,  770 },
  /*  7 */ {  380,  880 },
  /*  8 */ {  420, 1000 },
  /*  9 */ {  465, 1140 },
  /* 10 */ {  500, 1290 },
  /* 11 */ {  540, 1405 },
  /* 12 */ {  580, 1535 },
  /* 13 */ {  620, 1665 },
  /* 14 */ {  655, 1790 },
  /* 15 */ {  690, 1915 },
  /* 16 */ {  720, 2060 },
  /* 17 */ {  725, 2150 },
  /* 18 */ {  750, 2285 },
  /* 19 */ {  780, 2435 },
  /* 20 */ {  790, 2560 },
  /* 21 */ {  830, 2680 },
  /* 22 */ {  870, 2795 },
  /* 23 */ {  910, 2915 },
  /* 24 */ {  945, 3040 },
  /* 25 */ {  960, 3105 },
  /* 26 */ {  975, 3205 },
  /* 27 */ { 1005, 3300 },
  /* 28 */ { 1050, 3425 },
  /* 29 */ { 1085, 3535 },
  /* 30 */ { 1130, 3670 },
  /* 31 */ { 1175, 3915 },
  /* 32 */ { 1215, 4165 },
  /* 33 */ { 1220, 4355 },
  /* 34 */ { 1255, 4590 },
  /* 35 */ { 1280, 4830 },
  /* 36 */ { 1305, 5055 },
  /* 37 */ { 1320, 5265 },
  /* 38 */ { 1335, 5530 },
  /* 39 */ { 1350, 5715 },
  /* 40 */ { 1380, 5945 },
  /* 41 */ { 1400, 6080 },
  /* 42 */ { 1430, 6225 },
  /* 43 */ { 1440, 6320 },
  /* 44 */ { 1480, 6485 },
  /* 45 */ { 1490, 6565 },
  /* 46 */ { 1540, 6735 },
  /* 47 */ { 1545, 6785 },
  /* 48 */ { 1560, 6920 },
  /* 49 */ { 1575, 6970 },
  /* 50 */ { 1610, 7085 },
};

const standard_health_t& get_standard_health( int level )
{
  unsigned index = level;
  if ( index >= sizeof( standard_health_table ) / sizeof( standard_health_table[0] ) )
    index = 0;
  return standard_health_table[ index ];
}
} // ANONYMOUS namespace ====================================================

// rating_t::init ===========================================================

void rating_t::init( sim_t* sim, dbc_t& dbc, int level, int type )
{
  if ( sim -> debug ) log_t::output( sim, "rating_t::init: level=%d type=%s",
                                     level, util_t::player_type_string( type ) );

  if ( type == ENEMY || type == ENEMY_ADD )
  {
    double max = +1.0E+50;
    spell_alacrity    = max;
    spell_accuracy    = max;
    spell_crit        = max;
    attack_alacrity   = max;
    attack_accuracy   = max;
    attack_crit       = max;
    ranged_alacrity   = max;
    ranged_accuracy   = max;
    ranged_crit       = max;
    expertise         = max;
  }
  else
  {
    spell_alacrity    = dbc.combat_rating( RATING_SPELL_ALACRITY,  level );
    spell_accuracy    = dbc.combat_rating( RATING_SPELL_ACCURACY,    level );
    spell_crit        = dbc.combat_rating( RATING_SPELL_CRIT,   level );
    attack_alacrity   = dbc.combat_rating( RATING_MELEE_ALACRITY,  level );
    attack_accuracy   = dbc.combat_rating( RATING_MELEE_ACCURACY,    level );
    attack_crit       = dbc.combat_rating( RATING_MELEE_CRIT,   level );
    ranged_alacrity   = dbc.combat_rating( RATING_RANGED_ALACRITY, level );
    ranged_accuracy   = dbc.combat_rating( RATING_RANGED_ACCURACY,   level );
    ranged_crit       = dbc.combat_rating( RATING_RANGED_CRIT,  level );
    expertise         = dbc.combat_rating( RATING_EXPERTISE,    level );
  }
}

// rating_t::standardhealth_damage ==========================================

double rating_t::standardhealth_damage( int level )
{ return get_standard_health( level ).damage; }

// rating_t::standardhealth_healing =========================================

double rating_t::standardhealth_healing( int level )
{ return get_standard_health( level ).healing; }

// rating_t::get_base_health ================================================

int rating_t::get_base_health( int level )
{
  unsigned index = level;
  if ( index >= sizeof( base_health_table ) / sizeof( base_health_table[ 0 ] ) )
    index = 0;
  return base_health_table[ index ];
}

#if 0
// rating_t::interpolate ====================================================

double rating_t::interpolate( int    level,
                              double val_60,
                              double val_70,
                              double val_80,
                              double val_85 )
{
  if ( val_85 < 0 ) val_85 = val_80; // TODO
  if ( level <= 60 )
  {
    return val_60;
  }
  else if ( level == 70 )
  {
    return val_70;
  }
  else if ( level == 80 )
  {
    return val_80;
  }
  else if ( level >= 85 )
  {
    return val_85;
  }
  else if ( level < 70 )
  {
    // Assume linear progression for now.
    double adjust = ( level - 60 ) / 10.0;
    return val_60 + adjust * ( val_70 - val_60 );
  }
  else if ( level < 80 )
  {
    // Assume linear progression for now.
    double adjust = ( level - 70 ) / 10.0;
    return val_70 + adjust * ( val_80 - val_70 );
  }
  else // ( level < 85 )
  {
    // Assume linear progression for now.
    double adjust = ( level - 80 ) / 5.0;
    return val_80 + adjust * ( val_85 - val_80 );
  }
  assert( 0 );
  return 0;
}

// rating_t::get_attribute_base =============================================

double rating_t::get_attribute_base( sim_t* /* sim */, dbc_t& dbc, int level, player_type class_type, race_type race, base_stat_type stat_type )
{
  double res                       = 0.0;

  switch ( stat_type )
  {
  case BASE_STAT_STRENGTH:           res = dbc.race_base( race ).strength + dbc.attribute_base( class_type, level ).strength; break;
#if 0
  case BASE_STAT_AGILITY:            res = dbc.race_base( race ).agility + dbc.attribute_base( class_type, level ).agility; break;
  case BASE_STAT_STAMINA:            res = dbc.race_base( race ).stamina + dbc.attribute_base( class_type, level ).stamina; break;
  case BASE_STAT_INTELLECT:          res = dbc.race_base( race ).intellect + dbc.attribute_base( class_type, level ).intellect; break;
  case BASE_STAT_SPIRIT:             res = dbc.race_base( race ).spirit + dbc.attribute_base( class_type, level ).spirit;
                                     if ( race == RACE_HUMAN ) res *= 1.03; break;
  case BASE_STAT_MP5:                res = dbc.regen_base( class_type, level ); break;
#endif
  case BASE_STAT_HEALTH:             res = dbc.attribute_base( class_type, level ).base_health; break;
  case BASE_STAT_MANA:               res = dbc.attribute_base( class_type, level ).base_resource; break;
  case BASE_STAT_MELEE_CRIT_PER_AGI: res = dbc.melee_crit_scaling( class_type, level ); break;
  case BASE_STAT_SPELL_CRIT_PER_INT: res = dbc.spell_crit_scaling( class_type, level ); break;
  case BASE_STAT_DODGE_PER_AGI:      res = dbc.dodge_scaling( class_type, level ); break;
  case BASE_STAT_MELEE_CRIT:         res = dbc.melee_crit_base( class_type ); break;
  case BASE_STAT_SPELL_CRIT:         res = dbc.spell_crit_base( class_type ); break;
  case BASE_STAT_SPI_REGEN:          res = dbc.regen_spirit( class_type, level ); break;
  default: break;
  }

  return res;
}
#endif // 0
