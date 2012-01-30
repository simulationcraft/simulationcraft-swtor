// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// ==========================================================================
// Spell
// ==========================================================================

// spell_t::spell_t =========================================================

void spell_t::init_spell_t_()
{
  may_miss = may_resist = true;
  base_spell_power_multiplier = 1.0;
  crit_bonus = 0.5;
  min_gcd = timespan_t::from_seconds( 1.0 );
}

spell_t::spell_t( const char* n, player_t* p, int r, const school_type s, int t ) :
  action_t( ACTION_SPELL, n, p, r, s, t, true )
{
  init_spell_t_();
}

// spell_t::alacrity ===========================================================

double spell_t::alacrity() const
{
  return player -> composite_spell_alacrity();
}

// spell_t::gcd =============================================================

timespan_t spell_t::gcd() const
{
  timespan_t t = action_t::gcd();
  if ( t == timespan_t::zero ) return timespan_t::zero;

  // According to http://sithwarrior.com/forums/Thread-SWTOR-formula-list alacrity doesn't reduce the gcd
  // Actually it seems to be more complex, cast time spells ( eg. with 1.5s cast time ) get a reduced gcd, but instant cast spells do not
  // http://sithwarrior.com/forums/Thread-Alacrity-and-the-GCD?pid=9152#pid9152
  if ( base_execute_time > timespan_t::zero )
    t *= alacrity();

  if ( t < min_gcd ) t = min_gcd;

  return t;
}

// spell_t::execute_time ====================================================

timespan_t spell_t::execute_time() const
{
  timespan_t t = base_execute_time;

  if ( ! harmful && ! player -> in_combat )
    return timespan_t::zero;

  if ( t <= timespan_t::zero )
    return timespan_t::zero;

  t *= alacrity();

  return t;
}

// spell_t::player_buff =====================================================

void spell_t::player_buff()
{
  action_t::player_buff();

  player_t* p = player;

  player_hit  = p -> composite_spell_hit();
  player_crit = p -> composite_spell_crit();

  if ( sim -> debug ) log_t::output( sim, "spell_t::player_buff: %s hit=%.2f crit=%.2f",
                                     name(), player_hit, player_crit );
}

// spell_t::target_debuff ===================================================

void spell_t::target_debuff( player_t* t, int dmg_type )
{
  action_t::target_debuff( t, dmg_type );

  if ( sim -> debug )
    log_t::output( sim, "spell_t::target_debuff: %s multiplier=%.2f hit=%.2f crit=%.2f",
                   name(), target_multiplier, target_hit, target_crit );
}

// spell_t::miss_chance =====================================================

double spell_t::miss_chance( int delta_level ) const
{
  double miss=0;

  miss -= total_hit();

  if ( miss < 0.00 ) miss = 0.00;
  if ( miss > 0.99 ) miss = 0.99;

  return miss;
}

// spell_t::crit_chance =====================================================

double spell_t::crit_chance( int delta_level ) const
{
  double chance = total_crit();

  ( void ) delta_level;

  return chance;
}

// spell_t::calculate_result ================================================

void spell_t::calculate_result()
{
  int delta_level = target -> level - player -> level;

  direct_dmg = 0;
  result = RESULT_NONE;

  if ( ! harmful || ! may_hit ) return;

  if ( ( result == RESULT_NONE ) && may_miss )
  {
    if ( rng[ RESULT_MISS ] -> roll( miss_chance( delta_level ) ) )
    {
      result = RESULT_MISS;
    }
  }

  if ( ( result == RESULT_NONE ) && may_resist && binary )
  {
    if ( rng[ RESULT_RESIST ] -> roll( resistance() ) )
    {
      result = RESULT_RESIST;
    }
  }

  if ( result == RESULT_NONE )
  {
    result = RESULT_HIT;

    if ( may_crit )
    {
      if ( rng[ RESULT_CRIT ] -> roll( crit_chance( delta_level ) ) )
      {
        result = RESULT_CRIT;
      }
    }
  }

  if ( sim -> debug ) log_t::output( sim, "%s result for %s is %s", player -> name(), name(), util_t::result_type_string( result ) );
}

// spell_t::execute =========================================================

void spell_t::execute()
{
  action_t::execute();

  if ( player -> last_foreground_action == this )
    player -> debuffs.casting -> expire();

  if ( harmful && callbacks )
  {
    if ( result != RESULT_NONE )
    {
      action_callback_t::trigger( player -> spell_callbacks[ result ], this );
    }
    if ( ! background ) // OnSpellCast
    {
      action_callback_t::trigger( player -> spell_callbacks[ RESULT_NONE ], this );
    }
  }
}

// spell_t::schedule_execute ================================================

void spell_t::schedule_execute()
{
  action_t::schedule_execute();

  if ( time_to_execute > timespan_t::zero )
    player -> debuffs.casting -> trigger();
}
