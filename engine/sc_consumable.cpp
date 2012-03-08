// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// ==========================================================================
// Consumable
// ==========================================================================


struct stim_t : public action_t
{
  int type;
  gain_t* gain;

  stim_t( player_t* p, const std::string& options_str ) :
    action_t( ACTION_USE, "stim", p ), type( STIM_NONE )
  {
    std::string type_str;

    option_t options[] =
    {
      { "type", OPT_STRING, &type_str },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = timespan_t::zero;
    harmful = false;
    for ( int i=0; i < STIM_MAX; i++ )
    {
      if ( type_str == util_t::stim_type_string( i ) )
      {
        type = i;
        break;
      }
    }
    if ( type == STIM_NONE )
    {
      sim -> errorf( "Player %s attempting to use sim of type '%s', which is not supported.\n",
                     player -> name(), type_str.c_str() );
      sim -> cancel();
    }
    gain = p -> get_gain( "stim" );

  }

  virtual void execute()
  {
    player_t* p = player;
    if ( sim -> log ) log_t::output( sim, "%s uses Stim %s", p -> name(), util_t::stim_type_string( type ) );
    p -> stim = type;
    switch ( type )
    {
    case STIM_EXOTECH_RESOLVE:
      p -> stat_gain( STAT_WILLPOWER, 128, gain, this );
      p -> stat_gain( STAT_POWER, 52, gain, this );
      break;

    case STIM_RAKATA_RESOLVE:
      p -> stat_gain( STAT_WILLPOWER, 112, gain, this );
      p -> stat_gain( STAT_POWER, 46, gain, this );
      break;

    default: assert( 0 );
    }

  }

  virtual bool ready()
  { return ( player -> stim == STIM_NONE ); }
};

// ==========================================================================
// Power Potion
// ==========================================================================

struct power_potion_t : public action_t
{
  double amount;

  power_potion_t( player_t* p, const std::string& options_str ) :
    action_t( ACTION_USE, "power_potion", p ),
    amount( 535.0 ) // Exotech Attack Adrenal
  {
    option_t options[] =
    {
      { "amount", OPT_FLT, &amount },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = timespan_t::zero;
    harmful = false;
    cooldown = p -> get_cooldown( "potion" );
    cooldown -> duration = timespan_t::from_seconds( 180.0 );
  }

  virtual void execute()
  {
    if ( player -> in_combat )
    {
      player -> buffs.power_potion -> trigger( 1, amount );
    }
    else
    {
      cooldown -> duration -= timespan_t::from_seconds( 5.0 );
      player -> buffs.power_potion -> buff_duration -= timespan_t::from_seconds( 5.0 );
      player -> buffs.power_potion -> trigger( 1, amount);
      cooldown -> duration += timespan_t::from_seconds( 5.0 );
      player -> buffs.power_potion -> buff_duration += timespan_t::from_seconds( 5.0 );

    }

    if ( sim -> log ) log_t::output( sim, "%s uses %s", player -> name(), name() );


    if ( player -> in_combat ) player -> potion_used = 1;
    update_ready();
  }

  virtual bool ready()
  {
    if ( ! player -> in_combat && player -> use_pre_potion <= 0 )
      return false;

    return action_t::ready();
  }
};

// ==========================================================================
// consumable_t::create_action
// ==========================================================================

action_t* consumable_t::create_action( player_t*          p,
                                       const std::string& name,
                                       const std::string& options_str )
{
  if ( name == "stim"                 ) return new                 stim_t( p, options_str );
  if ( name == "power_potion"         ) return new         power_potion_t( p, options_str );

  return 0;
}
