// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// set_bonus_t::set_bonus_t =================================================

set_bonus_t::set_bonus_t( player_t* p , const std::string n, const std::string f ) :
 next( 0 ), player( p ), name_str( n ), filter_str( f ), count( 0 )
{

}

// set_bonus_t::tier11 ======================================================

int set_bonus_t::two_pc() const { return ( count >= 2 ) ? 1 : 0; }

int set_bonus_t::four_pc() const { return ( count >= 4 ) ? 1 : 0; }

// set_bonus_t::decode ======================================================

bool set_bonus_t::decode( player_t* /* p */,
                         item_t&   item ) const
{
  if ( ! item.name() )
    return SET_NONE;

  int set = decode_set( item, this );

  return ( set > 0 );
}

// set_bonus_t::init ========================================================

bool set_bonus_t::init( player_t* p )
{
  int num_items = ( int ) p -> items.size();

  for ( int i=0; i < num_items; i++ )
  {
    if ( decode( p, p -> items[ i ] ) )
      count += 1;
  }

  return true;
}

// set_bonus_t::decode_set =====================================================

bool set_bonus_t::decode_set( item_t& item, const set_bonus_t* sb ) const
{
  const char* s = item.name();

  if ( strstr( s, sb -> filter_str.c_str() ) )
      return true;

  return false;
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
