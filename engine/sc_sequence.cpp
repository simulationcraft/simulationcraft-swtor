// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// ==========================================================================
// Sequence Action
// ==========================================================================

// sequence_t::sequence_t ===================================================

sequence_t::sequence_t( player_t* p, const std::string& sub_action_str ) :
  action_t( ACTION_SEQUENCE, "default", p ),
  current_action( -1 ), restarted( false ), last_restart( timespan_t::min() )
{
  trigger_gcd = timespan_t::zero();

  std::vector<std::string> splits = split( sub_action_str, ':' );
  if ( ! splits.empty() )
  {
    option_t options[] =
    {
      { "name", OPT_STRING,  &name_str },
      { NULL,   OPT_UNKNOWN, NULL }
    };
    parse_options( options, splits[ 0 ] );
  }

  // First token is sequence options, so skip
  for ( size_t i=1; i < splits.size(); ++i )
  {
    std::string::size_type cut_pt = splits[ i ].find( ',' );
    std::string action_name( splits[ i ], 0, cut_pt );
    std::string action_options;

    if ( cut_pt != std::string::npos )
      action_options.assign( splits[ i ], cut_pt + 1, std::string::npos );

    action_t* a = p -> create_action( action_name, action_options );
    if ( ! a )
    {
      sim -> errorf( "Player %s has unknown sequence action: %s\n", p -> name(), splits[ i ].c_str() );
      sim -> cancel();
      continue;
    }

    a -> sequence = true;
    sub_actions.push_back( a );
  }
}

// sequence_t::schedule_execute =============================================

void sequence_t::schedule_execute()
{
  assert( 0 <= current_action && static_cast<std::size_t>( current_action ) < sub_actions.size() );
  if ( sim -> log ) log_t::output( sim, "Player %s executes Schedule %s action #%d \"%s\"", player -> name(), name(), current_action, sub_actions[ current_action ] -> name() );
  sub_actions[ current_action++ ] -> schedule_execute();
  // No longer restarted
  restarted = false;
}

// sequence_t::reset ========================================================

void sequence_t::reset()
{
  action_t::reset();
  if ( current_action == -1 )
  {
    for ( action_t* a : sub_actions )
    {
      if ( a -> wait_on_ready == -1 )
        a -> wait_on_ready = wait_on_ready;
    }
  }
  current_action = 0;
  restarted = false;
  last_restart = timespan_t::min();
}

// sequence_t::ready ========================================================

bool sequence_t::ready()
{
  if ( sub_actions.empty() ) return false;

  wait_on_ready = 0;

  for ( ; current_action < static_cast<int>( sub_actions.size() ); ++current_action )
  {
    action_t* a = sub_actions[ current_action ];

    if ( a -> ready() )
      return true;

    if ( a -> wait_on_ready == 1 )
    {
      wait_on_ready = 1;
      return false;
    }
  }

  return false;
}
