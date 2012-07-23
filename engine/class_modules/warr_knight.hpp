#ifndef WARR_KNIGHT_HPP
#define WARR_KNIGHT_HPP

#include "../simulationcraft.hpp"

namespace warr_knight {

class class_t : public player_t
{
public:
  typedef player_t base_t;

  struct buffs_t
  {
    //buff_t* cover;
  };
  buffs_t& buffs;

  struct gains_t
  {
    //gain_t* adrenaline_probe;
  };
  gains_t& gains;

  struct procs_t
  {
    //proc_t* corrosive_microbes;
  };
  procs_t& procs;

  struct rngs_t
  {
    //rng_t* corrosive_microbes;
  };
  rngs_t& rngs;

  struct benefits_t
  {
    //benefit_t* wb_poison_ticks;
  };
  benefits_t& benefits;

  struct talents_t
  {
    // Rage|Focus
    //t1
    talent_t* ravager;
    talent_t* malice;
    talent_t* decimate;
    //t2
    talent_t* payback;
    talent_t* overpower;
    talent_t* enraged_scream;
    talent_t* brutality;
    //t3
    talent_t* saber_strength;
    talent_t* obliterate;
    talent_t* strangulate;
    //t4
    talent_t* dominate;
    talent_t* shockwave;
    talent_t* berserker;
    //t5
    talent_t* gravity;
    talent_t* interceptor;
    talent_t* shii_cho_mastery;
    //t6
    talent_t* dark_resonance;
    talent_t* undying;
    //t7
    talent_t* force_crush;
  };
  talents_t& talents;

  struct mirror_t
  {
    // abilities
    std::string a_adrenaline_probe;

    // shared tree talents
    // t1
    //std::string t_deadly_directive;
    // t2
    // t3
    // t4
    // t5
    // t6
    // t7
  };
  mirror_t& m;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt, buffs_t& buffs, gains_t& gains, procs_t& procs, rngs_t& rngs, benefits_t& benefits, talents_t& talents, mirror_t& m ) :
    player_t( sim, pt, name, rt ), buffs(buffs), gains(gains), procs(procs), rngs(rngs), benefits(benefits), talents(talents), m(m)
  {
    primary_attribute   = ATTR_STRENGTH;
    secondary_attribute = ATTR_WILLPOWER;
  }

  virtual resource_type primary_resource() const
  { return RESOURCE_RAGE; }

  virtual void init_scaling()
  {
    player_t::init_scaling();
    scales_with[ STAT_FORCE_POWER ] = true;
  }

  virtual void init_base()
  {
    player_t::init_base();

    distance = default_distance = 3;
    resource_base[ RESOURCE_RAGE ] = 12;
  }

  virtual void init_resources( bool force )
  {
    player_t::init_resources( force );
    resource_current[ RESOURCE_RAGE ] = 0;
  }


  virtual ::action_t* create_action( const std::string& name, const std::string& options ) = 0;

  virtual void init_talents();

  virtual void init_benefits();

  virtual void init_buffs();

  virtual void init_actions();

  virtual void init_procs();

  virtual void init_rng();

  virtual void create_mirror();

  virtual void create_talents();
};

class targetdata_t : public ::targetdata_t
{
  public:
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

} // namespace warr_knight

#endif // WARR_KNIGHT_HPP
