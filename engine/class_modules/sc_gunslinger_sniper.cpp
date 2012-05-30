// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "agent_smug.hpp"

// ==========================================================================
// Gunslinger | Sniper
// ==========================================================================

// TODO: find out if we need to bother implementing Sector Ranger
// which reduces Orbital Strike cooldown when coverpulse activated

namespace { // ANONYMOUS ====================================================


namespace gunslinger_sniper { // ============================================

class class_t;

class targetdata_t : public agent_smug::targetdata_t
{
public:
  targetdata_t( class_t& source, player_t& target );

  buff_t* debuff_cluster_bombs;
  dot_t dot_interrogation_probe;
  dot_t dot_series_of_shots;
};

class class_t : public agent_smug::class_t
{
public:
  typedef agent_smug::class_t base_t;

  // Buffs
  struct buffs_t:base_t::buffs_t
  {
    buff_t* followthrough;
    buff_t* rapid_fire;
    buff_t* snap_shot;
    buff_t* sniper_volley;
  } buffs;

  // Gains
  struct gains_t:base_t::gains_t
  {
    gain_t* snipers_nest;
    gain_t* sniper_volley;
  } gains;

  // Procs
  struct procs_t:base_t::procs_t
  {
  } procs;

  // RNGs
  struct rngs_t:base_t::rngs_t
  {
    rng_t* reactive_shot;
  } rngs;

  // Benefits
  struct benefits_t:base_t::benefits_t
  {
  } benefits;

  // Cooldowns
  struct cooldowns_t
  {
    cooldown_t* ambush;
    cooldown_t* series_of_shots;
  } cooldowns;

  // Talents
  struct talents_t:base_t::talents_t
  {
    // Marksmanship|Sharpshooter
    // t1
    talent_t* cover_screen;
    talent_t* steady_shots;
    talent_t* marksmanship;
    // t2
    talent_t* heavy_shot;
    talent_t* ballistic_dampers;
    talent_t* precision_ambush;
    talent_t* imperial_demarcation;
    // t3
    talent_t* snap_shot;
    talent_t* diversion;
    talent_t* reactive_shot;
    // t4
    talent_t* between_the_eyes;
    talent_t* sector_ranger;
    talent_t* sniper_volley;
    talent_t* snipers_nest;
    // t5
    talent_t* recoil_control;
    talent_t* followthrough;
    talent_t* pillbox_sniper;
    // t6
    talent_t* imperial_assassin;
    talent_t* siege_bunker;
    // t7
    talent_t* rapid_fire;

    // Engineering|Saboteur
    // t1
    talent_t* engineers_tool_belt;
    talent_t* energy_tanks;
    talent_t* gearhead;
    // t2
    talent_t* explosive_engineering;
    talent_t* vital_regulators;
    talent_t* stroke_of_genius;
    talent_t* vitality_serum;
    // t3
    talent_t* cluster_bombs;
    talent_t* efficient_engineering;
    talent_t* interrogation_probe;
    talent_t* inventive_interrogation_techniques;
    // t4
    talent_t* imperial_methodology;
    talent_t* calculated_pursuit;
    talent_t* experimental_explosives;
    // t5
    talent_t* energy_overrides;
    talent_t* emp_discharge;
    talent_t* augmented_shields;
    // t6
    talent_t* deployed_shields;
    talent_t* electrified_railgun;
    // t7
    talent_t* plasma_probe;
  } talents;

  // Abilities
  struct abilities_t:base_t::abilities_t
  {
    std::string ambush;
    std::string followthrough;
    std::string interrogation_probe;
    std::string rapid_fire;
    std::string series_of_shots;
    std::string takedown;

    // buffs
    std::string snap_shot;
    std::string sniper_volley;
    std::string cluster_bombs;
    // rngs from talent
    std::string reactive_shot;
    // gains from talent
    std::string snipers_nest;
  } abilities;

  class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
    // TODO CHECK WTF IA_SNIPER ? IA_SNIPER : S_GUNSLINGER? IA_SNIPER is a constant it will always be true..?
    base_t( sim, pt == IA_SNIPER ? IA_SNIPER : S_GUNSLINGER, name, rt, buffs, gains, procs, rngs, benefits, talents, abilities ),
    buffs(), gains(), procs(), rngs(), benefits(), cooldowns(), talents()
  {
    tree_type[ IA_SNIPER_MARKSMANSHIP ] = TREE_MARKSMANSHIP;
    tree_type[ IA_SNIPER_ENGINEERING  ] = TREE_ENGINEERING;
    tree_type[ IA_LETHALITY           ] = TREE_LETHALITY;

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
  void      init_cooldowns();
  void      init_rng();
  void      init_actions();
  role_type primary_role() const;
  double    tech_accuracy_chance() const;
  double    range_accuracy_chance() const;
  double    tech_crit_chance() const;
  double    range_crit_chance() const;
  void      create_talents();

  double    energy_regen_per_second() const;
  void      regen( timespan_t periodicity );
  double    alacrity() const;

  void      _trigger_reactive_shot();
};

targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  agent_smug::targetdata_t( source, target )
{
// buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
   debuff_cluster_bombs   = new buff_t ( this, source.abilities.cluster_bombs, 2, from_seconds ( 0 ), from_seconds( 1.5 ), 100, false, true );

  dot_interrogation_probe = dot_t ( source.abilities.interrogation_probe, &source );
  dot_series_of_shots     = dot_t ( source.abilities.series_of_shots,     &source );

  add( dot_interrogation_probe );
  add( dot_series_of_shots     );

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
  range_attack_t( const std::string& n, class_t* p, school_type s=SCHOOL_KINETIC ) :
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
// Gunslinger / Sniper Abilities
// ==========================================================================

// Ambush | Aimed Shot ======================================================
struct ambush_t : public range_attack_t
{
  typedef range_attack_t base_t;
  ambush_t* offhand_attack;

  ambush_t( class_t* p, const std::string& n, const std::string& options_str,
     bool is_offhand=false ) :
    range_attack_t( n, p )
  {
    // todo: get this list right
    rank_level_list = { 8, 12, 16, 22, 31, 40, 50 };

    parse_options( options_str );

    base_cost                    = 15;
    base_execute_time            = from_seconds( 2.5 );
    cooldown -> duration         = from_seconds( 15 );
    range                        = 35.0;
    dd.standardhealthpercentmin  =
    dd.standardhealthpercentmax  = 0.329;
    dd.power_mod                 = 3.29;
    weapon                       = &( player -> main_hand_weapon );
    weapon_multiplier            = 1.2;

    if ( is_offhand )
    {
      base_cost                  = 0;
      background                 = true;
      dual                       = true;
      base_execute_time          = timespan_t::zero();
      trigger_gcd                = timespan_t::zero();
      weapon                     = &( player -> off_hand_weapon );
      rank_level_list            = { 0 };
      dd.power_mod               = 0;
    }

    else if ( player -> type == S_GUNSLINGER && &( player -> off_hand_weapon ) )
    {
      offhand_attack             = new ambush_t( p, n+"_offhand", options_str, true );
      add_child( offhand_attack );
    }
  }

  virtual void player_buff()
  {
    base_t::player_buff();
    if ( p() -> talents.precision_ambush -> rank() )
      player_armor_penetration -= 0.1;
  }

  virtual void execute()
  {
    base_t::execute();
    if ( offhand_attack )
      offhand_attack -> schedule_execute();
  }
};

// Explosive Probe | Sabotage Charge ========================================

struct explosive_probe_t : public agent_smug::explosive_probe_t
{
  typedef agent_smug::explosive_probe_t base_t;
  explosive_probe_t( class_t* p, const std::string& n, const std::string& options_str ) :
    agent_smug::explosive_probe_t( p, n, options_str )
  { }
  // TODO: explosive probe "attaches" to the target and detonates on damage
  // (tooltip says damage. game data has text that says blaster damage)
  // this is not implemented yet.

  virtual void execute()
  {
    base_t::execute();
    if ( dynamic_cast<class_t*>( player ) -> talents.cluster_bombs && result_is_hit() )
    {
      buff_t* cluster_bombs = dynamic_cast<gunslinger_sniper::targetdata_t*>( targetdata() ) -> debuff_cluster_bombs;
      if ( cluster_bombs -> up() )
        cluster_bombs -> refresh(2);
      else
        cluster_bombs -> start(2);
    }
  }
};

// Followthrough | Trickshot ================================================
struct followthrough_t : public agent_smug::range_attack_t
{
  typedef agent_smug::range_attack_t base_t;

  followthrough_t( class_t* p, const std::string& n, const std::string& options_str) :
    range_attack_t( n, p )
  {
    check_talent( p -> talents.followthrough -> rank() );

    // rank_level_list = { ... ,50 };

    parse_options( options_str );

    base_cost                    = 10 - 2 * p -> talents.recoil_control -> rank();
    cooldown -> duration         = from_seconds( 9 - 1.5 * p -> talents.recoil_control -> rank() );
    range                        = 35.0;
    dd.standardhealthpercentmin  =
    dd.standardhealthpercentmax  = 0.227;
    dd.power_mod                 = 2.27;
    weapon                       = &( player -> main_hand_weapon );
    weapon_multiplier            = 0.51;
    base_crit                   += 0.02 * p -> talents.between_the_eyes -> rank();
    crit_multiplier             += 0.1  * p -> talents.imperial_assassin -> rank();
  }

  virtual bool ready()
  {
    if ( static_cast<class_t*>(p())-> buffs.followthrough -> up() )
      return base_t::ready();
    else
      return false;
  }
};

// Interrogation Probe | Shock Charge =======================================
struct interrogation_probe_t : public agent_smug::tech_attack_t
{
  typedef agent_smug::tech_attack_t base_t;

  interrogation_probe_t( class_t* p, const std::string& n, const std::string& options_str) :
    tech_attack_t( n, p, SCHOOL_ENERGY )
  {
    check_talent( p -> talents.interrogation_probe -> rank() );

    // rank_level_list = { ... ,50 };

    parse_options( options_str );

    base_cost                    = 20;
    cooldown -> duration         = from_seconds( 18 );
    range                        = 35.0;
    td.standardhealthpercentmin  =
    td.standardhealthpercentmax  = 0.055;
    td.power_mod                 = 0.55;
    tick_zero                    = true;
    num_ticks                    = 6;
    base_tick_time  = from_seconds( 3.0 );
  }

  // not implemented: cooldown resets if ends prematurely (target dies, dispelled).
};

// Rapid Fire | Rapid Fire ==================================================

struct rapid_fire_t : public agent_smug::action_t
{
  typedef agent_smug::action_t base_t;

  rapid_fire_t( class_t* p, const std::string& n, const std::string& options_str ) :
    action_t( n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE )
  {
    check_talent( p -> talents.rapid_fire -> rank() );

    parse_options( options_str );

    cooldown -> duration = from_seconds( 90 );
    // TODO find out if this triggers GCD, and if it can be used off GCD
    //use_off_gcd = true;
    //trigger_gcd = timespan_t::zero();
  }

  void execute()
  {
    base_t::execute();
    class_t* p = dynamic_cast<class_t*>( player );

    p -> buffs.rapid_fire -> trigger();
    p -> cooldowns.series_of_shots -> reset();
  }
};


// Orbital Strike | XS Freighter Flyby ======================================

struct orbital_strike_t : public agent_smug::orbital_strike_t
{
  orbital_strike_t( class_t* p, const std::string& n, const std::string& options_str) :
      agent_smug::orbital_strike_t( p, n, options_str )
  {
    if ( unsigned rank = p -> talents.pillbox_sniper -> rank() )
    cooldown -> duration -= from_seconds( 7.5 * rank );
  }
};

// Series of Shots | Speed Shot =============================================
struct series_of_shots_t : public agent_smug::tech_attack_t
{
  typedef agent_smug::tech_attack_t base_t;

  series_of_shots_t( class_t* p, const std::string& n, const std::string& options_str) :
    tech_attack_t( n, p, SCHOOL_ENERGY )
  {
    // rank_level_list = { ... , 50 };

    parse_options( options_str );

    base_cost                    = 20;
    range                        = 35.0;
    cooldown -> duration         = from_seconds( 15.0 );
    channeled                    = true;
    tick_zero                    = true;
    num_ticks                    = 3;
    base_tick_time               = from_seconds( 1 );
    td.standardhealthpercentmin  =
    td.standardhealthpercentmax  = 0.098;
    td.weapon                    = &( player -> main_hand_weapon );
    td.power_mod                 = 0.98;
    td.weapon_multiplier         = -0.348;
    // tick crits. the invisible execute hit shouldn't
    may_crit                     = false;
    base_multiplier             += 0.03 * p -> talents.steady_shots      -> rank();
    base_crit                   += 0.02 * p -> talents.between_the_eyes  -> rank();
    crit_multiplier             += 0.1  * p -> talents.imperial_assassin -> rank();
  }

  virtual timespan_t execute_time() const
  {

    if ( dynamic_cast<class_t*>( player ) -> buffs.rapid_fire -> up() )
      return timespan_t::zero();

    return base_t::execute_time();
  }

  virtual void tick( dot_t* d )
  {
    base_t::tick( d );
    if ( result == RESULT_CRIT )
      dynamic_cast<class_t*>( player ) -> _trigger_reactive_shot();
  }

  virtual void execute()
  {
    base_t::execute();

    buff_t* rapid_fire = dynamic_cast<class_t*>( player ) -> buffs.rapid_fire;
    if ( rapid_fire -> up() )
      rapid_fire -> expire();
  }
};

// Snipe | Charged Burst ====================================================
struct snipe_t : public agent_smug::snipe_t
{
  typedef agent_smug::snipe_t base_t;
  snipe_t( class_t* p, const std::string& n, const std::string& options_str) :
    agent_smug::snipe_t( p, n, options_str )
  {
    base_multiplier             += 0.03 * p -> talents.steady_shots -> rank();
    base_crit                   += 0.02 * p -> talents.between_the_eyes -> rank();
  }

  virtual timespan_t execute_time() const
  {
    class_t* p = dynamic_cast<class_t*>( player );

    if ( p -> buffs.snap_shot -> up() )
      return timespan_t::zero();

    return base_t::execute_time();
  }

  virtual void execute()
  {
    base_t::execute();
    class_t* p = dynamic_cast<class_t*>( player );
    if ( p -> buffs.snap_shot -> up() )
      p -> buffs.snap_shot -> expire();

    if ( result == RESULT_CRIT )
      p -> _trigger_reactive_shot();
  }
};


// TODO steady_shots also affects Cull, but we still need to move that from
// scoundrel to agent

// Take Cover | Take Cover ==================================================
// REVIEW: could possibly do this as a trigger instead, but a trigger seems
// unwieldy since it would get checked on every action and need to test on name
struct take_cover_t : public agent_smug::take_cover_t
{
  typedef agent_smug::take_cover_t base_t;
  take_cover_t( class_t* p, const std::string& n, const std::string& options_str ) :
    agent_smug::take_cover_t( p, n, options_str )
  { }

  virtual void execute()
  {
    base_t::execute();
    class_t* p = dynamic_cast<class_t*>( player );
    if ( p -> talents.snap_shot -> rank() && p -> buffs.cover -> up() )
      p -> buffs.snap_shot -> trigger();
  }
};

// Takedown | Quickdraw =====================================================
struct takedown_t : public agent_smug::range_attack_t
{
  typedef agent_smug::range_attack_t base_t;

  takedown_t( class_t* p, const std::string& n, const std::string& options_str) :
    range_attack_t( n, p )
  {
    // rank_level_list = { ... ,50 };

    parse_options( options_str );

    base_cost                    = 15;
    cooldown -> duration         = from_seconds( 12 );
    range                        = 35.0;
    dd.standardhealthpercentmin  =
    dd.standardhealthpercentmax  = 0.27;
    dd.power_mod                 = 2.7;
    weapon                       = &( player -> main_hand_weapon );
    weapon_multiplier            = 0.8;
    crit_multiplier             += 0.1  * p -> talents.imperial_assassin -> rank();
  }

  virtual bool ready()
  {
    if ( target->health_percentage() >= 30 )
      return false;
    return base_t::ready();
  }
};

// ==========================================================================
// Gunslinger / Sniper Utility
// ==========================================================================
void class_t::_trigger_reactive_shot()
{
  unsigned rank = talents.reactive_shot -> rank();
    if ( rank && rngs.reactive_shot -> roll ( rank * 0.5 ))
      cooldowns.ambush -> reduce( from_seconds( 1 ) );
}

// ==========================================================================
// Gunslinger / Sniper Callbacks
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

// TODO trigger this manually in execute for each ability. too unwieldy for just 2 abilities
struct followthrough_trigger_callback_t : public action_callback_t
{
  followthrough_trigger_callback_t( class_t* p ) :
    action_callback_t( p )
  {}

  virtual void trigger (::action_t* a, void* /* call_data */)
  {
    if ( a -> name_str == p() -> abilities.snipe || a -> name_str == p() -> abilities.ambush )
    {
      p() -> buffs.followthrough -> trigger();
    }
  }
};

struct sniper_volley_callback_t : public action_callback_t
{
  sniper_volley_callback_t( class_t* p ) :
    action_callback_t( p )
  {}

  virtual void trigger (::action_t* a, void* /* call_data */)
  {
    // TODO test what constitutes "blaster fire" assuming any weapon attack
    if ( a -> weapon || a -> td.weapon )
      p() -> buffs.sniper_volley -> trigger();
  }
};

struct cluster_bombs_callback_t : public action_callback_t
{
  struct cluster_bombs_t : public agent_smug::tech_attack_t
  {
    typedef agent_smug::tech_attack_t base_t;

    cluster_bombs_t( class_t* p, const std::string& n) :
      agent_smug::tech_attack_t( n, p )
    {
      // rank_level_list = { ... ,50 };

      // unlimited? it will always hit
      range                        = 99.0;
      // always hit?
      base_accuracy                = 999;
      dd.standardhealthpercentmin  =
      dd.standardhealthpercentmax  = 0.047;
      dd.power_mod                 = 0.47;
      background                   = true;
      trigger_gcd                  = timespan_t::zero();
    }
  };

  cluster_bombs_t* cluster_bombs;

  cluster_bombs_callback_t( class_t* p ) :
    action_callback_t( p ),
    cluster_bombs( new cluster_bombs_t( p, p -> abilities.cluster_bombs ) )
  {}

  virtual void trigger (::action_t* a, void* /* call_data */)
  {
    // TODO test what constitutes "blaster fire" assuming any weapon attack
    if ( a -> weapon || a -> td.weapon )
    {
      buff_t* debuff_cluster_bombs = dynamic_cast<gunslinger_sniper::targetdata_t*>( a -> targetdata() )
                                     -> debuff_cluster_bombs;

      if ( debuff_cluster_bombs -> up() && debuff_cluster_bombs -> trigger() )
        cluster_bombs -> execute();
    }
  }
};


// ==========================================================================
// Gunslinger / Sniper Character Definition
// ==========================================================================

// class_t::create_action =======================================
::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  if ( name == abilities.ambush              ) return new ambush_t              ( this, name, options_str ) ;
  if ( name == abilities.followthrough       ) return new followthrough_t       ( this, name, options_str ) ;
  if ( name == abilities.interrogation_probe ) return new interrogation_probe_t ( this, name, options_str ) ;
  if ( name == abilities.rapid_fire          ) return new rapid_fire_t          ( this, name, options_str ) ;
  if ( name == abilities.series_of_shots     ) return new series_of_shots_t     ( this, name, options_str ) ;
  if ( name == abilities.takedown            ) return new takedown_t            ( this, name, options_str ) ;

  // overridden
  if ( name == abilities.explosive_probe     ) return new explosive_probe_t     ( this, name, options_str ) ;
  if ( name == abilities.orbital_strike      ) return new orbital_strike_t      ( this, name, options_str ) ;
  if ( name == abilities.snipe               ) return new snipe_t               ( this, name, options_str ) ;
  if ( name == abilities.take_cover          ) return new take_cover_t          ( this, name, options_str ) ;

  return base_t::create_action( name, options_str );
}

// class_t::init_abilities =======================================

void class_t::init_abilities()
{
  base_t::init_abilities();

  bool sn = type == IA_SNIPER;

  //                            =    ? SNIPER LABEL          : GUNSLINGER LABEL        ;
  abilities.ambush              = sn ? "ambush"              : "aimed_shot"            ;
  abilities.followthrough       = sn ? "followthrough"       : "trickshot"             ;
  abilities.interrogation_probe = sn ? "interrogation_probe" : "shock_charge"          ;
  abilities.rapid_fire          = sn ? "rapid_fire"          : "rapid_fire"            ;
  abilities.series_of_shots     = sn ? "series_of_shots"     : "speed_shot"            ;
  abilities.takedown            = sn ? "takedown"            : "quickdraw"             ;

  // buffs
  abilities.snap_shot           = sn ? "snap_shot"           : "snap_shot"             ;
  abilities.sniper_volley       = sn ? "sniper_volley"       : "burst_volley"          ;
  abilities.cluster_bombs       = sn ? "cluster_bombs"       : "contingency_charges"   ;
  // rngs
  abilities.reactive_shot       = sn ? "reactive_shot"       : "quick_aim"             ;
  // gains
  abilities.snipers_nest        = sn ? "snipers_nest"        : "fox_hole"              ;
}

// class_t::init_talents ========================================

void class_t::init_talents()
{
  base_t::init_talents();

  // Marksmanship|Sharpshooter
  // t1
  talents.cover_screen                        = find_talent( "Cover Screen"                       );
  talents.steady_shots                        = find_talent( "Steady Shots"                       );
  talents.marksmanship                        = find_talent( "Marksmanship"                       );
  // t2
  talents.heavy_shot                          = find_talent( "Heavy Shot"                         );
  talents.ballistic_dampers                   = find_talent( "Ballistic Dampers"                  );
  talents.precision_ambush                    = find_talent( "Precision Ambush"                   );
  talents.imperial_demarcation                = find_talent( "Imperial Demarcation"               );
  // t3
  talents.snap_shot                           = find_talent( "Snap Shot"                          );
  talents.diversion                           = find_talent( "Diversion"                          );
  talents.reactive_shot                       = find_talent( "Reactive Shot"                      );
  // t4
  talents.between_the_eyes                    = find_talent( "Between the Eyes"                   );
  talents.sector_ranger                       = find_talent( "Sector Ranger"                      );
  talents.sniper_volley                       = find_talent( "Sniper Volley"                      );
  talents.snipers_nest                        = find_talent( "Sniper's Nest"                      );
  // t5
  talents.recoil_control                      = find_talent( "Recoil Control"                     );
  talents.followthrough                       = find_talent( "Followthrough"                      );
  talents.pillbox_sniper                      = find_talent( "Pillbox Sniper"                     );
  // t6
  talents.imperial_assassin                   = find_talent( "Imperial Assassin"                  );
  talents.siege_bunker                        = find_talent( "Siege Bunker"                       );
  // t7
  talents.rapid_fire                          = find_talent( "Rapid Fire"                         );

  // Engineering|Saboteur
  // t1
  talents.engineers_tool_belt                 = find_talent( "Engineer's Tool Belt"               );
  talents.energy_tanks                        = find_talent( "Energy Tanks"                       );
  talents.gearhead                            = find_talent( "Gearhead"                           );
  // t2
  talents.explosive_engineering               = find_talent( "Explosive Engineering"              );
  talents.vital_regulators                    = find_talent( "Vital Regulators"                   );
  talents.stroke_of_genius                    = find_talent( "Stroke of Genius"                   );
  talents.vitality_serum                      = find_talent( "Vitality Serum"                     );
  // t3
  talents.cluster_bombs                       = find_talent( "Cluster Bombs"                      );
  talents.efficient_engineering               = find_talent( "Efficient Engineering"              );
  talents.interrogation_probe                 = find_talent( "Interrogation Probe"                );
  talents.inventive_interrogation_techniques  = find_talent( "Inventive Interrogation Techniques" );
  // t4
  talents.imperial_methodology                = find_talent( "Imperial Methodology"               );
  talents.calculated_pursuit                  = find_talent( "Calculated Pursuit"                 );
  talents.experimental_explosives             = find_talent( "Experimental Explosives"            );
  // t5
  talents.energy_overrides                    = find_talent( "Energy Overrides"                   );
  talents.emp_discharge                       = find_talent( "EMP Discharge"                      );
  talents.augmented_shields                   = find_talent( "Augmented Shields"                  );
  // t6
  talents.deployed_shields                    = find_talent( "Deployed Shields"                   );
  talents.electrified_railgun                 = find_talent( "Electrified Railgun"                );
  // t7
  talents.plasma_probe                        = find_talent( "Plasma Probe"                       );
}

// class_t::init_base ===========================================

void class_t::init_base()
{
  base_t::init_base();

  default_distance = 3;
  distance = default_distance;
  attribute_multiplier_initial[ ATTR_CUNNING ] += 0.03 * talents.gearhead -> rank();

  if ( unsigned rank = talents.energy_tanks -> rank() )
    resource_base[ RESOURCE_ENERGY ] += 5 * rank;
}

// class_t::init_benefits =======================================

void class_t::init_benefits()
{
  base_t::init_benefits();
}

// class_t::init_buffs ==========================================

void class_t::init_buffs()
{
  base_t::init_buffs();

// buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

  buffs.followthrough = new buff_t( this , abilities.followthrough , 1 ,  from_seconds( 4.5 ) );
  buffs.snap_shot     = new buff_t( this , abilities.snap_shot     , 1 ,  from_seconds( 10 ), from_seconds( 6 ), 0.5 * talents.snap_shot -> rank() );
  buffs.rapid_fire    = new buff_t( this , abilities.rapid_fire    , 1 ,  from_seconds( 10 ) );
  buffs.sniper_volley = new buff_t( this , abilities.sniper_volley , 1 ,  from_seconds( 10 ), from_seconds( 30 ), 0.05 * talents.sniper_volley -> rank() );
}

// class_t::init_gains ==========================================

void class_t::init_gains()
{
  base_t::init_gains();

  gains.snipers_nest  = get_gain( abilities.snipers_nest  );
  gains.sniper_volley = get_gain( abilities.sniper_volley );
}

// class_t::init_procs ==========================================

void class_t::init_procs()
{
  base_t::init_procs();

}

// class_t::init_cooldowns ======================================

void class_t::init_cooldowns()
{
  base_t::init_cooldowns();

  cooldowns.ambush          = get_cooldown( abilities.ambush );
  cooldowns.series_of_shots = get_cooldown( abilities.series_of_shots );
}

// class_t::init_rng ============================================

void class_t::init_rng()
{
  base_t::init_rng();
  rngs.reactive_shot = get_rng( abilities.reactive_shot );

}

// class_t::init_actions ========================================

void class_t::init_actions()
{
  const std::string sl = "/";

  if ( action_list_str.empty() )
  {
    action_list_default = true;
    action_list_str += "/stim,type=exotech_skill"
      + sl + abilities.coordination +
      "/snapshot_stats";

    action_list_str += sl + abilities.take_cover + ",if=buff." + abilities.cover + ".down";
    // guessing priority and optimal energy
    if ( talents.interrogation_probe -> rank() )
      action_list_str += sl + abilities.interrogation_probe + ",if=energy>65";
    action_list_str += sl + abilities.orbital_strike + ",if=energy>65";
    action_list_str += sl + abilities.takedown + ",if=energy>75";
    action_list_str += sl + abilities.series_of_shots + ",if=energy>80";
    if ( talents.rapid_fire -> rank() )
      action_list_str += sl + abilities.rapid_fire + ",if=cooldown." + abilities.series_of_shots + ".remains";
    if ( talents.followthrough -> rank() )
      action_list_str += sl + abilities.followthrough + ",if=buff." + abilities.followthrough + ".up";
    action_list_str += sl + abilities.explosive_probe + ",if=energy>80";
    // probably want to prioritise ambush/snipe if followthrough ICD is up
    action_list_str += sl + abilities.ambush + ",if=energy>75";
    action_list_str += sl + abilities.snipe + ",if=energy>95";
    action_list_str += sl + abilities.rifle_shot;
  }

  // TODO test "on hit"
  if ( talents.followthrough -> rank() )
    register_attack_callback( RESULT_HIT_MASK, new followthrough_trigger_callback_t( this ) );

  if ( talents.sniper_volley -> rank() )
  {
    register_attack_callback( RESULT_HIT_MASK, new sniper_volley_callback_t( this ) );
    register_tick_callback( RESULT_HIT_MASK, new sniper_volley_callback_t( this ) );
  }

  if ( talents.cluster_bombs -> rank() )
  {
    register_attack_callback( RESULT_HIT_MASK, new cluster_bombs_callback_t( this ) );
    register_tick_callback( RESULT_HIT_MASK, new cluster_bombs_callback_t( this ) );
  }

  base_t::init_actions();
}

// class_t::tech_accuracy_chance ================================
double class_t::tech_accuracy_chance() const
{
  return base_t::tech_accuracy_chance() + 0.01 * talents.marksmanship -> rank();
}

// class_t::range_accuracy_chance ===============================
double class_t::range_accuracy_chance() const
{
  return base_t::range_accuracy_chance() + 0.01 * talents.marksmanship -> rank();
}

// class_t::tech_crit_chance ====================================
double class_t::tech_crit_chance() const
{
  return base_t::tech_crit_chance() + 0.01 * talents.lethality -> rank();
}

// class_t::range_crit_chance ===================================
double class_t::range_crit_chance() const
{
  return base_t::range_crit_chance() + 0.01 * talents.lethality -> rank();
}

// class_t::energy_regen_per_second =============================
// MIRROR CHANGES WITH regen BELOW
double class_t::energy_regen_per_second() const
{
  double eps = base_t::energy_regen_per_second();
  if ( talents.snipers_nest -> rank() && buffs.cover -> up() )
    eps += 1;
  return eps;
}

// class_t::regen ===============================================
// MIRROR CHANGES WITH energy_regen_per_second ABOVE
void class_t::regen( timespan_t periodicity )
{
  base_t::regen( periodicity );
  if ( talents.snipers_nest -> rank() && buffs.cover -> up() )
    resource_gain( RESOURCE_ENERGY, to_seconds( periodicity ) * 1, gains.snipers_nest );

  if ( buffs.sniper_volley -> up() )
    // using ERPS instead of directly checking gains as a kind of double check that ERPS is maintained
    resource_gain( RESOURCE_ENERGY, to_seconds( periodicity ) * energy_regen_per_second(), gains.sniper_volley );
}

// class_t::alacrity ============================================
double class_t::alacrity() const
{
  return base_t::alacrity() - ( buffs.sniper_volley -> up() ? 0.1 : 0 );
}


// class_t::primary_role ========================================

role_type class_t::primary_role() const
{
  return ROLE_DPS;
}

// class_t::create_talents ======================================

void class_t::create_talents()
{
  base_t::create_talents();

  // Marksmanship|Sharpshooter
  static const talentinfo_t marksmanship_tree[] = {
     { "Cover Screen"      , 2 }, { "Steady Shots"      , 2 }, { "Marksmanship"     , 3 },
     { "Heavy Shot"        , 1 }, { "Ballistic Dampers" , 2 }, { "Precision Ambush" , 2 }, { "Imperial Demarcation" , 2 },
     { "Snap Shot"         , 2 }, { "Diversion"         , 1 }, { "Reactive Shot"    , 2 },
     { "Between the Eyes"  , 2 }, { "Sector Ranger"     , 1 }, { "Sniper Volley"    , 3 }, { "Sniper's Nest"        , 1 },
     { "Recoil Control"    , 2 }, { "Followthrough"     , 1 }, { "Pillbox Sniper"   , 2 },
     { "Imperial Assassin" , 3 }, { "Siege Bunker"      , 2 },
     { "Rapid Fire"        , 1 }
  };
  init_talent_tree( IA_SNIPER_MARKSMANSHIP, marksmanship_tree );

  // Engineering|Saboteur
  static const talentinfo_t engineering_tree[] = {
     { "Engineer's Tool Belt"  , 2 }, { "Energy Tanks"          , 2 }, { "Gearhead"                , 3 },
     { "Explosive Engineering" , 3 }, { "Vital Regulators"      , 2 }, { "Stroke of Genius"        , 2 }, { "Vitality Serum"                     , 2 },
     { "Cluster Bombs"         , 2 }, { "Efficient Engineering" , 2 }, { "Interrogation Probe"     , 1 }, { "Inventive Interrogation Techniques" , 2 },
     { "Imperial Methodology"  , 1 }, { "Calculated Pursuit"    , 2 }, { "Experimental Explosives" , 2 },
     { "Energy Overrides"      , 2 }, { "EMP Discharge"         , 1 }, { "Augmented Shields"       , 2 },
     { "Deployed Shields"      , 2 }, { "Electrified Railgun"   , 3 },
     { "Plasma Probe"          , 1 }
  };
  init_talent_tree( IA_SNIPER_ENGINEERING, engineering_tree );
}

} // namespace gunslinger_sniper ============================================

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_gunslinger  =============================================

player_t* player_t::create_gunslinger( sim_t* sim, const std::string& name, race_type r )
{
  return new gunslinger_sniper::class_t( sim, S_GUNSLINGER, name, r );
}

// player_t::create_sniper  =================================================

player_t* player_t::create_sniper( sim_t* sim, const std::string& name, race_type r )
{
  return new gunslinger_sniper::class_t( sim, IA_SNIPER, name, r );
}

// player_t::gunslinger_sniper_init =========================================

void player_t::gunslinger_sniper_init( sim_t* /* sim */ )
{
}

// player_t::gunslinger_sniper_combat_begin =================================

void player_t::gunslinger_sniper_combat_begin( sim_t* /* sim */ )
{
}
