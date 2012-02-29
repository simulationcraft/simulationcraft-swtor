// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// ==========================================================================
// Talent
// ==========================================================================

// talent_t::talent_t =======================================================

talent_t::talent_t( player_t* /* player */, const char* n, unsigned tab_page, unsigned max_rank ) :
  _name( n ), _tab_page( tab_page ), _max_rank( max_rank ),
  t_rank( 0 )
{
}

talent_t::~talent_t()
{

}

bool talent_t::ok() const
{
  return ( t_rank > 0 );
}
bool talent_t::set_rank( uint32_t r )
{
  if ( r > max_rank() )
    r = max_rank();

  t_rank       = r;

  return true;
}

uint32_t talent_t::max_rank() const
{
  return _max_rank;
}

uint32_t talent_t::rank() const
{
  return t_rank;
}

// ==========================================================================
// Spell ID
// ==========================================================================

// spell_id_t::spell_id_t ===================================================

spell_id_t::spell_id_t( player_t* player, const char* t_name ) :
  s_type( T_SPELL ), s_id( 0 ), s_data( 0 ), s_enabled( false ), s_player( player ),
  s_overridden( false ), s_token( t_name ? t_name : "" ),
  s_required_talent( 0 ), s_single( 0 ), s_tree( -1 ), s_list( 0 )
{
  armory_t::format( s_token, FORMAT_ASCII_MASK );
  range::fill( s_effects, 0 );
}

spell_id_t::spell_id_t( player_t* player, const char* t_name, const uint32_t id, talent_t* talent ) :
  s_type( T_SPELL ), s_id( id ), s_data( 0 ), s_enabled( false ), s_player( player ),
  s_overridden( false ), s_token( t_name ), s_required_talent( talent ), s_single( 0 ), s_tree( -1 ), s_list( 0 )
{
  initialize();
  armory_t::format( s_token, FORMAT_ASCII_MASK );
}

spell_id_t::spell_id_t( player_t* player, const char* t_name, const char* s_name, talent_t* talent ) :
  s_type( T_SPELL ), s_id( 0 ), s_data( 0 ), s_enabled( false ), s_player( player ),
  s_overridden( false ), s_token( t_name ), s_required_talent( talent ), s_single( 0 ), s_tree( -1 ), s_list( 0 )
{
  initialize( s_name );
  armory_t::format( s_token, FORMAT_ASCII_MASK );
}

spell_id_t::~spell_id_t()
{
  if ( s_list )
    s_list -> erase( s_list_iter );
}

void spell_id_t::queue()
{
  s_list = &s_player -> spell_list;
  s_list_iter = s_list->insert( s_list->end(), this );
}

bool spell_id_t::initialize( const char* s_name )
{
  range::fill( s_effects, 0 );

  assert( s_player && s_player -> sim );

  // For pets, find stuff based on owner class, as that's how our spell lists
  // are structured
  player_type player_class;
  if ( s_player -> is_pet() )
    player_class = s_player -> cast_pet() -> owner -> type;
  else
    player_class = s_player -> type;

  // Search using spell name to find the spell type
  if ( ! s_id )
  {
    if ( ! s_name || ! *s_name )
      return false;

    if ( ( s_id = s_player -> dbc.mastery_ability_id( player_class, s_name ) ) )
      s_type      = T_MASTERY;
    else if ( ! s_id && ( s_id = s_player -> dbc.specialization_ability_id( player_class, s_name ) ) )
      s_type      = T_SPEC;
    else if ( ! s_id && ( s_id = s_player -> dbc.class_ability_id( player_class, s_name ) ) )
      s_type      = T_CLASS;
    else if ( ! s_id && ( s_id = s_player -> dbc.race_ability_id( player_class, s_player -> race, s_name ) ) )
      s_type      = T_RACE;
    else if ( ! s_id && ( s_id = s_player -> dbc.glyph_spell_id( player_class, s_name ) ) )
      s_type      = T_GLYPH;
    else if ( ! s_id && ( s_id = s_player -> dbc.set_bonus_spell_id( player_class, s_name ) ) )
      s_type      = T_ITEM;
  }
  // Search using spell id to find the spell type
  else
  {
    if ( s_player -> dbc.is_mastery_ability( s_id ) )
      s_type      = T_MASTERY;
    else if ( s_player -> dbc.is_specialization_ability( s_id ) )
      s_type      = T_SPEC;
    else if ( s_player -> dbc.is_class_ability( s_id ) )
      s_type      = T_CLASS;
    else if ( s_player -> dbc.is_race_ability( s_id ) )
      s_type      = T_RACE;
    else if ( s_player -> dbc.is_glyph_spell( s_id ) )
      s_type      = T_GLYPH;
    else if ( s_player -> dbc.is_set_bonus_spell( s_id ) )
      s_type      = T_ITEM;
  }

  // At this point, our spell must exist or we are in trouble
  if ( ! s_id || ! s_player -> dbc.spell( s_id ) )
  {
    return false;
  }

  // Do second phase of spell initialization
  s_data = s_player -> dbc.spell( s_id );

  const_cast<spell_data_t*>( s_data ) -> set_used( true );

  // Some spells, namely specialization and class spells
  // can specify a tree for the spell
  switch ( s_type )
  {
  case T_SPEC:
    s_tree = s_player -> dbc.specialization_ability_tree( player_class, s_id );
    break;
  case T_CLASS:
    s_tree = s_player -> dbc.class_ability_tree( player_class, s_id );
    break;
  default:
    s_tree = -1;
    break;
  }

  s_enabled = s_data -> is_enabled() && s_data -> is_level( s_player -> level );

  // Warn if the player is enabling a spell that the player has no level for
  /*
  if ( ! s_player -> dbc.spell_is_level( s_id, s_player -> level ) )
  {
    s_player -> sim -> errorf( "Warning: Player %s level (%d) too low for spell %s, requires level %d",
      s_player -> name_str.c_str(),
      s_player -> level,
      s_data -> name,
      s_data -> spell_level );
  }
  */
  if ( s_type == T_MASTERY )
  {
    if ( s_player -> level < 75 )
      s_enabled = false;
  }

  // Map s_effects, figure out if this is a s_single-effect spell
  uint32_t n_effects = 0;
  for ( int i = 0; i < MAX_EFFECTS; i++ )
  {
    if ( ! s_data -> _effect[ i ] )
      continue;

    if ( ! s_player -> dbc.effect( s_data -> _effect[ i ] ) )
      continue;

    s_effects[ i ] = s_player -> dbc.effect( s_data -> _effect[ i ] );
    n_effects++;
  }

  if ( n_effects == 1 )
  {
    for ( int i = 0; i < MAX_EFFECTS; i++ )
    {
      if ( ! s_effects[ i ] )
        continue;

      s_single = s_effects[ i ];
      break;
    }
  }
  else
    s_single = 0;

  return true;
}

bool spell_id_t::enable( bool override_value )
{
  assert( s_player && s_player -> sim );

  s_overridden = true;
  s_enabled    = override_value;

  return true;
}

bool spell_id_t::ok() const
{
  bool res = s_enabled;

  if ( ! s_player || ! s_data || ! s_id )
    return false;

  if ( s_required_talent )
    res = res & s_required_talent -> ok();

  if ( s_type == T_SPEC )
    res = res & ( s_tree == s_player -> specialization );

  return res;
}

std::string spell_id_t::to_str() const
{
  std::ostringstream s;

  s << "enabled=" << ( s_enabled ? "true" : "false" );
  s << " (ok=" << ( ok() ? "true" : "false" ) << ")";
  if ( s_overridden ) s << " (forced)";
  s << " token=" << s_token;
  s << " type=" << s_type;
  s << " tree=" << s_tree;
  s << " id=" << s_id;
  s << " player=" << s_player -> name_str;

  return s.str();
}

const char* spell_id_t::real_name() const
{
  if ( ! s_player || ! s_data || ! s_id )
    return 0;

  return s_data -> name_cstr();
}

const std::string spell_id_t::token() const
{
  if ( ! s_player || ! s_data || ! s_id )
    return 0;

  return s_token;
}

double spell_id_t::missile_speed() const
{
  if ( ! ok() )
    return 0.0;

  return s_data -> missile_speed();
}

uint32_t spell_id_t::school_mask() const
{
  if ( ! ok() )
    return 0;

  return s_data -> school_mask();
}

uint32_t spell_id_t::get_school_mask( const school_type s )
{
  switch ( s )
  {
  default:
    return SCHOOL_NONE;
  }
  return 0x00;
}

bool spell_id_t::is_school( const school_type s, const school_type s2 )
{
  return ( get_school_mask( s ) & get_school_mask( s2 ) ) != 0;
}

school_type spell_id_t::get_school_type( const uint32_t mask )
{
  switch ( mask )
  {
  default:
    return SCHOOL_NONE;
  }
}

school_type spell_id_t::get_school_type() const
{
  if ( ! ok() )
    return SCHOOL_NONE;

  return get_school_type( school_mask() );
}

resource_type spell_id_t::power_type() const
{
  if ( ! ok() )
    return RESOURCE_NONE;

  return s_data -> power_type();
}

double spell_id_t::min_range() const
{
  if ( ! ok() )
    return 0.0;

  return s_data -> min_range();
}

double spell_id_t::max_range() const
{
  if ( ! ok() )
    return 0.0;

  return s_data -> max_range();
}

double spell_id_t::extra_coeff() const
{
  if ( ! ok() )
    return 0.0;

  return s_data -> extra_coeff();
}

bool spell_id_t::in_range() const
{
  if ( ! ok() )
    return false;

  return s_data -> in_range( s_player -> distance );
}

timespan_t spell_id_t::cooldown() const
{
  if ( ! ok() )
    return timespan_t::zero;

  double d = s_data -> cooldown().total_seconds();

  if ( d > ( s_player -> sim -> wheel_seconds - 2.0 ) )
    d = s_player -> sim -> wheel_seconds - 2.0;

  return timespan_t::from_seconds( d );
}

timespan_t spell_id_t::gcd() const
{
  if ( ! ok() )
    return timespan_t::zero;

  return s_data -> gcd();
}

uint32_t spell_id_t::category() const
{
  if ( ! ok() )
    return 0;

  return s_data -> category();
}

timespan_t spell_id_t::duration() const
{
  if ( ! ok() )
    return timespan_t::zero;

  timespan_t d = s_data -> duration();
  timespan_t player_wheel_seconds = timespan_t::from_seconds( s_player -> sim -> wheel_seconds - 2.0 );
  if ( d > player_wheel_seconds )
    d = player_wheel_seconds;

  return d;
}

double spell_id_t::cost() const
{
  if ( ! ok() )
    return 0.0;

  return s_data -> cost();
}

uint32_t spell_id_t::rune_cost() const
{
  if ( ! ok() )
    return 0;

  return s_data -> rune_cost();
}

double spell_id_t::runic_power_gain() const
{
  if ( ! ok() )
    return 0.0;

  return s_data -> runic_power_gain();
}

uint32_t spell_id_t::max_stacks() const
{
  if ( ! ok() )
    return 0;

  return s_data -> max_stacks();
}

uint32_t spell_id_t::initial_stacks() const
{
  if ( ! ok() )
    return 0;

  return s_data -> initial_stacks();
}

double spell_id_t::proc_chance() const
{
  if ( ! ok() )
    return 0.0;

  return s_data -> proc_chance();
}

timespan_t spell_id_t::cast_time() const
{
  if ( ! ok() )
    return timespan_t::zero;

  return s_data -> cast_time( s_player -> level );
}

uint32_t spell_id_t::effect_id( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0;

  return s_data -> effect_id( effect_num );
}

bool spell_id_t::flags( spell_attribute_t f ) const
{
  if ( ! ok() )
    return false;

  return s_data -> flags( f );
}

const char* spell_id_t::desc() const
{
  if ( ! ok() )
    return 0;

  return s_data -> desc();
}

const char* spell_id_t::tooltip() const
{
  if ( ! ok() )
    return 0;

  return s_data -> tooltip();
}

int32_t spell_id_t::effect_type( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> type();
}

int32_t spell_id_t::effect_subtype( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> subtype();
}

int32_t spell_id_t::effect_base_value( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> base_value();
}

int32_t spell_id_t::effect_misc_value1( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> misc_value1();
}

int32_t spell_id_t::effect_misc_value2( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> misc_value2();
}

uint32_t spell_id_t::effect_trigger_spell( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> trigger_spell_id();
}

double spell_id_t::effect_chain_multiplier( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> chain_multiplier();
}

double spell_id_t::effect_average( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0.0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect_average( effect_id, s_player -> level );
}

double spell_id_t::effect_delta( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0.0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect_delta( effect_id, s_player -> level );
}

double spell_id_t::effect_bonus( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0.0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect_bonus( effect_id, s_player -> level );
}

double spell_id_t::effect_min( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0.0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect_min( effect_id, s_player -> level );
}

double spell_id_t::effect_max( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0.0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect_max( effect_id, s_player -> level );
}

double spell_id_t::effect_coeff( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0.0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> coeff();
}

timespan_t spell_id_t::effect_period( uint32_t effect_num ) const
{
  if ( ! ok() )
    return timespan_t::zero;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> period();
}

double spell_id_t::effect_radius( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0.0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> radius();
}

double spell_id_t::effect_radius_max( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0.0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> radius_max();
}

double spell_id_t::effect_pp_combo_points( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0.0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> pp_combo_points();
}

double spell_id_t::effect_real_ppl( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0.0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> real_ppl();
}

int spell_id_t::effect_die_sides( uint32_t effect_num ) const
{
  if ( ! ok() )
    return 0;

  uint32_t effect_id = s_data -> effect_id( effect_num );

  return s_player -> dbc.effect( effect_id ) -> die_sides();
}

double spell_id_t::base_value( effect_type_t type, effect_subtype_t sub_type, int misc_value, int misc_value2 ) const
{
  if ( ! ok() )
    return 0.0;

  if ( s_single &&
       ( type == E_MAX || s_single -> type() == type ) &&
       ( sub_type == A_MAX || s_single -> subtype() == sub_type ) &&
       ( misc_value == DEFAULT_MISC_VALUE || s_single -> misc_value1() == misc_value ) &&
       ( misc_value2 == DEFAULT_MISC_VALUE || s_single -> misc_value2() == misc_value2 ) )
  {
    if ( ( type == E_MAX || s_single -> type() == type ) &&
         ( sub_type == A_MAX || s_single -> subtype() == sub_type ) &&
         ( misc_value == DEFAULT_MISC_VALUE || s_single -> misc_value1() == misc_value ) &&
         ( misc_value2 == DEFAULT_MISC_VALUE || s_single -> misc_value2() == misc_value2 ) )
      return dbc_t::fmt_value( s_single -> base_value(), s_single -> type(), s_single -> subtype() );
    else
      return 0.0;
  }

  for ( int i = 0; i < MAX_EFFECTS; i++ )
  {
    if ( ! s_effects[ i ] )
      continue;

    if ( ( type == E_MAX || s_effects[ i ] -> type() == type ) &&
         ( sub_type == A_MAX || s_effects[ i ] -> subtype() == sub_type ) &&
         ( misc_value == DEFAULT_MISC_VALUE || s_effects[ i ] -> misc_value1() == misc_value ) &&
         ( misc_value2 == DEFAULT_MISC_VALUE || s_effects[ i ] -> misc_value2() == misc_value2 ) )
      return dbc_t::fmt_value( s_effects[ i ] -> base_value(), type, sub_type );
  }

  return 0.0;
}

const spelleffect_data_t* spell_id_t::get_effect( property_type_t p_type ) const
{
  if ( ! ok() )
    return NULL;

  if ( s_single )
  {
    if ( ( p_type == P_MAX ) || ( s_single -> subtype() == A_ADD_FLAT_MODIFIER ) || ( s_single -> subtype() == A_ADD_PCT_MODIFIER ) )
      return s_single;
    else
      return NULL;
  }

  for ( int i = 0; i < MAX_EFFECTS; i++ )
  {
    if ( ! s_effects[ i ] )
      continue;

    if ( s_effects[ i ] -> subtype() != A_ADD_FLAT_MODIFIER && s_effects[ i ] -> subtype() != A_ADD_PCT_MODIFIER )
      continue;

    if ( p_type == P_MAX || s_effects[ i ] -> misc_value1() == p_type )
      return s_effects[ i ];
  }

  return NULL;
}

double spell_id_t::mod_additive( property_type_t p_type ) const
{
  // Move this somewhere sane, here for now
  static const double property_flat_divisor[] =
  {
    1.0,    // P_GENERIC
    1000.0, // P_DURATION
    1.0,    // P_THREAT
    1.0,    // P_EFFECT_1
    1.0,    // P_STACK
    1.0,    // P_RANGE
    1.0,    // P_RADIUS
    100.0,  // P_CRIT
    1.0,    // P_UNKNOWN_1
    1.0,    // P_PUSHBACK
    1000.0, // P_CAST_TIME
    1000.0, // P_COOLDOWN
    1.0,    // P_EFFECT_2
    1.0,    // Unused
    1.0,    // P_RESOURCE_COST
    1.0,    // P_CRIT_DAMAGE
    1.0,    // P_PENETRATION
    1.0,    // P_TARGET
    100.0,  // P_PROC_CHANCE
    1000.0, // P_TICK_TIME
    1.0,    // P_TARGET_BONUS
    1000.0, // P_GCD
    1.0,    // P_TICK_DAMAGE
    1.0,    // P_EFFECT_3
    100.0,  // P_SPELL_POWER
    1.0,    // Unused
    1.0,    // P_PROC_FREQUENCY
    1.0,    // P_DAMAGE_TAKEN
    100.0,  // P_DISPEL_CHANCE
  };

  const spelleffect_data_t* effect = get_effect( p_type );

  if( effect == NULL )
    return 0.0;

  // Divide by 100 for every A_ADD_PCT_MODIFIER
  if( effect -> subtype() == ( int ) A_ADD_PCT_MODIFIER )
    return effect -> base_value() / 100.0;
  // Divide by property_flat_divisor for every A_ADD_FLAT_MODIFIER
  else
    return effect -> base_value() / property_flat_divisor[ effect -> misc_value1() ];
}

timespan_t spell_id_t::mod_additive_time( property_type_t p_type ) const
{
  const spelleffect_data_t* effect = get_effect( p_type );

  if( effect == NULL )
    return timespan_t::zero;

  assert(effect -> subtype() == A_ADD_FLAT_MODIFIER);

  return timespan_t::from_millis(effect -> base_value());
}

// Glyph basic object

glyph_t::glyph_t( player_t* player, spell_data_t* _sd ) :
  spell_id_t( player, _sd -> name_cstr() ),
  // Future trimmed down access
  sd( _sd ), sd_enabled( spell_data_t::nil() )
{
  initialize( sd -> name_cstr() );
  if ( s_token.substr( 0, 9 ) == "glyph_of_" ) s_token.erase( 0, 9 );
  if ( s_token.substr( 0, 7 ) == "glyph__"   ) s_token.erase( 0, 7 );
  s_enabled = false;
}

bool glyph_t::enable( bool override_value )
{
  sd_enabled = override_value ? sd : spell_data_t::nil();

  spell_id_t::enable( override_value );

  if ( s_player -> sim -> debug )
    log_t::output( s_player -> sim, "Glyph Spell status: %s", to_str().c_str() );

  return s_enabled;
}

mastery_t::mastery_t( player_t* player, const char* t_name, const uint32_t id, int tree ) :
  spell_id_t( player, t_name, id ), m_tree( tree )
{
  queue();

  if ( s_player -> sim -> debug )
    log_t::output( s_player -> sim, "Mastery status: %s", to_str().c_str() );
}

mastery_t::mastery_t( player_t* player, const char* t_name, const char* s_name, int tree ) :
  spell_id_t( player, t_name, s_name ), m_tree( tree )
{
  queue();

  if ( s_player -> sim -> debug )
    log_t::output( s_player -> sim, "Mastery status: %s", to_str().c_str() );
}

bool mastery_t::ok() const
{
  return spell_id_t::ok() && ( s_player -> primary_tree() == m_tree );
}

double mastery_t::base_value( effect_type_t type, effect_subtype_t sub_type, int misc_value, int misc_value2 ) const
{
  return spell_id_t::base_value( type, sub_type, misc_value, misc_value2 ) / 10000.0;
}

std::string mastery_t::to_str() const
{
  std::ostringstream s;

  s << spell_id_t::to_str() << " mastery_tree=" << m_tree;

  return s.str();
}

