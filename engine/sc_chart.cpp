// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

namespace { // ANONYMOUS NAMESPACE ==========================================

namespace color {
// http://www.brobstsystems.com/colors1.htm
const char* const blue = "2459FF";
const char* const cyan = "69CCF0";
const char* const green = "336600";
const char* const grey = "C0C0C0";
const char* const nearly_white = "FCFFFF";
const char* const pink = "F58CBA";
const char* const purple = "9482C9";
const char* const purple_dark = "7668A1";
const char* const red = "C41F3B";
const char* const taupe = "C79C6E";
const char* const white = "FFFFFF";
const char* const yellow = "FFCC00";
const char* const olive = "909000";
const char* const orange = "FF6F00";
const char* const teal = "009090";
}

static const char* class_color( player_type type )
{
  switch ( type )
  {
  case PLAYER_NONE:     return color::white;

  case SITH_MARAUDER:
  case JEDI_SENTINEL:   return color::red;

  case JEDI_SHADOW:
  case SITH_ASSASSIN:   return color::yellow;

  case JEDI_SAGE:       return color::purple;
  case SITH_SORCERER:   return color::purple_dark;

  case IA_SNIPER:
  case S_GUNSLINGER:    return color::teal;

  case S_SCOUNDREL:
  case IA_OPERATIVE:    return color::grey;

  case JEDI_GUARDIAN:
  case SITH_JUGGERNAUT: return color::blue;

  case T_COMMANDO:
  case BH_MERCENARY:    return color::orange;

  case T_VANGUARD:
  case BH_POWERTECH:    return color::olive;

  case ENEMY:           return color::white;
  case ENEMY_ADD:       return color::nearly_white;

  default: assert( 0 ); break;
  }

  return 0;
}

// The above colors don't all work for text rendered on a light (white) background.
// These colors work better by reducing the brightness HSV component of the above colors.
static const char* class_text_color( player_type type, int print_styles=0 )
{
  if ( !print_styles )
  {
    return class_color( type );
  }
  else
  {
    switch ( type )
    {
      case SITH_ASSASSIN: return "000000";

      default:         return class_color( type );
    }
  }
}

// These colors are picked to sort of line up with classes, but match the "feel" of the spell class' color
static const char* school_color( int type )
{
  switch ( type )
  {
  case SCHOOL_INTERNAL:     return color::purple;
  case SCHOOL_ELEMENTAL:  	return color::blue;
  case SCHOOL_KINETIC:     	return color::taupe;
  case SCHOOL_ENERGY: 	    return color::yellow;
  case SCHOOL_NONE:         return color::white;
  default: return "";
  }
  return 0;
}

static const char* stat_color( int type )
{
  switch ( type )
  {
  case STAT_STRENGTH:                 return class_color( JEDI_SENTINEL );  // Red
  case STAT_WILLPOWER:                return class_color( SITH_SORCERER );  // Purple (dark)
  case STAT_AIM:                      return class_color( BH_MERCENARY );   // Orange
  case STAT_CUNNING:                  return class_color( IA_OPERATIVE );   // Grey

  case STAT_ACCURACY_RATING:          return color::white;
  case STAT_CRIT_RATING:              return color::green;
  case STAT_ALACRITY_RATING:          return color::yellow;
  case STAT_SURGE_RATING:             return color::olive;
  case STAT_POWER:                    return color::blue;
  case STAT_FORCE_POWER:              return color::cyan;
  case STAT_TECH_POWER:               return color::cyan;
  case STAT_WEAPON_DMG:               return color::pink;
  case STAT_WEAPON_OFFHAND_DMG:       return color::purple;

  case STAT_EXPERTISE_RATING:         return color::red;
  case STAT_DEFENSE_RATING:           return color::white;
  case STAT_SHIELD_RATING:            return color::white;
  case STAT_ABSORB_RATING:            return color::white;
  default:                            return color::white;
  }
}

static const char* get_color( player_t* p )
{
  if ( p -> is_pet() )
  {
    return class_color( p -> cast_pet() -> owner -> type );
  }
  return class_color( p -> type );
}

static const char* get_text_color( player_t* p )
{
  if ( p -> is_pet() )
  {
    return class_text_color( p -> cast_pet() -> owner -> type, p -> sim -> print_styles );
  }
  return class_text_color ( p -> type, p -> sim -> print_styles );
}

static unsigned char simple_encoding( int number )
{
  if ( number < 0  ) number = 0;
  if ( number > 61 ) number = 61;

  static const char encoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";

  return encoding[ number ];
}

static const char* chart_resource_type_string( int type )
{
  switch ( type )
  {
  case RESOURCE_NONE:         return "None";
  case RESOURCE_HEALTH:       return "Health";
  case RESOURCE_MANA:         return "Mana";
  case RESOURCE_RAGE:         return "Rage";
  case RESOURCE_ENERGY:       return "Energy";
  case RESOURCE_HEAT:         return "Heat";
  case RESOURCE_AMMO:         return "Ammo";
  case RESOURCE_FORCE:        return "Force";
  }
  return "Unknown";
}

static const char* get_chart_base_url()
{
  static int round_robin = -1;
  static const char* base_urls[] =
  {
    "http://0.chart.apis.google.com/chart?",
    "http://1.chart.apis.google.com/chart?",
    "http://2.chart.apis.google.com/chart?",
    "http://3.chart.apis.google.com/chart?",
    "http://4.chart.apis.google.com/chart?",
    "http://5.chart.apis.google.com/chart?",
    "http://6.chart.apis.google.com/chart?",
    "http://7.chart.apis.google.com/chart?",
    "http://8.chart.apis.google.com/chart?",
    "http://9.chart.apis.google.com/chart?",
  };
  round_robin = ( round_robin + 1 ) % sizeof_array( base_urls );

  return base_urls[ round_robin ];
}

#if 0
static std::string extended_encoding( int number )
{
  static const char encoding[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-.";

  if ( number < 0    ) number = 0;
  if ( number > 4095 ) number = 4095;

  std::string pair;

  pair += encoding[ number / 64 ];
  pair += encoding[ number % 64 ];

  return pair;
}
#endif

} // ANONYMOUS NAMESPACE =====================================================

// ==========================================================================
// Chart
// ==========================================================================

// chart_t::raid_dps ========================================================

int chart_t::raid_aps( std::vector<std::string>& images,
                       sim_t* sim,
                       std::vector<player_t*> players_by_aps,
                       bool dps )
{
  int num_players = static_cast<int>( players_by_aps.size() );

  if ( num_players == 0 )
    return 0;

  double max_aps = 0;
  if ( dps )
    max_aps = players_by_aps[ 0 ] -> dps.mean;
  else
    max_aps = players_by_aps[ 0 ] -> hps.mean;

  std::string s;
  char buffer[ 1024 ];
  bool first = true;

  std::vector<player_t*> player_list = players_by_aps;
  int max_players = MAX_PLAYERS_PER_CHART;

  // Ommit Player with 0 DPS/HPS
  for ( int i=0; i < num_players; i++ )
  {
    player_t* p = player_list[ i ];
    if ( dps ? p -> dps.mean <= 0 : p -> hps.mean <=0 )
    {
      player_list.resize( i );
      break;
    }
  }
  num_players = static_cast<int>( player_list.size() );

  if ( num_players == 0 )
    return 0;

  while ( true )
  {
    if ( num_players > max_players ) num_players = max_players;

    s = get_chart_base_url();
    snprintf( buffer, sizeof( buffer ), "chs=525x%d", num_players * 20 + ( first ? 20 : 0 ) ); s += buffer;
    s += "&amp;";
    s += "cht=bhg";
    s += "&amp;";
    if ( ! sim -> print_styles )
    {
      s += "chf=bg,s,333333";
      s += "&amp;";
    }
    s += "chbh=15";
    s += "&amp;";
    s += "chd=t:";

    for ( int i=0; i < num_players; i++ )
    {
      player_t* p = player_list[ i ];
      snprintf( buffer, sizeof( buffer ), "%s%.0f", ( i?"|":"" ), dps ? p -> dps.mean : p -> hps.mean ); s += buffer;
    }
    s += "&amp;";
    snprintf( buffer, sizeof( buffer ), "chds=0,%.0f", max_aps * 2.5 ); s += buffer;
    s += "&amp;";
    s += "chco=";
    for ( int i=0; i < num_players; i++ )
    {
      if ( i ) s += ",";
      s += get_color( player_list[ i ] );
    }
    s += "&amp;";
    s += "chm=";
    for ( int i=0; i < num_players; i++ )
    {
      player_t* p = player_list[ i ];
      std::string formatted_name;
      http_t::format( formatted_name, p -> name_str );
      snprintf( buffer, sizeof( buffer ), "%st++%.0f++%s,%s,%d,0,15", ( i?"|":"" ), dps ? p -> dps.mean : p -> hps.mean, formatted_name.c_str(), get_text_color( p ), i ); s += buffer;
    }
    s += "&amp;";
    if ( first )
    {
      snprintf( buffer, sizeof( buffer ), "chtt=%s Ranking", dps ? "DPS " : "HPS" ); s += buffer;
      s += "&amp;";
    }
    if ( sim -> print_styles )
    {
      s += "chts=666666,18";
    }
    else
    {
      s += "chts=dddddd,18";
    }

    images.push_back( s );

    first = false;
    player_list.erase( player_list.begin(), player_list.begin() + num_players );
    num_players = ( int ) player_list.size();
    if ( num_players == 0 ) break;
  }

  return ( int ) images.size();
}

// chart_t::raid_gear =======================================================

int chart_t::raid_gear( std::vector<std::string>& images,
                        sim_t* sim )
{
  int num_players = ( int ) sim -> players_by_dps.size();

  if ( num_players == 0 )
    return 0;

  std::vector<double> data_points[ STAT_MAX ];

  for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
  {
    for ( int j=0; j < num_players; j++ )
    {
      player_t* p = sim -> players_by_dps[ j ];

      data_points[ i ].push_back( ( p -> gear.   get_stat( i ) +
                                    p -> enchant.get_stat( i ) ) * gear_stats_t::stat_mod( i ) );
    }
  }

  double max_total=0;
  for ( int i=0; i < num_players; i++ )
  {
    double total=0;
    for ( stat_type j = STAT_NONE; j < STAT_MAX; j++ )
    {
      if ( ! stat_color( j ) ) continue;
      total += data_points[ j ][ i ];
    }
    if ( total > max_total ) max_total = total;
  }

  std::string s;
  char buffer[ 1024 ];
  bool first;

  std::vector<player_t*> player_list = sim -> players_by_dps;
  int max_players = MAX_PLAYERS_PER_CHART;

  while ( true )
  {
    if ( num_players > max_players ) num_players = max_players;

    int height = num_players * 20 + 30;

    if ( num_players <= 12 ) height += 70;

    s = get_chart_base_url();
    snprintf( buffer, sizeof( buffer ), "chs=525x%d", height ); s += buffer;
    s += "&amp;";
    s += "cht=bhs";
    s += "&amp;";
    if ( ! sim -> print_styles )
    {
      s += "chf=bg,s,333333";
      s += "&amp;";
    }
    s += "chbh=15";
    s += "&amp;";
    s += "chd=t:";
    first = true;
    for ( int i=0; i < STAT_MAX; i++ )
    {
      if ( ! stat_color( i ) ) continue;
      if ( ! first ) s += "|";
      first = false;
      for ( int j=0; j < num_players; j++ )
      {
        snprintf( buffer, sizeof( buffer ), "%s%.0f", ( j?",":"" ), data_points[ i ][ j ] ); s += buffer;
      }
    }
    s += "&amp;";
    snprintf( buffer, sizeof( buffer ), "chds=0,%.0f", max_total ); s += buffer;
    s += "&amp;";
    s += "chco=";
    first = true;
    for ( int i=0; i < STAT_MAX; i++ )
    {
      if ( ! stat_color( i ) ) continue;
      if ( ! first ) s += ",";
      first = false;
      s += stat_color( i );
    }
    s += "&amp;";
    s += "chxt=y";
    s += "&amp;";
    s += "chxl=0:";
    for ( int i = num_players-1; i >= 0; i-- )
    {
      std::string formatted_name = player_list[ i ] -> name_str;
      util_t::urlencode( util_t::str_to_utf8( formatted_name ) );

      s += "|";
      s += formatted_name;
    }
    s += "&amp;";
    if ( sim -> print_styles )
    {
      s += "chxs=0,000000,14";
    }
    else
    {
      s += "chxs=0,dddddd,14";
    }
    s += "&amp;";
    s += "chdl=";
    first = true;
    for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
    {
      if ( ! stat_color( i ) ) continue;
      if ( ! first ) s += "|";
      first = false;
      s += util_t::stat_type_abbrev( i );
    }
    s += "&amp;";
    if ( num_players <= 12 )
    {
      s += "chdlp=t&amp;";
    }
    if ( ! sim -> print_styles )
    {
      s += "chdls=dddddd,12";
      s += "&amp;";
    }
    s += "chtt=Gear+Overview";
    s += "&amp;";
    if ( sim -> print_styles )
    {
      s += "chts=666666,18";
    }
    else
    {
      s += "chts=dddddd,18";
    }

    images.push_back( s );

    for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
    {
      std::vector<double>& c = data_points[ i ];
      c.erase( c.begin(), c.begin() + num_players );
    }

    player_list.erase( player_list.begin(), player_list.begin() + num_players );
    num_players = ( int ) player_list.size();
    if ( num_players == 0 ) break;
  }

  return ( int ) images.size();
}

// chart_t::raid_downtime ===================================================

struct compare_downtime
{
  bool operator()( player_t* l, player_t* r ) const
  {
    return l -> waiting_time.mean > r -> waiting_time.mean;
  }
};

// chart_t::raid_downtime ===================================================

const char* chart_t::raid_downtime( std::string& s,
                                    sim_t* sim )
{
  int num_players = ( int ) sim -> players_by_name.size();

  if ( num_players == 0 )
    return 0;

  std::vector<player_t*> waiting_list;

  for ( int i=0; i < num_players; i++ )
  {
    player_t* p = sim -> players_by_name[ i ];
    if ( ( p -> waiting_time.mean / p -> fight_length.mean ) > 0.01 )
    {
      waiting_list.push_back( p );
    }
  }

  int num_waiting = ( int ) waiting_list.size();
  if ( num_waiting == 0 ) return 0;

  boost::sort( waiting_list, compare_downtime() );

  char buffer[ 1024 ];

  s = get_chart_base_url();
  snprintf( buffer, sizeof( buffer ), "chs=500x%d", num_waiting * 30 + 30 ); s += buffer;
  s += "&amp;";
  s += "cht=bhg";
  s += "&amp;";
  if ( ! sim -> print_styles )
  {
    s += "chf=bg,s,333333";
    s += "&amp;";
  }
  s += "chd=t:";
  double max_waiting=0;
  for ( int i=0; i < num_waiting; i++ )
  {
    player_t* p = waiting_list[ i ];
    double waiting = 100.0 * p -> waiting_time.mean / p -> fight_length.mean;
    if ( waiting > max_waiting ) max_waiting = waiting;
    snprintf( buffer, sizeof( buffer ), "%s%.0f", ( i?"|":"" ), waiting ); s += buffer;
  }
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chds=0,%.0f", max_waiting * 1.9 ); s += buffer;
  s += "&amp;";
  s += "chco=";
  for ( int i=0; i < num_waiting; i++ )
  {
    if ( i ) s += ",";
    s += get_color( waiting_list[ i ] );
  }
  s += "&amp;";
  s += "chm=";
  for ( int i=0; i < num_waiting; i++ )
  {
    player_t* p = waiting_list[ i ];
    std::string formatted_name = p -> name_str;
    util_t::urlencode( util_t::str_to_utf8( formatted_name ) );
    snprintf( buffer, sizeof( buffer ), "%st++%.0f%%++%s,%s,%d,0,15", ( i?"|":"" ), 100.0 * p -> waiting_time.mean / p -> fight_length.mean, formatted_name.c_str(), get_text_color( p ), i ); s += buffer;
  }
  s += "&amp;";
  s += "chtt=Player+Waiting-Time";
  s += "&amp;";
  if ( sim -> print_styles )
  {
    s += "chts=666666,18";
  }
  else
  {
    s += "chts=dddddd,18";
  }

  return s.c_str();
}

// chart_t::raid_dpet =======================================================

struct compare_dpet
{
  bool operator()( stats_t* l, stats_t* r ) const
  {
    return l -> apet > r -> apet;
  }
};

int chart_t::raid_dpet( std::vector<std::string>& images,
                        sim_t* sim )
{
  int num_players = ( int ) sim -> players_by_dps.size();

  if ( num_players == 0 )
    return 0;

  std::vector<stats_t*> stats_list;

  for ( int i=0; i < num_players; i++ )
  {
    player_t* p = sim -> players_by_dps[ i ];

    for ( stats_t* st = p -> stats_list; st; st = st -> next )
    {
      if ( st -> quiet ) continue;
      if ( st -> apet <= 0 ) continue;
      if ( st -> apet > ( 5 * p -> dps.mean ) ) continue;
      if ( ( p -> primary_role() == ROLE_HEAL ) != ( st -> type != STATS_DMG ) ) continue;

      stats_list.push_back( st );
    }
  }

  int num_stats = ( int ) stats_list.size();
  if ( num_stats == 0 ) return 0;

  boost::sort( stats_list, compare_dpet() );

  double max_dpet = stats_list[ 0 ] -> apet;

  int max_actions_per_chart = 20;
  int max_charts = 4;

  std::string s;
  char buffer[ 1024 ];

  for ( int chart=0; chart < max_charts; chart++ )
  {
    if ( num_stats > max_actions_per_chart ) num_stats = max_actions_per_chart;

    s = get_chart_base_url();
    snprintf( buffer, sizeof( buffer ), "chs=500x%d", num_stats * 15 + ( chart == 0 ? 20 : -10 ) ); s += buffer;
    s += "&amp;";
    s += "cht=bhg";
    s += "&amp;";
    if ( ! sim -> print_styles )
    {
      s += "chf=bg,s,333333";
      s += "&amp;";
    }
    s += "chbh=10";
    s += "&amp;";
    s += "chd=t:";
    for ( int i=0; i < num_stats; i++ )
    {
      stats_t* st = stats_list[ i ];
      snprintf( buffer, sizeof( buffer ), "%s%.0f", ( i?"|":"" ), st -> apet ); s += buffer;
    }
    s += "&amp;";
    snprintf( buffer, sizeof( buffer ), "chds=0,%.0f", max_dpet * 2.5 ); s += buffer;
    s += "&amp;";
    s += "chco=";
    for ( int i=0; i < num_stats; i++ )
    {
      if ( i ) s += ",";
      s += school_color ( stats_list[ i ] -> school );
    }
    s += "&amp;";
    s += "chm=";
    for ( int i=0; i < num_stats; i++ )
    {
      stats_t* st = stats_list[ i ];
      std::string formatted_name = st -> player -> name_str;
      util_t::urlencode( util_t::str_to_utf8( formatted_name ) );

      snprintf( buffer, sizeof( buffer ), "%st++%.0f++%s+(%s),%s,%d,0,10", ( i?"|":"" ),
                st -> apet, st -> name_str.c_str(), formatted_name.c_str(), get_text_color( st -> player ), i ); s += buffer;
    }
    s += "&amp;";
    if ( chart==0 )
    {
      s += "chtt=Raid+Damage+Per+Execute+Time";
      s += "&amp;";
    }
    if ( sim -> print_styles )
    {
      s += "chts=666666,18";
    }
    else
    {
      s += "chts=dddddd,18";
    }

    images.push_back( s );

    stats_list.erase( stats_list.begin(), stats_list.begin() + num_stats );
    num_stats = ( int ) stats_list.size();
    if ( num_stats == 0 ) break;
  }

  return ( int ) images.size();
}

// chart_t::action_dpet =====================================================

const char* chart_t::action_dpet( std::string& s,
                                  player_t* p )
{
  std::vector<stats_t*> stats_list;

  for ( stats_t* st = p -> stats_list; st; st = st -> next )
  {
    if ( st -> quiet ) continue;
    if ( st -> apet <= 0 ) continue;
    if ( st -> apet > ( 5 * ( ( p -> primary_role() == ROLE_HEAL ) ? p -> hps.mean : p -> dps.mean ) ) ) continue;
    if ( ( p -> primary_role() == ROLE_HEAL ) != ( st -> type != STATS_DMG ) ) continue;

    stats_list.push_back( st );
  }

  for ( pet_t* pet = p -> pet_list; pet; pet = pet -> next_pet )
  {
    for ( stats_t* st = pet -> stats_list; st; st = st -> next )
    {
      if ( st -> quiet ) continue;
      if ( st -> apet <= 0 ) continue;
      if ( st -> apet > ( 5 * ( ( p -> primary_role() == ROLE_HEAL ) ? p -> hps.mean : p -> dps.mean ) ) ) continue;
      if ( ( p -> primary_role() == ROLE_HEAL ) != ( st -> type != STATS_DMG ) ) continue;

      stats_list.push_back( st );
    }
  }

  int num_stats = ( int ) stats_list.size();
  if ( num_stats == 0 ) return 0;

  boost::sort( stats_list, compare_dpet() );

  char buffer[ 1024 ];

  s = get_chart_base_url();
  snprintf( buffer, sizeof( buffer ), "chs=550x%d", num_stats * 30 + 30 ); s += buffer;
  s += "&amp;";
  s += "cht=bhg";
  s += "&amp;";
  if ( ! p -> sim -> print_styles )
  {
    s += "chf=bg,s,333333";
    s += "&amp;";
  }
  s += "chd=t:";
  double max_apet=0;
  for ( int i=0; i < num_stats; i++ )
  {
    stats_t* st = stats_list[ i ];
    snprintf( buffer, sizeof( buffer ), "%s%.0f", ( i?"|":"" ), st -> apet ); s += buffer;
    if ( st -> apet > max_apet ) max_apet = st -> apet;
  }
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chds=0,%.0f", max_apet * 2 ); s += buffer;
  s += "&amp;";
  s += "chco=";
  for ( int i=0; i < num_stats; i++ )
  {
    if ( i ) s += ",";


    std::string school = school_color( stats_list[ i ] -> school );
    if ( school.empty() )
    {
      p -> sim -> errorf( "chart_t::action_dpet assertion error! School color unknown, stats %s from %s.\n", stats_list[ i ] -> name_str.c_str(), p -> name() );
      assert( 0 );
    }
    s += school;
  }
  s += "&amp;";
  s += "chm=";
  for ( int i=0; i < num_stats; i++ )
  {
    stats_t* st = stats_list[ i ];
    snprintf( buffer, sizeof( buffer ), "%st++%.0f++%s,%s,%d,0,15", ( i?"|":"" ), st -> apet, st -> name_str.c_str(), school_color( st -> school ), i ); s += buffer;
  }
  s += "&amp;";
  std::string formatted_name = p -> name_str;
  util_t::urlencode( util_t::str_to_utf8( formatted_name ) );
  snprintf( buffer, sizeof( buffer ), "chtt=%s+Damage+Per+Execute+Time", formatted_name.c_str() ); s += buffer;
  s += "&amp;";
  if ( p -> sim -> print_styles )
  {
    s += "chts=666666,18";
  }
  else
  {
    s += "chts=dddddd,18";
  }

  return s.c_str();
}

// chart_t::action_dmg ======================================================

struct compare_amount
{
  bool operator()( stats_t* l, stats_t* r ) const
  {
    return l -> actual_amount.mean > r -> actual_amount.mean;
  }
};

const char* chart_t::action_dmg( std::string& s,
                                 player_t* p )
{
  std::vector<stats_t*> stats_list;

  for ( stats_t* st = p -> stats_list; st; st = st -> next )
  {
    if ( st -> quiet ) continue;
    if ( st -> total_amount.mean <= 0 ) continue;
    if ( ( p -> primary_role() == ROLE_HEAL ) != ( st -> type != STATS_DMG ) ) continue;
    stats_list.push_back( st );
  }

  for ( pet_t* pet = p -> pet_list; pet; pet = pet -> next_pet )
  {
    for ( stats_t* st = pet -> stats_list; st; st = st -> next )
    {
      if ( st -> quiet ) continue;
      if ( st -> total_amount.mean <= 0 ) continue;
      if ( ( p -> primary_role() == ROLE_HEAL ) != ( st -> type != STATS_DMG ) ) continue;
      stats_list.push_back( st );
    }
  }

  int num_stats = ( int ) stats_list.size();
  if ( num_stats == 0 ) return 0;

  boost::sort( stats_list, compare_amount() );

  char buffer[ 1024 ];

  s = get_chart_base_url();
  snprintf( buffer, sizeof( buffer ), "chs=550x%d", 200 + num_stats * 10 ); s += buffer;
  s += "&amp;";
  s += "cht=p";
  s += "&amp;";
  if ( ! p -> sim -> print_styles )
  {
    s += "chf=bg,s,333333";
    s += "&amp;";
  }
  s += "chd=t:";
  for ( int i=0; i < num_stats; i++ )
  {
    stats_t* st = stats_list[ i ];
    snprintf( buffer, sizeof( buffer ), "%s%.0f", ( i?",":"" ), 100.0 * st -> actual_amount.mean / ( ( p -> primary_role() == ROLE_HEAL ) ? p -> heal.mean : p -> dmg.mean ) ); s += buffer;
  }
  s += "&amp;";
  s += "chds=0,100";
  s += "&amp;";
  s += "chdls=ffffff";
  s += "&amp;";
  s += "chco=";
  for ( int i=0; i < num_stats; i++ )
  {
    if ( i ) s += ",";

    std::string school = school_color( stats_list[ i ] -> school );
    if ( school.empty() )
    {
      p -> sim -> errorf( "chart_t::action_dmg assertion error! School unknown, stats %s from %s.\n", stats_list[ i ] -> name_str.c_str(), p -> name() );
      assert( 0 );
    }
    s += school;

  }
  s += "&amp;";
  s += "chl=";
  for ( int i=0; i < num_stats; i++ )
  {
    if ( i ) s += "|";
    s += stats_list[ i ] -> name_str.c_str();
  }
  s += "&amp;";
  std::string formatted_name = p -> name();
  util_t::urlencode( util_t::str_to_utf8( formatted_name ) );
  snprintf( buffer, sizeof( buffer ), "chtt=%s+%s+Sources", formatted_name.c_str(),
            ( p -> primary_role() == ROLE_HEAL ? "Healing" : "Damage" ) );
  s += buffer;
  s += "&amp;";
  if ( p -> sim -> print_styles )
  {
    s += "chts=666666,18";
  }
  else
  {
    s += "chts=dddddd,18";
  }

  return s.c_str();
}

// chart_t::spent_time ======================================================

namespace {

struct compare_time
{
  bool operator()( const stats_t* l, const stats_t* r ) const
  {
    return l -> total_time > r -> total_time;
  }
};

}

const char* chart_t::time_spent( std::string& s,
                                 player_t* p )
{
  std::vector<stats_t*> stats_list;

  for ( stats_t* st = p -> stats_list; st; st = st -> next )
  {
    if ( st -> quiet ) continue;
    if ( st -> total_time <= timespan_t::zero() ) continue;
    if ( st -> background ) continue;

    stats_list.push_back( st );
  }

  assert( stats_list.size() <= static_cast<std::size_t>( std::numeric_limits<int>::max() ) );
  int num_stats = static_cast<int>( stats_list.size() );
  if ( num_stats == 0 && p -> waiting_time.mean == 0 )
    return 0;

  boost::sort( stats_list, compare_time() );

  char buffer[ 1024 ];

  s = get_chart_base_url();
  snprintf( buffer, sizeof( buffer ), "chs=525x%d", 200 + num_stats * 10 ); s += buffer;
  s += "&amp;";
  s += "cht=p";
  s += "&amp;";
  if ( ! p -> sim -> print_styles )
  {
    s += "chf=bg,s,333333";
    s += "&amp;";
  }
  s += "chd=t:";
  for ( int i=0; i < num_stats; i++ )
  {
    snprintf( buffer, sizeof( buffer ), "%s%.1f", ( i?",":"" ), 100.0 * to_seconds( stats_list[ i ] -> total_time ) / p -> fight_length.mean ); s += buffer;
  }
  if ( p -> waiting_time.mean > 0 )
  {
    snprintf( buffer, sizeof( buffer ), "%s%.1f", ( num_stats > 0 ? ",":"" ), 100.0 * p -> waiting_time.mean / p -> fight_length.mean ); s += buffer;
  }
  s += "&amp;";
  s += "chds=0,100";
  s += "&amp;";
  s += "chdls=ffffff";
  s += "&amp;";
  s += "chco=";
  for ( int i=0; i < num_stats; i++ )
  {
    if ( i ) s += ",";

    const char* school = school_color( stats_list[ i ] -> school );
    if ( ! *school )
    {
      p -> sim -> errorf( "chart_t::time_spent assertion error! School unknown, stats %s from %s.\n", stats_list[ i ] -> name_str.c_str(), p -> name() );
      assert( 0 );
    }
    s += school;
  }
  if ( p -> waiting_time.mean > 0 )
  {
    if ( num_stats > 0 ) s += ",";
    s += "ffffff";
  }
  s += "&amp;";
  s += "chl=";
  for ( int i=0; i < num_stats; i++ )
  {
    stats_t* st = stats_list[ i ];
    if ( i ) s += "|";
    s += st -> name_str.c_str();
    snprintf( buffer, sizeof( buffer ), " %.1fs", to_seconds( st -> total_time ) ); s += buffer;
  }
  if ( p -> waiting_time.mean > 0 )
  {
    if ( num_stats > 0 )s += "|";
    s += "waiting";
    snprintf( buffer, sizeof( buffer ), " %.1fs", p -> waiting_time.mean ); s += buffer;
  }
  s += "&amp;";
  std::string formatted_name = p -> name();
  util_t::urlencode( util_t::str_to_utf8( formatted_name ) );
  snprintf( buffer, sizeof( buffer ), "chtt=%s+Spent Time", formatted_name.c_str() );
  s += buffer;
  s += "&amp;";
  if ( p -> sim -> print_styles )
  {
    s += "chts=666666,18";
  }
  else
  {
    s += "chts=dddddd,18";
  }

  return s.c_str();
}

// chart_t::gains ===========================================================

struct compare_gain
{
  bool operator()( gain_t* l, gain_t* r ) const
  {
    return l -> actual > r -> actual;
  }
};

const char* chart_t::gains( std::string& s,
                            player_t* p, resource_type type )
{
  std::vector<gain_t*> gains_list;

  double total_gain=0;
  for ( gain_t* g = p -> gain_list; g; g = g -> next )
  {
    if ( g -> actual <= 0 ) continue;
    if ( g -> type != type ) continue;
    total_gain += g -> actual;
    gains_list.push_back( g );
  }

  int num_gains = ( int ) gains_list.size();
  if ( num_gains == 0 ) return 0;

  boost::sort( gains_list, compare_gain() );

  char buffer[ 1024 ];

  s = get_chart_base_url();
  snprintf( buffer, sizeof( buffer ), "chs=550x%d", 200 + num_gains * 10 ); s += buffer;
  s += "&amp;";
  s += "cht=p";
  s += "&amp;";
  if ( ! p -> sim -> print_styles )
  {
    s += "chf=bg,s,333333";
    s += "&amp;";
  }
  s += "chd=t:";
  for ( int i=0; i < num_gains; i++ )
  {
    gain_t* g = gains_list[ i ];
    snprintf( buffer, sizeof( buffer ), "%s%d", ( i?",":"" ), ( int ) floor( 100.0 * g -> actual / total_gain + 0.5 ) ); s += buffer;
  }
  s += "&amp;";
  s += "chds=0,100";
  s += "&amp;";
  s += "chco=";
  s += resource_color( type );
  s += "&amp;";
  s += "chl=";
  for ( int i=0; i < num_gains; i++ )
  {
    if ( i ) s += "|";
    s += gains_list[ i ] -> name();
  }
  s += "&amp;";
  std::string formatted_name = p -> name_str;
  util_t::urlencode( util_t::str_to_utf8( formatted_name ) );
  snprintf( buffer, sizeof( buffer ), "chtt=%s+%s+Gains", formatted_name.c_str(), chart_resource_type_string( type ) ); s += buffer;
  s += "&amp;";
  if ( p -> sim -> print_styles )
  {
    s += "chts=666666,18";
  }
  else
  {
    s += "chts=dddddd,18";
  }

  return s.c_str();
}

// chart_t::scale_factors ===================================================

struct compare_scale_factors
{
  const gear_stats_t& scale_factors;
  compare_scale_factors( const gear_stats_t& s ) : scale_factors( s ) {}
  bool operator()( stat_type l, stat_type r ) const
  {
    return scale_factors.get_stat( l ) >
    scale_factors.get_stat( r );
  }
};

const char* chart_t::scale_factors( std::string& s,
                                    player_t* p, const std::string& scale_factors_name, const gear_stats_t& scale_factors, const gear_stats_t& scale_factors_error )
{
  std::vector<stat_type> scaling_stats;

  for ( stat_type i = STAT_NONE; i < STAT_MAX; ++i )
  {
    if ( p -> scales_with[ i ] && scale_factors.get_stat( i ) > 0 )
      scaling_stats.push_back( i );
  }

  int num_scaling_stats = scaling_stats.size();
  if ( ! num_scaling_stats ) return 0;

  boost::sort( scaling_stats, compare_scale_factors( scale_factors ) );

  double max_scale_factor = scale_factors.get_stat( scaling_stats[ 0 ] );

  char buffer[ 1024 ];

  s = get_chart_base_url();
  snprintf( buffer, sizeof( buffer ), "chs=525x%d", num_scaling_stats * 30 + 30 ); s += buffer;
  s += "&amp;";
  s += "cht=bhg";
  s += "&amp;";
  s += "chxs=0,ffffff|1,ffffff";
  s += "&amp;";
  if ( ! p -> sim -> print_styles )
  {
    s += "chf=bg,s,333333";
    s += "&amp;";
  }
  snprintf( buffer, sizeof( buffer ), "chd=t%i:" , 1 ); s += buffer;
  for ( int i=0; i < num_scaling_stats; i++ )
  {
    double factor = scale_factors.get_stat( scaling_stats[ i ] );
    snprintf( buffer, sizeof( buffer ), "%s%.*f", ( i?",":"" ), p -> sim -> report_precision, factor ); s += buffer;
  }
  s += "|";
  for ( int i=0; i < num_scaling_stats; i++ )
  {
    double factor = std::max( scale_factors.get_stat( scaling_stats[ i ] ) - scale_factors_error.get_stat( scaling_stats[ i ] ), 0.0 );
    snprintf( buffer, sizeof( buffer ), "%s%.*f", ( i?",":"" ), p -> sim -> report_precision, factor ); s += buffer;
  }
  s += "|";
  for ( int i=0; i < num_scaling_stats; i++ )
  {
    double factor = scale_factors.get_stat( scaling_stats[ i ] ) + scale_factors_error.get_stat( scaling_stats[ i ] );
    snprintf( buffer, sizeof( buffer ), "%s%.*f", ( i?",":"" ), p -> sim -> report_precision, factor ); s += buffer;
  }
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chds=0,%.*f", p -> sim -> report_precision, max_scale_factor * 2 ); s += buffer;
  s += "&amp;";
  s += "chco=";
  s += class_color( p -> type );
  s += "&amp;";
  s += "chm=";
  snprintf( buffer, sizeof( buffer ), "E,FF0000,1:0,,1:20|" ); s += buffer;
  for ( int i=0; i < num_scaling_stats; i++ )
  {
    double factor = scale_factors.get_stat( scaling_stats[ i ] );
    const char* name = util_t::stat_type_abbrev( scaling_stats[ i ] );
    snprintf( buffer, sizeof( buffer ), "%st++++%.*f++%s,%s,0,%d,15,0.1", ( i?"|":"" ),
              p -> sim -> report_precision, factor, name, class_text_color( p -> type ), i ); s += buffer;
  }

  s += "&amp;";
  std::string formatted_name = scale_factors_name;
  util_t::urlencode( util_t::str_to_utf8( formatted_name ) );
  snprintf( buffer, sizeof( buffer ), "chtt=%s+Scale+Factors", formatted_name.c_str() ); s += buffer;
  s += "&amp;";
  if ( p -> sim -> print_styles )
  {
    s += "chts=666666,18";
  }
  else
  {
    s += "chts=dddddd,18";
  }

  return s.c_str();
}

// chart_t::scaling_dps =====================================================

const char* chart_t::scaling_dps( std::string& s,
                                  player_t* p )
{
  double max_dps=0, min_dps=std::numeric_limits<double>::max();

  for ( size_t i=0; i < sizeof_array( p -> dps_plot_data ); ++i )
  {
    std::vector<double>& pd = p -> dps_plot_data[ i ];
    size_t size = pd.size();
    for ( size_t j=0; j < size; j++ )
    {
      if ( pd[ j ] > max_dps ) max_dps = pd[ j ];
      if ( pd[ j ] < min_dps ) min_dps = pd[ j ];
    }
  }
  if ( max_dps <= 0 ) return 0;

  double step = p -> sim -> plot -> dps_plot_step;
  int range = p -> sim -> plot -> dps_plot_points / 2;
  const int start = 0;	// start and end only used for dps_plot_positive
  const int end = 2 * range;
  size_t num_points = 1 + 2 * range;

  char buffer[ 1024 ];

  s = get_chart_base_url();
  s += "chs=550x300";
  s += "&amp;";
  s += "cht=lc";
  s += "&amp;";
  if ( ! p -> sim -> print_styles )
  {
    s += "chf=bg,s,333333";
    s += "&amp;";
  }
  s += "chd=t:";
  bool first=true;
  for ( int i=0; i < STAT_MAX; i++ )
  {
    if ( ! stat_color( i ) ) continue;
    std::vector<double>& pd = p -> dps_plot_data[ i ];
    size_t size = pd.size();
    if ( size != num_points ) continue;
    if ( ! first ) s += "|";
    for ( size_t j=0; j < size; j++ )
    {
      snprintf( buffer, sizeof( buffer ), "%s%.0f", ( j?",":"" ), pd[ j ] ); s += buffer;
    }
    first = false;
  }
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chds=%.0f,%.0f", min_dps, max_dps ); s += buffer;
  s += "&amp;";
  s += "chxt=x,y";
  s += "&amp;";
  if ( ! p -> sim -> plot -> dps_plot_positive )
  {
    snprintf( buffer, sizeof( buffer ), "chxl=0:|%.0f|%.0f|0|%%2b%.0f|%%2b%.0f|1:|%.0f|%.0f|%.0f", ( -range*step ), ( -range*step )/2, ( +range*step )/2, ( +range*step ), min_dps, p -> dps.mean, max_dps ); s += buffer;
  }
  else
  {
    snprintf( buffer, sizeof( buffer ), "chxl=0:|0|%%2b%.0f|%%2b%.0f|%%2b%.0f|%%2b%.0f|1:|%.0f|%.0f|%.0f", ( start + ( 1.0/4 )*end )*step, ( start + ( 2.0/4 )*end )*step, ( start + ( 3.0/4 )*end )*step, ( start + end )*step, min_dps, p -> dps.mean, max_dps ); s += buffer;
  }
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chxp=0,0,24.5,50,74.5,100|1,1,%.0f,100", 100.0 * ( p -> dps.mean - min_dps ) / ( max_dps - min_dps ) ); s += buffer;
  s += "&amp;";
  s += "chdl=";
  first = true;
  for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
  {
    if ( ! stat_color( i ) ) continue;
    size_t size = p -> dps_plot_data[ i ].size();
    if ( size != num_points ) continue;
    if ( ! first ) s += "|";
    s += util_t::stat_type_abbrev( i );
    first = false;
  }
  s += "&amp;";
  if ( ! p -> sim -> print_styles )
  {
    s += "chdls=dddddd,12";
    s += "&amp;";
  }
  s += "chco=";
  first = true;
  for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
  {
    if ( ! stat_color( i ) ) continue;
    size_t size = p -> dps_plot_data[ i ].size();
    if ( size != num_points ) continue;
    if ( ! first ) s += ",";
    first = false;
    s += stat_color( i );
  }
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chg=%.4f,10,1,3", floor( 10000.0 * 100.0 / ( num_points - 1 ) ) / 10000.0 ); s += buffer;
  s += "&amp;";
  std::string formatted_name = p -> name_str;
  util_t::urlencode( util_t::str_to_utf8( formatted_name ) );
  snprintf( buffer, sizeof( buffer ), "chtt=%s+DPS+Scaling", formatted_name.c_str() ); s += buffer;
  s += "&amp;";
  if ( p -> sim -> print_styles )
  {
    s += "chts=666666,18";
  }
  else
  {
    s += "chts=dddddd,18";
  }

  return s.c_str();
}

// ternary_coords ===========================================================

std::array<double,2> ternary_coords( const std::vector<reforge_plot_data_t>& xyz )
{
  return std::array<double,2>{{
    xyz[ 2 ].value/2.0 + xyz[ 1 ].value,
    xyz[ 2 ].value/2.0 * sqrt( 3.0 )
  }};
}

// color_temperature_gradient ===============================================

std::string color_temperature_gradient( double n, double min, double range )
{
  char buffer[ 12 ];

  const unsigned red = static_cast<unsigned char>( floor( 255.0 * ( n - min ) / range ) );
  const unsigned blue = 255 - red;

  snprintf( buffer, sizeof( buffer ), "%.2X00%.2X", red, blue );
  return buffer;
}

// chart_t::reforge_dps =====================================================

const char* chart_t::reforge_dps( std::string& s,
                                  player_t* p )
{
  double dps_range=0, min_dps=FLT_MAX, max_dps=0;

  if ( ! p )
    return 0;
  std::vector< std::vector<reforge_plot_data_t> >& pd = p -> reforge_plot_data;
  if ( pd.size() == 0 )
    return 0;

  size_t num_stats = pd[ 0 ].size() - 1;
  if ( num_stats != 3 && num_stats != 2 )
  {
    p -> sim -> errorf( "You must choose 2 or 3 stats to generate a reforge plot.\n" );
    return 0;
  }

  for ( size_t i=0; i < pd.size(); i++ )
  {
    assert( num_stats < pd[ i ].size() );
    if ( pd[ i ][ num_stats ].value < min_dps )
      min_dps = pd[ i ][ num_stats ].value;
    if ( pd[ i ][ num_stats ].value > max_dps )
      max_dps = pd[ i ][ num_stats ].value;
  }

  dps_range = max_dps - min_dps;

  if ( num_stats == 2 )
  {
    int range = p -> sim -> reforge_plot -> reforge_plot_amount;
    int step = p -> sim -> reforge_plot -> reforge_plot_step;
    int num_points = ( int ) pd.size();
    const auto& stat_indices = p -> sim -> reforge_plot -> reforge_plot_stat_indices;
//    const reforge_plot_data_t& baseline = pd[ num_points / 2 ][ 2 ];
    const reforge_plot_data_t baseline = { p -> dps.mean, p -> dps_error };
    double min_delta = baseline.value - ( min_dps - baseline.error / 2 );
    double max_delta = ( max_dps + baseline.error / 2 ) - baseline.value;
    double max_ydelta = std::max( min_delta, max_delta );
    int ysteps = 5;
    double ystep_amount = max_ydelta / ysteps;

    char buffer[ 1024 ];

    s = get_chart_base_url();
    s += "chs=525x300";
    s += "&amp;";
    s += "cht=lxy";
    s += "&amp;";
    if ( ! p -> sim -> print_styles )
    {
      s += "chf=bg,s,333333";
      s += "&amp;";
    }

    // Fill gaps at the start of each series with null values. Error bars don't line up with main graph otherwise.
    std::string start_null_values = "";
    for (int i = pd[ 0 ][ 0 ].value; i > -range; i -= step)
      start_null_values += "_,";

    // Fill gaps at the end of each series with null values. Error bars don't line up with main graph otherwise.
    std::string end_null_values = "";
    for (int i = pd[ num_points-1 ][ 0 ].value; i < range; i += step)
      end_null_values += ",_";

    // Series
    s += "chd=t2:_|";

    // Y series
    s += start_null_values;
    for ( int i=0; i < num_points; i++ )
    {
      snprintf( buffer, sizeof( buffer ), "%.0f", pd[ i ][ 2 ].value );
      s += buffer;
      if ( i < num_points - 1 )
        s+= ",";
    }
    s += end_null_values;

    // Min Y series
    s += "|_|";
    s += start_null_values;
    for ( int i=0; i < num_points; i++ )
    {
      double v = pd[ i ][ 2 ].value - pd[ i ][ 2 ].error / 2;
      snprintf( buffer, sizeof( buffer ), "%.0f", v );
      s += buffer;
      if ( i < num_points - 1 )
        s+= ",";
    }
    s += end_null_values;

    // Max Y series
    s += "|_|";
    s += start_null_values;
    for ( int i=0; i < num_points; i++ )
    {
      double v = pd[ i ][ 2 ].value + pd[ i ][ 2 ].error / 2;
      snprintf( buffer, sizeof( buffer ), "%.0f", v );
      s += buffer;
      if ( i < num_points - 1 )
        s+= ",";
    }
    s += end_null_values;

    s += "&amp;";

    // Axis dimensions
    snprintf( buffer, sizeof( buffer ), "chds=%d,%d,%.0f,%.0f",
              -range, +range,
              floor( baseline.value - max_ydelta ),
              ceil( baseline.value + max_ydelta ) );
    s += buffer;
    s += "&amp;chxt=x,y,x&amp;";

    // X Axis labels
    snprintf( buffer, sizeof( buffer ), "chxl=0:|%d|%d|0|%d|%d|",
              ( range ), ( range ) / 2, ( range ) / 2, ( range ) );
    s += buffer;

    // Y Axis labels
    s += "1:|";
    for ( int i = ysteps; i >= 1; i -= 1 )
    {
      snprintf( buffer, sizeof( buffer ), "%.0f (%.0f)|",
                baseline.value - i * ystep_amount,
                - ( i * ystep_amount ) );
      s += buffer;
    }
    snprintf( buffer, sizeof( buffer ), "%.0f|", baseline.value );
    s += buffer;
    for ( int i = 1; i <= ysteps; i += 1 )
    {
      snprintf( buffer, sizeof( buffer ), "%.0f (%%2b%.0f)|",
                baseline.value + i * ystep_amount,
                i * ystep_amount );
      s += buffer;
    }

    // X2 Axis labels
    snprintf( buffer, sizeof( buffer ), "2:|%s to %s||%s to %s",
              util_t::stat_type_abbrev( stat_indices[ 0 ] ),
              util_t::stat_type_abbrev( stat_indices[ 1 ] ),
              util_t::stat_type_abbrev( stat_indices[ 1 ] ),
              util_t::stat_type_abbrev( stat_indices[ 0 ] ) );
    s += buffer;
    s += "&amp;";

    // Chart legend
    if ( ! p -> sim -> print_styles )
    {
      s += "chdls=dddddd,12";
      s += "&amp;";
    }

    // Chart color
    s += "chco=";
    s += stat_color( stat_indices[ 0 ] );
    s += "&amp;";

    // Grid lines
    s += "chg=5,";
    s += util_t::to_string( 100 / ( ysteps * 2 ) );
    s += ",1,3";
    s += "&amp;";

    // Chart Title
    std::string formatted_name = p -> name_str;
    util_t::urlencode( util_t::str_to_utf8( formatted_name ) );
    snprintf( buffer, sizeof( buffer ), "chtt=%s+Reforge+Scaling", formatted_name.c_str() ); s += buffer;
    s += "&amp;";

    if ( p -> sim -> print_styles )
      s += "chts=666666,18";
    else
      s += "chts=dddddd,18";
    s += "&amp;";

    // Chart markers (Errorbars and Center-line)
    s += "chm=E,FF2222,1,-1,1:5|h,888888,1,0.5,1,-1.0";
  }
  else if ( num_stats == 3 )
  {
    if ( max_dps == 0 ) return 0;

    std::vector<std::array<double,2>> triangle_points;
    std::vector<std::string> colors;
    for ( int i=0; i < ( int ) pd.size(); i++ )
    {
      std::vector<reforge_plot_data_t> scaled_dps = pd[ i ];
      int ref_plot_amount = p -> sim -> reforge_plot -> reforge_plot_amount;
      for ( int j=0; j < 3; j++ )
        scaled_dps[ j ].value = ( scaled_dps[ j ].value + ref_plot_amount ) / ( 3. * ref_plot_amount );
      triangle_points.push_back( ternary_coords( scaled_dps ) );
      colors.push_back( color_temperature_gradient( pd[ i ][ 3 ].value, min_dps, dps_range ) );
    }

    char buffer[ 1024 ];

    s = "<form action='";
    s += get_chart_base_url();
    s += "' method='POST'>";
    s += "<input type='hidden' name='chs' value='525x425' />";
    s += "\n";
    s += "<input type='hidden' name='cht' value='s' />";
    s += "\n";
    if ( ! p -> sim -> print_styles )
    {
      s += "<input type='hidden' name='chf' value='bg,s,333333' />";
      s += "\n";
    }

    s += "<input type='hidden' name='chd' value='t:";
    for ( int j=0; j < 2; j++ )
    {
      for ( int i=0; i < ( int ) triangle_points.size(); i++ )
      {
        snprintf( buffer, sizeof( buffer ), "%f", triangle_points[ i ][ j ] );
        s += buffer;
        if ( i < ( int ) triangle_points.size() - 1 )
          s+= ",";
      }
      if ( j == 0 )
        s+= "|";
    }
    s+="' />";
    s += "\n";
    s += "<input type='hidden' name='chco' value='";
    for ( int i=0; i < ( int ) colors.size(); i++ )
    {
      s += colors[ i ];
      if ( i < ( int ) colors.size() - 1 )
        s+= "|";
    }
    s += "' />\n";
    s += "<input type='hidden' name='chds' value='-0.1,1.1,-0.1,0.95' />";
    s += "\n";

    if ( ! p -> sim -> print_styles )
    {
      s += "<input type='hidden' name='chdls' value='dddddd,12' />";
      s += "\n";
    }
    s += "\n";
    s += "<input type='hidden' name='chg' value='5,10,1,3'";
    s += "\n";
    std::string formatted_name = p -> name_str;
    util_t::urlencode( util_t::str_to_utf8( formatted_name ) );
    snprintf( buffer, sizeof( buffer ), "<input type='hidden' name='chtt' value='%s+Reforge+Scaling' />", formatted_name.c_str() ); s += buffer;
    s += "\n";
    if ( p -> sim -> print_styles )
    {
      s += "<input type='hidden' name='chts' value='666666,18' />";
    }
    else
    {
      s += "<input type='hidden' name='chts' value='dddddd,18' />";
    }
    s += "\n";
    s += "<input type='hidden' name='chem' value='";
    const auto& stat_indices = p -> sim -> reforge_plot -> reforge_plot_stat_indices;
    s += "y;s=text_outline;d=FF9473,18,l,000000,_,";
    snprintf( buffer, sizeof( buffer ), "%s", util_t::stat_type_string( stat_indices[ 0 ] ) );
    s += buffer;
    s += ";py=1.0;po=0.0,0.01;";
    s += "|y;s=text_outline;d=FF9473,18,r,000000,_,";
    snprintf( buffer, sizeof( buffer ), "%s", util_t::stat_type_string( stat_indices[ 1 ] ) );
    s += buffer;
    s += ";py=1.0;po=1.0,0.01;";
    s += "|y;s=text_outline;d=FF9473,18,h,000000,_,";
    snprintf( buffer, sizeof( buffer ), "%s", util_t::stat_type_string( stat_indices[ 2 ] ) );
    s += buffer;
    s += ";py=1.0;po=0.5,0.9' />";
    s += "\n";
    s += "<input type='submit'>";
    s += "\n";
    s += "</form>";
    s += "\n";
  }

  return s.c_str();
}

// chart_t::timeline_dps ====================================================

const char* chart_t::timeline( std::string& s,
                               player_t* p,
                               const std::vector<double>& timeline_data,
                               const std::string& timeline_name,
                               double avg,
                               const char* color )
{
  static const size_t max_points = 600;
  static const double timeline_range  = 60.0;

  size_t max_buckets = timeline_data.size();
  int increment = ( ( max_buckets > max_points ) ?
                    ( ( int ) floor( ( double ) max_buckets / max_points ) + 1 ) :
                    1 );

  double timeline_max = ( max_buckets ?
                          *std::max_element( timeline_data.begin(), timeline_data.end() ) :
                          0 );

  double timeline_adjust = timeline_range / timeline_max;

  char buffer[ 1024 ];

  s = get_chart_base_url();
  s += "chs=525x185";
  s += "&amp;";
  s += "cht=lc";
  s += "&amp;";
  s += "chxs=0,ffffff|1,ffffff";
  s += "&amp;";
  if ( p -> sim -> print_styles )
  {
    s += "chf=c,ls,0,EEEEEE,0.2,FFFFFF,0.2";
  }
  else
  {
    s += "chf=bg,s,333333";
  }
  s += "&amp;";
  s += "chg=100,20";
  s += "&amp;";
  s += "chd=s:";
  for ( size_t i=0; i < max_buckets; i += increment )
  {
    s += simple_encoding( ( int ) ( timeline_data[ i ] * timeline_adjust ) );
  }
  s += "&amp;";
  if ( ! p -> sim -> print_styles )
  {
    snprintf( buffer, sizeof( buffer ), "chco=%s", color ); s += buffer;
    s += "&amp;";
  }
  snprintf( buffer, sizeof( buffer ), "chds=0,%.0f", timeline_range ); s += buffer;
  s += "&amp;";
  s += "chxt=x,y";
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chxl=0:|0|sec=%d|1:|0|avg=%.0f|max=%.0f", ( int ) max_buckets, avg, timeline_max ); s += buffer;
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chxp=1,1,%.0f,100", 100.0 * avg / timeline_max ); s += buffer;
  s += "&amp;";

  snprintf( buffer, sizeof( buffer ), "chtt=%s+Timeline", timeline_name.c_str() ); s += buffer;
  s += "&amp;";
  if ( p -> sim -> print_styles )
  {
    s += "chts=666666,18";
  }
  else
  {
    s += "chts=dddddd,18";
  }

  return s.c_str();
}

// chart_t::distribution_dps ================================================

const char* chart_t::distribution( std::string& s,
                                   sim_t* sim,
                                   const std::vector<int>& dist_data,
                                   const std::string& distribution_name,
                                   double avg, double min, double max )
{
  int max_buckets = ( int ) dist_data.size();

  if ( ! max_buckets )
    return 0;

  int count_max = *std::max_element( dist_data.begin(), dist_data.end() );

  char buffer[ 1024 ];

  s = get_chart_base_url();
  s += "chs=525x185";
  s += "&amp;";
  s += "cht=bvs";
  s += "&amp;";
  if ( sim -> print_styles )
  {
    s += "chf=c,ls,0,EEEEEE,0.2,FFFFFF,0.2";
  }
  else
  {
    s += "chf=bg,s,333333";
  }
  s += "&amp;";
  s += "chg=100,100";
  s += "&amp;";
  s += "chxs=0,ffffff|1,ffffff";
  s += "&amp;";
  s += "chd=t:";
  for ( int i=0; i < max_buckets; i++ )
  {
    snprintf( buffer, sizeof( buffer ), "%s%d", ( i?",":"" ), dist_data[ i ] ); s += buffer;
  }
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chds=0,%d", count_max ); s += buffer;
  s += "&amp;";
  s += "chbh=5";
  s += "&amp;";
  s += "chxt=x";
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chxl=0:|min=%.0f|avg=%.0f|max=%.0f", min, avg, max ); s += buffer;
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chxp=0,1,%.0f,100", 100.0 * ( avg - min ) / ( max - min ) ); s += buffer;
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chtt=%s+Distribution", distribution_name.c_str() ); s += buffer;
  s += "&amp;";
  if ( sim -> print_styles )
  {
    s += "chts=666666,18";
  }
  else
  {
    s += "chts=dddddd,18";
  }

  return s.c_str();
}


// chart_t::gear_weights_wowhead ============================================

const char* chart_t::gear_weights_wowhead( std::string& s,
                                           player_t*    p )
{
  char buffer[ 1024 ];
  bool first=true;

  s = "http://www.torhead.com/?items&amp;filter=";

  switch ( p -> type )
  {
  default: break;
  }

  // Restrict wowhead to rare gems. When epic gems become available:"gm=4;gb=1;"
  s += "gm=3;gb=1;";

  // Automatically reforge items, and min ilvl of 346 (sensible for
  // current raid tier).
  s += "rf=1;minle=346;";

  std::string    id_string = "";
  std::string value_string = "";

  for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
  {
    double value = p -> scaling.get_stat( i );
    if ( value == 0 ) continue;

    int id=0;
    switch ( i )
    {
    case STAT_STRENGTH:                 id = 20;  break;
    case STAT_EXPERTISE_RATING:         id = 117; break;
    case STAT_CRIT_RATING:              id = 96;  break;
    case STAT_ALACRITY_RATING:          id = 103; break;
    case STAT_ARMOR:                    id = 41;  break;
    default: break;
    }

    if ( id )
    {
      if ( ! first )
      {
        id_string += ":";
        value_string += ":";
      }
      first = false;

      snprintf( buffer, sizeof( buffer ), "%d", id );
      id_string += buffer;

      snprintf( buffer, sizeof( buffer ), "%.*f", p -> sim -> report_precision, value );
      value_string += buffer;
    }
  }

  s += "wt="  +    id_string + ";";
  s += "wtv=" + value_string + ";";

  return s.c_str();
}

// chart_t::gear_weights_wowreforge =========================================

const char* chart_t::gear_weights_wowreforge( std::string& s,
                                              player_t*    p )
{
  char buffer[ 1024 ];

  std::string region_str, server_str, name_str;

  // Use valid names if we are provided those
  if ( ! p -> region_str.empty() && ! p -> server_str.empty() && ! p -> name_str.empty() )
  {
    s = "http://wowreforge.com/" + p -> region_str + "/" + p -> server_str + "/" + p -> name_str + "?Spec=Main&amp;template=";
  }
  else
  {
    if ( util_t::parse_origin( region_str, server_str, name_str, p -> origin_str ) )
    {
      s = "http://wowreforge.com/" + region_str + "/" + server_str + "/" + name_str + "?Spec=Main&amp;template=";
    }
    else
    {
      s = "http://wowreforge.com/?template=";
    }
  }

  s += "for:";
  s += util_t::player_type_string( p -> type );
  s += "-";
  s += util_t::talent_tree_string( p -> primary_tree() );

  for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
  {
    double value = p -> scaling.get_stat( i );
    if ( value == 0 ) continue;

    snprintf( buffer, sizeof( buffer ), ",%s:%.*f", util_t::stat_type_abbrev( i ), p -> sim -> report_precision, value );
    s += buffer;
  }

  util_t::urlencode( s );

  return s.c_str();
}

// chart_t::gear_weights_pawn ===============================================

struct compare_stat_scale_factors
{
  player_t* player;
  compare_stat_scale_factors( player_t* p ) : player( p ) {}
  bool operator()( stat_type l, stat_type r ) const
  {
    return( player -> scaling.get_stat( l ) >
            player -> scaling.get_stat( r ) );
  }
};

const char* chart_t::gear_weights_pawn( std::string& s,
                                        player_t*    p,
                                        bool accuracy_expertise )
{
  std::array<stat_type,STAT_MAX> stats;
  boost::iota( stats, STAT_NONE );
  boost::sort( stats, compare_stat_scale_factors( p ) );

  char buffer[ 1024 ];
  bool first = true;

  s.clear();
  snprintf( buffer, sizeof( buffer ), "( Pawn: v1: \"%s\": ", p -> name() );
  s += buffer;

  double maxR = 0;
  double maxY = 0;

  for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
  {
    stat_type stat = stats[ i ];

    double value = p -> scaling.get_stat( stat );
    if ( value == 0 ) continue;

    if ( ! accuracy_expertise )
      if ( stat == STAT_ACCURACY_RATING || stat == STAT_EXPERTISE_RATING )
        value = 0;

    const char* name=0;
    switch ( stat )
    {
    case STAT_STRENGTH:                 name = "Strength";         if ( value*20 > maxR ) maxR = value*20; break;
    case STAT_EXPERTISE_RATING:         name = "ExpertiseRating";  if ( value*20 > maxR ) maxR = value*20; break;
    case STAT_ACCURACY_RATING:          name = "AccuracyRating";   if ( value*20 > maxY ) maxY = value*20; break;
    case STAT_CRIT_RATING:              name = "CritRating";       if ( value*20 > maxY ) maxY = value*20; break;
    case STAT_ALACRITY_RATING:          name = "AlacrityRating";   if ( value*20 > maxY ) maxY = value*20; break;
    case STAT_ARMOR:                    name = "Armor";            break;
    default: break;
    }

    if ( name )
    {
      if ( ! first ) s += ",";
      first = false;
      snprintf( buffer, sizeof( buffer ), " %s=%.*f", name, p -> sim -> report_precision, value );
      s += buffer;
    }
  }

  s += " )";

  return s.c_str();
}

// chart_t::timeline_dps_error ==============================================

const char* chart_t::dps_error( std::string& s,
                                player_t* p )
{
  char buffer[ 1024 ];

  double std_dev = p -> dps.mean_std_dev;

  s = get_chart_base_url();
  s += "chs=525x185";
  s += "&amp;";
  s += "cht=lc";
  s += "&amp;";
  s += "chg=20,20";
  s += "&amp;";
  s += "chco=FF0000";
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chxr=0,%.0f,%.0f|2,0,%.4f", p -> dps.mean - std_dev * 4, p -> dps.mean + std_dev * 4, 1 / ( std_dev * sqrt ( 0.5 * M_PI ) ) ); s += buffer;

  s += "&amp;";
  if ( p -> sim -> print_styles )
  {
    s += "chf=c,ls,0,EEEEEE,0.2,FFFFFF,0.2";
  }
  else
  {
    s += "chf=bg,s,333333";
  }
  s += "&amp;";
  s += "chxt=x,x,y,y";
  s += "&amp;";
  s += "chxl=1:|DPS|3:|p";
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chtt=%.2f%%25+Confidence+Interval", p -> sim -> confidence * 100.0 ); s += buffer;

  s += "&amp;";
  if ( p -> sim -> print_styles )
  {
  s += "chxs=0,000000|1,000000|2,000000|3,000000";
  }
  else
  {
  s += "chxs=0,ffffff|1,ffffff|2,ffffff|3,ffffff";
  }
  s += "&amp;";
  if ( p -> sim -> print_styles )
  {
    s += "chts=666666,18";
  }
  else
  {
    s += "chts=dddddd,18";
  }
  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chfd=0,x,%.2f,%.2f,%.8f,100*exp(-(x-%.4f)^2/(2*%.4f^2))", p -> dps.mean - std_dev * 4, p -> dps.mean + std_dev * 4, std_dev / 100.0, p -> dps.mean, std_dev ); s += buffer;

  s += "&amp;";
  s += "chd=t:-1";

  s += "&amp;";
  snprintf( buffer, sizeof( buffer ), "chm=B,C6D9FD,0,%.0f:%.0f,0", std::max( 4 * std_dev - p -> dps_error, 0.0 ) * 100.0 / std_dev, floor( std::min( 4 * std_dev + p -> dps_error, 8* std_dev ) * 100.0 / std_dev ) ); s += buffer;


  return s.c_str();
}

// chart_t::resource_color ==============================================

const char* chart_t::resource_color( resource_type type )
{
  switch ( type )
  {
  case RESOURCE_HEALTH: return color::green;

  case RESOURCE_FORCE:  return color::blue;

  case RESOURCE_ENERGY:
  case RESOURCE_HEAT:   
  case RESOURCE_AMMO:   return color::yellow;

  case RESOURCE_RAGE:   return color::red;

  default:              return "000000";
  }
}
