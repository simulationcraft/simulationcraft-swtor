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

class class_t : public player_t
{
public:
  static const int regen_per_second = 8;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    player_t( sim, pt, name, rt )
  {}

  virtual void init_base()
  {
    player_t::init_base();
    player_t::base_force_regen_per_second = regen_per_second;
    resource_base[ RESOURCE_FORCE ] += 100;
  }
};

} // namespace cons_inq =====================================================

#endif // CONS_INQ_HPP
