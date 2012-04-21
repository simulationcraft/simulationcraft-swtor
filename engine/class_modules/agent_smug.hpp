// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#ifndef AGENT_SMUG_HPP
#define AGENT_SMUG_HPP

#include "../simulationcraft.hpp"

namespace agent_smug { // ===================================================

class class_t : public player_t
{
public:
  struct
  {
    gain_t* low;
    gain_t* medium;
    gain_t* high;
  } energy_gains;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    player_t( sim, pt, name, rt ), energy_gains()
  {
    primary_attribute   = ATTR_CUNNING;
    secondary_attribute = ATTR_AIM;
  }

  virtual resource_type primary_resource() const
  { return RESOURCE_ENERGY; }

  virtual double range_bonus_stats() const
{ return cunning() + player_t::range_bonus_stats(); }

  virtual double range_crit_from_stats() const
  { return rating_scaler.crit_from_stat( cunning() ) + player_t::range_crit_from_stats(); }

  virtual bool report_attack_type( action_t::policy_t policy )
  {
    return ( policy == action_t::range_policy ||
             policy == action_t::tech_policy ) ||
        ( primary_role() == ROLE_HEAL && policy == action_t::tech_heal_policy );
  }

  virtual void init_scaling()
  {
    player_t::init_scaling();
    scales_with[ STAT_TECH_POWER ] = true;
  }

  virtual void init_base()
  {
    player_t::init_base();

    distance = default_distance = 3;

    resource_base[ RESOURCE_ENERGY ] += 100;
    if ( set_bonus.rakata_enforcers -> four_pc() )
      resource_base[ RESOURCE_ENERGY ] += 5;


  }

  virtual void init_gains()
  {
    player_t::init_gains();
    energy_gains.low    = get_gain( "low"  );
    energy_gains.medium = get_gain( "med"  );
    energy_gains.high   = get_gain( "high" );
  }

  std::pair<int,gain_t*> energy_regen_bracket() const
  {
    if ( resource_current[ RESOURCE_ENERGY ] < 0.2 * resource_max[ RESOURCE_ENERGY ] )
      return std::make_pair( 2, energy_gains.low );
    else if ( resource_current[ RESOURCE_ENERGY ] < 0.6 * resource_max[ RESOURCE_ENERGY ] )
      return std::make_pair( 3, energy_gains.medium );
    else
      return std::make_pair( 5, energy_gains.high );
  }

  virtual double energy_regen_per_second() const
  { return energy_regen_bracket().first;; }

  virtual void regen( timespan_t periodicity )
  {
    std::pair<int,gain_t*> r = energy_regen_bracket();
    resource_gain( RESOURCE_ENERGY, to_seconds( periodicity ) * r.first, r.second );

    player_t::regen( periodicity );
  }
};

class targetdata_t : public ::targetdata_t
{
public:
  targetdata_t( class_t& source, player_t& target ) :
    ::targetdata_t( source, target )
  {}
};

} // namespace agent_smug ===================================================

#endif // AGENT_SMUG_HPP
