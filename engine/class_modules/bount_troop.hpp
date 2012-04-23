// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#ifndef bount_troop_HPP
#define bount_troop_HPP

#include "../simulationcraft.hpp"

namespace bount_troop { // ===================================================

class class_t : public player_t
{
public:

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    player_t( sim, pt, name, rt )
  {
    primary_attribute = ATTR_AIM;
    // TODO check
    secondary_attribute = ATTR_CUNNING;
  }

  virtual resource_type primary_resource() const
  { return RESOURCE_HEAT; }

  virtual double range_bonus_stats() const
{ return aim() + player_t::range_bonus_stats(); }

  virtual double range_crit_from_stats() const
  { return rating_scaler.crit_from_stat( aim() ) + player_t::range_crit_from_stats(); }

  virtual bool report_attack_type( action_t::policy_t policy )
  {
    return ( policy == action_t::range_policy ||
             policy == action_t::tech_policy ) ||
      // TODO check
        ( primary_role() == ROLE_HEAL && policy == action_t::tech_heal_policy );
  }

  virtual void init_scaling()
  {
    player_t::init_scaling();
    // TODO check
    scales_with[ STAT_TECH_POWER ] = true;
  }

  virtual void init_base()
  {
    player_t::init_base();

    distance = default_distance = 20;

    // TODO heat needs to be reversed, so it dissipates and abilities generate it
    // 
    resource_base[ RESOURCE_HEAT ] += 100;
  }

  virtual void init_gains()
  {
    player_t::init_gains();
    //energy_gains.low    = get_gain( "low"  );
  }

};

class targetdata_t : public ::targetdata_t
{
public:
  targetdata_t( class_t& source, player_t& target ) :
    ::targetdata_t( source, target )
  {}
};

} // namespace bount_troop ==================================================

#endif // BOUNT_TROOP_HPP
