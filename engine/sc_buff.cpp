// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

namespace { // ANONYMOUS ====================================================

struct expiration_t : public event_t
{
  buff_t* buff;

  expiration_t( sim_t* sim, player_t* p, buff_t* b, timespan_t d ) : event_t( sim, p, b -> name() ), buff( b )
  { sim -> add_event( this, d ); }

  virtual void execute()
  {
    buff -> expiration = 0;
    buff -> expire();
  }
};

struct buff_delay_t : public event_t
{
  double  value;
  buff_t* buff;
  int     stacks;

  buff_delay_t( sim_t* sim, player_t* p, buff_t* b, int stacks, double value ) :
    event_t( sim, p, b -> name() ), value( value ), buff( b ), stacks( stacks )
  {
    timespan_t delay_duration = sim -> gauss( sim -> default_aura_delay, sim -> default_aura_delay_stddev );

    sim -> add_event( this, delay_duration );
  }

  virtual void execute()
  {
    // Add a Cooldown check here to avoid extra processing due to delays
    if ( buff -> cooldown -> remains() ==  timespan_t::zero() )
      buff -> execute( stacks, value );

    buff -> delay = 0;
  }
};

} // ANONYMOUS namespace ====================================================

// ==========================================================================
// BUFF
// ==========================================================================

// buff_t::buff_t ===========================================================

buff_t::buff_t( sim_t*             s,
                const std::string& n,
                int                ms,
                timespan_t         d,
                timespan_t         cd,
                double             ch,
                bool               q,
                bool               r,
                rng_type           rt,
                int                id ) :
  buff_duration( d ), buff_cooldown( cd ), default_chance( ch ),
  name_str( n ), sim( s ), player( 0 ), source( 0 ), initial_source( 0 ),
  max_stack( ms ), aura_id( id ), rt( rt ),
  activated( true ), reverse( r ), constant( false ), quiet( q ),
  uptime_pct()
{
  init();
}

// buff_t::buff_t ===========================================================

buff_t::buff_t( actor_pair_t       p,
                const std::string& n,
                int                ms,
                timespan_t         d,
                timespan_t         cd,
                double             ch,
                bool               q,
                bool               r,
                rng_type           rt,
                int                id,
                bool               act ) :
  buff_duration( d ), buff_cooldown( cd ), default_chance( ch ),
  name_str( n ), sim( p.target -> sim ), player( p.target ), source( p.source ), initial_source( p.source ),
  max_stack( ms ), aura_id( id ), rt( rt ),
  activated( act ), reverse( r ), constant( false ), quiet( q ),
  uptime_pct()
{
  init();
}


// buff_t::parse_options ====================================================

void buff_t::parse_options( va_list vap )
{
  while ( true )
  {
    const char* parm = ( const char * ) va_arg( vap, const char * );
    if ( ! parm ) break;

    if ( ! strcmp( parm, "stacks" ) )
    {
      max_stack = ( int ) va_arg( vap, int );
    }
    else if ( ! strcmp( parm, "chance" ) )
    {
      default_chance = ( double ) va_arg( vap, double );
    }
    else if ( ! strcmp( parm, "duration" ) )
    {
      buff_duration = from_seconds( ( double ) va_arg( vap, double ) );
    }
    else if ( ! strcmp( parm, "cooldown" ) )
    {
      buff_cooldown = from_seconds( ( double ) va_arg( vap, double ) );
    }
    else if ( ! strcmp( parm, "reverse" ) )
    {
      reverse = va_arg( vap, int ) ? true : false;
    }
    else if ( ! strcmp( parm, "quiet" ) )
    {
      quiet = va_arg( vap, int ) ? true : false;
    }
    else if ( ! strcmp( parm, "rng" ) )
    {
      rt = ( rng_type ) va_arg( vap, int );
    }
  }
  va_end( vap );
}

// buff_t::init_buff_shared =============================================================

void buff_t::init_buff_shared()
{
  current_stack = 0;
  current_value = 0;
  last_start = timespan_t_min();
  last_trigger = timespan_t_min();
  start_intervals_sum = timespan_t::zero();
  trigger_intervals_sum = timespan_t::zero();
  iteration_uptime_sum = timespan_t::zero();
  up_count = 0;
  down_count = 0;
  start_intervals = 0;
  trigger_intervals = 0;
  start_count = 0;
  refresh_count = 0;
  trigger_attempts = 0;
  trigger_successes = 0;
  benefit_pct = 0;
  trigger_pct = 0;
  avg_start_interval = 0;
  avg_trigger_interval = 0;
  avg_start = 0;
  avg_refresh = 0;
  constant = false;
  overridden = false;
  expiration = 0;
  delay = 0;

  buff_duration = std::min( buff_duration, from_seconds( sim -> wheel_seconds - 2.0 ) );

  if ( max_stack >= 0 )
  {
    stack_occurrence.resize( max_stack + 1 );
    stack_react_time.resize( max_stack + 1 );
    stack_uptime.resize( max_stack + 1, buff_uptime_t( sim ) );
  }
}
// buff_t::init =============================================================

void buff_t::init()
{
  init_buff_shared();

  buff_t** tail=0;

  if ( initial_source )
  {
    cooldown = initial_source-> get_cooldown( "buff_" + name_str );
    if ( initial_source != player )
      name_str = name_str + ':' + initial_source->name_str;
    rng = initial_source-> get_rng( name_str, rt );
    tail = &( player -> buff_list );
  }
  else
  {
    cooldown = sim -> get_cooldown( "buff_" + name_str );
    rng = sim -> get_rng( name_str, rt );
    tail = &( sim -> buff_list );
  }
  cooldown -> duration = buff_cooldown;

  while ( *tail && name_str > ( ( *tail ) -> name_str ) )
  {
    tail = &( ( *tail ) -> next );
  }
  next = *tail;
  *tail = this;
}


// buff_t::init_buff_t_ =====================================================

void buff_t::init_buff_t_()
{
  // FIXME! For the love of all that is holy.... FIXME!
  // This routine will disappear once data-access rework is complete

  init_buff_shared();

  buff_t** tail = &(  player -> buff_list );

  while ( *tail && name_str > ( ( *tail ) -> name_str ) )
  {
    tail = &( ( *tail ) -> next );
  }
  next = *tail;
  *tail = this;
}

// buff_t::~buff_t ==========================================================

buff_t::~buff_t()
{}

// buff_t::combat_begin ==========================================================

void buff_t::combat_begin()
{
  iteration_uptime_sum = timespan_t::zero();
}

// buff_t::combat_end ==========================================================

void buff_t::combat_end()
{
  if ( player )
    uptime_pct.add( player -> iteration_fight_length != timespan_t::zero() ? 100.0 * iteration_uptime_sum / player -> iteration_fight_length : 0 );
  else
    uptime_pct.add( sim -> current_time != timespan_t::zero() ? 100.0 * iteration_uptime_sum / sim -> current_time : 0 );
}

// buff_t::may_react ========================================================

bool buff_t::may_react( int stack )
{
  if ( current_stack == 0    ) return false;
  if ( stack > current_stack ) return false;
  if ( stack < 1             ) return false;

  if ( stack > max_stack ) return false;

  timespan_t occur = stack_occurrence[ stack ];

  if ( occur <= timespan_t::zero() ) return true;

  return sim -> current_time > stack_react_time[ stack ];
}

// buff_t::stack_react ======================================================

int buff_t::stack_react()
{
  int stack = 0;

  for ( int i=1; i <= current_stack; i++ )
  {
    if ( stack_react_time[ i ] > sim -> current_time ) break;
    stack++;
  }

  return stack;
}

// buff_t::remains ==========================================================

timespan_t buff_t::remains()
{
  if ( current_stack <= 0 )
  {
    return timespan_t::zero();
  }
  if ( expiration )
  {
    return expiration -> occurs() - sim -> current_time;
  }
  return timespan_t_min();
}

// buff_t::remains_gt =======================================================

bool buff_t::remains_gt( timespan_t time )
{
  timespan_t time_remaining = remains();

  if ( time_remaining == timespan_t::zero() ) return false;

  if ( time_remaining == timespan_t_min() ) return true;

  return ( time_remaining > time );
}

// buff_t::remains_lt =======================================================

bool buff_t::remains_lt( timespan_t time )
{
  timespan_t time_remaining = remains();

  if ( time_remaining == timespan_t_min() ) return false;

  return ( time_remaining < time );
}

// buff_t::trigger ==========================================================

bool buff_t::trigger( action_t* a,
                      int       stacks,
                      double    value )
{
  double chance = default_chance;

  if ( chance < 0 ) chance = a -> ppm_proc_chance( -chance );

  return trigger( stacks, value, chance );
}

// buff_t::trigger ==========================================================

bool buff_t::trigger( int    stacks,
                      double value,
                      double chance )
{
  if ( max_stack == 0 || chance == 0 ) return false;

  if ( cooldown -> remains() != timespan_t::zero() )
    return false;

  if ( player && player -> sleeping )
    return false;

  trigger_attempts++;

  if ( chance < 0 ) chance = default_chance;

  if ( ! rng -> roll( chance ) )
    return false;

  if ( ! activated && player && player -> in_combat && sim -> default_aura_delay > timespan_t::zero() )
  {
    // In-game, procs that happen "close to eachother" are usually delayed into the
    // same time slot. We roughly model this by allowing procs that happen during the
    // buff's already existing delay period to trigger at the same time as the first
    // delayed proc will happen.
    if ( delay )
    {
      buff_delay_t& d = dynamic_cast<buff_delay_t&>( *delay );
      d.stacks += stacks;
      d.value = value;
    }
    else
      delay = new ( sim ) buff_delay_t( sim, player, this, stacks, value );
  }
  else
    execute( stacks, value );

  return true;
}

// buff_t::execute ==========================================================

void buff_t::execute( int stacks, double value )
{
  if ( last_trigger > timespan_t::zero() )
  {
    trigger_intervals_sum += sim -> current_time - last_trigger;
    trigger_intervals++;
  }
  last_trigger = sim -> current_time;

  if ( reverse )
  {
    decrement( stacks, value );
  }
  else
  {
    increment( stacks, value );
  }

  // new buff cooldown impl
  if ( cooldown -> duration > timespan_t::zero() )
  {
    if ( sim -> debug )
      log_t::output( sim, "%s starts buff %s cooldown (%s) with duration %.2f",
                     ( source ? source -> name() : "someone" ), name(), cooldown -> name(), to_seconds( cooldown -> duration ) );

    cooldown -> start();
  }

  trigger_successes++;
}

// buff_t::increment ========================================================

void buff_t::increment( int    stacks,
                        double value )
{
  if ( overridden ) return;

  if ( max_stack == 0 ) return;

  if ( current_stack == 0 )
  {
    start( stacks, value );
  }
  else
  {
    refresh( stacks, value );
  }
}

// buff_t::decrement ========================================================

void buff_t::decrement( int    stacks,
                        double value )
{
  if ( overridden ) return;

  if ( max_stack == 0 || current_stack <= 0 ) return;

  if ( stacks == 0 || current_stack <= stacks )
  {
    expire();
  }
  else
  {
    if ( static_cast<std::size_t>( current_stack ) < stack_uptime.size() )
      stack_uptime[ current_stack ].update( false );

    current_stack -= stacks;
    if ( value >= 0 ) current_value = value;

    if ( static_cast<std::size_t>( current_stack ) < stack_uptime.size() )
      stack_uptime[ current_stack ].update( true );

    if ( sim -> debug )
      log_t::output( sim, "buff %s decremented by %d to %d stacks",
                     name(), stacks, current_stack );
  }
}

// buff_t::extend_duration ==================================================

void buff_t::extend_duration( player_t* p, timespan_t extra_seconds )
{
  assert( expiration );
  assert( to_seconds( extra_seconds ) < sim -> wheel_seconds );

  if ( extra_seconds > timespan_t::zero() )
  {
    expiration -> reschedule( expiration -> remains() + extra_seconds );

    if ( sim -> debug )
      log_t::output( sim, "%s extends buff %s by %.1f seconds. New expiration time: %.1f",
                     p -> name(), name(), to_seconds( extra_seconds ), to_seconds( expiration -> occurs() ) );
  }
  else if ( extra_seconds < timespan_t::zero() )
  {
    timespan_t reschedule_time = expiration -> remains() + extra_seconds;

    if ( reschedule_time <= timespan_t::zero() )
    {
      // When Strength of Soul removes the Weakened Soul debuff completely,
      // there's a delay before the server notifies the client. Modeling
      // this effect as a world lag.
      timespan_t lag, dev;

      lag = p -> world_lag_override ? p -> world_lag : sim -> world_lag;
      dev = p -> world_lag_stddev_override ? p -> world_lag_stddev : sim -> world_lag_stddev;
      reschedule_time = p -> rngs.lag_world -> gauss( lag, dev );
    }

    event_t::cancel( expiration );

    expiration = new ( sim ) expiration_t( sim, player, this, reschedule_time );

    if ( sim -> debug )
      log_t::output( sim, "%s decreases buff %s by %.1f seconds. New expiration time: %.1f",
                     p -> name(), name(), to_seconds( extra_seconds ), to_seconds( expiration -> occurs() ) );
  }
}

// buff_t::start_expiration

void buff_t::start_expiration( timespan_t t )
{
  if ( expiration )
    return;

  if ( t > timespan_t::zero() )
  {
    expiration = new ( sim ) expiration_t( sim, player, this, t );
  }
}

// buff_t::start ============================================================

void buff_t::start( int    stacks,
                    double value )
{
  if ( max_stack == 0 ) return;

  if ( current_stack != 0 )
  {
    sim -> errorf( "buff_t::start assertion error current_stack is not zero, buff %s from %s.\n", name(), player -> name() );
    assert( 0 );
  }

  if ( sim -> current_time <= from_seconds( 0.01 ) ) constant = true;

  start_count++;

  bump( stacks, value );

  if ( last_start >= timespan_t::zero() )
  {
    start_intervals_sum += sim -> current_time - last_start;
    start_intervals++;
  }
  last_start = sim -> current_time;

  if ( buff_duration > timespan_t::zero() )
  {
    expiration = new ( sim ) expiration_t( sim, player, this, buff_duration );
  }
}

// buff_t::refresh ==========================================================

void buff_t::refresh( int    stacks,
                      double value )
{
  if ( max_stack == 0 ) return;

  refresh_count++;

  bump( stacks, value );

  if ( buff_duration > timespan_t::zero() )
  {
    assert( expiration );
    if ( expiration -> occurs() < sim -> current_time + buff_duration )
    {
      expiration -> reschedule( buff_duration );
    }
  }
}

// buff_t::bump =============================================================

void buff_t::bump( int    stacks,
                   double value )
{
  if ( max_stack == 0 ) return;

  if ( value >= 0 ) current_value = value;

  if ( max_stack < 0 )
  {
    current_stack += stacks;
  }
  else if ( current_stack < max_stack )
  {
    int before_stack = current_stack;
    stack_uptime[ current_stack ].update( false );

    current_stack += stacks;
    if ( current_stack > max_stack )
      current_stack = max_stack;

    stack_uptime[ current_stack ].update( true );

    aura_gain();

    timespan_t now = sim -> current_time;
    timespan_t react = now + ( player ? ( player -> total_reaction_time() ) : sim -> reaction_time );
    for ( int i=before_stack+1; i <= current_stack; i++ )
    {
      stack_occurrence[ i ] = now;
      stack_react_time[ i ] = react;
    }
  }
}

// buff_t::override =========================================================

void buff_t::override( int    stacks,
                       double value )
{
  if ( max_stack == 0 ) return;
  if ( current_stack != 0 )
  {
    sim -> errorf( "buff_t::override assertion error current_stack is not zero, buff %s from %s.\n", name(), player -> name() );
    assert( 0 );
  }

  buff_duration = timespan_t::zero();
  start( stacks, value );
  overridden = true;
}

// buff_t::expire ===========================================================

void buff_t::expire()
{
  if ( current_stack <= 0 ) return;
  event_t::cancel( expiration );
  source = 0;
  current_stack = 0;
  current_value = 0;
  aura_loss();
  if ( last_start >= timespan_t::zero() )
  {
    iteration_uptime_sum += sim -> current_time - last_start;
  }

  if ( sim -> target -> resource_base[ RESOURCE_HEALTH ] == 0 ||
       sim -> target -> resource_current[ RESOURCE_HEALTH ] > 0 )
    if ( ! overridden )
    {
      constant = false;
    }

  for ( unsigned int i = 0; i < stack_uptime.size(); i++ )
    stack_uptime[ i ].update( false );
}

// buff_t::predict ==========================================================

void buff_t::predict()
{
  // Guarantee that may_react() will return true if the buff is present.
  std::fill( &stack_occurrence[ 0 ], &stack_occurrence[ current_stack + 1 ], timespan_t_min() );
  std::fill( &stack_react_time[ 0 ], &stack_react_time[ current_stack + 1 ], timespan_t_min() );
}

// buff_t::aura_gain ========================================================

void buff_t::aura_gain()
{
  if ( sim -> log )
  {
    std::string s = name();
    if ( max_stack >= 0 )
      s += ( boost::format( "_%d" ) % current_stack ).str();

    if ( player )
      player -> aura_gain( s.c_str(), current_value );
    else
      sim -> aura_gain( s.c_str(), aura_id );
  }
}

// buff_t::aura_loss ========================================================

void buff_t::aura_loss()
{
  if ( player )
  {
    player -> aura_loss( name(), current_value );
  }
  else
  {
    sim -> aura_loss( name(), aura_id );
  }
}

// buff_t::reset ============================================================

void buff_t::reset()
{
  event_t::cancel( delay );
  cooldown -> reset();
  expire();
  last_start = timespan_t_min();
  last_trigger = timespan_t_min();
}

// buff_t::merge ============================================================

void buff_t::merge( const buff_t* other )
{
  start_intervals_sum   += other -> start_intervals_sum;
  trigger_intervals_sum += other -> trigger_intervals_sum;
  up_count              += other -> up_count;
  down_count            += other -> down_count;
  start_intervals       += other -> start_intervals;
  trigger_intervals     += other -> trigger_intervals;
  start_count           += other -> start_count;
  refresh_count         += other -> refresh_count;
  trigger_attempts      += other -> trigger_attempts;
  trigger_successes     += other -> trigger_successes;

  uptime_pct.merge( other -> uptime_pct );

  assert( stack_uptime.size() == other -> stack_uptime.size() );
  for ( unsigned int i = 0; i < stack_uptime.size(); i++ )
    stack_uptime[ i ].merge ( other -> stack_uptime[ i ] );
}

// buff_t::analyze ==========================================================

void buff_t::analyze()
{
  if ( up_count > 0 )
  {
    benefit_pct = 100.0 * up_count / ( up_count + down_count );
  }
  if ( trigger_attempts > 0 )
  {
    trigger_pct = 100.0 * trigger_successes / trigger_attempts;
  }
  if ( start_intervals > 0 )
  {
    avg_start_interval = to_seconds( start_intervals_sum ) / start_intervals;
  }
  if ( trigger_intervals > 0 )
  {
    avg_trigger_interval = to_seconds( trigger_intervals_sum ) / trigger_intervals;
  }
  avg_start   =   start_count / ( double ) sim -> iterations;
  avg_refresh = refresh_count / ( double ) sim -> iterations;
  uptime_pct.analyze();

  for( auto& i : stack_uptime )
    i.analyze();
}

// buff_t::find =============================================================

buff_t* buff_t::find( buff_t* b, const std::string& name_str )
{
  while ( b && name_str != b -> name_str )
    b = b -> next;

  return b;
}


// buff_t::create_expression ================================================

expr_ptr buff_t::create_expression( const std::string& type )
{
  if ( type == "remains" )
    return make_expr( "buff_" + type, [this]{ return to_seconds( remains() ); } );

  if ( type == "cooldown_remains" )
    return make_expr( "buff_" + type, [this]{ return to_seconds( cooldown -> remains() ); } );

  if ( type == "up" )
    return make_expr( "buff_" + type, [this]{ return check() > 0; } );

  if ( type == "down" )
    return make_expr( "buff_" + type, [this]{ return check() <= 0; } );

  if ( type == "stack" )
    return make_expr( "buff_" + type, [this]{ return check(); });

  if ( type == "value" )
    return make_expr( "buff_" + type, [this]{ return value(); });

  if ( type == "react" )
    return make_expr( "buff_" + type, [this]{ return stack_react(); });

  if ( type == "cooldown_react" )
    return make_expr( "buff_" + type, [this]{ return ( check() && ! may_react() ) ? 0.0 : to_seconds( cooldown -> remains() ); });

  return nullptr;
}

// ==========================================================================
// STAT_BUFF
// ==========================================================================

// stat_buff_t::stat_buff_t =================================================

stat_buff_t::stat_buff_t( player_t*          p,
                          const std::string& n,
                          int                st,
                          double             a,
                          int                ms,
                          timespan_t         d,
                          timespan_t         cd,
                          double             ch,
                          bool               q,
                          bool               r,
                          rng_type           rt,
                          int                id,
                          bool               act ) :
  buff_t( p, n, ms, d, cd, ch, q, r, rt, id, act ), amount( a ), stat( st )
{
}

// stat_buff_t::bump ========================================================

void stat_buff_t::bump( int    stacks,
                        double value )
{
  if ( max_stack == 0 ) return;
  if ( value > 0 )
  {
    amount = value;
  }
  buff_t::bump( stacks );
  double delta = amount * current_stack - current_value;
  if ( delta > 0 )
  {
    player -> stat_gain( stat, delta, 0, 0, buff_duration > timespan_t::zero() );
    current_value += delta;
  }
  else
    assert( delta == 0 );
}

// stat_buff_t::decrement ===================================================

void stat_buff_t::decrement( int    stacks,
                             double /* value */ )
{
  if ( max_stack == 0 ) return;
  if ( stacks == 0 || current_stack <= stacks )
  {
    expire();
  }
  else
  {
    double delta = amount * stacks;
    player -> stat_loss( stat, delta, 0, buff_duration > timespan_t::zero() );
    current_stack -= stacks;
    current_value -= delta;
  }
}

// stat_buff_t::expire ======================================================

void stat_buff_t::expire()
{
  if ( current_stack > 0 )
  {
    player -> stat_loss( stat, current_value, 0, buff_duration > timespan_t::zero() );
    buff_t::expire();
  }
}

// ==========================================================================
// COST_REDUCTION_BUFF
// ==========================================================================

// cost_reduction_buff_t::cost_reduction_buff_t =============================

cost_reduction_buff_t::cost_reduction_buff_t( player_t*          p,
                                              const std::string& n,
                                              int                sch,
                                              double             a,
                                              int                ms,
                                              timespan_t         d,
                                              timespan_t         cd,
                                              double             ch,
                                              bool               re,
                                              bool               q,
                                              bool               r,
                                              rng_type           rt,
                                              int                id,
                                              bool               act ) :
  buff_t( p, n, ms, d, cd, ch, q, r, rt, id, act ), amount( a ), school( sch ), refreshes( re )
{
}

// cost_reduction_buff_t::bump ==============================================

void cost_reduction_buff_t::bump( int    stacks,
                                  double value )
{
  if ( max_stack == 0 ) return;
  if ( value > 0 )
  {
    amount = value;
  }
  buff_t::bump( stacks );
  double delta = amount * current_stack - current_value;
  if ( delta > 0 )
  {
    player -> cost_reduction_gain( school, delta );
    current_value += delta;
  }
  else assert( delta == 0 );
}

// cost_reduction_buff_t::decrement =========================================

void cost_reduction_buff_t::decrement( int    stacks,
                                       double /* value */ )
{
  if ( max_stack == 0 ) return;
  if ( stacks == 0 || current_stack <= stacks )
  {
    expire();
  }
  else
  {
    double delta = amount * stacks;
    player -> cost_reduction_loss( school, delta );
    current_stack -= stacks;
    current_value -= delta;
  }
}

// cost_reduction_buff_t::expire ============================================

void cost_reduction_buff_t::expire()
{
  if ( current_stack > 0 )
  {
    player -> cost_reduction_loss( school, current_value );
    buff_t::expire();
  }
}

// cost_reduction_buff_t::refresh ===========================================

void cost_reduction_buff_t::refresh( int    stacks,
                                     double value )
{
  if ( ! refreshes )
  {
    if ( max_stack == 0 ) return;

    refresh_count++;

    bump( stacks, value );
    return;
  }

  buff_t::refresh( stacks, value );
}

// ==========================================================================
// DEBUFF
// ==========================================================================

// debuff_t::debuff_t =======================================================

debuff_t::debuff_t( player_t*          p,
                    const std::string& n,
                    int                ms,
                    timespan_t         d,
                    timespan_t         cd,
                    double             ch,
                    bool               q,
                    bool               r,
                    rng_type           rt,
                    int                id ) :
  buff_t( p, n, ms, d, cd, ch, q, r, rt, id )
{
}
