// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// ==========================================================================
// Consumables
// ==========================================================================

namespace { // ANONYMOUS ====================================================

struct use_stim_t : public action_t
{
  stim_type type;
  gain_t* gain;

  use_stim_t( player_t* p, const std::string& options_str ) :
    action_t( ACTION_USE, "stim", p ), type( stim_type::none ),
    gain( p -> get_gain( "stim" ) )
  {
    trigger_gcd = timespan_t::zero();
    harmful = false;

    std::string type_str;

    option_t options[] =
    {
      { "type", OPT_STRING, &type_str },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    if ( type_str.empty() )
    {
      sim -> errorf( "Player %s must specify type of stim to use.\n",
                     player -> name() );
      sim -> cancel();
      return;
    }

    type = util_t::parse_stim_type( type_str );
    if ( type == stim_type::none )
    {
      sim -> errorf( "Player %s attempting to use stim of unknown type '%s'.\n",
                     player -> name(), type_str.c_str() );
      sim -> cancel();
    }
  }

  virtual void execute()
  {
    player_t* p = player;
    if ( sim -> log ) log_t::output( sim, "%s uses Stim %s", p -> name(), util_t::stim_type_string( type ) );
    p -> stim = type;
    switch ( type )
    {
    case stim_type::prototype_nano_infused_resolve:
      p -> stat_gain( STAT_WILLPOWER, 169, gain, this );
      p -> stat_gain( STAT_POWER, 70, gain, this );
      break;
      
    case stim_type::exotech_resolve:
      p -> stat_gain( STAT_WILLPOWER, 128, gain, this );
      p -> stat_gain( STAT_POWER, 52, gain, this );
      break;

    case stim_type::rakata_resolve:
      p -> stat_gain( STAT_WILLPOWER, 112, gain, this );
      p -> stat_gain( STAT_POWER, 46, gain, this );
      break;

    case stim_type::prototype_nano_infused_skill:
      p -> stat_gain( STAT_CUNNING, 169, gain, this );
      p -> stat_gain( STAT_POWER, 70, gain, this );
      break;

    case stim_type::exotech_skill:
      p -> stat_gain( STAT_CUNNING, 128, gain, this );
      p -> stat_gain( STAT_POWER, 52, gain, this );
      break;

    case stim_type::rakata_skill:
      p -> stat_gain( STAT_CUNNING, 112, gain, this );
      p -> stat_gain( STAT_POWER, 46, gain, this );
      break;

    case stim_type::prototype_nano_infused_reflex:
      p -> stat_gain( STAT_AIM, 169, gain, this );
      p -> stat_gain( STAT_POWER, 70, gain, this );
      break;

    case stim_type::exotech_reflex:
      p -> stat_gain( STAT_AIM, 128, gain, this );
      p -> stat_gain( STAT_POWER, 52, gain, this );
      break;

    case stim_type::rakata_reflex:
      p -> stat_gain( STAT_AIM, 112, gain, this );
      p -> stat_gain( STAT_POWER, 46, gain, this );
      break;

    case stim_type::prototype_nano_infused_might:
      p -> stat_gain( STAT_STRENGTH, 169, gain, this );
      p -> stat_gain( STAT_POWER, 70, gain, this );
      break;

    case stim_type::exotech_might:
      p -> stat_gain( STAT_STRENGTH, 128, gain, this );
      p -> stat_gain( STAT_POWER, 52, gain, this );
      break;

    case stim_type::rakata_might:
      p -> stat_gain( STAT_STRENGTH, 112, gain, this );
      p -> stat_gain( STAT_POWER, 46, gain, this );
      break;

    default:
      assert( 0 );
      break;
    }
  }

  virtual bool ready()
  { return ( player -> stim == stim_type::none ); }
};

// ==========================================================================
// Power Potion
// ==========================================================================

struct power_potion_t : public action_t
{
  double amount;

  power_potion_t( player_t* p, const std::string& options_str ) :
    action_t( ACTION_USE, "power_potion", p ),
    amount( 675.0 ) // Prototype Nano Infused Attack Adrenal
  {
    option_t options[] =
    {
      { "amount", OPT_FLT, &amount },
      { NULL, OPT_UNKNOWN, NULL }
    };
    parse_options( options, options_str );

    trigger_gcd = timespan_t::zero();
    harmful = false;
    cooldown = p -> get_cooldown( "potion" );
    cooldown -> duration = from_seconds( 180.0 );
  }

  virtual void execute()
  {
    if ( player -> in_combat )
    {
      player -> buffs.power_potion -> trigger( 1, amount );
    }
    else
    {
      cooldown -> duration -= from_seconds( 5.0 );
      player -> buffs.power_potion -> buff_duration -= from_seconds( 5.0 );
      player -> buffs.power_potion -> trigger( 1, amount);
      cooldown -> duration += from_seconds( 5.0 );
      player -> buffs.power_potion -> buff_duration += from_seconds( 5.0 );
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

} // ANONYMOUS namespace ====================================================

// consumable_t::create_action ==============================================

action_t* consumable_t::create_action( player_t*          p,
                                       const std::string& name,
                                       const std::string& options_str )
{
  if ( name == "stim"            ) return new     use_stim_t( p, options_str );
  if ( name == "power_potion" ||
       name == "power_adrenal"   ) return new power_potion_t( p, options_str );

  return 0;
}
