// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// set_bonus_t::set_bonus_t =================================================

set_bonus_t::set_bonus_t( const std::string& n, const std::string& f, slot_mask_t s ) :
  has_2pc( false ), has_4pc( false ),
  next( 0 ), name( n ), count( 0 ), mask( s ),
  force_enable_2pc( false ), force_disable_2pc( false ),
  force_enable_4pc( false ), force_disable_4pc( false )
{
  if ( f.empty() )
    filters.push_back( name );
  else
    util_t::string_split( filters, f, "/" );
}

// set_bonus_t::decode ======================================================

bool set_bonus_t::decode( const item_t& item ) const
{
  if ( mask != 0 &&
       ( mask & bitmask( static_cast<slot_type>( item.slot ) ) ) == 0 )
    return false;

  if ( ! item.name() )
    return false;

  std::string s = item.name();

  for ( auto& filter : filters )
  {
    if ( s.find( filter ) != s.npos )
      return true;
  }

  return false;
}

// set_bonus_t::init ========================================================

void set_bonus_t::init( const player_t& p )
{
  count = 0;

  for( auto& i : p.items )
  {
    if ( decode( i ) )
      ++count;
  }

  has_2pc = force_enable_2pc || ( ! force_disable_2pc && count >= 2 );
  has_4pc = force_enable_4pc || ( ! force_disable_4pc && count >= 4 );
}
