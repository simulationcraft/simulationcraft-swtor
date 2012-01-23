// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// ==========================================================================
// Spell Data
// ==========================================================================

spell_data_nil_t spell_data_nil_t::singleton;

spell_data_nil_t::spell_data_nil_t() : spell_data_t()
{ _effect1 = _effect2 = _effect3 = spelleffect_data_t::nil(); }

// spell_data_t::set_used ===================================================

void spell_data_t::set_used( bool value )
{
  if ( value )
    _flags |= FLAG_USED;
  else
    _flags &= ~FLAG_USED;
}

// spell_data_t::set_enabled ================================================

void spell_data_t::set_enabled( bool value )
{
  if ( value )
    _flags &= ~FLAG_DISABLED;
  else
    _flags |= FLAG_DISABLED;
}

// spell_data_t::power_type =================================================

resource_type spell_data_t::power_type() const
{
  return RESOURCE_NONE;
}

// spell_data_t::is_class ===================================================

bool spell_data_t::is_class( player_type c ) const
{
  if ( ! _class_mask )
    return true;

  unsigned mask = util_t::class_id_mask( c );
  return ( _class_mask & mask ) == mask;
}

// spell_data_t::is_race ====================================================

bool spell_data_t::is_race( race_type r ) const
{
  unsigned mask = util_t::race_mask( r );
  return ( _race_mask & mask ) == mask;
}

// spell_data_t::scaling_class ==============================================

player_type spell_data_t::scaling_class() const
{
  switch ( _scaling_type )
  {
  case -1: return PLAYER_SPECIAL_SCALE;
  default: break;
  }

  return PLAYER_NONE;
}

// spell_data_t::cost =======================================================

double spell_data_t::cost() const
{
  double divisor;

  switch ( _power_type )
  {
  case POWER_MANA:
    divisor = 100;
    break;
  case POWER_RAGE:
  case POWER_RUNIC_POWER:
    divisor = 10;
    break;
  default:
    divisor = 1;
    break;
  }

  return _cost / divisor;
}

// spell_data_t::cast_time ==================================================

timespan_t spell_data_t::cast_time( uint32_t level ) const
{
  if ( _cast_div < 0 )
  {
    if ( _cast_min < 0 )
      return timespan_t::zero;
    return timespan_t::from_millis( _cast_min );
  }

  if ( level >= static_cast<uint32_t>( _cast_div ) )
    return timespan_t::from_millis( _cast_max );

  return timespan_t::from_millis( _cast_min + ( _cast_max - _cast_min ) * ( level - 1 ) / ( double )( _cast_div - 1 ) );
}

// spell_data_t::flags ======================================================

bool spell_data_t::flags( spell_attribute_t f ) const
{
  unsigned bit = static_cast<unsigned>( f ) & 0x1Fu;
  unsigned index = ( static_cast<unsigned>( f ) >> 8 ) & 0xFFu;
  uint32_t mask = 1u << bit;

  assert( index < NUM_SPELL_FLAGS );

  return ( _attributes[ index ] & mask ) != 0;
}

// ==========================================================================
// Spell Effect Data
// ==========================================================================

spelleffect_data_nil_t spelleffect_data_nil_t::singleton;

spelleffect_data_nil_t::spelleffect_data_nil_t() :
  spelleffect_data_t()
{ _spell = _trigger_spell = spell_data_t::nil(); }

void spelleffect_data_t::set_used( bool value )
{
  if ( value )
    _flags |= FLAG_USED;
  else
    _flags &= ~FLAG_USED;
}

void spelleffect_data_t::set_enabled( bool value )
{
  if ( value )
    _flags |= FLAG_ENABLED;
  else
    _flags &= ~FLAG_ENABLED;
}

// ==========================================================================
// Talent Data
// ==========================================================================

talent_data_nil_t talent_data_nil_t::singleton;

talent_data_nil_t::talent_data_nil_t() :
  talent_data_t()
{ spell1 = spell2 = spell3 = spell_data_t::nil(); }

void talent_data_t::set_used( bool value )
{
  if ( value )
    _flags |= FLAG_USED;
  else
    _flags &= ~FLAG_USED;
}

void talent_data_t::set_enabled( bool value )
{
  if ( value )
    _flags &= ~FLAG_DISABLED;
  else
    _flags |= FLAG_DISABLED;
}

bool talent_data_t::is_class( player_type c ) const
{
  unsigned mask = util_t::class_id_mask( c );

  if ( mask == 0 )
    return false;

  return ( ( _m_class & mask ) == mask );
}

bool talent_data_t::is_pet( pet_type_t p ) const
{
  unsigned mask = util_t::pet_mask( p );

  if ( mask == 0 )
    return false;

  return ( ( _m_pet & mask ) == mask );
}

unsigned talent_data_t::rank_spell_id( unsigned rank ) const
{
  assert( rank <= MAX_RANK );

  if ( rank == 0 )
    return 0;

  return _rank_id[ rank - 1 ];
}

unsigned talent_data_t::max_rank() const
{
  unsigned i;

  for ( i = 0; i < MAX_RANK; i++ )
  {
    if ( _rank_id[ i ] == 0 )
      break;
  }

  return i;
}
