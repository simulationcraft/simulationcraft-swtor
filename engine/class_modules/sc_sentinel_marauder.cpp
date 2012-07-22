// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "../simulationcraft.hpp"
#include "warr_knight.hpp"

// ==========================================================================
// Jedi Sentinel | Sith Marauder
// ==========================================================================

namespace { // ANONYMOUS ====================================================

namespace sentinel_marauder {

class class_t;

enum form_type
{
  FORM_NONE = 0, JUYO_FORM, SHII_CHO_FORM, ATARU_FORM
};

struct sentinel_marauder_targetdata_t : public targetdata_t
{
  dot_t rupture;
  dot_t deadly_saber;

  sentinel_marauder_targetdata_t( player_t& source, player_t& target )
    : targetdata_t( source, target ) {}
};


struct class_t : public warr_knight::class_t
{
    typedef warr_knight::class_t base_t;
    // Buffs
    struct buffs_t
    {
      buff_t* juyo_form;
      buff_t* annihilator;
      buff_t* fury;
      buff_t* deadly_saber;
      buff_t* berserk;
      buff_t* bloodthirst;
    } buffs;

    // Gains
    struct gains_t
    {
      gain_t* assault;
      gain_t* battering_assault;
      gain_t* force_charge;
    } gains;

    // Procs
    struct procs_t
    {

    } procs;

    // RNGs
    struct rngs_t
    {
      rng_t* enraged_charge;
      rng_t* pulverize;
      rng_t* empowerment;
    } rngs;

    // Benefits
    struct benefits_t
    {

    } benefits;

    // Cooldowns
    struct cooldowns_t
    {
      cooldown_t* deadly_saber;
      cooldown_t* force_charge;
      cooldown_t* rupture;
      cooldown_t* annihilate;
      cooldown_t* ravage;
      cooldown_t* battering_assault;
      cooldown_t* bloodthirst;
      cooldown_t* vicious_throw;
      cooldown_t* frenzy;
      cooldown_t* force_choke;
      cooldown_t* smash;
    } cooldowns;

    // Talents
    struct talents_t
    {
        // Annihilation|Watchman
        // t1
        talent_t* cloak_of_annihilation;
        talent_t* short_fuse;
        talent_t* enraged_slash;
        //t2
        talent_t* juyo_mastery;
        talent_t* seeping_wound;
        talent_t* hungering;
        //t3
        talent_t* bleedout;
        talent_t* deadly_saber;
        talent_t* blurred_speed;
        //t4
        talent_t* enraged_charge;
        talent_t* subjugation;
        talent_t* deep_wound;
        talent_t* close_quarters;
        //t5
        talent_t* phantom;
        talent_t* pulverize;
        //t6
        talent_t* empowerment;
        talent_t* hemorrhage;
        //t7
        talent_t* annihilate;

        // Carnage|Combat
        //t1
        talent_t* cloak_of_carnage;
        talent_t* dual_wield_mastery;
        talent_t* defensive_forms;
        //t2
        talent_t* narrowed_hatred;
        talent_t* defensive_roll;
        talent_t* stagger;
        //t3
        talent_t* execute;
        talent_t* ataru_form;
        talent_t* ataru_mastery;
        //t4
        talent_t* blood_frenzy;
        talent_t* towering_rage;
        talent_t* enraged_assault;
        talent_t* displacement;
        //t5
        talent_t* unbound;
        talent_t* gore;
        talent_t* rattling_voice;
        //t6
        talent_t* overwhelm;
        talent_t* sever;
        //t7
        talent_t* massacre;

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

    } talents;

    struct actives_t
    {
      form_type form;
    } actives;

    class_t( sim_t* sim, player_type pt, const std::string& name, race_type r = RACE_NONE ) :
      base_t( sim, pt == SITH_MARAUDER ? SITH_MARAUDER : JEDI_SENTINEL, name, ( r == RACE_NONE ) ? RACE_HUMAN : r ),
      buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents(), actives()
    {

      tree_type[ SITH_MARAUDER_ANNIHILATION ] = TREE_ANNIHILATION;
      tree_type[ SITH_MARAUDER_CARNAGE ] = TREE_CARNAGE;
      tree_type[ SITH_MARAUDER_RAGE ] = TREE_RAGE;

      create_talents();
      create_options();
    }

    // Character Definition
    virtual targetdata_t* new_targetdata( player_t& target ) // override
    { return new sentinel_marauder_targetdata_t( *this, target ); }

    virtual action_t* create_action( const std::string& name, const std::string& options );
    virtual void      init_talents();
    virtual void      init_base();
    virtual void      init_resources( bool force );
    virtual void      init_benefits();
    virtual void      init_buffs();
    virtual void      init_gains();
    virtual void      init_procs();
    virtual void      init_rng();
    virtual void      init_actions();
    virtual resource_type primary_resource() const;
    virtual int       fury_generated() const;
    virtual role_type primary_role() const;
            void      create_talents();

    virtual void init_scaling()
    {
      player_t::init_scaling();
      scales_with[ STAT_FORCE_POWER ] = true;
    }
};


class action_t : public ::action_t
{
public:
  typedef ::action_t base_t;
  action_t( const std::string& n, class_t* player,
                          attack_policy_t policy, resource_type r, school_type s ) :
    base_t( ACTION_ATTACK, n, player, policy, r, s )
  {}

  sentinel_marauder_targetdata_t* targetdata() const
  { return static_cast<sentinel_marauder_targetdata_t*>( action_t::targetdata() ); }

  class_t* p() const
  { return static_cast<class_t*>( player ); }

  class_t* cast() const { return p(); }
};

// ==========================================================================
// Sentinel / Marauder Abilities
// ==========================================================================

struct force_attack_t : public action_t
{
    force_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_ENERGY ) :
      action_t( n, p, force_policy, p -> primary_resource(), s )
    {
        may_crit   = true;

        base_multiplier += (0.02 * p -> buffs.juyo_form -> current_stack );

        if ( p -> buffs.bloodthirst -> up() )
        {
          player_multiplier += 0.15;
        }
    }

};

struct melee_attack_t : public action_t
{
  melee_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_ENERGY) :
    action_t(n, p, melee_policy, p -> primary_resource(), s)
  {
    may_crit = true;

    base_multiplier += (0.02 * p -> buffs.juyo_form -> current_stack );

    if ( p -> buffs.bloodthirst -> up() )
    {
      player_multiplier += 0.15;
    }
  }
  virtual void impact( player_t* t, result_type impact_result, double travel_dmg)
  {
    action_t::impact( t, impact_result, travel_dmg);

    if (result_is_hit( impact_result ) )
    {
      class_t* p = cast();

      if ( p -> actives.form == JUYO_FORM )
      {
        p->buffs.juyo_form-> increment();
      }

      if ( p -> buffs.deadly_saber -> up() )
      {
        p -> buffs.deadly_saber -> decrement();
      }
    }
  }
};

struct bleed_attack_t : public force_attack_t
{
  bleed_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_INTERNAL) :
    force_attack_t(n, p, s)
  {
    tick_may_crit = true;
    may_crit = false;

    if ( p -> actives.form == JUYO_FORM && p -> buffs.berserk -> up() )
    {
      base_crit += 1;
    }
  }

  virtual void execute()
  {
    class_t* p = cast();
    if ( p -> actives.form == JUYO_FORM && p -> buffs.berserk -> up() )
    {
      p -> buffs.berserk -> decrement();
    }
  }
};
// Berserk | Combat Focus =====================================================
struct berserk_t : public action_t
{
  typedef action_t basae_t;

  //TODO: Ataru Form Berserk

  berserk_t( class_t* p, const std::string& n, const std::string& options_str) :
    action_t( n, p, force_policy, p -> primary_resource(), SCHOOL_NONE )
  {
    parse_options( options_str );
    harmful = false;

    base_cost = 0;
    use_off_gcd = true;
    trigger_gcd = timespan_t::zero();
  }

  virtual bool ready()
  {
    class_t* p = cast();
    return p -> buffs.fury -> current_stack >= 30;
  }

  virtual void execute()
  {
    base_t::execute();

    class_t* p = cast();

    p -> buffs.berserk -> trigger( 6 );
    // TODO REVIEW: resets or just loses 30 stacks? ie is 35 possible, and would it go to 5 or 0?
    p -> buffs.fury -> reset();
  }
};

// Vicious Throw | Merciless Throw ============================================
struct vicious_throw_t : public melee_attack_t
{
  typedef melee_attack_t base_t;

  static int energy_cost( class_t* p ) { return 3 - ( p -> set_bonus.rakata_weaponmasters -> two_pc() ? 1 : 0 ); }

  vicious_throw_t( class_t* p, const std::string& n, const std::string& options_str) :
    base_t( n, p )
  {
    parse_options( options_str );

    base_cost = energy_cost( p );
    cooldown -> duration = from_seconds( 6 );
    range = 10.0;
    dd.standardhealthpercentmin = 0.265;
    dd.standardhealthpercentmax = 0.305;
    dd.power_mod = 2.85;
    weapon = &(player -> main_hand_weapon);
    weapon_multiplier = 0.9;
  }

  virtual bool ready()
  {
    return target -> health_percentage() >= 30 ? false : base_t::ready();
  }

  virtual void execute()
  {
    base_t::execute();

    class_t* p = cast();
    p -> buffs.fury -> increment( p -> fury_generated() );
  }
};

// Rupture | Cauterize ========================================================
struct rupture_t : public melee_attack_t
{
  typedef melee_attack_t base_t;

  struct rupture_dot_t : public bleed_attack_t
  {
    rupture_dot_t( class_t* p, const std::string& n) :
      bleed_attack_t( n, p )
    {
      td.standardhealthpercentmin =
      td.standardhealthpercentmax = 0.02;
      td.power_mod = 0.2;

      base_cost = 0;
      base_tick_time = from_seconds( 1.0 );
      num_ticks = 6;
      tick_zero = false;
      range = 4.0;
      background = true;
    }
  };

  rupture_dot_t* rupture_dot;
  rupture_t* offhand_attack;

  rupture_t( class_t* p, const std::string& n, const std::string& options_str,
             bool is_offhand = false ) :
    melee_attack_t( n, p ), rupture_dot( new rupture_dot_t( p, n + "_dot" ) )
  {
    parse_options( options_str );

    range = 4.0;
    base_cost = 2;
    cooldown -> duration = from_seconds( 15 );

    dd.standardhealthpercentmin =
    dd.standardhealthpercentmax = 0.06;
    dd.power_mod = 0.6;
    weapon = &(player -> main_hand_weapon);
    weapon_multiplier = -0.6;

    add_child( rupture_dot );

    if ( is_offhand )
    {
      base_cost = 0;
      background = true;
      dual = true;
      base_execute_time = timespan_t::zero();
      trigger_gcd = timespan_t::zero();
      weapon = &( player -> off_hand_weapon );
      rank_level_list = { 0 };
      dd.power_mod = 0;
    }
    else
    {
      offhand_attack = new rupture_t( p, n + "_offhand", options_str, true );
      add_child( offhand_attack );
    }
  }

  virtual void execute()
  {
    base_t::execute();
    if ( offhand_attack )
    {
      offhand_attack -> schedule_execute();
      class_t* p = cast();
      p -> buffs.fury -> increment( p -> fury_generated() );
    }
  }
};

// Annihilate | Merciless Slash ===============================================
struct annihilate_t : public melee_attack_t
{
  typedef melee_attack_t base_t;

  annihilate_t* offhand_attack;

  annihilate_t( class_t* p, const std::string& n, const std::string& options_str,
                bool is_offhand=false ) :
    base_t(n, p)
  {
    check_talent( p -> talents.annihilate -> rank());

    parse_options( options_str );

    base_cost = 5;
    cooldown -> duration = from_seconds( 12 - (1.5 * p -> buffs.annihilator -> current_stack));
    range = 4.0;

    dd.standardhealthpercentmin =
    dd.standardhealthpercentmax = 0.285;
    dd.power_mod = 2.85;
    weapon = &(player -> main_hand_weapon);
    weapon_multiplier = 0.9;

    if (is_offhand)
    {
      base_cost = 0;
      background = true;
      dual = true;
      base_execute_time = timespan_t::zero();
      trigger_gcd = timespan_t::zero();
      weapon = &( player -> off_hand_weapon );
      rank_level_list = { 0 };
      dd.power_mod = 0;
    }
    else
    {
      offhand_attack = new annihilate_t( p, n+"_offhand", options_str, true);
      add_child( offhand_attack );
    }
  }

  virtual void execute()
  {
    base_t::execute();

    class_t* p = cast();
    p -> buffs.annihilator -> increment();
    p -> buffs.fury -> increment( p -> fury_generated() );
  }
};

// Deadly Saber | Overload Saber ==============================================
struct deadly_saber_t : public action_t
{
  typedef action_t base_t;

  deadly_saber_t( class_t* p, const std::string& n, const std::string& options_str ) :
    base_t( n, p, force_policy, p -> primary_resource(), SCHOOL_NONE )
  {
    parse_options( options_str );
    harmful = false;
    cooldown -> duration = from_seconds( 12.0 );
    base_cost = 3;

    use_off_gcd = true;
    trigger_gcd = timespan_t::zero();
  }
  virtual void execute()
  {
    base_t::execute();

    class_t* p = cast();
    p -> buffs.fury -> increment( p -> fury_generated() );
    p -> buffs.deadly_saber -> trigger( 3 );
  }

};

struct deadly_saber_dot_t : public bleed_attack_t
{
  typedef bleed_attack_t base_t;

  deadly_saber_dot_t( class_t* p, const std::string& n, const std::string& options_str) :
    base_t( n, p )
  {
    parse_options( options_str );

    td.standardhealthpercentmin =
    td.standardhealthpercentmax = 0.02;
    td.power_mod = 0.2;
    base_cost = 0;

    base_tick_time = from_seconds( 3.0 );
    num_ticks = 3;
    tick_zero = true;
    background = true;
  }
};

// Force Charge | Force Leap ==================================================
struct force_charge_t : public melee_attack_t
{
  typedef melee_attack_t base_t;

  force_charge_t( class_t* p, const std::string& n, const std::string& options_str) :
    base_t( n, p )
  {
    parse_options( options_str );

    base_cost = 0;
    cooldown -> duration = from_seconds( 15 );
    range = 30.0;
    weapon_multiplier = -0.39;
    weapon = &(player -> main_hand_weapon);

    dd.standardhealthpercentmin =
    dd.standardhealthpercentmax = 0.091;
    dd.power_mod = 0.91;


    //TODO: minimum range?
    //TODO: travel time?
  }

  virtual void execute()
  {
    base_t::execute();

    class_t* p = cast();
    p -> resource_gain( RESOURCE_RAGE, 3, p -> gains.force_charge );
  }
};

// Ravage | Master Strike =====================================================
struct ravage_t : public melee_attack_t
{
  typedef melee_attack_t base_t;

  ravage_t* second_strike;
  ravage_t* third_strike;
  ravage_t* offhand_attack;

  ravage_t( class_t* p, const std::string& n, const std::string& options_str,
           int strike_number = 1, bool is_offhand = false ) :
    base_t( n, p )
  {
    parse_options( options_str );
    base_cost = 0;

    cooldown -> duration = from_seconds( 30 );

  }
};

// Battering Assault | Zealous Strike =========================================
struct battering_assault_t : public melee_attack_t
{
  typedef melee_attack_t base_t;

  battering_assault_t* second_strike;
  battering_assault_t* offhand_attack;
  bool rage_gain;

  battering_assault_t( class_t* p, const std::string& n, const std::string& options_str,
                       bool is_second_strike = false, bool is_offhand = false ) :
    base_t( n, p )
  {
    parse_options( options_str );

    base_cost = 0;
    range = 4.0;
    cooldown -> duration = from_seconds( 15 );
    weapon = &( player -> main_hand_weapon );
    weapon_multiplier = -0.835;
    dd.standardhealthpercentmin =
    dd.standardhealthpercentmax = 0.05;
    dd.power_mod = 0.5;
    rage_gain = true;

    if ( is_second_strike )
    {
      background = dual = true;
      trigger_gcd = timespan_t::zero();
      base_execute_time = from_seconds( 0.4 );
      rage_gain = false;
    }
    else if ( ! is_offhand )
    {
      second_strike = new battering_assault_t( p, n, options_str, true, is_offhand );
    }

    if ( is_offhand )
    {
      background = dual = true;
      trigger_gcd = timespan_t::zero();
      weapon = &( player -> off_hand_weapon );
      rank_level_list = { 0 };
      dd.power_mod = 0;
      rage_gain = false;
    }
    else
    {
      offhand_attack = new battering_assault_t( p, n + "_offhand", options_str, is_second_strike, true );
      add_child( offhand_attack );
    }

  }

  virtual void execute()
  {
    base_t::execute();
    if ( second_strike )
    {
      second_strike -> schedule_execute();
    }
    if ( offhand_attack )
    {
      offhand_attack -> schedule_execute();
    }
    if ( rage_gain )
    {
      class_t* p = cast();
      p -> resource_gain( RESOURCE_RAGE, 6, p -> gains.battering_assault );
    }
  }
};

// Vicious Slash | Slash ======================================================
struct vicious_slash_t : public melee_attack_t
{
  typedef melee_attack_t base_t;

  vicious_slash_t* offhand_attack;

  vicious_slash_t( class_t* p, const std::string& n, const std::string& options_str,
                  bool is_offhand = false ) :
    base_t( n, p )
  {
    parse_options( options_str );

    base_cost = 3;
    range = 4.0;

    dd.standardhealthpercentmin =
    dd.standardhealthpercentmax = 0.153;
    dd.power_mod = 1.54;
    weapon = &(player -> main_hand_weapon);
    weapon_multiplier = 0.02;

    if ( is_offhand )
    {
      base_cost = 0;
      background = true;
      dual = true;
      base_execute_time = timespan_t::zero();
      trigger_gcd = timespan_t::zero();
      weapon = &( player -> off_hand_weapon );
      rank_level_list = { 0 };
      dd.power_mod = 0;
    }
    else
    {
      offhand_attack = new vicious_slash_t( p, n+"_offhand", options_str, true);
      add_child( offhand_attack );
    }
  }

  virtual double cost() const
  {
    class_t* p = cast();
    //TODO: Hits one addition target too
    return ( p -> actives.form == SHII_CHO_FORM && p -> buffs.berserk -> check() )
      ? 0 : base_t::cost();
  }

  virtual void execute()
  {
    base_t::execute();

    if ( offhand_attack ) {
      offhand_attack -> schedule_execute();

      class_t* p = cast();
      if ( p -> actives.form == SHII_CHO_FORM && p -> buffs.berserk -> up() )
        p -> buffs.berserk -> decrement();

      p -> buffs.fury -> increment( p -> fury_generated() );
    }
  }
};

// Assault | Strike ===========================================================
struct assault_t : public melee_attack_t
{
  typedef melee_attack_t base_t;

  assault_t* second_strike;
  assault_t* third_strike;
  assault_t* offhand_strike;

  assault_t( class_t* p, const std::string& n, const std::string& options_str) :
    melee_attack_t( n, p )
    {
      parse_options( options_str );
      base_cost = 0;

      range = 4.0;
      base_accuracy -= 0.10;
    }

  virtual void execute()
  {
    base_t::execute();
    p() -> resource_gain( RESOURCE_RAGE, 2, p() -> gains.assault );
  }
};

// Bloodthirst | Inspiration ==================================================
struct bloodthirst_t : public action_t
{
  typedef action_t base_t;

  bloodthirst_t( class_t* p, const std::string& n, const std::string options_str ) :
    base_t( n, p, force_policy, RESOURCE_NONE, SCHOOL_NONE )
  {
    parse_options( options_str );
    base_cost = 0;
    harmful = false;

    use_off_gcd = true;
    trigger_gcd = timespan_t::zero();

    cooldown -> duration = from_seconds( 300 );
  }

  virtual void execute()
  {
    base_t::execute();

    class_t* p = cast();
    p -> buffs.bloodthirst -> trigger();
  }

  virtual bool ready()
  {
    class_t* p = cast();
    return p -> buffs.fury -> current_stack >= 30;
  }
};



// Force Choke | Force Stasis =================================================
struct force_choke_t : force_attack_t
{
  typedef force_attack_t base_t;

};

// Force Sream | Blade Storm ==================================================
struct force_scream_t : force_attack_t
{
  typedef force_attack_t base_t;

};

// Frenzy | Valorous Call =====================================================
struct frenzy_t : action_t
{
  typedef action_t base_t;

  frenzy_t( class_t* p, const std::string& n, const std::string options_str ) :
    base_t( n, p, force_policy, RESOURCE_NONE, SCHOOL_NONE )
    {
      parse_options( options_str );
      base_cost = 0;

      harmful = false;
      trigger_gcd = timespan_t::zero();
      use_off_gcd = true;

      cooldown -> duration = from_seconds( 180 );
    }

    virtual void execute()
    {
      base_t::execute();
      class_t* p = cast();
      p -> buffs.fury -> trigger( 30 );
    }

};

// Juyo Form ==================================================================
struct juyo_form_t : action_t
{
  typedef action_t base_t;

  juyo_form_t( class_t* p, const std::string& n, const std::string options_str ) :
    base_t( n, p, force_policy, RESOURCE_NONE, SCHOOL_NONE )
  {
    parse_options( options_str );

    base_cost = 0;

    harmful = false;
  }

  virtual void execute()
  {
    base_t::execute();
    class_t* p = cast();

    p -> actives.form = JUYO_FORM;
  }

};

// Shii-Cho Form ==============================================================
struct shii_cho_form_t : action_t
{
  typedef action_t base_t;
  // TODO: Rage
};

// Ataru Form =================================================================
struct ataru_form_t : action_t
{
  typedef action_t base_t;
  // TODO: Carnage
};

// Retaliate | Riposte ========================================================
struct retaliate_t : melee_attack_t
{
  typedef melee_attack_t base_t;
  //TODO: Not used by dps
};

// Smash | Sweep ==============================================================
struct smash_t : force_attack_t
{
  typedef force_attack_t base_t;
  //TODO: AOE?
};

// Sweeping Slash | Cyclone Slash =============================================
struct sweeping_slash_t : melee_attack_t
{
  typedef melee_attack_t base_t;
  //TODO: AOE?
};

// Unnatural Might | Force_Might ==============================================
struct unnatural_might_t : action_t
{
  typedef action_t base_t;

  unnatural_might_t( class_t* p, const std::string& n, const std::string options_str ) :
    base_t( n, p, force_policy, RESOURCE_NONE, SCHOOL_NONE )
  {
    parse_options( options_str );
    base_cost = 0;
  }

  virtual void execute()
  {
    base_t::execute();

    for ( player_t* p : list_range( sim -> player_list ) )
    {
      if ( p -> ooc_buffs() )
      {
        p -> buffs.unnatural_might -> trigger();
      }
    }
  }

  virtual bool ready()
  {
    if ( player -> buffs.unnatural_might -> check() )
    {
      return false;
    }

    return base_t::ready();
  }

};

// ============================================================================
// Carnage Talents ============================================================
// Gore | Precision Slash =====================================================
struct gore_t : public melee_attack_t
{
  typedef melee_attack_t base_t;

};
// Massacre | Blade Rush ======================================================
struct massacre_t : public melee_attack_t
{
  typedef melee_attack_t base_t;

};

// ============================================================================
// Rage Talents
// Obliterate | Zealous Leap ==================================================
struct obliterate_t : melee_attack_t
{
  typedef melee_attack_t base_t;

};

// Force Crush | Force Exhaustion =============================================
struct force_crush_t : force_attack_t
{
  typedef force_attack_t base_t;

};


// ==========================================================================
// sentinel_marauder Character Definition
// ==========================================================================

// class_t::create_action ====================================================

::action_t* class_t::create_action( const std::string& name,
                                            const std::string& options_str )
{
    if ( type == SITH_MARAUDER )
    {
      if ( name == "vicious_throw"    ) return new vicious_throw_t    ( this, name, options_str );
      if ( name == "annihilate"       ) return new annihilate_t       ( this, name, options_str );
      if ( name == "rupture"          ) return new rupture_t          ( this, name, options_str );
      if ( name == "deadly_saber"     ) return new deadly_saber_t     ( this, name, options_str );
      if ( name == "force_charge"     ) return new force_charge_t     ( this, name, options_str );
      if ( name == "ravage"           ) return new ravage_t           ( this, name, options_str );
      if ( name == "battering_assault") return new battering_assault_t( this, name, options_str );
      if ( name == "vicious_slash"    ) return new vicious_slash_t    ( this, name, options_str );
      if ( name == "assault"          ) return new assault_t          ( this, name, options_str );
      if ( name == "bloodthirst"      ) return new bloodthirst_t      ( this, name, options_str );
      if ( name == "berserk"          ) return new berserk_t          ( this, name, options_str );
      if ( name == "juyo_form"        ) return new juyo_form_t        ( this, name, options_str );
      if ( name == "frenzy"           ) return new frenzy_t           ( this, name, options_str );
      if ( name == "unnatural_might"  ) return new unnatural_might_t ( this, name, options_str );
    }
    else if ( type == JEDI_SENTINEL )
    {

    }

    //if ( name == "apply_charge"           ) return new        apply_charge_t( this, options_str );

    return base_t::create_action( name, options_str );
}

// class_t::init_talents =====================================================

void class_t::init_talents()
{
    base_t::init_talents();

     // Annihilation|Watchman
        // t1
        talents.cloak_of_annihilation       = find_talent( "Cloak of Annihilation" );
        talents.short_fuse                  = find_talent( "Short Fuse" );
        talents.enraged_slash               = find_talent( "Enraged Slash" );
        //t2
        talents.juyo_mastery                = find_talent( "Juyo Mastery" );
        talents.seeping_wound               = find_talent( "Seeping Wound" );
        talents.hungering                   = find_talent( "Hungering" );
        //t3
        talents.bleedout                    = find_talent( "Bleedout" );
        talents.deadly_saber                = find_talent( "Deadly Saber" );
        talents.blurred_speed               = find_talent( "Blurred Speed" );
        //t4
        talents.enraged_charge              = find_talent( "Enraged Charge" );
        talents.subjugation                 = find_talent( "Subjugation" );
        talents.deep_wound                  = find_talent( "Deep Wound" );
        talents.close_quarters              = find_talent( "Close Quarters" );
        //t5
        talents.phantom                     = find_talent( "Phantom" );
        talents.pulverize                   = find_talent( "Pulverize" );
        //t6
        talents.empowerment                 = find_talent( "Empowerment" );
        talents.hemorrhage                  = find_talent( "Hemorrhage" );
        //t7
        talents.annihilate                  = find_talent( "Annihilate" );

        // Carnage|Combat
        //t1
        talents.cloak_of_carnage            = find_talent( "Cloak of Carnage" );
        talents.dual_wield_mastery          = find_talent( "Dual Wield Mastery" );
        talents.defensive_forms             = find_talent( "Defensive Forms" );
        //t2
        talents.narrowed_hatred             = find_talent( "Narrowed Hatred" );
        talents.defensive_roll              = find_talent( "Defensive Roll" );
        talents.stagger                     = find_talent( "Stagger" );
        //t3
        talents.execute                     = find_talent( "Execute" );
        talents.ataru_form                  = find_talent( "Ataru Form" );
        talents.ataru_mastery               = find_talent( "Ataru Mastery" );
        //t4
        talents.blood_frenzy                = find_talent( "Blood Frenzy" );
        talents.towering_rage               = find_talent( "Towering Rage" );
        talents.enraged_assault             = find_talent( "Enraged Assault" );
        talents.displacement                = find_talent( "Displacement" );
        //t5
        talents.unbound                     = find_talent( "Unbound" );
        talents.gore                        = find_talent( "Gore" );
        talents.rattling_voice              = find_talent( "Rattling Voice");
        //t6
        talents.overwhelm                   = find_talent( "Overwhelm" );
        talents.sever                       = find_talent( "Sever" );
        //t7
        talents.massacre                    = find_talent( "Massacre" );

        // Rage|Focus
        //t1
        talents.ravager                     = find_talent( "Ravager" );
        talents.malice                      = find_talent( "Malice");
        talents.decimate                    = find_talent( "Decimate" );
        //t2
        talents.payback                     = find_talent( "Payback" );
        talents.overpower                   = find_talent( "Overpower" );
        talents.enraged_scream              = find_talent( "Enraged Scream" );
        talents.brutality                   = find_talent( "Brutality" );
        //t3
        talents.saber_strength              = find_talent( "Saber Strength" );
        talents.obliterate                  = find_talent( "Obliterate" );
        talents.strangulate                 = find_talent( "Strangulate" );
        //t4
        talents.dominate                    = find_talent( "Dominate" );
        talents.shockwave                   = find_talent( "Shockwave" );
        talents.berserker                   = find_talent( "Berserker" );
        //t5
        talents.gravity                     = find_talent( "Gravity" );
        talents.interceptor                 = find_talent( "Interceptor" );
        talents.shii_cho_mastery            = find_talent( "Shii Cho Mastery" );
        //t6
        talents.dark_resonance              = find_talent( "Dark Resonance" );
        talents.undying                     = find_talent( "Undying" );
        //t7
        talents.force_crush                 = find_talent( "Force Crush" );
}

// class_t::init_base ========================================================

void class_t::init_base()
{
    player_t::init_base();

    default_distance = 3;
    distance = default_distance;
    resource_base[ RESOURCE_RAGE ] = 12;
}
// class_t::init_resources ======================================================

void class_t::init_resources( bool force )
{
  player_t::init_resources( force );
  resource_current[ RESOURCE_RAGE ] = 0;
}

// class_t::init_benefits =======================================================

void class_t::init_benefits()
{
    player_t::init_benefits();

}

// class_t::init_buffs =======================================================

void class_t::init_buffs()
{
    player_t::init_buffs();

    // buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
    // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
    // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
    buffs.deadly_saber = new buff_t( this, "deadly_saber", 3, from_seconds( 15 ), from_seconds( 12 ) );
    buffs.juyo_form = new buff_t( this, "juyo_form", 5, from_seconds( 15 ), from_seconds( 1.5 ) );
    buffs.bloodthirst = new buff_t( this, "bloodthirst", 1, from_seconds( 15 ));


    buffs.juyo_form = new buff_t( this, "Juyo Form", 5, from_seconds( 15.0 ), from_seconds( 1.5 ) );
    buffs.annihilator = new buff_t( this, "Annihilator", 3, from_seconds( 15.0 ) );
    buffs.fury = new buff_t( this, "Fury", 30, from_seconds( 60 ) );
    buffs.deadly_saber = new buff_t( this, "Deadly Saber", 3, from_seconds( 15.0 ), from_seconds( 1.5 ) );
    buffs.berserk = new buff_t( this, "Berserk", 6, from_seconds( 20 ) );
    buffs.bloodthirst = new buff_t( this, "Bloodthirst", 1, from_seconds( 15 ) );
}

// class_t::init_gains =======================================================

void class_t::init_gains()
{
    player_t::init_gains();
    bool is_marauder = ( type == SITH_MARAUDER );
    const char* assault = is_marauder ? "assault" : "strike";
    const char* battering_assault = is_marauder ? "battering_assault" : "zealous_strike";
    const char* force_charge = is_marauder ? "force_charge" : "force_leap";

    gains.assault = get_gain( assault );
    gains.battering_assault = get_gain( battering_assault );
    gains.force_charge = get_gain( force_charge );

}

// class_t::init_procs =======================================================

void class_t::init_procs()
{
    player_t::init_procs();

}

// class_t::init_rng =========================================================

void class_t::init_rng()
{
    player_t::init_rng();



}

// class_t::init_actions =====================================================

void class_t::init_actions()
{
    //======================================================================================
    //
    //   Please Mirror all changes between Jedi Shadow and Sith Assassin!!!
    //
    //======================================================================================

    if ( action_list_str.empty() )
    {
        if ( type == JEDI_SENTINEL )
        {
            action_list_str += "stim,type=exotech_might";
            action_list_str += "/snapshot_stats";

            switch ( primary_tree() )
            {


            default: break;
            }

            action_list_default = 1;
        }

        // Sith MARAUDER
        else
        {
            action_list_str += "stim,type=exotech_might";
            action_list_str += "/snapshot_stats";
            // ANNIHILATION
            action_list_str += "/deadly_saber,if=!buff.deadly_saber.up";
            action_list_str += "/battering_assault,if=rage<7";
            action_list_str += "/berserk";
            action_list_str += "/annihilate";
            action_list_str += "/rupture";
            action_list_str += "/vicious_throw";
            action_list_str += "/ravage";
            action_list_str += "/vicious_slash";

            switch ( primary_tree() )
            {

            default: break;
            }

            action_list_default = 1;
        }
    }

    player_t::init_actions();
}

// class_t::primary_resource ==================================================

resource_type class_t::primary_resource() const
{ return RESOURCE_RAGE; }

// class_t::fury_generated ==================================================
int class_t::fury_generated() const
{
  return 2;
}

// class_t::primary_role ==================================================

role_type class_t::primary_role() const
{
    return ROLE_DPS;
}

// class_t::create_talents ==================================================

void class_t::create_talents()
{
  static const talentinfo_t annihilation_tree[] = {
    // t1
    { "Cloak of Annihilation"   , 2 }, { "Short Fuse"         , 2 }, { "Enraged Slash"    , 3 },
    // t2
    { "Juyo Mastery"            , 3 }, { "Seeping Wound"      , 2 }, { "Hungering"        , 2 },
    // t3
    { "Bleedout"                , 2 }, { "Deadly Saber"       , 1 }, { "Blurred Speed"    , 2 },
    // t4
    { "Enraged Charge"          , 2 }, { "Subjugation"        , 2 }, { "Deep Wound"       , 1 }, { "Close Quarters"   , 2 },
    // t5
    { "Phantom"                 , 2 }, { "Pulverize"          , 3 },
    //t6
    { "Empowerment"             , 2 }, { "Hemorrhage"         , 3 },
    // t7
    { "Annihilate"              , 1 }
  };
  init_talent_tree( SITH_MARAUDER_ANNIHILATION, annihilation_tree );

  static const talentinfo_t carnage_tree[] = {
    // t1
    { "Cloak of Carnage"        , 2 }, { "Dual Wield Mastery" , 3 }, { "Defensive Forms"  , 2 },
    // t2
    { "Narrowed Hatred"         , 3 }, { "Defensive Roll"     , 2 }, { "Stagger"          , 2 },
    // t3
    { "Execute"                 , 2 }, { "Ataru Form"         , 1 }, { "Ataru Mastery"    , 2 },
    // t4
    { "Blood Frenzy"            , 1 }, { "Towering Rage"      , 2 }, { "Enraged Assault"  , 2 }, { "Displacement"     , 2 },
    // t5
    { "Unbound"                 , 2 }, { "Gore"               , 1 }, { "Rattling Voice"   , 2 },
    //t6
    { "Overwhelm"               , 2 }, { "Sever"              , 3 },
    //t7
    { "Massacre"                , 1 }
  };
  init_talent_tree( SITH_MARAUDER_CARNAGE, carnage_tree );

  static const talentinfo_t rage_tree[] = {
    // t1
    { "Ravager"                 , 3 }, { "Malice"             , 3 }, { "Decimate"         , 2 },
    // t2
    { "Payback"                 , 2 }, { "Overpower"          , 2 }, { "Enraged Scream"   , 1 }, { "Brutality"        , 2 },
    // t3
    { "Saber Strength"          , 2 }, { "Obliterate"         , 1 }, { "Strangulate"      , 2 },
    // t4
    { "Dominate"                , 3 }, { "Shockwave"          , 2 }, { "Berserker"        , 2 },
    // t5
    { "Gravity"                 , 1 }, { "Interceptor"        , 2 }, { "Shii Cho Mastery" , 2 },
    // t6
    { "Dark Resonance"          , 3 }, { "Undying"            , 2 },
    // t7
    { "Force Crush"             , 1 }
  };
  init_talent_tree( SITH_MARAUDER_RAGE, rage_tree );
}

} // namespace sentinel_marauder ============================================

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_jedi_sentinel  ============================================

player_t* player_t::create_jedi_sentinel( sim_t* sim, const std::string& name, race_type r )
{
    return new sentinel_marauder::class_t( sim, JEDI_SENTINEL, name, r );
}

// player_t::create_SITH_MARAUDER  ==========================================

player_t* player_t::create_sith_marauder( sim_t* sim, const std::string& name, race_type r )
{
    return new sentinel_marauder::class_t( sim, SITH_MARAUDER, name, r );
}

// player_t::sentinel_marauder_init ===========================================

void player_t::sentinel_marauder_init( sim_t* /* sim */ )
{

}

// player_t::sentinel_marauder_combat_begin ===================================

void player_t::sentinel_marauder_combat_begin( sim_t* /* sim */ )
{

}
