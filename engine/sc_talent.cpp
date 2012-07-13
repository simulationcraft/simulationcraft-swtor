// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// ==========================================================================
// Talent
// ==========================================================================

// talent_t::talent_t =======================================================

talent_t::talent_t( player_t* /* player */, const std::string n, unsigned tab_page, unsigned max_rank ) :
  _rank( 0 ), _tab_page( tab_page ), _name( n ), _max_rank( max_rank )
{}

bool talent_t::set_rank( unsigned r )
{
  _rank = std::min( r, max_rank() );
  return true;
}
