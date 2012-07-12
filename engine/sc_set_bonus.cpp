// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// set_bonus_t::set_bonus_t =================================================

set_bonus_t::set_bonus_t( const std::string& n, const std::string& shell_f, const std::string& armoring_f, slot_mask_t s ) :
  has_2pc( false ), has_4pc( false ),
  next( 0 ), name( n ), count( 0 ), mask( s ),
  force_enable_2pc( false ), force_disable_2pc( false ),
  force_enable_4pc( false ), force_disable_4pc( false )
{
  //set up shell filters for pieces rakata and below (ilevel <=58)
  if ( shell_f.empty() )
    shell_filters.push_back( name );
  else
    util_t::string_split( shell_filters, shell_f, "/" );

  //set up armoring filters for pieces campaign and above (ilevel >=61)
  if ( armoring_f.empty() )
    armoring_filters.push_back( name );
  else
    util_t::string_split( armoring_filters, armoring_f, "/" );
}

// set_bonus_t::decode ======================================================

bool set_bonus_t::decode( const std::string name, const std::vector<std::string> filters ) const
{
  for ( auto& filter : filters )
    {
      if ( name.find( filter ) != name.npos )
        return true;
    }

  return false;
}

// set_bonus_t::decode_by_shell ======================================================

bool set_bonus_t::decode_by_shell( const item_t& item ) const
{
  if ( ! item.name() )
    return false;

  std::string s = item.name();

  return decode( s , shell_filters );

}

// set_bonus_t::decode_by_armoring ==========================================

bool set_bonus_t::decode_by_armoring( const item_t& item ) const
{
  return decode ( item.armoring(), armoring_filters );
}


// set_bonus_t::init ========================================================

void set_bonus_t::init( const player_t& p )
{
  count = 0;
  bool result = false;

  for( auto& i : p.items )
    {

      // Before checking through the armoring filters we should check that we're even
      // looking at a valid item
      if ( mask != 0 &&
           ( mask & bitmask( static_cast<slot_type>( i.slot ) ) ) == 0 )
        continue;

      result = false;


      // Campaign and above pieces (ilevel >=61) bonuses are attached to the armoring
      // Armoring bonuses take precedence over shell bonuses.
      // If an armoring bonus is present it overrides the shell
      if ( decode( i.armoring() , p.armoring_filters ) )
        result = decode_by_armoring( i );
      else
        result = decode_by_shell( i );

      if ( result )
        ++count;
    }

  has_2pc = force_enable_2pc || ( ! force_disable_2pc && count >= 2 );
  has_4pc = force_enable_4pc || ( ! force_disable_4pc && count >= 4 );
}
