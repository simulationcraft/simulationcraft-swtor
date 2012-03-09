// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"


namespace { // ANONYMOUS NAMESPACE ==========================================

// is_number ================================================================

bool is_number( const std::string& s )
{ return std::all_of( s.begin(), s.end(), []( char c ) { return std::isdigit( c ); }); }

// stat_search ==============================================================

void stat_search( std::string&              encoding_str,
                  std::vector<std::string>& description_tokens,
                  int                       stat_type,
                  const std::string&        stat_str )
{
  std::vector<std::string> stat_tokens;
  int num_stats = util_t::string_split( stat_tokens, stat_str, " " );
  int num_descriptions = description_tokens.size();

  for ( int i=0; i < num_descriptions; i++ )
  {
    bool match = true;

    for ( int j=0; j < num_stats && match; j++ )
    {
      if ( ( i + j ) == num_descriptions )
      {
        match = false;
      }
      else
      {
        if ( stat_tokens[ j ][ 0 ] == '!' )
        {
          if ( stat_tokens[ j ].substr( 1 ) == description_tokens[ i + j ] )
          {
            match = false;
          }
        }
        else
        {
          if ( stat_tokens[ j ] != description_tokens[ i + j ] )
          {
            match = false;
          }
        }
      }
    }

    if ( match )
    {
      std::string value_str;

      if ( ( i > 0 ) &&
           ( is_number( description_tokens[ i-1 ] ) ) )
      {
        value_str = description_tokens[ i-1 ];
      }
      if ( ( ( i + num_stats + 1 ) < num_descriptions ) &&
           ( description_tokens[ i + num_stats ] == "by" ) &&
           ( is_number( description_tokens[ i + num_stats + 1 ] ) ) )
      {
        value_str = description_tokens[ i + num_stats + 1 ];
      }

      if ( ! value_str.empty() )
      {
        encoding_str += "_" + value_str + util_t::stat_type_abbrev( stat_type );
      }
    }
  }
}

// is_proc_description ======================================================

bool is_proc_description( const std::string& description_str )
{
  if ( description_str.find( "chance" ) != std::string::npos ) return true;
  if ( description_str.find( "stack"  ) != std::string::npos ) return true;
  if ( description_str.find( "time"   ) != std::string::npos ) return true;
  if ( ( description_str.find( "_sec"   ) != std::string::npos ) &&
       ! ( ( description_str.find( "restores" ) != std::string::npos ) &&
           ( ( description_str.find( "_per_5_sec" ) != std::string::npos ) ||
             ( description_str.find( "_every_5_sec" ) != std::string::npos ) ) ) )
    return true;

  return false;
}
} // ANONYMOUS NAMESPACE ===================================================

#if 0
// armory_t::fuzzy_stats ====================================================

void armory_t::fuzzy_stats( std::string&       encoding_str,
                            const std::string& description_str )
{
  if ( description_str.empty() ) return;

  std::string buffer = description_str;
  armory_t::format( buffer );

  if ( is_proc_description( buffer ) )
    return;

  std::vector<std::string> splits;
  util_t::string_split( splits, buffer, "_." );

  stat_search( encoding_str, splits, STAT_MAX,  "all stats" );
  stat_search( encoding_str, splits, STAT_MAX,  "to all stats" );

  stat_search( encoding_str, splits, STAT_STRENGTH,  "strength" );
  stat_search( encoding_str, splits, STAT_AIM,       "aim" );
  stat_search( encoding_str, splits, STAT_CUNNING,   "cunning" );
  stat_search( encoding_str, splits, STAT_WILLPOWER, "willpower" );
  stat_search( encoding_str, splits, STAT_ENDURANCE, "endurance" );
  stat_search( encoding_str, splits, STAT_PRESENCE,  "presence" );

  stat_search( encoding_str, splits, STAT_EXPERTISE_RATING,         "expertise rating" );

  stat_search( encoding_str, splits, STAT_ALACRITY_RATING,         "alacrity rating" );
  stat_search( encoding_str, splits, STAT_ACCURACY_RATING,           "ranged accuracy rating" );
  stat_search( encoding_str, splits, STAT_ACCURACY_RATING,           "accuracy rating" );
  stat_search( encoding_str, splits, STAT_CRIT_RATING,          "ranged critical strike" );
  stat_search( encoding_str, splits, STAT_CRIT_RATING,          "critical strike rating" );
  stat_search( encoding_str, splits, STAT_CRIT_RATING,          "crit rating" );

  stat_search( encoding_str, splits, STAT_BONUS_ARMOR,    "armor !penetration" );
  stat_search( encoding_str, splits, STAT_DEFENSE_RATING, "defense rating" );
  stat_search( encoding_str, splits, STAT_SHIELD_RATING,  "shield rating" );
  stat_search( encoding_str, splits, STAT_ABSORB_RATING,  "absorb rating" );
}
#endif

// armory_t::format_ ========================================================

void armory_t::format_( std::string& name )
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

    else if ( c == ' ' )
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
