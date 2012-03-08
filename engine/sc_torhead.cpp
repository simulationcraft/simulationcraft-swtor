// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

namespace torhead { // ======================================================

namespace { // ANONYMOUS ====================================================

// The wowhead encoding that represents pairs of integers in [0..5] with a
// single character. The character at index i in this array encodes the pair
// ( floor( i / 6 ), i % 6 ).
const base36_t::encoding_t talent_encoding =
{
  '0', 'o', 'b', 'h', 'L', 'x',
  'z', 'k', 'd', 'u', 'p', 't',
  'M', 'R', 'r', 'G', 'T', 'g',
  'c', 's', 'f', 'I', 'j', 'e',
  'm', 'a', 'w', 'N', 'n', 'v',
  'V', 'q', 'i', 'A', 'y', 'E',
};

const base36_t decoder( talent_encoding );

inline char encode_pair( int first, int second=0 )
{
  assert( 0 <= first  && first <= 5 );
  assert( 0 <= second && second <= 5 );
  return talent_encoding[ 6 * first + second ];
}

std::string encode_tree( const std::vector<talent_t*>& tree )
{
  std::string result;

  unsigned i = 0;
  while( i < tree.size() )
  {
    int first = 0;
    if ( tree[ i ] )
      first = tree[ i ] -> rank();
    ++i;

    int second = 0;
    if ( i < tree.size() && tree[ i ] )
      second = tree[ i ] -> rank();
    ++i;

    result += encode_pair( first, second );
  }

  unsigned length = result.size();
  while ( length > 0 && result[ length - 1 ] == '0' )
    --length;
  result.resize( length );

  return result;
}

const char* ac_string( player_type pt )
{
  switch( pt )
  {
  case SITH_MARAUDER:
    return "100";
  case SITH_JUGGERNAUT:
    return "101";
  case SITH_ASSASSIN:
    return "200";
  case SITH_SORCERER:
    return "201";
  case BH_MERCENARY:
    return "300";
  case BH_POWERTECH:
    return "301";
  case IA_SNIPER:
    return "400";
  case IA_OPERATIVE:
    return "401";

  case JEDI_GUARDIAN:
    return "500";
  case JEDI_SENTINEL:
    return "501";
  case JEDI_SAGE:
    return "600";
  case JEDI_SHADOW:
    return "601";
  case S_GUNSLINGER:
    return "700";
  case S_SCOUNDREL:
    return "701";
  case T_COMMANDO:
    return "800";
  case T_VANGUARD:
    return "801";

  default:
    return "0";
  }
}

} // ANONYMOUS namespace ====================================================

// torhead::parse_talents ===================================================

bool parse_talents( player_t& p, const std::string& talent_string )
{
  // wowhead format: [tree_1]Z[tree_2]Z[tree_3] where the trees are character encodings
  // each character expands to a pair of numbers [0-5][0-5]
  // unused deeper talents are simply left blank instead of filling up the string with zero-zero encodings

  int encoding[ MAX_TALENT_SLOTS ];
  boost::fill( encoding, 0 );

  int tree = 0;
  unsigned tree_count = 0;
  size_t count = 0;

  for ( unsigned int i=0; i < talent_string.length(); i++ )
  {
    if ( tree >= MAX_TALENT_TREES )
    {
      p.sim -> errorf( "Player %s has malformed wowhead talent string. Too many talent trees specified.\n",
                       p.name() );
      return false;
    }

    char c = talent_string[ i ];

    if ( c == '.' ) break;

    if ( c == 'Z' )
    {
      ++tree;
      count = 0;
      for ( int j=0; j < tree; j++ )
        count += p.talent_trees[ j ].size();
      tree_count = 0;
      continue;
    }

    try
    {
      base36_t::pair_t decoding = decoder( c );

      encoding[ count++ ] = decoding.first;
      ++tree_count;

      if ( tree_count < p.talent_trees[ tree ].size() )
      {
        encoding[ count++ ] = decoding.second;
        ++tree_count;
      }

      if ( tree_count > p.talent_trees[ tree ].size() )
      {
        ++tree;
        tree_count = 0;
      }
    }

    catch ( base36_t::bad_char bc )
    {
      p.sim -> errorf( "Player %s has malformed wowhead talent string. Translation for '%c' unknown.\n",
                       p.name(), bc.c );
      return false;
    }
  }

  if ( p.sim -> debug )
  {
    std::string str_out;
    for ( size_t i = 0; i < count; i++ ) str_out += '0' + encoding[i];
    log_t::output( p.sim, "%s Wowhead talent string translation: %s\n", p.name(), str_out.c_str() );
  }

  return p.parse_talent_trees( encoding );
}

std::string encode_talents( const player_t& p )
{
  std::string encoding;

  if ( const char* ac_code = ac_string( p.type ) )
  {
    std::stringstream ss;

    ss << "http://www.torhead.com/skill-calc#" << ac_code;

    // This is necessary because sometimes the talent trees change shape between live/ptr.
    for ( unsigned i=0; i < sizeof_array( p.talent_trees ); ++i )
    {
      if ( i > 0 ) ss << 'Z';
      ss << encode_tree( p.talent_trees[ i ] );
    }

    ss << ".1";
    encoding = ss.str();
  }

  return encoding;
}

} // namespace torhead ======================================================
