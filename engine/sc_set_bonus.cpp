// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// set_bonus_t::set_bonus_t =================================================

set_bonus_t::set_bonus_t( const std::string& n, const std::string& f ) :
 next( 0 ), name( n ), count( 0 )
{
  if ( f.empty() )
    filters.push_back( name );
  else
    util_t::string_split( filters, f, "/" );
}

// set_bonus_t::decode ======================================================

bool set_bonus_t::decode( const item_t& item ) const
{
  if ( item.name() )
  {
    std::string s = item.name();

    for ( unsigned int i = 0; i < filters.size(); i++ )
    {
      if ( s.find( filters[ i ] ) != s.npos )
        return true;
    }
  }

  return false;
}

// set_bonus_t::init ========================================================

void set_bonus_t::init( const player_t& p )
{
  count = 0;

  int num_items = p.items.size();
  for ( int i=0; i < num_items; i++ )
  {
    if ( decode( p.items[ i ] ) )
      ++count;
  }
}


/*
action_expr_t* set_bonus_t::create_expression( action_t* action,
                                               const std::string& type )
{
  set_type bonus_type = util_t::parse_set_bonus( type );

  if ( bonus_type != SET_NONE )
  {
    struct set_bonus_expr_t : public action_expr_t
    {
      set_type set_bonus_type;
      set_bonus_expr_t( action_t* a, set_type bonus_type ) : action_expr_t( a, util_t::set_bonus_string( bonus_type ), TOK_NUM ), set_bonus_type( bonus_type ) {}
      virtual int evaluate() { result_num = action -> player -> sets -> has_set_bonus( set_bonus_type ); return TOK_NUM; }
    };
    return new set_bonus_expr_t( action, bonus_type );
  }

  return 0;
}
*/
