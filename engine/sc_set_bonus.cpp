// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// set_bonus_t::set_bonus_t =================================================

set_bonus_t::set_bonus_t( const std::string& n, const std::string& f, int64_t s_mask ) :
 next( 0 ), name( n ), count( 0 ), slot_mask( s_mask )
{
  if ( f.empty() )
    filters.push_back( name );
  else
    util_t::string_split( filters, f, "/" );
}

// set_bonus_t::decode ======================================================

bool set_bonus_t::decode( const item_t& item ) const
{
  if ( ( item.slot > 0 && slot_mask < 0 ) || ( slot_mask & ( int64_t( 1 ) << item.slot ) ) )
  {
    if ( item.name() )
    {
      std::string s = item.name();

      for ( unsigned int i = 0; i < filters.size(); i++ )
      {
        if ( s.find( filters[ i ] ) != s.npos )
          return true;
      }
    }
  }

  return false;
}

// set_bonus_t::init ========================================================

void set_bonus_t::init( const player_t& p )
{
  count = 0;

  int num_items = p.items.size();
  for ( int i=0; i < num_items; i++ )
  {
    if ( decode( p.items[ i ] ) )
      ++count;
  }
}
