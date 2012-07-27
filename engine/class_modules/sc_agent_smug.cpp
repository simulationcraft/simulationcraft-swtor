// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "agent_smug.hpp"

namespace agent_smug { // ===================================================

targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  ::targetdata_t( source, target ),
  debuff_weakening_blast  ( new debuff_t ( this, source.m.t_weakening_blast, 10, from_seconds (  15 ) ) ),
  dot_adrenaline_probe       ( source.m.a_adrenaline_probe            , &source ),
  dot_corrosive_dart         ( source.m.a_corrosive_dart              , &source ),
  dot_corrosive_dart_weak    ( source.m.a_corrosive_dart + "_weak"    , &source ),
  dot_corrosive_grenade      ( source.m.a_corrosive_grenade           , &source ),
  dot_corrosive_grenade_weak ( source.m.a_corrosive_grenade + "_weak" , &source ),
  dot_orbital_strike         ( source.m.a_orbital_strike              , &source )
{
  add( *debuff_weakening_blast    );
  add( dot_adrenaline_probe       );
  add( dot_corrosive_dart         );
  add( dot_corrosive_dart_weak    );
  add( dot_corrosive_grenade      );
  add( dot_corrosive_grenade_weak );
  add( dot_orbital_strike         );
}

// Attack ======================================================

attack_t::attack_t( const std::string& n, class_t* p, attack_policy_t policy, school_type s ) :
  base_t( n, p, policy, RESOURCE_ENERGY, s )
{
  harmful       = true;
  may_crit      = true;
  tick_may_crit = true;
}

// Tech Attack =================================================

tech_attack_t::tech_attack_t( const std::string& n, class_t* p, school_type s ) :
  base_t( n, p, tech_policy, s )
{}

// Range Attack ================================================

range_attack_t::range_attack_t( const std::string& n, class_t* p, school_type s ) :
  base_t( n, p, range_policy, s )
{}

// Poison Attack ===============================================

poison_attack_t::poison_attack_t( const std::string& n, class_t* p, school_type s ) :
  base_t( n, p, s )
{
  base_crit     += .04 * p -> talents.lethal_dose -> rank();
}

void poison_attack_t::target_debuff( player_t* tgt, dmg_type type )
{
  base_t::target_debuff( tgt, type );

  class_t& p = *cast();

  if ( unsigned rank = p.talents.devouring_microbes -> rank() )
  {
    bool up = tgt -> health_percentage() < 30;
    p.benefits.devouring_microbes_ticks -> update( up );
    if ( up )
      target_multiplier += 0.05 * rank;
  }

  if ( type == DMG_OVER_TIME && p.talents.weakening_blast -> rank() )
  {
    targetdata_t& td = *targetdata();
    bool up = td.debuff_weakening_blast -> up();
    p.benefits.wb_poison_ticks -> update( up );
    if ( up )
    {
      td.debuff_weakening_blast -> decrement();
      target_multiplier += 0.3;
    }
  }
}

// ==========================================================================
// Smuggler / Agent Abilities
// ==========================================================================

// Adrenaline Probe | Cool Head =============================================

adrenaline_probe_t::adrenaline_probe_t( class_t* p, const std::string& n, const std::string& options_str ) :
  base_t(n, p, default_policy, RESOURCE_ENERGY, SCHOOL_NONE)
{
  parse_options( options_str );

  cooldown -> duration = from_seconds( 120 - 15 * p -> talents.lethal_purpose -> rank() );
  use_off_gcd = true;
  trigger_gcd = timespan_t::zero();

  num_ticks = 2;
  base_tick_time = from_seconds( 1.5 );
}

int adrenaline_probe_t::energy_returned_initial() { return 34; }

int adrenaline_probe_t::energy_returned_tick() { return 8; }

// the combat log isn't in sync with the game here.
// the combat log shows after 1.5 seconds a tick of 8 and 34, and then another tick of 8 1.5s later.
// what happens in game is you instantly get 34, and then two ticks of 8.
void adrenaline_probe_t::execute()
{
  base_t::execute();
  class_t* p = cast();

  p -> buffs.adrenaline_probe -> trigger();
  p -> resource_gain( RESOURCE_ENERGY, energy_returned_initial(), p -> gains.adrenaline_probe );
}

void adrenaline_probe_t::tick(dot_t* d)
{
  base_t::tick(d);
  class_t* p = cast();

  p -> resource_gain( RESOURCE_ENERGY, energy_returned_tick(), p -> gains.adrenaline_probe );
}

// Coordination | Lucky Shots ===============================================

coordination_t::coordination_t( class_t* p, const std::string& n, const std::string& options_str ) :
  base_t( n, p, tech_policy, RESOURCE_NONE, SCHOOL_NONE )
{
  parse_options( options_str );
  base_cost = 0.0;
}

void coordination_t::execute()
{
  base_t::execute();

  for ( player_t* p : list_range( sim -> player_list ) )
  {
    if ( p -> ooc_buffs() )
      p -> buffs.coordination -> trigger();
  }
}

bool coordination_t::ready()
{
  if ( player -> buffs.coordination -> check() )
    return false;

  return base_t::ready();
}

// Corrosive Dart | Vital Shot ==============================================
corrosive_dart_t::corrosive_dart_t( class_t* p, const std::string& n, const std::string& options_str, bool weak ) :
  base_t( n, p ),
  corrosive_dart_weak()
{
  rank_level_list = { 5, 7, 10, 13, 17, 23, 35, 45, 50 };

  parse_options( options_str );

  //may_crit = false; // only ticks may crit
  range = 30.0;
  base_tick_time = from_seconds( 3.0 );

  if ( weak )
  {
    // infinite?
    range                       = 30.0;
    base_cost                   = 0;
    td.standardhealthpercentmin =
    td.standardhealthpercentmax = 0.01;
    td.power_mod                = 0.1;
    num_ticks                   = 3 + p -> talents.lethal_injectors -> rank();
    background                  = true;
    trigger_gcd                 = timespan_t::zero();
  }
  else
  {
    base_cost                   = energy_cost();
    td.standardhealthpercentmin =
    td.standardhealthpercentmax = 0.04;
    td.power_mod                = 0.4;
    num_ticks                   = 5 + p -> talents.lethal_injectors -> rank();

    if ( p -> talents.lingering_toxins -> rank() )
      corrosive_dart_weak = new corrosive_dart_t( p, n + "_weak", options_str, true );
  }
}

int corrosive_dart_t::energy_cost() { return 20; }

void corrosive_dart_t::tick( dot_t* d )
{
  base_t::tick( d );

  class_t& p = *cast();

  if ( p.talents.corrosive_microbes -> rank()
       && p.rngs.corrosive_microbes -> roll( 0.125 * p.talents.corrosive_microbes -> rank() ) )
  {
    p.procs.corrosive_microbes -> occur();
    extra_tick();
  }
}

void corrosive_dart_t::last_tick( dot_t* d )
{
  base_t::last_tick( d );

  if ( corrosive_dart_weak )
    corrosive_dart_weak -> execute();
}

void corrosive_dart_t::execute()
{
  if ( corrosive_dart_weak )
    targetdata() -> dot_corrosive_dart_weak.cancel();

  base_t::execute();
}

//  Corrosive Grenade | Shrap Bomb ===========================================
corrosive_grenade_t::corrosive_grenade_t( class_t* p, const std::string& n, const std::string& options_str, bool weak ) :
  base_t( n, p),
  corrosive_grenade_weak()
{
  check_talent( p -> talents.corrosive_grenade -> rank() );

  parse_options( options_str );

  range = 30.0;
  base_tick_time = from_seconds( 3 );
  //may_crit = false; // only ticks may crit

  if ( weak )
  {
    // infinite?
    range                       = 30.0;
    base_accuracy               = 999;
    base_cost                   = 0;
    td.standardhealthpercentmin =
    td.standardhealthpercentmax = 0.0065 * p -> talents.lingering_toxins -> rank();
    td.power_mod                = 0.065  * p -> talents.lingering_toxins -> rank();
    num_ticks                   = 3;
    background                  = true;
    trigger_gcd                 = timespan_t::zero();
  }
  else
  {
    base_cost                   = energy_cost();
    cooldown -> duration        = from_seconds( 12.0 );
    td.standardhealthpercentmin =
    td.standardhealthpercentmax = 0.032;
    td.power_mod                = 0.32;
    num_ticks                   = 7;
    tick_zero                   = true;
    aoe                         = 3;

    if ( p -> talents.lingering_toxins -> rank() )
      corrosive_grenade_weak = new corrosive_grenade_t( p, n + "_weak", options_str, true );
  }
}

int corrosive_grenade_t::energy_cost() { return 20; }

void corrosive_grenade_t::last_tick( dot_t* d )
{
  base_t::last_tick( d );

  if ( corrosive_grenade_weak )
    corrosive_grenade_weak -> execute();
}

void corrosive_grenade_t::execute()
{
  if ( corrosive_grenade_weak )
    targetdata() -> dot_corrosive_grenade_weak.cancel();

  base_t::execute();
}

// Explosive Probe | Sabotage Charge ========================================
explosive_probe_t::explosive_probe_t( class_t* p, const std::string& n, const std::string& options_str) :
  base_t( n, p )
{
  // rank_level_list = { 50 };

  parse_options( options_str );

  base_cost                   = energy_cost();
  range                       = 30.0;
  cooldown -> duration        = from_seconds( 30 );
  dd.standardhealthpercentmin = 0.23;
  dd.standardhealthpercentmax = 0.25;
  dd.power_mod                = 2.4;
}
// TODO: explosive probe "attaches" to the target and detonates on damage
// (tooltip says damage. game data has text that says blaster damage)
// this is not implemented yet.

int explosive_probe_t::energy_cost() { return 20; }

bool explosive_probe_t::ready()
{
  // TODO only ready if in cover
  // probably not worth the complication to implement?
  return base_t::ready();
}

// Cull | Wounding Shot =====================================================

cull_t::cull_t( class_t* p, const std::string& n, const std::string& options_str ) :
  base_t( n, p )
{
  check_talent( p -> talents.cull -> rank() );

  parse_options( options_str );

  base_cost                   = energy_cost();
  range                       =  10.0;
  base_multiplier             += .03 * p -> talents.cut_down->rank();
}

int cull_t::energy_cost() { return 25; }

void cull_t::init()
{
  base_t::init();

  extra_strike                = get_extra_strike();
  add_child(extra_strike);
}

cull_extra_t::cull_extra_t( class_t* p, const std::string& n ) :
  base_t( n, p )
{
  background                  = true;
  trigger_gcd                 =  timespan_t::zero();
  base_multiplier             += .03 * p -> talents.cut_down->rank();
}

void cull_t::execute()
{
  base_t::execute();

  if ( ! td.weapon && result_is_hit() )
  {
    targetdata_t* td = targetdata();
    if ( td -> dot_corrosive_dart.ticking || td -> dot_corrosive_dart_weak.ticking )
      extra_strike -> execute();
    if ( td -> dot_corrosive_grenade.ticking || td -> dot_corrosive_grenade_weak.ticking )
      extra_strike -> execute();
  }
}

void cull_t::tick( dot_t* dot )
{
  base_t::tick( dot );

  if ( result_is_hit() )
  {
    targetdata_t* td = targetdata();
    if ( td -> dot_corrosive_dart.ticking || td -> dot_corrosive_dart_weak.ticking )
      extra_strike -> execute();
    if ( td -> dot_corrosive_grenade.ticking || td -> dot_corrosive_grenade_weak.ticking )
      extra_strike -> execute();
  }
}

// Fragmentation Grenade | Thermal Grenade ==================================

fragmentation_grenade_t::fragmentation_grenade_t( class_t* p, const std::string& n, const std::string& options_str ) :
  base_t( n, p )
{
  parse_options( options_str );

  base_cost                   = 20;
  cooldown -> duration        = from_seconds( 6.0 );
  range                       = 30.0;
  dd.standardhealthpercentmin = 0.109;
  dd.standardhealthpercentmax = 0.149;
  dd.power_mod                = 1.29;
  aoe                         = 4;
}

// Orbital Strike | XS Freighter Flyby ======================================
orbital_strike_t::orbital_strike_t( class_t* p, const std::string& n, const std::string& options_str) :
  base_t( n, p, SCHOOL_ELEMENTAL )
{
  parse_options( options_str );

  base_cost                   = energy_cost();
  range                       = 30.0;
  cooldown -> duration        = from_seconds( 60 );
  td.standardhealthpercentmin =
  td.standardhealthpercentmax = 0.177;
  td.power_mod                = 1.77;
  num_ticks                   = 3;
  if ( player -> set_bonus.battlemaster_field_techs -> two_pc() )
    num_ticks                 += 1;
  base_tick_time              = from_seconds( 3 );
  base_execute_time           = from_seconds( 3 );

  aoe = 99; // TODO FIX: unlimited. "all targets in area"
}

int orbital_strike_t::energy_cost() { return 30; }

// Overload Shot | Quick Shot ===============================================

overload_shot_t::overload_shot_t( class_t* p, const std::string& n, const std::string& options_str) :
  base_t( n, p )
{
  rank_level_list = { 8, 12, 16, 22, 31, 40, 50 };

  parse_options( options_str );

  base_cost                   = 17;
  dd.standardhealthpercentmin =
  dd.standardhealthpercentmax = 0.124;
  dd.power_mod                = 1.24;
  weapon                      = &( player -> main_hand_weapon );
  weapon_multiplier           = -0.17;
  base_multiplier             += .03 * p -> talents.cut_down->rank();
}

// Rifle Shot | Flurry Of Bolts =========================================================

rifle_shot_t::rifle_shot_t( class_t* p, const std::string& n, const std::string& options_str, bool is_consequent_strike, bool is_offhand_attack ) :
  base_t( n, p ), second_strike( 0 ), offhand_attack( 0 )
{
  parse_options( options_str );

  base_cost = 0;
  range = ( player -> type == IA_SNIPER || player -> type == S_GUNSLINGER ) ? 35 : 30.0;
  weapon = &( player->main_hand_weapon );
  weapon_multiplier = -0.5;
  dd.power_mod = 0.5;
  // Is a Basic attack
  base_accuracy -= 0.10;

  if ( is_consequent_strike )
  {
    background = dual = true;
    trigger_gcd = timespan_t::zero();
    base_execute_time = from_seconds( 0.75 );
  }
  else if ( ! is_offhand_attack )
  {
    second_strike = new rifle_shot_t( p, n, options_str, true, is_offhand_attack );
  }

  if ( is_offhand_attack )
  {
    background                 = true;
    dual                       = true;
    trigger_gcd                = timespan_t::zero();
    weapon                     = &( player -> off_hand_weapon );
    rank_level_list            = { 0 };
    dd.power_mod               = 0;
  }
  else if ( player -> type == S_GUNSLINGER )
  {
    offhand_attack             = new rifle_shot_t( p, n+"_offhand", options_str, is_consequent_strike, true );
    add_child( offhand_attack );
  }
}

void rifle_shot_t::execute()
{
  base_t::execute();
  if ( second_strike )
    second_strike->schedule_execute();
  if ( offhand_attack )
    offhand_attack->schedule_execute();
}

// Shiv | Blaster Whip ======================================================

shiv_t::shiv_t( class_t* p, const std::string& n, const std::string& options_str ) :
  base_t( n, p )
{
  rank_level_list = { 2, 5, 8, 11, 14, 19, 29, 38, 50 };

  parse_options( options_str );

  base_cost                    = 15;
  cooldown -> duration         = from_seconds( 6.0 );
  range                        = 4.0;
  dd.standardhealthpercentmin  = 0.148;
  dd.standardhealthpercentmax  = 0.188;
  dd.power_mod                 = 1.68;
}

// Snipe | Charged Burst ====================================================

snipe_t::snipe_t( class_t* p, const std::string& n, const std::string& options_str) :
  base_t( n, p )
{
  parse_options( options_str );

  range = ( player -> type == IA_SNIPER || player -> type == S_GUNSLINGER ) ? 35 : 30.0;

  base_cost                   = energy_cost();
  base_execute_time           = from_seconds( 1.5 );
  dd.power_mod                = 1.85;
  dd.standardhealthpercentmin =
  dd.standardhealthpercentmax = 0.185;
  weapon                      = &( player->main_hand_weapon );
  weapon_multiplier           = 0.23;
}

int snipe_t::energy_cost() { return 20; }

bool snipe_t::ready()
{
  // TODO only ready if in cover
  // cover not yet implemented
  return base_t::ready();
}

// Take Cover | Take Cover ==================================================

take_cover_t::take_cover_t( class_t* p, const std::string& n, const std::string& options_str ) :
  base_t( n, p, tech_policy, RESOURCE_ENERGY, SCHOOL_NONE )
{
  parse_options( options_str );


  use_off_gcd = true;
  trigger_gcd = timespan_t::zero();
}

void take_cover_t::execute()
{
  base_t::execute();

  class_t& p = *cast();
  if ( p.buffs.cover -> check() )
    p.buffs.cover -> expire();
  else
    p.buffs.cover -> trigger();
}

// Weakening Blast | Hemorrhaging Blast =================================================

weakening_blast_t::weakening_blast_t( class_t* p, const std::string& n, const std::string& options_str) :
  base_t( n, p )
{
  check_talent( p -> talents.weakening_blast -> rank() );

  parse_options( options_str );

  range                       = 10.0;
  cooldown -> duration        = from_seconds( 15 );
  dd.standardhealthpercentmin =
  dd.standardhealthpercentmax = 0.087;
  dd.power_mod                = 0.87;
  weapon                      = &( player -> main_hand_weapon );
  weapon_multiplier           = -0.42;
}

void weakening_blast_t::execute()
{
  base_t::execute();

  if ( result_is_hit() )
    targetdata() -> debuff_weakening_blast -> trigger( 10 );
}

// ==========================================================================
// Smuggler / Agent Callbacks
// ==========================================================================

class action_callback_t : public ::action_callback_t
{
public:
  action_callback_t( class_t* player ) :
    ::action_callback_t( player )
  {}

  class_t* p() const { return static_cast<class_t*>( listener ); }
  class_t* cast() const { return p(); }
};

struct poison_crit_callback_t : public action_callback_t
{
  poison_crit_callback_t( class_t* p ) :
    action_callback_t( p )
  {}

  virtual void trigger (::action_t* a, void* /* call_data */)
  {
    if ( dynamic_cast<agent_smug::poison_attack_t*>(a) != NULL )
    {
      std::cout << a -> name() << "XXXX\n";
      p()->resource_gain( RESOURCE_ENERGY, p()->talents.lethal_purpose -> rank(), p()->gains.lethal_purpose );
    }
  }
};

// ==========================================================================
// Smuggler / Agent Character Definition
// ==========================================================================

// class_t::alacrity ============================================
double class_t::alacrity() const
{
  return base_t::alacrity() - ( talents.deadly_directive -> rank() * 0.02 );
}


// class_t::create_action =======================================

::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  if ( name == m.a_adrenaline_probe      ) return new adrenaline_probe_t      ( this, name, options_str ) ;
  if ( name == m.a_coordination          ) return new coordination_t          ( this, name, options_str ) ;
  if ( name == m.a_corrosive_dart        ) return new corrosive_dart_t        ( this, name, options_str ) ;
  if ( name == m.a_corrosive_grenade     ) return new corrosive_grenade_t     ( this, name, options_str ) ;
  if ( name == m.a_explosive_probe       ) return new explosive_probe_t       ( this, name, options_str ) ;
  if ( name == m.a_fragmentation_grenade ) return new fragmentation_grenade_t ( this, name, options_str ) ;
  if ( name == m.a_orbital_strike        ) return new orbital_strike_t        ( this, name, options_str ) ;
  if ( name == m.a_overload_shot         ) return new overload_shot_t         ( this, name, options_str ) ;
  if ( name == m.a_rifle_shot            ) return new rifle_shot_t            ( this, name, options_str ) ;
  if ( name == m.a_shiv                  ) return new shiv_t                  ( this, name, options_str ) ;
  if ( name == m.a_snipe                 ) return new snipe_t                 ( this, name, options_str ) ;
  if ( name == m.a_take_cover            ) return new take_cover_t            ( this, name, options_str ) ;
  if ( name == m.t_weakening_blast       ) return new weakening_blast_t       ( this, name, options_str ) ;

  return base_t::create_action( name, options_str );
}

// class_t::init_talents =======================================

void class_t::init_talents()
{
  base_t::init_talents();

  // Lethality|Dirty Fighting
  // t1
  talents.deadly_directive     = find_talent( m.t_deadly_directive    );
  talents.lethality            = find_talent( m.t_lethality           );
  talents.razor_edge           = find_talent( m.t_razor_edge          );
  // t2
  talents.slip_away            = find_talent( m.t_slip_away           );
  talents.flash_powder         = find_talent( m.t_flash_powder        );
  talents.corrosive_microbes   = find_talent( m.t_corrosive_microbes  );
  talents.lethal_injectors     = find_talent( m.t_lethal_injectors    );
  // t3
  talents.corrosive_grenade    = find_talent( m.t_corrosive_grenade   );
  talents.cut_down             = find_talent( m.t_cut_down            );
  // t4
  talents.lethal_purpose       = find_talent( m.t_lethal_purpose      );
  talents.adhesive_corrosives  = find_talent( m.t_adhesive_corrosives );
  talents.lethal_dose          = find_talent( m.t_lethal_dose         );
  // t5
  talents.cull                 = find_talent( m.t_cull                );
  talents.counterstrike        = find_talent( m.t_counterstrike       );
  // t6
  talents.devouring_microbes   = find_talent( m.t_devouring_microbes  );
  talents.lingering_toxins     = find_talent( m.t_lingering_toxins    );
  // t7
  talents.weakening_blast      = find_talent( m.t_weakening_blast     );
}

// class_t::init_benefits =====================================

void class_t::init_benefits()
{
  base_t::init_benefits();

  benefits.devouring_microbes_ticks = get_benefit( "Poison ticks with Devouring Microbes" );
  benefits.wb_poison_ticks          = get_benefit( "Poison ticks with Weakening Blast" );
}

// class_t::init_buffs ===========================================

void class_t::init_buffs()
{
  base_t::init_buffs();

  buffs.adrenaline_probe   = new buff_t( this , m.a_adrenaline_probe  , 1 ,  from_seconds(  3 ) );
  buffs.cover              = new buff_t( this , m.a_take_cover        , 1);
}

// class_t::init_gains ========================================

void class_t::init_gains()
{
  base_t::init_gains();

  energy_gains.minimum   = get_gain( "min"  );
  energy_gains.low       = get_gain( "low"  );
  energy_gains.medium    = get_gain( "med"  );
  energy_gains.high      = get_gain( "high" );

  gains.adrenaline_probe = get_gain( m.a_adrenaline_probe );
  gains.lethal_purpose   = get_gain( m.t_lethal_purpose );
}

// class_t::init_procs ========================================

void class_t::init_procs()
{
  base_t::init_procs();

  bool is_op = ( type == IA_OPERATIVE );
  const char* corrosive_microbes = is_op ? "Corrosive Microbes tick" : "Mortal Wounds tick" ;

  procs.corrosive_microbes = get_proc( corrosive_microbes );
}

// class_t::init_rng ==========================================

void class_t::init_rng()
{
  base_t::init_rng();

  bool is_op = ( type == IA_OPERATIVE );
  const char* corrosive_microbes = is_op ? "corrosive_microbes" : "mortal_wounds" ;

  rngs.corrosive_microbes = get_rng( corrosive_microbes );
}

// class_t::init_actions ========================================

void class_t::init_actions()
{
  base_t::init_actions();

  // cull extra
  // XXX and tick zeros?
  register_attack_callback( RESULT_CRIT_MASK, new poison_crit_callback_t ( this ) );
  // SERGE: run above with log=1 look for XXX output. now swap it for the one below. no XXX output.
  //register_direct_damage_callback( RESULT_CRIT_MASK, new poison_crit_callback_t ( this ) );
  // poisons
  //register_tick_callback( RESULT_CRIT_MASK, new poison_crit_callback_t ( this ) );
}

// class_t::create_mirror =======================================

void class_t::create_mirror()
{
  bool ag = type == IA_SNIPER || type == IA_OPERATIVE;

  // ABILITY                  =    ? AGENT LABEL              : SMUGGLER LABEL          ;
  m.a_adrenaline_probe        = ag ? "adrenaline_probe"       : "cool_head"             ;
  m.a_coordination            = ag ? "coordination"           : "lucky_shots"           ;
  m.a_corrosive_dart          = ag ? "corrosive_dart"         : "vital_shot"            ;
  m.a_corrosive_grenade       = ag ? "corrosive_grenade"      : "shrap_bomb"            ;
  m.a_corrosive_microbes      = ag ? "corrosive_microbes"     : "mortal_wounds"         ;
  m.a_explosive_probe         = ag ? "explosive_probe"        : "sabotage_charge"       ;
  m.a_fragmentation_grenade   = ag ? "fragmentation_grenade"  : "thermal_grenade"       ;
  m.a_orbital_strike          = ag ? "orbital_strike"         : "xs_freighter_flyby"    ;
  m.a_overload_shot           = ag ? "overload_shot"          : "quick_shot"            ;
  m.a_rifle_shot              = ag ? "rifle_shot"             : "flurry_of_bolts"       ;
  m.a_shiv                    = ag ? "shiv"                   : "blaster_whip"          ;
  m.a_snipe                   = ag ? "snipe"                  : "charged_burst"         ;
  m.a_take_cover              = ag ? "take_cover"             : "take_cover"            ;

  // Lethality|Dirty Fighting
  // t1
  m.t_deadly_directive          = ag ? "deadly_directive"     : "black_market_mods"     ;
  m.t_lethality                 = ag ? "lethality"            : "no_holds_barred"       ;
  m.t_razor_edge                = ag ? "razor_edge"           : "holdout_defense"       ;
  // t2
  m.t_slip_away                 = ag ? "slip_away"            : "dirty_escape"          ;
  m.t_flash_powder              = ag ? "flash_powder"         : "flash_powder"          ;
  m.t_corrosive_microbes        = ag ? "corrosive_microbes"   : "mortal_wound"          ;
  m.t_lethal_injectors          = ag ? "lethal_injectors"     : "open_wound"            ;
  // t3
  m.t_corrosive_grenade         = ag ? "corrosive_grenade"    : "shrap_bomb"            ;
  // m.t_targeted_demolition specified in advanced class
  m.t_cut_down                  = ag ? "cut_down"             : "cheap_shots"           ;
  // t4
  m.t_lethal_purpose            = ag ? "lethal_purpose"       : "fighting_spirit"       ;
  m.t_adhesive_corrosives       = ag ? "adhesive_corrosives"  : "feelin_woozy"          ;
  // m.t_hold_your_ground
  m.t_lethal_dose               = ag ? "lethal_dose"          : "black_market_equipment";
  // t5
  m.t_cull                      = ag ? "cull"                 : "wounding_shots"        ;
  // m.t_razor_round specified in advanced class
  m.t_counterstrike             = ag ? "counterstrike"        : "dirty_trickster"       ;
  // t6
  m.t_devouring_microbes        = ag ? "devouring_microbes"   : "cold_blooded"          ;
  m.t_lingering_toxins          = ag ? "lingering_toxins"     : "nice_try"              ;
  // t7
  m.t_weakening_blast           = ag ? "weakening_blast"      : "hemorrhaging_blast"    ;
}

// class_t::create_talents =======================================

void class_t::create_talents()
{
  // Lethality|Dirty Fighting
  const talentinfo_t lethality_tree[] = {
    { m.t_deadly_directive    , 2 }, { m.t_lethality            , 3 }, { m.t_razor_edge         , 2 },
    { m.t_slip_away           , 2 }, { m.t_flash_powder         , 2 }, { m.t_corrosive_microbes , 2 }, { m.t_lethal_injectors , 1 },
    { m.t_corrosive_grenade   , 1 }, { m.t_targeted_demolition  , 2 }, { m.t_cut_down           , 2 },
    { m.t_lethal_purpose      , 2 }, { m.t_adhesive_corrosives  , 2 }, { m.t_hold_your_ground   , 2 }, { m.t_lethal_dose      , 3 },
    { m.t_cull                , 1 }, { m.t_razor_rounds         , 2 }, { m.t_counterstrike      , 2 },
    { m.t_devouring_microbes  , 3 }, { m.t_lingering_toxins     , 2 },
    { m.t_weakening_blast     , 1 }
  };
  init_talent_tree( IA_LETHALITY, lethality_tree );
}

} // namespace agent_smug ===================================================
