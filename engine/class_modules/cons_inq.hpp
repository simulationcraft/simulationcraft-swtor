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

  struct gains_t
  {
    gain_t* force_regen;
    gain_t* rakata_stalker_2pc;
  } gains;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    player_t( sim, pt, name, rt ), gains()
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
    if ( this -> set_bonus.rakata_force_mystics -> four_pc() )
      resource_base[ RESOURCE_FORCE ] += 50;
  }

  virtual void init_gains()
  {
    player_t::init_gains();

    gains.force_regen = get_gain( "force_regen" );
    gains.rakata_stalker_2pc = get_gain( "rakata_stalker_2pc" );
  }

  virtual resource_type primary_resource() const
  { return RESOURCE_FORCE; }

  virtual double force_regen_per_second() const
  { return base_regen_per_second; }

  virtual void regen( timespan_t periodicity )
  {
    resource_gain( RESOURCE_FORCE, base_regen_per_second * to_seconds( periodicity ), gains.force_regen );
    player_t::regen( periodicity );
  }
};

class targetdata_t : public ::targetdata_t
{
public:
  dot_t dot_telekinetic_throw;
  dot_t dot_mind_crush;
  dot_t dot_sever_force;
  dot_t dot_rippling_force_tt;
  dot_t dot_rippling_force_d;

  targetdata_t( class_t& source, player_t& target ) :
    ::targetdata_t( source, target ),
    dot_telekinetic_throw( "telekinetic_throw", &source ),
    dot_mind_crush( "mind_crush", &source ),
    dot_sever_force( "sever_force", &source ),
    dot_rippling_force_tt( "rippling_force_tt", &source ),
    dot_rippling_force_d( "rippling_force_d", &source )
  {
    add( dot_telekinetic_throw );
    alias( dot_telekinetic_throw, "force_lightning" );

    add( dot_mind_crush );
    alias( dot_mind_crush, "mind_crush_dot" );
    alias( dot_mind_crush, "crushing_darkness" );
    alias( dot_mind_crush, "crushing_darkness_dot" );

    add( dot_sever_force );
    alias( dot_sever_force, "creeping_terror" );

    add( dot_rippling_force_tt );
    alias( dot_rippling_force_tt, "lightning_burns_fl" );

    add( dot_rippling_force_d );
    alias( dot_rippling_force_d, "lightning_burns_ls" );

  }
};

// Saber Strike ==================================

template <class Base>
class saber_strike_t : public Base
{
  saber_strike_t* second_strike;
  saber_strike_t* third_strike;

public:
  typedef Base base_t;
  typedef typename std::remove_pointer<decltype( static_cast<base_t*>( 0 ) -> cast() )>::type class_t;

  saber_strike_t( class_t* p, const std::string& options_str, bool is_consequent_strike = false ) :
    base_t( "saber_strike", p ), second_strike(), third_strike()
  {
    base_t::parse_options( options_str );

    base_t::base_cost = 0;
    base_t::range = 4.0;

    base_t::weapon = &( p -> main_hand_weapon );
    base_t::weapon_multiplier = -.066;
    base_t::dd.power_mod = .33;

    // Is a Basic attack
    base_t::base_accuracy -= 0.10;

    if ( is_consequent_strike )
    {
      base_t::background = true;
      base_t::dual = true;
    }
    else
    {
      second_strike = new saber_strike_t( p, options_str, true );
      second_strike -> base_execute_time = from_seconds( 0.5 );
      third_strike = new saber_strike_t( p, options_str, true );
      third_strike -> base_execute_time = from_seconds( 1.0 );
    }
  }

  void execute() // override
  {
    base_t::execute();

    if ( second_strike )
    {
      second_strike -> schedule_execute();
      assert( third_strike );
      third_strike -> schedule_execute();
    }

    class_t& p = *base_t::cast();

    if ( p.set_bonus.rakata_stalkers -> two_pc() )
      p.resource_gain( RESOURCE_FORCE, 1, p.cons_inq::class_t::gains.rakata_stalker_2pc );
  }
};

} // namespace cons_inq =====================================================

#endif // CONS_INQ_HPP
