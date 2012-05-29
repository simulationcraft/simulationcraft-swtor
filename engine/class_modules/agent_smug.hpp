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

  struct buffs_t
  {
    buff_t* adrenaline_probe;
    buff_t* cover;
  };
  buffs_t& buffs;

  struct gains_t
  {
    gain_t* adrenaline_probe;
    gain_t* lethal_purpose;
  };
  gains_t& gains;

  struct procs_t
  {
    proc_t* corrosive_microbes;
  };
  procs_t& procs;

  struct rngs_t
  {
    rng_t* corrosive_microbes;
  };
  rngs_t& rngs;

  struct benefits_t
  {
    benefit_t* devouring_microbes_ticks;
    benefit_t* wb_poison_ticks;
  };
  benefits_t& benefits;

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
    std::string adrenaline_probe;
    std::string coordination;
    std::string corrosive_dart;
    std::string corrosive_dart_weak;
    std::string corrosive_grenade;
    std::string corrosive_grenade_weak;
    std::string corrosive_microbes;
    std::string corrosive_microbes_tick;
    std::string explosive_probe;
    std::string fragmentation_grenade;
    std::string lethal_purpose;
    std::string orbital_strike;
    std::string overload_shot;
    std::string rifle_shot;
    std::string shiv;
    std::string snipe;
    std::string take_cover;
    std::string weakening_blast;

    // buff names
    std::string cover;
  };
  abilities_t& abilities;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt, buffs_t& buffs, gains_t& gains, procs_t& procs, rngs_t& rngs, benefits_t& benefits, talents_t& talents, abilities_t& abilities ) :
    player_t( sim, pt, name, rt ), energy_gains(), buffs(buffs), gains(gains), procs(procs), rngs(rngs), benefits(benefits), talents(talents), abilities(abilities)
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

  std::pair<int,gain_t*> energy_regen_bracket() const
  {
    // TODO test: do these brackets change with snipers' Energy Tanks talent?
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
  { return energy_regen_bracket().first; }

  virtual void regen( timespan_t periodicity )
  {
    std::pair<int,gain_t*> r = energy_regen_bracket();
    resource_gain( RESOURCE_ENERGY, to_seconds( periodicity ) * r.first, r.second );

    player_t::regen( periodicity );
  }

  virtual ::action_t* create_action( const std::string& name, const std::string& options ) = 0;

  virtual void init_abilities();

  virtual void init_talents();

  virtual void init_benefits();

  virtual void init_buffs();

  virtual void init_actions();

  virtual void init_gains();

  virtual void init_procs();

  virtual void init_rng();

  virtual void create_talents();
};

class targetdata_t : public ::targetdata_t
{
public:
  buff_t* debuff_weakening_blast;

  dot_t dot_adrenaline_probe;
  dot_t dot_corrosive_dart;
  dot_t dot_corrosive_dart_weak;
  dot_t dot_corrosive_grenade;
  dot_t dot_corrosive_grenade_weak;
  dot_t dot_orbital_strike;

  targetdata_t( class_t& source, player_t& target );
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

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( base_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return p(); }
};

struct attack_t : public action_t
{
  attack_t( const std::string& n, class_t* p, attack_policy_t policy, school_type s ) :
    action_t( n, p, policy, RESOURCE_ENERGY, s )
  {
    harmful       = true;
    may_crit      = true;
    tick_may_crit = true;
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

struct poison_attack_t : public tech_attack_t
{
  poison_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_INTERNAL ) :
    tech_attack_t( n, p, s )
  {
    may_crit       =  false;
    base_crit     += .04 * p -> talents.lethal_dose -> rank();
  }

  void target_debuff( player_t* tgt, dmg_type type )
  {
    tech_attack_t::target_debuff( tgt, type );

    class_t& p = *cast();

    if ( unsigned rank = p.talents.devouring_microbes -> rank() )
    {
      bool up = tgt -> health_percentage() < 30;
      p.benefits.devouring_microbes_ticks -> update( up );
      if ( up )
        target_multiplier += 0.05 * rank;
    }

    if ( p.talents.weakening_blast -> rank() )
    {
      targetdata_t& td = *targetdata();
      bool up = td.debuff_weakening_blast -> up();
      p.benefits.wb_poison_ticks -> update( up );
      if ( up )
      {
        td.debuff_weakening_blast -> decrement();
        target_multiplier += 0.3;
      }
    }
  }
};

// Snipe | Charged Burst ====================================================

struct snipe_t : public range_attack_t
{
  typedef range_attack_t base_t;
  snipe_t( class_t* p, const std::string& n, const std::string& options_str) :
    range_attack_t( n, p )
  {

    parse_options( options_str );

    range = ( player -> type == IA_SNIPER || player -> type == S_GUNSLINGER ) ? 35 : 30.0;

    base_cost                   = 20;
    base_execute_time           = from_seconds( 1.5 );
    dd.power_mod                = 1.85;
    dd.standardhealthpercentmin =
    dd.standardhealthpercentmax = 0.185;
    weapon                      = &( player->main_hand_weapon );
    weapon_multiplier           = 0.23;
  }

  virtual bool ready()
  {
    // TODO only ready if in cover
    // cover not yet implemented
    return base_t::ready();
  }
};

// Take Cover | Take Cover ==================================================

struct take_cover_t : public action_t
{
  typedef action_t base_t;
  take_cover_t( class_t* p, const std::string& n, const std::string& options_str ) :
    action_t( n, p, tech_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    parse_options( options_str );

    trigger_gcd = timespan_t::zero();
  }

  void execute()
  {
    action_t::execute();

    class_t& p = *cast();
    if ( p.buffs.cover -> up() )
      p.buffs.cover -> expire();
    else
      p.buffs.cover -> trigger();
  }
};


} // namespace agent_smug ===================================================

#endif // AGENT_SMUG_HPP
