// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "agent_smug.hpp"

namespace scoundrel_operative { // ==========================================

class class_t;

struct targetdata_t : public agent_smug::targetdata_t
{
  buff_t* debuff_weakening_blast;

  dot_t dot_acid_blade_poison;
  dot_t dot_corrosive_dart;
  dot_t dot_corrosive_dart_weak;
  dot_t dot_corrosive_grenade;
  dot_t dot_corrosive_grenade_weak;
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
    gain_t* stim_boost;
    gain_t* revitalizers;
    gain_t* combat_stims;
  } gains;

  // Procs
  struct procs_t
  {
    proc_t* corrosive_microbes;
  } procs;

  // RNGs
  struct rngs_t
  {
    rng_t* collateral_strike;
    rng_t* corrosive_microbes;
  } rngs;

  // Benefits
  struct benefits_t:base_t::benefits_t
  {
    benefit_t* wb_poison_ticks;
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
    talent_t* surical_steadiness;
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

  // Abilities
  struct abilities_t:base_t::abilities_t
  {
    std::string acid_blade;
    std::string acid_blade_arpen;
    std::string acid_blade_coating;
    std::string acid_blade_poison;
    std::string backstab;
    std::string combat_stims;
    std::string cull;
    std::string hidden_strike;
    std::string laceration;
    std::string revitalizers;
    std::string stealth;
    std::string stim_boost;
    std::string tactical_advantage;
    std::string weakening_blast;
  } abilities;

  action_t* acid_blade_poison;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    base_t( sim, pt == IA_OPERATIVE ? IA_OPERATIVE : S_SCOUNDREL, name, rt, buffs, gains, benefits, talents, abilities ),
    buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents(), acid_blade_poison()
  {
    tree_type[ IA_OPERATIVE_MEDICINE    ] = TREE_MEDICINE;
    tree_type[ IA_OPERATIVE_CONCEALMENT ] = TREE_CONCEALMENT;
    tree_type[ IA_LETHALITY             ] = TREE_LETHALITY;

    create_talents();
    create_options();
  }

  // Character Definition
  targetdata_t* new_targetdata( player_t& target ) // override
  { return new targetdata_t( *this, target ); }

  ::action_t* create_action( const std::string& name, const std::string& options );
  void      init_talents();
  void      init_abilities();
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
  void      create_talents();
  role_type primary_role() const;
};


targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  agent_smug::targetdata_t( source, target )
{
  bool is_op = ( source.type == IA_OPERATIVE );
  const char* weakening_blast        = is_op ? "weakening_blast"        : "hemorrhaging_blast" ; 
  const char* corrosive_dart_weak    = is_op ? "corrosive_dart_weak"    : "vital_shot_weak"    ;
  const char* corrosive_grenade      = is_op ? "corrosive_grenade"      : "shrap_bomb"         ; 
  const char* corrosive_grenade_weak = is_op ? "corrosive_grenade_weak" : "shrap_bomb_weak"    ; 
  const char* stim_boost             = is_op ? "stim_boost"             : "pugnacity"          ; 

  debuff_weakening_blast     = new buff_t ( this, weakening_blast, 10, from_seconds (  15 ) ); 

  dot_acid_blade_poison      = dot_t ( source.abilities.acid_blade_poison , &source ); 
  dot_corrosive_dart         = dot_t ( source.abilities.corrosive_dart    , &source ); 
  dot_corrosive_dart_weak    = dot_t ( corrosive_dart_weak                , &source ); 
  dot_corrosive_grenade      = dot_t ( corrosive_grenade                  , &source ); 
  dot_corrosive_grenade_weak = dot_t ( corrosive_grenade_weak             , &source ); 
  dot_stim_boost             = dot_t ( stim_boost                         , &source ); 

  add( *debuff_weakening_blast    );
  add( dot_acid_blade_poison      );
  add( dot_corrosive_dart         );
  add( dot_corrosive_dart_weak    );
  add( dot_corrosive_grenade      );
  add( dot_corrosive_grenade_weak );
  add( dot_stim_boost             );
}

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

struct poison_attack_t : public tech_attack_t
{
  poison_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_INTERNAL ) :
    tech_attack_t( n, p, s )
  {
    may_crit       =  false;
    tick_may_crit  =  true;
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


// Consume Acid Blade Poison Attack | ??? ===================================

struct consume_acid_blade_attack_t : public tech_attack_t
{
  consume_acid_blade_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_KINETIC ) :
    tech_attack_t( n, p, s )
  {}

  void execute()
  {
    tech_attack_t::execute();

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
  struct acid_blade_poison_t : public poison_attack_t
  {
    acid_blade_poison_t( class_t* p, const std::string& n ) :
      poison_attack_t( n, p )
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
    action_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_INTERNAL ),
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

// Stealth | ??? ============================================================

struct stealth_t : public action_t
{
  stealth_t( class_t* p, const std::string& n, const std::string& options_str ) :
    action_t( n, p, tech_policy, RESOURCE_ENERGY, SCHOOL_NONE )
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

    return action_t::ready();
  }

  void execute()
  {
    p() -> buffs.stealth -> trigger();
    action_t::execute();
  }
};

// Shiv | ??? ===============================================================

struct shiv_t : public tech_attack_t
{
  shiv_t( class_t* p, const std::string& n, const std::string& options_str ) :
    tech_attack_t( n, p )
  {
    rank_level_list = { 2, 5, 8, 11, 14, 19, 29, 38, 50 };

    parse_options( options_str );

    base_cost                    = 15;
    cooldown -> duration         = from_seconds( 6.0 );
    range                        = 4.0;
    dd.standardhealthpercentmin  = 0.148;
    dd.standardhealthpercentmax  = 0.188;
    dd.power_mod                 = 1.68;
    base_multiplier             += p -> talents.surgical_strikes -> rank() * 0.02
                                 + p -> talents.razor_edge       -> rank() * 0.04;
  }

  virtual void execute()
  {
    tech_attack_t::execute();

    // TODO check if granted on misses etc?
    if ( result_is_hit() )
      p() -> buffs.tactical_advantage -> trigger();
  }
};

// Backstab | ??? ===========================================================

struct backstab_t : public consume_acid_blade_attack_t
{
  backstab_t( class_t* p, const std::string& n, const std::string& options_str ) :
    consume_acid_blade_attack_t( n, p )
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
                               +  p -> set_bonus.rakata_enforcers       -> two_pc() ? 0.15 : 0;
    crit_bonus                 += p -> talents.meticulously_kept_blades -> rank()   * 0.1;
  }
};

// Laceration | ??? =========================================================

struct laceration_t : public tech_attack_t
{
  struct collateral_strike_t : public tech_attack_t
  {
    collateral_strike_t( class_t* p, const std::string& n, const std::string& options_str ) :
      tech_attack_t( n, p )
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
      tech_attack_t::execute();

      // if target is poisoned regrant TA
      targetdata_t* td = targetdata();
      if ( td -> dot_acid_blade_poison.ticking || td -> dot_corrosive_dart.ticking )
        p() -> buffs.tactical_advantage -> trigger();
    }
  };

  collateral_strike_t* collateral_strike;

  laceration_t( class_t* p, const std::string& n, const std::string& options_str ) :
    tech_attack_t( n, p ), collateral_strike( 0 )
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
    if ( ! p() -> buffs.tactical_advantage -> check() )
      return false;

    return tech_attack_t::ready();
  }

  virtual void execute()
  {

    tech_attack_t::execute();

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
  hidden_strike_t( class_t* p, const std::string& n, const std::string& options_str ) :
    consume_acid_blade_attack_t( n, p )
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

    return action_t::ready();
  }

  virtual void execute()
  {
    class_t& p = *cast();
    p.buffs.stealth -> up();
    consume_acid_blade_attack_t::execute();
    p.buffs.tactical_advantage -> trigger();
  }
  // TODO check for talent and trigger knockdown (jarring strike)
};

// Fragmentation Grenade | ??? ==============================================

struct fragmentation_grenade_t : public tech_attack_t
{
  fragmentation_grenade_t( class_t* p, const std::string& n, const std::string& options_str ) :
    tech_attack_t( n, p )
  {
    parse_options( options_str );

    base_cost                   = 20;
    cooldown -> duration        = from_seconds( 6.0 );
    range                       = 30.0;
    dd.standardhealthpercentmin = 0.109;
    dd.standardhealthpercentmax = 0.149;
    dd.power_mod                = 1.29;
    aoe                         = 4;
  }
};

// Corrosive Grenade | ??? ==================================================

struct corrosive_grenade_t : public poison_attack_t
{
  corrosive_grenade_t* corrosive_grenade_weak;

  corrosive_grenade_t( class_t* p, const std::string& n, const std::string& options_str, bool weak=false ) :
    poison_attack_t( n, p ), corrosive_grenade_weak()
  {
    check_talent( p -> talents.corrosive_grenade -> rank() );

    parse_options( options_str );

    range = 30.0;
    base_tick_time = from_seconds( 3 );

    if ( weak )
    {
      // infinite?
      range                       = 30.0;
      base_cost                   = 0;
      td.standardhealthpercentmin = 
      td.standardhealthpercentmax = 0.0065 * p -> talents.lingering_toxins -> rank();
      td.power_mod                = 0.065  * p -> talents.lingering_toxins -> rank();
      num_ticks                   = 3;
      background                  = true;
      trigger_gcd                 = timespan_t::zero();
    }
    else
    {
      base_cost                   = 20;
      cooldown -> duration        = from_seconds( 12.0 );
      td.standardhealthpercentmin = 
      td.standardhealthpercentmax = 0.032;
      td.power_mod                = 0.32;
      num_ticks                   = 7;
      tick_zero                   = true;
      // TEST: maybe not limited?
      aoe                         = 5;

      if ( p -> talents.lingering_toxins -> rank() )
        corrosive_grenade_weak = new corrosive_grenade_t( p, n + "_weak", options_str, true );
    }
  }

  virtual void last_tick( dot_t* d )
  {
    poison_attack_t::last_tick( d );

    if ( corrosive_grenade_weak )
      corrosive_grenade_weak -> execute();
  }

  virtual void execute()
  {
    if ( corrosive_grenade_weak )
      targetdata() -> dot_corrosive_grenade_weak.cancel();

    poison_attack_t::execute();
  }
};

// Corrosive Dart | ??? =====================================================

struct corrosive_dart_t : public poison_attack_t
{
  corrosive_dart_t* corrosive_dart_weak;

  corrosive_dart_t( class_t* p, const std::string& n, const std::string& options_str, bool weak=false ) :
    poison_attack_t( n, p ),
    corrosive_dart_weak()
  {
    rank_level_list = { 5, 7, 10, 13, 17, 23, 35, 45, 50 };

    parse_options( options_str );

    range = 30.0;
    base_tick_time = from_seconds( 3.0 );

    if ( weak )
    {
      // infinite?
      range                       = 30.0;
      base_cost                   = 0;
      td.standardhealthpercentmin = 
      td.standardhealthpercentmax = 0.01;
      td.power_mod                = 0.1;
      num_ticks                   = 3 + p -> talents.lethal_injectors -> rank();
      background                  = true;
      trigger_gcd                 = timespan_t::zero();
    }
    else
    {
      base_cost                   = 20;
      td.standardhealthpercentmin = 
      td.standardhealthpercentmax = 0.04;
      td.power_mod                = 0.4;
      num_ticks                   = 5 + p -> talents.lethal_injectors -> rank();

      if ( p -> talents.lingering_toxins -> rank() )
        corrosive_dart_weak = new corrosive_dart_t( p, n + "_weak", options_str, true );
    }
  }

  virtual void tick( dot_t* d )
  {
    poison_attack_t::tick( d );

    class_t& p = *cast();

    if ( p.talents.corrosive_microbes -> rank()
         && p.rngs.corrosive_microbes -> roll( 0.125 * p.talents.corrosive_microbes -> rank() ) )
    {
      p.procs.corrosive_microbes -> occur();
      extra_tick();
    }
  }

  virtual void last_tick( dot_t* d )
  {
    poison_attack_t::last_tick( d );

    if ( corrosive_dart_weak )
      corrosive_dart_weak -> execute();
  }

  virtual void execute()
  {
    if ( corrosive_dart_weak )
      targetdata() -> dot_corrosive_dart_weak.cancel();

    poison_attack_t::execute();
  }
};

// Cull | ??? ===============================================================

struct cull_t : public range_attack_t
{
  struct cull_extra_t : public tech_attack_t
  {
    cull_extra_t( class_t* p, const std::string& n ) :
      tech_attack_t( n, p, SCHOOL_INTERNAL )
    {
      dd.standardhealthpercentmin =  
      dd.standardhealthpercentmax =  0.066;
      dd.power_mod                =  0.66;
      dual                        =
      background                  = true;
      trigger_gcd                 =  timespan_t::zero();
      base_multiplier             += .03 * p -> talents.cut_down->rank();
    }
  };

  cull_extra_t* extra_strike;

  cull_t( class_t* p, const std::string& n, const std::string& options_str ) :
    range_attack_t( n, p ),
    extra_strike( new cull_extra_t( p, n + "_extra" ) )
  {
    check_talent( p -> talents.cull -> rank() );

    parse_options( options_str );

    base_cost                   =  25 - 3 * p -> talents.license_to_kill -> rank();
    range                       =  10.0;
    weapon                      =  &( player->main_hand_weapon );
    weapon_multiplier           =  -0.1;
    dd.standardhealthpercentmin =  
    dd.standardhealthpercentmax =  0.135;
    dd.power_mod                =  1.35;
    base_multiplier             += .03 * p -> talents.cut_down->rank();

    add_child( extra_strike );
  }

  virtual bool ready()
  {
    if ( ! p() -> buffs.tactical_advantage -> check() )
      return false;

    return range_attack_t::ready();
  }

  virtual void execute()
  {
    range_attack_t::execute();

    if ( result_is_hit() )
    {
      p() -> buffs.tactical_advantage -> decrement();

      targetdata_t* td = targetdata();
      assert( ! td -> dot_acid_blade_poison.ticking );
      if ( td -> dot_corrosive_dart.ticking || td -> dot_corrosive_dart_weak.ticking )
        extra_strike -> execute();
      if ( td -> dot_corrosive_grenade.ticking || td -> dot_corrosive_grenade_weak.ticking )
        extra_strike -> execute();
    }
  }
};

// Overload Shot | ??? ======================================================

struct overload_shot_t : public range_attack_t
{
  overload_shot_t( class_t* p, const std::string& n, const std::string& options_str) :
    range_attack_t( n, p )
  {
    rank_level_list = { 8, 12, 16, 22, 31, 40, 50 };

    parse_options( options_str );

    base_cost                   =  17;
    range                       =  10.0;
    dd.standardhealthpercentmin =
    dd.standardhealthpercentmax = 0.124;
    dd.power_mod                =  1.24;
    weapon                      =  &( player -> main_hand_weapon );
    weapon_multiplier           =  -0.17;
    base_multiplier             += 0.15 // passive: skirmisher
                                +  .03 * p -> talents.cut_down->rank();
  }
};

// Stim Boost | ??? =========================================================

struct stim_boost_t : public action_t
{
  static double tick_amount( const class_t& op )
  { return 3 + 0.5 * op.talents.culling -> rank(); }

  stim_boost_t( class_t* p, const std::string& n, const std::string& options_str ) :
    action_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    parse_options( options_str );

    cooldown -> duration = from_seconds( 35 - p -> talents.combat_stims -> rank() * 7.5 );
    use_off_gcd          = true;
    trigger_gcd          = timespan_t::zero();
    num_ticks            = 15;
    base_tick_time       = from_seconds( 3 );
  }

  virtual bool ready()
  {
    if ( ! p() -> buffs.tactical_advantage -> check() )
      return false;

    return action_t::ready();
  }

  virtual void tick( dot_t* d )
  {
    action_t::tick(d);

    class_t& p = *cast();

    p.resource_gain( RESOURCE_ENERGY, tick_amount( p ), p.gains.stim_boost );
    if ( p.talents.revitalizers -> rank() )
      p.resource_gain( RESOURCE_HEALTH, p.resource_max[ RESOURCE_HEALTH ] * 0.02, p.gains.revitalizers );
  }

  virtual void execute()
  {
    action_t::execute();

    class_t& p = *cast();
    p.buffs.stim_boost -> trigger();
    p.buffs.tactical_advantage -> decrement();
    if ( p.talents.combat_stims -> rank() )
      p.resource_gain( RESOURCE_ENERGY, 5 * p.talents.combat_stims -> rank(), p.gains.combat_stims );
  }
};

// Weakening Blast | ??? ====================================================

struct weakening_blast_t : public range_attack_t
{
  weakening_blast_t( class_t* p, const std::string& n, const std::string& options_str) :
    range_attack_t( n, p )
  {
    check_talent( p -> talents.weakening_blast -> rank() );

    parse_options( options_str );

    range                       = 10.0;
    cooldown -> duration        = from_seconds( 15 );
    dd.standardhealthpercentmin =
    dd.standardhealthpercentmax = 0.087;
    dd.power_mod                = 0.87;
    weapon                      = &( player -> main_hand_weapon );
    weapon_multiplier           = -0.42;
  }

  virtual void execute()
  {
    range_attack_t::execute();

    if ( result_is_hit() )
      targetdata() -> debuff_weakening_blast -> trigger( 10 );
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

struct poison_tick_crit_callback_t : public action_callback_t
{
  poison_tick_crit_callback_t( class_t* p ) :
    action_callback_t( p )
  {}

  virtual void trigger (::action_t* a, void* /* call_data */)
  {
    if (a->name_str == p()->abilities.acid_blade_poison)
    {
      p()->resource_gain( RESOURCE_ENERGY, p()->talents.lethal_purpose -> rank(), p()->gains.lethal_purpose );
    }
  }
};

// ==========================================================================
// scoundrel_operative Character Definition
// ==========================================================================

// class_t::create_action =====================================

::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  if ( name == abilities.acid_blade            ) return new acid_blade_t            ( this, name, options_str ) ;
  if ( name == abilities.backstab              ) return new backstab_t              ( this, name, options_str ) ;
  if ( name == abilities.corrosive_dart        ) return new corrosive_dart_t        ( this, name, options_str ) ;
  if ( name == abilities.corrosive_grenade     ) return new corrosive_grenade_t     ( this, name, options_str ) ;
  if ( name == abilities.cull                  ) return new cull_t                  ( this, name, options_str ) ;
  if ( name == abilities.fragmentation_grenade ) return new fragmentation_grenade_t ( this, name, options_str ) ;
  if ( name == abilities.hidden_strike         ) return new hidden_strike_t         ( this, name, options_str ) ;
  if ( name == abilities.laceration            ) return new laceration_t            ( this, name, options_str ) ;
  if ( name == abilities.overload_shot         ) return new overload_shot_t         ( this, name, options_str ) ;
  if ( name == abilities.shiv                  ) return new shiv_t                  ( this, name, options_str ) ;
  if ( name == abilities.stealth               ) return new stealth_t               ( this, name, options_str ) ;
  if ( name == abilities.stim_boost            ) return new stim_boost_t            ( this, name, options_str ) ;
  if ( name == abilities.weakening_blast       ) return new weakening_blast_t       ( this, name, options_str ) ;

  return base_t::create_action( name, options_str );
}

// class_t::init_abilities =======================================

void class_t::init_abilities()
{
  base_t::init_abilities();

  bool op = type == IA_OPERATIVE;

  // ABILITY                   =    ? OPERATIVE LABEL      : SCOUNDREL LABEL           ;
  abilities.acid_blade         = op ? "acid_blade"         : "flechette_round"         ; 
  abilities.acid_blade_arpen   = op ? "acid_blade_arpen"   : "flechette_round_arpen"   ; 
  abilities.acid_blade_coating = op ? "acid_blade_coating" : "flechette_round_coating" ; 
  abilities.acid_blade_poison  = op ? "acid_blade_poison"  : "flechette_round_poison"  ; 
  abilities.backstab           = op ? "backstab"           : "back_blast"              ; 
  abilities.combat_stims       = op ? "combat_stims"       : "street_tough"            ; 
  abilities.cull               = op ? "cull"               : "wounding_shot"           ; 
  abilities.hidden_strike      = op ? "hidden_strike"      : "shoot_first"             ; 
  abilities.laceration         = op ? "laceration"         : "sucker_punch"            ; 
  abilities.revitalizers       = op ? "revitalizers"       : "surprise_comeback"       ; 
  abilities.stealth            = op ? "stealth"            : "stealth"                 ; 
  abilities.stim_boost         = op ? "stim_boost"         : "pugnacity"               ; 
  abilities.tactical_advantage = op ? "tactical_advantage" : "upper_hand"              ; 
  abilities.weakening_blast    = op ? "weakening_blast"    : "hemorrhaging_blast"      ; 
}

// class_t::init_talents ======================================

void class_t::init_talents()
{
  base_t::init_talents();

  // Medicine|Sawbones
  // t1
  talents.incisive_action           = find_talent( "Incisive Action"          );
  talents.precision_instruments     = find_talent( "Precision Instruments"    );
  talents.imperial_education        = find_talent( "Imperial Education"       );
  // t2
  talents.endorphin_rush            = find_talent( "Endorphin Rush"           );
  talents.medical_consult           = find_talent( "Medical Consult"          );
  talents.surical_steadiness        = find_talent( "Surgical Steadiness"      );
  talents.chem_resistant_inlays     = find_talent( "Chem-resistant Inlays"    );
  // t3
  talents.prognosis_critical        = find_talent( "Prognosis Critical"       );
  talents.kolto_probe               = find_talent( "Kolto Probe"              );
  talents.sedatives                 = find_talent( "Sedatives"                );
  // t4
  talents.patient_studies           = find_talent( "Patient Studies"          );
  talents.medical_engineering       = find_talent( "Medical Engineering"      );
  talents.evasive_imperative        = find_talent( "Evasive Imperative"       );
  // t5
  talents.tox_scan                  = find_talent( "Tox Scan"                 );
  talents.medical_therapy           = find_talent( "Medical Therapy"          );
  talents.surgical_probe            = find_talent( "Surgical Probe"           );
  talents.surgical_precision        = find_talent( "Surgical Precision"       );
  // t6
  talents.med_shield                = find_talent( "Med Shield"               );
  talents.accomplished_doctor       = find_talent( "Accomplished Doctor"      );
  // t7
  talents.recuperative_nanotech     = find_talent( "Recuperative Nanotech"    );

  // Concealment
  talents.concealed_attacks         = find_talent( "Concealed Attacks"        );
  talents.imperial_brew             = find_talent( "Imperial Brew"            );
  talents.survival_training         = find_talent( "Survival Training"        );
  talents.infiltrator               = find_talent( "Infiltrator"              );
  talents.surgical_strikes          = find_talent( "Surgical Strikes"         );
  talents.inclement_conditioning    = find_talent( "Inclement Conditioning"   );
  talents.scouting                  = find_talent( "Scouting"                 );
  talents.flanking                  = find_talent( "Flanking"                 );
  talents.laceration                = find_talent( "Laceration"               );
  talents.collateral_strike         = find_talent( "Collateral Strike"        );
  talents.revitalizers              = find_talent( "Revitalizers"             );
  talents.pin_down                  = find_talent( "Pin Down"                 );
  talents.tactical_opportunity      = find_talent( "Tactical Opportunity"     );
  talents.energy_screen             = find_talent( "Energy Screen"            );
  talents.waylay                    = find_talent( "Waylay"                   );
  talents.culling                   = find_talent( "Culling"                  );
  talents.advanced_cloaking         = find_talent( "Advanced Cloaking"        );
  talents.meticulously_kept_blades  = find_talent( "Meticulously Kept Blades" );
  talents.jarring_strike            = find_talent( "Jarring Strike"           );
  talents.acid_blade                = find_talent( "Acid Blade"               );
}

// class_t::init_base =========================================

void class_t::init_base()
{
  base_t::init_base();

  attribute_multiplier_initial[ ATTR_CUNNING ] += 0.03 * talents.imperial_education -> rank();
  set_base_crit( get_base_crit() + 0.02 * talents.lethality -> rank() );
  set_base_alacrity( get_base_alacrity() + 0.02 * talents.deadly_directive -> rank() );
}

// class_t::init_benefits =====================================

void class_t::init_benefits()
{
  base_t::init_benefits();

  benefits.devouring_microbes_ticks = get_benefit( "Poison ticks with Devouring Microbes" );
  benefits.wb_poison_ticks          = get_benefit( "Poison ticks with Weakening Blast" );
}

// class_t::init_buffs ========================================

void class_t::init_buffs()
{
  base_t::init_buffs();

  // buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

  buffs.acid_blade_coating = new buff_t( this , abilities.acid_blade_coating , 1 ,  from_seconds( 20 ) );
  buffs.acid_blade_arpen   = new buff_t( this , abilities.acid_blade_arpen   , 1 ,  from_seconds( 15 ) );
  buffs.stealth            = new buff_t( this , abilities.stealth            , 1                       );
  buffs.stim_boost         = new buff_t( this , abilities.stim_boost         , 1 ,  from_seconds( 45 ) );
  buffs.tactical_advantage = new buff_t( this , abilities.tactical_advantage , 2 ,  from_seconds( 10 ) );
}

// class_t::init_gains ========================================

void class_t::init_gains()
{
  base_t::init_gains();

  gains.combat_stims = get_gain( abilities.combat_stims );
  gains.revitalizers = get_gain( abilities.revitalizers );
  gains.stim_boost   = get_gain( abilities.stim_boost   );
}

// class_t::init_procs ========================================

void class_t::init_procs()
{
  base_t::init_procs();

  bool is_op = ( type == IA_OPERATIVE );
  const char* corrosive_microbes = is_op ? "Corrosive Microbes tick" : "Mortal Wounds tick" ; 

  procs.corrosive_microbes = get_proc( corrosive_microbes );
}

// class_t::init_rng ==========================================

void class_t::init_rng()
{
  base_t::init_rng();

  bool is_op = ( type == IA_OPERATIVE );
  const char* collateral_strike  = is_op ? "collateral_strike"  : "flying_fists"  ; 
  const char* corrosive_microbes = is_op ? "corrosive_microbes" : "mortal_wounds" ; 

  rngs.collateral_strike  = get_rng( collateral_strike  );
  rngs.corrosive_microbes = get_rng( corrosive_microbes );
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
                       + sl + abilities.coordination +
                       "/snapshot_stats";

    action_list_str += "/stealth"
                       + sl + abilities.adrenaline_probe + ",if=energy<=60"
                       + sl + abilities.stim_boost + ",if=buff." + abilities.tactical_advantage + ".stack>=2";

    if ( talents.acid_blade -> rank() )
      action_list_str += sl + abilities.acid_blade + ",if=!buff." + abilities.acid_blade + "_coating.up&!cooldown." + abilities.backstab + ".remains";
    action_list_str += "/use_relics"
                       + sl + abilities.hidden_strike;


    if ( talents.flanking -> rank() )
      action_list_str += sl + abilities.backstab + ",if=energy>=65";
    if ( talents.acid_blade -> rank() )
      action_list_str += ",if=buff." + abilities.acid_blade + "_coating.up";

    if ( talents.weakening_blast -> rank() )
      action_list_str += sl + abilities.weakening_blast;

    action_list_str += sl + abilities.shiv + ",if=energy>=75&buff." + abilities.tactical_advantage + ".stack<2"
                       + sl + abilities.corrosive_dart + ",if=!ticking&energy>=75";

    if ( talents.corrosive_grenade -> rank() )
    action_list_str += sl + abilities.corrosive_grenade + ",if=!ticking&energy>=80";

    if ( talents.laceration -> rank() )
      action_list_str += sl + abilities.laceration + ",if=energy>=75";

    if ( talents.cull -> rank() )
      action_list_str += sl + abilities.cull + ",if=energy>=50&buff." +abilities. tactical_advantage + ".stack>=2"
                         "&(dot." + abilities.corrosive_dart + ".ticking|dot." + abilities.corrosive_dart + "_weak.ticking)"
                         "&(dot." + abilities.corrosive_grenade + ".ticking|dot." + abilities.corrosive_grenade + "_weak.ticking)";

    if ( ! talents.flanking -> rank() )
      action_list_str += sl + abilities.backstab + ",if=energy>=70";

    action_list_str += sl + abilities.orbital_strike + ",if=energy>65";
    action_list_str += sl + abilities.overload_shot + ",if=energy>";
    if ( set_bonus.rakata_enforcers -> four_pc() )
      action_list_str += "100";
    else
      action_list_str += "95";
    action_list_str += sl + abilities.rifle_shot;

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
  register_tick_callback( RESULT_CRIT_MASK, new poison_tick_crit_callback_t ( this ) );

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

// class_t::create_talents ====================================

void class_t::create_talents()
{
  base_t::create_talents();

  // Medicine
  static const talentinfo_t medicine_tree[] = {
     { "Incisive Action"       , 2 }, { "Precision Instruments" , 2 }, { "Imperial Education"  , 3 },
     { "Endorphin Rush"        , 2 }, { "Medical Consult"       , 3 }, { "Surgical Steadiness" , 2 }, { "Chem-resistant Inlays" , 2 },
     { "Prognosis Critical"    , 2 }, { "Kolto Probe"           , 1 }, { "Sedatives"           , 2 },
     { "Patient Studies"       , 2 }, { "Medical Engineering"   , 3 }, { "Evasive Imperative"  , 2 },
     { "Tox Scan"              , 1 }, { "Medical Therapy"       , 2 }, { "Surgical Probe"      , 1 }, { "Surgical Precision"    , 1 },
     { "Med Shield"            , 2 }, { "Accomplished Doctor"   , 3 },
     { "Recuperative Nanotech" , 1 },
  };
  init_talent_tree( IA_OPERATIVE_MEDICINE, medicine_tree );

  // Concealment
  static const talentinfo_t concealment_tree[] = {
     { "Concealed Attacks"        , 2 },  { "Imperial Brew"        , 3 },  { "Survival Training"      , 3 },
     { "Infiltrator"              , 3 },  { "Surgical Strikes"     , 2 },  { "Inclement Conditioning" , 2 },  { "Scouting"     , 2 },
     { "Flanking"                 , 1 },  { "Laceration"           , 1 },  { "Collateral Strike"      , 2 },  { "Revitalizers" , 1 },
     { "Pin Down"                 , 2 },  { "Tactical Opportunity" , 2 },  { "Energy Screen"          , 1 },
     { "Waylay"                   , 1 },  { "Culling"              , 2 },  { "Advanced Cloaking"      , 2 },
     { "Meticulously Kept Blades" , 3 },  { "Jarring Strike"       , 2 },
     { "Acid Blade"               , 1 },
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
