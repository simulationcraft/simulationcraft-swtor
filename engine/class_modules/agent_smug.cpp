// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "agent_smug.hpp"

namespace agent_smug { // ===================================================

targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  ::targetdata_t( source, target )
{
  debuff_weakening_blast     = new buff_t ( this, source.abilities.weakening_blast, 10, from_seconds (  15 ) );
  dot_adrenaline_probe       = dot_t ( source.abilities.adrenaline_probe       , &source );
  dot_corrosive_dart         = dot_t ( source.abilities.corrosive_dart         , &source );
  dot_corrosive_dart_weak    = dot_t ( source.abilities.corrosive_dart_weak    , &source );
  dot_corrosive_grenade      = dot_t ( source.abilities.corrosive_grenade      , &source );
  dot_corrosive_grenade_weak = dot_t ( source.abilities.corrosive_grenade_weak , &source );
  dot_cull                   = dot_t ( source.abilities.cull                   , &source );
  dot_orbital_strike         = dot_t ( source.abilities.orbital_strike         , &source );

  add( *debuff_weakening_blast    );
  add( dot_adrenaline_probe       );
  add( dot_corrosive_dart         );
  add( dot_corrosive_dart_weak    );
  add( dot_corrosive_grenade      );
  add( dot_corrosive_grenade_weak );
  add( dot_cull                   );
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
  may_crit       =  false;
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

  if ( p.talents.weakening_blast -> rank() )
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

// the combat log isn't in sync with the game here.
// the combat log shows after 1.5 seconds a tick of 8 and 34, and then another tick of 8 1.5s later.
// what happens in game is you instantly get 34, and then two ticks of 8.
void adrenaline_probe_t::execute()
{
  base_t::execute();
  class_t* p = cast();

  p -> buffs.adrenaline_probe -> trigger();
  p -> resource_gain( RESOURCE_ENERGY, 34, p -> gains.adrenaline_probe );
}

void adrenaline_probe_t::tick(dot_t* d)
{
  base_t::tick(d);
  class_t* p = cast();

  p -> resource_gain( RESOURCE_ENERGY, 8, p -> gains.adrenaline_probe );
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
    base_cost                   = 20;
    td.standardhealthpercentmin =
    td.standardhealthpercentmax = 0.04;
    td.power_mod                = 0.4;
    num_ticks                   = 5 + p -> talents.lethal_injectors -> rank();

    if ( p -> talents.lingering_toxins -> rank() )
      corrosive_dart_weak = new corrosive_dart_t( p, n + "_weak", options_str, true );
  }
}

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
    base_cost                   = 20;
    cooldown -> duration        = from_seconds( 12.0 );
    td.standardhealthpercentmin =
    td.standardhealthpercentmax = 0.032;
    td.power_mod                = 0.32;
    num_ticks                   = 7;
    tick_zero                   = true;
    // TEST: maybe not limited?
    aoe                         = 3;

    if ( p -> talents.lingering_toxins -> rank() )
      corrosive_grenade_weak = new corrosive_grenade_t( p, n + "_weak", options_str, true );
  }
}

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

  base_cost                   = 20;
  range                       = 30.0;
  cooldown -> duration        = from_seconds( 30 );
  dd.standardhealthpercentmin = 0.23;
  dd.standardhealthpercentmax = 0.25;
  dd.power_mod                = 2.4;
}
// TODO: explosive probe "attaches" to the target and detonates on damage
// (tooltip says damage. game data has text that says blaster damage)
// this is not implemented yet.

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

  base_cost                   = 25;
  range                       =  10.0;
  base_multiplier             += .03 * p -> talents.cut_down->rank();
}

void cull_t::init()
{
  base_t::init();

  extra_strike                = get_extra_strike();
  add_child(extra_strike);
}

cull_extra_t::cull_extra_t( class_t* p, const std::string& n ) :
  base_t( n, p )
{
  dual                        =
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

  base_cost                   = 30;
  range                       = 30.0;
  cooldown -> duration        = from_seconds( 60 );
  td.standardhealthpercentmin =
  td.standardhealthpercentmax = 0.177;
  td.power_mod                = 1.77;
  num_ticks                   = 3; // TODO: sniper set bonus? +1 tick
  base_tick_time              = from_seconds( 3 );
  base_execute_time           = from_seconds( 3 );

  aoe = 99; // TODO FIX: unlimited. "all targets in area"
}

// Overload Shot | Quick Shot ===============================================

overload_shot_t::overload_shot_t( class_t* p, const std::string& n, const std::string& options_str) :
  base_t( n, p )
{
  rank_level_list = { 8, 12, 16, 22, 31, 40, 50 };

  parse_options( options_str );

  base_cost                   = 17;
  range                       = ( player -> type == IA_OPERATIVE || player -> type == S_SCOUNDREL ) ? 10 : 30;
  dd.standardhealthpercentmin =
  dd.standardhealthpercentmax = 0.124;
  dd.power_mod                = 1.24;
  weapon                      = &( player -> main_hand_weapon );
  weapon_multiplier           = -0.17;
  // TODO move this tidbit into scoundrel_operative: 15% comes from operative's "skirmisher" passive
  base_multiplier             += ( player -> type == IA_OPERATIVE || player -> type == S_SCOUNDREL ? 0.15 : 0 )
      +  .03 * p -> talents.cut_down->rank();
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

// Snipe | Charged Burst ====================================================

snipe_t::snipe_t( class_t* p, const std::string& n, const std::string& options_str) :
  base_t( n, p )
{
  parse_options( options_str );

  range = ( player -> type == IA_SNIPER || player -> type == S_GUNSLINGER ) ? 35 : 30.0;

  base_cost                   = 20;
  base_execute_time           = from_seconds( 1.5 );
  dd.power_mod                = 1.85;
  dd.standardhealthpercentmin =
  dd.standardhealthpercentmax = 0.185;
  weapon                      = &( player->main_hand_weapon );
  weapon_multiplier           = 0.23;
}

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

  trigger_gcd = timespan_t::zero();
}

void take_cover_t::execute()
{
  base_t::execute();

  class_t& p = *cast();
  if ( p.buffs.cover -> up() )
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

struct poison_tick_crit_callback_t : public action_callback_t
{
  poison_tick_crit_callback_t( class_t* p ) :
    action_callback_t( p )
  {}

  virtual void trigger (::action_t* a, void* /* call_data */)
  {
    if ( dynamic_cast<agent_smug::poison_attack_t*>(a) != NULL )
    {
      p()->resource_gain( RESOURCE_ENERGY, p()->talents.lethal_purpose -> rank(), p()->gains.lethal_purpose );
    }
  }
};

// ==========================================================================
// Smuggler / Agent Character Definition
// ==========================================================================

// class_t::create_action =======================================

::action_t* class_t::create_action( const std::string& name,
                                    const std::string& options_str )
{
  if ( name == abilities.adrenaline_probe      ) return new adrenaline_probe_t      ( this, name, options_str ) ;
  if ( name == abilities.coordination          ) return new coordination_t          ( this, name, options_str ) ;
  if ( name == abilities.corrosive_dart        ) return new corrosive_dart_t        ( this, name, options_str ) ;
  if ( name == abilities.corrosive_grenade     ) return new corrosive_grenade_t     ( this, name, options_str ) ;
  if ( name == abilities.explosive_probe       ) return new explosive_probe_t       ( this, name, options_str ) ;
  if ( name == abilities.fragmentation_grenade ) return new fragmentation_grenade_t ( this, name, options_str ) ;
  if ( name == abilities.orbital_strike        ) return new orbital_strike_t        ( this, name, options_str ) ;
  if ( name == abilities.overload_shot         ) return new overload_shot_t         ( this, name, options_str ) ;
  if ( name == abilities.rifle_shot            ) return new rifle_shot_t            ( this, name, options_str ) ;
  if ( name == abilities.snipe                 ) return new snipe_t                 ( this, name, options_str ) ;
  if ( name == abilities.take_cover            ) return new take_cover_t            ( this, name, options_str ) ;
  if ( name == abilities.weakening_blast       ) return new weakening_blast_t       ( this, name, options_str ) ;

  return base_t::create_action( name, options_str );
}

// class_t::init_abilities =======================================

void class_t::init_abilities()
{
  //=======================================================================
  //
  //   Please Mirror all changes between Smuggler and Agent!!!
  //
  //=======================================================================
  bool ag = type == IA_SNIPER || type == IA_OPERATIVE;

  // ABILITY                        =    ? AGENT LABEL              : SMUGGLER LABEL       ;
  abilities.adrenaline_probe        = ag ? "adrenaline_probe"       : "cool_head"          ;
  abilities.coordination            = ag ? "coordination"           : "lucky_shots"        ;
  abilities.corrosive_dart          = ag ? "corrosive_dart"         : "vital_shot"         ;
  abilities.corrosive_dart_weak     = ag ? "corrosive_dart_weak"    : "vital_shot_weak"    ;
  abilities.corrosive_grenade       = ag ? "corrosive_grenade"      : "shrap_bomb"         ;
  abilities.corrosive_grenade_weak  = ag ? "corrosive_grenade_weak" : "shrap_bomb_weak"    ;
  abilities.corrosive_microbes      = ag ? "corrosive_microbes"     : "mortal_wounds"      ;
  abilities.corrosive_microbes_tick = ag ? "corrosive_microbes_tick": "mortal_wounds_tick" ;
  abilities.cull                    = ag ? "cull"                   : "wounding_shot"      ;
  abilities.explosive_probe         = ag ? "explosive_probe"        : "sabotage_charge"    ;
  abilities.fragmentation_grenade   = ag ? "fragmentation_grenade"  : "thermal_grenade"    ;
  abilities.lethal_purpose          = ag ? "lethal_purpose"         : "fighting_spirit"    ;
  abilities.orbital_strike          = ag ? "orbital_strike"         : "xs_freighter_flyby" ;
  abilities.overload_shot           = ag ? "overload_shot"          : "quick_shot"         ;
  abilities.rifle_shot              = ag ? "rifle_shot"             : "flurry_of_bolts"    ;
  abilities.shiv                    = ag ? "shiv"                   : "blaster_whip"       ;
  abilities.snipe                   = ag ? "snipe"                  : "charged_burst"      ;
  abilities.take_cover              = ag ? "take_cover"             : "take_cover"         ;
  abilities.weakening_blast         = ag ? "weakening_blast"        : "hemorrhaging_blast" ;

  // buffs
  abilities.cover                   = ag ? "cover"                  : "cover"              ;
}

// class_t::init_talents =======================================

void class_t::init_talents()
{
  base_t::init_talents();

  // Lethality|Dirty Fighting
  // t1
  talents.deadly_directive     = find_talent( "Deadly Directive"     );
  talents.lethality            = find_talent( "Lethality"            );
  talents.razor_edge           = find_talent( "Razor Edge"           );
  // t2
  talents.slip_away            = find_talent( "Slip Away"            );
  talents.flash_powder         = find_talent( "Flash Powder"         );
  talents.corrosive_microbes   = find_talent( "Corrosive Microbes"   );
  talents.lethal_injectors     = find_talent( "Lethal Injectors"     );
  // t3
  talents.corrosive_grenade    = find_talent( "Corrosive Grenade"    );
  // snipers get targeted demolition, operatives get combat stims
  if ( type == IA_OPERATIVE || type == S_SCOUNDREL )
    talents.combat_stims         = find_talent( "Combat Stims"         );
  else
    talents.targeted_demolition  = find_talent( "Targeted Demolition"  );
  talents.cut_down             = find_talent( "Cut Down"             );
  // t4
  talents.lethal_purpose       = find_talent( "Lethal Purpose"       );
  talents.adhesive_corrosives  = find_talent( "Adhesive Corrosives"  );
  // snipers get hold your ground, operatives get escape plan
  if ( type == IA_OPERATIVE || type == S_SCOUNDREL )
    talents.escape_plan          = find_talent( "Escape Plan"          );
  else
  {
    talents.hold_your_ground     = find_talent( "Hold Your Ground"     );
  }
  talents.lethal_dose          = find_talent( "Lethal Dose"          );
  // t5
  talents.cull                 = find_talent( "Cull"                 );
  // snipers get razor rounds, operatives get license to kill
  if ( type == IA_OPERATIVE || type == S_SCOUNDREL )
    talents.license_to_kill      = find_talent( "License to Kill"      );
  else
    talents.razor_rounds         = find_talent( "Razor Rounds"         );
  talents.counterstrike        = find_talent( "Counterstrike"        );
  // t6
  talents.devouring_microbes   = find_talent( "Devouring Microbes"   );
  talents.lingering_toxins     = find_talent( "Lingering Toxins"     );
  // t7
  talents.weakening_blast      = find_talent( "Weakening Blast"      );
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

  buffs.adrenaline_probe   = new buff_t( this , abilities.adrenaline_probe   , 1 ,  from_seconds(  3 ) );
  buffs.cover              = new buff_t( this , abilities.cover              , 1);
}

// class_t::init_gains ========================================

void class_t::init_gains()
{
  base_t::init_gains();

  energy_gains.minimum   = get_gain( "min"  );
  energy_gains.low       = get_gain( "low"  );
  energy_gains.medium    = get_gain( "med"  );
  energy_gains.high      = get_gain( "high" );

  gains.adrenaline_probe = get_gain( abilities.adrenaline_probe );
  gains.lethal_purpose   = get_gain( abilities.lethal_purpose );
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

  register_tick_callback( RESULT_CRIT_MASK, new poison_tick_crit_callback_t ( this ) );
}

// class_t::create_talents =======================================

void class_t::create_talents()
{
  bool is_op_sc = ( type == IA_OPERATIVE || type == S_SCOUNDREL );
  const char* ltkrr  = is_op_sc ? "License to Kill" : "Razor Rounds"         ;
  const char* cstd   = is_op_sc ? "Combat Stims"    : "Targeted Demolition"  ;
  const char* eshyg  = is_op_sc ? "Escape Plan"     : "Hold Your Ground"     ;
  // Lethality|Dirty Fighting
  const talentinfo_t lethality_tree[] = {
     { "Deadly Directive"   , 2 }, { "Lethality"           , 3 }, { "Razor Edge"         , 2 },
     { "Slip Away"          , 2 }, { "Flash Powder"        , 2 }, { "Corrosive Microbes" , 2 }, { "Lethal Injectors" , 1 },
     { "Corrosive Grenade"  , 1 }, { cstd                  , 2 }, { "Cut Down"           , 2 },
     { "Lethal Purpose"     , 2 }, { "Adhesive Corrosives" , 2 }, { eshyg                , 2 }, { "Lethal Dose"      , 3 },
     { "Cull"               , 1 }, { ltkrr                 , 2 }, { "Counterstrike"      , 2 },
     { "Devouring Microbes" , 3 }, { "Lingering Toxins"    , 2 },
     { "Weakening Blast"    , 1 },
  };
  init_talent_tree( is_op_sc ? IA_OPERATIVE_LETHALITY : IA_SNIPER_LETHALITY, lethality_tree );
}

} // namespace agent_smug ===================================================
