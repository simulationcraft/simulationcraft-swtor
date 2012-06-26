// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "agent_smug.hpp"

namespace scoundrel_operative { // ==========================================

class class_t;

struct targetdata_t : public agent_smug::targetdata_t
{
  dot_t dot_acid_blade_poison;
  dot_t dot_stim_boost;

  targetdata_t( class_t& source, player_t& target );
};

// ==========================================================================
// Scoundrel / Operative
// ==========================================================================

struct class_t : public agent_smug::class_t
{
  typedef agent_smug::class_t base_t;

  // Buffs
  struct buffs_t:base_t::buffs_t
  {
    // buffs from talents
    buff_t* acid_blade_coating;
    buff_t* acid_blade_arpen;
    //buff_t* advanced_cloaking;

    // core buffs
    buff_t* stealth;
    buff_t* stim_boost;
    buff_t* tactical_advantage;
    //buff_t* cloaking_screen;
  } buffs;

  // Gains
  struct gains_t:base_t::gains_t
  {
    gain_t* endorphin_rush;
    gain_t* combat_stims;
    gain_t* revitalizers;
    gain_t* stim_boost;
  } gains;

  // Procs
  struct procs_t:base_t::procs_t
  {
  } procs;

  // RNGs
  struct rngs_t:base_t::rngs_t
  {
    rng_t* collateral_strike;
  } rngs;

  // Benefits
  struct benefits_t:base_t::benefits_t
  {
  } benefits;

  // Cooldowns
  struct cooldowns_t
  {
  } cooldowns;

  // Talents
  struct talents_t:base_t::talents_t
  {
    // Medicine|Sawbones
    // t1
    talent_t* incisive_action;
    talent_t* precision_instruments;
    talent_t* imperial_education;
    // t2
    talent_t* endorphin_rush;
    talent_t* medical_consult;
    talent_t* surgical_steadiness;
    talent_t* chem_resistant_inlays;
    // t3
    talent_t* prognosis_critical;
    talent_t* kolto_probe;
    talent_t* sedatives;
    // t4
    talent_t* patient_studies;
    talent_t* medical_engineering;
    talent_t* evasive_imperative;
    // t5
    talent_t* tox_scan;
    talent_t* medical_therapy;
    talent_t* surgical_probe;
    talent_t* surgical_precision;
    // t6
    talent_t* med_shield;
    talent_t* accomplished_doctor;
    // t7
    talent_t* recuperative_nanotech;

    // Concealment|Scrapper
    // t1
    talent_t* concealed_attacks;
    talent_t* imperial_brew;
    talent_t* survival_training;
    // t2
    talent_t* infiltrator;
    talent_t* surgical_strikes;
    talent_t* inclement_conditioning;
    talent_t* scouting;
    // t3
    talent_t* flanking;
    talent_t* laceration;
    talent_t* collateral_strike;
    talent_t* revitalizers;
    // t4
    talent_t* pin_down;
    talent_t* tactical_opportunity;
    talent_t* energy_screen;
    // t5
    talent_t* waylay;
    talent_t* culling;
    talent_t* advanced_cloaking;
    // t6
    talent_t* meticulously_kept_blades;
    talent_t* jarring_strike;
    // t7
    talent_t* acid_blade;
  } talents;

  // Mirror names
  struct mirror_t:base_t::mirror_t
  {
    // abilities
    std::string a_backstab;
    std::string a_combat_stims;
    std::string a_hidden_strike;
    std::string a_stealth;
    std::string a_stim_boost;
    std::string a_tactical_advantage;

    // Medicine|Sawbones
    // t1
    std::string t_incisive_action;
    std::string t_precision_instruments;
    std::string t_imperial_education;
    // t2
    std::string t_endorphin_rush;
    std::string t_medical_consult;
    std::string t_surgical_steadiness;
    std::string t_chem_resistant_inlays;
    // t3
    std::string t_prognosis_critical;
    std::string t_kolto_probe;
    std::string t_sedatives;
    // t4
    std::string t_patient_studies;
    std::string t_medical_engineering;
    std::string t_evasive_imperative;
    // t5
    std::string t_tox_scan;
    std::string t_medical_therapy;
    std::string t_surgical_probe;
    std::string t_surgical_precision;
    // t6
    std::string t_med_shield;
    std::string t_accomplished_doctor;
    // t7
    std::string t_recuperative_nanotech;

    // Concealment|Scrapper
    // t1
    std::string t_concealed_attacks;
    std::string t_imperial_brew;
    std::string t_survival_training;
    // t2
    std::string t_infiltrator;
    std::string t_surgical_strikes;
    std::string t_inclement_conditioning;
    std::string t_scouting;
    // t3
    std::string t_flanking;
    std::string t_laceration;
    std::string t_collateral_strike;
    std::string t_revitalizers;
    // t4
    std::string t_pin_down;
    std::string t_tactical_opportunity;
    std::string t_energy_screen;
    // t5
    std::string t_waylay;
    std::string t_culling;
    std::string t_advanced_cloaking;
    // t6
    std::string t_meticulously_kept_blades;
    std::string t_jarring_strike;
    // t7
    std::string t_acid_blade;
  } m;

  action_t* acid_blade_poison;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    base_t( sim, pt == IA_OPERATIVE ? IA_OPERATIVE : S_SCOUNDREL, name, rt, buffs, gains, procs, rngs, benefits, talents, m ),
    buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents(), acid_blade_poison()
  {
    tree_type[ IA_OPERATIVE_MEDICINE    ] = TREE_MEDICINE;
    tree_type[ IA_OPERATIVE_CONCEALMENT ] = TREE_CONCEALMENT;
    tree_type[ IA_LETHALITY             ] = TREE_LETHALITY;

    create_mirror();
    create_talents();
    create_options();
  }

  // Character Definition
  targetdata_t* new_targetdata( player_t& target ) // override
  { return new targetdata_t( *this, target ); }

  ::action_t* create_action( const std::string& name, const std::string& options );
  void      init_talents();
  void      init_base();
  void      init_benefits();
  void      init_buffs();
  void      init_gains();
  void      init_procs();
  void      init_rng();
  void      init_actions();
  void      reset();
  double    armor_penetration() const;
  double    energy_regen_per_second() const;
  double    composite_player_multiplier(school_type school, ::action_t *a) const;
  void      create_mirror();
  void      create_talents();
  role_type primary_role() const;
};


targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  agent_smug::targetdata_t( source, target ),
  dot_acid_blade_poison ( source.m.t_acid_blade + "_poison" , &source ),
  dot_stim_boost        ( source.m.a_stim_boost             , &source )
{
  add( dot_acid_blade_poison );
  add( dot_stim_boost        );
}

struct action_t : public agent_smug::action_t
{
  action_t( const std::string& n, class_t* player, attack_policy_t policy, resource_type r, school_type s ) :
    agent_smug::action_t( n, player, policy, r, s )
  {}

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( agent_smug::action_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }
};

struct attack_t : public agent_smug::attack_t
{
  attack_t( const std::string& n, class_t* p, attack_policy_t policy, school_type s ) :
    agent_smug::attack_t( n, p, policy, s )
  {}

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( agent_smug::action_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }
};

struct tech_attack_t : public agent_smug::tech_attack_t
{
  tech_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_KINETIC ) :
   agent_smug::tech_attack_t( n, p, s )
  {}

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( agent_smug::action_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }
};

struct range_attack_t : public agent_smug::range_attack_t
{
  range_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_ENERGY ) :
    agent_smug::range_attack_t( n, p, s )
  {}

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( agent_smug::action_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }
};

struct poison_attack_t : public agent_smug::poison_attack_t
{
  poison_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_INTERNAL ) :
    agent_smug::poison_attack_t( n, p, s )
  {}

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( agent_smug::poison_attack_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }
};

// ==========================================================================
// Scoundrel / Operative Abilities
// ==========================================================================

// 4 types of attack: melee, ranged, force, tech. denoted by policy.
// 4 types of damage: kinetic, energy, internal, elemental.
// all combinations possible, but usually patterend.


// action hierarchy
// ----------------
// _action_t - attack - tech_attack  - consume_acid_blade_attack - hidden strike
//                                                               - backstab
//
//                                   - poison_attack             - corrosive dart
//                                                               - corrosive grenade
//                                                               - acid blade poison
//
//                                   - most abilities except...
//
//                    - range_attack - rifle_shot
//                                   - overload_shot
//
//           - acid_blade
//           - adrenaline_probe
//           - stealth

// Consume Acid Blade Poison Attack | ??? ===================================

struct consume_acid_blade_attack_t : public tech_attack_t
{
  typedef tech_attack_t base_t;

  consume_acid_blade_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_KINETIC ) :
    base_t( n, p, s )
  {}

  void execute()
  {
    base_t::execute();

    class_t* p = cast();
    if ( p -> buffs.acid_blade_coating -> up() )
    {
      p -> buffs.acid_blade_coating -> decrement();
      p -> buffs.acid_blade_arpen -> trigger();

      assert( p -> acid_blade_poison );
      p -> acid_blade_poison -> target = target;
      p -> acid_blade_poison -> execute();
    }
  }
};

// Acid Blade | ??? =========================================================

struct acid_blade_t : public action_t
{
  typedef action_t base_t;

  struct acid_blade_poison_t : public poison_attack_t
  {
    typedef poison_attack_t base_t;

    acid_blade_poison_t( class_t* p, const std::string& n ) :
      base_t( n, p )
    {
      td.standardhealthpercentmin = td.standardhealthpercentmax = 0.031;
      td.power_mod = 0.31;
      background = true;
      trigger_gcd = timespan_t::zero();

      num_ticks = 6;
      base_tick_time = from_seconds( 1.0 );
    }
  };

  acid_blade_poison_t* poison;

  acid_blade_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_INTERNAL ),
    poison( new acid_blade_poison_t( p, n + "_poison" ) )
  {
    check_talent( p -> talents.acid_blade -> rank() );

    parse_options( options_str );

    base_cost = 10;
    cooldown -> duration = from_seconds( 2.0 );
    use_off_gcd = true;
    trigger_gcd = timespan_t::zero();

    add_child( poison );
  }

  void execute()
  {
    action_t::execute();

    class_t* p = cast();
    p -> buffs.acid_blade_coating -> trigger();
    p -> acid_blade_poison = poison;
  }
};

// Adrenaline Probe | Cool Head =============================================
struct adrenaline_probe_t : public agent_smug::adrenaline_probe_t
{
  typedef agent_smug::adrenaline_probe_t base_t;

  class_t* p() const { return static_cast<class_t*>( player ); }

  adrenaline_probe_t( class_t* p, const std::string& n, const std::string& options_str) :
    base_t( p, n, options_str )
  { }

  static int energy_returned_endorphin_rush( class_t* p )
  {
    return 8 * p -> talents.endorphin_rush -> rank();
  }

  void execute()
  {
    base_t::execute();
    if ( int energy = energy_returned_endorphin_rush( p() ) )
      p() -> resource_gain( RESOURCE_ENERGY, energy, p() -> gains.endorphin_rush );
  }
};


// Overload Shot | Quick Shot ===============================================

struct overload_shot_t : public agent_smug::overload_shot_t
{
  typedef agent_smug::overload_shot_t base_t;

  overload_shot_t( class_t* p, const std::string& n, const std::string& options_str) :
    base_t( p, n, options_str )
  {
    range = 10;
    base_multiplier += 0.15;
  }
};

// Stealth | ??? ============================================================

struct stealth_t : public action_t
{
  typedef action_t base_t;

  stealth_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( n, p, tech_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    parse_options( options_str );

    // does trigger gcd, but since we only use it out of combat lets make it instant
    trigger_gcd = timespan_t::zero();
  }

  bool ready()
  {
    if ( p() -> in_combat )
      return false;

    if ( p() -> buffs.stealth -> check() )
      return false;

    return base_t::ready();
  }

  void execute()
  {
    p() -> buffs.stealth -> trigger();
    base_t::execute();
  }
};

// Shiv | Blaster Whip ======================================================

struct shiv_t : public agent_smug::shiv_t
{
  typedef agent_smug::shiv_t base_t;

  class_t* p() const { return static_cast<class_t*>( player ); }

  shiv_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( p, n, options_str )
  {
    base_multiplier += p -> talents.surgical_strikes -> rank() * 0.02
                     + p -> talents.razor_edge       -> rank() * 0.04;
  }

  void execute()
  {
    base_t::execute();

    // TODO check if granted on misses etc?
    if ( result_is_hit() )
      p() -> buffs.tactical_advantage -> trigger();
  }
};

// Backstab | ??? ===========================================================

struct backstab_t : public consume_acid_blade_attack_t
{
  typedef consume_acid_blade_attack_t base_t;

  backstab_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( n, p )
  {
    rank_level_list = { 10, 13, 17, 23, 35, 47, 50 };

    parse_options( options_str );

    cooldown -> duration        = from_seconds( 12.0 );
    range                       = 4.0;
    dd.standardhealthpercentmin = 0.165;
    dd.standardhealthpercentmax = 0.245;
    dd.power_mod                = 2.05;
    base_cost                   = 10
                                - p -> talents.flanking                 -> rank()   * 5;
    base_multiplier            += p -> talents.surgical_strikes         -> rank()   * 0.02
                               +  p -> talents.waylay                   -> rank()   * 0.04;
    base_crit                  += p -> talents.concealed_attacks        -> rank()   * 0.08
                               +  ( p -> set_bonus.rakata_enforcers       -> two_pc() ? 0.15 : 0);
    crit_bonus                 += p -> talents.meticulously_kept_blades -> rank()   * 0.1;
  }
};

// Laceration | ??? =========================================================

struct laceration_t : public tech_attack_t
{
  typedef tech_attack_t base_t;

  struct collateral_strike_t : public tech_attack_t
  {
    typedef tech_attack_t base_t;

    collateral_strike_t( class_t* p, const std::string& n, const std::string& options_str ) :
      base_t( n, p )
    {
      parse_options( options_str );

      cooldown -> duration        =  from_seconds( 10.0 );
      dd.standardhealthpercentmin =  0.14;
      dd.standardhealthpercentmax =  0.22;
      dd.power_mod                =  1.8;
      base_multiplier             += p -> talents.culling->rank() * 0.02;
      background                  =  true;
      trigger_gcd                 =  timespan_t::zero();
    }

    virtual void execute()
    {
      base_t::execute();

      // if target is poisoned regrant TA
      targetdata_t* td = targetdata();
      if ( td -> dot_acid_blade_poison.ticking || td -> dot_corrosive_dart.ticking )
        p() -> buffs.tactical_advantage -> trigger();
    }
  };

  collateral_strike_t* collateral_strike;

  laceration_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( n, p ), collateral_strike( 0 )
  {
    check_talent( p -> talents.laceration -> rank() );

    parse_options( options_str );

    base_cost                   =  10;
    range                       =  4.0;
    dd.standardhealthpercentmin =  0.14;
    dd.standardhealthpercentmax =  0.22;
    dd.power_mod                =  1.8;
    base_multiplier             += p -> talents.culling->rank() * 0.02;

    if (  p -> talents.collateral_strike -> rank() )
    {
      collateral_strike = new collateral_strike_t( p, n + "_collateral_strike", options_str );
      add_child( collateral_strike );
    }
  }

  virtual bool ready()
  {
    return p() -> buffs.tactical_advantage -> check() ? base_t::ready() : false;
  }

  virtual void execute()
  {
    base_t::execute();

    class_t* p = cast();

    // TODO check if a miss etc consumes the TA
    if ( result_is_hit() )
    {
      p -> buffs.tactical_advantage -> decrement();

      if ( collateral_strike != nullptr
          && collateral_strike -> cooldown -> remains() <= timespan_t::zero()
          && p -> rngs.collateral_strike -> roll ( p -> talents.collateral_strike -> rank() * 0.25 ))
        collateral_strike -> execute();
    }
  }
};

// Hidden Strike | ??? ======================================================

struct hidden_strike_t : public consume_acid_blade_attack_t
{
  typedef consume_acid_blade_attack_t base_t;

  hidden_strike_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( n, p )
  {
    rank_level_list = { 36, 50 };

    parse_options( options_str );

    base_cost                   =  17;
    range                       =  4.0;
    cooldown -> duration        =  from_seconds( 7.5 ); // FIXME TEST: 7.5 or 8?
    dd.standardhealthpercentmin =  0.218;
    dd.standardhealthpercentmax =  0.278;
    dd.power_mod                =  2.48;
    base_crit                   += p -> talents.concealed_attacks        -> rank() * 0.08;
    crit_bonus                  += p -> talents.meticulously_kept_blades -> rank() * 0.1;
  }

  virtual bool ready()
  {
    if ( ! p() -> buffs.stealth -> check() )
      return false;

    return base_t::ready();
  }

  virtual void execute()
  {
    class_t& p = *cast();
    p.buffs.stealth -> up();
    base_t::execute();
    p.buffs.tactical_advantage -> trigger();
  }
  // TODO check for talent and trigger knockdown (jarring strike)
};

// Cull | Wounding Shot =====================================================

struct cull_extra_t : public agent_smug::cull_extra_t
{
  typedef agent_smug::cull_extra_t base_t;

  cull_extra_t( class_t* p, const std::string& n ) :
    base_t( p, n )
  {
    dd.standardhealthpercentmin =
    dd.standardhealthpercentmax =  0.066;
    dd.power_mod                =  0.66;
  }
};

struct cull_t : public agent_smug::cull_t
{
  typedef agent_smug::cull_t base_t;

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( agent_smug::cull_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return static_cast<class_t*>( player ); }

  cull_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( p, n, options_str )
  {
    check_talent( p -> talents.cull -> rank() );

    parse_options( options_str );

    base_cost                  -=  3 * p -> talents.razor_rounds -> rank();
    weapon                      =  &( player->main_hand_weapon );
    weapon_multiplier           =  -0.1;
    dd.standardhealthpercentmin =
    dd.standardhealthpercentmax =  0.135;
    dd.power_mod                =  1.35;
  }

  agent_smug::cull_extra_t* get_extra_strike()
  {
    return new cull_extra_t( p(), name_str + "_extra" );
  }

  bool ready()
  {
    return p() -> buffs.tactical_advantage -> check() ? base_t::ready() : false;
  }

  void execute()
  {
    base_t::execute();
    p() -> buffs.tactical_advantage -> decrement();
  }
};

// Stim Boost | ??? =========================================================

struct stim_boost_t : public action_t
{
  typedef action_t base_t;

  static double tick_amount( const class_t& op )
  { return 3 + 0.5 * op.talents.culling -> rank(); }

  stim_boost_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    parse_options( options_str );

    cooldown -> duration = from_seconds( 35 - p -> talents.targeted_demolition -> rank() * 7.5 );
    use_off_gcd          = true;
    trigger_gcd          = timespan_t::zero();
    num_ticks            = 15;
    base_tick_time       = from_seconds( 3 );
  }

  virtual bool ready()
  {
    if ( ! p() -> buffs.tactical_advantage -> check() )
      return false;

    return base_t::ready();
  }

  virtual void tick( dot_t* d )
  {
    base_t::tick(d);

    class_t& p = *cast();

    p.resource_gain( RESOURCE_ENERGY, tick_amount( p ), p.gains.stim_boost );
    if ( p.talents.revitalizers -> rank() )
      p.resource_gain( RESOURCE_HEALTH, p.resource_max[ RESOURCE_HEALTH ] * 0.02, p.gains.revitalizers );
  }

  virtual void execute()
  {
    base_t::execute();

    class_t& p = *cast();
    p.buffs.stim_boost -> trigger();
    p.buffs.tactical_advantage -> decrement();
    if ( p.talents.targeted_demolition -> rank() )
      p.resource_gain( RESOURCE_ENERGY, 5 * p.talents.targeted_demolition -> rank(), p.gains.combat_stims );
  }
};

// Cloaking Screen | ??? ====================================================
// "vanish" allows reusing hidden strike.

// Sever Tendon | ??? =======================================================
// does damage, snares and roots talented. mainly for pvp.

// Debilitate | ??? =========================================================
// stuns non bosses and does damage. pvp mainly.
// slip away talent reduces cooldown by 15s

// Flash Bang | ??? =========================================================
// aoe cc 5 targets 8 seconds.
// flash powder talent reduces accuracy for 8 seconds after ends


// ==========================================================================
// Scoundrel / Operative Callbacks
// ==========================================================================

class action_callback_t : public ::action_callback_t
{
public:
  action_callback_t( class_t* player ) :
    ::action_callback_t( player )
  {}

  class_t* p() const { return static_cast<class_t*>( listener ); }
  class_t* cast() const { return p(); }
};

struct all_attack_callback_t : public action_callback_t
{
  all_attack_callback_t( class_t* p ) :
    action_callback_t( p )
  {}

  virtual void trigger( ::action_t* /* a */, void* /* call_data */)
  {
    p() -> buffs.stealth -> expire();
  }
};

// ==========================================================================
// scoundrel_operative Character Definition
// ==========================================================================

// class_t::create_action =====================================

::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  if ( name == m.t_acid_blade       ) return new acid_blade_t       ( this, name, options_str ) ;
  if ( name == m.a_backstab         ) return new backstab_t         ( this, name, options_str ) ;
  if ( name == m.a_hidden_strike    ) return new hidden_strike_t    ( this, name, options_str ) ;
  if ( name == m.t_laceration       ) return new laceration_t       ( this, name, options_str ) ;
  if ( name == m.a_stealth          ) return new stealth_t          ( this, name, options_str ) ;
  if ( name == m.a_stim_boost       ) return new stim_boost_t       ( this, name, options_str ) ;

  // extended
  if ( name == m.a_adrenaline_probe ) return new adrenaline_probe_t ( this, name, options_str ) ;
  if ( name == m.t_cull             ) return new cull_t             ( this, name, options_str ) ;
  if ( name == m.a_overload_shot    ) return new overload_shot_t    ( this, name, options_str ) ;
  if ( name == m.a_shiv             ) return new shiv_t             ( this, name, options_str ) ;

  return base_t::create_action( name, options_str );
}

// class_t::init_talents ======================================

void class_t::init_talents()
{
  base_t::init_talents();

  // Lethality
  // t3
  talents.targeted_demolition       = find_talent( m.t_targeted_demolition  );
  // t4
  talents.hold_your_ground          = find_talent( m.t_hold_your_ground     );
  // t5
  talents.razor_rounds              = find_talent( m.t_razor_rounds         );

  // Medicine|Sawbones
  // t1
  talents.incisive_action           = find_talent( m.t_incisive_action );
  talents.precision_instruments     = find_talent( m.t_precision_instruments    );
  talents.imperial_education        = find_talent( m.t_imperial_education       );
  // t2
  talents.endorphin_rush            = find_talent( m.t_endorphin_rush           );
  talents.medical_consult           = find_talent( m.t_medical_consult          );
  talents.surgical_steadiness       = find_talent( m.t_surgical_steadiness      );
  talents.chem_resistant_inlays     = find_talent( m.t_chem_resistant_inlays    );
  // t3
  talents.prognosis_critical        = find_talent( m.t_prognosis_critical       );
  talents.kolto_probe               = find_talent( m.t_kolto_probe              );
  talents.sedatives                 = find_talent( m.t_sedatives                );
  // t4
  talents.patient_studies           = find_talent( m.t_patient_studies          );
  talents.medical_engineering       = find_talent( m.t_medical_engineering      );
  talents.evasive_imperative        = find_talent( m.t_evasive_imperative       );
  // t5
  talents.tox_scan                  = find_talent( m.t_tox_scan                 );
  talents.medical_therapy           = find_talent( m.t_medical_therapy          );
  talents.surgical_probe            = find_talent( m.t_surgical_probe           );
  talents.surgical_precision        = find_talent( m.t_surgical_precision       );
  // t6
  talents.med_shield                = find_talent( m.t_med_shield               );
  talents.accomplished_doctor       = find_talent( m.t_accomplished_doctor      );
  // t7
  talents.recuperative_nanotech     = find_talent( m.t_recuperative_nanotech    );

  // concealment
  talents.concealed_attacks         = find_talent( m.t_concealed_attacks        );
  talents.imperial_brew             = find_talent( m.t_imperial_brew            );
  talents.survival_training         = find_talent( m.t_survival_training        );
  talents.infiltrator               = find_talent( m.t_infiltrator              );
  talents.surgical_strikes          = find_talent( m.t_surgical_strikes         );
  talents.inclement_conditioning    = find_talent( m.t_inclement_conditioning   );
  talents.scouting                  = find_talent( m.t_scouting                 );
  talents.flanking                  = find_talent( m.t_flanking                 );
  talents.laceration                = find_talent( m.t_laceration               );
  talents.collateral_strike         = find_talent( m.t_collateral_strike        );
  talents.revitalizers              = find_talent( m.t_revitalizers             );
  talents.pin_down                  = find_talent( m.t_pin_down                 );
  talents.tactical_opportunity      = find_talent( m.t_tactical_opportunity     );
  talents.energy_screen             = find_talent( m.t_energy_screen            );
  talents.waylay                    = find_talent( m.t_waylay                   );
  talents.culling                   = find_talent( m.t_culling                  );
  talents.advanced_cloaking         = find_talent( m.t_advanced_cloaking        );
  talents.meticulously_kept_blades  = find_talent( m.t_meticulously_kept_blades );
  talents.jarring_strike            = find_talent( m.t_jarring_strike           );
  talents.acid_blade                = find_talent( m.t_acid_blade               );
}

// class_t::init_base =========================================

void class_t::init_base()
{
  base_t::init_base();

  attribute_multiplier_initial[ ATTR_CUNNING ] += 0.03 * talents.imperial_education -> rank();
}

// class_t::init_benefits =====================================

void class_t::init_benefits()
{
  base_t::init_benefits();
}

// class_t::init_buffs ========================================

void class_t::init_buffs()
{
  base_t::init_buffs();

  // buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

  buffs.acid_blade_coating = new buff_t( this , m.t_acid_blade + "_coating" , 1 ,  from_seconds( 20 ) );
  buffs.acid_blade_arpen   = new buff_t( this , m.t_acid_blade + "_arpen"   , 1 ,  from_seconds( 15 ) );
  buffs.stealth            = new buff_t( this , m.a_stealth                 , 1                       );
  buffs.stim_boost         = new buff_t( this , m.a_stim_boost              , 1 ,  from_seconds( 45 ) );
  buffs.tactical_advantage = new buff_t( this , m.a_tactical_advantage      , 2 ,  from_seconds( 10 ) );
}

// class_t::init_gains ========================================

void class_t::init_gains()
{
  base_t::init_gains();

  gains.endorphin_rush = get_gain( m.t_endorphin_rush );
  gains.combat_stims   = get_gain( m.a_combat_stims   );
  gains.revitalizers   = get_gain( m.t_revitalizers   );
  gains.stim_boost     = get_gain( m.a_stim_boost     );
}

// class_t::init_procs ========================================

void class_t::init_procs()
{
  base_t::init_procs();
}

// class_t::init_rng ==========================================

void class_t::init_rng()
{
  base_t::init_rng();

  bool is_op = ( type == IA_OPERATIVE );
  const char* collateral_strike  = is_op ? "collateral_strike"  : "flying_fists"  ;

  rngs.collateral_strike  = get_rng( collateral_strike  );
}

// class_t::init_actions ======================================

void class_t::init_actions()
{
  //=========================================================================
  //
  //   Please Mirror all changes between Empire/Republic
  //
  //=========================================================================

  const std::string sl = "/";

  if ( action_list_str.empty() )
  {
    action_list_default = true;

    action_list_str += "/stim,type=exotech_skill"
                       + sl + m.a_coordination
                       + "/snapshot_stats"
                       + "/stealth"
                       + sl + m.a_adrenaline_probe + ",if=energy<=60"
                       + sl + m.a_stim_boost + ",if=buff." + m.a_tactical_advantage + ".stack>1";

    if ( talents.acid_blade -> rank() )
      action_list_str += sl + m.t_acid_blade + ",if=!buff." + m.t_acid_blade + "_coating.up&!cooldown." + m.a_backstab + ".remains";
    action_list_str += "/use_relics/power_potion"
                       + sl + m.a_hidden_strike;


    if ( talents.flanking -> rank() )
      action_list_str += sl + m.a_backstab + ",if=energy>=65";
    if ( talents.acid_blade -> rank() )
      action_list_str += ",if=buff." + m.t_acid_blade + "_coating.up";

    if ( talents.weakening_blast -> rank() )
      action_list_str += sl + m.t_weakening_blast;

    action_list_str += sl + m.a_shiv + ",if=energy>=75&buff." + m.a_tactical_advantage + ".stack<2"
                       + sl + m.a_corrosive_dart + ",if=!ticking&energy>=75";

    if ( talents.corrosive_grenade -> rank() )
    action_list_str += sl + m.a_corrosive_grenade + ",if=!ticking&energy>=80";

    if ( talents.laceration -> rank() )
    {
      action_list_str += sl + m.t_laceration + ",if=energy>=75&buff." + m.a_tactical_advantage + ".stack>1";
      action_list_str += sl + m.t_laceration + ",if=energy>=75&cooldown." + m.a_shiv + ".remains<2";
    }

    if ( talents.acid_blade -> rank() )
      action_list_str += sl + m.t_acid_blade + ",if=!buff." + m.t_acid_blade + "_coating.up&energy>75";

    if ( talents.cull -> rank() )
      action_list_str += sl + m.t_cull + ",if=energy>=50&buff." +m.a_tactical_advantage + ".stack>=2"
                         "&(dot." + m.a_corrosive_dart + ".ticking|dot." + m.a_corrosive_dart + "_weak.ticking)"
                         "&(dot." + m.a_corrosive_grenade + ".ticking|dot." + m.a_corrosive_grenade + "_weak.ticking)";

    if ( ! talents.flanking -> rank() )
      action_list_str += sl + m.a_backstab + ",if=energy>=70";

    action_list_str += sl + m.a_orbital_strike + ",if=energy>65";
    // energy dumps
    action_list_str += sl + m.a_explosive_probe + ",if=energy>"; // should use cover before, but not enforcing cover yet
    if ( set_bonus.rakata_enforcers -> four_pc() )
      action_list_str += "100";
    else
      action_list_str += "95";
    action_list_str += "&cooldown." + m.a_shiv + ".remains>2";
    action_list_str += sl + m.a_overload_shot + ",if=energy>";
    if ( set_bonus.rakata_enforcers -> four_pc() )
      action_list_str += "100";
    else
      action_list_str += "95";
    action_list_str += "&cooldown." + m.a_shiv + ".remains>2";
    action_list_str += sl +m.a_rifle_shot;

    if ( false )
    {
      switch ( primary_tree() )
      {
      case TREE_MEDICINE:
        break;

      case TREE_CONCEALMENT:
        break;

      case TREE_LETHALITY:
        break;

      default: break;
      }
    }
  }

  register_attack_callback( RESULT_ALL_MASK, new all_attack_callback_t( this ) );

  base_t::init_actions();
}

// class_t::reset =============================================

void class_t::reset()
{
  acid_blade_poison = 0;
  base_t::reset();
}

// class_t::primary_role ======================================

role_type class_t::primary_role() const
{
  switch ( base_t::primary_role() )
  {
  case ROLE_TANK:
    return ROLE_TANK;
  case ROLE_DPS:
    return ROLE_DPS;
  case ROLE_HYBRID:
    return ROLE_HYBRID;
  default:
     if ( primary_tree() == TREE_MEDICINE )
        return ROLE_HEAL;
     if ( primary_tree() == TREE_CONCEALMENT )
       return ROLE_DPS;
     if ( primary_tree() == TREE_LETHALITY )
       return ROLE_DPS;
    break;
  }

  return ROLE_HYBRID;
}

// class_t::armor_penetration =================================

double class_t::armor_penetration() const
{
  double arpen = base_t::armor_penetration();

  if ( buffs.acid_blade_arpen -> up() )
    arpen *= ( 1 - 0.3 );

  return arpen;
}

// class_t::energy_regen_per_second ===========================

double class_t::energy_regen_per_second() const
{
  double eps = base_t::energy_regen_per_second();

  if ( buffs.stim_boost -> check() )
    eps += stim_boost_t::tick_amount( *this ) * ( 1.0 / 3 );

  return eps;
}

// class_t::composite_player_multiplier ===========================

double class_t::composite_player_multiplier(school_type school, ::action_t *a) const
{
  double multiplier = base_t::composite_player_multiplier(school, a);

  if ( buffs.tactical_advantage -> up() )
    multiplier += 0.02;

  return multiplier;
}

// class_t::create_mirror =======================================

void class_t::create_mirror()
{
  base_t::create_mirror();

  bool op = type == IA_OPERATIVE;

  // ABILITY              =    ? OPERATIVE LABEL      : SCOUNDREL LABEL           ;
  m.a_backstab            = op ? "backstab"           : "back_blast"              ;
  m.a_combat_stims        = op ? "combat_stims"       : "street_tough"            ;
  m.a_hidden_strike       = op ? "hidden_strike"      : "shoot_first"             ;
  m.a_stealth             = op ? "stealth"            : "stealth"                 ;
  m.a_stim_boost          = op ? "stim_boost"         : "pugnacity"               ;
  m.a_tactical_advantage  = op ? "tactical_advantage" : "upper_hand"              ;

  // gains
  m.t_endorphin_rush      = op  ? "endorphin_rush"    : "keep_cool"               ;

  // Lethality|Dirty Fighting
  // t3
  m.t_targeted_demolition       = op ? "comtat_stims"             : "street_tough"          ;
  // t4
  m.t_hold_your_ground          = op ? "escape_plan"              : "smuggled_defenses"     ;
  // t5
  m.t_razor_rounds              = op ? "license_to_kill"          : "rough_and_tumble"      ;

  // Medicine|Sawbones
  // t1
  m.t_incisive_action           = op ? "incisive_action"          : "exploratory_surgery"   ;
  m.t_precision_instruments     = op ? "precision_instruments"    : "anatomy_lessons"       ;
  m.t_imperial_education        = op ? "imperial_education"       : "bedside_manner"        ;
  // t2
  m.t_endorphin_rush            = op ? "endorphin_rush"           : "keep_cool"             ;
  m.t_medical_consult           = op ? "medical_consult"          : "healing_hand"          ;
  m.t_surgical_steadiness       = op ? "surgical_steadiness"      : "smuggled_technology"   ;
  m.t_chem_resistant_inlays     = op ? "chem_resistant_inlays"    : "scar_tissue"           ;
  // t3
  m.t_prognosis_critical        = op ? "prognosis_critical"       : "prognosis_critical"    ;
  m.t_kolto_probe               = op ? "kolto_probe"              : "slow_release_medpac"   ;
  m.t_sedatives                 = op ? "sedatives"                : "sedatives"             ;
  // t4
  m.t_patient_studies           = op ? "patient_studies"          : "patient_studies"       ;
  m.t_medical_engineering       = op ? "medical_engineering"      : "medpac_mastery"        ;
  m.t_evasive_imperative        = op ? "evasive_imperative"       : "scramble"              ;
  // t5
  m.t_tox_scan                  = op ? "tox_scan"                 : "psych_meds"            ;
  m.t_medical_therapy           = op ? "medical_therapy"          : "homegrown_pharmacology";
  m.t_surgical_probe            = op ? "surgical_probe"           : "emergency_medpac"      ;
  m.t_surgical_precision        = op ? "surgical_precision"       : "emergent_emergencies"  ;
  // t6
  m.t_med_shield                = op ? "med_shield"               : "med_screen"            ;
  m.t_accomplished_doctor       = op ? "accomplished_doctor"      : "accomplished_sawbones" ;
  // t7
  m.t_recuperative_nanotech     = op ? "recuperative_nanotech"    : "kolto_cloud"           ;

  // Concealment|Scrapper
  // t1
  m.t_concealed_attacks         = op ? "concealed_attacks"        : "element_of_surprise"   ;
  m.t_imperial_brew             = op ? "imperial_brew"            : "browbeater"            ;
  m.t_survival_training         = op ? "survival_training"        : "survivors_scars"       ;
  // t2
  m.t_infiltrator               = op ? "infiltrator"              : "sneaky"                ;
  m.t_surgical_strikes          = op ? "surgical_strikes"         : "scrappy"               ;
  m.t_inclement_conditioning    = op ? "inclement_conditioning"   : "brawlers_grit"         ;
  m.t_scouting                  = op ? "scouting"                 : "shifty_eyed"           ;
  // t3
  m.t_flanking                  = op ? "flanking"                 : "flanking"              ;
  m.t_laceration                = op ? "laceration"               : "sucker_punch"          ;
  m.t_collateral_strike         = op ? "collateral_strike"        : "flying_fists"          ;
  m.t_revitalizers              = op ? "revitalizers"             : "surprise_comeback"     ;
  // t4
  m.t_pin_down                  = op ? "pin_down"                 : "stopping_power"        ;
  m.t_tactical_opportunity      = op ? "tactical_opportunity"     : "round_two"             ;
  m.t_energy_screen             = op ? "energy_screen"            : "fight_or_flight"       ;
  // t5
  m.t_waylay                    = op ? "waylay"                   : "sawed_off"             ;
  m.t_culling                   = op ? "culling"                  : "turn_the_tables"       ;
  m.t_advanced_cloaking         = op ? "advanced_cloaking"        : "flee_the_scene"        ;
  // t6
  m.t_meticulously_kept_blades  = op ? "meticulously_kept_blades" : "underdog"              ;
  m.t_jarring_strike            = op ? "jarring_strike"           : "k_o"                   ;
  // t7
  m.t_acid_blade                = op ? "acid_blade"               : "flechette_round"       ;
}

// class_t::create_talents ====================================

void class_t::create_talents()
{
  base_t::create_talents();

  // medicine
  const talentinfo_t medicine_tree[] = {
    { m.t_incisive_action       , 2 }, { m.t_precision_instruments , 2 }, { m.t_imperial_education  , 3 },
    { m.t_endorphin_rush        , 2 }, { m.t_medical_consult       , 3 }, { m.t_surgical_steadiness , 2 }, { m.t_chem_resistant_inlays , 2 },
    { m.t_prognosis_critical    , 2 }, { m.t_kolto_probe           , 1 }, { m.t_sedatives           , 2 },
    { m.t_patient_studies       , 2 }, { m.t_medical_engineering   , 3 }, { m.t_evasive_imperative  , 2 },
    { m.t_tox_scan              , 1 }, { m.t_medical_therapy       , 2 }, { m.t_surgical_probe      , 1 }, { m.t_surgical_precision    , 1 },
    { m.t_med_shield            , 2 }, { m.t_accomplished_doctor   , 3 },
    { m.t_recuperative_nanotech , 1 },
  };
  init_talent_tree( IA_OPERATIVE_MEDICINE, medicine_tree );

  // concealment
  const talentinfo_t concealment_tree[] = {
    { m.t_concealed_attacks        , 2 },  { m.t_imperial_brew        , 3 },  { m.t_survival_training      , 3 },
    { m.t_infiltrator              , 3 },  { m.t_surgical_strikes     , 2 },  { m.t_inclement_conditioning , 2 },  { m.t_scouting     , 2 },
    { m.t_flanking                 , 1 },  { m.t_laceration           , 1 },  { m.t_collateral_strike      , 2 },  { m.t_revitalizers , 1 },
    { m.t_pin_down                 , 2 },  { m.t_tactical_opportunity , 2 },  { m.t_energy_screen          , 1 },
    { m.t_waylay                   , 1 },  { m.t_culling              , 2 },  { m.t_advanced_cloaking      , 2 },
    { m.t_meticulously_kept_blades , 3 },  { m.t_jarring_strike       , 2 },
    { m.t_acid_blade               , 1 },
  };
  init_talent_tree( IA_OPERATIVE_CONCEALMENT , concealment_tree );
}

} // namespace scoundrel_operative ==========================================

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_scoundrel ===============================================

player_t* player_t::create_scoundrel( sim_t* sim, const std::string& name, race_type r )
{
  return new scoundrel_operative::class_t( sim, S_SCOUNDREL, name, r );
}

// player_t::create_operative ===============================================

player_t* player_t::create_operative( sim_t* sim, const std::string& name, race_type r )
{
  return new scoundrel_operative::class_t( sim, IA_OPERATIVE, name, r );
}

// player_t::scoundrel_operative_init =======================================

void player_t::scoundrel_operative_init( sim_t* /* sim */ )
{
  // TODO add buff here like in sc_commando_mercenary ?
}

// player_t::scoundrel_operative_combat_begin ===============================

void player_t::scoundrel_operative_combat_begin( sim_t* /* sim */ )
{

}
