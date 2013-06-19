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
  struct
  {
    gain_t* low;
    gain_t* medium;
    gain_t* high;
  } heat_gains;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    player_t( sim, pt, name, rt )
  {
    primary_attribute = ATTR_AIM;
    // TODO check
    secondary_attribute = ATTR_CUNNING;
  }

  virtual resource_type primary_resource() const
  { return RESOURCE_HEAT; }

  virtual bool report_attack_type( action_t::policy_t policy )
  {
    return ( policy == action_t::range_policy ||
             policy == action_t::tech_policy ) ||
      // TODO check
        ( primary_role() == ROLE_HEAL && policy == action_t::tech_heal_policy );
  }

  virtual double tech_bonus_stats() const
  { return aim() + player_t::tech_bonus_stats(); }

  virtual double tech_crit_from_stats() const
  { return rating_scaler.crit_from_stat( aim() ) + player_t::tech_crit_from_stats(); }

  virtual void init_scaling()
  {
    scales_with[ STAT_TECH_POWER ]         = true;
    scales_with[ STAT_WEAPON_OFFHAND_DMG ] = true; // empire only

    player_t::init_scaling();
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
    heat_gains.low     = get_gain( "low"  );
    heat_gains.medium  = get_gain( "med"  );
    heat_gains.high    = get_gain( "high" );
  }

  std::pair<double,gain_t*> heat_regen_bracket() const
  {
    if ( resource_current[ RESOURCE_HEAT ] > 60 )
      return std::make_pair( 5 * alacrity(), heat_gains.high );
    else if ( resource_current[ RESOURCE_HEAT ] > 20 )
      return std::make_pair( 3 * alacrity(), heat_gains.medium );
    else
      return std::make_pair( 2 * alacrity(), heat_gains.low );
  }

  virtual double heat_regen_per_second() const
  { return heat_regen_bracket().first; }

  virtual void regen( timespan_t periodicity )
  {
    std::pair<double,gain_t*> r = heat_regen_bracket();
    resource_gain( RESOURCE_HEAT, to_seconds( periodicity ) * r.first, r.second );

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

} // namespace bount_troop ==================================================

#endif // BOUNT_TROOP_HPP
