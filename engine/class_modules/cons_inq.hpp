// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#ifndef CONS_INQ_HPP
#define CONS_INQ_HPP

#include "../simulationcraft.hpp"

namespace cons_inq { // =====================================================

class targetdata_t : public ::targetdata_t
{
public:
  dot_t dot_telekinetic_throw;
  dot_t dot_mind_crush;
  dot_t dot_sever_force;

  targetdata_t( player_t& source, player_t& target ) :
    ::targetdata_t( source, target ),
    dot_telekinetic_throw( "telekinetic_throw", &source ),
    dot_mind_crush( "mind_crush", &source ),
    dot_sever_force( "sever_force", &source )
  {
    add( dot_telekinetic_throw );
    alias( dot_telekinetic_throw, "force_lightning" );

    add( dot_mind_crush );
    alias( dot_mind_crush, "mind_crush_dot" );
    alias( dot_mind_crush, "crushing_darkness" );
    alias( dot_mind_crush, "crushing_darkness_dot" );

    add( dot_sever_force );
    alias( dot_sever_force, "creeping_terror" );
  }
};

} // namespace cons_unq =====================================================

#endif // CONS_INQ_HPP
