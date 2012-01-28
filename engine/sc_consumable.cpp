// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

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
    double willpower = 0, power = 0;
    switch ( type )
    {

    case STIM_RAKATA_RESOLVE:
      willpower = 136.0; power = 56;
      p -> stat_gain( STAT_WILLPOWER, willpower, gain, this );
      p -> stat_gain( STAT_POWER, power, gain, this );
      break;

    default: assert( 0 );
    }

  }

  virtual bool ready()
  {
    return( player -> stim           ==  STIM_NONE );
  }
};

// ==========================================================================
// Food
// ==========================================================================

struct food_t : public action_t
{
  int type;
  gain_t* gain;

  food_t( player_t* p, const std::string& options_str ) :
    action_t( ACTION_USE, "food", p ), type( FOOD_NONE )
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
    for ( int i=0; i < FOOD_MAX; i++ )
    {
      if ( type_str == util_t::food_type_string( i ) )
      {
        type = i;
        break;
      }
    }
    assert( type != FOOD_NONE );
    gain = p -> get_gain( "food" );
  }

  virtual void execute()
  {
    player_t* p = player;
    if ( sim -> log ) log_t::output( sim, "%s uses Food %s", p -> name(), util_t::food_type_string( type ) );
    p -> food = type;
    switch ( type )
    {
/*
      case FOOD_BAKED_ROCKFISH:
      p -> stat_gain( STAT_CRIT_RATING, 90 );
      stamina = 90; p -> stat_gain( STAT_STAMINA, stamina );
      break;
*/
    default: assert( 0 );
    }

  }

  virtual bool ready()
  {
    return( player -> food == FOOD_NONE );
  }
};

// ==========================================================================
// Power Potion
// ==========================================================================

struct power_potion_t : public action_t
{
  double amount;

  power_potion_t( player_t* p, const std::string& options_str ) :
    action_t( ACTION_USE, "power_potion", p ),
    amount( 565.0 )
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
  if ( name == "food"                 ) return new                 food_t( p, options_str );
  if ( name == "power_potion"         ) return new         power_potion_t( p, options_str );

  return 0;
}
