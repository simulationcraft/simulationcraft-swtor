// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"
#include <stdexcept>

namespace knotor { // =======================================================

namespace { // ANONYMOUS ====================================================

// Exception to throw for bad input; purely a type tag for catch()
struct bad_encoding {};

// Knotor encodes (row, column, points) for each nonzero talent.
// We use a tree_shape_t to map (row, column) to an index into
// player_t::talent_trees[TREE]. Entry is -1 if (row, column)
// does not refer to a valid talent.

typedef int tree_shape_t[3][7][4];

struct map_t
{
  // Knotor ac_id that maps to Simulationcraft player_type
  struct {
    int ac_id; player_type type;
  } class_map[2];
  // Both Repub. and Empire mappings

  // Version of the tree_shape
  int version;
  tree_shape_t tree_shape;
};

const std::string B64( "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+-=" );

const map_t knotor_map[] = {
  {
    { { 1, JEDI_SAGE }, { 7, SITH_SORCERER } },
    2,
    {
      {
        // Seer|Corruption
        { -1, 18, -1, -1 },
        { -1, 16, 17, -1 },
        { 13, 14, 15, -1 },
        { 10, 11, 12, -1 },
        {  6,  7,  8,  9 },
        {  3,  4,  5, -1 },
        {  0,  1,  2, -1 }
      }, {
        // Telekinetics|Lightning
        { -1, -1, 18, -1 },
        { -1, -1, 17, -1 },
        { 14, 15, 16, -1 },
        { -1, 11, 12, 13 },
        {  7,  8,  9, 10 },
        {  3,  4,  5,  6 },
        { -1,  0,  1,  2 }
      },{
        // Balance|Madness
        { -1, 18, -1, -1 },
        { -1, 16, 17, -1 },
        { -1, 13, 14, 15 },
        { 10, -1, 11, 12 },
        { -1,  7,  8,  9 },
        {  3,  4,  5,  6 },
        { -1,  0,  1,  2 }
      }
    }
  },
  {
    { { 10, JEDI_SHADOW }, { 14, SITH_ASSASSIN } },
    2,
    {
      {
        // Kinetic Combat|Darkness
        { -1, -1, 19, -1 },
        { -1, 17, -1, 18 },
        { 13, 14, 15, 16 },
        { -1, 11, 12, -1 },
        {  7,  8,  9, 10 },
        {  3,  4,  5,  6 },
        { -1,  0,  1,  2 }
      }, {
        // Infiltration|Deception
        { -1, -1, 18, -1 },
        { -1, 17, -1, -1 },
        { 14, 15, 16, -1 },
        { 10, 11, 12, 13 },
        {  6,  7,  8,  9 },
        {  3,  4, -1,  5 },
        {  0,  1,  2, -1 }
      },{
        // Balance|Madness
        { -1, 18, -1, -1 },
        { -1, 16, 17, -1 },
        { -1, 13, 14, 15 },
        { 10, -1, 11, 12 },
        { -1,  7,  8,  9 },
        {  3,  4,  5,  6 },
        { -1,  0,  1,  2 }
      }
    }
  },
  {
    { { 2, JEDI_GUARDIAN }, { 15, SITH_JUGGERNAUT } },
    2,
    {
      {
        // Defense|Immortal
        { -1, -1, 19, -1 },
        { -1, 17, 18, -1 },
        { 13, 14, 15, 16 },
        { 10, 11, -1, 12 },
        {  6,  7,  8,  9 },
        { -1,  3,  4,  5 },
        { -1,  0,  1,  2 }
      }, {
        // Vigilance|Vengeance
        { -1, 19, -1, -1 },
        { -1, 17, 18, -1 },
        { 13, 14, 15, 16 },
        { -1, 10, 11, 12 },
        {  6,  7,  8,  9 },
        {  3, -1,  4,  5 },
        { -1,  0,  1,  2 }
      },{
        // Focus|Rage
        { -1, 19, -1, -1 },
        { -1, 17, 18, -1 },
        { 14, 15, 16, -1 },
        { 10, 11, 12, 13 },
        {  7,  8,  9, -1 },
        {  3,  4,  5,  6 },
        {  0,  1,  2, -1 }
      }
    }
  },
  {
    { { 13, JEDI_SENTINEL }, { 4, SITH_MARAUDER } },
    2,
    {
      {
        // Watchman|Annihilation
        { -1, -1, 18, -1 },
        { -1, 16, 17, -1 },
        { -1, 13, 14, 15 },
        {  9, 10, 11, 12 },
        { -1,  6,  7,  8 },
        { -1,  3,  4,  5 },
        { -1,  0,  1,  2 }
      }, {
        // Combat|Carnage
        { -1, 19, -1, -1 },
        { -1, 17, 18, -1 },
        { 14, 15, 16, -1 },
        { 10, 11, 12, 13 },
        {  7,  8,  9, -1 },
        {  3,  4,  5,  6 },
        {  0,  1,  2, -1 }
      },{
        // Focus|Rage
        { -1, 19, -1, -1 },
        { -1, 17, 18, -1 },
        { 14, 15, 16, -1 },
        { 10, 11, 12, 13 },
        {  7,  8,  9, -1 },
        {  3,  4,  5,  6 },
        {  0,  1,  2, -1 }
      }
    }
  },
};

const map_t& get_map( int advanced_class, int version )
{
  for ( unsigned i = 0; i < sizeof_array( knotor_map ); ++i )
  {
    const map_t& map = knotor_map[ i ];
    if ( version == map.version &&
        ( map.class_map[ 0 ].ac_id == advanced_class ||
          map.class_map[ 1 ].ac_id == advanced_class ) )
      return map;
  }

  throw bad_encoding();
}

player_type advanced_class_decode( int ac )
{
  switch( ac )
  {
  case  0: return IA_OPERATIVE;
  case  1: return JEDI_SAGE;
  case  2: return JEDI_GUARDIAN;
  case  3: return BH_POWERTECH;
  case  4: return SITH_MARAUDER;
  case  5: return S_GUNSLINGER;
  case  6: return IA_SNIPER;
  case  7: return SITH_SORCERER;
  case  8: return S_SCOUNDREL;
  case  9: return T_COMMANDO;
  case 10: return JEDI_SHADOW;
  case 11: return T_VANGUARD;
  case 12: return BH_MERCENARY;
  case 13: return JEDI_SENTINEL;
  case 14: return SITH_ASSASSIN;
  case 15: return SITH_JUGGERNAUT;
  default: return PLAYER_NONE;
  }
}

uint8_t base64_decode_one( char c )
{
  std::string::size_type pos = B64.find( c );
  if ( pos == B64.npos )
    throw bad_encoding();
  return pos;
}

std::vector<uint8_t> base64_decode( const std::string& encoded )
{
  typedef std::string::const_iterator iter;

  std::vector<uint8_t> decoded;

  iter i = encoded.begin();
  while ( i != encoded.end() )
  {
    uint8_t o1 = base64_decode_one( *i++ );
    uint8_t o2 = (i != encoded.end() ? base64_decode_one( *i++ ) : 0);
    uint8_t o3 = (i != encoded.end() ? base64_decode_one( *i++ ) : 0);
    uint8_t o4 = (i != encoded.end() ? base64_decode_one( *i++ ) : 0);
    decoded.push_back( (o1 << 2) | (o2 >> 4) );
    decoded.push_back( ((o2 & 0xf) << 4) | (o3 >> 2) );
    decoded.push_back( ((o3 & 0x3) << 6) | o4 );
  }

  return decoded;
}

} // ANONYMOUS namespace ====================================================

bool parse_talents( player_t& p, const std::string& encoded_talent_data )
{
  std::vector<uint8_t> talent_data = knotor::base64_decode( encoded_talent_data );

  unsigned i = 0;

  try
  {
    // version of the talent tree
    uint8_t version = talent_data.at( i++ );

    // AC identifier
    uint8_t advanced_class = talent_data.at( i++ );

    const map_t& map = get_map( advanced_class, version );

    if ( map.class_map[ 0 ].type != p.type && map.class_map[ 1 ].type != p.type )
    {
      // talent spec is for the wrong class
      return false;
    }

    // # of talent points invested in each of the 3 trees.
    int tree_points[MAX_TALENT_TREES];
    for ( unsigned tree = 0; tree < MAX_TALENT_TREES; ++tree )
      tree_points[ tree ] = talent_data.at( i++ );

    for ( unsigned tree = 0; tree < MAX_TALENT_TREES; ++tree )
    {
      while ( tree_points[ tree ] > 0 )
      {
        if ( i >= talent_data.size() )
        {
          // Malformed input
          return false;
        }

        unsigned item = talent_data[ i++ ];
        unsigned row = item >> 5;
        unsigned col = (item >> 3) & 3;
        unsigned points = item & 7;

        tree_points[ tree ] -= points;

        int talent_index = map.tree_shape[ tree ][ row ][ col ];

        if ( false )
        {
          p.sim -> errorf( "Tree(%u) Row(%u) Col(%u) = %d : %u\n",
                              tree, row, col, talent_index, points );
        }

        if ( talent_index >= 0 )
          p.talent_trees[ tree ][ talent_index ] -> set_rank( points );
      }
    }
  }

  catch ( bad_encoding )
  {
    return false;
  }

  catch ( std::out_of_range )
  {
    return false;
  }

  return true;
}

} // namespace knotor =======================================================
