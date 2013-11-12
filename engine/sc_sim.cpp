// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"
#include "utf8.h"

#ifdef SC_SIGACTION
#include <signal.h>
#endif

namespace { // ANONYMOUS NAMESPACE ==========================================

#ifdef SC_SIGACTION
// POSIX-only signal handler ================================================

struct sim_signal_handler_t
{
  static sim_t* global_sim;

  static void sigint( int )
  {
    if ( global_sim )
    {
      if ( global_sim -> canceled ) exit( 0 );
      global_sim -> cancel();
    }
  }

  static void callback( int signal )
  {
    if ( global_sim )
    {
      const char* name = strsignal( signal );
      fprintf( stderr, "sim_signal_handler: %s! Seed=%d Iteration=%d\n",
               name, global_sim -> seed, global_sim -> current_iteration );
      fflush( stderr );
    }
    exit( 0 );
  }

  sim_signal_handler_t( sim_t* sim )
  {
    assert ( ! global_sim );
    global_sim = sim;
    struct sigaction sa;
    sigemptyset( &sa.sa_mask );
    sa.sa_flags = 0;

    sa.sa_handler = callback;
    sigaction( SIGSEGV, &sa, 0 );

    sa.sa_handler = sigint;
    sigaction( SIGINT,  &sa, 0 );
  }

  ~sim_signal_handler_t()
  { global_sim = 0; }
};
sim_t* sim_signal_handler_t::global_sim = 0;
#else
struct sim_signal_handler_t
{
  sim_signal_handler_t( sim_t* ) {}
};
#endif

// need_to_save_profiles ====================================================

static bool need_to_save_profiles( sim_t* sim )
{
  if ( sim -> save_profiles ) return true;

  for ( player_t* p = sim -> player_list; p; p = p -> next )
    if ( ! p -> save_str.empty() || ! p -> save_json_str.empty() )
      return true;

  return false;
}

// parse_ptr ================================================================

static bool parse_ptr( sim_t*             sim,
                       const std::string& name,
                       const std::string& value )
{
  if ( name != "ptr" ) return false;

  if ( SC_USE_PTR )
    sim -> ptr = atoi( value.c_str() ) != 0;
  else
    sim -> errorf( "SimulationCraft has not been built with PTR data.  The 'ptr=' option is ignored.\n" );

  return true;
}

// parse_active =============================================================

static bool parse_active( sim_t*             sim,
                          const std::string& name,
                          const std::string& value )
{
  if ( name != "active" ) return false;

  if ( value == "owner" )
  {
    sim -> active_player = sim -> active_player -> cast_pet() -> owner;
  }
  else if ( value == "none" || value == "0" )
  {
    sim -> active_player = 0;
  }
  else
  {
    if ( sim -> active_player )
    {
      sim -> active_player = sim -> active_player -> find_pet( value );
    }
    if ( ! sim -> active_player )
    {
      sim -> active_player = sim -> find_player( value );
    }
    if ( ! sim -> active_player )
    {
      sim -> errorf( "Unable to find player %s to make active.\n", value.c_str() );
      return false;
    }
  }

  return true;
}

// parse_optimal_raid =======================================================

static bool parse_optimal_raid( sim_t*             sim,
                                const std::string& name,
                                const std::string& value )
{
  if ( name != "optimal_raid" ) return false;

  sim -> use_optimal_buffs_and_debuffs( atoi( value.c_str() ) );

  return true;
}


// parse_player =============================================================

static bool parse_player( sim_t*             sim,
                          const std::string& name,
                          const std::string& value )
{
#if 0
  if ( name == "player" )
  {
    std::string::size_type cut_pt = value.find( ',' );
    std::string player_name( value, 0, cut_pt );

    std::string player_options;
    if ( cut_pt != value.npos )
      player_options.assign( value, cut_pt + 1, value.npos );

    std::string wowhead;
    std::string region = sim -> default_region_str;
    std::string server = sim -> default_server_str;
    std::string talents = "active";
    int use_cache=0;

    option_t options[] =
    {
      { "wowhead", OPT_STRING, &wowhead   },
      { "region",  OPT_STRING, &region    },
      { "server",  OPT_STRING, &server    },
      { "talents", OPT_STRING, &talents   },
      { "cache",   OPT_BOOL,   &use_cache },
      { NULL, OPT_UNKNOWN, NULL }
    };

    option_t::parse( sim, "player", options, player_options );

    sim -> input_is_utf8 = utf8::is_valid( player_name.begin(), player_name.end() ) && utf8::is_valid( server.begin(), server.end() );
    cache::behavior_t caching = use_cache ? cache::ANY : cache::players();

    if ( wowhead.empty() )
      sim -> active_player = bcp_api::download_player( sim, region, server, player_name, "active", caching );
    else
    {
      sim -> active_player = wowhead_t::download_player( sim, wowhead, ( talents == "active" ), caching );

      if ( sim -> active_player && player_name != sim -> active_player -> name() )
        sim -> errorf( "Mismatch between player name '%s' and wowhead name '%s' for id '%s'\n",
                       player_name.c_str(), sim -> active_player -> name(), wowhead.c_str() );
    }
  }
  else
#endif // 0

  if ( name == "pet" )
  {
    std::string::size_type cut_pt = value.find( ',' );
    std::string pet_type( value, 0, cut_pt );

    std::string pet_name;
    if ( cut_pt != value.npos )
      pet_name.assign( value, cut_pt + 1, value.npos );
    else
      pet_name = value;

    sim -> active_player = sim -> active_player -> create_pet( pet_name, pet_type );
  }

  else if ( name == "copy" )
  {
    std::string::size_type cut_pt = value.find( ',' );
    std::string player_name( value, 0, cut_pt );

    player_t* source;
    if ( cut_pt == value.npos )
      source = sim -> active_player;
    else
      source = sim -> find_player( value.substr( cut_pt + 1 ) );

    if ( source == 0 )
    {
      sim -> errorf( "Invalid source for profile copy - format is copy=target[,source], source defaults to active player." );
      return false;
    }

    sim -> active_player = player_t::create( sim, util_t::player_type_string( source -> type ), player_name );
    if ( sim -> active_player != 0 ) sim -> active_player -> copy_from( *source );
  }

  else
    sim -> active_player = player_t::create( sim, name, value );

  return sim -> active_player != 0;
}

// parse_proxy ==============================================================

static bool parse_proxy( sim_t*             sim,
                         const std::string& /* name */,
                         const std::string& value )
{

  std::vector<std::string> splits;
  int num_splits = util_t::string_split( splits, value, "," );

  if ( num_splits != 3 )
  {
    sim -> errorf( "Expected format is: proxy=type,host,port\n" );
    return false;
  }

  int port = atoi( splits[ 2 ].c_str() );
  if ( splits[ 0 ] == "http" && port > 0 && port < 65536 )
  {
    http_t::proxy.type = splits[ 0 ];
    http_t::proxy.host = splits[ 1 ];
    http_t::proxy.port = port;
    return true;
  }

  return false;
}

// parse_cache ==============================================================

static bool parse_cache( sim_t*             /* sim */,
                         const std::string& name,
                         const std::string& value )
{
  if ( name == "cache_players" )
  {
    if ( value == "1" ) cache::players( cache::ANY );
    else if ( value == "0" ) cache::players( cache::CURRENT );
    else if ( util_t::str_compare_ci( value, "only" ) ) cache::players( cache::ONLY );
    else return false;

    return true;
  }

  else if ( name == "cache_items" )
  {
    if ( value == "1" ) cache::items( cache::ANY );
    else if ( value == "0" ) cache::items( cache::CURRENT );
    else if ( util_t::str_compare_ci( value, "only" ) ) cache::items( cache::ONLY );
    else return false;

    return true;
  }

  else
    return false;


  return true;
}
// parse_mrrobot =============================================================

static bool parse_mrrobot( sim_t*             sim,
                           const std::string& name,
                           const std::string& value )
{
  assert( name == "mrrobot" ); ( void )name;

  sim -> active_player = mrrobot::download_player( sim, value );
  if ( ! sim -> active_player )
    return false;
  else
    return true;
}


#if 0
// parse_armory =============================================================

static bool parse_armory( sim_t*             sim,
                          const std::string& name,
                          const std::string& value )
{
  if ( name == "armory" )
  {
    std::vector<std::string> splits;
    int num_splits = util_t::string_split( splits, value, "," );

    if ( num_splits < 3 )
    {
      sim -> errorf( "Expected format is: armory=region,server,player1,player2,...\n" );
      return false;
    }

    const std::string& region = splits[ 0 ];
    const std::string& server = splits[ 1 ];

    for ( int i=2; i < num_splits; i++ )
    {
      std::string player_name = splits[ i ];
      std::string description = "active";
      if ( player_name[ 0 ] == '!' )
      {
        player_name.erase( 0, 1 );
        description = "inactive";
      }
      std::string::size_type pos = player_name.find( '|' );
      if ( pos != player_name.npos )
      {
        description.assign( player_name, pos + 1, player_name.npos );
        player_name.erase( pos );
      }

      if ( ! sim -> input_is_utf8 )
        sim -> input_is_utf8 = utf8::is_valid( player_name.begin(), player_name.end() ) && utf8::is_valid( server.begin(), server.end() );

      sim -> active_player = bcp_api::download_player( sim, region, server, player_name, description );
      if ( ! sim -> active_player ) return false;
    }
    return true;
  }
  else if ( name == "guild" )
  {
    std::string::size_type cut_pt = value.find( ',' );
    std::string guild_name( value, 0, cut_pt );

    std::string guild_options;
    if ( cut_pt != value.npos )
      guild_options.assign( value, cut_pt + 1, value.npos );

    std::string region = sim -> default_region_str;
    std::string server = sim -> default_server_str;
    std::string type_str;
    std::string ranks_str;
    int max_rank = 0;
    int use_cache = 0;

    option_t options[] =
    {
      { "region",   OPT_STRING, &region    },
      { "server",   OPT_STRING, &server    },
      { "class",    OPT_STRING, &type_str  },
      { "max_rank", OPT_INT,    &max_rank  },
      { "ranks",    OPT_STRING, &ranks_str },
      { "cache",    OPT_BOOL,   &use_cache },
      { NULL, OPT_UNKNOWN, NULL }
    };

    if ( ! option_t::parse( sim, "guild", options, guild_options ) )
      return false;

    // Save Raid Summary file when guilds are downloaded
    sim -> save_raid_summary = 1;

    std::vector<int> ranks_list;
    if ( ! ranks_str.empty() )
    {
      std::vector<std::string> ranks;
      int n_ranks = util_t::string_split( ranks, ranks_str, "/" );
      if ( n_ranks > 0 )
      {
        for ( int i = 0; i < n_ranks; i++ )
          ranks_list.push_back( atoi( ranks[i].c_str() ) );
      }
    }

    sim -> input_is_utf8 = utf8::is_valid( guild_name.begin(), guild_name.end() ) && utf8::is_valid( server.begin(), server.end() );

    int player_type = PLAYER_NONE;
    if ( ! type_str.empty() ) player_type = util_t::parse_player_type( type_str );

    cache::behavior_t caching = use_cache ? cache::ANY : cache::players();

    return bcp_api::download_guild( sim, region, server, guild_name, ranks_list, player_type, max_rank, caching );
  }

  return false;
}

// parse_wowhead ============================================================

static bool parse_wowhead( sim_t*             sim,
                           const std::string& name,
                           const std::string& value )
{
  if ( name == "wowhead" )
  {
    std::vector<std::string> splits;
    int num_splits = util_t::string_split( splits, value, ",." );

    if ( num_splits == 1 )
    {
      std::string player_id = splits[ 0 ];
      bool active = true;
      if ( player_id[ 0 ] == '!' )
      {
        player_id.erase( 0, 1 );
        active = false;
      }
      sim -> active_player = wowhead_t::download_player( sim, player_id, active );
    }
    else if ( num_splits >= 3 )
    {
      std::string region = splits[ 0 ];
      std::string server = splits[ 1 ];

      for ( int i=2; i < num_splits; i++ )
      {
        std::string player_name = splits[ i ];
        bool active = true;
        if ( player_name[ 0 ] == '!' )
        {
          player_name.erase( 0, 1 );
          active = false;
        }
        sim -> active_player = wowhead_t::download_player( sim, region, server, player_name, active );
        if ( ! sim -> active_player ) return false;
      }
    }
    else
    {
      sim -> errorf( "Expected format is: wowhead=id OR wowhead=region,server,player1,player2,...\n" );
      return false;
    }
  }

  return sim -> active_player != 0;
}

// parse_wowreforge =========================================================

static bool parse_wowreforge( sim_t*             sim,
                              const std::string& name,
                              const std::string& value )
{
  if ( name == "wowreforge" )
  {
    sim -> active_player = wowreforge::download_player( sim, value );
  }

  return sim -> active_player != 0;
}
#endif // 0

// parse_fight_style ========================================================

static bool parse_fight_style( sim_t*             sim,
                               const std::string& name,
                               const std::string& value )
{
  if ( name != "fight_style" ) return false;

  if ( util_t::str_compare_ci( value, "Patchwerk" ) )
  {
    sim -> fight_style = "Patchwerk";
    sim -> raid_events_str.clear();
  }
  else if ( util_t::str_compare_ci( value, "Ultraxion" ) )
  {
    sim -> fight_style = "Ultraxion";
    sim -> max_time    = from_seconds( 366.0 );
    sim -> fixed_time  = 1;
    sim -> vary_combat_length = 0.0;
    sim -> raid_events_str =  "flying,first=0,duration=500,cooldown=500";
    sim -> raid_events_str +=  "/position_switch,first=0,duration=500,cooldown=500";
    sim -> raid_events_str += "/stun,duration=1.0,first=45.0,period=45.0";
    sim -> raid_events_str += "/stun,duration=1.0,first=57.0,period=57.0";
    sim -> raid_events_str += "/damage,first=6.0,period=6.0,last=59.5,amount=44000,type=shadow";
    sim -> raid_events_str += "/damage,first=60.0,period=5.0,last=119.5,amount=44855,type=shadow";
    sim -> raid_events_str += "/damage,first=120.0,period=4.0,last=179.5,amount=44855,type=shadow";
    sim -> raid_events_str += "/damage,first=180.0,period=3.0,last=239.5,amount=44855,type=shadow";
    sim -> raid_events_str += "/damage,first=240.0,period=2.0,last=299.5,amount=44855,type=shadow";
    sim -> raid_events_str += "/damage,first=300.0,period=1.0,amount=44855,type=shadow";
  }
  else if ( util_t::str_compare_ci( value, "HelterSkelter" ) )
  {
    sim -> fight_style = "HelterSkelter";
    sim -> raid_events_str = "casting,cooldown=30,duration=3,first=15";
    sim -> raid_events_str += "/movement,cooldown=30,duration=5";
    sim -> raid_events_str += "/stun,cooldown=60,duration=2";
    sim -> raid_events_str += "/invulnerable,cooldown=120,duration=3";
  }
  else if ( util_t::str_compare_ci( value, "LightMovement" ) )
  {
    sim -> fight_style = "LightMovement";
    sim -> raid_events_str = "/movement,players_only=1,first=";
    sim -> raid_events_str += util_t::to_string( to_seconds<int>( sim -> max_time * 0.1 ) );
    sim -> raid_events_str += ",cooldown=85,duration=7,last=";
    sim -> raid_events_str += util_t::to_string( to_seconds<int>( sim -> max_time * 0.8 ) );
  }
  else if ( util_t::str_compare_ci( value, "HeavyMovement" ) )
  {
    sim -> fight_style = "HeavyMovement";
    sim -> raid_events_str = "/movement,players_only=1,first=10,cooldown=10,duration=4";
  }
  else
  {
    log_t::output( sim, "Custom fight style specified: %s", value.c_str() );
    sim -> fight_style = value;
  }

  return true;
}

// parse_item_sources =======================================================

static bool parse_item_sources( sim_t*             sim,
                                const std::string& /* name */,
                                const std::string& value )
{
  std::vector<std::string> sources;

  util_t::string_split( sources, value, ":/|", false );

  sim -> item_db_sources.clear();

  for ( size_t i = 0; i < sources.size(); i++ )
  {
    if ( ! util_t::str_compare_ci( sources[ i ], "local" ) &&
         ! util_t::str_compare_ci( sources[ i ], "mmoc" ) &&
         ! util_t::str_compare_ci( sources[ i ], "wowhead" ) &&
         ! util_t::str_compare_ci( sources[ i ], "ptrhead" ) &&
         ! util_t::str_compare_ci( sources[ i ], "armory" ) &&
         ! util_t::str_compare_ci( sources[ i ], "bcpapi" ) )
    {
      continue;
    }

    sim -> item_db_sources.push_back( util_t::format_name( sources[ i ] ) );
  }

  if ( sim -> item_db_sources.empty() )
  {
    sim -> errorf( "Your global data source string \"%s\" contained no valid data sources. Valid identifiers are: local, mmoc, wowhead, ptrhead and armory.\n",
                   value.c_str() );
    return false;
  }

  return true;
}

} // ANONYMOUS NAMESPACE ===================================================

// ==========================================================================
// Simulator
// ==========================================================================

// sim_t::sim_t =============================================================

sim_t::sim_t( sim_t* p, int index ) :
  parent( p ),
  target_list( 0 ), player_list( 0 ), active_player( 0 ), num_players( 0 ),
  num_enemies( 0 ), num_targetdata_ids( 0 ), max_player_level( -1 ), canceled( 0 ),

  queue_lag( from_seconds( 0.037 ) ), queue_lag_stddev( timespan_t::zero() ),
  gcd_lag( from_seconds( 0.150 ) ), gcd_lag_stddev( timespan_t::zero() ),
  channel_lag( from_seconds( 0.200 ) ), channel_lag_stddev( timespan_t::zero() ),
  queue_gcd_reduction( from_seconds( 0.032 ) ), strict_gcd_queue( 0 ),
  confidence( 0.95 ), confidence_estimator( 1.96 ),
  world_lag( from_seconds( 0.1 ) ), world_lag_stddev( timespan_t_min() ),
  travel_variance( 0 ), default_skill( 1.0 ), reaction_time( from_seconds( 0.5 ) ),
  regen_periodicity( from_seconds( 1.0 ) ), // observed by philoptik@gmail.com 03/03/2012 according to ingame tooltip
  current_time( timespan_t::zero() ), max_time( from_seconds( 300 ) ),
  expected_time( timespan_t::zero() ), vary_combat_length( 0.2 ),
  last_event( timespan_t::zero() ), fixed_time( 0 ),
  events_remaining( 0 ), max_events_remaining( 0 ),
  events_processed( 0 ), total_events_processed( 0 ),
  seed( 0 ), id( 0 ), iterations( 1000 ), current_iteration( -1 ), current_slot( -1 ),
  armor_update_interval( 20 ),
  optimal_raid( 0 ), log( 0 ), debug( 0 ), save_profiles( 0 ), default_actions( 0 ),
  normalized_stat( STAT_NONE ),
  default_region_str( "us" ),
  save_prefix_str( "save_" ),
  save_talent_str( 0 ),
  input_is_utf8( false ), ptr( false ),
  target_death_pct( 0 ), target_health( 0 ), target_level( -1 ), target_adds( 0 ),
  default_rng_( 0 ), rng_list( 0 ), deterministic_roll( false ),
  rng(), deterministic_rng(), smooth_rng( false ), average_range( true ), average_gauss( false ),
  timing_wheel( 0 ), wheel_seconds( 0 ), wheel_size( 0 ), wheel_mask( 0 ), timing_slice( 0 ), wheel_granularity( 0.0 ),
  fight_style( "Patchwerk" ), overrides( overrides_t() ), auras( auras_t() ),
  buff_list( 0 ), aura_delay( from_seconds( 0.5 ) ), default_aura_delay( from_seconds( 0.3 ) ),
  default_aura_delay_stddev( from_seconds( 0.05 ) ),
  cooldown_list( 0 ),
  elapsed_cpu( timespan_t::zero() ), iteration_dmg( 0 ), iteration_heal( 0 ),
  raid_dps(), total_dmg(), raid_hps(), total_heal(), simulation_length( false ),
  report_progress( 1 ),
  path_str( "." ), output_file( stdout ),
  debug_exp( 0 ),
  // Report
  report_precision( 4 ),report_pets_separately( 0 ), report_targets( 1 ), report_details( 1 ),
  report_rng( 0 ), hosted_html( 0 ), print_styles( false ), report_overheal( 0 ),
  save_raid_summary( 0 ), statistics_level( 1 ), separate_stats_by_actions( 0 ),
  // Multi-Threading
  threads( 0 ), thread_index( index )
{
  path_str += "|profiles";

  path_str += "|..";
  path_str += DIRECTORY_DELIMITER;
  path_str += "profiles";

  // sage_sorcerer
  path_str += "|profiles";
  path_str += DIRECTORY_DELIMITER;
  path_str += "sage_sorcerer";
  path_str += "|..";
  path_str += DIRECTORY_DELIMITER;
  path_str += "profiles";
  path_str += DIRECTORY_DELIMITER;
  path_str += "sage_sorcerer";

  // shadow_assassin
  path_str += "|profiles";
  path_str += DIRECTORY_DELIMITER;
  path_str += "shadow_assassin";
  path_str += "|..";
  path_str += DIRECTORY_DELIMITER;
  path_str += "profiles";
  path_str += DIRECTORY_DELIMITER;
  path_str += "shadow_assassin";

  // scoundrel_operative
  path_str += "|profiles";
  path_str += DIRECTORY_DELIMITER;
  path_str += "scoundrel_operative";
  path_str += "|..";
  path_str += DIRECTORY_DELIMITER;
  path_str += "profiles";
  path_str += DIRECTORY_DELIMITER;
  path_str += "scoundrel_operative";

  // gunslinger_sniper
  path_str += "|profiles";
  path_str += DIRECTORY_DELIMITER;
  path_str += "gunslinger_sniper";
  path_str += "|..";
  path_str += DIRECTORY_DELIMITER;
  path_str += "profiles";
  path_str += DIRECTORY_DELIMITER;
  path_str += "gunslinger_sniper";

  // commando_mercenary
  path_str += "|profiles";
  path_str += DIRECTORY_DELIMITER;
  path_str += "commando_mercenary";
  path_str += "|..";
  path_str += DIRECTORY_DELIMITER;
  path_str += "profiles";
  path_str += DIRECTORY_DELIMITER;
  path_str += "commando_mercenary";

  path_str += "|profiles_heal";

  path_str += "|..";
  path_str += DIRECTORY_DELIMITER;
  path_str += "profiles_heal";

  // sage_sorcerer HEAL
  path_str += "|profiles_heal";
  path_str += DIRECTORY_DELIMITER;
  path_str += "sage_sorcerer";
  path_str += "|..";
  path_str += DIRECTORY_DELIMITER;
  path_str += "profiles_heal";
  path_str += DIRECTORY_DELIMITER;
  path_str += "sage_sorcerer";

  path_str += "|profiles_tank";

  path_str += "|..";
  path_str += DIRECTORY_DELIMITER;
  path_str += "profiles_tank";

  // shadow_assassin tank
  path_str += "|profiles_tank";
  path_str += DIRECTORY_DELIMITER;
  path_str += "shadow_assassin";
  path_str += "|..";
  path_str += DIRECTORY_DELIMITER;
  path_str += "profiles_tank";
  path_str += DIRECTORY_DELIMITER;
  path_str += "shadow_assassin";

  // Initialize the default item database source order
  static const char* const dbsources[] = { "local", "bcpapi", "wowhead", "mmoc", "armory", "ptrhead" };
  item_db_sources.assign( boost::begin( dbsources ), boost::end( dbsources ) );

  scaling.reset( new scaling_t( this ) );
  plot.reset( new plot_t( this ) );
  reforge_plot.reset( new reforge_plot_t( this ) );

  use_optimal_buffs_and_debuffs( 1 );

  create_options();

  if ( parent )
  {
    // Import the config file
    parse_options( parent -> argc, parent -> argv );

    // Inherit 'scaling' settings from parent because these are set outside of the config file
    scaling -> scale_stat  = parent -> scaling -> scale_stat;
    scaling -> scale_value = parent -> scaling -> scale_value;

    // Inherit reporting directives from parent
    report_progress = parent -> report_progress;
    output_file     = parent -> output_file;

    // Inherit 'plot' settings from parent because are set outside of the config file
    enchant = parent -> enchant;

    seed = parent -> seed;
  }
}

// sim_t::~sim_t ============================================================

sim_t::~sim_t()
{
  flush_events();

  list_dispose( target_list );
  list_dispose( player_list );
  list_dispose( rng_list );
  list_dispose( buff_list );
  list_dispose( cooldown_list );
}

// sim_t::add_event =========================================================

void sim_t::add_event( event_t* e,
                       timespan_t delta_time )
{
  if ( delta_time < timespan_t::zero() )
    delta_time = timespan_t::zero();

  e -> time = current_time + delta_time;
  e -> id   = ++id;

  if ( unlikely( ! ( to_seconds<int>( delta_time ) <= wheel_seconds ) ) )
  {
    errorf( "sim_t::add_event assertion error! delta_time > wheel_seconds, event %s from %s.\n",
            e -> name, e -> player ? e -> player -> name() : "no-one" );
    assert( 0 );
  }

  if ( e -> time > last_event ) last_event = e -> time;

  uint32_t slice = to_seconds<uint32_t>( e -> time * wheel_granularity ) & wheel_mask;

  event_t** prev = &( timing_wheel[ slice ] );

  while ( ( *prev ) && ( *prev ) -> time <= e -> time ) prev = &( ( *prev ) -> next );

  e -> next = *prev;
  *prev = e;

  events_remaining++;
  if ( events_remaining > max_events_remaining ) max_events_remaining = events_remaining;
  if ( e -> player ) e -> player -> events++;

  if ( debug )
  {
    log_t::output( this, "Add Event: %s %.2f %d", e -> name, to_seconds( e -> time ), e -> id );
    if ( e -> player ) log_t::output( this, "Actor %s has %d scheduled events", e -> player -> name(), e -> player -> events );
  }
}

// sim_t::reschedule_event ==================================================

void sim_t::reschedule_event( event_t* e )
{
  if ( debug ) log_t::output( this, "Reschedule Event: %s %d", e -> name, e -> id );

  add_event( e, ( e -> reschedule_time - current_time ) );

  e -> reschedule_time = timespan_t::zero();
}

// sim_t::next_event ========================================================

event_t* sim_t::next_event()
{
  if ( events_remaining == 0 ) return 0;

  while ( true )
  {
    event_t*& event_list = timing_wheel[ timing_slice ];

    if ( event_list )
    {
      event_t* e = event_list;
      event_list = e -> next;
      events_remaining--;
      events_processed++;
      return e;
    }

    if ( ++timing_slice >= timing_wheel.size() )
    {
      timing_slice = 0;
      if ( debug ) log_t::output( this, "Time Wheel turns around." );
    }
  }

  return 0;
}

// sim_t::flush_events ======================================================

void sim_t::flush_events()
{
  if ( debug ) log_t::output( this, "Flush Events" );

  for ( size_t i = 0; i < timing_wheel.size(); ++i )
  {
    event_t* list = timing_wheel[ i ];
    timing_wheel[ i ] = 0;

    while ( list )
    {
      std::unique_ptr<event_t> e( list );
      list = list -> next;
      // Make sure we dont recancel events, although it should
      // not technically matter
      if ( e -> player && ! e -> canceled )
      {
        if ( --e -> player -> events < 0 )
        {
          errorf( "sim_t::flush_events assertion error! flushing event %s leaves negative event count for user %s.\n",
                  e -> name, e -> player -> name() );
          assert( false );
        }
      }
    }
  }

  events_remaining = 0;
  events_processed = 0;
  timing_slice = 0;
  id = 0;
}

// sim_t::cancel_events =====================================================

void sim_t::cancel_events( player_t* p )
{
  if ( p -> events <= 0 ) return;

  if ( debug ) log_t::output( this, "Canceling events for player %s, events to cancel %d", p -> name(), p -> events );

  size_t end_slice = to_seconds<size_t>( last_event * wheel_granularity ) & wheel_mask;

  // Loop only partial wheel, [current_time..last_event], as that's the range where there
  // are events for actors in the sim
  if ( timing_slice <= end_slice )
  {
    for ( size_t i = timing_slice; i <= end_slice && p -> events > 0; i++ )
    {
      for ( event_t* e = timing_wheel[ i ]; e && p -> events > 0; e = e -> next )
      {
        if ( e -> player == p )
        {
          if ( ! e -> canceled )
            p -> events--;

          e -> canceled = 1;
        }
      }
    }
  }
  // Loop only partial wheel in two places, as the wheel has wrapped around, but simulation
  // current time is still at the tail-end, [begin_slice..wheel_size[ and [0..last_event]
  else
  {
    for ( size_t i = timing_slice; i < timing_wheel.size() && p -> events > 0; i++ )
    {
      for ( event_t* e = timing_wheel[ i ]; e && p -> events > 0; e = e -> next )
      {
        if ( e -> player == p )
        {
          if ( ! e -> canceled )
            p -> events--;

          e -> canceled = 1;
        }
      }
    }

    for ( size_t i = 0; i <= end_slice && p -> events > 0; i++ )
    {
      for ( event_t* e = timing_wheel[ i ]; e && p -> events > 0; e = e -> next )
      {
        if ( e -> player == p )
        {
          if ( ! e -> canceled )
            p -> events--;

          e -> canceled = 1;
        }
      }
    }
  }

  assert( p -> events == 0 );
}

// sim_t::combat ============================================================

void sim_t::combat( int iteration )
{
  if ( debug ) log_t::output( this, "Starting Simulator" );

  current_iteration = iteration;

  combat_begin();

  while ( event_t* e = next_event() )
  {
    current_time = e -> time;

    // Perform actor event bookkeeping first
    if ( e -> player && ! e -> canceled )
    {
      e -> player -> events--;
      if ( e -> player -> events < 0 )
      {
        errorf( "sim_t::combat assertion error! canceling event %s leaves negative event count for user %s.\n", e -> name, e -> player -> name() );
        assert( 0 );
      }
    }

    if ( fixed_time || ( target -> resource_base[ RESOURCE_HEALTH ] == 0 ) )
    {
      // The first iteration is always time-limited since we do not yet have inferred health
      if ( current_time > expected_time )
      {
        if ( debug ) log_t::output( this, "Reached expected_time=%.2f, ending simulation", to_seconds( expected_time ) );
        // Set this last event as canceled, so asserts dont fire when odd things happen at the
        // tail-end of the simulation iteration
        e -> canceled = 1;
        delete e;
        break;
      }
    }
    else
    {
      if ( expected_time > timespan_t::zero() && current_time > ( expected_time * 2.0 ) )
      {
        if ( debug ) log_t::output( this, "Target proving tough to kill, ending simulation" );
        // Set this last event as canceled, so asserts dont fire when odd things happen at the
        // tail-end of the simulation iteration
        e -> canceled = 1;
        delete e;
        break;
      }

      if (  target -> resource_current[ RESOURCE_HEALTH ] / target -> resource_max[ RESOURCE_HEALTH ] <= target_death_pct / 100.0 )
      {
        if ( debug ) log_t::output( this, "Target %s has died, ending simulation", target -> name() );
        // Set this last event as canceled, so asserts dont fire when odd things happen at the
        // tail-end of the simulation iteration
        e -> canceled = 1;
        delete e;
        break;
      }
    }

    if ( unlikely( e -> canceled ) )
    {
      if ( debug ) log_t::output( this, "Canceled event: %s", e -> name );
    }
    else if ( unlikely( e -> reschedule_time > e -> time ) )
    {
      reschedule_event( e );
      continue;
    }
    else
    {
      if ( debug ) log_t::output( this, "Executing event: %s", e -> name );
      try
      {
        e -> execute();
      }
      catch ( cancel_t& err )
      {
        errorf( "%s\n", err.message.c_str() );
        cancel();
      }
    }
    delete e;
  }

  combat_end();
}

// sim_t::reset =============================================================

void sim_t::reset()
{
  if ( debug ) log_t::output( this, "Resetting Simulator" );
  expected_time = max_time * ( 1.0 + vary_combat_length * iteration_adjust() );
  id = 0;
  current_time = timespan_t::zero();
  last_event = timespan_t::zero();
  for ( buff_t* b = buff_list; b; b = b -> next )
  {
    b -> reset();
  }
  for ( player_t* t = target_list; t; t = t -> next )
  {
    t -> reset();
  }
  for ( player_t* p = player_list; p; p = p -> next )
  {
    p -> reset();
  }
  raid_event_t::reset( this );
}

// sim_t::combat_begin ======================================================

void sim_t::combat_begin()
{
  if ( debug ) log_t::output( this, "Combat Begin" );

  reset();

  iteration_dmg = iteration_heal = 0;

  for ( player_t* t = target_list; t; t = t -> next )
  {
    t -> combat_begin();
  }

  for ( buff_t* b = buff_list; b; b = b -> next )
    b -> combat_begin();

  player_t::combat_begin( this );

  raid_event_t::combat_begin( this );

  for ( player_t* p = player_list; p; p = p -> next )
  {
    p -> combat_begin();
  }
  new ( this ) regen_event_t( this );
}

// sim_t::combat_end ========================================================

void sim_t::combat_end()
{
  if ( debug ) log_t::output( this, "Combat End" );

  iteration_timeline.push_back( current_time );
  simulation_length.add( to_seconds( current_time ) );

  total_events_processed += events_processed;

  for ( player_t* t = target_list; t; t = t -> next )
  {
    if ( t -> is_add() ) continue;
    t -> combat_end();
  }
  player_t::combat_end( this );

  raid_event_t::combat_end( this );

  for ( player_t* p = player_list; p; p = p -> next )
  {
    if ( p -> is_pet() ) continue;
    p -> combat_end();
  }

  for ( buff_t* b = buff_list; b; b = b -> next )
  {
    b -> expire();
    b -> combat_end();
  }

  total_dmg.add( iteration_dmg );
  raid_dps.add( current_time != timespan_t::zero() ? iteration_dmg / to_seconds( current_time ) : 0 );
  total_heal.add( iteration_heal );
  raid_hps.add( current_time != timespan_t::zero() ? iteration_heal / to_seconds( current_time ) : 0 );

  flush_events();
}

// sim_t::init ==============================================================

bool sim_t::init()
{
  try
  {
    if ( seed == 0 ) seed = ( int ) time( NULL );

    if ( ! parent ) srand( seed );

    rng.reset( rng_t::create( this, "global", RNG_MERSENNE_TWISTER ) );

    deterministic_rng.reset( rng_t::create( this, "global_deterministic", RNG_MERSENNE_TWISTER ) );
    deterministic_rng -> seed( 31459 + thread_index );

    if ( scaling -> smooth_scale_factors &&
         scaling -> scale_stat != STAT_NONE )
    {
      smooth_rng = true;
      average_range = true;
      deterministic_roll = true;
    }

    default_rng_ = get_pointer( deterministic_roll ? deterministic_rng : rng );

    // Timing wheel depth defaults to about 17 minutes with a granularity of 32 buckets per second.
    // This makes wheel_size = 32K and it's fully used.
    if ( wheel_seconds     <  600 ) wheel_seconds     = 1024; // 2^10  Min of 600 to ensure no wrap-around bugs with Water Shield
    if ( wheel_granularity <=   0 ) wheel_granularity = 32;   // 2^5

    wheel_size = static_cast<size_t>( wheel_seconds * wheel_granularity );

    // Round up the wheel depth to the nearest power of 2 to enable a fast "mod" operation.
    wheel_mask = 2;
    while ( wheel_mask < wheel_size )
      wheel_mask *= 2;
    wheel_size = wheel_mask;
    --wheel_mask;

    // The timing wheel represents an array of event lists: Each time slice has an event list.
    timing_wheel.assign( wheel_size, nullptr );

    if (   queue_lag_stddev == timespan_t::zero() )   queue_lag_stddev =   queue_lag * 0.25;
    if (     gcd_lag_stddev == timespan_t::zero() )     gcd_lag_stddev =     gcd_lag * 0.25;
    if ( channel_lag_stddev == timespan_t::zero() ) channel_lag_stddev = channel_lag * 0.25;
    if ( world_lag_stddev    < timespan_t::zero() ) world_lag_stddev   =   world_lag * 0.1;

    // Find Already defined target, otherwise create a new one.
    if ( debug )
      log_t::output( this, "Creating Enemys." );

    if ( target_list )
    {
      target = target_list;
    }
    else if ( ! main_target_str.empty() )
    {
      player_t* p = find_player( main_target_str );
      if ( p )
        target = p;
    }
    else
      target = player_t::create( this, "enemy", "Fluffy_Pillow" );


    if ( max_player_level < 0 )
    {
      for ( player_t* p = player_list; p; p = p -> next )
      {
        if ( p -> is_enemy() || p -> is_add() )
          continue;
        if ( max_player_level < p -> level )
          max_player_level = p -> level;
      }
    }

    if ( ! player_t::init( this ) ) return false;

    // Target overrides 2
    for ( player_t* t = target_list; t; t = t -> next )
    {
      if ( ! target_race.empty() )
      {
        t -> race = util_t::parse_race_type( target_race );
        t -> race_str = util_t::race_type_string( t -> race );
      }
    }

    raid_event_t::init( this );

    if ( report_precision < 0 ) report_precision = 3;

    raid_dps.reserve( iterations );
    total_dmg.reserve( iterations );
    raid_hps.reserve( iterations );
    total_heal.reserve( iterations );
    simulation_length.reserve( iterations );
  }
  catch ( cancel_t& c )
  {
    errorf( "%s\n", c.message.c_str() );
    cancel();
  }

  return canceled ? false : true;
}

// compare_dps ==============================================================

struct compare_dps
{
  bool operator()( player_t* l, player_t* r ) const
  {
    return l -> dps.mean > r -> dps.mean;
  }
};

// compare_hps ==============================================================

struct compare_hps
{
  bool operator()( player_t* l, player_t* r ) const
  {
    return l -> hps.mean > r -> hps.mean;
  }
};

// compare_name =============================================================

struct compare_name
{
  bool operator()( player_t* l, player_t* r ) const
  {
    if ( l -> type != r -> type )
    {
      return l -> type < r -> type;
    }
    if ( l -> primary_tree() != r -> primary_tree() )
    {
      return l -> primary_tree() < r -> primary_tree();
    }
    return l -> name_str < r -> name_str;
  }
};

// sim_t::analyze_player ====================================================

void sim_t::analyze_player( player_t* p )
{
  assert( iterations > 0 );

  p -> pre_analyze_hook();

  // Sample Data Analysis ========================================================

  // sample_data_t::analyze(calc_basics,calc_variance,sort )

  p -> deaths.analyze( true, true, true, 50 );

  p -> fight_length.analyze( true, true );
  p -> waiting_time.analyze();
  p -> executed_foreground_actions.analyze();

  p -> dmg.analyze( true, true );
  p -> compound_dmg.analyze();
  p -> dps.analyze( true, true, true, 50 );
  p -> dpse.analyze();

  p -> dmg_taken.analyze();
  p -> dtps.analyze( true, true );

  p -> heal.analyze();
  p -> compound_heal.analyze();
  p -> hps.analyze( true, true, true, 50 );
  p -> hpse.analyze();

  p -> heal_taken.analyze();
  p -> htps.analyze( true, true );

  p -> deaths_error = p -> deaths.mean_std_dev * confidence_estimator;
  p -> dps_error = p -> dps.mean_std_dev * confidence_estimator;
  p -> dtps_error = p -> dtps.mean_std_dev * confidence_estimator;
  p -> hps_error = p -> hps.mean_std_dev * confidence_estimator;



  for ( buff_t* b = p -> buff_list; b; b = b -> next )
    b -> analyze();

  for ( benefit_t* u = p -> benefit_list; u; u = u -> next )
    u -> analyze();

  for ( uptime_t* u = p -> uptime_list; u; u = u -> next )
    u -> analyze();



  if ( p -> quiet ) return;
  if ( p -> fight_length.mean == 0 ) return;



  // Pet Chart Adjustment ===================================================
  int max_buckets = ( int ) p -> fight_length.max;

  // Make the pet graphs the same length as owner's
  if ( p -> is_pet() )
  {
    player_t* o = p -> cast_pet() -> owner;
    max_buckets = ( int ) o -> fight_length.max;
  }


  // Stats Analysis =========================================================
  std::vector<stats_t*> stats_list;

  for ( stats_t* s = p -> stats_list; s; s = s -> next )
    stats_list.push_back( s );

  for ( pet_t* pet = p -> pet_list; pet; pet = pet -> next_pet )
    for ( stats_t* s = pet -> stats_list; s; s = s -> next )
      stats_list.push_back( s );

  int num_stats = ( int ) stats_list.size();

  if ( ! p -> is_pet() )
  {
    for ( int i=0; i < num_stats; i++ )
    {
      stats_t* s = stats_list[ i ];
      s -> analyze();

      // Create Stats Timeline Chart
      s -> timeline_aps.clear();
      s -> timeline_aps.reserve( max_buckets );
      s -> timeline_amount.resize( max_buckets );
      sliding_window_average<10>( s -> timeline_amount, std::back_inserter( s -> timeline_aps ) );
      assert( s -> timeline_aps.size() == ( std::size_t ) max_buckets );

      chart_t::timeline( s -> timeline_aps_chart, p, s -> timeline_aps, s -> name_str + " APS", s -> aps );
      chart_t::distribution( s -> aps_distribution_chart,this, s -> portion_aps.distribution, s -> name_str + " APS", s -> portion_aps.mean, s -> portion_aps.min, s -> portion_aps.max );

      if ( s -> type == STATS_DMG )
      {
        s -> portion_amount = p -> compound_dmg.mean ? s -> compound_amount / p -> compound_dmg.mean : 0 ;
      }
      else
      {
        s -> portion_amount = p -> compound_heal.mean ? s -> compound_amount / p -> compound_heal.mean : 0;
      }
    }
  }


  // Actor Lists ============================================================
  if ( ! p -> quiet && ! p -> is_enemy() && ! p -> is_add() && ! ( p -> is_pet() && report_pets_separately ) )
  {
    players_by_dps.push_back( p );
    players_by_hps.push_back( p );
    players_by_name.push_back( p );
  }
  if ( ! p -> quiet && ( p -> is_enemy() || p -> is_add() ) && ! ( p -> is_pet() && report_pets_separately ) )
    targets_by_name.push_back( p );


  // Resources & Gains ======================================================
  for ( resource_type i = RESOURCE_NONE; i < RESOURCE_MAX; i++ )
  {
    int num_buckets = ( int ) p -> timeline_resource[i].size();

    if ( num_buckets > max_buckets ) p -> timeline_resource[i].resize( max_buckets );

    for ( int j=0; j < max_buckets; j++ )
    {
      p -> timeline_resource[i][ j ] /= divisor_timeline[ j ];
    }
  }

  for ( resource_type i = RESOURCE_NONE; i < RESOURCE_MAX; i++ )
  {
    p -> resource_lost  [ i ] /= iterations;
    p -> resource_gained[ i ] /= iterations;
  }

  double rl = p -> resource_lost[ p -> primary_resource() ];
  p -> dpr = ( rl > 0 ) ? ( p -> dmg.mean / rl ) : -1.0;
  p -> hpr = ( rl > 0 ) ? ( p -> heal.mean / rl ) : -1.0;

  p -> rps_loss = p -> resource_lost  [ p -> primary_resource() ] / p -> fight_length.mean;
  p -> rps_gain = p -> resource_gained[ p -> primary_resource() ] / p -> fight_length.mean;

  for ( gain_t* g = p -> gain_list; g; g = g -> next )
    g -> analyze( this );

  for ( pet_t* pet = p -> pet_list; pet; pet = pet -> next_pet )
  {
    for ( gain_t* g = pet -> gain_list; g; g = g -> next )
      g -> analyze( this );
  }

  // Procs ==================================================================

  for ( proc_t* proc = p -> proc_list; proc; proc = proc -> next )
    proc -> analyze( this );

  // Damage Timelines =======================================================

  p -> timeline_dmg.assign( max_buckets, 0 );
  for ( int i=0, is_hps = ( p -> primary_role() == ROLE_HEAL ); i < num_stats; i++ )
  {
    stats_t* s = stats_list[ i ];
    if ( ( s -> type != STATS_DMG ) == is_hps )
    {
      int j_max = std::min( max_buckets, ( int ) s -> timeline_amount.size() );
      for ( int j = 0; j < j_max; j++ )
        p -> timeline_dmg[ j ] += s -> timeline_amount[ j ];
    }
  }

  p -> timeline_dps.reserve( max_buckets );
  sliding_window_average<10>( p -> timeline_dmg,
                              std::back_inserter( p -> timeline_dps ) );
  assert( p -> timeline_dps.size() == ( std::size_t ) max_buckets );

  // Charts =================================================================

  chart_t::action_dpet       ( p -> action_dpet_chart,               p );
  chart_t::action_dmg        ( p -> action_dmg_chart,                p );
  chart_t::time_spent        ( p -> time_spent_chart,                p );

  std::string encoded_name;
  http_t::format( encoded_name, p -> name_str );

  for ( resource_type i = RESOURCE_NONE; i < RESOURCE_MAX; i++ )
  {
    chart_t::timeline        ( p -> timeline_resource_chart[i],      p,
                               p -> timeline_resource[i],
                               encoded_name + ' ' + util_t::resource_type_string( i ),
                               0,
                               chart_t::resource_color( i ) );
  }

  chart_t::timeline          ( p -> timeline_dps_chart,              p,
                               p -> timeline_dps,
                               encoded_name + " DPS",
                               p -> dps.mean );

  chart_t::dps_error         ( p -> dps_error_chart,                 p );

  if ( p -> primary_role() == ROLE_HEAL )
  {
    chart_t::distribution      ( p -> distribution_dps_chart,          this,
                                 p -> hps.distribution, encoded_name + " HPS",
                                 p -> hps.mean,
                                 p -> hps.min,
                                 p -> hps.max );
  }
  else
  {
    chart_t::distribution      ( p -> distribution_dps_chart,          this,
                                 p -> dps.distribution, encoded_name + " DPS",
                                 p -> dps.mean,
                                 p -> dps.min,
                                 p -> dps.max );
  }

  chart_t::distribution      ( p -> distribution_deaths_chart,       this,
                               p -> deaths.distribution, encoded_name + " Death",
                               p -> deaths.mean,
                               p -> deaths.min,
                               p -> deaths.max );
}

// sim_t::analyze ===========================================================

void sim_t::analyze()
{
  simulation_length.analyze( true, true, true, 50 );
  if ( simulation_length.mean == 0 ) return;

  // divisor_timeline is necessary because not all iterations go the same length of time

  int max_buckets = ( int ) simulation_length.max + 1;
  divisor_timeline.assign( max_buckets, 0 );

  size_t num_timelines = iteration_timeline.size();
  for ( size_t i=0; i < num_timelines; i++ )
  {
    int last = to_seconds<int>( iteration_timeline[ i ] );
    size_t num_buckets = divisor_timeline.size();
    if ( 1 + last > ( int ) num_buckets ) divisor_timeline.resize( 1 + last, 0 );
    for ( int j=0; j <= last; j++ ) divisor_timeline[ j ] += 1;
  }

  for ( buff_t* b = buff_list; b; b = b -> next )
    b -> analyze();

  total_dmg.analyze();
  raid_dps.analyze();
  total_heal.analyze();
  raid_hps.analyze();

  confidence_estimator = rng -> stdnormal_inv( 1.0 - ( 1.0 - confidence ) / 2.0 );

  for ( size_t i = 0; i < actor_list.size(); i++ )
    analyze_player( actor_list[i] );


  boost::sort( players_by_dps, compare_dps() );
  boost::sort( players_by_hps, compare_hps() );
  boost::sort( players_by_name, compare_name() );
  boost::sort( targets_by_name, compare_name() );

  chart_t::raid_aps     ( dps_charts,     this, players_by_dps, true );
  chart_t::raid_aps     ( hps_charts,     this, players_by_hps, false );
  chart_t::raid_dpet    ( dpet_charts,    this );
  chart_t::raid_gear    ( gear_charts,    this );
  chart_t::raid_downtime( downtime_chart, this );
  chart_t::distribution( timeline_chart, this,
                         simulation_length.distribution, "Timeline",
                         simulation_length.mean,
                         simulation_length.min,
                         simulation_length.max );
}

// sim_t::iterate ===========================================================

bool sim_t::iterate()
{
  if ( ! init() ) return false;

  int message_interval = iterations/10;
  int message_index = 10;

  for ( int i=0; i < iterations; i++ )
  {
    if ( canceled )
    {
      iterations = current_iteration + 1;
      break;
    }

    if ( report_progress && ( message_interval > 0 ) && ( i % message_interval == 0 ) && ( message_index > 0 ) )
    {
      util_t::fprintf( stdout, "%d... ", message_index-- );
      fflush( stdout );
    }
    combat( i );
  }
  if ( report_progress ) util_t::fprintf( stdout, "\n" );

  reset();

  return true;
}

// sim_t::merge =============================================================

void sim_t::merge( sim_t& other_sim )
{
  iterations             += other_sim.iterations;
  total_events_processed += other_sim.total_events_processed;

  simulation_length.merge( other_sim.simulation_length );
  total_dmg.merge( other_sim.total_dmg );
  raid_dps.merge( other_sim.raid_dps );
  total_heal.merge( other_sim.total_heal );
  raid_hps.merge( other_sim.raid_hps );

  if ( max_events_remaining < other_sim.max_events_remaining ) max_events_remaining = other_sim.max_events_remaining;

  boost::copy( other_sim.iteration_timeline, std::back_inserter( iteration_timeline ) );

  for ( buff_t* b = buff_list; b; b = b -> next )
  {
    b -> merge( buff_t::find( &other_sim, b -> name() ) );
  }

  for ( size_t i = 0; i < actor_list.size(); i++ )
  {
    player_t* p = actor_list[i];
    player_t* other_p = other_sim.find_player( p -> index );
    assert( other_p );
    p -> merge( *other_p );
  }
}

// sim_t::merge =============================================================

void sim_t::merge()
{
  int num_children = ( int ) children.size();

  for ( int i=0; i < num_children; i++ )
  {
    sim_t* child = children[ i ];
    child -> wait();
    merge( *child );
  }

  children.dispose();
}

// sim_t::partition =========================================================

void sim_t::partition()
{
  if ( threads <= 1 ) return;
  if ( iterations < threads ) return;

#if defined( NO_THREADS )
  util_t::fprintf( output_file, "simulationcraft: This executable was built without thread support, please remove 'threads=N' from config file.\n" );
  exit( 0 );
#endif

  int remainder = iterations % threads;
  iterations /= threads;

  int num_children = threads - 1;
  children.resize( num_children );

  for ( int i=0; i < num_children; i++ )
  {
    sim_t* child = children[ i ] = new sim_t( this, i+1 );
    child -> iterations /= threads;
    if ( remainder )
    {
      remainder--;
      child -> iterations += 1;
    }
    child -> report_progress = 0;
  }

  for ( int i=0; i < num_children; i++ )
    children[ i ] -> launch();
}

// sim_t::execute ===========================================================

bool sim_t::execute()
{
  typedef std::chrono::high_resolution_clock clock;

  clock::time_point start_time = clock::now();

  partition();
  if ( ! iterate() ) return false;
  merge();
  analyze();

  elapsed_cpu = std::chrono::duration_cast<timespan_t>( clock::now() - start_time );

  return true;
}

// sim_t::find_player =======================================================

player_t* sim_t::find_player( const std::string& name )
{
  for ( size_t i = 0; i < actor_list.size(); i++ )
  {
    player_t* p = actor_list[i];
    if ( name == p -> name() ) return p;
  }
  return 0;
}

// sim_t::find_player =======================================================

player_t* sim_t::find_player( int index )
{
  for ( size_t i = 0; i < actor_list.size(); i++ )
  {
    player_t* p = actor_list[i];
    if ( index == p -> index ) return p;
  }
  return 0;
}

// sim_t::get_cooldown ======================================================

cooldown_t* sim_t::get_cooldown( const std::string& name )
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

// sim_t::use_optimal_buffs_and_debuffs =====================================

void sim_t::use_optimal_buffs_and_debuffs( int value )
{
  optimal_raid = value;

  overrides.bleeding                    = optimal_raid;
  overrides.coordination                = optimal_raid;
  overrides.force_valor                 = optimal_raid;
  overrides.fortification_hunters_boon  = optimal_raid;
  overrides.shatter_shot                = optimal_raid;
  overrides.sunder                      = optimal_raid;
  overrides.heat_signature              = optimal_raid;
  overrides.unnatural_might             = optimal_raid;
}

// post_parse ===============================================================
void sim_t::post_parse()
{
  if ( overrides.ignore_player_arpen_debuffs == -1 )
    overrides.ignore_player_arpen_debuffs = ( overrides.shatter_shot | overrides.sunder | overrides.heat_signature ) ? 1 : 0;
}


// sim_t::aura_gain =========================================================

void sim_t::aura_gain( const std::string& aura_name , int /* aura_id */ )
{
  if ( log ) log_t::output( this, "Raid gains %s", aura_name.c_str() );
}

// sim_t::aura_loss =========================================================

void sim_t::aura_loss( const std::string& aura_name , int /* aura_id */ )
{
  if ( log ) log_t::output( this, "Raid loses %s", aura_name.c_str() );
}

// sim_t::time_to_think =====================================================

bool sim_t::time_to_think( timespan_t proc_time )
{
  if ( proc_time == timespan_t::zero() ) return false;
  if ( proc_time < timespan_t::zero() ) return true;
  return current_time - proc_time > reaction_time;
}

// sim_t::range =============================================================

double sim_t::range( double min,
                     double max )
{
  if ( average_range ) return ( min + max ) / 2.0;

  return default_rng_ -> range( min, max );
}

// sim_t::gauss =============================================================

double sim_t::gauss( double mean,
                     double stddev )
{
  if ( average_gauss ) return mean;

  return default_rng_ -> gauss( mean, stddev );
}

// sim_t::get_rng ===========================================================

rng_t* sim_t::get_rng( const std::string& n, rng_type type )
{
  assert( rng );

  if ( type == RNG_GLOBAL ) return get_pointer( rng );
  if ( type == RNG_DETERMINISTIC ) return get_pointer( deterministic_rng );

  if ( ! smooth_rng ) return default_rng_;

  rng_t* r=0;

  for ( r = rng_list; r; r = r -> next )
  {
    if ( r -> name_str == n )
      return r;
  }

  r = rng_t::create( this, n, type );
  r -> next = rng_list;
  rng_list = r;

  return r;
}

// sim_t::iteration_adjust ==================================================

double sim_t::iteration_adjust()
{
  if ( iterations <= 1 )
    return 0.0;

  if ( current_iteration == 0 )
    return 0.0;

  return ( 2.0 * current_iteration / ( double ) iterations ) - 1.0;
}

// sim_t::create_expression =================================================

expr_ptr sim_t::create_expression( action_t* a, const std::string& name_str )
{
  assert( a -> sim == this );

  if ( name_str == "time" )
    return make_expr( name_str, [this]{ return to_seconds( current_time ); } );

  if ( name_str == "enemies" )
    return make_expr( name_str, [this]{ return num_enemies; } );

  std::vector<std::string> splits;
  int num_splits = util_t::string_split( splits, name_str, "." );

  if ( num_splits == 3 && splits[ 0 ] == "aura" )
  {
    if ( buff_t* buff = buff_t::find( this, splits[ 1 ] ) )
      return buff -> create_expression( splits[ 2 ] );
  }

  else if ( num_splits >= 3 && splits[ 0 ] == "actors" )
  {
    if ( player_t* actor = sim_t::find_player( splits[ 1 ] ) )
      return actor -> create_expression( a, join( splits.begin() + 2, splits.end(), '.' ) );
  }

  else if ( num_splits >= 2 && splits[ 0 ] == "target" )
  {
    if ( target )
      return target -> create_expression( a, join( splits.begin() + 1, splits.end(), ',' ) );
  }

  return nullptr;
}

// sim_t::print_options =====================================================

void sim_t::print_options()
{
  util_t::fprintf( output_file, "\nWorld of Warcraft Raid Simulator Options:\n" );

  int num_options = ( int ) options.size();

  util_t::fprintf( output_file, "\nSimulation Engine:\n" );
  for ( int i=0; i < num_options; i++ ) options[ i ].print( output_file );

  for ( player_t* p = player_list; p; p = p -> next )
  {
    num_options = ( int ) p -> options.size();

    util_t::fprintf( output_file, "\nPlayer: %s (%s)\n", p -> name(), util_t::player_type_string( p -> type ) );
    for ( int i=0; i < num_options; i++ ) p -> options[ i ].print( output_file );
  }

  util_t::fprintf( output_file, "\n" );
  fflush( output_file );
}

// sim_t::create_options ====================================================

void sim_t::create_options()
{
  option_t global_options[] =
  {
    // General
    { "iterations",                       OPT_INT,    &( iterations                               ) },
    { "max_time",                         OPT_TIMESPAN, &( max_time                               ) },
    { "fixed_time",                       OPT_BOOL,   &( fixed_time                               ) },
    { "vary_combat_length",               OPT_FLT,    &( vary_combat_length                       ) },
    { "optimal_raid",                     OPT_FUNC,   ( void* ) ::parse_optimal_raid                },
    { "ptr",                              OPT_FUNC,   ( void* ) ::parse_ptr                         },
    { "threads",                          OPT_INT,    &( threads                                  ) },
    { "confidence",                       OPT_FLT,    &( confidence                               ) },
    { "item_db_source",                   OPT_FUNC,   ( void* ) ::parse_item_sources                },
    { "proxy",                            OPT_FUNC,   ( void* ) ::parse_proxy                       },
    // Lag
    { "channel_lag",                      OPT_TIMESPAN, &( channel_lag                            ) },
    { "channel_lag_stddev",               OPT_TIMESPAN, &( channel_lag_stddev                     ) },
    { "gcd_lag",                          OPT_TIMESPAN, &( gcd_lag                                ) },
    { "gcd_lag_stddev",                   OPT_TIMESPAN, &( gcd_lag_stddev                         ) },
    { "queue_lag",                        OPT_TIMESPAN, &( queue_lag                              ) },
    { "queue_lag_stddev",                 OPT_TIMESPAN, &( queue_lag_stddev                       ) },
    { "queue_gcd_reduction",              OPT_TIMESPAN, &( queue_gcd_reduction                    ) },
    { "strict_gcd_queue",                 OPT_BOOL,   &( strict_gcd_queue                         ) },
    { "default_world_lag",                OPT_TIMESPAN, &( world_lag                              ) },
    { "default_world_lag_stddev",         OPT_TIMESPAN, &( world_lag_stddev                       ) },
    { "default_aura_delay",               OPT_TIMESPAN, &( default_aura_delay                     ) },
    { "default_aura_delay_stddev",        OPT_TIMESPAN, &( default_aura_delay_stddev              ) },
    { "default_skill",                    OPT_FLT,    &( default_skill                            ) },
    { "reaction_time",                    OPT_TIMESPAN, &( reaction_time                          ) },
    { "travel_variance",                  OPT_FLT,    &( travel_variance                          ) },
    // Output
    { "save_profiles",                    OPT_BOOL,   &( save_profiles                            ) },
    { "default_actions",                  OPT_BOOL,   &( default_actions                          ) },
    { "debug",                            OPT_BOOL,   &( debug                                    ) },
    { "html",                             OPT_STRING, &( html_file_str                            ) },
    { "hosted_html",                      OPT_BOOL,   &( hosted_html                              ) },
    { "print_styles",                     OPT_BOOL,   &( print_styles                             ) },
    { "xml",                              OPT_STRING, &( xml_file_str                             ) },
    { "xml_style",                        OPT_STRING, &( xml_stylesheet_file_str                  ) },
    { "log",                              OPT_BOOL,   &( log                                      ) },
    { "output",                           OPT_STRING, &( output_file_str                          ) },
    { "path",                             OPT_STRING, &( path_str                                 ) },
    { "path+",                            OPT_APPEND, &( path_str                                 ) },
    { "save_raid_summary",                OPT_BOOL,   &( save_raid_summary                        ) },
    // Overrides"
    { "override.bleeding",                OPT_BOOL,   &( overrides.bleeding                       ) },
    // Regen
    { "regen_periodicity",                OPT_TIMESPAN, &( regen_periodicity                      ) },
    // RNG
    { "smooth_rng",                       OPT_DEPRECATED,   &( smooth_rng                               ) },
    { "deterministic_roll",               OPT_BOOL,   &( deterministic_roll                       ) },
    { "average_range",                    OPT_BOOL,   &( average_range                            ) },
    { "average_gauss",                    OPT_BOOL,   &( average_gauss                            ) },
    // Misc
    { "party",                            OPT_LIST,   &( party_encoding                           ) },
    { "active",                           OPT_FUNC,   ( void* ) ::parse_active                      },
    { "armor_update_internval",           OPT_INT,    &( armor_update_interval                    ) },
    { "aura_delay",                       OPT_TIMESPAN, &( aura_delay                             ) },
    { "seed",                             OPT_INT,    &( seed                                     ) },
    { "wheel_granularity",                OPT_FLT,    &( wheel_granularity                        ) },
    { "wheel_seconds",                    OPT_INT,    &( wheel_seconds                            ) },
    { "reference_player",                 OPT_STRING, &( reference_player_str                     ) },
    { "raid_events",                      OPT_STRING, &( raid_events_str                          ) },
    { "raid_events+",                     OPT_APPEND, &( raid_events_str                          ) },
    { "fight_style",                      OPT_FUNC,   ( void* ) ::parse_fight_style                 },
    { "debug_exp",                        OPT_INT,    &( debug_exp                                ) },
    { "main_target",                      OPT_STRING, &( main_target_str                          ) },
    { "target_death_pct",                 OPT_FLT,    &( target_death_pct                         ) },
    { "target_health",                    OPT_FLT,    &( target_health                            ) },
    { "target_level",                     OPT_INT,    &( target_level                             ) },
    { "target_race",                      OPT_STRING, &( target_race                              ) },
    // Character Creation
    { "jedi_sage",                        OPT_FUNC,   ( void* ) ::parse_player                      },
    { "sith_sorcerer",                    OPT_FUNC,   ( void* ) ::parse_player                      },
    { "jedi_shadow",                      OPT_FUNC,   ( void* ) ::parse_player                      },
    { "sith_assassin",                    OPT_FUNC,   ( void* ) ::parse_player                      },
    { "sith_juggernaut",                  OPT_FUNC,   ( void* ) ::parse_player                      },
    { "jedi_guardian",                    OPT_FUNC,   ( void* ) ::parse_player                      },
    { "sith_marauder",                    OPT_FUNC,   ( void* ) ::parse_player                      },
    { "jedi_sentinel",                    OPT_FUNC,   ( void* ) ::parse_player                      },
    { "commando",                         OPT_FUNC,   ( void* ) ::parse_player                      },
    { "mercenary",                        OPT_FUNC,   ( void* ) ::parse_player                      },
    { "gunslinger",                       OPT_FUNC,   ( void* ) ::parse_player                      },
    { "sniper",                           OPT_FUNC,   ( void* ) ::parse_player                      },
    { "scoundrel",                        OPT_FUNC,   ( void* ) ::parse_player                      },
    { "operative",                        OPT_FUNC,   ( void* ) ::parse_player                      },
    { "vanguard",                         OPT_FUNC,   ( void* ) ::parse_player                      },
    { "powertech",                        OPT_FUNC,   ( void* ) ::parse_player                      },

    { "enemy",                            OPT_FUNC,   ( void* ) ::parse_player                      },
    { "pet",                              OPT_FUNC,   ( void* ) ::parse_player                      },
    { "player",                           OPT_FUNC,   ( void* ) ::parse_player                      },
    { "copy",                             OPT_FUNC,   ( void* ) ::parse_player                      },
    { "mrrobot",                          OPT_FUNC,   ( void* ) ::parse_mrrobot                     },
#if 0
    { "armory",                           OPT_DEPRECATED,   ( void* ) ::parse_armory                      },
    { "guild",                            OPT_DEPRECATED,   ( void* ) ::parse_armory                      },
    { "wowhead",                          OPT_DEPRECATED,   ( void* ) ::parse_wowhead                     },
    { "wowreforge",                       OPT_DEPRECATED,   ( void* ) ::parse_wowreforge                  },
#endif
    { "http_clear_cache",                 OPT_FUNC,   ( void* ) ::http_t::clear_cache               },
    { "cache_items",                      OPT_FUNC,   ( void* ) ::parse_cache                       },
    { "cache_players",                    OPT_FUNC,   ( void* ) ::parse_cache                       },
    { "default_region",                   OPT_STRING, &( default_region_str                       ) },
    { "default_server",                   OPT_STRING, &( default_server_str                       ) },
    { "save_prefix",                      OPT_STRING, &( save_prefix_str                          ) },
    { "save_suffix",                      OPT_STRING, &( save_suffix_str                          ) },
    { "save_talent_str",                  OPT_BOOL,   &( save_talent_str                          ) },
    // Overrides
    { "override.bleeding",                OPT_BOOL,   &( overrides.bleeding                        ) },
    { "override.coordination",            OPT_BOOL,   &( overrides.coordination                    ) },
    { "override.force_valor",             OPT_BOOL,   &( overrides.force_valor                     ) },
    { "override.fortification_hunters_boon", OPT_BOOL, &( overrides.fortification_hunters_boon     ) },
    { "override.shatter_shot",            OPT_INT,    &( overrides.shatter_shot                    ) },
    { "override.sunder",                  OPT_INT,    &( overrides.sunder                          ) },
    { "override.heat_signature",          OPT_INT,    &( overrides.heat_signature                  ) },
    { "override.unnatural_might",         OPT_BOOL,   &( overrides.unnatural_might                 ) },
    { "override.ignore_player_arpen_debuffs", OPT_BOOL, &( overrides.ignore_player_arpen_debuffs   ) },
    // Stat Enchants
    { "default_enchant_strength",                 OPT_FLT,  &( enchant.attribute[ ATTR_STRENGTH  ] ) },
    { "default_enchant_aim",                      OPT_FLT,  &( enchant.attribute[ ATTR_AIM       ] ) },
    { "default_enchant_cunning",                  OPT_FLT,  &( enchant.attribute[ ATTR_CUNNING   ] ) },
    { "default_enchant_willpower",                OPT_FLT,  &( enchant.attribute[ ATTR_WILLPOWER ] ) },
    { "default_enchant_endurance",                OPT_FLT,  &( enchant.attribute[ ATTR_ENDURANCE ] ) },
    { "default_enchant_presence",                 OPT_FLT,  &( enchant.attribute[ ATTR_PRESENCE  ] ) },
    { "default_enchant_power",                    OPT_FLT,  &( enchant.power                       ) },
    { "default_enchant_expertise_rating",         OPT_FLT,  &( enchant.expertise_rating            ) },
    { "default_enchant_armor",                    OPT_FLT,  &( enchant.bonus_armor                 ) },
    { "default_enchant_alacrity_rating",          OPT_FLT,  &( enchant.alacrity_rating             ) },
    { "default_enchant_accuracy_rating",          OPT_FLT,  &( enchant.accuracy_rating             ) },
    { "default_enchant_crit_rating",              OPT_FLT,  &( enchant.crit_rating                 ) },
    { "default_enchant_weapon_dmg",               OPT_FLT,  &( enchant.weapon_dmg                  ) },
    { "default_enchant_weapon_offhand_dmg",       OPT_FLT,  &( enchant.weapon_offhand_dmg          ) },
    { "default_enchant_health",                   OPT_FLT,  &( enchant.resource[ RESOURCE_HEALTH ] ) },
    { "default_enchant_mana",                     OPT_FLT,  &( enchant.resource[ RESOURCE_MANA   ] ) },
    { "default_enchant_rage",                     OPT_FLT,  &( enchant.resource[ RESOURCE_RAGE   ] ) },
    { "default_enchant_energy",                   OPT_FLT,  &( enchant.resource[ RESOURCE_ENERGY ] ) },
    { "default_enchant_ammo",                     OPT_FLT,  &( enchant.resource[ RESOURCE_AMMO  ] ) },
    // Report
    { "report_precision",                 OPT_INT,    &( report_precision                         ) },
    { "report_pets_separately",           OPT_BOOL,   &( report_pets_separately                   ) },
    { "report_targets",                   OPT_BOOL,   &( report_targets                           ) },
    { "report_details",                   OPT_BOOL,   &( report_details                           ) },
    { "report_rng",                       OPT_BOOL,   &( report_rng                               ) },
    { "report_overheal",                  OPT_BOOL,   &( report_overheal                          ) },
    { "statistics_level",                 OPT_INT,    &( statistics_level                         ) },
    { "separate_stats_by_actions",        OPT_BOOL,    &( separate_stats_by_actions                         ) },
    { NULL, OPT_UNKNOWN, NULL }
  };

  option_t::copy( options, global_options );
}

// sim_t::parse_option ======================================================

bool sim_t::parse_option( const std::string& name,
                          const std::string& value )
{
  if ( canceled ) return false;

  if ( active_player )
    if ( option_t::parse( this, active_player -> options, name, value ) )
      return true;

  if ( option_t::parse( this, options, name, value ) )
    return true;

  return false;
}

// sim_t::parse_options =====================================================

bool sim_t::parse_options( int    _argc,
                           char** _argv )
{
  argc = _argc;
  argv = _argv;

  if ( argc <= 1 ) return false;

  if ( ! parent )
    cache::advance_era();

  overrides.ignore_player_arpen_debuffs = -1; // XXX cough hack hack
  for ( int i=1; i < argc; i++ )
  {
    if ( ! option_t::parse_line( this, argv[ i ] ) )
      return false;
  }
  sim_t::post_parse();

  if ( player_list == NULL )
  {
    errorf( "Nothing to sim!\n" );
    cancel();
    return false;
  }

  if ( parent )
  {
    debug = 0;
    log = 0;
  }
  else if ( ! output_file_str.empty() )
  {
    FILE* f = fopen( output_file_str.c_str(), "w" );
    if ( f )
    {
      output_file = f;
    }
    else
    {
      errorf( "Unable to open output file '%s'\n", output_file_str.c_str() );
      cancel();
      return false;
    }
  }
  if ( debug )
  {
    log = 1;
    print_options();
  }
  if ( log )
  {
    iterations = 1;
    threads = 1;
  }

  return true;
}

// sim_t::cancel ============================================================

void sim_t::cancel()
{
  if ( canceled ) return;

  if ( current_iteration >= 0 )
  {
    errorf( "Simulation has been canceled after %d iterations! (thread=%d)\n", current_iteration+1, thread_index );
  }
  else
  {
    errorf( "Simulation has been canceled during player setup! (thread=%d)\n", thread_index );
  }
  fflush( output_file );

  canceled = 1;

  int num_children = ( int ) children.size();

  for ( int i=0; i < num_children; i++ )
  {
    children[ i ] -> cancel();
  }
}

// sim_t::progress ==========================================================

double sim_t::progress( std::string& phase )
{
  if ( canceled )
  {
    phase = "Canceled";
    return 1.0;
  }

  if ( plot -> num_plot_stats > 0 &&
       plot -> remaining_plot_stats > 0 )
  {
    return plot -> progress( phase );
  }
  else if ( scaling -> calculate_scale_factors &&
            scaling -> num_scaling_stats > 0 &&
            scaling -> remaining_scaling_stats > 0 )
  {
    return scaling -> progress( phase );
  }
  else if ( reforge_plot -> num_stat_combos > 0 )
  {
    return reforge_plot -> progress( phase );
  }
  else if ( current_iteration >= 0 )
  {
    phase = "Simulating";
    return current_iteration / ( double ) iterations;
  }
  else if ( current_slot >= 0 )
  {
    phase = current_name;
    return current_slot / ( double ) SLOT_MAX;
  }

  return 0.0;
}

// sim_t::main ==============================================================

int sim_t::main( int argc, char** argv )
{
  sim_signal_handler_t handler( this );

  thread_t::init();
  http_t::cache_load();
  //dbc_t::init();

  if ( ! parse_options( argc, argv ) )
  {
    errorf( "ERROR! Incorrect option format..\n" );
    cancel();
  }

  if ( canceled ) return 0;

  util_t::fprintf( output_file, "\nSimulationCraft %s-%s for Star Wars: The Old Republic %s %s \n",
                   SC_MAJOR_VERSION, SC_MINOR_VERSION, ptr ? SWTOR_VERSION_PTR : SWTOR_VERSION_LIVE, ( ptr ? "PTR" : "Live" ) );
  fflush( output_file );

  if ( need_to_save_profiles( this ) )
  {
    init();
    util_t::fprintf( stdout, "\nGenerating profiles... \n" ); fflush( stdout );
    report_t::print_profiles( this );
    report_t::print_json_profiles( this );
  }
  else
  {
    if ( max_time <= timespan_t::zero() )
    {
      util_t::fprintf( output_file, "simulationcraft: One of -max_time or -target_health must be specified.\n" );
      exit( 0 );
    }
    if ( abs( vary_combat_length ) >= 1.0 )
    {
      util_t::fprintf( output_file, "|vary_combat_length| >= 1.0.\n" );
      exit( 0 );
    }
    if ( confidence <= 0.0 || confidence >= 1.0 )
    {
      util_t::fprintf( output_file, "Invalid confidence: %f.\n", confidence );
      exit( 0 );
    }

    util_t::fprintf( output_file,
                     "\nSimulating... ( iterations=%d, max_time=%.0f, vary_combat_length=%0.2f, optimal_raid=%d, fight_style=%s )\n",
                     iterations, to_seconds( max_time ), vary_combat_length, optimal_raid, fight_style.c_str() );
    fflush( output_file );

    util_t::fprintf( stdout, "\nGenerating baseline... \n" ); fflush( stdout );

    if ( execute() )
    {
      scaling      -> analyze();
      plot         -> analyze();
      reforge_plot -> analyze();
      util_t::fprintf( stdout, "\nGenerating reports...\n" ); fflush( stdout );
      report_t::print_suite( this );
    }
  }

  if ( output_file != stdout ) fclose( output_file );

  http_t::cache_save();
  thread_t::de_init();
  //dbc_t::de_init();

  return 0;
}

// sim_t::errorf ============================================================

int sim_t::errorf( const char* format, ... )
{
  std::string p_locale = setlocale( LC_CTYPE, NULL );
  setlocale( LC_CTYPE, "" );

  va_list fmtargs;
  va_start( fmtargs, format );

  char buffer[ 1024 ];
  int retcode = vsnprintf( buffer, sizeof( buffer ), format, fmtargs );

  va_end( fmtargs );
  assert( retcode >= 0 );

  fputs( buffer, output_file );
  fputc( '\n', output_file );

  setlocale( LC_CTYPE, p_locale.c_str() );

  error_list.push_back( buffer );
  return retcode;
}
