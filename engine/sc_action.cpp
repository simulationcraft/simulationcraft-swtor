// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// ==========================================================================
// Action
// ==========================================================================

// Attack policies ==========================================================

namespace { // ANONYMOUS ====================================================

class default_policy_t : public attack_policy_interface_t
{
public:
  default_policy_t() { name_ = "none"; }

  double accuracy_chance( const player_t& ) const
  { return -1; }
  double avoidance( const player_t& ) const
  { return 0; }
  double crit_chance( const player_t&) const
  { return -1; }
  double damage_bonus( const player_t& ) const
  { return 0; }
  double shield_chance( const player_t& ) const
  { return 0; }
  double shield_absorb( const player_t& ) const
  { return 0; }
};

class physical_policy_t : public attack_policy_interface_t
{
public:
  double shield_chance( const player_t& t ) const
  { return t.shield_chance(); }
  double shield_absorb( const player_t& t ) const
  { return t.shield_absorb(); }
};

class melee_policy_t : public physical_policy_t
{
public:
  melee_policy_t() { name_ = "melee"; }

  double accuracy_chance( const player_t& player ) const
  { return player.melee_accuracy_chance(); }
  double avoidance( const player_t& player ) const
  { return player.melee_avoidance(); }
  double crit_chance( const player_t& player ) const
  { return player.melee_crit_chance(); }
  double damage_bonus( const player_t& player ) const
  { return player.melee_damage_bonus(); }
};

class range_policy_t : public physical_policy_t
{
public:
  range_policy_t() { name_ = "range"; }

  double accuracy_chance( const player_t& player ) const
  { return player.range_accuracy_chance(); }
  double avoidance( const player_t& player ) const
  { return player.range_avoidance(); }
  double crit_chance( const player_t& player ) const
  { return player.range_crit_chance(); }
  double damage_bonus( const player_t& player ) const
  { return player.range_damage_bonus(); }
};

class spell_policy_t : public attack_policy_interface_t
{
public:
  double shield_chance( const player_t& ) const
  { return 0; }
  double shield_absorb( const player_t& ) const
  { return 0; }
};

class force_policy_t : public spell_policy_t
{
public:
  force_policy_t() { name_ = "force"; }

  double accuracy_chance( const player_t& player ) const
  { return player.force_accuracy_chance(); }
  double avoidance( const player_t& player ) const
  { return player.force_avoidance(); }
  double crit_chance( const player_t& player ) const
  { return player.force_crit_chance(); }
  double damage_bonus( const player_t& player ) const
  { return player.force_damage_bonus(); }
};

class tech_policy_t : public spell_policy_t
{
public:
  tech_policy_t() { name_ = "tech"; }

  double accuracy_chance( const player_t& player ) const
  { return player.tech_accuracy_chance(); }
  double avoidance( const player_t& player ) const
  { return player.tech_avoidance(); }
  double crit_chance( const player_t& player ) const
  { return player.tech_crit_chance(); }
  double damage_bonus( const player_t& player ) const
  { return player.tech_damage_bonus(); }
};

class heal_policy_t : public spell_policy_t
{
public:
  double avoidance( const player_t& ) const { return 0; }
};

class force_heal_policy_t : public heal_policy_t
{
public:
  force_heal_policy_t() { name_ = "force_heal"; }

  double accuracy_chance( const player_t& ) const
  { return 1.0; }
  double crit_chance( const player_t& player ) const
  { return player.force_healing_crit_chance(); }
  double damage_bonus( const player_t& player ) const
  { return player.force_healing_bonus(); }
};

class tech_heal_policy_t : public heal_policy_t
{
public:
  tech_heal_policy_t() { name_ = "tech_heal"; }

  double accuracy_chance( const player_t& ) const
  { return 1.0; }
  double crit_chance( const player_t& player ) const
  { return player.tech_healing_crit_chance(); }
  double damage_bonus( const player_t& player ) const
  { return player.tech_healing_bonus(); }
};

const default_policy_t the_default_policy;
const melee_policy_t the_melee_policy;
const range_policy_t the_range_policy;
const force_policy_t the_force_policy;
const tech_policy_t the_tech_policy;
const force_heal_policy_t the_force_heal_policy;
const tech_heal_policy_t the_tech_heal_policy;
}

const action_t::policy_t action_t::default_policy = &the_default_policy;
const action_t::policy_t action_t::melee_policy = &the_melee_policy;
const action_t::policy_t action_t::range_policy = &the_range_policy;
const action_t::policy_t action_t::force_policy = &the_force_policy;
const action_t::policy_t action_t::tech_policy = &the_tech_policy;
const action_t::policy_t action_t::force_heal_policy = &the_force_heal_policy;
const action_t::policy_t action_t::tech_heal_policy = &the_tech_heal_policy;

// action_t::action_t =======================================================

void action_t::init_action_t_()
{
  id                             = 0;
  result                         = RESULT_NONE;
  aoe                            = 0;
  dual                           = false;
  callbacks                      = true;
  channeled                      = false;
  background                     = false;
  sequence                       = false;
  use_off_gcd                    = false;
  direct_tick                    = false;
  repeating                      = false;
  harmful                        = true;
  proc                           = false;
  item_proc                      = false;
  proc_ignores_slot              = false;
  discharge_proc                 = false;
  auto_cast                      = false;
  initialized                    = false;
  may_crit                       = false;
  tick_may_crit                  = false;
  tick_zero                      = false;
  hasted_ticks                   = false;
  no_buffs                       = false;
  no_debuffs                     = false;
  dot_behavior                   = DOT_CLIP;
  ability_lag                    = timespan_t::zero();
  ability_lag_stddev             = timespan_t::zero();
  min_gcd                        = from_seconds( 1.0 );
  trigger_gcd                    = player -> base_gcd;

  range                          = -1.0;
  // Prevent melee from being scheduled when player is moving
  // FIXME: What is this doing?
  if ( range < 0 ) range = 5;

  base_execute_time              = timespan_t::zero();
  base_tick_time                 = timespan_t::zero();
  base_cost                      = 0.0;
  base_multiplier                = 1.0;
  base_accuracy                  = 0.0;
  base_crit                      = 0.0;
  base_armor_penetration         = 1.0;
  player_multiplier              = 1.0;
  player_accuracy                     = 0.0;
  player_crit                    = 0.0;
  player_armor_penetration       = 1.0;
  target_multiplier              = 1.0;
  target_accuracy                     = 0.0;
  target_crit                    = 0.0;
  target_armor_penetration       = 1.0;
  crit_multiplier                = 1.0;
  crit_bonus                     = 0.5;
  crit_bonus_multiplier          = 1.0;
  base_dd_adder                  = 0.0;
  player_dd_adder                = 0.0;
  target_dd_adder                = 0.0;
  player_alacrity                = 1.0;
  resource_consumed              = 0.0;
  direct_dmg                     = 0.0;
  tick_dmg                       = 0.0;
  num_ticks                      = 0;
  weapon                         = nullptr;
  td.weapon                      = nullptr;
  weapon_multiplier              = std::numeric_limits<decltype(weapon_multiplier)>::lowest();
  td.weapon_multiplier           = std::numeric_limits<decltype(weapon_multiplier)>::lowest();
  normalize_weapon_speed         = false;
  base_add_multiplier            = 1.0;
  base_aoe_multiplier            = 1.0;
  rng_travel                     = NULL;
  stats                          = NULL;
  execute_event                  = NULL;
  travel_event                   = NULL;
  time_to_execute                = timespan_t::zero();
  time_to_travel                 = timespan_t::zero();
  travel_speed                   = 0.0;
  moving                         = -1;
  vulnerable                     = 0;
  invulnerable                   = 0;
  not_flying                     = 0;
  flying                         = 0;
  wait_on_ready                  = -1;
  interrupt                      = 0;
  round_base_dmg                 = true;
  if_expr_str.clear();
  if_expr.release();
  interrupt_if_expr_str.clear();
  interrupt_if_expr.release();
  sync_str.clear();
  sync_action                    = NULL;
  next                           = NULL;
  last_reaction_time             = timespan_t::zero();
  cached_targetdata = NULL;
  cached_targetdata_target = NULL;

  cached_dot = nullptr;
  cached_dot_target = nullptr;

  if ( sim -> debug ) log_t::output( sim, "Player %s creates action %s", player -> name(), name() );

  if ( unlikely( ! player -> initialized ) )
  {
    sim -> errorf( "Actions must not be created before player_t::init().  Culprit: %s %s\n", player -> name(), name() );
    sim -> cancel();
  }

  action_t** last = &( player -> action_list );
  while ( *last ) last = &( ( *last ) -> next );
  *last = this;

  boost::fill( rng, nullptr );

  cooldown = player -> get_cooldown( name_str );

  stats = player -> get_stats( name_str , this );

  rank_level = 0;
}

action_t::action_t( action_type        ty,
                    const std::string& n,
                    player_t*          p,
                    const policy_t     policy,
                    resource_type      r,
                    school_type        s ) :
  sim( p -> sim ), type( ty ), name_str( n ),
  player( p ), target( p -> target ), attack_policy( policy ),
  school( s ), resource( r )
{
  assert( policy );
  init_action_t_();
}

action_t::~action_t()
{}

// action_t::parse_options ==================================================

void action_t::parse_options( option_t*          options,
                              const std::string& options_str )
{
  option_t base_options[] =
  {
    { "if",                     OPT_STRING, &if_expr_str           },
    { "interrupt_if",           OPT_STRING, &interrupt_if_expr_str },
    { "interrupt",              OPT_BOOL,   &interrupt             },
    { "invulnerable",           OPT_BOOL,   &invulnerable          },
    { "not_flying",             OPT_BOOL,   &not_flying            },
    { "flying",                 OPT_BOOL,   &flying                },
    { "moving",                 OPT_BOOL,   &moving                },
    { "sync",                   OPT_STRING, &sync_str              },
    { "travel_speed",           OPT_FLT,    &travel_speed          },
    { "vulnerable",             OPT_BOOL,   &vulnerable            },
    { "wait_on_ready",          OPT_BOOL,   &wait_on_ready         },
    { "target",                 OPT_STRING, &target_str            },
    { "label",                  OPT_STRING, &label_str             },
    { "use_off_gcd",            OPT_BOOL,   &use_off_gcd           },
    { NULL,                     OPT_NONE,   NULL                   }
  };

  std::vector<option_t> merged_options;
  option_t::merge( merged_options, options, base_options );

  std::string::size_type cut_pt = options_str.find( ':' );

  std::string options_buffer;
  if ( cut_pt != options_str.npos )
  {
    options_buffer = options_str.substr( cut_pt + 1 );
  }
  else options_buffer = options_str;

  if ( options_buffer.empty()     ) return;

  if ( ! option_t::parse( sim, name(), merged_options, options_buffer ) )
  {
    sim -> errorf( "%s %s: Unable to parse options str '%s'.\n", player -> name(), name(), options_str.c_str() );
    sim -> cancel();
  }

  // FIXME: Move into constructor when parse_action is called from there.
  if ( ! target_str.empty() )
  {
    player_t* p = sim -> find_player( target_str );

    if ( p )
      target = p;
    else
    {
      sim -> errorf( "%s %s: Unable to locate target '%s'.\n", player -> name(), name(), options_str.c_str() );
    }
  }
}

// action_t::cost ===========================================================

double action_t::cost() const
{
  if ( ! harmful && ! player -> in_combat )
    return 0;

  double c = base_cost;

  c -= player -> resource_reduction[ school ];
  if ( c < 0 ) c = 0;

  if ( sim -> debug ) log_t::output( sim, "action_t::cost: %s %.2f %.2f %s", name(), base_cost, c, util_t::resource_type_string( resource ) );

  return floor( c );
}

// action_t::gcd ============================================================

timespan_t action_t::gcd() const
{
  if ( ! harmful && ! player -> in_combat )
    return timespan_t::zero();

  timespan_t t = trigger_gcd;

  if ( t != timespan_t::zero() )
  {
    // According to http://sithwarrior.com/forums/Thread-SWTOR-formula-list alacrity doesn't reduce the gcd
    // cast time abilities get a reduced gcd, but instant cast abilities do not
    // http://sithwarrior.com/forums/Thread-Alacrity-and-the-GCD?pid=9152#pid9152
    // abilities with base_execute_time > 0 but with time_to_execute=0 ( e.g., because of procs ) don't get
    // a reduced gcd. Tested visually by Kor, 15/2/2012
    // Above is irrelevant with 2.0 changes. Alacrity affects all abilities now and redices the GCD
    t /= alacrity();

    if ( t < min_gcd ) t = min_gcd;  // TODO: Check if there is actually a minimum GCD. Although I doubt a 1 sec GCD could ever be reached...
  }

  return t;
}

// action_t::travel_time ====================================================

timespan_t action_t::travel_time()
{
  if ( travel_speed == 0 ) return timespan_t::zero();

  if ( player -> distance == 0 ) return timespan_t::zero();

  double t = player -> distance / travel_speed;

  if ( double v = sim -> travel_variance )
    t = rng_travel -> gauss( t, v );

  return from_seconds( t );
}

// action_t::player_buff ====================================================

void action_t::player_buff()
{
  player_multiplier              = 1.0;
  dd.player_multiplier           = 1.0;
  td.player_multiplier           = 1.0;
  player_accuracy                = attack_policy -> accuracy_chance( *player );
  player_crit                    = attack_policy -> crit_chance( *player );
  player_armor_penetration       = player -> armor_penetration();
  player_dd_adder                = 0;

  if ( ! no_buffs )
  {
    player_t* p = player;

    player_multiplier    = p -> composite_player_multiplier   ( school, this );
    dd.player_multiplier = p -> composite_player_dd_multiplier( school, this );
    td.player_multiplier = p -> composite_player_td_multiplier( school, this );
  }

  player_alacrity = alacrity();

  if ( sim -> debug )
    log_t::output( sim, "action_t::player_buff: %s accuracy=%.2f crit=%.2f",
                   name(), player_accuracy, player_crit );
}

// action_t::target_debuff ==================================================

void action_t::target_debuff( player_t* t, dmg_type )
{
  target_avoidance     = attack_policy -> avoidance( *t );
  target_shield        = attack_policy -> shield_chance( *t );
  target_absorb        = attack_policy -> shield_absorb( *t );
  target_multiplier    = 1.0;
  target_dd_adder      = 0;
  dd.target_multiplier = 1.0;
  td.target_multiplier = 1.0;
  target_crit          = 0;
  target_armor_penetration = t -> armor_penetration_debuff();

  if ( ! no_debuffs )
  {
    if ( t -> debuffs.vulnerable -> up() && type == ACTION_ATTACK )
      target_multiplier *= t -> debuffs.vulnerable -> value();
  }

  if ( sim -> debug )
    log_t::output( sim, "action_t::target_debuff: %s (target=%s) multiplier=%.2f avoid=%.2f shield=%.2f absorb=%.2f",
                   name(), t -> name(), target_multiplier, target_avoidance, target_shield, target_absorb );
}

// action_t::result_is_hit ==================================================

bool action_t::result_is_hit( result_type r )
{
  return( r == RESULT_HIT        ||
          r == RESULT_CRIT       ||
          r == RESULT_BLOCK      ||
          r == RESULT_NONE       );
}

// action_t::result_is_miss =================================================

bool action_t::result_is_miss( result_type r )
{ return ( r == RESULT_MISS || r == RESULT_AVOID ); }

// action_t::armor ==========================================================

double action_t::armor() const
{
  double a = target -> composite_armor();
  a *= total_armor_penetration();
  return a;
}

// action_t::total_crit_bonus ===============================================

double action_t::total_crit_bonus() const
{
  double bonus = ( ( 1.0 + crit_bonus + player -> surge_bonus ) * crit_multiplier - 1.0 ) * crit_bonus_multiplier;

  if ( sim -> debug )
  {
    log_t::output( sim, "%s crit_bonus for %s: cb=%.3f b_cb=%.2f b_cm=%.2f b_cbm=%.2f",
                   player -> name(), name(), bonus, crit_bonus, crit_multiplier, crit_bonus_multiplier );
  }

  return bonus;
}

// action_t::total_power ====================================================

double action_t::total_power() const
{
  double power = attack_policy -> damage_bonus( *player );

  if ( sim -> debug )
  {
    log_t::output( sim, "%s total power for %s: power=%.2f",
                   player -> name(), name(), power );
  }

  return power;
}

// action_t::calculate_weapon_damage ========================================

double action_t::calculate_weapon_damage( weapon_t* weapon )
{
  if ( ! weapon ) return 0;

  double dmg = sim -> range( weapon -> min_dmg, weapon -> max_dmg ) + weapon -> bonus_dmg;

  // OH penalty
  if ( weapon -> slot == SLOT_OFF_HAND )
    //dmg *= 0.3;
    dmg *= player -> offhand_multiplier();

  if ( sim -> debug )
  {
    log_t::output( sim, "%s weapon damage for %s: dmg=%.3f bd=%.3f",
                   player -> name(), name(), dmg, weapon -> bonus_dmg );
  }

  return dmg;
}

// action_t::calculate_tick_damage ==========================================

double action_t::calculate_tick_damage()
{
  if ( td.base_max == 0 && td.power_mod == 0 && td.weapon == 0 ) return 0;

  double dmg = sim -> range( td.base_min, td.base_max );

  if ( round_base_dmg ) dmg = floor( dmg + 0.5 );

  double base_tick_dmg = dmg;

  dmg += total_power() * td.power_mod;

  double weapon_dmg = 0;
  if ( td.weapon )
  {
    // x% weapon damage + Y
    // e.g. Obliterate, Shred, Backstab
    weapon_dmg = calculate_weapon_damage( td.weapon ) * ( 1 + td.weapon_multiplier );
    dmg += weapon_dmg;
  }

  dmg *= total_td_multiplier();

  double init_tick_dmg = dmg;

  if ( result == RESULT_CRIT )
  {
    dmg *= 1.0 + total_crit_bonus();
  }

  if ( ! sim -> average_range )
    dmg = floor( dmg + sim -> real() );

  if ( sim -> debug )
  {
    log_t::output( sim, "%s dmg for %s: td=%.2f i_td=%.2f w_td=%2f b_td=%.2f mod=%.2f power=%.2f b_mult=%.2f p_mult=%.2f t_mult=%.2f w_mult=%2f",
                   player -> name(), name(), dmg, init_tick_dmg, weapon_dmg, base_tick_dmg, td.power_mod,
                   total_power(), base_multiplier * td.base_multiplier, player_multiplier, target_multiplier, td.weapon ? td.weapon_multiplier : 0 );
  }

  return dmg;
}

// action_t::calculate_direct_damage ========================================

double action_t::calculate_direct_damage( int chain_target )
{
  if ( dd.base_max == 0 && weapon == 0 && dd.power_mod == 0 ) return 0;

  double dmg = sim -> range( dd.base_min, dd.base_max );

  if ( round_base_dmg ) dmg = floor( dmg + 0.5 );

  double base_direct_dmg = dmg;

  dmg += base_dd_adder + player_dd_adder + target_dd_adder;
  dmg += dd.power_mod * total_power();

  assert( ( weapon == nullptr ) == ( weapon_multiplier == std::numeric_limits<decltype( weapon_multiplier )>::lowest() ) );
  double weapon_dmg = 0;
  if ( weapon )
  {
    // x% weapon damage + Y
    // e.g. Obliterate, Shred, Backstab
    weapon_dmg = calculate_weapon_damage( weapon ) * ( 1 + weapon_multiplier );
    dmg += weapon_dmg;
  }

  dmg *= total_dd_multiplier();

  double init_direct_dmg = dmg;

  if ( result == RESULT_CRIT )
  {
    dmg *= 1.0 + total_crit_bonus();
  }

  // AoE with decay per target
  if ( chain_target > 0 && base_add_multiplier != 1.0 )
    dmg *= pow( base_add_multiplier, chain_target );

  // AoE with static reduced damage per target
  if ( chain_target > 1 && base_aoe_multiplier != 1.0 )
    dmg *= base_aoe_multiplier;

  if ( ! sim -> average_range ) dmg = floor( dmg + sim -> real() );

  if ( sim -> debug )
  {
    log_t::output( sim, "%s dmg for %s: dd=%.0f i_dd=%.0f w_dd=%.0f b_dd=%.0f mod=%.2f power=%.0f b_mult=%.2f p_mult=%.2f t_mult=%.2f w_mult=%.2f",
                   player -> name(), name(), dmg, init_direct_dmg, weapon_dmg, base_direct_dmg, dd.power_mod,
                   total_power(), base_multiplier * dd.base_multiplier, player_multiplier, target_multiplier, weapon ? weapon_multiplier : 0 );
  }

  return dmg;
}

// action_t::consume_resource ===============================================

void action_t::consume_resource()
{
  if ( resource == RESOURCE_NONE ) return;

  resource_consumed = cost();

  if ( player -> resource_reverse[ resource ] )
    player -> resource_gain( resource, resource_consumed, 0, this );
  else
    player -> resource_loss( resource, resource_consumed, 0, this );

  if ( sim -> log )
    log_t::output( sim, "%s consumes %.1f %s for %s (%.0f)", player -> name(),
                   resource_consumed, util_t::resource_type_string( resource ),
                   name(), player -> resource_current[ resource] );

  stats -> consume_resource( resource_consumed );
}

// action_t::available_targets ==============================================

std::vector<player_t*> action_t::available_targets() const
{
  // TODO: This does not work for heals at all, as it presumes enemies in the
  // actor list.

  std::vector<player_t*> tl;

  if ( target )
    tl.push_back( target );

  bool harmful = ( type != ACTION_HEAL ) && ( type != ACTION_ABSORB );

  for ( size_t i = 0; i < sim -> actor_list.size(); i++ )
  {
    player_t& t = *sim -> actor_list[ i ];
    if ( ! t.sleeping && ( harmful == t.is_enemy() ) && &t != target )
      tl.push_back( &t );
  }

  return tl;
}

// action_t::target_list ====================================================

std::vector<player_t*> action_t::target_list() const
{
  // A very simple target list for aoe spells, pick any and all targets, up to
  // aoe amount, or if aoe == -1, pick all (enemy) targets

  std::vector<player_t*> t = available_targets();

  if ( aoe != -1 && t.size() > static_cast< size_t >( aoe + 1 ) )
  {
    // Drop out targets from the end
    t.resize( aoe + 1 );
  }

  return t;
}

// action_t::execute ========================================================

void action_t::execute()
{
  if ( unlikely( ! initialized ) )
  {
    sim -> errorf( "action_t::execute: action %s from player %s is not initialized.\n", name(), player -> name() );
    assert( 0 );
  }

  if ( sim -> log && ! dual )
  {
    log_t::output( sim, "%s performs %s (%.0f)", player -> name(), name(),
                   player -> resource_current[ player -> primary_resource() ] );
  }

  if ( unlikely( harmful && ! player -> in_combat ) )
  {
    if ( sim -> debug )
      log_t::output( sim, "%s enters combat.", player -> name() );

    player -> in_combat = true;
  }

  player_buff();

  if ( aoe )
  {
    std::vector< player_t* > tl = target_list();

    for ( size_t t = 0; t < tl.size(); t++ )
    {
      target_debuff( tl[ t ], DMG_DIRECT );

      // no overarching hit/miss for weapon ticks. assessed per tick
      if ( ! td.weapon )
      {
        calculate_result();

        if ( result_is_hit() )
          direct_dmg = calculate_direct_damage( t + 1 );
      }

      schedule_travel( tl[ t ] );
    }
  }
  else
  {
    target_debuff( target, DMG_DIRECT );

    // no overarching hit/miss for weapon ticks. assessed per tick
    if ( ! td.weapon )
    {
      calculate_result();

      if ( result_is_hit() )
        direct_dmg = calculate_direct_damage();
    }

    schedule_travel( target );
  }

  consume_resource();

  update_ready();

  if ( ! dual ) stats -> add_execute( time_to_execute );

  if ( repeating && ! proc ) schedule_execute();

  if ( harmful && callbacks )
  {
    if ( result != RESULT_NONE )
      action_callback_t::trigger( player -> attack_callbacks[ result ], this );

    if ( ! background ) // OnSpellCast
      action_callback_t::trigger( player -> attack_callbacks[ RESULT_NONE ], this );
  }
}

// action_t::calculate_result ===============================================

void action_t::calculate_result()
{
  direct_dmg = 0;
  result = RESULT_NONE;

  if ( ! harmful ) return;

  // First roll: Accuracy vs. Defense determines if attack lands
  double random = sim -> real();
  double accuracy = 1.0 + total_accuracy();
  if (
      ( td.weapon && td.weapon -> slot == SLOT_OFF_HAND )
      || ( weapon && weapon -> slot == SLOT_OFF_HAND )
  )
    accuracy -= 0.33;

  if ( random < accuracy - target_avoidance )
  {
    // Second roll: Crit vs. Shield chance
    // Kinetic and Energy are only school's that can be shielded
    random = sim -> real();

    if ( ( ( td.weapon && tick_may_crit ) || ( !td.weapon && may_crit ) )
        && random < total_crit() )
      result = RESULT_CRIT;
    else if ( random > 1.0 - target_shield && (school == SCHOOL_KINETIC || school == SCHOOL_ENERGY) )
      result = RESULT_BLOCK;
    else
      result = RESULT_HIT;
  }
  else if ( random < accuracy )
    result = RESULT_AVOID;
  else
    result = RESULT_MISS;

  if ( sim -> debug ) log_t::output( sim, "%s result for %s is %s", player -> name(), name(), util_t::result_type_string( result ) );
}

// action_t::tick_ ==========================================================

void action_t::tick_( timespan_t tick_time )
{

  tick_dmg = 0;
  if ( td.weapon )
  {
    calculate_result();
    if ( result_is_hit() )
    {
      player_buff();
      target_debuff( target, type == ACTION_HEAL ? HEAL_OVER_TIME : DMG_OVER_TIME );
      tick_dmg = calculate_tick_damage();
    }
  }
  else
  {
    calculate_result();
    if( result_is_hit() )
    {
        player_buff(); // 23/01/2012 According to http://sithwarrior.com/forums/Thread-Madness-Balance-Sorcerer-DPS-Compendium--573?pid=11311#pid11311
        target_debuff( target, type == ACTION_HEAL ? HEAL_OVER_TIME : DMG_OVER_TIME );
        if ( tick_may_crit )
        {
          if ( sim -> roll( total_crit() ) )
            result = RESULT_CRIT;
        }

        tick_dmg = calculate_tick_damage();
    }
  }

  assess_damage( target, tick_dmg, type == ACTION_HEAL ? HEAL_OVER_TIME : DMG_OVER_TIME, result );

  if ( harmful && callbacks ) action_callback_t::trigger( player -> tick_callbacks[ result ], this );

  stats -> add_tick( tick_time );
}

// action_t::tick ===========================================================

void action_t::tick( dot_t* d )
{
  if ( sim -> debug ) log_t::output( sim, "%s ticks (%d of %d)", name(), d -> current_tick, d -> num_ticks );

  tick_( d -> time_to_tick );
}

// action_t::extra_tick======================================================

void action_t::extra_tick()
{
  if ( sim -> debug ) log_t::output( sim, "%s ticks (extra)", name() );

  tick_( timespan_t::zero() );
}

// action_t::last_tick ======================================================

void action_t::last_tick( dot_t* d )
{
  if ( sim -> debug ) log_t::output( sim, "%s fades from %s", d -> name(), target -> name() );

  d -> ticking = false;

  //if ( school == SCHOOL_BLEED ) target -> debuffs.bleeding -> decrement();
}

// action_t::impact =========================================================

void action_t::impact( player_t* t, result_type impact_result, double travel_dmg=0 )
{
  // XXX what's this and do we need it for td.weapon hacks
  if ( ! td.weapon )
    assess_damage( t, travel_dmg, type == ACTION_HEAL ? HEAL_DIRECT : DMG_DIRECT, impact_result );

  // HACK: Set target so aoe dots work
  player_t* orig_target = target;
  target = t;

  if ( td.weapon || result_is_hit( impact_result ) )
  {
    if ( num_ticks > 0 )
    {
      dot_t* dot = this -> dot();
      assert( dot );
      if ( dot_behavior != DOT_REFRESH ) dot -> cancel();
      dot -> action = this;
      dot -> num_ticks = hasted_num_ticks();
      dot -> current_tick = 0;
      dot -> added_ticks = 0;
      dot -> added_seconds = timespan_t::zero();
      if ( dot -> ticking )
      {
        assert( dot -> tick_event );
        if ( ! channeled )
        {
          // Recasting a dot while it's still ticking gives it an extra tick in total
          dot -> num_ticks++;

          // tick_zero dots tick again when reapplied
          if ( tick_zero )
          {
            tick( dot );
          }
        }
      }
      else
      {
        //if ( school == SCHOOL_BLEED ) target -> debuffs.bleeding -> increment();

        dot -> schedule_tick();

        if ( sim -> log && ( ! result_is_hit( impact_result ) && ! td.weapon ) )
        {
          log_t::output( sim, "%s %s ticks (%d of %d) %s (miss)",
                         dot -> action -> player -> name(), dot -> action -> name(),
                         dot -> current_tick, dot -> num_ticks,
                         dot -> action -> target -> name() );
        }
      }
      dot -> recalculate_ready();

      if ( sim -> debug )
        log_t::output( sim, "%s extends dot-ready to %.2f for %s (%s)",
                       player -> name(), to_seconds( dot -> ready ), name(), dot -> name() );
    }
  }
  else
  {
    if ( sim -> log )
    {
      log_t::output( sim, "Target %s avoids %s %s (%s)", target -> name(), player -> name(), name(), util_t::result_type_string( impact_result ) );
    }
  }

  // Reset target
  target = orig_target;
}

// action_t::assess_damage ==================================================

void action_t::assess_damage( player_t*   t,
                              double      dmg_amount,
                              dmg_type    dmg_type,
                              result_type dmg_result )
{
  double dmg_adjusted = t -> assess_damage( dmg_amount, school, dmg_type, dmg_result, this );
  double actual_amount = t -> infinite_resource[ RESOURCE_HEALTH ] ? dmg_adjusted : std::min( dmg_adjusted, t -> resource_current[ RESOURCE_HEALTH ] );

  if ( dmg_type == DMG_DIRECT )
  {
    if ( sim -> log )
    {
      log_t::output( sim, "%s %s hits %s for %.0f %s damage (%s)",
                     player -> name(), name(),
                     t -> name(), dmg_adjusted,
                     util_t::school_type_string( school ),
                     util_t::result_type_string( dmg_result ) );
    }

    direct_dmg = dmg_adjusted;

    if ( callbacks ) action_callback_t::trigger( player -> direct_damage_callbacks[ school ], this );
  }
  else // DMG_OVER_TIME
  {
    if ( sim -> log )
    {
      dot_t* dot = this -> dot();
      log_t::output( sim, "%s %s ticks (%d of %d) %s for %.0f %s damage (%s)",
                     player -> name(), name(),
                     dot -> current_tick, dot -> num_ticks,
                     t -> name(), dmg_adjusted,
                     util_t::school_type_string( school ),
                     util_t::result_type_string( dmg_result ) );
    }

    tick_dmg = dmg_adjusted;

    if ( callbacks ) action_callback_t::trigger( player -> tick_damage_callbacks[ school ], this );
  }

  stats -> add_result( actual_amount, dmg_adjusted, ( direct_tick ? DMG_OVER_TIME : dmg_type ), dmg_result );
}

// action_t::additional_damage ==============================================

void action_t::additional_damage( player_t*   t,
                                  double      dmg_amount,
                                  dmg_type    dmg_type,
                                  result_type dmg_result )
{
  dmg_amount /= target_multiplier; // FIXME! Weak lip-service to the fact that the adds probably will not be properly debuffed.
  double dmg_adjusted = t -> assess_damage( dmg_amount, school, dmg_type, dmg_result, this );
  double actual_amount = std::min( dmg_adjusted, t -> resource_current[ resource ] );
  stats -> add_result( actual_amount, dmg_amount, dmg_type, dmg_result );
}

// action_t::alacrity =======================================================

double action_t::alacrity() const
{ return player -> alacrity(); }

// action_t::execute_time ===================================================

timespan_t action_t::execute_time() const
{
  if ( unlikely( ! harmful && ! player -> in_combat ) ||
       base_execute_time == timespan_t::zero() )
    return timespan_t::zero();
  else
    return base_execute_time / alacrity();
}

// action_t::schedule_execute ===============================================

void action_t::schedule_execute()
{
  if ( sim -> log )
    log_t::output( sim, "%s schedules execute for %s", player -> name(), name() );

  time_to_execute = execute_time();

  execute_event = new ( sim ) action_execute_event_t( this, time_to_execute );

  if ( ! background )
  {
    player -> executing = this;
    player -> gcd_ready = sim -> current_time + gcd();
    if ( player -> action_queued && sim -> strict_gcd_queue )
    {
      player -> gcd_ready -= sim -> queue_gcd_reduction;
    }
  }
}

// action_t::schedule_travel ================================================

void action_t::schedule_travel( player_t* t )
{
  time_to_travel = travel_time();

  if ( time_to_travel == timespan_t::zero() )
  {
    impact( t, result, direct_dmg );
  }
  else
  {
    if ( sim -> log )
    {
      log_t::output( sim, "%s schedules travel (%.2f) for %s",
                     player -> name(), to_seconds( time_to_travel ), name() );
    }

    travel_event = new ( sim ) action_travel_event_t( t, this, time_to_travel );
  }
}

// action_t::reschedule_execute =============================================

void action_t::reschedule_execute( timespan_t time )
{
  if ( sim -> log )
  {
    log_t::output( sim, "%s reschedules execute for %s", player -> name(), name() );
  }

  timespan_t delta_time = sim -> current_time + time - execute_event -> occurs();

  time_to_execute += delta_time;

  if ( delta_time > timespan_t::zero() )
  {
    execute_event -> reschedule( time );
  }
  else // Impossible to reschedule events "early".  Need to be canceled and re-created.
  {
    event_t::cancel( execute_event );
    execute_event = new ( sim ) action_execute_event_t( this, time );
  }
}

// action_t::update_ready ===================================================

void action_t::update_ready()
{
  timespan_t delay = timespan_t::zero();
  if ( cooldown -> duration > timespan_t::zero() && ! dual )
  {

    if ( ! background && ! proc )
    {
      timespan_t lag, dev;

      lag = player -> world_lag_override ? player -> world_lag : sim -> world_lag;
      dev = player -> world_lag_stddev_override ? player -> world_lag_stddev : sim -> world_lag_stddev;
      delay = player -> rngs.lag_world -> gauss( lag, dev );
      if ( sim -> debug ) log_t::output( sim, "%s delaying the cooldown finish of %s by %f", player -> name(), name(), to_seconds( delay ) );
    }

    cooldown -> start( timespan_t_min(), delay );

    if ( sim -> debug ) log_t::output( sim, "%s starts cooldown for %s (%s). Will be ready at %.4f", player -> name(), name(), cooldown -> name(), to_seconds( cooldown -> ready ) );
  }
  if ( num_ticks )
  {
    if ( result_is_miss() )
    {
      dot_t* dot = this -> dot();
      last_reaction_time = player -> total_reaction_time();
      if ( sim -> debug )
        log_t::output( sim, "%s pushes out re-cast (%.2f) on miss for %s (%s)",
                       player -> name(), to_seconds( last_reaction_time ), name(), dot -> name() );

      dot -> miss_time = sim -> current_time;
    }
  }
}

// action_t::usable_moving ==================================================

bool action_t::usable_moving()
{
  bool usable = true;

  if ( execute_time() > timespan_t::zero() )
    return false;

  if ( channeled )
    return false;

  if ( range > 0 && range <= 5 )
    return false;

  return usable;
}

// action_t::ready ==========================================================

bool action_t::ready()
{
  player_t* t = target;

  if ( player -> skill < 1.0 )
    if ( ! sim -> roll( player -> skill ) )
      return false;

  if ( cooldown -> remains() != timespan_t::zero() )
    return false;

  if ( ! player -> resource_available( resource, cost() ) )
    return false;

  if ( if_expr && ! if_expr -> success() )
    return false;

  if ( sync_action && ! sync_action -> ready() )
    return false;

  if ( unlikely( t -> sleeping ) )
    return false;

  if ( target -> debuffs.invulnerable -> check() )
    if ( harmful )
      return false;

  if ( player -> is_moving() )
    if ( ! usable_moving() )
      return false;

  if ( moving != -1 )
    if ( ( moving != 0 ) != player -> is_moving() )
      return false;

  if ( vulnerable )
    if ( ! t -> debuffs.vulnerable -> check() )
      return false;

  if ( invulnerable )
    if ( ! t -> debuffs.invulnerable -> check() )
      return false;

  if ( not_flying )
    if ( t -> debuffs.flying -> check() )
      return false;

  if ( flying )
    if ( ! t -> debuffs.flying -> check() )
      return false;

  return true;
}

// action_t::init ===========================================================

void action_t::init()
{
  if ( initialized ) return;

  for ( result_type i=RESULT_NONE; i < RESULT_MAX; ++i )
  {
    rng[ i ] = player -> get_rng( std::string( name() ) + '_' + util_t::result_type_string( i ),
                                  ( ( i == RESULT_CRIT ) ? RNG_DISTRIBUTED : RNG_CYCLIC ) );
  }

  if ( ! rank_level )
  {
    if ( rank_level_list.empty() )
      rank_level = player -> level;
    else
    {
      assert( boost::is_sorted( rank_level_list ) );
      for ( int rl : rank_level_list )
      {
        if ( player -> level < rl ) break;
        rank_level = rl;
      }
    }
  }

  // if min/max mods change during combat this needs to be re set
  set_base_min_max();

  if ( ! sync_str.empty() )
  {
    sync_action = player -> find_action( sync_str );

    if ( ! sync_action )
    {
      sim -> errorf( "Unable to find sync action '%s' for primary action '%s'\n", sync_str.c_str(), name() );
      sim -> cancel();
    }
  }

  if ( ! if_expr_str.empty() )
    if_expr = expr_t::parse( this, if_expr_str );

  if ( ! interrupt_if_expr_str.empty() )
    interrupt_if_expr = expr_t::parse( this, interrupt_if_expr_str );

  if ( sim -> travel_variance && travel_speed && player -> distance )
    rng_travel = player -> get_rng( name_str + "_travel", RNG_DISTRIBUTED );

  if ( sim -> debug )
    log_t::output( sim, "%s initialised: ranklevel(%d amount:%f) dd.bmin(%f) dd.bmax(%f) td.bmin(%f) tdbmax(%f)",
        name(), rank_level, get_standard_rank_amount(), dd.base_min, dd.base_max, td.base_min, td.base_max  );

  initialized = true;
}

// action_t::get_standard_rank_amount =======================================

double action_t::get_standard_rank_amount() const
{
  return
    ( type == ACTION_HEAL || type == ACTION_ABSORB ) ?
      rating_t::standardhealth_healing( rank_level ) :
      rating_t::standardhealth_damage( rank_level );
}

// action_t::set_base_min_max ===============================================

void action_t::set_base_min_max()
{
  const double standard_rank_amount = get_standard_rank_amount();

  if ( dd.standardhealthpercentmin > 0 )
    dd.base_min = dd.standardhealthpercentmin * standard_rank_amount;

  if ( dd.standardhealthpercentmax > 0 )
    dd.base_max = dd.standardhealthpercentmax * standard_rank_amount;

  if ( td.standardhealthpercentmin > 0 )
    td.base_min = td.standardhealthpercentmin * standard_rank_amount;

  if ( td.standardhealthpercentmax > 0 )
    td.base_max = td.standardhealthpercentmax * standard_rank_amount;
}

// action_t::reset ==========================================================

void action_t::reset()
{
  cooldown -> reset();
  result = RESULT_NONE;
  execute_event = 0;
  travel_event = 0;
}

// action_t::cancel =========================================================

void action_t::cancel()
{
  if ( sim -> debug ) log_t::output( sim, "action %s of %s is canceled", name(), player -> name() );

  if ( channeled )
  {
    dot_t* dot = this -> dot();
    if ( dot -> ticking )
    {
      last_tick( dot );
      event_t::cancel( dot -> tick_event );
      dot -> reset();
    }
  }

  if ( player -> executing  == this ) player -> executing  = 0;
  if ( player -> channeling == this ) player -> channeling = 0;

  event_t::cancel( execute_event );
}

// action_t::interrupt ======================================================

void action_t::interrupt_action()
{
  if ( sim -> debug ) log_t::output( sim, "action %s of %s is interrupted", name(), player -> name() );

  if ( cooldown -> duration > timespan_t::zero() && ! dual )
  {
    if ( sim -> debug ) log_t::output( sim, "%s starts cooldown for %s (%s)", player -> name(), name(), cooldown -> name() );

    cooldown -> start();
  }

  if ( player -> executing  == this ) player -> executing  = 0;
  if ( player -> channeling == this )
  {
    player -> channeling = 0;

    dot_t* dot = this -> dot();
    if ( dot -> ticking ) last_tick( dot );
    event_t::cancel( dot -> tick_event );
    dot -> reset();
  }

  event_t::cancel( execute_event );
}

// action_t::check_talent ===================================================

void action_t::check_talent( int talent_rank )
{
  if ( talent_rank != 0 ) return;

  if ( player -> is_pet() )
  {
    pet_t* p = player -> cast_pet();
    sim -> errorf( "Player %s has pet %s attempting to execute action %s without the required talent.\n",
                   p -> owner -> name(), p -> name(), name() );
  }
  else
  {
    sim -> errorf( "Player %s attempting to execute action %s without the required talent.\n", player -> name(), name() );
  }

  background = true; // prevent action from being executed
}

// action_t::check_race =====================================================

void action_t::check_race( race_type race )
{
  if ( player -> race != race )
  {
    sim -> errorf( "Player %s attempting to execute action %s while not being a %s.\n", player -> name(), name(), util_t::race_type_string( race ) );

    background = true; // prevent action from being executed
  }
}

// action_t::check_spec =====================================================

void action_t::check_spec( talent_tree_type necessary_spec )
{
  if ( player -> primary_tree() != necessary_spec )
  {
    sim -> errorf( "Player %s attempting to execute action %s without %s spec.\n",
                   player -> name(), name(), util_t::talent_tree_string( necessary_spec ) );

    background = true; // prevent action from being executed
  }
}

// action_t::create_expression ==============================================

expr_ptr action_t::create_expression( const std::string& name_str )
{
  if ( name_str == "ticking" )
    return make_expr( name_str, [this](){ return dot() -> ticking; } );

  if ( name_str == "ticks" )
    return make_expr( name_str, [this](){ return dot() -> current_tick; } );

  if ( name_str == "ticks_remain" )
    return make_expr( name_str, [this](){ return dot() -> ticks(); } );

  if ( name_str == "remains" )
    return make_expr( name_str, [this](){ return to_seconds( dot() -> remains() ); } );

  if ( name_str == "cast_time" )
    return make_expr( name_str, [this](){ return to_seconds( execute_time() ); } );

  if ( name_str == "cooldown" )
    return make_expr( name_str, [this](){ return to_seconds( cooldown -> duration ); } );

  if ( name_str == "tick_time" )
    return make_expr( name_str, [this](){ return ( dot() -> ticking ? to_seconds( tick_time() ) : 0.0 ); } );

  if ( name_str == "gcd" )
    return make_expr( name_str, [this](){ return to_seconds( gcd() ); } );

  if ( name_str == "travel_time" )
    return make_expr( name_str, [this](){ return to_seconds( travel_time() ); } );

  if ( name_str == "in_flight" )
    return make_expr( name_str, [this](){ return travel_event != nullptr; } );

  if ( name_str == "miss_react" )
  {
    return make_expr( name_str, [this]() -> bool
      {
        timespan_t miss_time = dot() -> miss_time;
        return miss_time <= timespan_t::zero() ||
               sim -> current_time >= ( miss_time + last_reaction_time );
      } );
  }

  if ( name_str == "cast_delay" )
  {
    return make_expr( name_str, [this]() -> bool
      {
        if ( sim -> debug )
        {
          log_t::output( sim, "%s %s cast_delay(): can_react_at=%f cur_time=%f",
                         player -> name_str.c_str(),
                         this -> name_str.c_str(),
                         to_seconds( player -> cast_delay_occurred + player -> cast_delay_reaction ),
                         to_seconds( sim -> current_time ) );
        }

        return player -> cast_delay_occurred <= timespan_t::zero() ||
               player -> cast_delay_occurred + player -> cast_delay_reaction < sim -> current_time;
      } );
  }

  std::vector<std::string> splits;
  int num_splits = util_t::string_split( splits, name_str, "." );

  if ( num_splits == 2 && splits[ 0 ] == "prev" )
  {
    std::string& prev = splits[ 1 ];
    return make_expr( "prev", [this,prev]
      { return player -> last_foreground_action &&
               player -> last_foreground_action -> name_str == prev; } );
  }

  if ( num_splits == 3 && ( splits[0] == "buff" || splits[0] == "debuff" ) )
  {
    if ( targetdata_t* td = player -> get_targetdata( target ) )
    {
      buff_t* buff;
      if ( player == target )
        buff = td -> get_buff( splits[ 1 ] );
      else
        buff = td -> get_buff( splits[ 1 ] + ':' + player->name() );

      if ( buff )
        return buff -> create_expression( splits[ 2 ] );
    }
  }

  if ( num_splits == 3 && splits[0] == "aura" )
  {
    if ( targetdata_t* td = player -> get_targetdata( target ) )
    {
      if ( buff_t* buff = td -> get_buff( splits[ 1 ] ) )
        return buff -> create_expression( splits[ 2 ] );
    }
  }

  if ( num_splits == 3 && splits[0] == "dot" )
  {
    if ( targetdata_t* td = player -> get_targetdata( target ) )
    {
      if ( dot_t* dot = td -> get_dot( splits[ 1 ] ) )
        return dot -> create_expression( splits[ 2 ] );
    }
  }

  if ( num_splits >= 2 && ( splits[ 0 ] == "debuff" || splits[ 0 ] == "dot" ) )
  {
    return target -> create_expression( this, name_str );
  }

  if ( num_splits >= 2 && splits[ 0 ] == "aura" )
    return sim -> create_expression( this, name_str );

  if ( num_splits > 1 && splits[ 0 ] == "target" )
    return target -> create_expression( this, join( splits.begin() + 1, splits.end(), '.' ) );

  if ( num_splits > 2 && splits[ 0 ] == "actor" )
  {
    // Find target
    player_t* expr_target = sim -> find_player( splits[ 1 ] );
    if ( ! expr_target )
    {
      sim -> errorf( "Unable to find actor %s for expression %s", splits[ 1 ].c_str(), name_str.c_str() );
      sim -> cancel();
      return nullptr;
    }

    return expr_target -> create_expression( this, join( splits.begin() + 2, splits.end(), '.' ) );
  }

  // necessary for self.target.*, self.dot.*
  if ( num_splits > 1 && splits[ 0 ] == "self" )
    return player -> create_expression( this, join( splits.begin() + 1, splits.end(), '.' ) );

  // necessary for sim.target.*
  if ( num_splits >= 2 && splits[ 0 ] == "sim" )
    return sim -> create_expression( this, join( splits.begin() + 1, splits.end(), '.' ) );

  return player -> create_expression( this, name_str );
}

// action_t::ppm_proc_chance ================================================

double action_t::ppm_proc_chance( double PPM ) const
{
  timespan_t time = channeled ? dot() -> time_to_tick : time_to_execute;

  if ( time == timespan_t::zero() ) time = player -> base_gcd;

  return ( PPM * to_minutes( time ) );
}

// action_t::tick_time ======================================================

timespan_t action_t::tick_time() const
{
  timespan_t t = base_tick_time;
  if ( channeled || hasted_ticks )
  {
    t /= player_alacrity;
  }
  return t;
}

// action_t::hasted_num_ticks ===============================================

int action_t::hasted_num_ticks( timespan_t d ) const
{
  if ( ! hasted_ticks ) return num_ticks;

  assert( player_alacrity > 0.0 );

  // For the purposes of calculating the number of ticks, the tick time is rounded to the 3rd decimal place.
  // It's important that we're accurate here so that we model alacrity breakpoints correctly.

  if ( d < timespan_t::zero() )
    d = num_ticks * base_tick_time;

  timespan_t t = from_millis( (int) ( to_millis( base_tick_time ) / player_alacrity + 0.5 ) );

  double n = d / t;

  // banker's rounding
  if ( n - 0.5 == ( double ) ( int ) n && ( ( int ) n ) % 2 == 0 )
    return ( int ) std::ceil ( n - 0.5 );

  return ( int ) std::floor( n + 0.5 );
}
