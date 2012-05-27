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
  typedef player_t base_t;

  struct
  {
    gain_t* minimum;
    gain_t* low;
    gain_t* medium;
    gain_t* high;
  } energy_gains;

  struct talents_t
  {
    // Lethality|Dirty Fighting
    // t1
    talent_t* deadly_directive;
    talent_t* lethality;
    talent_t* razor_edge;
    // t2
    talent_t* slip_away;
    talent_t* flash_powder;
    talent_t* corrosive_microbes;
    talent_t* lethal_injectors;
    // t3
    talent_t* corrosive_grenade;
    talent_t* combat_stims;
    talent_t* cut_down;
    // t4
    talent_t* lethal_purpose;
    talent_t* adhesive_corrosives;
    talent_t* escape_plan;
    talent_t* lethal_dose;
    // t5
    talent_t* cull;
    talent_t* license_to_kill;
    talent_t* counterstrike;
    // t6
    talent_t* devouring_microbes;
    talent_t* lingering_toxins;
    // t7
    talent_t* weakening_blast;
  };
  talents_t& talents;

  struct abilities_t
  {
    std::string rifle_shot;
  };
  abilities_t& abilities;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt, talents_t& talents, abilities_t& abilities ) :
    player_t( sim, pt, name, rt ), energy_gains(), talents(talents), abilities(abilities)
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
    energy_gains.minimum = get_gain( "min"  );
    energy_gains.low     = get_gain( "low"  );
    energy_gains.medium  = get_gain( "med"  );
    energy_gains.high    = get_gain( "high" );
  }

  std::pair<int,gain_t*> energy_regen_bracket() const
  {
    if ( resource_current[ RESOURCE_ENERGY ] <= 20 )
      return std::make_pair( 2, energy_gains.minimum );
    else if ( resource_current[ RESOURCE_ENERGY ] <= 40 )
      return std::make_pair( 3, energy_gains.low );
    else if ( resource_current[ RESOURCE_ENERGY ] <= 60 )
      return std::make_pair( 4, energy_gains.medium );
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

  virtual ::action_t* create_action( const std::string& name, const std::string& options ) = 0;

  virtual void init_abilities();

  virtual void init_talents();

  virtual void create_talents();
};

class targetdata_t : public ::targetdata_t
{
public:
  targetdata_t( class_t& source, player_t& target ) :
    ::targetdata_t( source, target )
  {}
};

class action_t : public ::action_t
{
  typedef ::action_t base_t;
public:
  action_t( const std::string& n, class_t* player,
            attack_policy_t policy, resource_type r, school_type s ) :
  base_t( ACTION_ATTACK, n, player, policy, r, s )
  {
    harmful = false;
  }

  targetdata_t* targetdata() const
  { return static_cast<targetdata_t*>( base_t::targetdata() ); }

  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return p(); }
};

struct attack_t : public action_t
{
  attack_t( const std::string& n, class_t* p, attack_policy_t policy, school_type s ) :
    action_t( n, p, policy, RESOURCE_ENERGY, s )
  {
    harmful  = true;
    may_crit = true;
  }
};

struct tech_attack_t : public attack_t
{
  tech_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_KINETIC ) :
    attack_t( n, p, tech_policy, s )
  {
  }
};

struct range_attack_t : public attack_t
{
  range_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_ENERGY ) :
    attack_t( n, p, range_policy, s )
  {
  }
};

} // namespace agent_smug ===================================================

#endif // AGENT_SMUG_HPP
