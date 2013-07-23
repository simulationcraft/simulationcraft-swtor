// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// TODO
// * Move all of the buffed_XXX stats into player_t::buffed.

namespace { // ANONYMOUS NAMESPACE ==========================================

// FIXME: Lazy init may interfere with "self.dot."
const bool LAZY_TARGETDATA_INIT = true;

// compare_talents ==========================================================

struct compare_talents
{
  bool operator()( const talent_t* l, const talent_t* r ) const
  {
    return ( l -> tab_page() < r -> tab_page() );
  }
};

// has_foreground_actions ===================================================

static bool has_foreground_actions( player_t* p )
{
  for ( action_t* a = p -> action_list; a; a = a -> next )
  {
    if ( ! a -> background ) return true;
  }
  return false;
}

// parse_talent_string ======================================================

static bool parse_talent_string( player_t& p, const std::string& url )
{
  p.talents_str = url;

  std::string::size_type hash_pos = url.find( '#' );

  if ( url.find( "torhead" ) != url.npos )
  {
    // http://torhead.com/skill-calc#...
    if ( hash_pos != url.npos )
      return torhead::parse_talents( p, url.substr( hash_pos + 4 ) );
  }

  else if ( url.find( "knotor" ) != url.npos )
  {
    // http://knotor.com/skills#.....
    if ( hash_pos != url.npos )
      return knotor::parse_talents( p, url.substr( hash_pos + 1 ) );
  }

  else if ( url.find( "mrrobot.com" ) != url.npos )
  {
    // http://swtor.askmrrobot.com/skills/[advancedclassname]#[tree 1]-[tree 2]-[tree 3]
    if ( hash_pos != url.npos )
      return mrrobot::parse_talents( p, url.substr( hash_pos + 1 ) );
  }

  else
  {
    bool all_digits = true;
    for ( size_t i=0; i < url.size() && all_digits; i++ )
    {
      if ( ! isdigit( url[ i ] ) )
        all_digits = false;
    }

    if ( all_digits )
      return p.parse_talents_armory( url );
  }

  p.sim -> errorf( "Unable to decode talent string %s for %s\n", url.c_str(), p.name() );

  return false;
}

static bool parse_talent_url( sim_t* sim,
                              const std::string& name,
                              const std::string& url )
{
  assert( name == "talents" ); ( void )name;
  assert( sim -> active_player );
  return parse_talent_string( *sim -> active_player, url );
}

// parse_role_string ========================================================

static bool parse_role_string( sim_t* sim,
                               const std::string& name,
                               const std::string& value )
{
  assert( name == "role" ); ( void )name;



  sim -> active_player -> role = util_t::parse_role_type( value );

  return true;
}

// parse_role_string ========================================================

static bool parse_set_bonus( sim_t* sim,
                             const std::string& opt_name,
                             const std::string& value )
{
  assert( opt_name == "set_bonus" ); ( void )opt_name;

  bool disable = value.find( "no_" ) == 0;
  std::string::size_type cut_pt, first = disable ? 3 : 0;

  bool is_2pc;
  if ( ( cut_pt = value.find( "_2pc", first ) ) != value.npos )
    is_2pc = true;
  else if ( ( cut_pt = value.find( "_4pc", first ) ) != value.npos )
    is_2pc = false;
  else
    return false;

  std::string name = value.substr( first, cut_pt - first );

  set_bonus_t* sb = sim -> active_player -> find_set_bonus( name );
  if ( ! sb )
    return false;

  if ( disable )
  {
    if ( is_2pc )
      sb -> force_disable_2pc = true;
    else
      sb -> force_disable_4pc = true;
  }
  else
  {
    if ( is_2pc )
      sb -> force_enable_2pc = true;
    else
      sb -> force_enable_4pc = true;
  }

  return true;
}

// parse_world_lag ==========================================================

static bool parse_world_lag( sim_t* sim,
                             const std::string& name,
                             const std::string& value )
{
  assert( name == "world_lag" ); ( void )name;

  sim -> active_player -> world_lag = from_seconds( atof( value.c_str() ) );

  if ( sim -> active_player -> world_lag < timespan_t::zero() )
  {
    sim -> active_player -> world_lag = timespan_t::zero();
  }

  sim -> active_player -> world_lag_override = true;

  return true;
}


// parse_world_lag ==========================================================

static bool parse_world_lag_stddev( sim_t* sim,
                                    const std::string& name,
                                    const std::string& value )
{
  assert( name == "world_lag_stddev" ); ( void )name;

  sim -> active_player -> world_lag_stddev = from_seconds( atof( value.c_str() ) );

  if ( sim -> active_player -> world_lag_stddev < timespan_t::zero() )
  {
    sim -> active_player -> world_lag_stddev = timespan_t::zero();
  }

  sim -> active_player -> world_lag_stddev_override = true;

  return true;
}

// parse_brain_lag ==========================================================

static bool parse_brain_lag( sim_t* sim,
                             const std::string& name,
                             const std::string& value )
{
  assert( name == "brain_lag" ); ( void )name;

  sim -> active_player -> brain_lag = from_seconds( atof( value.c_str() ) );

  if ( sim -> active_player -> brain_lag < timespan_t::zero() )
  {
    sim -> active_player -> brain_lag = timespan_t::zero();
  }

  return true;
}


// parse_brain_lag_stddev ===================================================

static bool parse_brain_lag_stddev( sim_t* sim,
                                    const std::string& name,
                                    const std::string& value )
{
  assert( name == "brain_lag_stddev" ); ( void )name;

  sim -> active_player -> brain_lag_stddev = from_seconds( atof( value.c_str() ) );

  if ( sim -> active_player -> brain_lag_stddev < timespan_t::zero() )
  {
    sim -> active_player -> brain_lag_stddev = timespan_t::zero();
  }

  return true;
}

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// Player
// ==========================================================================

// player_t::player_t =======================================================

player_t::player_t( sim_t*             s,
                    player_type        t,
                    const std::string& n,
                    race_type          r ) :
  sim( s ), name_str( n ),
  region_str( s -> default_region_str ), server_str( s -> default_server_str ),
  origin_str( "unknown" ), next( 0 ), index( -1 ), type( t ), role( ROLE_HYBRID ),
  target( 0 ), level( 50 ), use_pre_potion( 0 ), party( 0 ), member( 0 ),
  skill( 0 ), initial_skill( s -> default_skill ), distance( 0 ), default_distance( 0 ),
  gcd_ready( timespan_t::zero() ), base_gcd( from_seconds( 1.5 ) ), potion_used( 0 ),
  sleeping( 1 ), initial_sleeping( 0 ), initialized( 0 ), pet_list( 0 ), bugs( true ),
  specialization( TALENT_TAB_NONE ), invert_scaling( 0 ),
  vengeance_enabled( false ), vengeance_damage( 0.0 ), vengeance_value( 0.0 ),
  vengeance_max( 0.0 ), vengeance_was_attacked( false ),
  active_pets( 0 ),
  reaction_mean( from_seconds( 0.5 ) ), reaction_stddev( timespan_t::zero() ),
  reaction_nu( from_seconds( 0.5 ) ),
  scale_player( 1 ), avg_ilvl( 0 ), active_companion( 0 ), ptr( s -> ptr ),
  // Latency
  world_lag( from_seconds( 0.1 ) ), world_lag_stddev( timespan_t_min() ),
  brain_lag( timespan_t_min() ), brain_lag_stddev( timespan_t_min() ),
  world_lag_override( false ), world_lag_stddev_override( false ),
  events( 0 ),
  race( r == RACE_NONE ? RACE_HUMAN : r ),

  // for companion bonuses
  bonus_accuracy_pc_( 0 ), bonus_crit_pc_( 0 ), bonus_surge_pc_( 0 ), bonus_health_pc_( 0 ),

  // Ratings
  initial_accuracy_rating( 0 ), accuracy_rating_( 0 ), base_accuracy_( 0 ), computed_accuracy( 0 ),
  initial_alacrity_rating( 0 ), alacrity_rating_( 0 ), base_alacrity_( 0 ), computed_alacrity( 0 ),
  initial_crit_rating( 0 ), crit_rating( 0 ), base_crit_chance_( 0.05 ),
  initial_surge_rating( 0 ), surge_rating( 0 ),
  initial_defense_rating( 0 ), defense_rating( 0 ),
  initial_shield_rating( 0 ), shield_rating( 0 ),
  initial_absorb_rating( 0 ), absorb_rating( 0 ),
  crit_from_rating( 0 ), defense_from_rating( 0 ), shield_from_rating( 0 ), absorb_from_rating( 0 ),

  primary_attribute( ATTRIBUTE_NONE ), secondary_attribute( ATTRIBUTE_NONE ),

  // Attack Mechanics
  base_power( 0 ),       initial_power( 0 ),       power( 0 ),
  base_force_power( 0 ), initial_force_power( 0 ), force_power( 0 ),
  base_tech_power( 0 ),  initial_tech_power( 0 ),  tech_power( 0 ),
  surge_bonus( 0 ),
  base_armor_penetration( 1 ),

  position( POSITION_BACK ),

  // Defense Mechanics
  target_auto_attack( 0 ),
  base_armor( 0 ),         initial_armor( 0 ),       armor( 0 ),
  base_bonus_armor( 0 ),   initial_bonus_armor( 0 ), bonus_armor( 0 ),
  armor_multiplier( 1.0 ), initial_armor_multiplier( 1.0 ),

  base_shield_chance( 0 ), base_shield_absorb( 0 ),

  base_melee_avoidance( .05 ), base_range_avoidance( .05 ),
  base_force_avoidance( 0 ),   base_tech_avoidance( 0 ),

  base_DR( 0 ),

  // Consumables
  stim( stim_type::none ),
  // Events
  executing( 0 ), channeling( 0 ), readying( 0 ), off_gcd( 0 ), in_combat( false ), action_queued( false ),
  cast_delay_reaction( timespan_t::zero() ), cast_delay_occurred( timespan_t::zero() ),
  // Actions
  action_list( 0 ), action_list_default( 0 ), cooldown_list( 0 ), dot_list( 0 ),
  // Reporting
  quiet( 0 ), last_foreground_action( 0 ),
  iteration_fight_length( timespan_t::zero() ), arise_time( timespan_t::zero() ),
  fight_length( s -> statistics_level < 2, true ), waiting_time( true ), executed_foreground_actions( s -> statistics_level < 3 ),
  iteration_waiting_time( timespan_t::zero() ), iteration_executed_foreground_actions( 0 ),
  rps_gain( 0 ), rps_loss( 0 ),
  deaths( s -> statistics_level < 2 ), deaths_error( 0 ),
  buffed( buffed_stats_t() ),
  buff_list( 0 ), proc_list( 0 ), gain_list( 0 ), stats_list( 0 ), benefit_list( 0 ), uptime_list( 0 ),set_bonus_list( 0 ),
  // Damage
  iteration_dmg( 0 ), iteration_dmg_taken( 0 ),
  dps_error( 0 ), dpr( 0 ), dtps_error( 0 ),
  dmg( s -> statistics_level < 2 ), compound_dmg( s -> statistics_level < 2 ),
  dps( s -> statistics_level < 1 ), dpse( s -> statistics_level < 2 ),
  dtps( s -> statistics_level < 2 ), dmg_taken( s -> statistics_level < 2 ),
  // Heal
  iteration_heal( 0 ),iteration_heal_taken( 0 ),
  hps_error( 0 ), hpr( 0 ),
  heal( s -> statistics_level < 2 ), compound_heal( s -> statistics_level < 2 ),
  hps( s -> statistics_level < 1 ), hpse( s -> statistics_level < 2 ),
  htps( s -> statistics_level < 2 ), heal_taken( s -> statistics_level < 2 ),
  // Scaling
  scaling_lag( 0 ), scaling_lag_error( 0 ),
  // Movement & Position
  base_movement_speed( 7.0 ), x_position( 0.0 ), y_position( 0.0 ),
  buffs( buffs_t() ), debuffs( debuffs_t() ), gains( gains_t() ), rng_list( 0 ), rngs( rngs_t() ),
  set_bonus( set_bonuses_t() ), targetdata_id( s -> num_targetdata_ids++ )
{
  sim -> actor_list.push_back( this );

  if ( type != ENEMY && type != ENEMY_ADD )
  {
    if ( sim -> debug ) log_t::output( sim, "Creating Player %s", name() );
    player_t** last = &( sim -> player_list );
    while ( *last ) last = &( ( *last ) -> next );
    *last = this;
    next = 0;
    index = ++( sim -> num_players );
  }
  else
  {
    index = - ( ++( sim -> num_enemies ) );
  }

  race_str = util_t::race_type_string( race );

  if ( is_pet() ) skill = 1.0;

  boost::fill( attribute, 0 );
  boost::fill( attribute_base, 0 );
  boost::fill( attribute_initial, 0 );

  boost::fill( attribute_multiplier, 1 );
  boost::fill( attribute_multiplier_initial, 1 );

  boost::fill( infinite_resource, false );
  infinite_resource[ RESOURCE_HEALTH ] = true;

  boost::fill( resource_reduction, 0 );
  boost::fill( initial_resource_reduction, 0 );

  boost::fill( resource_reverse, false );
  resource_reverse[ RESOURCE_HEAT ] = true;

  boost::fill( resource_base, 0 );
  boost::fill( resource_initial, 0 );
  boost::fill( resource_max, 0 );
  boost::fill( resource_current, 0 );
  boost::fill( resource_lost, 0 );
  boost::fill( resource_gained, 0 );

  boost::fill( profession, 0 );

  boost::fill( scales_with, false );
  boost::fill( over_cap, 0 );

  items.resize( SLOT_MAX );
  for ( slot_type i = SLOT_MIN; i < SLOT_MAX; i++ )
  {
    items[ i ].slot = i;
    items[ i ].sim = sim;
    items[ i ].player = this;
  }

  main_hand_weapon.slot = SLOT_MAIN_HAND;
  off_hand_weapon.slot = SLOT_OFF_HAND;

  if ( ! sim -> active_files.empty() ) origin_str = sim -> active_files.top();

  boost::fill( talent_tab_points, 0 );
  boost::fill( tree_type, TREE_NONE );

  if ( reaction_stddev == timespan_t::zero() ) reaction_stddev = reaction_mean * 0.25;


  // inquisitors
  // ==== PvE sets
  set_bonus.underworld_force_masters = get_set_bonus( "underworld_force_masters", "kell_dragon_force_masters_/underworld_force_masters_/arkanian_force_masters_", "" );   // MrRobot ItemSetID = 52
  set_bonus.underworld_force_mystics = get_set_bonus( "underworld_force_mystics", "kell_dragon_force_mystics_/underworld_force_mystics_/arkanian_force_mystics_", "" );   // MrRobot ItemSetID = 30
  set_bonus.underworld_stalkers = get_set_bonus( "underworld_stalkers", "kell_dragon_stalkers_/underworld_stalkers_/arkanian_stalkers_", "" );                       // MrRobot ItemSetID = 35
  set_bonus.underworld_survivors = get_set_bonus( "underworld_survivors", "kell_dragon_survivors_/underworld_survivors_/arkanian_survivors_", "" );                   // MrRobot ItemSetID = 38
  set_bonus.rakata_force_masters = get_set_bonus( "rakata_force_masters", "tionese_force_masters_/columi_force_masters_/rakata_force_masters_/campaign_force_masters_/dread_guard_force_masters_", "" ); // MrRobot ItemSetID = 24
  set_bonus.rakata_force_mystics = get_set_bonus( "rakata_force_mystics", "tionese_force_mystics_/columi_force_mystics_/rakata_force_mystics_/campaign_force_mystics_/dread_guard_force_mystics_", "" ); // MrRobot ItemSetID = 2
  set_bonus.rakata_stalkers = get_set_bonus( "rakata_stalkers", "tionese_stalkers_/columi_stalkers_/rakata_stalkers_/campaign_stalkers_/dread_guard_stalkers_", "" );                                    // MrRobot ItemSetID = 7
  set_bonus.rakata_survivors = get_set_bonus( "unimplemented_rakata_survivors", "tionese_survivors_/columi_survivors_/rakata_survivors_/campaign_survivors_/dread_guard_survivors_", "" );               // MrRobot ItemSetID = 10
  // ==== PvP sets
  set_bonus.partisan_force_masters = get_set_bonus( "unimplemented_partisan_force_masters", "partisan_force_masters_/conqueror_force_masters_", "" );  // MrRobot ItemSetID = 31
  set_bonus.partisan_force_mystics = get_set_bonus( "unimplemented_partisan_force_mystics", "partisan_force_mystics_/conqueror_force_mystics_", "" );  // MrRobot ItemSetID = 48
  set_bonus.partisan_stalkers = get_set_bonus( "unimplemented_partisan_stalkers", "partisan_stalkers_/conqueror_stalkers", "" );                       // MrRobot ItemSetID = 49
  set_bonus.partisan_survivors = get_set_bonus( "unimplemented_partisan_survivors", "partisan_survivors_/conqueror_survivors", "" );                   // MrRobot ItemSetID = 29
  set_bonus.battlemaster_force_masters = get_set_bonus( "battlemaster_force_masters", "centurion_force_masters_/champion_force_masters_/battlemaster_force_masters_/warhero_force_masters_/elite_warhero_force_masters_", "" );               // MrRobot ItemSetID = 3
  set_bonus.battlemaster_force_mystics = get_set_bonus( "unimplemented_battlemaster_force_mystics", "centurion_force_mystics_/champion_force_mystics_/battlemaster_force_mystics_/warhero_force_mystics_/elite_warhero_force_mystics_", "" ); // MrRobot ItemSetID = 20
  set_bonus.battlemaster_stalkers = get_set_bonus( "battlemaster_stalkers", "centurion_stalkers_/champion_stalkers_/battlemaster_stalkers_/warhero_stalkers_/elite_warhero_stalkers_", "" );                                                  // MrRobot ItemSetID = 21
  set_bonus.battlemaster_survivors = get_set_bonus( "unimplemented_battlemaster_survivors", "centurion_survivors_/champion_survivors_/battlemaster_survivors_/warhero_survivors_/elite_warhero_survivors_", "" );                             // MrRobot ItemSetID = 1


  // agent/smuggler
  // ==== PvE sets
  set_bonus.underworld_enforcers = get_set_bonus( "underworld_enforcers", "kell_dragon_enforcers_/underworld_enforcers_/arkanian_enforcers_", "" );                   // MrRobot ItemSetID = 42
  set_bonus.underworld_field_medics = get_set_bonus( "underworld_field_medics", "kell_dragon_field_medics_/underworld_field_medics_/arkanian_field_medics_", "" );       // MrRobot ItemSetID = 32
  set_bonus.underworld_field_techs = get_set_bonus( "underworld_field_techs", "kell_dragon_field_techs_/underworld_field_techs_/arkanian_field_techs_/kell_dragon_professionals_/underworld_professionals_/arkanian_professionals_", "" ); // MrRobot ItemSetID = 45 (same as professionals)
  set_bonus.rakata_enforcers = get_set_bonus( "rakata_enforcers", "tionese_enforcers_/columi_enforcers_/rakata_enforcers_/campaign_enforcers_/dread_guard_enforcers_", "" );                      // MrRobot ItemSetID = 14
  set_bonus.rakata_field_medics = get_set_bonus( "rakata_field_medics", "tionese_field_medics_/columi_field_medics_/rakata_field_medics_/campaign_field_medics_/dread_guard_field_medics_", "" ); // MrRobot ItemSetID = 4
  set_bonus.rakata_field_techs = get_set_bonus( "rakata_field_techs", "tionese_field_techs_/columi_field_techs_/rakata_field_techs_/campaign_field_techs_/dread_guard_field_techs_", "" );        // MrRobot ItemSetID = 17
  // ==== PvP sets
  set_bonus.partisan_enforcers = get_set_bonus( "unimplemented_partisan_enforcers", "partisan_enforcers_/conqueror_enforcers_", "" );               // MrRobot ItemSetID = 36
  set_bonus.partisan_field_medics = get_set_bonus( "unimplemented_partisan_field_medics", "partisan_field_medics_/conqueror_field_medics_", "" );   // MrRobot ItemSetID = 53
  set_bonus.partisan_field_techs = get_set_bonus( "unimplemented_partisan_field_techs", "partisan_field_techs_/conqueror_field_techs/partisan_professionals_/conqueror_professionals", "" );  // MrRobot ItemSetID = 40
  set_bonus.battlemaster_enforcers = get_set_bonus( "battlemaster_enforcers", "centurion_enforcers_/champion_enforcers_/battlemaster_enforcers_/warhero_enforcers_/elite_warhero_enforcers_", "" );                                    // MrRobot ItemSetID = 8
  set_bonus.battlemaster_field_medics = get_set_bonus( "unimplemented_battlemaster_field_medics", "centurion_field_medics_/champion_field_medics_/battlemaster_field_medics_/warhero_field_medics_/elite_warhero_field_medics_", "" ); // MrRobot ItemSetID = 25
  set_bonus.battlemaster_field_techs = get_set_bonus( "battlemaster_field_techs", "centurion_field_techs_/champion_field_techs_/battlemaster_field_techs_/warhero_field_techs_/elite_warhero_field_techs_", "" );                      // MrRobot ItemSetID = 12


  // bountyhunter/trooper
  // ==== PvE sets
  set_bonus.underworld_eliminators = get_set_bonus( "underworld_eliminators", "kell_dragon_eliminators_/underworld_eliminators_/arkanian_eliminators_", "" );           // MrRobot ItemSetID = 50
  set_bonus.underworld_combat_medics = get_set_bonus( "underworld_combat_medics", "kell_dragon_combat_medics_/underworld_combat_medics_/arkanian_combat_medics_", "" );   // MrRobot ItemSetID = 46
  set_bonus.underworld_combat_techs = get_set_bonus( "underworld_combat_techs", "kell_dragon_combat_techs_/underworld_combat_techs_/arkanian_combat_techs_", "" );       // MrRobot ItemSetID = 47
  set_bonus.underworld_supercomandos = get_set_bonus( "underworld_supercomandos", "kell_dragon_supercomandos_/underworld_supercomandos_/arkanian_supercomandos_", "" );   // MrRobot ItemSetID = 44
  set_bonus.rakata_eliminators = get_set_bonus( "rakata_eliminators", "tionese_eliminators_/columi_eliminators_/rakata_eliminators_/campaign_eliminators_/dread_guard_eliminators_", "" );                              // MrRobot ItemSetID = 22
  set_bonus.rakata_combat_medics = get_set_bonus( "unimplemented_rakata_combat_medics", "tionese_combat_medics_/columi_combat_medics_/rakata_combat_medics_/campaign_combat_medics_/dread_guard_combat_medics_", ""  ); // MrRobot ItemSetID = 18
  set_bonus.rakata_combat_techs = get_set_bonus( "unimplemented_rakata_combat_techs", "tionese_combat_techs_/columi_combat_techs_/rakata_combat_techs_/campaign_combat_techs_/dread_guard_combat_techs_", ""  );        // MrRobot ItemSetID = 19
  set_bonus.rakata_supercomandos = get_set_bonus( "unimplemented_rakata_supercomandos", "tionese_supercomandos_/columi_supercomandos_/rakata_supercomandos_/campaign_supercomandos_/dread_guard_supercomandos_", ""  ); // MrRobot ItemSetID = 16
  // ==== PvP sets
  set_bonus.partisan_eliminators = get_set_bonus( "unimplemented_partisan_eliminators", "partisan_eliminators_/conqueror_eliminators_", "" );          // MrRobot ItemSetID = 56
  set_bonus.partisan_combat_medics = get_set_bonus( "unimplemented_partisan_combat_medics", "partisan_combat_medics_/conqueror_combat_medics_", "" );  // MrRobot ItemSetID = 37
  set_bonus.partisan_combat_techs = get_set_bonus( "unimplemented_partisan_combat_techs", "partisan_combat_techs_/conqueror_combat_techs", "" );       // MrRobot ItemSetID = 39
  set_bonus.partisan_supercomandos = get_set_bonus( "unimplemented_partisan_supercomandos", "partisan_supercomandos_/conqueror_supercomandos", "" );   // MrRobot ItemSetID = 55
  set_bonus.battlemaster_eliminators = get_set_bonus( "battlemaster_eliminators", "centurion_eliminators_/champion_eliminators_/battlemaster_eliminators_/warhero_eliminators_/elite_warhero_eliminators_", "" );                             // MrRobot ItemSetID = 28
  set_bonus.battlemaster_combat_medics = get_set_bonus( "unimplemented_battlemaster_combat_medics", "centurion_combat_medics_/champion_combat_medics_/battlemaster_combat_medics_/warhero_combat_medics_/elite_warhero_combat_medics_", "" ); // MrRobot ItemSetID = 9
  set_bonus.battlemaster_combat_techs = get_set_bonus( "unimplemented_battlemaster_combat_techs", "centurion_combat_techs_/champion_combat_techs_/battlemaster_combat_techs_/warhero_combat_techs_/elite_warhero_combat_techs_", "" );        // MrRobot ItemSetID = 11
  set_bonus.battlemaster_supercomandos = get_set_bonus( "unimplemented_battlemaster_supercomandos", "centurion_supercomandos_/champion_supercomandos_/battlemaster_supercomandos_/warhero_supercomandos_/elite_warhero_supercomandos_", "" ); // MrRobot ItemSetID = 27


  // warrior/knight
  // ==== PvE sets
  set_bonus.underworld_weaponmasters = get_set_bonus( "underworld_weaponmasters", "kell_dragon_weaponmasters_/underworld_weaponmasters_/arkanian_weaponmasters_/kell_dragon_challengers_/underworld_challengers_/arkanian_challengers_", "" );  // MrRobot ItemSetID = 33  (Same as challengers)
  set_bonus.underworld_vindicators = get_set_bonus( "underworld_vindicators", "kell_dragon_vindicators_/underworld_vindicators_/arkanian_vindicators_", "" );          // MrRobot ItemSetID = 43
  set_bonus.underworld_war_leaders = get_set_bonus( "underworld_war_leaders", "kell_dragon_war_leaders_/underworld_war_leaders_/arkanian_war_leaders_", "" );          // MrRobot ItemSetID = 41
  set_bonus.rakata_weaponmasters = get_set_bonus( "unimplemented_rakata_weaponmasterss", "tionese_weaponmasters_/columi_weaponmasters_/rakata_weaponmasters_/campaign_weaponmasters_/dread_guard_weaponmasters_", "" ); // MrRobot ItemSetID = 5
  set_bonus.rakata_vindicators = get_set_bonus( "unimplemented_rakata_vindicators", "tionese_vindicators_/columi_vindicators_/rakata_vindicators_/campaign_vindicators_/dread_guard_vindicators_", "" );                // MrRobot ItemSetID = 15
  set_bonus.rakata_war_leaders = get_set_bonus( "unimplemented_rakata_war_leaders", "tionese_war_leaders_/columi_war_leaders_/rakata_war_leaders_/campaign_war_leaders_/dread_guard_war_leaders_", "" );                // MrRobot ItemSetID = 13
  // ==== PvP sets
  set_bonus.partisan_weaponmasters = get_set_bonus( "unimplemented_partisan_weaponmasters", "partisan_weaponmasters_/conqueror_weaponmasters_/partisan_challengers_/conqueror_challengers_", "" ); // MrRobot ItemSetID = 34
  set_bonus.partisan_vindicators = get_set_bonus( "unimplemented_partisan_vindicators", "partisan_vindicators_/conqueror_vindicators_", "" );         // MrRobot ItemSetID = 54
  set_bonus.partisan_war_leaders = get_set_bonus( "unimplemented_partisan_war_leaders", "partisan_war_leaders_/conqueror_war_leaders", "" );          // MrRobot ItemSetID = 51
  set_bonus.battlemaster_weaponmasters = get_set_bonus( "unimplemented_battlemaster_weaponmasters", "centurion_weaponmasters_/champion_weaponmasters_/battlemaster_weaponmasters_/warhero_weaponmasters_/elite_warhero_weaponmasters_", "" ); // MrRobot ItemSetID = 6
  set_bonus.battlemaster_vindicators = get_set_bonus( "unimplemented_battlemaster_vindicators", "centurion_vindicators_/champion_vindicators_/battlemaster_vindicators_/warhero_vindicators_/elite_warhero_vindicators_", "" );               // MrRobot ItemSetID = 26
  set_bonus.battlemaster_war_leaders = get_set_bonus( "unimplemented_battlemaster_war_leaders", "centurion_war_leaders_/champion_war_leaders_/battlemaster_war_leaders_/warhero_war_leaders_/elite_warhero_war_leaders_", "" );               // MrRobot ItemSetID = 23
}

// player_t::~player_t ======================================================

player_t::~player_t()
{
  list_dispose( action_list );
  list_dispose( proc_list );
  list_dispose( gain_list );
  list_dispose( stats_list );
  list_dispose( uptime_list );
  list_dispose( benefit_list );
  list_dispose( rng_list );
  list_dispose( dot_list );
  list_dispose( buff_list );
  list_dispose( cooldown_list );

  // NOTE: Must defend against multiple free()ing, since the same callback
  //       pointer is often registered multiple times.
  assert( boost::unique<boost::return_found>( boost::sort( all_callbacks ) ) == all_callbacks.end() );
}

// player_t::init_talent_tree ===============================================

void player_t::init_talent_tree( unsigned tree_index,
                                 const talentinfo_t* first,
                                 const talentinfo_t* last )
{
  talent_trees[ tree_index ].clear();
  for( ; first != last; ++first )
  {
    talent_t* t = new talent_t( this, first -> name, tree_index, first -> points );
    talent_trees[ tree_index ].push_back( t );
  }
}

// player_t::init ===========================================================

bool player_t::init( sim_t* sim )
{
  // ========================================================================
  {
    if ( sim -> debug )
      log_t::output( sim, "Creating Pets." );

    for ( size_t i = 0; i < sim -> actor_list.size(); i++ )
    {
      player_t* p = sim -> actor_list[i];
      p -> create_pets();
    }
  }

  // ========================================================================
  {
    if ( sim -> debug )
      log_t::output( sim, "Initializing Targetdata." );

    for ( player_t* p : sim -> actor_list )
    {
      p -> targetdata.resize( sim -> num_targetdata_ids );
      p -> sourcedata.resize( sim -> num_targetdata_ids );
    }

    if ( ! LAZY_TARGETDATA_INIT )
    {
      for ( player_t* p : sim -> actor_list )
      {
        assert( p );
        for ( player_t* q : sim -> actor_list )
        {
          assert( q );
          targetdata_t* td = p -> new_targetdata( *q );
          p -> targetdata[ q -> targetdata_id ] = td;
          q -> sourcedata[ p -> targetdata_id ] = td;
        }
      }
    }
  }

  // ========================================================================
  if ( sim -> debug )
    log_t::output( sim, "Initializing Auras, Buffs, and De-Buffs." );

  player_t::commando_mercenary_init( sim );
  player_t::gunslinger_sniper_init( sim );
  player_t::juggernaut_guardian_init( sim );
  player_t::sage_sorcerer_init( sim );
  player_t::scoundrel_operative_init( sim );
  player_t::sentinel_marauder_init( sim );
  player_t::shadow_assassin_init( sim );
  player_t::vanguard_powertech_init( sim );
  player_t::enemy_init( sim );

  // ========================================================================
  if ( sim -> debug )
    log_t::output( sim, "Initializing Players." );

  bool too_quiet = true; // Check for at least 1 active player
  bool zero_dds = true; // Check for at least 1 player != TANK/HEAL

  for ( size_t i = 0; i < sim -> actor_list.size(); i++ )
  {
    player_t* p = sim -> actor_list[i];
    if ( sim -> default_actions && ! p -> is_pet() ) p -> action_list_str.clear();
    p -> init();
    if ( ! p -> quiet ) too_quiet = false;
    if ( p -> primary_role() != ROLE_HEAL && p -> primary_role() != ROLE_TANK && ! p -> is_pet() ) zero_dds = false;
  }

  // ========================================================================
  if ( too_quiet && ! sim -> debug )
  {
    sim -> errorf( "No active players in sim!" );
    return false;
  }

  // Set Fixed_Time when there are no DD's present
  if ( zero_dds && ! sim -> debug )
    sim -> fixed_time = true;


  // ========================================================================
  if ( sim -> debug )
    log_t::output( sim, "Building Parties." );

  int party_index=0;
  for ( size_t i=0; i < sim -> party_encoding.size(); i++ )
  {
    std::string& party_str = sim -> party_encoding[ i ];

    if ( party_str == "reset" )
    {
      party_index = 0;
      for ( player_t* p = sim -> player_list; p; p = p -> next ) p -> party = 0;
    }
    else if ( party_str == "all" )
    {
      int member_index = 0;
      for ( player_t* p = sim -> player_list; p; p = p -> next )
      {
        p -> party = 1;
        p -> member = member_index++;
      }
    }
    else
    {
      party_index++;

      std::vector<std::string> player_names;
      int num_players = util_t::string_split( player_names, party_str, ",;/" );
      int member_index=0;

      for ( int j=0; j < num_players; j++ )
      {
        player_t* p = sim -> find_player( player_names[ j ] );
        if ( ! p )
        {
          sim -> errorf( "Unable to find player %s for party creation.\n", player_names[ j ].c_str() );
          return false;
        }
        p -> party = party_index;
        p -> member = member_index++;
        for ( pet_t* pet = p -> pet_list; pet; pet = pet -> next_pet )
        {
          pet -> party = party_index;
          pet -> member = member_index++;
        }
      }
    }
  }


  // ========================================================================
  if ( sim -> debug )
    log_t::output( sim, "Registering Callbacks." );

  for ( size_t i = 0; i < sim -> actor_list.size(); i++ )
  {
    player_t* p = sim -> actor_list[i];
    p -> register_callbacks();
  }

  return true;
}

// player_t::init ===========================================================

void player_t::init()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing player %s", name() );

  initialized = 1;
  rating_scaler.init( level );

  init_target();
  init_items();
  init_weapon( &main_hand_weapon );
  init_weapon( &off_hand_weapon );
  init_talents();
  init_spells();
  init_race();
  init_base();
  init_racials();
  init_position();
  init_professions();
  init_core();
  init_spell();
  init_attack();
  init_defense();
  init_professions_bonus();
  init_unique_gear();
  init_enchant();
  init_scaling();
  init_buffs();
  init_values();
  init_actions();
  init_gains();
  init_procs();
  init_cooldowns();
  init_uptimes();
  init_benefits();
  init_rng();
  init_stats();
}

// player_t::init_base ======================================================

void player_t::init_base()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing base for player (%s)", name() );

  attribute_base[ ATTR_STRENGTH    ] =
    attribute_base[ ATTR_AIM       ] =
    attribute_base[ ATTR_CUNNING   ] =
    attribute_base[ ATTR_WILLPOWER ] = floor( 10 + 0.8 * level );
  attribute_base[ ATTR_ENDURANCE   ] =
    attribute_base[ ATTR_PRESENCE  ] = floor( 45 + 3.6 * level );

  assert( primary_attribute != secondary_attribute );

  if(level <= 50)
      attribute_base[ primary_attribute   ] = floor( 50 + 4 * level );
  else
      attribute_base[ primary_attribute   ] = floor( 250 + 30 * (level-50));
  
  //FIXME:  This is most likely wrong for the expansion, but not hugely important
  attribute_base[ secondary_attribute ] = floor( 20 + 1.6 * level );

  resource_base[ RESOURCE_HEALTH ] = rating_t::get_base_health( level );

  /*
  attribute_base[ ATTR_STRENGTH  ] = rating_t::get_attribute_base( sim, dbc, level, type, race, BASE_STAT_STRENGTH );
  attribute_base[ ATTR_AGILITY   ] = rating_t::get_attribute_base( sim, dbc, level, type, race, BASE_STAT_AGILITY );
  attribute_base[ ATTR_STAMINA   ] = rating_t::get_attribute_base( sim, dbc, level, type, race, BASE_STAT_STAMINA );
  attribute_base[ ATTR_INTELLECT ] = rating_t::get_attribute_base( sim, dbc, level, type, race, BASE_STAT_INTELLECT );
  attribute_base[ ATTR_SPIRIT    ] = rating_t::get_attribute_base( sim, dbc, level, type, race, BASE_STAT_SPIRIT );
  resource_base[ RESOURCE_HEALTH ] = rating_t::get_attribute_base( sim, dbc, level, type, race, BASE_STAT_HEALTH );
  resource_base[ RESOURCE_MANA   ] = rating_t::get_attribute_base( sim, dbc, level, type, race, BASE_STAT_MANA );
  initial_spell_crit_per_intellect = rating_t::get_attribute_base( sim, dbc, level, type, race, BASE_STAT_SPELL_CRIT_PER_INT );
  initial_attack_crit_per_agility  = rating_t::get_attribute_base( sim, dbc, level, type, race, BASE_STAT_MELEE_CRIT_PER_AGI );
  base_mp5                         = rating_t::get_attribute_base( sim, dbc, level, type, race, BASE_STAT_MP5 );
*/

  if ( world_lag_stddev < timespan_t::zero() ) world_lag_stddev = world_lag * 0.1;
  if ( brain_lag_stddev < timespan_t::zero() ) brain_lag_stddev = brain_lag * 0.1;
}

// player_t::init_items =====================================================

void player_t::init_items()
{
  if ( is_pet() ) return;

  if ( sim -> debug ) log_t::output( sim, "Initializing items for player (%s)", name() );

  for ( auto& s : split( items_str, '/' ) )
  {
    if ( find_item( s ) )
      sim -> errorf( "Player %s has multiple %s equipped.\n", name(), s.c_str() );

    items.push_back( item_t( this, std::move( s ) ) );
  }

  gear_stats_t item_stats;
  int num_ilvl_items = 0;
  avg_ilvl = 0;

  for ( size_t i = 0, num_items = items.size(); i < num_items; ++i )
  {
    item_t& item = items[ i ];

    // If the item has been specified in options we want to start from scratch,
    // forgetting about lingering stuff from profile copy
    if ( ! item.options_str.empty() )
    {
      item = item_t( this, item.options_str );
      item.slot = static_cast<slot_type>( i );
    }

    if ( ! item.init() )
    {
      sim -> errorf( "Unable to initialize item '%s' on player '%s'\n", item.name(), name() );
      sim -> cancel();
      return;
    }

    if ( item.slot != SLOT_SHIRT && item.slot != SLOT_TABARD && item.active() )
    {
      avg_ilvl += item.ilevel;
      ++num_ilvl_items;
    }

    for ( stat_type j = STAT_NONE; j < STAT_MAX; ++j )
      item_stats.add_stat( j, item.stats.get_stat( j ) );
  }

  if ( num_ilvl_items > 1 )
    avg_ilvl /= num_ilvl_items;

  for ( stat_type i = STAT_NONE; i < STAT_MAX; ++i )
    gear.add_stat( i, item_stats.get_stat( i ) );

  if ( sim -> debug )
  {
    log_t::output( sim, "%s gear:", name() );
    gear.print( sim -> output_file );
  }

  for ( set_bonus_t* sb = set_bonus_list; sb; sb = sb -> next )
    sb -> init( *this );
}

// player_t::init_core ======================================================

void player_t::init_core()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing core for player (%s)", name() );

  initial_stats.accuracy_rating = gear.accuracy_rating  + enchant.accuracy_rating   + ( is_pet() ? 0 : sim -> enchant.accuracy_rating );
  initial_stats.crit_rating     = gear.crit_rating      + enchant.crit_rating       + ( is_pet() ? 0 : sim -> enchant.crit_rating );
  initial_stats.alacrity_rating = gear.alacrity_rating  + enchant.alacrity_rating   + ( is_pet() ? 0 : sim -> enchant.alacrity_rating );
  initial_stats.surge_rating    = gear.surge_rating     + enchant.surge_rating      + ( is_pet() ? 0 : sim -> enchant.surge_rating );

  initial_alacrity_rating = initial_stats.alacrity_rating;
  initial_crit_rating     = initial_stats.crit_rating;
  initial_accuracy_rating = initial_stats.accuracy_rating;
  initial_surge_rating    = initial_stats.surge_rating;

  for ( int i=0; i < ATTRIBUTE_MAX; i++ )
  {
    initial_stats.attribute[ i ] = gear.attribute[ i ] + enchant.attribute[ i ] + ( is_pet() ? 0 : sim -> enchant.attribute[ i ] );

    attribute[ i ] = attribute_initial[ i ] = attribute_base[ i ] + initial_stats.attribute[ i ];
  }
}

// player_t::init_position ==================================================

void player_t::init_position()
{
  if ( sim -> debug )
    log_t::output( sim, "Initializing position for player (%s)", name() );

  if ( ! position_str.empty() )
    position = util_t::parse_position_type( position_str );

  // default to back when we have an invalid position
  if ( position == POSITION_NONE )
  {
    sim -> errorf( "Player %s has an invalid position of '%s', defaulting to back.\n",
                   name(), position_str.c_str() );
    position = POSITION_BACK;
  }

  position_str = util_t::position_type_string( position );
}

// player_t::init_race ======================================================

void player_t::init_race()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing race for player (%s)", name() );

  if ( race_str.empty() )
  {
    race_str = util_t::race_type_string( race );
  }
  else
  {
    race = util_t::parse_race_type( race_str );
  }
}

// player_t::init_racials ===================================================

void player_t::init_racials()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing racials for player (%s)", name() );

}

// player_t::init_spell =====================================================

void player_t::init_spell()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing spells for player (%s)", name() );

  initial_stats.power       = gear.power       + enchant.power       + ( is_pet() ? 0 : sim -> enchant.power );
  initial_stats.force_power = gear.force_power + enchant.force_power + ( is_pet() ? 0 : sim -> enchant.force_power );
  initial_stats.tech_power  = gear.tech_power  + enchant.tech_power  + ( is_pet() ? 0 : sim -> enchant.tech_power );

  initial_power       = base_power       + initial_stats.power;
  initial_force_power = base_force_power + initial_stats.force_power;
  initial_tech_power =  base_tech_power  + initial_stats.tech_power;
}

// player_t::init_attack ====================================================

void player_t::init_attack()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing attack for player (%s)", name() );

  initial_stats.expertise_rating   = gear.expertise_rating + enchant.expertise_rating   + ( is_pet() ? 0 : sim -> enchant.expertise_rating );
  initial_stats.weapon_dmg         =                         enchant.weapon_dmg         + ( is_pet() ? 0 : sim -> enchant.weapon_dmg );
  initial_stats.weapon_offhand_dmg =                         enchant.weapon_offhand_dmg + ( is_pet() ? 0 : sim -> enchant.weapon_offhand_dmg );
}

// player_t::init_defense ===================================================

void player_t::init_defense()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing defense for player (%s)", name() );

  base_melee_avoidance = base_range_avoidance = .05;
  base_force_avoidance = base_tech_avoidance = 0;

  initial_stats.armor          = gear.armor          + enchant.armor          + ( is_pet() ? 0 : sim -> enchant.armor );
  initial_stats.bonus_armor    = gear.bonus_armor    + enchant.bonus_armor    + ( is_pet() ? 0 : sim -> enchant.bonus_armor );
  initial_stats.defense_rating = gear.defense_rating + enchant.defense_rating + ( is_pet() ? 0 : sim -> enchant.defense_rating );
  initial_stats.shield_rating  = gear.shield_rating  + enchant.shield_rating  + ( is_pet() ? 0 : sim -> enchant.shield_rating );
  initial_stats.absorb_rating  = gear.absorb_rating  + enchant.absorb_rating  + ( is_pet() ? 0 : sim -> enchant.absorb_rating );

  initial_armor             = base_armor       + initial_stats.armor;
  initial_bonus_armor       = base_bonus_armor + initial_stats.bonus_armor;
  initial_shield_rating     = initial_stats.shield_rating;
  initial_absorb_rating     = initial_stats.absorb_rating;

  initial_shield_chance     = base_shield_chance + rating_scaler.absorb( initial_absorb_rating );

  initial_melee_avoidance   = base_melee_avoidance + rating_scaler.defense( defense_rating );
  initial_range_avoidance   = base_range_avoidance + rating_scaler.defense( defense_rating );
  initial_force_avoidance   = base_force_avoidance;
  initial_tech_avoidance    = base_tech_avoidance;

  if ( primary_role() == ROLE_TANK ) position = POSITION_FRONT;
}

// player_t::init_weapon ====================================================

void player_t::init_weapon( weapon_t* )
{}

// player_t::init_unique_gear ===============================================

void player_t::init_unique_gear()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing unique gear for player (%s)", name() );
  unique_gear_t::init( this );
}

// player_t::init_enchant ===================================================

void player_t::init_enchant()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing enchants for player (%s)", name() );
  enchant_t::init( this );
}

// player_t::init_resources =================================================

void player_t::init_resources( bool force )
{
  if ( sim -> debug ) log_t::output( sim, "Initializing resources for player (%s)", name() );

  for ( resource_type i = RESOURCE_NONE; i < RESOURCE_MAX; i++ )
  {
    if ( force || resource_max[ i ] == 0)
    {
      resource_max[ i ] = resource_base[ i ] + gear.resource[ i ] + enchant.resource[ i ] + ( is_pet() ? 0 : sim -> enchant.resource[ i ] );

      if ( i == RESOURCE_HEALTH )
      {
        resource_max[ i ] += floor( endurance() ) * health_per_endurance;
      }
    }
    resource_current[ i ] = resource_initial[ i ] = resource_reverse[ i ] ? 0 : resource_max[ i ];
  }

  if ( timeline_resource[ 0 ].empty() )
  {
    int size = to_seconds<int>( sim -> max_time * ( 1.0 + sim -> vary_combat_length ) );
    if ( size <= 0 ) size = 600; // Default to 10 minutes
    size *= 2;
    size += 3; // Buffer against rounding.

    for ( auto& i : timeline_resource )
      i.assign( size, 0 );
  }
}

// player_t::init_professions ===============================================

void player_t::init_professions()
{
  if ( professions_str.empty() ) return;

  if ( sim -> debug ) log_t::output( sim, "Initializing professions for player (%s)", name() );

  std::vector<std::string> splits;
  int size = util_t::string_split( splits, professions_str, ",/" );

  for ( int i=0; i < size; i++ )
  {
    std::string prof_name;
    int prof_value=0;

    if ( 2 != util_t::string_split( splits[ i ], "=", "S i", &prof_name, &prof_value ) )
    {
      prof_name  = splits[ i ];
      prof_value = 525;
    }

    profession_type prof_type = util_t::parse_profession_type( prof_name );

    if ( prof_type == PROFESSION_NONE )
      continue;

    if ( prof_type == PROFESSION_UNKNOWN )
    {
      sim -> errorf( "Invalid profession encoding: %s\n", prof_name.c_str() );
      continue;
    }

    profession[ prof_type ] = prof_value;
  }
}

// player_t::init_professions_bonus =========================================

void player_t::init_professions_bonus()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing professions bonuses for player (%s)", name() );

  // This has to be called after init_attack() and init_core()

}

// Execute Pet Action =======================================================

struct execute_pet_action_t : public action_t
{
  action_t* pet_action;
  pet_t* pet;
  std::string action_str;

  execute_pet_action_t( player_t* player, pet_t* p, const std::string& as, const std::string& options_str ) :
    action_t( ACTION_OTHER, "execute_pet_action", player ), pet_action( 0 ), pet( p ), action_str( as )
  {
    parse_options( NULL, options_str );
    trigger_gcd = timespan_t::zero();
  }

  virtual void reset()
  {
    for ( action_t* action = pet -> action_list; action; action = action -> next )
    {
      if ( action -> name_str == action_str )
      {
        action -> background = true;
        pet_action = action;
      }
    }
  }

  virtual void execute()
  {
    pet_action -> execute();
  }

  virtual bool ready()
  {
    if ( ! action_t::ready() ) return false;
    if ( pet_action -> player -> sleeping ) return false;
    return pet_action -> ready();
  }
};

// player_t::init_target ====================================================

void player_t::init_target()
{
  if ( ! target_str.empty() )
  {
    target = sim -> find_player( target_str );
  }
  if ( ! target )
  {
    target = sim -> target;
  }
}

// player_t::init_use_item_actions ==========================================

std::string player_t::init_use_item_actions( const std::string& append )
{
  std::string buffer;
  int num_items = ( int ) items.size();
  for ( int i=0; i < num_items; i++ )
  {
    if ( items[ i ].use.active() )
    {
      buffer += "/use_item,name=";
      buffer += items[ i ].name();
      if ( ! append.empty() )
      {
        buffer += append;
      }
    }
  }

  return buffer;
}

// player_t::init_use_profession_actions ====================================

std::string player_t::init_use_profession_actions( const std::string& /* append */ )
{
  std::string buffer;
  // Lifeblood

  return buffer;
}

// player_t::init_use_racial_actions ========================================

std::string player_t::init_use_racial_actions( const std::string& append )
{
  std::string buffer;
  bool race_action_found = false;


  if ( race_action_found && ! append.empty() )
  {
    buffer += append;
  }

  return buffer;
}

// player_t::init_actions ===================================================

void player_t::init_actions()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing actions for player (%s)", name() );

  if ( ! choose_action_list.empty() )
  {
    action_priority_list_t* chosen_action_list = find_action_priority_list( choose_action_list );

    if ( chosen_action_list )
      action_list_str = chosen_action_list -> action_list_str;
    else if ( choose_action_list != "default" )
    {
      sim -> errorf( "Action List %s not found, using default action list.\n", choose_action_list.c_str() );
      action_priority_list_t* default_action_list = find_action_priority_list( "default" );
      if ( default_action_list )
        action_list_str = default_action_list -> action_list_str;
      else if ( action_list_str.empty() )
        sim -> errorf( "No Default Action List available.\n" );
    }
  }
  else if ( action_list_str.empty() )
  {
    action_priority_list_t* chosen_action_list = find_action_priority_list( "default" );
    if ( chosen_action_list )
      action_list_str = chosen_action_list -> action_list_str;
  }

  if ( ! action_list_str.empty() )
  {
    if ( action_list_default && sim -> debug ) log_t::output( sim, "Player %s using default actions", name() );

    if ( sim -> debug ) log_t::output( sim, "Player %s: action_list_str=%s", name(), action_list_str.c_str() );

    std::string modify_action_options = "";

    if ( ! modify_action.empty() )
    {
      std::string::size_type cut_pt = modify_action.find( "," );

      if ( cut_pt != modify_action.npos )
      {
        modify_action_options = modify_action.substr( cut_pt + 1 );
        modify_action         = modify_action.substr( 0, cut_pt );
      }
    }

    std::vector<std::string> splits;
    int num_splits = util_t::string_split( splits, action_list_str, "/" );

    for ( int i=0; i < num_splits; i++ )
    {
      std::string action_name    = splits[ i ];
      std::string action_options = "";

      std::string::size_type cut_pt = action_name.find( "," );

      if ( cut_pt != action_name.npos )
      {
        action_options = action_name.substr( cut_pt + 1 );
        action_name    = action_name.substr( 0, cut_pt );
      }

      action_t* a=0;

      cut_pt = action_name.find( ":" );
      if ( cut_pt != action_name.npos )
      {
        std::string pet_name   = action_name.substr( 0, cut_pt );
        std::string pet_action = action_name.substr( cut_pt + 1 );

        pet_t* pet = find_pet( pet_name );
        if ( pet )
        {
          a =  new execute_pet_action_t( this, pet, pet_action, action_options );
        }
        else
        {
          sim -> errorf( "Player %s refers to unknown pet %s in action: %s\n",
                         name(), pet_name.c_str(), splits[ i ].c_str() );
        }
      }
      else
      {
        if ( action_name == modify_action )
        {
          if ( sim -> debug )
            log_t::output( sim, "Player %s: modify_action=%s", name(), modify_action.c_str() );

          action_options = modify_action_options;
          splits[ i ] = modify_action + "," + modify_action_options;
        }
        a = create_action( action_name, action_options );
      }

      if ( a )
      {
        a -> marker = ( char ) ( ( i < 10 ) ? ( '0' + i      ) :
                                 ( i < 36 ) ? ( 'A' + i - 10 ) :
                                 ( i < 58 ) ? ( 'a' + i - 36 ) : '.' );

        a -> signature_str = splits[ i ];

        if (  sim -> separate_stats_by_actions > 0 && !is_pet() )
        {
          a -> stats = get_stats( a -> name_str + "__" + a -> marker, a );
        }
      }
      else
      {
        sim -> errorf( "Player %s unable to create action: %s\n", name(), splits[ i ].c_str() );
        sim -> cancel();
        return;
      }
    }
  }

  if ( ! action_list_skip.empty() )
  {
    if ( sim -> debug )
      log_t::output( sim, "Player %s: action_list_skip=%s", name(), action_list_skip.c_str() );

    std::vector<std::string> splits;
    int num_splits = util_t::string_split( splits, action_list_skip, "/" );
    for ( int i=0; i < num_splits; i++ )
    {
      action_t* action = find_action( splits[ i ] );
      if ( action ) action -> background = true;
    }
  }

  for ( action_t* action = action_list; action; action = action -> next )
  {
    action -> init();
    if ( action -> trigger_gcd == timespan_t::zero() &&
         ! action -> background && action -> use_off_gcd )
      off_gcd_actions.push_back( action );

  }

  int capacity = std::max( 1200, ( int ) to_seconds( sim -> max_time ) / 2 );
  action_sequence.clear();
  action_sequence.reserve( capacity );
}

// player_t::init_talents ===================================================

void player_t::init_talents()
{
  for ( int i=0; i < MAX_TALENT_TREES; i++ )
  {
    talent_tab_points[ i ] = 0;

    size_t size=talent_trees[ i ].size();
    for ( size_t j=0; j < size; j++ )
    {
      talent_tab_points[ i ] += talent_trees[ i ][ j ] -> rank();
    }
  }

  specialization = primary_tab();
}

// player_t::init_spells ====================================================

void player_t::init_spells()
{}

// player_t::init_buffs =====================================================

void player_t::init_buffs()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing buffs for player (%s)", name() );

  buffs.power_potion        = new stat_buff_t( this, "power_potion", STAT_POWER, 0, 1, from_seconds( 15.0 ), from_seconds( 180.0 ) );

  buffs.coordination        = new buff_t( this, "coordination",     1 );
  buffs.coordination -> constant = true;

  buffs.unnatural_might     = new buff_t( this, "unnatural_might",  1 );
  buffs.unnatural_might -> constant = true;

  buffs.raid_movement       = new buff_t( this, "raid_movement",    1 );
  buffs.self_movement       = new buff_t( this, "self_movement",    1 );
  buffs.stunned             = new buff_t( this, "stunned",         -1 );
  debuffs.bleeding          = new debuff_t( this, "bleeding",     -1 );
  debuffs.invulnerable      = new debuff_t( this, "invulnerable", -1 );
  debuffs.vulnerable        = new debuff_t( this, "vulnerable",   -1 );

  debuffs.shatter_shot      = new debuff_t( this, "shatter_shot:1", 1, from_seconds( 45.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.shatter_shot_2      = new debuff_t( this, "shatter_shot:2", 1, from_seconds( 45.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.shatter_shot_3      = new debuff_t( this, "shatter_shot:3", 1, from_seconds( 45.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.shatter_shot_4      = new debuff_t( this, "shatter_shot:4", 1, from_seconds( 45.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.shatter_shot_5      = new debuff_t( this, "shatter_shot:5", 1, from_seconds( 45.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.sunder            = new debuff_t( this, "sunder:1", 5, from_seconds( 15.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.sunder_2            = new debuff_t( this, "sunder:2", 5, from_seconds( 15.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.sunder_3            = new debuff_t( this, "sunder:3", 5, from_seconds( 15.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.sunder_4            = new debuff_t( this, "sunder:4", 5, from_seconds( 15.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.sunder_5            = new debuff_t( this, "sunder:5", 5, from_seconds( 15.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.heat_signature    = new debuff_t( this, "heat_signature:1", 5, from_seconds( 15.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.heat_signature_2    = new debuff_t( this, "heat_signature:2", 5, from_seconds( 15.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.heat_signature_3    = new debuff_t( this, "heat_signature:3", 5, from_seconds( 15.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.heat_signature_4    = new debuff_t( this, "heat_signature:4", 5, from_seconds( 15.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.heat_signature_5    = new debuff_t( this, "heat_signature:5", 5, from_seconds( 15.0 ) ); // TODO: move to player_t extension of correct class
  debuffs.flying            = new debuff_t( this, "flying",   -1 );
}

// player_t::init_gains =====================================================

void player_t::init_gains()
{
  // XXX why is this here
  gains.ammo_regen   = get_gain( "ammo_regen" );
}

// player_t::init_procs =====================================================

void player_t::init_procs()
{}

// player_t::init_uptimes ===================================================

void player_t::init_uptimes()
{
  primary_resource_cap = get_uptime( util_t::resource_type_string( primary_resource() ) + ( std::string ) "_cap" );
}

// player_t::init_benefits ===================================================

void player_t::init_benefits()
{ }

// player_t::init_cooldowns  =================================================

void player_t::init_cooldowns()
{ }

// player_t::init_rng =======================================================

void player_t::init_rng()
{
  rngs.lag_channel  = get_rng( "lag_channel"  );
  rngs.lag_ability  = get_rng( "lag_ability"  );
  rngs.lag_brain    = get_rng( "lag_brain"    );
  rngs.lag_gcd      = get_rng( "lag_gcd"      );
  rngs.lag_queue    = get_rng( "lag_queue"    );
  rngs.lag_reaction = get_rng( "lag_reaction" );
  rngs.lag_world    = get_rng( "lag_world"    );
}

// player_t::init_stats =====================================================

void player_t::init_stats()
{
  boost::fill( resource_lost, 0 );
  boost::fill( resource_gained, 0 );

  fight_length.reserve( sim -> iterations );
  waiting_time.reserve( sim -> iterations );
  executed_foreground_actions.reserve( sim -> iterations );

  dmg.reserve( sim -> iterations );
  compound_dmg.reserve( sim -> iterations );
  dps.reserve( sim -> iterations );
  dpse.reserve( sim -> iterations );
  dtps.reserve( sim -> iterations );

  heal.reserve( sim -> iterations );
  compound_heal.reserve( sim -> iterations );
  hps.reserve( sim -> iterations );
  hpse.reserve( sim -> iterations );
  htps.reserve( sim -> iterations );
}

// player_t::init_values ====================================================

void player_t::init_values()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing values for player (%s)", name() );
}

// player_t::init_scaling ===================================================

void player_t::init_scaling()
{
  if ( sim -> debug ) log_t::output( sim, "Initializing scaling for player (%s)", name() );

  if ( ! is_pet() && ! is_enemy() )
  {
    invert_scaling = 0;

    int role = primary_role();

    bool attack = ( role == ROLE_ATTACK || role == ROLE_HYBRID || role == ROLE_TANK || role == ROLE_DPS );
    bool spell  = ( role == ROLE_SPELL  || role == ROLE_HYBRID || role == ROLE_HEAL || role == ROLE_DPS );
    bool tank   = ( role == ROLE_TANK );

    scales_with[ primary_attribute    ] = true;
    scales_with[ secondary_attribute  ] = true;

    scales_with[ STAT_ACCURACY_RATING ] = true;
    scales_with[ STAT_WEAPON_DMG      ] = attack;

    scales_with[ STAT_CRIT_RATING     ] = true;
    scales_with[ STAT_SURGE_RATING    ] = true;
    scales_with[ STAT_ALACRITY_RATING ] = spell;

    scales_with[ STAT_ARMOR           ] = tank;
    scales_with[ STAT_DEFENSE_RATING  ] = tank;
    scales_with[ STAT_SHIELD_RATING   ] = tank;
    scales_with[ STAT_ABSORB_RATING   ] = tank;

    scales_with[ STAT_POWER           ] = true;

    if ( sim -> scaling -> scale_stat != STAT_NONE && scale_player )
    {
      double v = sim -> scaling -> scale_value;

      switch ( sim -> scaling -> scale_stat )
      {
      case STAT_STRENGTH:  attribute_initial[ ATTR_STRENGTH  ] += v; break;
      case STAT_AIM:       attribute_initial[ ATTR_AIM       ] += v; break;
      case STAT_CUNNING:   attribute_initial[ ATTR_CUNNING   ] += v; break;
      case STAT_WILLPOWER: attribute_initial[ ATTR_WILLPOWER ] += v; break;
      case STAT_ENDURANCE: attribute_initial[ ATTR_ENDURANCE ] += v; break;
      case STAT_PRESENCE:  attribute_initial[ ATTR_PRESENCE  ] += v; break;

      case STAT_POWER:           initial_power           += v; break;
      case STAT_FORCE_POWER:     initial_force_power     += v; break;
      case STAT_TECH_POWER:      initial_tech_power      += v; break;

      case STAT_SURGE_RATING:    initial_surge_rating    += v; break;

      case STAT_DEFENSE_RATING:  initial_defense_rating  += v; break;
      case STAT_SHIELD_RATING:   initial_shield_rating   += v; break;
      case STAT_ABSORB_RATING:   initial_absorb_rating   += v; break;

      case STAT_ACCURACY_RATING: initial_accuracy_rating += v; break;
      case STAT_CRIT_RATING:     initial_crit_rating     += v; break;
      case STAT_ALACRITY_RATING: initial_alacrity_rating += v; break;

      case STAT_WEAPON_DMG:
        if ( main_hand_weapon.damage > 0 )
        {
          main_hand_weapon.damage  += v;
          main_hand_weapon.min_dmg += v;
          main_hand_weapon.max_dmg += v;
        }
        break;

      case STAT_WEAPON_OFFHAND_DMG:
        if ( off_hand_weapon.damage > 0 )
        {
          off_hand_weapon.damage   += v;
          off_hand_weapon.min_dmg  += v;
          off_hand_weapon.max_dmg  += v;
        }
        break;

      case STAT_ARMOR:          initial_armor       += v; break;
      case STAT_BONUS_ARMOR:    initial_bonus_armor += v; break;

      case STAT_ALL: break;

      default: assert( 0 );
      }
    }
  }
}

// player_t::find_item ======================================================

item_t* player_t::find_item( const std::string& str )
{
  int num_items = ( int ) items.size();

  for ( int i=0; i < num_items; i++ )
    if ( str == items[ i ].name() )
      return &( items[ i ] );

  return 0;
}

// player_t::get_attribute() ================================================

double player_t::get_attribute( attribute_type a ) const
{ return attribute[ a ] * composite_attribute_multiplier( a ); }

// player_t::energy_regen_per_second() ======================================

double player_t::energy_regen_per_second() const
{ return 0; }

// player_t::ammo_regen_per_second() ========================================

double player_t::ammo_regen_per_second() const
{ return 0; }

// player_t::ammo_regen_per_second() ========================================
double player_t::heat_regen_per_second() const
{ return 0; }

// player_t::force_regen_per_second() =======================================

double player_t::force_regen_per_second() const
{ return 0; }

// player_t::composite_power() ==============================================

double player_t::composite_power() const
{ return power; }

// player_t::composite_force_power() ========================================

double player_t::composite_force_power() const
{ return force_power; }

// player_t::composite_force_power() ========================================

double player_t::composite_tech_power() const
{ return tech_power; }

// player_t::composite_armor ================================================

double player_t::composite_armor() const
{ return armor * composite_armor_multiplier() + bonus_armor; }

// player_t::composite_armor_multiplier =====================================

double player_t::composite_armor_multiplier() const
{ return armor_multiplier; }

// player_t::armor_penetration ==============================================

double player_t::armor_penetration() const
{ return base_armor_penetration; }

// player_t::armor_penetration_debuff =======================================

double player_t::armor_penetration_debuff() const
{
  // assuming these are additive
  double arpen = 0;
  // TODO: update these with republic names as needed
  const std::string shatter_shot = "shatter_shot";
  const std::string flourish_shot = "flourish_shot";
  const std::string sunder = "sunder";
  const std::string heat_signature = "heat_signature";
  for ( buff_t* b = buff_list; b; b = b -> next )
  {
    if ( sim -> overrides.ignore_player_arpen_debuffs && ! b -> initial_source -> is_enemy() )
      continue;
    if ( b -> name_str.compare( 0, shatter_shot.length(), shatter_shot ) == 0
         || b -> name_str.compare( 0, flourish_shot.length(), flourish_shot ) == 0
         || b -> name_str.compare( 0, sunder.length(), sunder ) == 0
         || b -> name_str.compare( 0, heat_signature.length(), heat_signature ) == 0 )
      arpen += b -> check() ? 0.2 : 0;
  }
  // TODO test: for now assuming 1.3 armor debuff changes means maximum of 20% armor reduction
  return arpen > 0.2 ? 0.8 : 1 - arpen;
}

// player_t::kinetic_damage_reduction =======================================

double player_t::kinetic_damage_reduction() const
{ return base_DR; }

// player_t::energy_damage_reduction ========================================

double player_t::energy_damage_reduction() const
{ return base_DR; }

// player_t::internal_damage_reduction ======================================

double player_t::internal_damage_reduction() const
{
  double dr = base_DR;
  if ( buffs.force_valor -> up() )
    dr += .10;
  return dr;
}

// player_t::elemental_damage_reduction =====================================

double player_t::elemental_damage_reduction() const
{
  double dr = base_DR;
  if ( buffs.force_valor -> up() )
    dr += .10;
  return dr;
}

// player_t::composite_attribute_multiplier =================================

double player_t::composite_attribute_multiplier( attribute_type attr ) const
{
  double m = attribute_multiplier[ attr ];

  switch( attr )
  {
  case ATTR_STRENGTH:
  case ATTR_AIM:
  case ATTR_CUNNING:
  case ATTR_WILLPOWER:
    if ( buffs.force_valor -> up() )
      m += 0.05;
    break;
  case ATTR_ENDURANCE:
    if ( buffs.fortification_hunters_boon -> up() )
      m += 0.05;
    break;
    break;
  default:
    break;
  }

  return m;
}

// player_t::composite_player_multiplier ====================================

double player_t::composite_player_multiplier( const school_type /* school */, action_t* /* a */ ) const
{ return 1.0; }

// player_t::composite_player_td_multiplier =================================

double player_t::composite_player_td_multiplier( const school_type /* school */, action_t* /* a */ ) const
{ return 1.0; }

// player_t::composite_player_heal_multiplier ===============================

double player_t::composite_player_heal_multiplier( const school_type /* school */ ) const
{ return 1.0; }

// player_t::composite_player_th_multiplier =================================

double player_t::composite_player_th_multiplier( const school_type /* school */ ) const
{ return 1.0; }

// player_t::composite_player_absorb_multiplier =============================

double player_t::composite_player_absorb_multiplier( const school_type /* school */ ) const
{ return 1.0; }

// player_t::composite_movement_speed =======================================

double player_t::composite_movement_speed() const
{ return base_movement_speed; }

// player_t::damage_bonus ===================================================

double player_t::damage_bonus( double stats, double multiplier, double extra_power ) const
{ return multiplier * ( 0.2 * stats + 0.23 * ( composite_power() + extra_power ) ); }

double player_t::default_bonus_multiplier() const
{
  double m = 1.0;

  if ( buffs.unnatural_might -> up() )
    m += .05;

  return m;
}

double player_t::default_crit_chance() const
{
  double c = base_crit_chance_ + crit_from_rating + ( (double) bonus_crit_pc_ / 100 );

  if ( buffs.coordination -> up() )
    c += 0.05;

  return c;
}

// player_t::melee_damage_bonus() ===========================================

double player_t::melee_bonus_stats() const
{ return strength(); }

double player_t::melee_bonus_multiplier() const
{ return default_bonus_multiplier(); }

double player_t::melee_crit_from_stats() const
{ return rating_scaler.crit_from_stat( strength() ); }

double player_t::melee_damage_bonus() const
{
  return damage_bonus( melee_bonus_stats(),
                       melee_bonus_multiplier() );
}

double player_t::melee_accuracy_chance() const
{ return default_accuracy_chance(); }

double player_t::melee_crit_chance() const
{ return default_crit_chance() + melee_crit_from_stats(); }

double player_t::melee_avoidance() const
{ return base_melee_avoidance + defense_from_rating; }

// player_t::range_damage_bonus() ===========================================

double player_t::range_bonus_stats() const
{ return aim(); }

double player_t::range_bonus_multiplier() const
{ return default_bonus_multiplier(); }

double player_t::range_crit_from_stats() const
{ return rating_scaler.crit_from_stat( aim() ); }

double player_t::range_damage_bonus() const
{
  return damage_bonus( range_bonus_stats(),
                       range_bonus_multiplier() );
}

double player_t::range_accuracy_chance() const
{ return default_accuracy_chance(); }

double player_t::range_crit_chance() const
{ return default_crit_chance() + range_crit_from_stats(); }

double player_t::range_avoidance() const
{ return base_range_avoidance + defense_from_rating; }

// player_t::force_damage_bonus() ===========================================

double player_t::force_bonus_stats() const
{ return willpower(); }

double player_t::force_bonus_multiplier() const
{ return default_bonus_multiplier(); }

double player_t::force_crit_from_stats() const
{ return rating_scaler.crit_from_stat( willpower() ); }

double player_t::force_damage_bonus() const
{
  return damage_bonus( force_bonus_stats(),
                       force_bonus_multiplier(),
                       composite_force_power() );
}

double player_t::force_accuracy_chance() const
{ return default_accuracy_chance(); }

double player_t::force_crit_chance() const
{ return default_crit_chance() + force_crit_from_stats(); }

double player_t::force_avoidance() const
{ return base_force_avoidance; }

// player_t::tech_damage_bonus() ============================================

double player_t::tech_bonus_stats() const
{ return cunning(); }

double player_t::tech_bonus_multiplier() const
{ return default_bonus_multiplier(); }

double player_t::tech_crit_from_stats() const
{ return rating_scaler.crit_from_stat( cunning() ); }

double player_t::tech_damage_bonus() const
{
  return damage_bonus( tech_bonus_stats(),
                       tech_bonus_multiplier(),
                       composite_tech_power() );
}

double player_t::tech_accuracy_chance() const
{ return default_accuracy_chance(); }

double player_t::tech_crit_chance() const
{ return default_crit_chance() + tech_crit_from_stats(); }

double player_t::tech_avoidance() const
{ return base_tech_avoidance; }

// player_t::healing_bonus() ================================================

double player_t::healing_bonus( double stats, double multiplier, double extra_power ) const
{ return multiplier * ( 0.14 * stats + 0.17 * ( composite_power() + extra_power ) ); }

// player_t::force_healing_bonus() ==========================================

double player_t::force_healing_bonus_stats() const
{ return willpower(); }

double player_t::force_healing_crit_from_stats() const
{ return rating_scaler.crit_from_stat( willpower() ); }

double player_t::force_healing_crit_chance() const
{ return default_crit_chance() + force_healing_crit_from_stats(); }

double player_t::force_healing_bonus_multiplier() const
{ return default_bonus_multiplier(); }

double player_t::force_healing_bonus() const
{
  return healing_bonus( force_healing_bonus_stats(),
                        force_healing_bonus_multiplier(),
                        composite_force_power() );
}

// player_t::tech_healing_bonus() ===========================================

double player_t::tech_healing_bonus_stats() const
{ return cunning(); }

double player_t::tech_healing_crit_from_stats() const
{ return rating_scaler.crit_from_stat( cunning() ); }

double player_t::tech_healing_crit_chance() const
{ return default_crit_chance() + tech_healing_crit_from_stats(); }

double player_t::tech_healing_bonus_multiplier() const
{ return default_bonus_multiplier(); }

double player_t::tech_healing_bonus() const
{
  return healing_bonus( tech_healing_bonus_stats(),
                        tech_healing_bonus_multiplier(),
                        composite_tech_power() );
}

double player_t::shield_chance() const
{ return base_shield_chance + shield_from_rating; }

double player_t::shield_absorb() const
{ return base_shield_absorb + absorb_from_rating; }

// player_t::combat_begin ===================================================

void player_t::combat_begin( sim_t* sim )
{
  player_t::commando_mercenary_combat_begin( sim );
  player_t::gunslinger_sniper_combat_begin( sim );
  player_t::juggernaut_guardian_combat_begin( sim );
  player_t::sage_sorcerer_combat_begin( sim );
  player_t::scoundrel_operative_combat_begin( sim );
  player_t::sentinel_marauder_combat_begin( sim );
  player_t::shadow_assassin_combat_begin( sim );
  player_t::vanguard_powertech_combat_begin( sim );
  player_t::enemy_combat_begin( sim );

  // FIXME: Move to correct classes
  for ( player_t* p = sim -> player_list; p; p = p -> next )
  {
    if ( p -> ooc_buffs() )
    {
      if ( sim -> overrides.coordination > 0 )
        p -> buffs.coordination -> override();

      if ( sim -> overrides.unnatural_might > 0 )
        p -> buffs.unnatural_might -> override();
    }
  }
}

// player_t::combat_begin ===================================================

void player_t::combat_begin()
{
  if ( sim -> debug ) log_t::output( sim, "Combat begins for player %s", name() );

  if ( ! is_pet() && ! is_add() )
  {
    arise();
  }

  init_resources( true );

  if ( primary_resource() == RESOURCE_FORCE )
  {
    gain_t* initial_force = get_gain( "initial_force" );
    initial_force -> type = RESOURCE_FORCE;
    initial_force -> add( resource_max[ RESOURCE_FORCE ] );
  }

  action_sequence.clear();

  iteration_fight_length = timespan_t::zero();
  iteration_waiting_time = timespan_t::zero();
  iteration_executed_foreground_actions = 0;
  iteration_dmg = 0;
  iteration_heal = 0;
  iteration_dmg_taken = 0;
  iteration_heal_taken = 0;

  for ( buff_t* b = buff_list; b; b = b -> next )
    b -> combat_begin();

  for ( stats_t* s = stats_list; s; s = s -> next )
    s -> combat_begin();
}

// player_t::combat_end =====================================================

void player_t::combat_end( sim_t* sim )
{
  player_t::commando_mercenary_combat_end( sim );
  player_t::gunslinger_sniper_combat_end( sim );
  player_t::juggernaut_guardian_combat_end( sim );
  player_t::sage_sorcerer_combat_end( sim );
  player_t::scoundrel_operative_combat_end( sim );
  player_t::sentinel_marauder_combat_end( sim );
  player_t::shadow_assassin_combat_end( sim );
  player_t::vanguard_powertech_combat_end( sim );
  player_t::enemy_combat_end( sim );
}

// player_t::combat_end =====================================================

void player_t::combat_end()
{
  for ( pet_t* pet = pet_list; pet; pet = pet -> next_pet )
  {
    pet -> combat_end();
  }

  if ( ! is_pet() )
  {
    demise();
  }
  else if ( is_pet() )
    cast_pet() -> dismiss();

  fight_length.add( to_seconds( iteration_fight_length ) );

  executed_foreground_actions.add( iteration_executed_foreground_actions );
  waiting_time.add( to_seconds( iteration_waiting_time ) );

  for ( stats_t* s = stats_list; s; s = s -> next )
    s -> combat_end();

  // DMG
  dmg.add( iteration_dmg );
  if ( ! is_enemy() && ! is_add() )
    sim -> iteration_dmg += iteration_dmg;
  for ( pet_t* pet = pet_list; pet; pet = pet -> next_pet )
  {
    iteration_dmg += pet -> iteration_dmg;
  }
  compound_dmg.add( iteration_dmg );

  dps.add( iteration_fight_length != timespan_t::zero() ? iteration_dmg / to_seconds( iteration_fight_length ) : 0 );
  dpse.add( sim -> current_time != timespan_t::zero() ? iteration_dmg / to_seconds( sim -> current_time ) : 0 );

  if ( sim -> debug ) log_t::output( sim, "Combat ends for player %s at time %.4f fight_length=%.4f", name(), to_seconds( sim -> current_time ), to_seconds( iteration_fight_length ) );

  // Heal
  heal.add( iteration_heal );
  if ( ! is_enemy() && ! is_add() )
    sim -> iteration_heal += iteration_heal;
  for ( pet_t* pet = pet_list; pet; pet = pet -> next_pet )
  {
    iteration_heal += pet -> iteration_heal;
  }
  compound_heal.add( iteration_heal );

  hps.add( iteration_fight_length != timespan_t::zero() ? iteration_heal / to_seconds( iteration_fight_length ) : 0 );
  hpse.add( sim -> current_time != timespan_t::zero() ? iteration_heal / to_seconds( sim -> current_time ) : 0 );

  dmg_taken.add( iteration_dmg_taken );
  dtps.add( iteration_fight_length != timespan_t::zero() ? iteration_dmg_taken / to_seconds( iteration_fight_length ) : 0 );

  heal_taken.add( iteration_heal_taken );
  htps.add( iteration_fight_length != timespan_t::zero() ? iteration_heal_taken / to_seconds( iteration_fight_length ) : 0 );

  for ( buff_t* b = buff_list; b; b = b -> next )
  {
    b -> combat_end();
  }
}

// player_t::merge ==========================================================

void player_t::merge( player_t& other )
{
  fight_length.merge( other.fight_length );
  waiting_time.merge( other.waiting_time );
  executed_foreground_actions.merge( other.executed_foreground_actions );

  dmg.merge( other.dmg );
  compound_dmg.merge( other.compound_dmg );
  dps.merge( other.dps );
  dtps.merge( other.dtps );
  dpse.merge( other.dpse );
  dmg_taken.merge( other.dmg_taken );

  heal.merge( other.heal );
  compound_heal.merge( other.compound_heal );
  hps.merge( other.hps );
  htps.merge( other.htps );
  hpse.merge( other.hpse );
  heal_taken.merge( other.heal_taken );

  deaths.merge( other.deaths );

  for ( resource_type i = RESOURCE_NONE; i < RESOURCE_MAX; i++ )
  {
    int num_buckets = ( int ) std::min(       timeline_resource[i].size(),
                                        other.timeline_resource[i].size() );

    for ( int j=0; j < num_buckets; j++ )
    {
      timeline_resource[i][ j ] += other.timeline_resource[i][ j ];
    }

    resource_lost  [ i ] += other.resource_lost  [ i ];
    resource_gained[ i ] += other.resource_gained[ i ];
  }

  for ( buff_t* b = buff_list; b; b = b -> next )
  {
    buff_t* other_b = buff_t::find( &other, b -> name() );
    assert( other_b );
    b -> merge( other_b );
  }

  for ( proc_t* proc = proc_list; proc; proc = proc -> next )
  {
    proc -> merge( other.get_proc( proc -> name_str ) );
  }

  for ( gain_t* gain = gain_list; gain; gain = gain -> next )
  {
    gain -> merge( other.get_gain( gain -> name_str ) );
  }

  for ( stats_t* stats = stats_list; stats; stats = stats -> next )
  {
    stats -> merge( other.get_stats( stats -> name_str ) );
  }

  for ( uptime_t* uptime = uptime_list; uptime; uptime = uptime -> next )
  {
    uptime -> merge( *other.get_uptime( uptime -> name_str ) );
  }

  for ( benefit_t* benefit = benefit_list; benefit; benefit = benefit -> next )
  {
    benefit -> merge( other.get_benefit( benefit -> name_str ) );
  }

  for ( auto const& item :  other.action_map )
    action_map[ item.first ] += item.second;
}

// player_t::reset ==========================================================

void player_t::reset()
{
  if ( sim -> debug ) log_t::output( sim, "Resetting player %s", name() );

  skill = initial_skill;

  gcd_ready = timespan_t::zero();

  sleeping = 1;
  events = 0;

  active_companion = 0;

  stats = initial_stats;

  vengeance_damage = vengeance_value = vengeance_max = 0.0;

  set_alacrity_rating( initial_alacrity_rating );
  crit_rating = initial_crit_rating;
  set_accuracy_rating( initial_accuracy_rating );
  surge_rating = initial_surge_rating;
  defense_rating = initial_defense_rating;
  shield_rating = initial_shield_rating;
  absorb_rating = initial_absorb_rating;

  recalculate_crit_from_rating();
  recalculate_surge_from_rating();
  recalculate_defense_from_rating();
  recalculate_shield_from_rating();
  recalculate_absorb_from_rating();

  boost::copy( attribute_initial, attribute );
  boost::copy( attribute_multiplier_initial, attribute_multiplier );
  boost::copy( initial_resource_reduction, resource_reduction );

  armor           = initial_armor;
  bonus_armor     = initial_bonus_armor;

  power           = initial_power;
  force_power     = initial_force_power;
  tech_power      = initial_tech_power;

  armor_multiplier   = initial_armor_multiplier;

  for ( buff_t* b = buff_list; b; b = b -> next )
  {
    b -> reset();
  }

  last_foreground_action = 0;

  executing = 0;
  channeling = 0;
  readying = 0;
  off_gcd = 0;
  in_combat = false;

  cast_delay_reaction = timespan_t::zero();
  cast_delay_occurred = timespan_t::zero();

  main_hand_weapon.buff_type  = 0;
  main_hand_weapon.buff_value = 0;
  main_hand_weapon.bonus_dmg  = initial_stats.weapon_dmg;

  off_hand_weapon.buff_type  = 0;
  off_hand_weapon.buff_value = 0;
  off_hand_weapon.bonus_dmg  = initial_stats.weapon_offhand_dmg;

  stim = stim_type::none;

  for ( resource_type i = RESOURCE_NONE; i < RESOURCE_MAX; i++ )
  {
    action_callback_t::reset( resource_gain_callbacks[ i ] );
    action_callback_t::reset( resource_loss_callbacks[ i ] );
  }
  for ( result_type i = RESULT_NONE; i < RESULT_MAX; i++ )
  {
    action_callback_t::reset( attack_callbacks[ i ] );
    action_callback_t::reset( spell_callbacks [ i ] );
    action_callback_t::reset( tick_callbacks  [ i ] );
  }
  for ( school_type i = SCHOOL_NONE; i < SCHOOL_MAX; i++ )
  {
    action_callback_t::reset( tick_damage_callbacks  [ i ] );
    action_callback_t::reset( direct_damage_callbacks[ i ] );
  }

  init_resources( true );

  for ( action_t* a = action_list; a; a = a -> next ) a -> reset();

  for ( cooldown_t* c = cooldown_list; c; c = c -> next ) c -> reset();

  for ( dot_t* d = dot_list; d; d = d -> next ) d -> reset();

  for ( auto p : targetdata )
  {
    if ( p )
      p -> reset();
  }

  for ( stats_t* s = stats_list; s; s = s -> next ) s -> reset();

  potion_used = 0;

  temporary = gear_stats_t();
}

// player_t::schedule_ready =================================================

void player_t::schedule_ready( timespan_t delta_time,
                               bool   waiting )
{
  if ( readying )
  {
    sim -> errorf( "\nplayer_t::schedule_ready assertion error: readying == true ( player %s )\n", name() );
    assert( 0 );
  }
  action_t* was_executing = ( channeling ? channeling : executing );

  if ( sleeping )
    return;

  executing = 0;
  channeling = 0;
  action_queued = false;

  if ( ! has_foreground_actions( this ) ) return;

  timespan_t gcd_adjust = gcd_ready - ( sim -> current_time + delta_time );
  if ( gcd_adjust > timespan_t::zero() ) delta_time += gcd_adjust;

  if ( unlikely( waiting ) )
  {
    iteration_waiting_time += delta_time;
  }
  else
  {
    timespan_t lag = timespan_t::zero();

    if ( last_foreground_action && ! last_foreground_action -> auto_cast )
    {
      if ( last_foreground_action -> ability_lag > timespan_t::zero() )
      {
        timespan_t ability_lag = rngs.lag_ability -> gauss( last_foreground_action -> ability_lag, last_foreground_action -> ability_lag_stddev );
        timespan_t gcd_lag     = rngs.lag_gcd   -> gauss( sim ->   gcd_lag, sim ->   gcd_lag_stddev );
        timespan_t diff        = ( gcd_ready + gcd_lag ) - ( sim -> current_time + ability_lag );
        if ( diff > timespan_t::zero() && sim -> strict_gcd_queue )
        {
          lag = gcd_lag;
        }
        else
        {
          lag = ability_lag;
          action_queued = true;
        }
      }
      else if ( last_foreground_action -> gcd() == timespan_t::zero() )
      {
        lag = timespan_t::zero();
      }
      else if ( last_foreground_action -> channeled )
      {
        lag = rngs.lag_channel -> gauss( sim -> channel_lag, sim -> channel_lag_stddev );
      }
      else
      {
        timespan_t   gcd_lag = rngs.lag_gcd   -> gauss( sim ->   gcd_lag, sim ->   gcd_lag_stddev );
        timespan_t queue_lag = rngs.lag_queue -> gauss( sim -> queue_lag, sim -> queue_lag_stddev );

        timespan_t diff = ( gcd_ready + gcd_lag ) - ( sim -> current_time + queue_lag );

        if ( diff > timespan_t::zero() && sim -> strict_gcd_queue )
        {
          lag = gcd_lag;
        }
        else
        {
          lag = queue_lag;
          action_queued = true;
        }
      }
    }

    if ( lag < timespan_t::zero() ) lag = timespan_t::zero();

    delta_time += lag;
  }

  if ( last_foreground_action )
  {
    last_foreground_action -> stats -> total_execute_time += delta_time;
  }

  readying = new ( sim ) player_ready_event_t( this, delta_time );

  if ( was_executing && was_executing -> gcd() > timespan_t::zero() && ! was_executing -> background && ! was_executing -> proc && ! was_executing -> repeating )
  {
    // Record the last ability use time for cast_react
    cast_delay_occurred = readying -> occurs();
    cast_delay_reaction = rngs.lag_brain -> gauss( brain_lag, brain_lag_stddev );
    if ( sim -> debug )
    {
      log_t::output( sim, "%s %s schedule_ready(): cast_finishes=%f cast_delay=%f",
                     name_str.c_str(),
                     was_executing -> name_str.c_str(),
                     to_seconds( readying -> occurs() ),
                     to_seconds( cast_delay_reaction >= timespan_t::zero() ? cast_delay_reaction : timespan_t::zero() ) );
    }
  }
}

// player_t::arise ==========================================================

void player_t::arise()
{
  if ( sim -> log )
    log_t::output( sim, "%s arises.", name() );

  if ( ! initial_sleeping )
    sleeping = 0;

  if ( sleeping )
    return;

  init_resources( true );

  readying = 0;
  off_gcd = 0;

  arise_time = sim -> current_time;

  schedule_ready();
}

// player_t::demise =========================================================

void player_t::demise()
{
  // No point in demising anything if we're not even active
  if ( sleeping == 1 ) return;

  if ( sim -> log )
    log_t::output( sim, "%s demises.", name() );

  assert( arise_time >= timespan_t::zero() );
  iteration_fight_length += sim -> current_time - arise_time;
  arise_time = timespan_t_min();

  sleeping = 1;
  if ( readying )
  {
    event_t::cancel( readying );
    readying = 0;
  }

  event_t::cancel( off_gcd );

  for ( buff_t* b = buff_list; b; b = b -> next )
  {
    b -> expire();
    // Dead actors speak no lies .. or proc aura delayed buffs
    if ( b -> delay ) event_t::cancel( b -> delay );
  }
  for ( action_t* a = action_list; a; a = a -> next )
  {
    a -> cancel();
  }

  //sim -> cancel_events( this );

  for ( pet_t* pet = pet_list; pet; pet = pet -> next_pet )
  {
    pet -> demise();
  }
}

// player_t::interrupt ======================================================

void player_t::interrupt()
{
  // FIXME! Players will need to override this to handle background repeating actions.

  if ( sim -> log ) log_t::output( sim, "%s is interrupted", name() );

  if ( executing  ) executing  -> interrupt_action();
  if ( channeling ) channeling -> interrupt_action();

  if ( buffs.stunned -> check() )
  {
    if ( readying ) event_t::cancel( readying );
    if ( off_gcd ) event_t::cancel( off_gcd );
  }
  else
  {
    if ( ! readying && ! sleeping ) schedule_ready();
  }
}

// player_t::halt ===========================================================

void player_t::halt()
{
  if ( sim -> log ) log_t::output( sim, "%s is halted", name() );

  interrupt();
}

// player_t::stun() =========================================================

void player_t::stun()
{
  halt();
}

// player_t::moving =========================================================

void player_t::moving()
{
  // FIXME! In the future, some movement events may not cause auto-attack to stop.

  halt();
}

// player_t::clear_debuffs===================================================

void player_t::clear_debuffs()
{
  // FIXME! At the moment we are just clearing DoTs

  if ( sim -> log ) log_t::output( sim, "%s clears all debuffs", name() );

  for ( auto p : targetdata )
  {
    if ( p )
      p -> clear_debuffs();
  }
}

// player_t::print_action_map ===============================================

std::string player_t::print_action_map( int iterations, int precision ) const
{
  std::ostringstream ret;
  ret.precision( precision );
  ret << "Label: Number of executes (Average number of executes per iteration)<br />\n";

  std::vector<std::pair<std::string, int>> vec( action_map.begin(), action_map.end() );
  boost::sort( vec );

  for ( auto const& item : vec )
  {
    ret << item.first << ": " << item.second;
    if ( iterations > 0 ) ret << " (" << ( ( double )item.second ) / iterations << ')';
    ret << "<br />\n";
  }

  return ret.str();
}

// player_t::execute_action =================================================

action_t* player_t::execute_action()
{
  readying = 0;
  off_gcd = 0;

  action_t* action=0;

  for ( action = action_list; action; action = action -> next )
  {
    if ( action -> background ||
         action -> sequence )
      continue;

    if ( action -> ready() )
      break;

    if ( action -> wait_on_ready == 1 )
    {
      action = 0;
      break;
    }
  }

  last_foreground_action = action;

  if ( action )
  {
    action -> schedule_execute();
    iteration_executed_foreground_actions++;
    if ( action -> marker ) action_sequence += action -> marker;
    if ( ! action -> label_str.empty() )
      action_map[ action -> label_str ] += 1;
  }

  return action;
}

// player_t::regen ==========================================================

void player_t::regen( const timespan_t periodicity )
{
  const unsigned index = to_seconds<unsigned>( sim -> current_time );

  for ( resource_type i = RESOURCE_NONE; i < RESOURCE_MAX; i++ )
  {
    if ( resource_max[ i ] != 0 )
      timeline_resource[ i ][ index ] += resource_current[ i ] * to_seconds( periodicity );
  }
}

// player_t::resource_loss ==================================================

double player_t::resource_loss( resource_type resource,
                                double        amount,
                                gain_t*       source,
                                action_t*     action )
{
  if ( sleeping || amount == 0 )
    return 0;

  double actual_amount = amount;
  if ( !infinite_resource[ resource ] )
    actual_amount = std::min( amount, resource_current[ resource ] );

  resource_current[ resource ] -= actual_amount;
  resource_lost   [ resource ] += actual_amount;

  if ( resource == primary_resource() )
    primary_resource_cap -> update( resource_used_amount( resource ) <= 0 );


  if ( source )
  {
    if ( source -> type == RESOURCE_NONE )
      source -> type = static_cast<resource_type>( resource );

    if ( resource != source -> type )
    {
      sim -> errorf( "player_t::resource_loss: player=%s gain=%s resource type not identical to gain -> resource type..\n resource=%s gain=%s",
                     name(), source -> name_str.c_str(), util_t::resource_type_string( resource ), util_t::resource_type_string( source -> type ) );
      assert ( 0 );
    }

    int invert = resource_reverse[ resource ] ? -1 : 1;
    source -> add( -actual_amount * invert, ( actual_amount - amount ) * invert);
  }

  action_callback_t::trigger( resource_loss_callbacks[ resource ], action, &actual_amount );

  if ( sim -> debug )
    log_t::output( sim, "Player %s loses %.2f (%.2f) %s. health pct: %.2f",
                   name(), actual_amount, amount, util_t::resource_type_string( resource ), health_percentage()  );

  return actual_amount;
}

// player_t::resource_gain ==================================================

double player_t::resource_gain( resource_type resource,
                                double        amount,
                                gain_t*       source,
                                action_t*     action )
{
  if ( sleeping )
    return 0;

  double actual_amount = amount;
  if ( !infinite_resource[ resource ] )
    actual_amount = std::min( amount, resource_max[ resource ] - resource_current[ resource ] );

  resource_current[ resource ] += actual_amount;
  resource_gained [ resource ] += actual_amount;

  if ( resource == primary_resource() )
    primary_resource_cap -> update( resource_used_amount( resource ) <= 0 );


  if ( source )
  {
    if ( source -> type == RESOURCE_NONE )
      source -> type = static_cast<resource_type>( resource );

    if ( resource != source -> type )
    {
      sim -> errorf( "player_t::resource_gain: player=%s gain=%s resource type not identical to gain -> resource type..\n resource=%s gain=%s",
                     name(), source -> name_str.c_str(), util_t::resource_type_string( resource ), util_t::resource_type_string( source -> type ) );
      assert ( 0 );
    }

    int invert = resource_reverse[ resource ] ? -1 : 1;
    source -> add( actual_amount * invert, ( amount - actual_amount ) * invert );
  }

  action_callback_t::trigger( resource_gain_callbacks[ resource ], action, &actual_amount );

  if ( sim -> log )
  {
    log_t::output( sim, "%s gains %.2f (%.2f) %s from %s (%.2f/%.2f)",
                   name(), actual_amount, amount,
                   util_t::resource_type_string( resource ),
                   source ? source -> name() : action ? action -> name() : "unknown",
                   resource_current[ resource ], resource_max[ resource ] );
  }

  return actual_amount;
}

// player_t::resource_available =============================================

bool player_t::resource_available( resource_type resource,
                                   double        cost ) const
{
  if ( resource == RESOURCE_NONE || cost <= 0 || infinite_resource[ resource ] == 1 )
    return true;

  if ( resource_reverse[ resource ] )
      return resource_max[ resource ] - resource_current[ resource ] >= cost;
  return resource_current[ resource ] >= cost;
}

// player_t::resource_available =============================================

double player_t::resource_available_amount( resource_type resource ) const
{
  if ( resource == RESOURCE_NONE )
    return 0;

  if (infinite_resource[ resource ] )
      return resource_max[ resource ];

  if ( resource_reverse[ resource ] )
      return resource_max[ resource ] - resource_current[ resource ];

  return resource_current[ resource ];
}

// player_t::resource_available =============================================

double player_t::resource_used_amount( resource_type resource ) const
{
  if (( resource == RESOURCE_NONE ) || (infinite_resource[ resource ] ))
    return 0;

  if ( resource_reverse[ resource ] )
      return resource_current[ resource ];

  return resource_max[ resource ] - resource_current[ resource ];
}

// player_t::recalculate_resource_max =======================================

void player_t::recalculate_resource_max( resource_type resource )
{
  // The first 20pts of intellect/stamina only provide 1pt of mana/health.

  resource_max[ resource ] = resource_base[ resource ] +
                             gear.resource[ resource ] +
                             enchant.resource[ resource ] +
                             ( is_pet() ? 0 : sim -> enchant.resource[ resource ] );

  switch ( resource )
  {
  case RESOURCE_HEALTH:
  {
    double adjust = ( is_pet() || is_enemy() || is_add() ) ? 0 : std::min( 20, ( int ) floor( endurance() ) );
    resource_max[ resource ] += ( floor( endurance() ) - adjust ) * health_per_endurance + adjust;

    break;
  }
  default: break;
  }
}

// player_t::primary_tab ====================================================

talent_tab_type player_t::primary_tab() const
{
  int best = 0;

  for ( int i=1; i < MAX_TALENT_TREES; i++ )
  {
    if ( talent_tab_points[ i ] > talent_tab_points[ best ] )
      best = i;
  }

  return static_cast<talent_tab_type>( best );
}

// player_t::primary_role ===================================================

role_type player_t::primary_role() const
{ return role; }

// player_t::primary_tree_name ==============================================

const char* player_t::primary_tree_name() const
{
  return util_t::talent_tree_string( primary_tree() );
}

// player_t::primary_tree ===================================================

talent_tree_type player_t::primary_tree() const
{
  if ( specialization == TALENT_TAB_NONE )
    return TREE_NONE;

  return tree_type[ specialization ];
}

// player_t::normalize_by ===================================================

stat_type player_t::normalize_by() const
{
  if ( sim -> normalized_stat != STAT_NONE )
    return sim -> normalized_stat;

  if ( primary_role() == ROLE_TANK )
    return STAT_ARMOR;

  return STAT_POWER;
}

// player_t::health_percentage() ============================================

double player_t::health_percentage() const
{
  return resource_current[ RESOURCE_HEALTH ] / resource_max[ RESOURCE_HEALTH ] * 100 ;
}

// target_t::time_to_die ====================================================

timespan_t player_t::time_to_die() const
{
  // FIXME: Someone can figure out a better way to do this, for now, we NEED to
  // wait a minimum gcd before starting to estimate fight duration based on health,
  // otherwise very odd things happen with multi-actor simulations and time_to_die
  // expressions
  if ( resource_base[ RESOURCE_HEALTH ] > 0 && sim -> current_time >= from_seconds( 1.0 ) )
  {
    return sim -> current_time * ( resource_current[ RESOURCE_HEALTH ] / iteration_dmg_taken );
  }
  else
  {
    return ( sim -> expected_time - sim -> current_time );
  }
}

// player_t::total_reaction_time ============================================

timespan_t player_t::total_reaction_time() const
{
  return rngs.lag_reaction -> exgauss( reaction_mean, reaction_stddev, reaction_nu );
}

// player_t::stat_gain ======================================================

void player_t::stat_gain( stat_type stat,
                          double    amount,
                          gain_t*   gain,
                          action_t* action,
                          bool      temporary_stat )
{
  if ( amount <= 0 ) return;

  if ( sim -> log ) log_t::output( sim, "%s gains %.0f %s%s", name(), amount, util_t::stat_type_string( stat ), temporary_stat ? " (temporary)" : "" );

  int temp_value = temporary_stat ? 1 : 0;
  switch ( stat )
  {
  case STAT_STRENGTH:  stats.attribute[ ATTR_STRENGTH  ] += amount; attribute[ ATTR_STRENGTH  ] += amount; temporary.attribute[ ATTR_STRENGTH  ] += temp_value * amount; break;
  case STAT_AIM:       stats.attribute[ ATTR_AIM       ] += amount; attribute[ ATTR_AIM       ] += amount; temporary.attribute[ ATTR_AIM       ] += temp_value * amount; break;
  case STAT_CUNNING:   stats.attribute[ ATTR_CUNNING   ] += amount; attribute[ ATTR_CUNNING   ] += amount; temporary.attribute[ ATTR_CUNNING   ] += temp_value * amount; break;
  case STAT_WILLPOWER: stats.attribute[ ATTR_WILLPOWER ] += amount; attribute[ ATTR_WILLPOWER ] += amount; temporary.attribute[ ATTR_WILLPOWER ] += temp_value * amount; break;
  case STAT_ENDURANCE: stats.attribute[ ATTR_ENDURANCE ] += amount; attribute[ ATTR_ENDURANCE ] += amount; temporary.attribute[ ATTR_ENDURANCE ] += temp_value * amount; recalculate_resource_max( RESOURCE_HEALTH ); break;
  case STAT_PRESENCE:  stats.attribute[ ATTR_PRESENCE  ] += amount; attribute[ ATTR_PRESENCE  ] += amount; temporary.attribute[ ATTR_PRESENCE  ] += temp_value * amount; break;

  case STAT_ALL:
    for ( attribute_type i = ATTRIBUTE_NONE; i < ATTRIBUTE_MAX; i++ )
    {
      stats.attribute[ i ] += amount;
      temporary.attribute[ i ] += temp_value * amount;
      attribute[ i ] += amount;
    }
    break;

  case STAT_HEALTH: resource_gain( RESOURCE_HEALTH, amount, gain, action ); break;
  case STAT_MANA:   resource_gain( RESOURCE_MANA,   amount, gain, action ); break;
  case STAT_RAGE:   resource_gain( RESOURCE_RAGE,   amount, gain, action ); break;
  case STAT_ENERGY: resource_gain( RESOURCE_ENERGY, amount, gain, action ); break;
  case STAT_AMMO:   resource_gain( RESOURCE_AMMO,   amount, gain, action ); break;
  case STAT_HEAT:   resource_gain( RESOURCE_HEAT,   amount, gain, action ); break;

  case STAT_MAX_HEALTH: resource_max[ RESOURCE_HEALTH ] += amount; resource_gain( RESOURCE_HEALTH, amount, gain, action ); break;
  case STAT_MAX_MANA:   resource_max[ RESOURCE_MANA   ] += amount; resource_gain( RESOURCE_MANA,   amount, gain, action ); break;
  case STAT_MAX_RAGE:   resource_max[ RESOURCE_RAGE   ] += amount; resource_gain( RESOURCE_RAGE,   amount, gain, action ); break;
  case STAT_MAX_ENERGY: resource_max[ RESOURCE_ENERGY ] += amount; resource_gain( RESOURCE_ENERGY, amount, gain, action ); break;
  case STAT_MAX_AMMO:   resource_max[ RESOURCE_AMMO   ] += amount; resource_gain( RESOURCE_AMMO,   amount, gain, action ); break;
  case STAT_MAX_HEAT:   resource_max[ RESOURCE_HEAT   ] += amount; resource_gain( RESOURCE_HEAT,   amount, gain, action ); break;

  case STAT_POWER:             stats.power             += amount; power                     += amount; break;
  case STAT_FORCE_POWER:       stats.force_power       += amount; force_power               += amount; break;
  case STAT_TECH_POWER:        stats.tech_power        += amount; tech_power                += amount; break;

  case STAT_ACCURACY_RATING:
    stats.accuracy_rating += amount;
    temporary.accuracy_rating += temp_value * amount;
    set_accuracy_rating( get_accuracy_rating() + amount );
    break;

  case STAT_CRIT_RATING:
    stats.crit_rating += amount;
    temporary.crit_rating += temp_value * amount;
    crit_rating += amount;
    recalculate_crit_from_rating();
    break;

  case STAT_ALACRITY_RATING:
    stats.alacrity_rating += amount;
    temporary.alacrity_rating += temp_value * amount;
    set_alacrity_rating( get_alacrity_rating() + amount );
    break;

  case STAT_SURGE_RATING:
    stats.surge_rating += amount;
    temporary.surge_rating += temp_value * amount;
    surge_rating       += amount;
    recalculate_surge_from_rating();
    break;

  case STAT_DEFENSE_RATING:
    stats.defense_rating += amount;
    temporary.defense_rating += temp_value * amount;
    defense_rating += amount;
    recalculate_defense_from_rating();
    break;

  case STAT_SHIELD_RATING:
    stats.shield_rating += amount;
    temporary.shield_rating += temp_value * amount;
    shield_rating += amount;
    recalculate_shield_from_rating();
    break;

  case STAT_ABSORB_RATING:
    stats.absorb_rating += amount;
    temporary.absorb_rating += temp_value * amount;
    absorb_rating += amount;
    recalculate_absorb_from_rating();
    break;

  case STAT_ARMOR:          stats.armor          += amount; temporary.armor += temp_value * amount; armor       += amount;                  break;
  case STAT_BONUS_ARMOR:    stats.bonus_armor    += amount; bonus_armor += amount;                  break;

  default: assert( 0 );
  }
}

// player_t::stat_loss ======================================================

void player_t::stat_loss( stat_type stat,
                          double    amount,
                          action_t* action,
                          bool      temporary_buff )
{
  if ( amount <= 0 ) return;

  if ( sim -> log ) log_t::output( sim, "%s loses %.0f %s%s", name(), amount, util_t::stat_type_string( stat ), ( temporary_buff ) ? " (temporary)" : "" );

  int temp_value = temporary_buff ? 1 : 0;
  switch ( stat )
  {
  case STAT_STRENGTH:  stats.attribute[ ATTR_STRENGTH  ] -= amount; temporary.attribute[ ATTR_STRENGTH  ] -= temp_value * amount; attribute[ ATTR_STRENGTH  ] -= amount; break;
  case STAT_AIM:       stats.attribute[ ATTR_AIM       ] -= amount; temporary.attribute[ ATTR_AIM       ] -= temp_value * amount; attribute[ ATTR_AIM       ] -= amount; break;
  case STAT_CUNNING:   stats.attribute[ ATTR_CUNNING   ] -= amount; temporary.attribute[ ATTR_CUNNING   ] -= temp_value * amount; attribute[ ATTR_CUNNING   ] -= amount; break;
  case STAT_WILLPOWER: stats.attribute[ ATTR_WILLPOWER ] -= amount; temporary.attribute[ ATTR_WILLPOWER ] -= temp_value * amount; attribute[ ATTR_WILLPOWER ] -= amount; break;
  case STAT_ENDURANCE: stats.attribute[ ATTR_ENDURANCE ] -= amount; temporary.attribute[ ATTR_ENDURANCE ] -= temp_value * amount; attribute[ ATTR_ENDURANCE ] -= amount; stat_loss( STAT_MAX_HEALTH, floor( amount * composite_attribute_multiplier( ATTR_ENDURANCE ) ) * health_per_endurance, action ); break;
  case STAT_PRESENCE:  stats.attribute[ ATTR_PRESENCE  ] -= amount; temporary.attribute[ ATTR_PRESENCE  ] -= temp_value * amount; attribute[ ATTR_PRESENCE  ] -= amount; break;

  case STAT_ALL:
    for ( attribute_type i = ATTRIBUTE_NONE; i < ATTRIBUTE_MAX; i++ )
    {
      stats.attribute[ i ] -= amount;
      temporary.attribute[ i ] -= temp_value * amount;
      attribute[ i ] -= amount;
    }
    break;

  case STAT_HEALTH: resource_loss( RESOURCE_HEALTH, amount, 0, action ); break;
  case STAT_MANA:   resource_loss( RESOURCE_MANA,   amount, 0, action ); break;
  case STAT_RAGE:   resource_loss( RESOURCE_RAGE,   amount, 0, action ); break;
  case STAT_ENERGY: resource_loss( RESOURCE_ENERGY, amount, 0, action ); break;
  case STAT_AMMO:   resource_loss( RESOURCE_AMMO,   amount, 0, action ); break;
  case STAT_HEAT:   resource_loss( RESOURCE_HEAT,   amount, 0, action ); break;

  case STAT_MAX_HEALTH:
  case STAT_MAX_MANA:
  case STAT_MAX_RAGE:
  case STAT_MAX_ENERGY:
  case STAT_MAX_AMMO:
  case STAT_MAX_HEAT:
  {
    resource_type r = ( stat == STAT_MAX_HEALTH ? RESOURCE_HEALTH :
                      ( stat == STAT_MAX_MANA   ? RESOURCE_MANA   :
                      ( stat == STAT_MAX_RAGE   ? RESOURCE_RAGE   :
                      ( stat == STAT_MAX_ENERGY ? RESOURCE_ENERGY :
                      ( stat == STAT_MAX_HEAT   ? RESOURCE_HEAT   :  RESOURCE_AMMO )))));
    recalculate_resource_max( r );
    double delta = resource_current[ r ] - resource_max[ r ];
    if ( delta > 0 ) resource_loss( r, delta, 0, action );
  }
  break;

  case STAT_POWER:       stats.power       -= amount; power       -= amount; break;
  case STAT_FORCE_POWER: stats.force_power -= amount; force_power -= amount; break;
  case STAT_TECH_POWER:  stats.tech_power  -= amount; tech_power  -= amount; break;

  case STAT_ACCURACY_RATING:
    stats.accuracy_rating -= amount;
    temporary.accuracy_rating -= temp_value * amount;
    set_accuracy_rating( get_accuracy_rating() - amount );
    break;

  case STAT_CRIT_RATING:
    stats.crit_rating -= amount;
    temporary.crit_rating -= temp_value * amount;
    crit_rating       -= amount;
    recalculate_crit_from_rating();
    break;

  case STAT_ALACRITY_RATING:
    stats.alacrity_rating -= amount;
    temporary.alacrity_rating -= temp_value * amount;
    set_alacrity_rating( get_alacrity_rating() - amount );
    break;

  case STAT_SURGE_RATING:
    stats.surge_rating -= amount;
    temporary.surge_rating -= temp_value * amount;
    surge_rating       -= amount;
    recalculate_surge_from_rating();
    break;

  case STAT_DEFENSE_RATING:
    stats.defense_rating -= amount;
    temporary.defense_rating -= temp_value * amount;
    defense_rating       -= amount;
    recalculate_defense_from_rating();
    break;

  case STAT_SHIELD_RATING:
    stats.shield_rating -= amount;
    temporary.shield_rating -= temp_value * amount;
    shield_rating       -= amount;
    recalculate_shield_from_rating();
    break;

  case STAT_ABSORB_RATING:
    stats.absorb_rating -= amount;
    temporary.absorb_rating -= temp_value * amount;
    absorb_rating       -= amount;
    recalculate_absorb_from_rating();
    break;

  case STAT_ARMOR:          stats.armor          -= amount; temporary.armor -= temp_value * amount; armor       -= amount;                  break;
  case STAT_BONUS_ARMOR:    stats.bonus_armor    -= amount; bonus_armor -= amount;                  break;

  default: assert( 0 );
  }
}

// player_t::cost_reduction_gain ============================================

void player_t::cost_reduction_gain( school_type school,
                                    double      amount,
                                    gain_t*,
                                    action_t* )
{
  if ( amount <= 0 ) return;

  if ( sim -> log ) log_t::output( sim, "%s gains a cost reduction of %.0f on abilities of school %s", name(), amount, util_t::school_type_string( school ) );

  resource_reduction[ school ] += amount;
}

// player_t::cost_reduction_loss ============================================

void player_t::cost_reduction_loss( school_type school,
                                    double      amount,
                                    action_t* )
{
  if ( amount <= 0 ) return;

  if ( sim -> log ) log_t::output( sim, "%s loses a cost reduction %.0f on abilities of school %s", name(), amount, util_t::school_type_string( school ) );

  resource_reduction[ school ] -= amount;
}

// player_t::assess_damage ==================================================

double player_t::assess_damage( double      amount,
                                school_type school,
                                dmg_type    dmg_type,
                                result_type result,
                                action_t*   action )
{
  double mitigated_amount = target_mitigation( amount, school, dmg_type, result, action );

  size_t num_absorbs = absorb_buffs.size();
  double absorbed_amount = 0;
  if ( num_absorbs > 0 )
  {
    for ( size_t i = 0; i < num_absorbs; i++ )
    {
      double buff_value = absorb_buffs[ i ] -> value();
      double value = std::min( mitigated_amount - absorbed_amount, buff_value );
      absorbed_amount += value;
      if ( sim -> debug ) log_t::output( sim, "%s %s absorbs %.2f",
                                         name(), absorb_buffs[ i ] -> name(), value );
      if ( value == buff_value )
        absorb_buffs[ i ] -> expire();
      else
      {
        absorb_buffs[ i ] -> current_value -= value;
        if ( sim -> debug ) log_t::output( sim, "%s %s absorb remaining %.2f",
                                           name(), absorb_buffs[ i ] -> name(), absorb_buffs[ i ] -> current_value );
      }
    }
  }
  mitigated_amount -= absorbed_amount;

  iteration_dmg_taken += mitigated_amount;

  double actual_amount = resource_loss( RESOURCE_HEALTH, mitigated_amount, 0, action );

  if ( resource_current[ RESOURCE_HEALTH ] <= 0 && !is_enemy() && infinite_resource[ RESOURCE_HEALTH ] == 0 )
  {
    // This can only save the target, if the damage is less than 200% of the target's health as of 4.0.6

    if ( ! sleeping )
    {
      if ( ! sleeping )
      {
        deaths.add( to_seconds( sim -> current_time ) );
      }
      if ( sim -> log ) log_t::output( sim, "%s has died.", name() );
      demise();
    }
    if ( sim -> log ) log_t::output( sim, "%s has died.", name() );

    demise();
  }

  if ( vengeance_enabled )
  {
    vengeance_damage += actual_amount;
    vengeance_was_attacked = true;
  }

  return mitigated_amount;
}

double player_t::school_damage_reduction( school_type school ) const
{
  switch( school )
  {
  case SCHOOL_KINETIC:
    return kinetic_damage_reduction();
  case SCHOOL_ENERGY:
    return energy_damage_reduction();
  case SCHOOL_INTERNAL:
    return internal_damage_reduction();
  case SCHOOL_ELEMENTAL:
    return elemental_damage_reduction();
  default:
    return 0;
  }
}

void player_t::recalculate_alacrity()
{ computed_alacrity = get_base_alacrity() + rating_scaler.alacrity( get_alacrity_rating() ); }

void player_t::recalculate_crit_from_rating()
{ crit_from_rating = rating_scaler.crit( crit_rating ); }

void player_t::recalculate_accuracy()
{ computed_accuracy = get_base_accuracy() + rating_scaler.accuracy( get_accuracy_rating() ) + ( (double) bonus_accuracy_pc_ / 100 ); }

void player_t::recalculate_surge_from_rating()
{ surge_bonus = rating_scaler.surge( surge_rating ) + ( (double) bonus_surge_pc_ / 100 ); }

void player_t::recalculate_defense_from_rating()
{ defense_from_rating = rating_scaler.defense( defense_rating ); }

void player_t::recalculate_shield_from_rating()
{ shield_from_rating = rating_scaler.shield( shield_rating ); }

void player_t::recalculate_absorb_from_rating()
{ absorb_from_rating = rating_scaler.absorb( absorb_rating ); }


// player_t::offhand_multiplier =============================================
double player_t::offhand_multiplier()
{
  return 0.3;
}

// player_t::target_mitigation ==============================================

double player_t::target_mitigation( double      amount,
                                    school_type school,
                                    dmg_type,
                                    result_type result,
                                    action_t*   action )
{
  if ( amount <= 0 )
    return 0;

  double mitigated_amount = amount;

  if ( result == RESULT_BLOCK )
  {
    double absorb = shield_absorb();
    mitigated_amount *= ( 1 - absorb );

    if ( sim -> debug )
      log_t::output( sim, "%s shield absorbs %.1f%%", name(), absorb * 100.0 );

    if ( mitigated_amount <= 0 ) return 0;
  }

  double resist = school_damage_reduction( school );
  double armor_resist = 0;

  if ( school == SCHOOL_KINETIC || school == SCHOOL_ENERGY )
  {
    assert( action );
    double action_armor = action -> armor();

    armor_resist = action_armor / ( action_armor + rating_t::armor_divisor( action -> player -> level ) );
    armor_resist = std::min( 0.75, std::max( 0.0, armor_resist ) );

    if ( sim -> debug )
      log_t::output( sim, "%s %.0f armor mitigates %.1f%%", name(), action_armor, armor_resist * 100.0 );

    resist += armor_resist;
  }

  if ( sim -> debug && resist > armor_resist )
    log_t::output( sim, "%s total damage reduction %.1f%%", name(), resist * 100.0 );

  mitigated_amount *= 1.0 - resist;

  return std::max( 0.0, mitigated_amount );
}

// player_t::assess_heal ====================================================

player_t::heal_info_t player_t::assess_heal( double      amount,
                                             school_type,
                                             dmg_type,
                                             result_type,
                                             action_t*   action )
{
  heal_info_t heal;

  heal.amount = resource_gain( RESOURCE_HEALTH, amount, 0, action );
  heal.actual = amount;

  iteration_heal_taken += amount;

  return heal;
}

// player_t::summon_pet =====================================================

void player_t::summon_pet( const std::string& pet_name,
                           timespan_t         duration )
{
  for ( pet_t* p = pet_list; p; p = p -> next_pet )
  {
    if ( p -> name_str == pet_name )
    {
      p -> summon( duration );
      return;
    }
  }
  sim -> errorf( "Player %s is unable to summon pet '%s'\n", name(), pet_name.c_str() );
}

// player_t::dismiss_pet ====================================================

void player_t::dismiss_pet( const std::string& pet_name )
{
  for ( pet_t* p = pet_list; p; p = p -> next_pet )
  {
    if ( p -> name_str == pet_name )
    {
      p -> dismiss();
      return;
    }
  }
  sim -> errorf( "Player %s can't find pet '%s' to dismiss\n", name(), pet_name.c_str() );
}

// player_t::register_callbacks =============================================

void player_t::register_callbacks()
{}

// player_t::register_resource_gain_callback ================================

void player_t::register_resource_gain_callback( resource_type      resource,
                                                action_callback_t* cb )
{
  resource_gain_callbacks[ resource ].push_back( cb );
}

// player_t::register_resource_loss_callback ================================

void player_t::register_resource_loss_callback( resource_type      resource,
                                                action_callback_t* cb )
{
  resource_loss_callbacks[ resource ].push_back( cb );
}

// player_t::register_attack_callback =======================================

void player_t::register_attack_callback( int64_t mask,
                                         action_callback_t* cb )
{
  for ( result_type i = RESULT_NONE; i < RESULT_MAX; i++ )
  {
    if ( ( i > 0 && mask < 0 ) || ( mask & ( int64_t( 1 ) << i ) ) )
    {
      attack_callbacks[ i ].push_back( cb );
    }
  }
}

// player_t::register_spell_callback ========================================

void player_t::register_spell_callback( int64_t mask,
                                        action_callback_t* cb )
{
  for ( result_type i = RESULT_NONE; i < RESULT_MAX; i++ )
  {
    if ( ( i > 0 && mask < 0 ) || ( mask & ( int64_t( 1 ) << i ) ) )
    {
      spell_callbacks[ i ].push_back( cb );
      heal_callbacks[ i ].push_back( cb );
    }
  }
}

// player_t::register_tick_callback =========================================

void player_t::register_tick_callback( int64_t mask,
                                       action_callback_t* cb )
{
  for ( result_type i = RESULT_NONE; i < RESULT_MAX; i++ )
  {
    if ( ( i > 0 && mask < 0 ) || ( mask & ( int64_t( 1 ) << i ) ) )
    {
      tick_callbacks[ i ].push_back( cb );
    }
  }
}

// player_t::register_heal_callback =========================================

void player_t::register_heal_callback( int64_t mask,
                                       action_callback_t* cb )
{
  for ( result_type i = RESULT_NONE; i < RESULT_MAX; i++ )
  {
    if ( ( i > 0 && mask < 0 ) || ( mask & ( int64_t( 1 ) << i ) ) )
    {
      heal_callbacks[ i ].push_back( cb );
    }
  }
}

// player_t::register_harmful_spell_callback ================================

void player_t::register_harmful_spell_callback( int64_t mask,
                                                action_callback_t* cb )
{
  for ( result_type i = RESULT_NONE; i < RESULT_MAX; i++ )
  {
    if ( ( i > 0 && mask < 0 ) || ( mask & ( int64_t( 1 ) << i ) ) )
    {
      spell_callbacks[ i ].push_back( cb );
    }
  }
}

// player_t::register_tick_damage_callback ==================================

void player_t::register_tick_damage_callback( int64_t mask,
                                              action_callback_t* cb )
{
  for ( school_type i=SCHOOL_NONE; i < SCHOOL_MAX; ++i )
  {
    if ( mask < 0 || ( mask & bitmask( i ) ) != 0 )
    {
      tick_damage_callbacks[ i ].push_back( cb );
    }
  }
}

// player_t::register_direct_damage_callback ================================

void player_t::register_direct_damage_callback( int64_t mask,
                                                action_callback_t* cb )
{
  for ( school_type i=SCHOOL_NONE; i < SCHOOL_MAX; i++ )
  {
    if ( mask < 0 || ( mask & bitmask( i ) ) != 0 )
    {
      direct_damage_callbacks[ i ].push_back( cb );
    }
  }
}

// player_t::register_tick_heal_callback ====================================

void player_t::register_tick_heal_callback( int64_t mask,
                                            action_callback_t* cb )
{
  for ( school_type i = SCHOOL_NONE; i < SCHOOL_MAX; i++ )
  {
    if ( mask < 0 || ( mask & bitmask( i ) ) != 0 )
    {
      tick_heal_callbacks[ i ].push_back( cb );
    }
  }
}

// player_t::register_direct_heal_callback ==================================

void player_t::register_direct_heal_callback( int64_t mask,
                                              action_callback_t* cb )
{
  for ( school_type i = SCHOOL_NONE; i < SCHOOL_MAX; i++ )
  {
    if ( mask < 0 || ( mask & bitmask( i ) ) != 0 )
    {
      direct_heal_callbacks[ i ].push_back( cb );
    }
  }
}

// player_t::alacrity =======================================================

double player_t::alacrity() const
{ return 1.0 + computed_alacrity; }

// player_t::find_action ====================================================

action_t* player_t::find_action( const std::string& str )
{
  for ( action_t* a = action_list; a; a = a -> next )
    if ( str == a -> name_str )
      return a;

  return 0;
}

// player_t::aura_gain ======================================================

void player_t::aura_gain( const std::string& aura_name , double value )
{
  if ( sim -> log && ! sleeping )
  {
    log_t::output( sim, "%s gains %s ( value=%.2f )", name(), aura_name.c_str(), value );
  }
}

// player_t::aura_loss ======================================================

void player_t::aura_loss( const std::string& aura_name , double /* value */ )
{
  if ( sim -> log && ! sleeping )
  {
    log_t::output( sim, "%s loses %s", name(), aura_name.c_str() );
  }
}

// player_t::find_cooldown ==================================================

cooldown_t* player_t::find_cooldown( const std::string& name ) const
{
  for ( cooldown_t* c = cooldown_list; c; c = c -> next )
  {
    if ( c -> name_str == name )
      return c;
  }

  return 0;
}

// player_t::find_dot =======================================================

dot_t* player_t::find_dot( const std::string& name ) const
{
  for ( dot_t* d = dot_list; d; d = d -> next )
  {
    if ( d -> name_str == name )
      return d;
  }

  return 0;
}

// player_t::find_action_priority_list( const std::string& name ) ===========

action_priority_list_t* player_t::find_action_priority_list( const std::string& name ) const
{
  for ( size_t i = 0; i < action_priority_list.size(); i++ )
  {
    action_priority_list_t* a = action_priority_list[i];
    if ( a -> name_str == name )
      return a;
  }

  return 0;
}

// player_t::find_set_bonus =======================================================

set_bonus_t* player_t::find_set_bonus( const std::string& name ) const
{
  set_bonus_t* sb=0;

  for ( sb = set_bonus_list; sb; sb = sb -> next )
  {
    if ( sb -> name == name )
      return sb;
  }

  return 0;
}

// player_t::find_stats ======================================================

stats_t* player_t::find_stats( const std::string& n )
{
  stats_t* stats = 0;

  for ( stats = stats_list; stats; stats = stats -> next )
  {
    if ( stats -> name_str == n )
      break;
  }

  return stats;
}

// player_t::get_cooldown ===================================================

cooldown_t* player_t::get_cooldown( const std::string& name )
{
  cooldown_t* c=0;

  for ( c = cooldown_list; c; c = c -> next )
  {
    if ( c -> name_str == name )
      return c;
  }

  c = new cooldown_t( name, this );

  cooldown_t** tail = &cooldown_list;

  while ( *tail && name > ( ( *tail ) -> name_str ) )
  {
    tail = &( ( *tail ) -> next );
  }

  c -> next = *tail;
  *tail = c;

  return c;
}

// player_t::get_dot ========================================================

dot_t* player_t::get_dot( const std::string& name )
{
  dot_t* d=0;

  for ( d = dot_list; d; d = d -> next )
  {
    if ( d -> name_str == name )
      return d;
  }

  d = new dot_t( name, this );

  dot_t** tail = &dot_list;

  while ( *tail && name > ( ( *tail ) -> name_str ) )
  {
    tail = &( ( *tail ) -> next );
  }

  d -> next = *tail;
  *tail = d;

  return d;
}

// player_t::get_gain =======================================================

gain_t* player_t::get_gain( const std::string& name )
{
  gain_t* g=0;

  for ( g = gain_list; g; g = g -> next )
  {
    if ( g -> name_str == name )
      return g;
  }

  g = new gain_t( name );

  gain_t** tail = &gain_list;

  while ( *tail && name > ( ( *tail ) -> name_str ) )
  {
    tail = &( ( *tail ) -> next );
  }

  g -> next = *tail;
  *tail = g;

  return g;
}

// player_t::get_proc =======================================================

proc_t* player_t::get_proc( const std::string& name )
{
  proc_t* p=0;

  for ( p = proc_list; p; p = p -> next )
  {
    if ( p -> name_str == name )
      return p;
  }

  p = new proc_t( sim, name );

  proc_t** tail = &proc_list;

  while ( *tail && name > ( ( *tail ) -> name_str ) )
  {
    tail = &( ( *tail ) -> next );
  }

  p -> next = *tail;
  *tail = p;

  p -> player = this;

  return p;
}

// player_t::get_stats ======================================================

stats_t* player_t::get_stats( const std::string& n, action_t* a )
{
  stats_t* stats;

  for ( stats = stats_list; stats; stats = stats -> next )
  {
    if ( stats -> name_str == n )
      break;
  }

  if ( ! stats )
  {
    stats = new stats_t( n, this );

    stats_t** tail= &stats_list;
    while ( *tail && n > ( ( *tail ) -> name_str ) )
    {
      tail = &( ( *tail ) -> next );
    }
    stats -> next = *tail;
    *tail = stats;
  }

  assert( stats -> player == this );

  if ( a ) stats -> action_list.push_back( a );
  return stats;
}

// player_t::get_benefit =====================================================

benefit_t* player_t::get_benefit( const std::string& name )
{
  benefit_t* u=0;

  for ( u = benefit_list; u; u = u -> next )
  {
    if ( u -> name_str == name )
      return u;
  }

  u = new benefit_t( name );

  benefit_t** tail = &benefit_list;

  while ( *tail && name > ( ( *tail ) -> name_str ) )
  {
    tail = &( ( *tail ) -> next );
  }

  u -> next = *tail;
  *tail = u;

  return u;
}

// player_t::get_uptime =====================================================

uptime_t* player_t::get_uptime( const std::string& name )
{
  for ( uptime_t* u = uptime_list; u; u = u -> next )
  {
    if ( u -> name_str == name )
      return u;
  }

  uptime_t* u = new uptime_t( sim, name );

  uptime_t** tail = &uptime_list;

  while ( *tail && name > ( ( *tail ) -> name_str ) )
  {
    tail = &( ( *tail ) -> next );
  }

  u -> next = *tail;
  *tail = u;

  return u;
}

// player_t::get_rng ========================================================

rng_t* player_t::get_rng( const std::string& n, rng_type type )
{
  assert( sim -> rng );

  if ( type == RNG_GLOBAL ) return get_pointer( sim -> rng );
  if ( type == RNG_DETERMINISTIC ) return get_pointer( sim -> deterministic_rng );

  if ( ! sim -> smooth_rng ) return sim -> default_rng();

  for ( rng_t* rng = rng_list; rng; rng = rng -> next )
  {
    if ( rng -> name_str == n )
      return rng;
  }

  rng_t* rng = rng_t::create( sim, n, type );
  rng -> next = rng_list;
  rng_list = rng;
  return rng;
}

// player_t::get_set_bonus =======================================================

set_bonus_t* player_t::get_set_bonus( const std::string& name, std::string shell_filter, std::string armoring_filter, slot_mask_t slot_filter )
{
  set_bonus_t* sb=find_set_bonus( name );

  if ( ! sb )
  {
    sb = new set_bonus_t( name, shell_filter, armoring_filter, slot_filter );

    set_bonus_t** tail = &set_bonus_list;

    while ( *tail && name > ( ( *tail ) -> name ) )
    {
      tail = &( ( *tail ) -> next );
    }

    sb -> next = *tail;
    *tail = sb;
  }

  return sb;
}

// player_t::get_armoring_set_bonus_name ========================================

std::string player_t::get_armoring_set_bonus_name( const std::string armoring_name )
{
  set_bonus_t* sb=0;

  for ( sb = set_bonus_list; sb; sb = sb -> next)
    {
      for ( std::string armoring_filter: sb -> armoring_filters )
        {
          if ( armoring_name.find( armoring_filter ) != armoring_name.npos )
            {
              return sb -> name;
            }
        }
    }

  return "";
}

// player_t::get_shell_set_bonus_name ========================================

std::string player_t::get_shell_set_bonus_name( const std::string shell_name )
{
  set_bonus_t* sb=0;

  for ( sb = set_bonus_list; sb; sb = sb -> next)
    {
      for ( std::string shell_filter: sb -> shell_filters )
        {
          if ( shell_name.find( shell_filter ) != shell_name.npos )
            {
              return sb -> name;
            }
        }
    }

  return "";
}

// player_t::get_position_distance ==========================================

double player_t::get_position_distance( double m, double v ) const
{
  // Square of Euclidean distance since sqrt() is slow
  double delta_x = this -> x_position - m;
  double delta_y = this -> y_position - v;
  return delta_x * delta_x + delta_y * delta_y;
}

// player_t::get_player_distance ============================================

double player_t::get_player_distance( const player_t* p ) const
{ return get_position_distance( p -> x_position, p -> y_position ); }

// player_t::get_action_priority_list( const std::string& name ) ============

action_priority_list_t* player_t::get_action_priority_list( const std::string& name )
{
  action_priority_list_t* a = find_action_priority_list( name );
  if ( ! a )
  {
    a = new action_priority_list_t( name, this );
    action_priority_list.push_back( a );
  }
  return a;
}

// player_t::debuffs_t::snared ==============================================

bool player_t::debuffs_t::snared()
{
  return false;
}

// Summon Pet Spell =========================================================

struct summon_companion_t : public action_t
{
  timespan_t summoning_duration;
  pet_t* pet;

private:
  void _init_summon_companion_t( const std::string& pet_name )
  {
    harmful = false;

    pet = player -> find_pet( pet_name );
    if ( ! pet )
    {
      sim -> errorf( "Player %s unable to find pet %s for summons.\n", player -> name(), pet_name.c_str() );
      sim -> cancel();
    }
  }

public:
  summon_companion_t( player_t* p, const std::string& options_str ) :
    action_t( ACTION_OTHER, "summon_companion", p ), summoning_duration ( timespan_t::zero() ), pet( 0 )
  {
    std::string pet_name;
    option_t options[] =
    {
      { "name", OPT_STRING,  &pet_name     },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    _init_summon_companion_t( pet_name );
  }

  virtual void execute()
  {
    pet -> summon( timespan_t::zero() );

    player -> active_companion = pet;

    action_t::execute();
  }

  virtual void calculate_result()
  {
    result = RESULT_NONE;
  }

  virtual void schedule_execute()
  {
    action_t::schedule_execute();

    if ( player -> active_companion )
      player -> active_companion -> dismiss();
  }

  virtual bool ready()
  {
    if ( player -> active_companion == pet )
      return false;

    return action_t::ready();
  }

};

// Chosen Movement Actions ==================================================

struct start_moving_t : public action_t
{
  start_moving_t( player_t* player, const std::string& options_str ) :
    action_t( ACTION_OTHER, "start_moving", player )
  {
    parse_options( NULL, options_str );
    trigger_gcd = timespan_t::zero();
    cooldown -> duration = from_seconds( 0.5 );
    harmful = false;
  }

  virtual void execute()
  {
    player -> buffs.self_movement -> trigger();

    if ( sim -> log ) log_t::output( sim, "%s starts moving.", player -> name() );
    update_ready();
  }

  virtual bool ready()
  {
    if ( player -> buffs.self_movement -> check() )
      return false;

    return action_t::ready();
  }
};

struct stop_moving_t : public action_t
{
  stop_moving_t( player_t* player, const std::string& options_str ) :
    action_t( ACTION_OTHER, "stop_moving", player )
  {
    parse_options( NULL, options_str );
    trigger_gcd = timespan_t::zero();
    cooldown -> duration = from_seconds( 0.5 );
    harmful = false;
  }

  virtual void execute()
  {
    player -> buffs.self_movement -> expire();

    if ( sim -> log ) log_t::output( sim, "%s stops moving.", player -> name() );
    update_ready();
  }

  virtual bool ready()
  {
    if ( ! player -> buffs.self_movement -> check() )
      return false;

    return action_t::ready();
  }
};

// ===== Racial Abilities ===================================================


// Cycle Action =============================================================

struct cycle_t : public action_t
{
  action_t* current_action;

  cycle_t( player_t* player, const std::string& options_str ) :
    action_t( ACTION_OTHER, "cycle", player ), current_action( 0 )
  {
    parse_options( NULL, options_str );
  }

  virtual void reset()
  {
    action_t::reset();

    if ( ! current_action )
    {
      current_action = next;
      if ( ! current_action )
      {
        sim -> errorf( "Player %s has no actions after 'cycle'\n", player -> name() );
        sim -> cancel();
      }
      for ( action_t* a = next; a; a = a -> next ) a -> background = true;
    }
  }

  virtual void schedule_execute()
  {
    player -> last_foreground_action = current_action;
    current_action -> schedule_execute();
    current_action = current_action -> next;
    if ( ! current_action ) current_action = next;
  }

  virtual bool ready()
  {
    if ( ! current_action ) return false;

    return current_action -> ready();
  }
};

// Restart Sequence Action ==================================================

struct restart_sequence_t : public action_t
{
  sequence_t* seq;
  std::string seq_name_str;

  restart_sequence_t( player_t* player, const std::string& options_str ) :
    action_t( ACTION_OTHER, "restart_sequence", player ),
    seq( 0 ), seq_name_str( "default" ) // matches default name for sequences
  {
    option_t options[] =
    {
      { "name", OPT_STRING,  &seq_name_str },
      { NULL,   OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = timespan_t::zero();
  }

  virtual void execute()
  {
    if ( ! seq )
    {
      for ( action_t* a = player -> action_list; a && ! seq; a = a -> next )
      {
        if ( a -> type != ACTION_SEQUENCE )
          continue;

        if ( ! seq_name_str.empty() )
          if ( seq_name_str != a -> name_str )
            continue;

        seq = dynamic_cast<sequence_t*>( a );
      }

      if ( !seq )
      {
        sim -> errorf( "Can't find sequence %s\n",
                       seq_name_str.empty() ? "(default)" : seq_name_str.c_str() );
        sim -> cancel();
        return;
      }
    }

    seq -> restart();
  }

  virtual bool ready()
  {
    if ( seq ) return seq -> can_restart();
    return action_t::ready();
  }
};

#if 0
// Restore Mana Action ======================================================

struct restore_mana_t : public action_t
{
  double mana;

  restore_mana_t( player_t* player, const std::string& options_str ) :
    action_t( ACTION_OTHER, "restore_mana", player ), mana( 0 )
  {
    option_t options[] =
    {
      { "mana", OPT_FLT, &mana },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = timespan_t::zero();
  }

  virtual void execute()
  {
    double mana_missing = player -> resource_max[ RESOURCE_MANA ] - player -> resource_current[ RESOURCE_MANA ];
    double mana_gain = mana;

    if ( mana_gain == 0 || mana_gain > mana_missing ) mana_gain = mana_missing;

    if ( mana_gain > 0 )
    {
      player -> resource_gain( RESOURCE_MANA, mana_gain, player -> gains.restore_mana );
    }
  }
};
#endif

// Snapshot Stats ===========================================================

struct snapshot_stats_t : public action_t
{
  bool completed;

  snapshot_stats_t( player_t* player, const std::string& options_str ) :
    action_t( ACTION_OTHER, "snapshot_stats", player ),
    completed( false )
  {
    parse_options( NULL, options_str );
    trigger_gcd = timespan_t::zero();
  }

  virtual void execute()
  {
    player_t* p = player;

    if ( sim -> log ) log_t::output( sim, "%s performs %s", p -> name(), name() );

    p -> buffed.alacrity  = p -> alacrity();

    for ( int i=ATTRIBUTE_NONE; i < ATTRIBUTE_MAX; ++i )
      p -> buffed.attribute[ i ] = floor( p -> get_attribute( static_cast<attribute_type>( i ) ) );

    boost::copy( p -> resource_max, p -> buffed.resource );

    p -> buffed.power             = p -> composite_power();
    p -> buffed.force_power       = p -> composite_force_power();
    p -> buffed.tech_power        = p -> composite_tech_power();

    p -> buffed.surge             = p -> surge_bonus;

    p -> buffed.melee_accuracy    = p -> melee_accuracy_chance();
    p -> buffed.melee_crit        = p -> melee_crit_chance();
    p -> buffed.melee_avoidance   = p -> melee_avoidance();

    p -> buffed.range_accuracy    = p -> range_accuracy_chance();
    p -> buffed.range_crit        = p -> range_crit_chance();
    p -> buffed.range_avoidance   = p -> range_avoidance();

    p -> buffed.force_accuracy    = p -> force_accuracy_chance();
    p -> buffed.force_crit        = p -> force_crit_chance();
    p -> buffed.force_avoidance   = p -> force_avoidance();

    p -> buffed.tech_accuracy     = p -> tech_accuracy_chance();
    p -> buffed.tech_crit         = p -> tech_crit_chance();
    p -> buffed.tech_avoidance    = p -> tech_avoidance();

    p -> buffed.armor       = p -> composite_armor();
    p -> buffed.armor_penetration_debuff = p -> armor_penetration_debuff();

    p -> buffed.shield_chance = p -> shield_chance();
    p -> buffed.shield_absorb = p -> shield_absorb();

    p -> buffed.melee_damage_bonus  = p -> melee_damage_bonus();
    p -> buffed.range_damage_bonus  = p -> range_damage_bonus();
    p -> buffed.force_damage_bonus  = p -> force_damage_bonus();
    p -> buffed.tech_damage_bonus   = p -> tech_damage_bonus();
    p -> buffed.force_healing_bonus = p -> force_healing_bonus();
    p -> buffed.tech_healing_bonus  = p -> tech_healing_bonus();


    completed = true;
  }

  virtual bool ready()
  {
    if ( sim -> current_iteration > 0 ) return false;
    if ( completed ) return false;
    return action_t::ready();
  }
};

// Wait Fixed Action ========================================================

struct wait_fixed_t : public wait_action_base_t
{
  expr_ptr time_expr;
  std::string wait_str;

  wait_fixed_t( player_t* player, const std::string& options_str ) :
    wait_action_base_t( player, "wait" ), wait_str( "1.0" )
  {
    option_t options[] =
    {
      { "sec", OPT_STRING, &wait_str },
      { NULL,  OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    time_expr = expr_t::parse( this, wait_str );
  }

  virtual timespan_t execute_time() const
  {
    timespan_t wait = from_seconds( time_expr -> evaluate() );
    if ( wait > timespan_t::zero() )
      return wait;
    else
      return player -> available();
  }
};

// Wait Until Ready Action ==================================================

struct wait_until_ready_t : public wait_fixed_t
{
  wait_until_ready_t( player_t* player, const std::string& options_str ) :
    wait_fixed_t( player, options_str )
  {}

  virtual timespan_t execute_time() const
  {
    timespan_t wait = wait_fixed_t::execute_time();
    timespan_t remains = timespan_t::zero();

    for ( action_t* a = player -> action_list; a; a = a -> next )
    {
      if ( a -> background ) continue;

      remains = a -> cooldown -> remains();
      if ( remains > timespan_t::zero() && remains < wait ) wait = remains;

      remains = a -> dot() -> remains();
      if ( remains > timespan_t::zero() && remains < wait ) wait = remains;
    }

    if ( wait <= timespan_t::zero() ) wait = player -> available();

    return wait;
  }
};

// Wait For Cooldown Action =================================================

wait_for_cooldown_t::wait_for_cooldown_t( player_t* player, const char* cd_name ) :
  wait_action_base_t( player, ( "wait_for_" + std::string( cd_name ) ).c_str() ),
  wait_cd( player -> get_cooldown( cd_name ) ), a( player -> find_action( cd_name ) )
{
  assert( a );
}

timespan_t wait_for_cooldown_t::execute_time() const
{ return wait_cd -> remains(); }

// Use Item Actions =========================================================

struct base_use_item_t : public action_t
{
  item_t* item;
  action_t* discharge;
  action_callback_t* trigger;
  stat_buff_t* buff;
  std::string use_name;

  base_use_item_t( player_t* player ) :
    action_t( ACTION_OTHER, "use_item", player ),
    item(), discharge(), trigger(), buff()
  {}

  virtual void init()
  {
    if ( ! item )
      return;

    if ( ! item -> use.active() )
    {
      sim -> errorf( "Player %s attempting 'use_item' action with item '%s' which has no 'use=' encoding.\n", player -> name(), item->name() );
      item = 0;
      return;
    }

    name_str += '_';
    name_str += item -> name();

    stats = player -> get_stats( name_str, this );

    item_t::special_effect_t& e = item -> use;

    use_name = e.name_str.empty() ? item -> name() : e.name_str;

    if ( e.trigger_type )
    {
      if ( e.cost_reduction && e.school && e.discharge_amount )
      {
        trigger = unique_gear_t::register_cost_reduction_proc( e.trigger_type, e.trigger_mask, use_name, player,
                                                               e.school, e.max_stacks, e.discharge_amount,
                                                               e.proc_chance, timespan_t::zero()/*dur*/, timespan_t::zero()/*cd*/, false, e.reverse );
      }
      else if ( e.stat )
      {
        trigger = unique_gear_t::register_stat_proc( e.trigger_type, e.trigger_mask, use_name, player,
                                                     e.stat, e.max_stacks, e.stat_amount,
                                                     e.proc_chance, timespan_t::zero()/*dur*/, timespan_t::zero()/*cd*/, e.tick, e.reverse );
      }
      else if ( e.school )
      {
        trigger = unique_gear_t::register_discharge_proc( e.trigger_type, e.trigger_mask, use_name, player,
                                                          e.max_stacks, e.school, e.policy, e.discharge_amount, e.discharge_scaling,
                                                          e.proc_chance, timespan_t::zero()/*cd*/, e.no_crit, e.no_player_benefits, e.no_debuffs );
      }

      if ( trigger ) trigger -> deactivate();
    }
    else if ( e.school )
    {
      struct discharge_spell_t : public action_t
      {
        discharge_spell_t( const std::string& n, player_t* p, double a, const school_type s ) :
          action_t( ACTION_ATTACK, n, p, force_policy, RESOURCE_NONE, s )
        {
          trigger_gcd = timespan_t::zero();
          dd.base_min = dd.base_max = a;
          may_crit    = true;
          background  = true;
          init();
        }
      };

      discharge = new discharge_spell_t( use_name, player, e.discharge_amount, e.school );
    }
    else if ( e.stat )
    {
      if ( e.max_stacks  == 0 ) e.max_stacks  = 1;
      if ( e.proc_chance == 0 ) e.proc_chance = 1;

      buff = new stat_buff_t( player, use_name, e.stat, e.stat_amount, e.max_stacks, e.duration, timespan_t::zero(), e.proc_chance, false, e.reverse );
    }
    else assert( false );

    std::string cooldown_name = use_name + '_' + item -> slot_name();

    cooldown = player -> get_cooldown( cooldown_name );
    cooldown -> duration = item -> use.cooldown;
    trigger_gcd = timespan_t::zero();

    if ( buff != 0 ) buff -> cooldown = cooldown;
  }

  void lockout( timespan_t duration )
  {
    if ( duration <= timespan_t::zero() ) return;
    timespan_t ready = sim -> current_time + duration;
    for ( action_t* a = player -> action_list; a; a = a -> next )
    {
      if ( a -> name_str.substr(0, 8) == "use_item" )
      {
        if ( ready > a -> cooldown -> ready )
        {
          a -> cooldown -> ready = ready;
        }
      }
    }
  }

  virtual void execute()
  {
    if ( discharge )
    {
      discharge -> execute();
    }

    else if ( trigger )
    {
      if ( sim -> log ) log_t::output( sim, "%s performs %s", player -> name(), use_name.c_str() );

      trigger -> activate();

      if ( item -> use.duration != timespan_t::zero() )
      {
        struct trigger_expiration_t : public event_t
        {
          action_callback_t* trigger;

          trigger_expiration_t( player_t* player, item_t* item, action_callback_t* t ) :
            event_t( player, item -> name() ), trigger( t )
          { sim -> add_event( this, item -> use.duration ); }

          virtual void execute()
          { trigger -> deactivate(); }
        };

        new ( sim ) trigger_expiration_t( player, item, trigger );

        lockout( item -> use.duration );
      }
    }

    else if ( buff )
    {
      if ( sim -> log ) log_t::output( sim, "%s performs %s", player -> name(), use_name.c_str() );
      buff -> trigger();
      lockout( buff -> buff_duration );
    }

    else assert( false );

    // Enable to report use_item ability
    //if ( ! dual ) stats -> add_execute( time_to_execute );

    update_ready();
  }

  virtual void reset()
  {
    action_t::reset();
    if ( trigger ) trigger -> deactivate();
  }

  virtual bool ready()
  {
    if ( ! item ) return false;
    return action_t::ready();
  }
};

struct use_item_t : public base_use_item_t
{
  use_item_t( player_t* player, const std::string& options_str ) :
    base_use_item_t( player )
  {
    std::string item_name;
    option_t options[] =
    {
      { "name", OPT_STRING, &item_name },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    if ( item_name.empty() )
    {
      sim -> errorf( "Player %s has 'use_item' action with no 'name=' option.\n", player -> name() );
      return;
    }

    item = player -> find_item( item_name );
    if ( ! item )
    {
      sim -> errorf( "Player %s attempting 'use_item' action with item '%s' which is not currently equipped.\n", player -> name(), item_name.c_str() );
      return;
    }
    if ( ! item -> use.active() )
    {
      sim -> errorf( "Player %s attempting 'use_item' action with item '%s' which has no 'use=' encoding.\n", player -> name(), item_name.c_str() );
      item = 0;
      return;
    }
  }
};

struct use_slot_t : public base_use_item_t
{
  use_slot_t( player_t* player, const std::string& options_str ) :
    base_use_item_t( player )
  {
    std::string slot_name;
    int quiet = 0;

    option_t options[] =
    {
      { "quiet", OPT_BOOL,   &quiet },
      { "slot",  OPT_STRING, &slot_name },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    if ( slot_name.empty() )
    {
      sim -> errorf( "Player %s has 'use_slot' action with no 'slot=' option.\n", player -> name() );
      return;
    }

    int slot = util_t::parse_slot_type( slot_name );
    if ( slot == SLOT_INVALID )
    {
      if ( ! quiet )
        sim -> errorf( "Player %s has 'use_slot' action with invalid slot name '%s'.\n", player -> name(), slot_name.c_str() );
      return;
    }

    for ( size_t i = 0; i < player -> items.size(); ++i )
    {
      if ( player -> items[ i ].slot == slot )
      {
        item = &player -> items[ i ];
        break;
      }
    }

    if ( ! item )
    {
      if ( ! quiet )
      sim -> errorf( "Player %s attempting 'use_slot' action for slot '%s' with no item equipped.\n", player -> name(), slot_name.c_str() );
      return;
    }

    if ( ! item -> use.active() )
    {
      if ( !quiet )
        sim -> errorf( "Player %s attempting 'use_slot' action with item '%s' which has no 'use=' encoding.\n", player -> name(), item -> name() );
      item = 0;
      return;
    }
  }
};


struct use_relics_t : public action_t
{
  use_slot_t *relic1, *relic2;

  use_relics_t( player_t* player, const std::string& options_str ) :
    action_t( ACTION_OTHER, "use_relics", player ),
    relic1( new use_slot_t( player, "slot=relic1,quiet=1" ) ),
    relic2( new use_slot_t( player, "slot=relic2,quiet=1" ) )
  {
    parse_options( 0, options_str );

    harmful = false;
    trigger_gcd = timespan_t::zero();
  }

  virtual void execute()
  {
    if ( relic1 -> ready() )
      relic1 -> execute();
    else if ( relic2 -> ready() )
      relic2 -> execute();
  }

  virtual bool ready()
  {
    if ( ! relic1 -> ready() && ! relic2 -> ready () )
      return false;
    return action_t::ready();
  }
};

// Cancel Buff ==============================================================

struct cancel_buff_t : public action_t
{
  buff_t* buff;

  cancel_buff_t( player_t* player, const std::string& options_str ) :
    action_t( ACTION_OTHER, "cancel_buff", player ), buff( 0 )
  {
    std::string buff_name;
    option_t options[] =
    {
      { "name", OPT_STRING, &buff_name },
      { NULL,  OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    if ( buff_name.empty() )
    {
      sim -> errorf( "Player %s uses cancel_buff without specifying the name of the buff\n", player -> name() );
      sim -> cancel();
    }

    buff = buff_t::find( player, buff_name );

    if ( ! buff )
    {
      sim -> errorf( "Player %s uses cancel_buff with unknown buff %s\n", player -> name(), buff_name.c_str() );
      sim -> cancel();
    }
    trigger_gcd = timespan_t::zero();
  }

  virtual void execute()
  {
    if ( sim -> log ) log_t::output( sim, "%s cancels buff %s", player -> name(), buff -> name() );
    buff -> expire();
  }

  virtual bool ready()
  {
    if ( ! buff || ! buff -> check() )
      return false;
    return action_t::ready();
  }
};

// player_t::create_action ==================================================

action_t* player_t::create_action( const std::string& name,
                                   const std::string& options_str )
{
  if ( name == "cancel_buff"      ) return new      cancel_buff_t( this, options_str );
  if ( name == "cycle"            ) return new            cycle_t( this, options_str );
  if ( name == "restart_sequence" ) return new restart_sequence_t( this, options_str );
  //if ( name == "restore_mana"     ) return new     restore_mana_t( this, options_str );
  if ( name == "sequence"         ) return new         sequence_t( this, options_str );
  if ( name == "snapshot_stats"   ) return new   snapshot_stats_t( this, options_str );
  if ( name == "start_moving"     ) return new     start_moving_t( this, options_str );
  if ( name == "stop_moving"      ) return new      stop_moving_t( this, options_str );
  if ( name == "use_item"         ) return new         use_item_t( this, options_str );
  if ( name == "use_slot"         ) return new         use_slot_t( this, options_str );
  if ( name == "use_relics"       ) return new       use_relics_t( this, options_str );
  if ( name == "wait"             ) return new       wait_fixed_t( this, options_str );
  if ( name == "wait_until_ready" ) return new wait_until_ready_t( this, options_str );
  if ( name == "summon_companion" ) return new summon_companion_t( this, options_str );


  return consumable_t::create_action( this, name, options_str );
}

// player_t::find_pet =======================================================

pet_t* player_t::find_pet( const std::string& pet_name )
{
  for ( pet_t* p = pet_list; p; p = p -> next_pet )
    if ( p -> name_str == pet_name )
      return p;

  return 0;
}

// player_t::parse_talent_trees =============================================

bool player_t::parse_talent_trees( const int encoding[ MAX_TALENT_SLOTS ] )
{
  int index=0;

  for ( int i=0; i < MAX_TALENT_TREES; i++ )
  {
    size_t tree_size = talent_trees[ i ].size();

    for ( size_t j=0; j < tree_size; j++ )
    {
      talent_trees[ i ][ j ] -> set_rank( encoding[ index++ ] );
    }
  }

  return true;
}

// player_t::parse_talents_armory ===========================================

bool player_t::parse_talents_armory( const std::string& talent_string )
{
  int encoding[ MAX_TALENT_SLOTS ];

  size_t i;
  size_t i_max = std::min( talent_string.size(),
                           static_cast< size_t >( MAX_TALENT_SLOTS ) );
  for ( i = 0; i < i_max; i++ )
  {
    char c = talent_string[ i ];
    if ( c < '0' || c > '5' )
    {
      sim -> errorf( "Player %s has illegal character '%c' in talent encoding.\n", name(), c );
      return false;
    }
    encoding[ i ] = c - '0';
  }

  while ( i < MAX_TALENT_SLOTS ) encoding[ i++ ] = 0;

  return parse_talent_trees( encoding );
}

// player_t::find_talent ====================================================

talent_t* player_t::find_talent( const std::string& n,
                                 talent_tab_type tree ) const
{
  for ( int i=0; i < MAX_TALENT_TREES; i++ )
  {
    if ( tree != TALENT_TAB_NONE && tree != i )
      continue;

    size_t size=talent_trees[ i ].size();
    for ( size_t j=0; j < size; j++ )
    {
      talent_t* t = talent_trees[ i ][ j ];

      if ( n == t -> name() )
      {
        return t;
      }
    }
  }

  sim -> errorf( "Player %s unable to find talent %s\n", name(), n.c_str() );
  return 0;
}

// player_t::create_expression ==============================================

namespace {
expr_ptr deprecate_expression( player_t* p, action_t* a, const std::string& old_name, const std::string& new_name )
{
  p -> sim -> errorf( "Use of \"%s\" in action expressions is deprecated: use \"%s\" instead.\n",
                      old_name.c_str(), new_name.c_str() );
  return p -> create_expression( a, new_name );
}
}

expr_ptr player_t::create_expression( action_t* a,
                                      const std::string& name_str )
{
  if ( name_str == "health_pct" )
    return deprecate_expression( this, a, name_str, "health.pct" );

  if ( name_str == "energy_regen" )
    return deprecate_expression( this, a, name_str, "energy.regen" );

  if ( name_str == "ammo_regen" )
    return deprecate_expression( this, a, name_str, "ammo.regen" );

  if ( name_str == "time_to_max_energy" )
    return deprecate_expression( this, a, name_str, "energy.time_to_max" );

  if ( name_str == "time_to_max_ammo" )
    return deprecate_expression( this, a, name_str, "ammo.time_to_max" );

  if ( name_str == "time_to_max_force" )
    return deprecate_expression( this, a, name_str, "force.time_to_max" );

  resource_type rt = static_cast<resource_type>( util_t::parse_resource_type( name_str ) );
  if ( rt != RESOURCE_NONE )
    return make_expr( name_str, [this,rt]{ return resource_current[ rt ]; } );

  if ( name_str == "level" )
    return make_expr( name_str, [this]{ return level; } );

  if ( name_str == "multiplier" )
    return make_expr( name_str, [this,a]{ return composite_player_multiplier( a -> school, a ); } );

  if ( name_str == "in_combat" )
    return make_expr( name_str, [this]{ return in_combat != 0; } );

  if ( name_str == "alacrity" )
    return make_expr( name_str, [this]{ return alacrity(); } );

  if ( name_str == "time_to_die" )
    return make_expr( name_str, [this]{ return to_seconds( time_to_die() ); } );

  if ( name_str == "ptr" )
    return make_expr( name_str, [this]{ return ptr; } );

  if ( name_str == "position_front" )
    return make_expr( name_str, [this]{ return position == POSITION_FRONT ||
                                               position == POSITION_RANGED_FRONT; } );

  if ( name_str == "position_back" )
    return make_expr( name_str, [this]{ return position == POSITION_BACK ||
                                               position == POSITION_RANGED_BACK; } );

  std::vector<std::string> splits;
  int num_splits = util_t::string_split( splits, name_str, "." );

  if ( num_splits == 2 && ( rt = static_cast<resource_type>( util_t::parse_resource_type( splits[ 0 ] ) ) ) != RESOURCE_NONE )
  {
    if ( splits[ 1 ] == "deficit" )
      return make_expr( name_str, [this,rt]{ return resource_max[ rt ] - resource_current[ rt ]; } );

    else if ( splits[ 1 ] == "pct" || splits[ 1 ] == "percent" )
    {
      if ( rt == RESOURCE_HEALTH )
        return make_expr( name_str, [this]{ return health_percentage() / 100.0; } );
      else
        return make_expr( name_str, [this,rt]{ return resource_current[ rt ] / resource_max[ rt ]; } );
    }

    else if ( splits[ 1 ] == "max" )
      return make_expr( name_str, [this,rt]{ return resource_max[ rt ]; } );

    else if ( splits[ 1 ] == "regen" )
    {
      if ( rt == RESOURCE_FORCE )
        return make_expr( name_str, [this]{ return force_regen_per_second(); } );
      if ( rt == RESOURCE_ENERGY )
        return make_expr( name_str, [this]{ return energy_regen_per_second(); } );
      if ( rt == RESOURCE_AMMO )
        return make_expr( name_str, [this]{ return ammo_regen_per_second(); } );
      if ( rt == RESOURCE_HEAT )
        return make_expr( name_str, [this]{ return heat_regen_per_second(); } );
    }

    else if ( splits[ 1 ] == "time_to_max" )
    {
      if ( rt == RESOURCE_FORCE )
        return make_expr( name_str, [this]{ return ( resource_used_amount( RESOURCE_FORCE ) ) / force_regen_per_second(); } );
      if ( rt == RESOURCE_ENERGY )
        return make_expr( name_str, [this]{ return ( resource_used_amount( RESOURCE_ENERGY ) ) / energy_regen_per_second(); } );
      if ( rt == RESOURCE_AMMO )
        return make_expr( name_str, [this]{ return ( resource_used_amount( RESOURCE_AMMO ) ) / ammo_regen_per_second(); } );
      if ( rt == RESOURCE_HEAT )   // Will only be accurate for the first heat bracket.
        return make_expr( name_str, [this]{ return ( resource_used_amount( RESOURCE_HEAT ) ) / heat_regen_per_second(); } );
    }
  }

  else if ( splits[ 0 ] == "pet" )
  {
    pet_t* pet = find_pet( splits[ 1 ] );
    if ( pet )
    {
      if ( splits[ 2 ] == "active" )
        return make_expr( name_str, [pet]{ return ! pet -> sleeping; } );
      else if ( splits[ 2 ] == "remains" )
        return make_expr( name_str, [pet]() -> double
          {
            event_t* e = pet -> expiration;
            if ( e && e -> remains() > timespan_t::zero() )
              return to_seconds( e -> remains() );
            else
              return 0;
          } );
      else
        return pet -> create_expression( a, join( splits.begin() + 2, splits.end(), '.' ) );
    }
  }

  else if ( splits[ 0 ] == "owner" )
  {
    if ( pet_t* pet = dynamic_cast<pet_t*>( this ) )
    {
      if ( pet -> owner )
        return pet -> owner -> create_expression( a, name_str.substr( 6 ) );
    }
  }

  else if ( splits[ 0 ] == "temporary_bonus" )
  {
    stat_type stat = util_t::parse_stat_type( splits[ 1 ] );

    if ( stat != STAT_NONE )
    {
      switch ( stat )
      {
        case STAT_STRENGTH:
        case STAT_AIM:
        case STAT_CUNNING:
        case STAT_WILLPOWER:
        case STAT_ENDURANCE:
        case STAT_PRESENCE:
          // Relies on STAT_XXX == ATTR_XXX
          return make_expr( name_str, [this,stat]{
              attribute_type attr = static_cast<attribute_type>( stat );
              return temporary.attribute[ attr ] * composite_attribute_multiplier( attr );
            } );
        case STAT_EXPERTISE_RATING:
          return make_expr( name_str, [this]{ return temporary.expertise_rating; } );
        case STAT_ACCURACY_RATING:
          return make_expr( name_str, [this]{ return temporary.accuracy_rating; } );
        case STAT_CRIT_RATING:
          return make_expr( name_str, [this]{ return temporary.crit_rating; } );
        case STAT_ALACRITY_RATING:
          return make_expr( name_str, [this]{ return temporary.alacrity_rating; } );
        case STAT_ARMOR:
          return make_expr( name_str, [this]{ return temporary.armor; } );
        case STAT_POWER:
          return make_expr( name_str, [this]{ return temporary.power; } );
        case STAT_FORCE_POWER:
          return make_expr( name_str, [this]{ return temporary.force_power; } );
        case STAT_TECH_POWER:
          return make_expr( name_str, [this]{ return temporary.tech_power; } );
        case STAT_SURGE_RATING:
          return make_expr( name_str, [this]{ return temporary.surge_rating; } );
        case STAT_DEFENSE_RATING:
          return make_expr( name_str, [this]{ return temporary.defense_rating; } );
        case STAT_SHIELD_RATING:
          return make_expr( name_str, [this]{ return temporary.shield_rating; } );
        case STAT_ABSORB_RATING:
          return make_expr( name_str, [this]{ return temporary.absorb_rating; } );

        default: break;
      }
    }
  }

  else if ( num_splits == 3 )
  {
    if ( splits[ 0 ] == "buff" || splits[ 0 ] == "debuff" || splits[ 0 ] == "aura" )
    {
      buff_t* buff = get_targetdata( this ) -> get_buff( splits[ 1 ] );
      if ( ! buff ) buff = buff_t::find( this, splits[ 1 ] );
      if ( ! buff ) buff = buff_t::find( sim, splits[ 1 ] );
      if ( ! buff ) return 0;
      return buff -> create_expression( splits[ 2 ] );
    }
    else if ( splits[ 0 ] == "cooldown" )
    {
      cooldown_t* cooldown = get_cooldown( splits[ 1 ] );
      if ( splits[ 2 ] == "remains" )
        return make_expr( name_str, [cooldown]{ return to_seconds( cooldown -> remains() ); } );
    }
    else if ( splits[ 0 ] == "dot" )
    {
      dot_t* dot = nullptr;
      for ( targetdata_t* p : sourcedata )
      {
        if ( p && ( dot = p -> get_dot( splits[ 1 ] ) ) )
          break;
      }
      if ( ! dot )
        dot = get_dot( splits[ 1 ] );
      if ( ! dot )
        return nullptr;
      return dot -> create_expression( splits[ 2 ] );
    }
    else if ( splits[ 0 ] == "action" )
    {
      if ( splits[ 2 ] == "in_flight" )
      {
        std::vector<action_t*> in_flight_list;
        for ( action_t* action = action_list; action; action = action -> next )
        {
          if ( action -> name_str == splits[ 1 ] )
            in_flight_list.push_back( action );
        }
        if ( ! in_flight_list.empty() )
        {
          return make_expr( name_str, [in_flight_list]() -> bool
            {
              for ( action_t& a : in_flight_list | boost::adaptors::indirected )
                if ( a.travel_event ) return true;
              return false;
            } );
        }
      }
      else
      {
        if ( action_t* action = find_action( splits[ 1 ] ) )
          return action -> create_expression( splits[ 2 ] );
      }
    }
  }

  else if ( splits[ 0 ] == "set_bonus" )
  {
    if ( num_splits == 2 )
    {
      if ( set_bonus_t* sb = find_set_bonus( splits[ 1 ] ) )
        return make_expr( name_str, [sb]{ return sb -> count; } );
    }
  }

  else if ( num_splits >= 2 && splits[ 0 ] == "target" )
    return target -> create_expression( a, join( splits.begin() + 1, splits.end(), '.' ) );

  return sim -> create_expression( a, name_str );
}

// player_t::create_profile =================================================

void player_t::create_profile( std::ostream& os, save_type savetype )
{
  if ( savetype == SAVE_ALL )
    os << "#!./simc\n\n";

  if ( ! comment_str.empty() )
    os << "# " << comment_str << '\n';

  if ( savetype == SAVE_ALL )
  {
    os << util_t::player_type_string( type ) << "=\"" << name_str << "\"\n"
          "origin=\"" << origin_str << "\"\n"
          "level=" << level << "\n"
          "race=" << race_str << "\n"
          "position=" << position_str << "\n"
          "role=" << util_t::role_type_string( primary_role() ) << "\n"
          "use_pre_potion=" << use_pre_potion << '\n';

    if ( ! professions_str.empty() )
      os << "professions=" << professions_str << '\n';
  }

  if ( savetype == SAVE_ALL || savetype == SAVE_TALENTS )
  {
    talents_str = mrrobot::encode_talents( *this );
    // talents_str = torhead::encode_talents( *this );
    if ( ! talents_str.empty() )
      os << "talents=" << talents_str << '\n';
  }

  if ( ( savetype == SAVE_ALL || savetype == SAVE_ACTIONS )
       && ! action_list_str.empty() )
  {
    bool first = true;
    for ( action_t* a = action_list; a; a = a -> next )
    {
      if ( ! a -> signature_str.empty() )
      {
        os << "actions" <<  ( first ? "=" : "+=/" )
           << a -> signature_str << '\n';
        first = false;
      }
    }
  }

  if ( savetype == SAVE_ALL || savetype == SAVE_GEAR )
  {
    for ( item_t& item : items )
    {
      if ( item.active() )
        os << item.slot_name() << '=' << item.options_str << '\n';
    }

    if ( ! items_str.empty() )
      os << "items=" << items_str << '\n';

    os << "# Gear Summary\n";
    for ( stat_type i = STAT_NONE; i < STAT_MAX; ++i )
    {
      double value = initial_stats.get_stat( i );
      if ( value != 0 )
        os << "# gear_" << util_t::stat_type_string( i )
           << '=' << value << '\n';
    }

    os << "# Set Bonuses\n";
    for ( set_bonus_t* sb = set_bonus_list; sb; sb = sb -> next )
    {
      std::string name = sb -> name + "_2pc\n";
      if ( sb -> force_enable_2pc )
        os << "set_bonus=" << name;
      else if ( sb -> force_disable_2pc )
        os << "set_bonus=no_" << name;
      else if ( sb -> two_pc() )
        os << "# set_bonus=" << name;

      name = sb -> name + "_4pc\n";
      if ( sb -> force_enable_4pc )
        os << "set_bonus=" << name;
      else if ( sb -> force_disable_4pc )
        os << "set_bonus=no_" << name;
      else if ( sb -> four_pc() )
        os << "# set_bonus=" << name;
    }

    for ( item_t& item : items )
    {
      if ( ! item.active() ) continue;
      if ( item.unique || item.unique_enchant || item.unique_addon || ! item.encoded_weapon_str.empty() )
      {
        os << "# " << item.slot_name() << '=' << item.name();
        if ( item.heroic() ) os << ",heroic=1";
        if ( ! item.encoded_weapon_str.empty() ) os << ",weapon=" << item.encoded_weapon_str;
        if ( item.unique_enchant ) os << ",enchant=" << item.encoded_enchant_str;
        if ( item.unique_addon   ) os << ",addon=" << item.encoded_addon_str;
        os << '\n';
      }
    }

    if ( enchant.attribute[ ATTR_STRENGTH  ] != 0 )  os << "enchant_strength="         << enchant.attribute[ ATTR_STRENGTH  ] << '\n';
    if ( enchant.attribute[ ATTR_AIM       ] != 0 )  os << "enchant_aim="              << enchant.attribute[ ATTR_AIM       ] << '\n';
    if ( enchant.attribute[ ATTR_CUNNING   ] != 0 )  os << "enchant_cunning="          << enchant.attribute[ ATTR_CUNNING   ] << '\n';
    if ( enchant.attribute[ ATTR_WILLPOWER ] != 0 )  os << "enchant_willpower="        << enchant.attribute[ ATTR_WILLPOWER ] << '\n';
    if ( enchant.attribute[ ATTR_ENDURANCE ] != 0 )  os << "enchant_endurance="        << enchant.attribute[ ATTR_ENDURANCE ] << '\n';
    if ( enchant.attribute[ ATTR_PRESENCE  ] != 0 )  os << "enchant_presence="         << enchant.attribute[ ATTR_PRESENCE  ] << '\n';
    if ( enchant.power                       != 0 )  os << "enchant_power="            << enchant.power << '\n';
    if ( enchant.force_power                 != 0 )  os << "enchant_force_power="      << enchant.force_power << '\n';
    if ( enchant.tech_power                  != 0 )  os << "enchant_tech_power="       << enchant.tech_power << '\n';
    if ( enchant.expertise_rating            != 0 )  os << "enchant_expertise_rating=" << enchant.expertise_rating << '\n';
    if ( enchant.armor                       != 0 )  os << "enchant_armor="            << enchant.armor << '\n';
    if ( enchant.alacrity_rating             != 0 )  os << "enchant_alacrity_rating="  << enchant.alacrity_rating << '\n';
    if ( enchant.accuracy_rating             != 0 )  os << "enchant_accuracy_rating="  << enchant.accuracy_rating << '\n';
    if ( enchant.crit_rating                 != 0 )  os << "enchant_crit_rating="      << enchant.crit_rating << '\n';
    if ( enchant.weapon_dmg                  != 0 )  os << "enchant_weapon_dmg="       << enchant.weapon_dmg << '\n';
    if ( enchant.weapon_offhand_dmg          != 0 )  os << "enchant_weapon_offhand_dmg=" << enchant.weapon_offhand_dmg << '\n';
    if ( enchant.resource[ RESOURCE_HEALTH ] != 0 )  os << "enchant_health="           << enchant.resource[ RESOURCE_HEALTH ] << '\n';
    if ( enchant.resource[ RESOURCE_MANA   ] != 0 )  os << "enchant_mana="             << enchant.resource[ RESOURCE_MANA   ] << '\n';
    if ( enchant.resource[ RESOURCE_RAGE   ] != 0 )  os << "enchant_rage="             << enchant.resource[ RESOURCE_RAGE   ] << '\n';
    if ( enchant.resource[ RESOURCE_ENERGY ] != 0 )  os << "enchant_energy="           << enchant.resource[ RESOURCE_ENERGY ] << '\n';
    if ( enchant.resource[ RESOURCE_AMMO   ] != 0 )  os << "enchant_ammo="             << enchant.resource[ RESOURCE_AMMO   ] << '\n';
    if ( enchant.resource[ RESOURCE_HEAT   ] != 0 )  os << "enchant_heat="             << enchant.resource[ RESOURCE_HEAT   ] << '\n';
    if ( bonus_surge_pc_ != 0 )  os << "bonus_surge_pc="             << bonus_surge_pc_ << '\n';
    if ( bonus_crit_pc_ != 0 )  os << "bonus_crit_pc="             << bonus_crit_pc_ << '\n';
    if ( bonus_accuracy_pc_ != 0 )  os << "bonus_accuracy_pc="             << bonus_accuracy_pc_ << '\n';
    if ( bonus_health_pc_ != 0 )  os << "bonus_health_pc="             << bonus_health_pc_ << '\n';
  }
}

void player_t::create_json_profile( std::ostream& os, save_type savetype )
{
  os << "{\n";
  if ( savetype == SAVE_ALL )
  {
    os << "\t\"Class\" : \"" << util_t::player_type_string( type ) << "\",\n"
          "\t\"Name\" : \"" << name_str << "\",\n"
          "\t\"Origin\" : \"" << origin_str << "\"\n"
          "\t\"Level\" : " << level << ",\n"
          "\t\"Race\" : \"" << race_str << "\",\n";

    if ( ! professions_str.empty() )
      os << "\t\"Professions\" : \"" << professions_str << "\",\n";
  }

  talents_str = torhead::encode_talents( *this );
  if ( ! talents_str.empty() )
    os << "\t\"Talents\" : \"" << talents_str << "\",\n";

  os << "\t\"Gear\" : {\n";
  for ( item_t& item : items )
  {
    if ( item.active() )
      os << "\t\t\"" << item.slot_name() << "\" : \"" + item.options_str + "\",\n";
  }
  os << "\t},\n";

  os << "\t\"Position\" : \"" << position_str << "\",\n"
        "\t\"Role\" : \"" << util_t::role_type_string( primary_role() ) << "\",\n"
        "\t\"Use_pre_potion\" : " << use_pre_potion << ",\n";

  os << "\t\"Actions\" : [\n";
  for ( action_t* a = action_list; a; a = a -> next )
  {
    if ( ! a -> signature_str.empty() )
      os << "\t\t\"" << a -> signature_str << "\",\n";
  }
  os << "\t],\n";

  os << "\t\"Gear Summary\" : {\n";
  for ( stat_type i = STAT_NONE; i < STAT_MAX; ++i )
  {
    double value = initial_stats.get_stat( i );
    if ( value )
      os << "\t\t\"" << util_t::stat_type_string( i ) << "\" : " << value << ",\n";
  }
  os << "\t},\n";

  os << "\t\"Set Bonuses\" : [\n";
  for ( set_bonus_t* sb = set_bonus_list; sb; sb = sb -> next )
  {
    std::string name = sb -> name + "_2pc";
    if ( sb -> force_enable_2pc )
      os << "\t\t\"" << name << "\",\n";
    else if ( sb -> force_disable_2pc )
      os << "\t\t\"no_" << name << "\",\n";
    else if ( sb -> two_pc() )
      os << "\t\t\"#" << name << "\",\n";

    name = sb -> name + "_4pc";
    if ( sb -> force_enable_4pc )
      os << "\t\t\"" << name << "\",\n";
    else if ( sb -> force_disable_4pc )
      os << "\t\t\"no_" << name << "\",\n";
    else if ( sb -> four_pc() )
      os << "\t\t\"#" << name << "\",\n";
  }
  os << "\t],\n";

  os << "}";
}

// player_t::copy_from ======================================================

void player_t::copy_from( const player_t& source )
{
  origin_str = source.origin_str;
  level = source.level;
  race_str = source.race_str;
  role = source.role;
  professions_str = source.professions_str;

  position = source.position;
  position_str = source.position_str;

  use_pre_potion = source.use_pre_potion;

  if ( ptr == source.ptr && ! source.talents_str.empty() )
    parse_talent_string( *this, source.talents_str );
  else
  {
    // This is necessary because sometimes the talent trees change shape between live/ptr.
    std::stringstream ss;

    for ( int i=0; i < MAX_TALENT_TREES; i++ )
    {
      for ( size_t j = 0; j < talent_trees[ i ].size(); j++ )
      {
        talent_t* t = talent_trees[ i ][ j ];
        int rank = 0;
        if ( const talent_t* source_t = source.find_talent( t -> name() ) )
        {
          rank = source_t -> rank();
          t -> set_rank( rank );
        }
        ss << rank;
      }
    }

    talents_str = ss.str();
  }

  action_list_str = source.action_list_str;
  action_priority_list.clear();
  for ( size_t i = 0; i < source.action_priority_list.size(); i++ )
  {
    action_priority_list.push_back( source.action_priority_list[ i ] );
  }

  int num_items = ( int ) items.size();
  for ( int i=0; i < num_items; i++ )
  {
    items[ i ] = source.items[ i ];
    items[ i ].player = this;
  }
  gear               = source.gear;
  enchant            = source.enchant;
  bonus_surge_pc_    = source.bonus_surge_pc_;
  bonus_crit_pc_     = source.bonus_crit_pc_;
  bonus_accuracy_pc_ = source.bonus_accuracy_pc_;
  bonus_health_pc_   = source.bonus_health_pc_;
}

// player_t::create_options =================================================

void player_t::create_options()
{
  option_t player_options[] =
  {
    // General
    { "name",                                 OPT_STRING,   &( name_str                               ) },
    { "origin",                               OPT_STRING,   &( origin_str                             ) },
    { "region",                               OPT_STRING,   &( region_str                             ) },
    { "server",                               OPT_STRING,   &( server_str                             ) },
    { "id",                                   OPT_STRING,   &( id_str                                 ) },
    { "talents",                              OPT_FUNC,     ( void* ) ::parse_talent_url                },
    //{ "glyphs",                               OPT_STRING,   &( glyphs_str                             ) },
    { "race",                                 OPT_STRING,   &( race_str                               ) },
    { "level",                                OPT_INT,      &( level                                  ) },
    { "use_pre_potion",                       OPT_INT,      &( use_pre_potion                         ) },
    { "role",                                 OPT_FUNC,     ( void* ) ::parse_role_string               },
    { "set_bonus",                            OPT_FUNC,     ( void* ) ::parse_set_bonus                 },
    { "target",                               OPT_STRING,   &( target_str                             ) },
    { "skill",                                OPT_FLT,      &( initial_skill                          ) },
    { "distance",                             OPT_FLT,      &( distance                               ) },
    { "position",                             OPT_STRING,   &( position_str                           ) },
    { "professions",                          OPT_STRING,   &( professions_str                        ) },
    { "actions",                              OPT_STRING,   &( action_list_str                        ) },
    { "actions+",                             OPT_APPEND,   &( action_list_str                        ) },
    { "action_list",                          OPT_STRING,   &( choose_action_list                     ) },
    { "sleeping",                             OPT_BOOL,     &( initial_sleeping                       ) },
    { "quiet",                                OPT_BOOL,     &( quiet                                  ) },
    { "save",                                 OPT_STRING,   &( save_str                               ) },
    { "save_gear",                            OPT_STRING,   &( save_gear_str                          ) },
    { "save_talents",                         OPT_STRING,   &( save_talents_str                       ) },
    { "save_actions",                         OPT_STRING,   &( save_actions_str                       ) },
    { "save_json",                            OPT_STRING,   &( save_json_str                          ) },
    { "comment",                              OPT_STRING,   &( comment_str                            ) },
    { "bugs",                                 OPT_BOOL,     &( bugs                                   ) },
    { "world_lag",                            OPT_FUNC,     ( void* ) ::parse_world_lag                 },
    { "world_lag_stddev",                     OPT_FUNC,     ( void* ) ::parse_world_lag_stddev          },
    { "brain_lag",                            OPT_FUNC,     ( void* ) ::parse_brain_lag                 },
    { "brain_lag_stddev",                     OPT_FUNC,     ( void* ) ::parse_brain_lag_stddev          },
    { "scale_player",                         OPT_BOOL,     &( scale_player                           ) },
    // Items
    { "items",                                OPT_STRING,   &( items_str                              ) },
    { "items+",                               OPT_APPEND,   &( items_str                              ) },
    { "head",                                 OPT_STRING,   &( items[ SLOT_HEAD      ].options_str    ) },
    { "ear",                                  OPT_STRING,   &( items[ SLOT_EAR       ].options_str    ) },
    { "shoulders",                            OPT_STRING,   &( items[ SLOT_SHOULDERS ].options_str    ) },
    { "shoulder",                             OPT_STRING,   &( items[ SLOT_SHOULDERS ].options_str    ) },
    { "shirt",                                OPT_STRING,   &( items[ SLOT_SHIRT     ].options_str    ) },
    { "chest",                                OPT_STRING,   &( items[ SLOT_CHEST     ].options_str    ) },
    { "waist",                                OPT_STRING,   &( items[ SLOT_WAIST     ].options_str    ) },
    { "legs",                                 OPT_STRING,   &( items[ SLOT_LEGS      ].options_str    ) },
    { "leg",                                  OPT_STRING,   &( items[ SLOT_LEGS      ].options_str    ) },
    { "feet",                                 OPT_STRING,   &( items[ SLOT_FEET      ].options_str    ) },
    { "foot",                                 OPT_STRING,   &( items[ SLOT_FEET      ].options_str    ) },
    { "wrists",                               OPT_STRING,   &( items[ SLOT_WRISTS    ].options_str    ) },
    { "wrist",                                OPT_STRING,   &( items[ SLOT_WRISTS    ].options_str    ) },
    { "hands",                                OPT_STRING,   &( items[ SLOT_HANDS     ].options_str    ) },
    { "hand",                                 OPT_STRING,   &( items[ SLOT_HANDS     ].options_str    ) },
    { "implant1",                             OPT_STRING,   &( items[ SLOT_IMPLANT_1 ].options_str    ) },
    { "implant2",                             OPT_STRING,   &( items[ SLOT_IMPLANT_2 ].options_str    ) },
    { "relic1",                               OPT_STRING,   &( items[ SLOT_RELIC_1   ].options_str    ) },
    { "relic2",                               OPT_STRING,   &( items[ SLOT_RELIC_2   ].options_str    ) },
    { "back",                                 OPT_STRING,   &( items[ SLOT_BACK      ].options_str    ) },
    { "main_hand",                            OPT_STRING,   &( items[ SLOT_MAIN_HAND ].options_str    ) },
    { "off_hand",                             OPT_STRING,   &( items[ SLOT_OFF_HAND  ].options_str    ) },
    { "ranged",                               OPT_STRING,   &( items[ SLOT_RANGED    ].options_str    ) },
    { "tabard",                               OPT_STRING,   &( items[ SLOT_TABARD    ].options_str    ) },

    // Gear Stats
    { "gear_strength",                        OPT_FLT,  &( gear.attribute[ ATTR_STRENGTH  ]           ) },
    { "gear_aim",                             OPT_FLT,  &( gear.attribute[ ATTR_AIM       ]           ) },
    { "gear_cunning",                         OPT_FLT,  &( gear.attribute[ ATTR_CUNNING   ]           ) },
    { "gear_willpower",                       OPT_FLT,  &( gear.attribute[ ATTR_WILLPOWER ]           ) },
    { "gear_endurance",                       OPT_FLT,  &( gear.attribute[ ATTR_ENDURANCE ]           ) },
    { "gear_presence",                        OPT_FLT,  &( gear.attribute[ ATTR_PRESENCE  ]           ) },
    { "gear_power",                           OPT_FLT,  &( gear.power                                 ) },
    { "gear_force_power",                     OPT_FLT,  &( gear.force_power                           ) },
    { "gear_expertise_rating",                OPT_FLT,  &( gear.expertise_rating                      ) },
    { "gear_alacrity_rating",                 OPT_FLT,  &( gear.alacrity_rating                       ) },
    { "gear_accuracy_rating",                 OPT_FLT,  &( gear.accuracy_rating                       ) },
    { "gear_surge_rating",                    OPT_FLT,  &( gear.surge_rating                          ) },
    { "gear_crit_rating",                     OPT_FLT,  &( gear.crit_rating                           ) },
    { "gear_health",                          OPT_FLT,  &( gear.resource[ RESOURCE_HEALTH ]           ) },
    { "gear_mana",                            OPT_FLT,  &( gear.resource[ RESOURCE_MANA   ]           ) },
    { "gear_rage",                            OPT_FLT,  &( gear.resource[ RESOURCE_RAGE   ]           ) },
    { "gear_energy",                          OPT_FLT,  &( gear.resource[ RESOURCE_ENERGY ]           ) },
    { "gear_ammo",                            OPT_FLT,  &( gear.resource[ RESOURCE_AMMO   ]           ) },
    { "gear_heat",                            OPT_FLT,  &( gear.resource[ RESOURCE_HEAT   ]           ) },
    { "gear_armor",                           OPT_FLT,  &( gear.armor                                 ) },
    // Stat Enchants
    { "enchant_strength",                     OPT_FLT,  &( enchant.attribute[ ATTR_STRENGTH  ]        ) },
    { "enchant_aim",                          OPT_FLT,  &( enchant.attribute[ ATTR_AIM       ]        ) },
    { "enchant_cunning",                      OPT_FLT,  &( enchant.attribute[ ATTR_CUNNING   ]        ) },
    { "enchant_willpower",                    OPT_FLT,  &( enchant.attribute[ ATTR_WILLPOWER ]        ) },
    { "enchant_endurance",                    OPT_FLT,  &( enchant.attribute[ ATTR_ENDURANCE ]        ) },
    { "enchant_presence",                     OPT_FLT,  &( enchant.attribute[ ATTR_PRESENCE  ]        ) },
    { "enchant_power",                        OPT_FLT,  &( enchant.power                              ) },
    { "enchant_force_power",                  OPT_FLT,  &( enchant.force_power                        ) },
    { "enchant_expertise_rating",             OPT_FLT,  &( enchant.expertise_rating                   ) },
    { "enchant_armor",                        OPT_FLT,  &( enchant.armor                              ) },
    { "enchant_alacrity_rating",              OPT_FLT,  &( enchant.alacrity_rating                    ) },
    { "enchant_accuracy_rating",              OPT_FLT,  &( enchant.accuracy_rating                    ) },
    { "enchant_crit_rating",                  OPT_FLT,  &( enchant.crit_rating                        ) },
    { "enchant_weapon_dmg",                   OPT_FLT,  &( enchant.weapon_dmg                         ) },
    { "enchant_weapon_offhand_dmg",           OPT_FLT,  &( enchant.weapon_offhand_dmg                 ) },
    { "enchant_health",                       OPT_FLT,  &( enchant.resource[ RESOURCE_HEALTH ]        ) },
    { "enchant_mana",                         OPT_FLT,  &( enchant.resource[ RESOURCE_MANA   ]        ) },
    { "enchant_rage",                         OPT_FLT,  &( enchant.resource[ RESOURCE_RAGE   ]        ) },
    { "enchant_energy",                       OPT_FLT,  &( enchant.resource[ RESOURCE_ENERGY ]        ) },
    { "enchant_heat",                         OPT_FLT,  &( enchant.resource[ RESOURCE_HEAT   ]        ) },
    // Regen
    { "infinite_energy",                      OPT_BOOL,   &( infinite_resource[ RESOURCE_ENERGY ]     ) },
    { "infinite_ammo",                        OPT_BOOL,   &( infinite_resource[ RESOURCE_AMMO   ]     ) },
    // XXX this could be fun. heat works in reverse, so infinite heat is is 0 
    { "infinite_heat",                        OPT_BOOL,   &( infinite_resource[ RESOURCE_HEAT   ]     ) },
    { "infinite_health",                      OPT_BOOL,   &( infinite_resource[ RESOURCE_HEALTH ]     ) },
    { "infinite_mana",                        OPT_BOOL,   &( infinite_resource[ RESOURCE_MANA   ]     ) },
    { "infinite_rage",                        OPT_BOOL,   &( infinite_resource[ RESOURCE_RAGE   ]     ) },
    // Misc
    { "skip_actions",                         OPT_STRING, &( action_list_skip                         ) },
    { "modify_action",                        OPT_STRING, &( modify_action                            ) },
    { "reaction_time_mean",                   OPT_TIMESPAN, &( reaction_mean                          ) },
    { "reaction_time_stddev",                 OPT_TIMESPAN, &( reaction_stddev                        ) },
    { "reaction_time_nu",                     OPT_TIMESPAN, &( reaction_nu                            ) },
    // stat bonuses- outside dr, for companions
    { "bonus_crit_pc",                        OPT_INT,    &( bonus_crit_pc_                           ) },
    { "bonus_accuracy_pc",                    OPT_INT,    &( bonus_accuracy_pc_                       ) },
    { "bonus_surge_pc",                       OPT_INT,    &( bonus_surge_pc_                          ) },
    // XXX FIX how does companion health bonus work. % or set amount?
    { "bonus_health_pc",                     OPT_INT,    &( bonus_health_pc_                          ) },
    { NULL, OPT_UNKNOWN, NULL },

  };

  option_t::copy( options, player_options );
}


// player_t::create =========================================================

player_t* player_t::create( sim_t*             sim,
                            const std::string& type,
                            const std::string& name,
                            race_type r )
{
  if ( type == "jedi_sage" )
  {
    return player_t::create_jedi_sage( sim, name, r );
  }
  else if ( type == "sith_sorcerer" )
  {
    return player_t::create_sith_sorcerer( sim, name, r );
  }
  else if ( type == "jedi_shadow" )
  {
    return player_t::create_jedi_shadow( sim, name, r );
  }
  else if ( type == "sith_assassin" )
  {
    return player_t::create_sith_assassin( sim, name, r );
  }
  else if ( type == "sith_juggernaut" )
  {
    return player_t::create_sith_juggernaut( sim, name, r );
  }
  else if ( type == "jedi_guardian" )
  {
    return player_t::create_jedi_guardian( sim, name, r );
  }
  else if ( type == "sith_marauder" )
  {
    return player_t::create_sith_marauder( sim, name, r );
  }
  else if ( type == "jedi_sentinel" )
  {
    return player_t::create_jedi_sentinel( sim, name, r );
  }
  else if ( type == "commando" )
  {
    return player_t::create_commando( sim, name, r );
  }
  else if ( type == "mercenary" )
  {
    return player_t::create_mercenary( sim, name, r );
  }
  else if ( type == "scoundrel" )
  {
    return player_t::create_scoundrel( sim, name, r );
  }
  else if ( type == "operative" )
  {
    return player_t::create_operative( sim, name, r );
  }
  else if ( type == "vanguard" )
  {
    return player_t::create_vanguard( sim, name, r );
  }
  else if ( type == "powertech" )
  {
    return player_t::create_powertech( sim, name, r );
  }
  else if ( type == "gunslinger" )
  {
    return player_t::create_gunslinger( sim, name, r );
  }
  else if ( type == "sniper" )
  {
    return player_t::create_sniper( sim, name, r );
  }

  else if ( type == "enemy" )
  {
    return player_t::create_enemy( sim, name, r );
  }
  return 0;
}

// ==========================================================================
// Target data
// ==========================================================================

targetdata_t* player_t::new_targetdata( player_t& target )
{ return new targetdata_t( *this, target ); }

targetdata_t* player_t::get_targetdata( player_t* target )
{
  if ( ! target ) return nullptr;

  const size_t id = target -> targetdata_id;
  assert( id < targetdata.size() );

  targetdata_t*& td = targetdata[ id ];

  if ( LAZY_TARGETDATA_INIT && td == nullptr )
  {
    assert( targetdata_id < target -> sourcedata.size() );
    assert( target -> sourcedata[ targetdata_id ] == nullptr );
    target -> sourcedata[ targetdata_id ] =
        td = new_targetdata( *target );
  }

  assert( td );
  return td;
}

targetdata_t* player_t::get_sourcedata( player_t* source )
{
  if ( ! source ) return nullptr;
  return source -> get_targetdata( this );
}

targetdata_t::targetdata_t( player_t& s, player_t& t )
  : source( s ), target( t )
{}

targetdata_t::~targetdata_t()
{}

void targetdata_t::add( buff_t& b )
{
  assert( boost::find( buffs, &b ) == buffs.end() );
  buffs.push_back( &b );
  alias( b, b.name_str );
}

void targetdata_t::alias( buff_t& b, const std::string& name )
{ buffs_by_name[ name ] = &b; }

buff_t* targetdata_t::get_buff( const std::string& name ) const
{
  auto p = buffs_by_name.find( name );
  if ( p != buffs_by_name.end() )
    return p -> second;
  else
    return nullptr;
}

void targetdata_t::add( dot_t& d )
{
  assert( boost::find( dots, &d ) == dots.end() );
  dots.push_back( &d );
  alias( d, d.name_str );
}

void targetdata_t::alias( dot_t& d, const std::string& name )
{ dots_by_name[ name ] = &d; }

dot_t* targetdata_t::get_dot( const std::string& name ) const
{
  const bool DEBUG_PARANOIA = false;

  if ( DEBUG_PARANOIA )
    std::cerr << "get_dot(\"" << name << "\") = ";
  auto p = dots_by_name.find( name );
  if ( p != dots_by_name.end() )
  {
    if ( DEBUG_PARANOIA )
      std::cerr << p -> second << '\n';
    return p -> second;
  }
  else
  {
    if ( DEBUG_PARANOIA )
      std::cerr << "nullptr\n";
    return nullptr;
  }
}

void targetdata_t::reset()
{
  for ( dot_t* d : dots )
    d -> reset();
}

void targetdata_t::clear_debuffs()
{
  // FIXME: should clear debuffs as well according to similar FIXME in player_t::clear_debuffs()

  for ( dot_t* d : dots )
    d -> cancel();
}
