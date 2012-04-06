// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// ==========================================================================
// Heal
// ==========================================================================

// ==========================================================================
// Created by philoptik@gmail.com
//
// heal_target is set to player for now.
// ==========================================================================

// heal_t::heal_t ===========================================================

heal_t::heal_t( const std::string& n, player_t* p, policy_t policy,
                resource_type r, school_type s ) :
    action_t( ACTION_HEAL, n, p, policy, r, s )
{
  if ( ! target || target -> is_enemy() || target -> is_add() )
    target = player;

  group_only = false;

  may_crit          = true;
  tick_may_crit     = true;

  stats -> type = STATS_HEAL;
}

// heal_t::player_buff ======================================================

void heal_t::player_buff()
{
  action_t::player_buff();

  player_multiplier    = player -> composite_player_heal_multiplier( school );
  dd.player_multiplier = player -> composite_player_dh_multiplier( school );
  td.player_multiplier = player -> composite_player_th_multiplier( school );

  if ( sim -> debug ) log_t::output( sim, "heal_t::player_buff: %s mult=%.2f dd_mult=%.2f td_mult=%.2f crit=%.2f",
                                     name(), player_multiplier, dd.player_multiplier, td.player_multiplier, player_crit );
}

// heal_t::execute ==========================================================

void heal_t::execute()
{
  action_t::execute();

  if ( harmful && callbacks )
  {
    if ( result != RESULT_NONE )
      action_callback_t::trigger( player -> heal_callbacks[ result ], this );

    if ( ! background ) // OnSpellCast
      action_callback_t::trigger( player -> heal_callbacks[ RESULT_NONE ], this );
  }
}

// heal_t::assess_damage ====================================================

void heal_t::assess_damage( player_t*   t,
                            double      heal_amount,
                            dmg_type    heal_type,
                            result_type heal_result )
{
  player_t::heal_info_t heal = t -> assess_heal( heal_amount, school, heal_type, heal_result, this );

  if ( heal_type == HEAL_DIRECT )
  {
    if ( sim -> log )
    {
      log_t::output( sim, "%s %s heals %s for %.0f (%.0f) (%s)",
                     player -> name(), name(),
                     t -> name(), heal.amount, heal.actual,
                     util_t::result_type_string( result ) );
    }

    if ( callbacks ) action_callback_t::trigger( player -> direct_heal_callbacks[ school ], this );
  }
  else // HEAL_OVER_TIME
  {
    if ( sim -> log )
    {
      dot_t* dot = this -> dot();
      log_t::output( sim, "%s %s ticks (%d of %d) %s for %.0f (%.0f) heal (%s)",
                     player -> name(), name(),
                     dot -> current_tick, dot -> num_ticks,
                     t -> name(), heal.amount, heal.actual,
                     util_t::result_type_string( result ) );
    }

    if ( callbacks ) action_callback_t::trigger( player -> tick_heal_callbacks[ school ], this );
  }

  stats -> add_result( sim -> report_overheal ? heal.amount : heal.actual, heal.actual, ( direct_tick ? HEAL_OVER_TIME : heal_type ), heal_result );

}

// heal_t::find_greatest_difference_player ==================================

player_t* heal_t::find_greatest_difference_player()
{
  double diff=0;
  double max=0;
  player_t* max_player = player;
  for ( player_t* p = sim -> player_list; p; p = p -> next )
  {
    // No love for pets right now
    diff = p -> is_pet() ? 0 : p -> resource_max[ RESOURCE_HEALTH ] - p -> resource_current[ RESOURCE_HEALTH ];
    if ( diff > max )
    {
      max = diff;
      max_player = p;
    }
  }
  return max_player;
}

// heal_t::find_lowest_player ===============================================

player_t* heal_t::find_lowest_player()
{
  double diff=0;
  double max=0;
  player_t* max_player = player;
  for ( player_t* p = sim -> player_list; p; p = p -> next )
  {
    // No love for pets right now
    diff =  p -> is_pet() ? 0 : 1.0 / p -> resource_current[ RESOURCE_HEALTH ];
    if ( diff > max )
    {
      max = diff;
      max_player = p;
    }
  }
  return max_player;
}

// heal_t::available_targets ==============================================

std::vector<player_t*> heal_t::available_targets() const
{
  std::vector<player_t*> tl = action_t::available_targets();

  if ( group_only )
  {
    int party = target -> party;

    for ( size_t i = 0; i < tl.size(); ++i )
    {
      if ( tl[ i ] -> party != party )
      {
        for ( size_t j = i + 1; j < tl.size(); ++j )
        {
          if ( tl[ j ] -> party == party )
            tl[ i++ ] = tl[ j ];
        }
        tl.resize( i );
        break;
      }
    }
  }

  return tl;
}

// ==========================================================================
// Absorb
// ==========================================================================

// ==========================================================================
// Created by philoptik@gmail.com
//
// heal_target is set to player for now.
// dmg_type = ABSORB, all crits killed
// ==========================================================================

// absorb_t::absorb_t =======================

absorb_t::absorb_t( const std::string& n, player_t* p, policy_t policy,
                    resource_type r, school_type s ) :
  action_t( ACTION_ABSORB, n, p, policy, r, s )
{
  if ( ! target || target -> is_enemy() || target -> is_add() )
    target = player;

  stats -> type = STATS_ABSORB;
}

// absorb_t::player_buff ====================================================

void absorb_t::player_buff()
{
  action_t::player_buff();

  player_multiplier = player -> composite_player_absorb_multiplier( school );

  if ( sim -> debug ) log_t::output( sim, "absorb_t::player_buff: %s crit=%.2f",
                                     name(), player_crit );
}

// absorb_t::execute ========================================================

void absorb_t::execute()
{
  action_t::execute();

  if ( harmful && callbacks )
  {
    if ( ! background ) // OnSpellCast
      action_callback_t::trigger( player -> attack_callbacks[ RESULT_NONE ], this );
  }
}

// absorb_t::impact =========================================================

void absorb_t::impact( player_t* t, result_type impact_result, double travel_dmg=0 )
{
  if ( travel_dmg > 0 )
  {
    assess_damage( t, travel_dmg, ABSORB, impact_result );
  }
}

// absorb_t::assess_damage ==================================================

void absorb_t::assess_damage( player_t*   t,
                              double      heal_amount,
                              dmg_type    heal_type,
                              result_type heal_result )
{
  double heal_actual = direct_dmg = t -> resource_gain( RESOURCE_HEALTH, heal_amount, 0, this );

  if ( heal_type == ABSORB )
  {
    if ( sim -> log )
    {
      log_t::output( sim, "%s %s heals %s for %.0f (%.0f) (%s)",
                     player -> name(), name(),
                     t -> name(), heal_actual, heal_amount,
                     util_t::result_type_string( result ) );
    }

  }

  stats -> add_result( heal_actual, heal_amount, heal_type, heal_result );
}

// absorb_t::calculate_result ===============================================

void absorb_t::calculate_result()
{
  direct_dmg = 0;
  result = RESULT_NONE;

  if ( ! harmful ) return;

  result = RESULT_HIT;

  if ( sim -> debug ) log_t::output( sim, "%s result for %s is %s", player -> name(), name(), util_t::result_type_string( result ) );
}
