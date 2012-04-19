// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#ifndef CONS_INQ_HPP
#define CONS_INQ_HPP

#include "../simulationcraft.hpp"

namespace cons_inq { // =====================================================

class class_t : public player_t
{
public:
  static constexpr double base_regen_per_second = 8;

  gain_t* gains_force_regen;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    player_t( sim, pt, name, rt ), gains_force_regen()
  {
    primary_attribute   = ATTR_WILLPOWER;
    secondary_attribute = ATTR_STRENGTH;
  }

  virtual void init_scaling()
  {
    player_t::init_scaling();
    scales_with[ STAT_FORCE_POWER ] = true;
  }

  virtual void init_base()
  {
    player_t::init_base();
    resource_base[ RESOURCE_FORCE ] += 100;
  }

  virtual void init_gains()
  {
    player_t::init_gains();
    gains_force_regen = get_gain( "force_regen" );
  }

  virtual resource_type primary_resource() const
  { return RESOURCE_FORCE; }

  virtual double force_regen_per_second() const
  { return base_regen_per_second; }

  virtual void regen( timespan_t periodicity )
  {
    resource_gain( RESOURCE_FORCE, base_regen_per_second * to_seconds( periodicity ), gains_force_regen );
    player_t::regen( periodicity );
  }
};

class targetdata_t : public ::targetdata_t
{
public:
  dot_t dot_telekinetic_throw;
  dot_t dot_mind_crush;
  dot_t dot_sever_force;

  targetdata_t( class_t& source, player_t& target ) :
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

} // namespace cons_inq =====================================================

#endif // CONS_INQ_HPP
