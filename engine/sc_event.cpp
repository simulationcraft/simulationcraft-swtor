// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// ==========================================================================
// Event Memory Management
// ==========================================================================

// event_freelist_t::allocate ===============================================

void* event_freelist_t::allocate( std::size_t size )
{
  // This override of ::new is ONLY for event_t memory management!
  static const std::size_t SIZE = 2 * sizeof( event_t );
  assert( SIZE > size ); ( void )size;

  free_event_t* new_event = list;

  if ( new_event )
  {
    list = list -> next;
  }
  else
  {
    new_event = static_cast<free_event_t*>( ::operator new( SIZE ) );
  }

  return new_event;
}

// event_freelist_t::deallocate =============================================

void event_freelist_t::deallocate( void* p )
{
  free_event_t* fe = new( p ) free_event_t;
  fe -> next = list;
  list = fe;
}

// event_freelist_t::~event_freelist_t ======================================

event_freelist_t::~event_freelist_t()
{ list_dispose( list ); }

// ==========================================================================
// Event
// ==========================================================================

// event_t::new =============================================================

void* event_t::operator new( std::size_t /* size */ ) throw()
{
  util_t::fprintf( stderr, "All events must be allocated via: new (sim) event_class_name_t()\n" );
  fflush( stderr );
  assert( 0 );
  return NULL;
}

void event_t::reschedule( timespan_t new_time )
{
  reschedule_time = sim -> current_time + new_time;

  if ( sim -> debug ) log_t::output( sim, "Rescheduling event %s (%d) from %.2f to %.2f", name, id, to_seconds( time ), to_seconds( reschedule_time ) );

//  if ( ! strcmp( name, "Rabid Expiration" ) ) assert( false );
}

// event_t::cancel_ =========================================================

void event_t::cancel_( event_t* e )
{
  assert( e );
  if ( e -> player && ! e -> canceled )
  {
    e -> player -> events--;
    if ( e -> player -> events < 0 )
    {
      e -> sim -> errorf( "event_t::cancel assertion error: e -> player -> events < 0, event %s from %s.\n", e -> name, e -> player -> name() );
      assert( 0 );
    }
  }
  e -> canceled = 1;
}

// event_t::early_ ==========================================================

void event_t::early_( event_t* e )
{
  assert( e );
  if ( e -> player && ! e -> canceled )
  {
    e -> player -> events--;
    assert( e -> player -> events >= 0 );
  }
  e -> canceled = 1;
  e -> execute();
}

// event_t::execute =========================================================

void event_t::execute()
{
  util_t::printf( "event_t::execute() called for event \"%s\"\n",
                  name ? name : "(no name)" );
  assert( 0 );
}

// ==========================================================================
// Player Ready Event
// ==========================================================================

// player_ready_event_t::player_ready_event_t ===============================

player_ready_event_t::player_ready_event_t( player_t* p,
                                            timespan_t delta_time ) :
  event_t( p, "Player-Ready" )
{
  if ( sim -> debug ) log_t::output( sim, "New Player-Ready Event: %s", p -> name() );
  sim -> add_event( this, delta_time );
}

// player_ready_event_t::execute ============================================

void player_ready_event_t::execute()
{
  // Player that's checking for off gcd actions to use, cancels that checking
  // when there's a ready event firing.
  if ( player -> off_gcd )
    event_t::cancel( player -> off_gcd );

  if ( ! player -> execute_action() )
  {
    timespan_t x = player -> available();

    player -> schedule_ready( x, true );
    // Waiting Debug
    if ( sim -> debug )
    {
      log_t::output( sim, "%s is waiting for %.4f resource=%.2f", player -> name(), to_seconds( x ), player -> resource_current[ player -> primary_resource() ] );
    }
  }
}

// ==========================================================================
// Player GCD Event
// ==========================================================================

// player_gcd_event_t::player_ready_event_t =================================

player_gcd_event_t::player_gcd_event_t( player_t* p,
                                        timespan_t delta_time ) :
  event_t( p, "Player-Ready-GCD" )
{
  if ( sim -> debug ) log_t::output( sim, "New Player-Ready-GCD Event: %s", p -> name() );
  sim -> add_event( this, delta_time );
}

// player_gcd_event_t::execute ==============================================

void player_gcd_event_t::execute()
{
  for ( action_t* a : player -> off_gcd_actions )
  {
    if ( a -> ready() )
    {
      player -> last_foreground_action = a;

      a -> execute();
      player -> iteration_executed_foreground_actions++;
      if ( a -> marker ) player -> action_sequence += a -> marker;
      if ( ! a -> label_str.empty() )
        player -> action_map[ a -> label_str ] += 1;
    }
  }

  assert( player -> off_gcd == this );
  player -> off_gcd = new ( sim ) player_gcd_event_t( player, from_seconds( 0.1 ) );
}

// ==========================================================================
// Action Execute Event
// ==========================================================================

// action_execute_event_t::action_execute_event_t ===========================

action_execute_event_t::action_execute_event_t( action_t* a,
                                                timespan_t time_to_execute ) :
  event_t( a -> player, "Action-Execute" ), action( a )
{
  if ( sim -> debug )
    log_t::output( sim, "New Action Execute Event: %s %s %.1f",
                   player -> name(), a -> name(), to_seconds( time_to_execute ) );
  sim -> add_event( this, time_to_execute );
}

// action_execute_event_t::execute ==========================================

void action_execute_event_t::execute()
{
  action -> execute_event = 0;
  action -> execute();

  if ( ! action -> background &&
       ! player -> channeling )
  {
    if ( player -> readying ) fprintf( sim -> output_file, "Danger Will Robinson!  Danger!  action %s\n", action -> name() );

    player -> schedule_ready( timespan_t::zero() );
  }

  if ( player -> off_gcd_actions.size() == 0 )
    return;

  // Kick off the during-gcd checker, first run is immediately after
  if ( player -> off_gcd )
    event_t::cancel( player -> off_gcd );

  player -> off_gcd = new ( sim ) player_gcd_event_t( player, timespan_t::zero() );
}

// ==========================================================================
// DoT Tick Event
// ==========================================================================

// dot_tick_event_t::dot_tick_event_t =======================================

dot_tick_event_t::dot_tick_event_t( dot_t* d,
                                    timespan_t time_to_tick ) :
  event_t( d -> player, "DoT Tick" ), dot( d )
{
  sim_t* sim = d -> player -> sim;

  if ( sim -> debug )
    log_t::output( sim, "New DoT Tick Event: %s %s %d-of-%d %.2f",
                   player -> name(), dot -> name(), dot -> current_tick + 1, dot -> num_ticks, to_seconds( time_to_tick ) );

  sim -> add_event( this, time_to_tick );
}

// dot_tick_event_t::execute ================================================

void dot_tick_event_t::execute()
{
  if ( dot -> current_tick >= dot -> num_ticks )
  {
    sim -> errorf( "Player %s has corrupt tick (%d of %d) event on action %s!\n",
                   player -> name(), dot -> current_tick, dot -> num_ticks, dot -> name() );
    sim -> cancel();
  }

  assert( dot -> tick_event == this );
  dot -> tick_event = nullptr;
  dot -> current_tick++;

  bool do_tick = false;
  if ( dot -> action -> player -> skill < 1.0 &&
       dot -> action -> channeled &&
       dot -> current_tick == dot -> num_ticks )
  {
    if ( sim -> roll( dot -> action -> player -> skill ) )
    {
      do_tick = true;
    }
  }
  else // No skill-check required
  {
    do_tick = true;
  }

  if ( do_tick && dot -> action -> td.weapon && dot -> action -> channeled )
  {
    // do a hit check each time chanelling a weapon skill
    dot -> action -> calculate_result();
    do_tick = dot -> action -> result_is_hit();

    if ( ! do_tick && sim -> log )
    {
      log_t::output( sim, "%s %s ticks (%d of %d) %s (miss)",
                     dot -> action -> player -> name(), dot -> action -> name(),
                     dot -> current_tick, dot -> num_ticks,
                     dot -> action -> target -> name() );
    }
  }

  if ( do_tick )
  {
    dot -> action -> tick( dot );
  }

  if ( dot -> action -> channeled && ( dot -> ticks() > 0 ) )
  {
    auto& expr = dot -> action -> interrupt_if_expr;
    if ( expr && expr -> success() )
      dot -> current_tick = dot -> num_ticks;
    else if ( dot -> action -> interrupt )
    {
      // Interrupt if any higher priority action is ready.
      for ( action_t* a = dot -> action -> player -> action_list; a != dot -> action; a = a -> next )
      {
        if ( a -> background ) continue;
        if ( a -> ready() )
        {
          dot -> current_tick = dot -> num_ticks;
          break;
        }
      }
    }
  }

  if ( dot -> current_tick == dot -> num_ticks )
  {
    dot -> time_to_tick = timespan_t::zero();
    dot -> action -> last_tick( dot );

    if ( dot -> action -> channeled && ! dot -> action -> background )
    {
      if ( dot -> action -> player -> readying ) fprintf( sim -> output_file, "Danger Will Robinson!  Danger!  %s\n", dot -> name() );

      dot -> action -> player -> schedule_ready( timespan_t::zero() );
    }
  }
  else dot -> schedule_tick();
}

// ==========================================================================
// Action Travel Event
// ==========================================================================

// action_travel_event_t::action_travel_event_t =============================

action_travel_event_t::action_travel_event_t( player_t* t,
                                              action_t* a,
                                              timespan_t time_to_travel ) :
  event_t( a -> player, "Action Travel" ), action( a ), target( t ),
  result( a -> result ), damage( a -> direct_dmg )
{
  if ( sim -> debug )
    log_t::output( sim, "New Action Travel Event: %s %s %.2f",
                   player -> name(), a -> name(), to_seconds( time_to_travel ) );

  sim -> add_event( this, time_to_travel );
}

// action_travel_event_t::execute ===========================================

void action_travel_event_t::execute()
{
  action -> impact( target, result, damage );
  if ( action -> travel_event == this )
    action -> travel_event = nullptr;
}

// ==========================================================================
// Regen Event
// ==========================================================================

// regen_event_t::regen_event_t =============================================

regen_event_t::regen_event_t( sim_t* sim ) :
  event_t( sim, "Regen Event" )
{
  if ( sim -> debug ) log_t::output( sim, "New Regen Event" );
  sim -> add_event( this, sim -> regen_periodicity );
}

// regen_event_t::execute ===================================================

void regen_event_t::execute()
{
  for ( size_t i = 0; i < sim -> actor_list.size(); i++ )
  {
    player_t* p = sim -> actor_list[i];
    if ( p -> sleeping ) continue;
    if ( p -> primary_resource() == RESOURCE_NONE ) continue;

    p -> regen( sim -> regen_periodicity );
  }

  new ( sim ) regen_event_t( sim );
}
