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
    // snipers get targeted demolition, operatives get combat stims
    talent_t* combat_stims;
    talent_t* targeted_demolition;
    talent_t* cut_down;
    // t4
    talent_t* lethal_purpose;
    talent_t* adhesive_corrosives;
    // snipers get hold your ground, operatives get escape plan
    talent_t* escape_plan;
    talent_t* hold_your_ground;
    talent_t* lethal_dose;
    // t5
    talent_t* cull;
    // snipers get razor rounds, operatives get license to kill
    talent_t* license_to_kill;
    talent_t* razor_rounds;
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
    std::string cull;
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

    set_base_alacrity( get_base_alacrity() + 0.02 * talents.deadly_directive -> rank() );
  }

  virtual double tech_crit_chance() const
  {
    return base_t::tech_crit_chance() + 0.01 * talents.lethality -> rank();
  }

  virtual double range_crit_chance() const
  {
    return base_t::range_crit_chance() + 0.01 * talents.lethality -> rank();
  }


  std::pair<int,gain_t*> energy_regen_bracket() const
  {
    // TODO test: do these brackets change with snipers' Energy Tanks talent?
    int base = talented_energy();
    if ( resource_current[ RESOURCE_ENERGY ] < 0.2 * base )
      return std::make_pair( 2, energy_gains.minimum );
    else if ( resource_current[ RESOURCE_ENERGY ] < 0.4 * base )
      return std::make_pair( 3, energy_gains.low );
    else if ( resource_current[ RESOURCE_ENERGY ] < 0.6 * base )
      return std::make_pair( 4, energy_gains.medium );
    else
      return std::make_pair( 5, energy_gains.high );
  }

  // basically here to get energy without the 4pc bonus
  // so operative will be 100. snipers may be 100, 105, or 110 depending on energy tanks
  virtual int talented_energy() const
  {
    return 100;
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
  debuff_t* debuff_weakening_blast;

  dot_t dot_adrenaline_probe;
  dot_t dot_corrosive_dart;
  dot_t dot_corrosive_dart_weak;
  dot_t dot_corrosive_grenade;
  dot_t dot_corrosive_grenade_weak;
  dot_t dot_cull; // sniper is dot, agent is direct
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

class attack_t : public action_t
{
  typedef action_t base_t;
public:
  attack_t( const std::string& n, class_t* p, attack_policy_t policy, school_type s );
};

class tech_attack_t : public attack_t
{
  typedef attack_t base_t;
public:
  tech_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_KINETIC );
};

class range_attack_t : public attack_t
{
  typedef attack_t base_t;
public:
  range_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_ENERGY );
};

class poison_attack_t : public tech_attack_t
{
  typedef tech_attack_t base_t;
public:
  poison_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_INTERNAL );
  virtual void target_debuff( player_t* tgt, dmg_type type );
};

class adrenaline_probe_t : public action_t
{
  typedef action_t base_t;
public:
  adrenaline_probe_t( class_t* p, const std::string& n, const std::string& options_str );
  static int energy_returned_initial();
  static int energy_returned_tick();
  virtual void execute();
  virtual void tick(dot_t* d);
};

class coordination_t : public action_t
{
  typedef action_t base_t;
public:
  coordination_t( class_t* p, const std::string& n, const std::string& options_str );
  virtual void execute();
  virtual bool ready();
};

class corrosive_dart_t : public poison_attack_t
{
  typedef poison_attack_t base_t;
public:
  corrosive_dart_t* corrosive_dart_weak;
  corrosive_dart_t( class_t* p, const std::string& n, const std::string& options_str, bool weak=false );
  static int energy_cost();
  virtual void tick( dot_t* d );
  virtual void last_tick( dot_t* d );
  virtual void execute();
};

class corrosive_grenade_t : public poison_attack_t
{
  typedef poison_attack_t base_t;
public:
  corrosive_grenade_t* corrosive_grenade_weak;
  corrosive_grenade_t( class_t* p, const std::string& n, const std::string& options_str, bool weak=false );
  static int energy_cost();
  virtual void last_tick( dot_t* d );
  virtual void execute();
};

class cull_extra_t;
class cull_t : public range_attack_t
{
  typedef range_attack_t base_t;
public:
  cull_extra_t* extra_strike;
  cull_t( class_t* p, const std::string& n, const std::string& options_str );
  static int energy_cost();
  virtual void init();
  virtual cull_extra_t* get_extra_strike() = 0;
  virtual void execute();
  virtual void tick( dot_t* dot );
};

class cull_extra_t : public poison_attack_t
{
  typedef poison_attack_t base_t;
public:
  cull_extra_t( class_t* p, const std::string& n );
};

class explosive_probe_t : public tech_attack_t
{
  typedef tech_attack_t base_t;
public:
  explosive_probe_t( class_t* p, const std::string& n, const std::string& options_str);
  static int energy_cost();
  virtual bool ready();
};

class fragmentation_grenade_t : public tech_attack_t
{
  typedef tech_attack_t base_t;
public:
  fragmentation_grenade_t( class_t* p, const std::string& n, const std::string& options_str );
};

class overload_shot_t : public range_attack_t
{
  typedef range_attack_t base_t;
public:
  overload_shot_t( class_t* p, const std::string& n, const std::string& options_str);
};

class rifle_shot_t : public range_attack_t
{
  typedef range_attack_t base_t;
public:
  rifle_shot_t* second_strike;
  rifle_shot_t* offhand_attack;
  rifle_shot_t( class_t* p, const std::string& n, const std::string& options_str, bool is_consequent_strike = false, bool is_offhand_attack = false );
  virtual void execute();
};

class shiv_t : public tech_attack_t
{
  typedef tech_attack_t base_t;
public:
  shiv_t( class_t* p, const std::string& n, const std::string& options_str );
};

class snipe_t : public range_attack_t
{
  typedef range_attack_t base_t;
public:
  snipe_t( class_t* p, const std::string& n, const std::string& options_str );
  static int energy_cost();
  virtual bool ready();
};

class take_cover_t : public action_t
{
  typedef action_t base_t;
public:
  take_cover_t( class_t* p, const std::string& n, const std::string& options_str );
  virtual void execute();
};

class orbital_strike_t : public tech_attack_t
{
  typedef tech_attack_t base_t;
public:
  orbital_strike_t( class_t* p, const std::string& n, const std::string& options_str);
  static int energy_cost();
};

class weakening_blast_t : public range_attack_t
{
  typedef range_attack_t base_t;
public:
  weakening_blast_t( class_t* p, const std::string& n, const std::string& options_str);
  virtual void execute();
};

} // namespace agent_smug ===================================================

#endif // AGENT_SMUG_HPP
