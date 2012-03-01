// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

namespace mrrobot { // ======================================================

namespace { // ANONYMOUS ====================================================

// Encoding used by askmrrobot's talent builder.
const base36_t::encoding_t talent_encoding =
{
  '0', '1', '2', '3', '4', '5',
  '6', '7', '8', '9', 'a', 'b',
  'c', 'd', 'e', 'f', 'g', 'h',
  'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't',
  'u', 'v', 'w', 'x', 'y', 'z',
};

const base36_t decoder( talent_encoding );

} // ANONYMOUS namespace ====================================================

// mrrobot::parse_talents ===================================================

bool parse_talents( player_t& p, const std::string& talent_string )
{
  // format: [tree_1]-[tree_2]-[tree_3] where each tree is a
  // sum over all talents of [# of points] * 6 ^ [0-based talent index]
  // in base 36.

  int encoding[ MAX_TALENT_SLOTS ];
  range::fill( encoding, 0 );

  std::vector<std::string> tree_strings;
  util_t::string_split( tree_strings, talent_string, "-" );

  for ( unsigned tree=0; tree < MAX_TALENT_TREES && tree < tree_strings.size(); ++tree )
  {
    unsigned count = 0;
    for ( unsigned j=0; j < tree; j++ )
      count += p.talent_trees[ j ].size();
    unsigned tree_size = p.talent_trees[ tree ].size();
    std::string::size_type pos = tree_strings[ tree ].length();
    unsigned tree_count = 0;
    while ( pos-- > 0 )
    {
      try
      {
        base36_t::pair_t point_pair = decoder( tree_strings[ tree ][ pos ] );
        if ( ++tree_count >= tree_size )
          break;
        encoding[ count++ ] = point_pair.second;
        if ( ++tree_count >= tree_size )
          break;
        encoding[ count++ ] = point_pair.first;
      }

      catch ( base36_t::bad_char bc )
      {
        p.sim -> errorf( "Player %s has malformed wowhead talent string. Translation for '%c' unknown.\n",
                         p.name(), bc.c );
        return false;
      }
    }
  }

  if ( p.sim -> debug )
  {
    std::string str_out;
    for ( size_t i = 0; i < MAX_TALENT_SLOTS; ++i )
      str_out += '0' + encoding[i];
    std::string::size_type pos = str_out.find_last_not_of( '0' );
    if ( pos != str_out.npos )
    {
      str_out.resize( pos );
      log_t::output( p.sim, "%s MrRobot talent string translation: %s\n", p.name(), str_out.c_str() );
    }
  }

  return p.parse_talent_trees( encoding );
}

} // namespace mrrobot ======================================================
