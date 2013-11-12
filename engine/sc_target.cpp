// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// ==========================================================================
// Enemy
// ==========================================================================

struct enemy_t : public player_t
{
  double fixed_health, initial_health;
  double fixed_health_percentage, initial_health_percentage;
  timespan_t waiting_time;

  enemy_t( sim_t* s, const std::string& n, race_type r = RACE_HUMANOID ) :
    player_t( s, ENEMY, n, r ),
    fixed_health( 0 ), initial_health( 0 ),
    fixed_health_percentage( 0 ), initial_health_percentage( 100.0 ),
    waiting_time( from_seconds( 1.0 ) )

  {
    player_t** last = &( sim -> target_list );
    while ( *last ) last = &( ( *last ) -> next );
    *last = this;
    next = 0;

    create_options();
  }

  virtual void init_defense()
  {
    player_t::init_defense();

    // Current best guess
    base_melee_avoidance = base_range_avoidance = .10;
    base_force_avoidance = base_tech_avoidance = .10;
  }

// target_t::combat_begin ===================================================

  virtual void combat_begin()
  {
    player_t::combat_begin();

    if ( sim -> overrides.bleeding ) debuffs.bleeding -> override();
  }

// target_t::primary_role ===================================================

  virtual role_type primary_role() const
  { return ROLE_TANK; }

  virtual resource_type primary_resource() const
  { return RESOURCE_HEALTH; }

  virtual action_t* create_action( const std::string& name, const std::string& options_str );
  virtual void init();
  virtual void init_base();
  virtual void init_resources( bool force=false );
  virtual void init_target();
  virtual void init_actions();
  virtual void create_options();
  virtual pet_t* create_pet( const std::string& add_name, const std::string& pet_type = std::string() );
  virtual void create_pets();
  virtual pet_t* find_pet( const std::string& add_name );
  virtual double health_percentage() const;
  virtual void combat_end();
  virtual void recalculate_health();
  virtual expr_ptr create_expression( action_t* action, const std::string& type );
  virtual timespan_t available() const { return waiting_time; }
};

// ==========================================================================
// Enemy Add
// ==========================================================================

struct enemy_add_t : public pet_t
{
  enemy_add_t( sim_t* s, player_t* o, const std::string& n, pet_type pt = PET_ENEMY ) :
    pet_t( s, o, n, pt, ENEMY_ADD )
  {
    create_options();
  }

  virtual void init_actions()
  {
    if ( action_list_str.empty() )
    {
      action_list_str += "/snapshot_stats";
    }

    pet_t::init_actions();
  }

  virtual resource_type primary_resource() const
  { return RESOURCE_HEALTH; }

  virtual action_t* create_action( const std::string& name, const std::string& options_str );
};

namespace { // ANONYMOUS NAMESPACE ==========================================

// Melee ====================================================================

struct melee_attack_t : public action_t
{
  melee_attack_t( player_t* player, const std::string& options_str ) :
    action_t( ACTION_ATTACK, "melee_attack" , player, melee_policy, RESOURCE_NONE, SCHOOL_KINETIC )
  {
    dd.base_min = 2000;

    int aoe_tanks = 0;
    option_t options[] =
    {
      { "damage",       OPT_FLT,      &dd.base_min          },
      { "trigger_gcd",  OPT_TIMESPAN, &trigger_gcd    },
      { "cooldown",     OPT_TIMESPAN, &cooldown -> duration },
      { "aoe_tanks",    OPT_BOOL,     &aoe_tanks },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    if ( aoe_tanks == 1 )
      aoe = -1;
    dd.base_max = dd.base_min;
    if ( trigger_gcd <= timespan_t::zero() )
      trigger_gcd = from_seconds( 3.0 );

    may_crit    = true;
    base_cost   = 0;
  }

  virtual std::vector<player_t*> available_targets() const
  {
    std::vector<player_t*> tl;
    tl.push_back( target );

    for ( size_t i = 0; i < sim -> actor_list.size(); i++ )
    {
      if ( ! sim -> actor_list[ i ] -> sleeping &&
           !sim -> actor_list[ i ] -> is_enemy() && sim -> actor_list[ i ] -> primary_role() == ROLE_TANK &&
           sim -> actor_list[ i ] != target )
        tl.push_back( sim -> actor_list[ i ] );
    }

    return tl;
  }
};

// Spell Nuke ===============================================================

struct spell_nuke_t : public action_t
{
  spell_nuke_t( player_t* p, const std::string& options_str ) :
    action_t( ACTION_ATTACK, "spell_nuke", p, force_policy, RESOURCE_NONE, SCHOOL_INTERNAL )
  {
    base_execute_time = from_seconds( 3.0 );
    dd.base_min = 4000;

    cooldown = player -> get_cooldown( name_str + "_" + target -> name() );

    int aoe_tanks = 0;
    option_t options[] =
    {
      { "damage",       OPT_FLT,      &dd.base_min          },
      { "attack_speed", OPT_TIMESPAN, &base_execute_time    },
      { "cooldown",     OPT_TIMESPAN, &cooldown -> duration },
      { "aoe_tanks",    OPT_BOOL,     &aoe_tanks },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    dd.base_max = dd.base_min;
    if ( base_execute_time < timespan_t::zero() )
      base_execute_time = from_seconds( 1.0 );

    stats = player -> get_stats( name_str + "_" + target -> name(), this );
    stats -> school = school;
    name_str = name_str + "_" + target -> name();

    may_crit = false;
    if ( aoe_tanks == 1 )
      aoe = -1;
  }

  virtual std::vector<player_t*> available_targets() const
  {
    std::vector<player_t*> tl;
    tl.push_back( target );

    for ( size_t i = 0; i < sim -> actor_list.size(); i++ )
    {
      if ( ! sim -> actor_list[ i ] -> sleeping &&
           !sim -> actor_list[ i ] -> is_enemy() && sim -> actor_list[ i ] -> primary_role() == ROLE_TANK &&
           sim -> actor_list[ i ] != target )
        tl.push_back( sim -> actor_list[ i ] );
    }

    return tl;
  }
};

// Spell AoE ================================================================

struct spell_aoe_t : public action_t
{
  spell_aoe_t( player_t* p, const std::string& options_str ) :
    action_t( ACTION_ATTACK, "spell_aoe", p, force_policy, RESOURCE_NONE, SCHOOL_ELEMENTAL )
  {
    base_execute_time = from_seconds( 3.0 );
    dd.base_min = 50000;

    cooldown = player -> get_cooldown( name_str + "_" + target -> name() );

    option_t options[] =
    {
      { "damage",    OPT_FLT,      &dd.base_min          },
      { "cast_time", OPT_TIMESPAN, &base_execute_time    },
      { "cooldown",  OPT_TIMESPAN, &cooldown -> duration },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    dd.base_max = dd.base_min;
    if ( base_execute_time < from_seconds( 0.01 ) )
      base_execute_time = from_seconds( 3.0 );

    stats = player -> get_stats( name_str + "_" + target -> name(), this );
    stats -> school = school;
    name_str = name_str + "_" + target -> name();

    may_crit = false;

    aoe = -1;
  }

  virtual std::vector<player_t*> available_targets() const
  {
    std::vector<player_t*> tl;
    tl.push_back( target );

    for ( size_t i = 0; i < sim -> actor_list.size(); i++ )
    {
      if ( ! sim -> actor_list[ i ] -> sleeping &&
           !sim -> actor_list[ i ] -> is_enemy() &&
           sim -> actor_list[ i ] != target )
        tl.push_back( sim -> actor_list[ i ] );
    }

    return tl;
  }
};

// Summon Add ===============================================================

struct summon_add_t : public action_t
{
  std::string add_name;
  timespan_t summoning_duration;
  pet_t* pet;

  summon_add_t( player_t* player, const std::string& options_str ) :
    action_t( ACTION_OTHER, "summon_add", player ),
    summoning_duration( timespan_t::zero() ), pet( 0 )
  {
    option_t options[] =
    {
      { "name",     OPT_STRING,   &add_name             },
      { "duration", OPT_TIMESPAN, &summoning_duration   },
      { "cooldown", OPT_TIMESPAN, &cooldown -> duration },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    enemy_t* p = player -> cast_enemy();

    pet = p -> find_pet( add_name );
    if ( ! pet )
    {
      sim -> errorf( "Player %s unable to find pet %s for summons.\n", p -> name(), add_name.c_str() );
      sim -> cancel();
    }

    harmful = false;

    trigger_gcd = from_seconds( 1.5 );
  }

  virtual void execute()
  {
    enemy_t* p = player -> cast_enemy();

    action_t::execute();

    p -> summon_pet( add_name, summoning_duration );
  }

  virtual bool ready()
  {
    if ( ! pet -> sleeping )
      return false;

    return action_t::ready();
  }
};
}

// ==========================================================================
// Enemy Extensions
// ==========================================================================

// enemy_t::create_action ===================================================

action_t* enemy_t::create_action( const std::string& name,
                                  const std::string& options_str )
{
  if ( name == "melee_attack") return new  melee_attack_t( this, options_str );
  if ( name == "spell_nuke"  ) return new  spell_nuke_t( this, options_str );
  if ( name == "spell_aoe"   ) return new   spell_aoe_t( this, options_str );
  if ( name == "summon_add"  ) return new  summon_add_t( this, options_str );

  return player_t::create_action( name, options_str );
}

// enemy_t::init ============================================================

void enemy_t::init()
{
  level = sim -> max_player_level;

  if ( sim -> target_level >= 0 )
    level = sim -> target_level;

  player_t::init();
}

// enemy_t::init_base =======================================================

void enemy_t::init_base()
{
  waiting_time = std::min( from_seconds( std::min( to_seconds<int>( sim -> max_time ), sim -> wheel_seconds ) ),
                           from_seconds( 1.0 ) );

  if ( initial_armor <= 0 )
  {
    static const double target_armor_percentage = 0.35;
    initial_armor = ( 200 * level + 800 ) * target_armor_percentage / ( 1.0 - target_armor_percentage );
  }
  player_t::base_armor = initial_armor;

  fixed_health = sim -> target_health;
  initial_health = fixed_health;

  if ( ( initial_health_percentage < 1   ) ||
       ( initial_health_percentage > 100 ) )
  {
    initial_health_percentage = 100.0;
  }
}

// enemy_t::init_resources ==================================================

void enemy_t::init_resources( bool /* force */ )
{
  double health_adjust = 1.0 + sim -> vary_combat_length * sim -> iteration_adjust();

  resource_base[ RESOURCE_HEALTH ] = initial_health * health_adjust;

  player_t::init_resources( true );

  if ( initial_health_percentage > 0 )
  {
    resource_max[ RESOURCE_HEALTH ] *= 100.0 / initial_health_percentage;
  }
}

// enemy_t::init_target ====================================================

void enemy_t::init_target()
{
  if ( ! target_str.empty() )
  {
    target = sim -> find_player( target_str );
  }

  if ( target )
    return;

  for ( player_t* q = sim -> player_list; q; q = q -> next )
  {
    if ( q -> primary_role() != ROLE_TANK )
      continue;

    target = q;
    break;
  }

  if ( !target )
    target = sim -> target;
}

// enemy_t::init_actions ====================================================

void enemy_t::init_actions()
{
  if ( !is_add() )
  {
    if ( action_list_str.empty() )
    {
      action_list_str += "/snapshot_stats";

      if ( target != this )
      {
        action_list_str += "/spell_nuke,damage=2000,cooldown=10,attack_speed=1.0,aoe_tanks=1";
        action_list_str += "/melee_attack,damage=2000,trigger_gcd=3.0,aoe_tanks=1";
      }
    }
  }
  player_t::init_actions();

  // Small hack to increase waiting time for target without any actions
  for ( action_t* action = action_list; action; action = action -> next )
  {
    if ( action -> background ) continue;
    if ( action -> name_str == "snapshot_stats" ) continue;
    if ( action -> name_str.find( "auto_attack" ) != std::string::npos )
      continue;
    waiting_time = from_seconds( 1.0 );
    break;
  }
}

// enemy_t::create_options ==================================================

void enemy_t::create_options()
{
  option_t target_options[] =
  {
//    { "target_health",                    OPT_FLT,    &( fixed_health                      ) },
    { "target_initial_health_percentage", OPT_FLT,    &( initial_health_percentage         ) },
    { "target_fixed_health_percentage",   OPT_FLT,    &( fixed_health_percentage           ) },
    { "target_tank",                      OPT_STRING, &( target_str                        ) },
    { NULL, OPT_UNKNOWN, NULL }
  };

  option_t::copy( sim -> options, target_options );

  player_t::create_options();
}

// enemy_t::create_add ======================================================

pet_t* enemy_t::create_pet( const std::string& add_name, const std::string& /* pet_type */ )
{
  pet_t* p = find_pet( add_name );
  if ( p ) return p;

  return new enemy_add_t( sim, this, add_name, PET_ENEMY );

  return 0;
}

// enemy_t::create_pets =====================================================

void enemy_t::create_pets()
{
  for ( int i=0; i < sim -> target_adds; i++ )
    create_pet( ( boost::format( "add_%d" ) % i ).str() );
}

// enemy_t::find_add ========================================================

pet_t* enemy_t::find_pet( const std::string& add_name )
{
  for ( pet_t* p = pet_list; p; p = p -> next_pet )
    if ( p -> name_str == add_name )
      return p;

  return 0;
}

// enemy_t::health_percentage() =============================================

double enemy_t::health_percentage() const
{
  if ( fixed_health_percentage > 0 ) return fixed_health_percentage;

  if ( resource_base[ RESOURCE_HEALTH ] == 0 ) // first iteration
  {
    timespan_t remainder = std::max( timespan_t::zero(), ( sim -> expected_time - sim -> current_time ) );

    return ( remainder / sim -> expected_time ) * ( initial_health_percentage - sim -> target_death_pct ) + sim ->  target_death_pct;
  }

  return player_t::health_percentage();
}

// enemy_t::recalculate_health ==============================================

void enemy_t::recalculate_health()
{
  if ( sim -> expected_time <= timespan_t::zero() || fixed_health > 0 ) return;

  if ( initial_health == 0 ) // first iteration
  {
    initial_health = iteration_dmg_taken * ( sim -> expected_time / sim -> current_time );
  }
  else
  {
    timespan_t delta_time = sim -> current_time - sim -> expected_time;
    delta_time /= ( sim -> current_iteration + 1 ); // dampening factor
    double factor = 1.0 - ( delta_time / sim -> expected_time );

    if ( factor > 1.5 ) factor = 1.5;
    if ( factor < 0.5 ) factor = 0.5;

    initial_health *= factor;
  }

  if ( sim -> debug ) log_t::output( sim, "Target %s initial health calculated to be %.0f. Damage was %.0f", name(), initial_health, iteration_dmg_taken );
}

// enemy_t::create_expression ===============================================

expr_ptr enemy_t::create_expression( action_t* action,
                                     const std::string& name_str )
{
  if ( name_str == "adds" )
    return make_expr( name_str, [this]{ return active_pets; } );

  return player_t::create_expression( action, name_str );
}

// enemy_t::combat_end ======================================================

void enemy_t::combat_end()
{
  player_t::combat_end();

  recalculate_health();
}

// ==========================================================================
// Enemy Add Extensions
// ==========================================================================

action_t* enemy_add_t::create_action( const std::string& name,
                                      const std::string& options_str )
{
  //if ( name == "auto_attack"             ) return new              auto_attack_t( this, options_str );
  if ( name == "spell_nuke"              ) return new               spell_nuke_t( this, options_str );

  return pet_t::create_action( name, options_str );
}

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_enemy ===================================================

player_t* player_t::create_enemy( sim_t* sim, const std::string& name, race_type /* r */ )
{
  return new enemy_t( sim, name );
}

// player_t::enemy_init =====================================================

void player_t::enemy_init( sim_t* /* sim */ )
{

}

// player_t::enemy_combat_begin =============================================

void player_t::enemy_combat_begin( sim_t* sim )
{
  for ( player_t* p = sim -> target_list; p; p = p -> next )
  {
      if ( sim -> overrides.shatter_shot )
        p -> debuffs.shatter_shot -> override(1, -1); // move to correct class
      if ( sim -> overrides.shatter_shot >= 2 )
        p -> debuffs.shatter_shot_2 -> override(1, -1); // move to correct class
      if ( sim -> overrides.shatter_shot >= 3 )
        p -> debuffs.shatter_shot_3 -> override(1, -1); // move to correct class
      if ( sim -> overrides.shatter_shot >= 4 )
        p -> debuffs.shatter_shot_4 -> override(1, -1); // move to correct class
      if ( sim -> overrides.shatter_shot >= 5 )
        p -> debuffs.shatter_shot_5 -> override(1, -1); // move to correct class

      if ( sim -> overrides.sunder )
        p -> debuffs.sunder -> override(5, -1); // move to correct class
      if ( sim -> overrides.sunder >= 2 )
        p -> debuffs.sunder_2 -> override(5, -1); // move to correct class
      if ( sim -> overrides.sunder >= 3 )
        p -> debuffs.sunder_3 -> override(5, -1); // move to correct class
      if ( sim -> overrides.sunder >= 4 )
        p -> debuffs.sunder_4 -> override(5, -1); // move to correct class
      if ( sim -> overrides.sunder >= 5 )
        p -> debuffs.sunder_5 -> override(5, -1); // move to correct class

      if ( sim -> overrides.heat_signature )
        p -> debuffs.heat_signature -> override(5, -1); // move to correct class
      if ( sim -> overrides.heat_signature >= 2 )
        p -> debuffs.heat_signature_2 -> override(5, -1); // move to correct class
      if ( sim -> overrides.heat_signature >= 3 )
        p -> debuffs.heat_signature_3 -> override(5, -1); // move to correct class
      if ( sim -> overrides.heat_signature >= 4 )
        p -> debuffs.heat_signature_4 -> override(5, -1); // move to correct class
      if ( sim -> overrides.heat_signature >= 5 )
        p -> debuffs.heat_signature -> override(5, -1); // move to correct class
  }
}
