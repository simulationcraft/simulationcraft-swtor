// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

struct shadow_assassin_targetdata_t : public targetdata_t
{

  shadow_assassin_targetdata_t( player_t* source, player_t* target )
    : targetdata_t( source, target )
  {
  }
};

void register_shadow_assassin_targetdata( sim_t* /* sim */ )
{
  //player_type t = JEDI_SHADOW;
  typedef shadow_assassin_targetdata_t type;

}


// ==========================================================================
// Shadow Assassin
// ==========================================================================

struct shadow_assassin_t : public player_t
{

  // Buffs

  // Gains

  // Procs
  //proc_t* procs_<procname>;

  // RNG


  // Talents
  struct talents_t
  {
    // TREE_DARKNESS
    talent_t* thrashing_blades;
    talent_t* charge_mastery;    

    // TREE_DECEPTION
    talent_t* insulation;
    talent_t* duplicity;
    talent_t* dark_embrace;
    talent_t* obfuscation;
    talent_t* recirculation;
    talent_t* avoidance;
    talent_t* induction;
    talent_t* surging_charge;
    talent_t* darkswell;
    talent_t* deceptive_power;
    talent_t* entropic_field;
    talent_t* saber_conduct;
    talent_t* fade;
    talent_t* static_cling;
    talent_t* resourcefulness;
    talent_t* static_charges;
    talent_t* low_slash;
    talent_t* crackling_blasts;
    talent_t* voltaic_slash;
    
    // TREE_MADNESS
    talent_t* exploitive_strikes;
    talent_t* sith_defiance;
    talent_t* crackling_charge;
    talent_t* oppressing_force;
    talent_t* chain_shock;
    talent_t* parasitism;
    talent_t* torment;
    talent_t* death_field;
    talent_t* fanaticism;
    talent_t* claws_of_decay;
    talent_t* haunted_dreams;
    talent_t* corrupted_flesh;
    talent_t* raze;
    talent_t* deathmark;
    talent_t* calculating_mind;
    talent_t* unearthed_knowledge;
    talent_t* creeping_death;
    talent_t* devour;
    talent_t* creeping_terror;

    talents_t() { memset( ( void* ) this, 0x0, sizeof( talents_t ) ); }
  };
  talents_t talents;

  shadow_assassin_t( sim_t* sim, player_type pt, const std::string& name, race_type r = RACE_NONE ) :
    player_t( sim, pt == SITH_ASSASSIN ? SITH_ASSASSIN : JEDI_SHADOW, name, ( r == RACE_NONE ) ? RACE_HUMAN : r )
  {
    if ( pt == SITH_ASSASSIN )
    {
      tree_type[ SITH_ASSASSIN_DARKNESS ]   = TREE_DARKNESS;
      tree_type[ SITH_ASSASSIN_DECEPTION ]  = TREE_DECEPTION;
      tree_type[ SITH_ASSASSIN_MADNESS ]    = TREE_MADNESS;
    }
    else
    {
      tree_type[ JEDI_SHADOW_KINETIC_COMBAT ] = TREE_KINETIC_COMBAT;
      tree_type[ JEDI_SHADOW_INFILTRATION ]   = TREE_INFILTRATION;
      tree_type[ JEDI_SHADOW_BALANCE ]        = TREE_BALANCE;
    }

    create_talents();
    create_glyphs();
    create_options();
  }

  // Character Definition
  virtual targetdata_t* new_targetdata( player_t* source, player_t* target ) {return new shadow_assassin_targetdata_t( source, target );}
  virtual action_t* create_action( const std::string& name, const std::string& options );
  virtual void      init_talents();
  virtual void      init_base();
  virtual void      init_benefits();
  virtual void      init_buffs();
  virtual void      init_gains();
  virtual void      init_procs();
  virtual void      init_rng();
  virtual void      init_actions();
  virtual int       primary_resource() const;
  virtual int       primary_role() const;
  virtual void      regen( timespan_t periodicity );
  virtual double    composite_force_damage_bonus() const;
  virtual double    composite_spell_alacrity() const;
  virtual void      create_talents();
};

namespace { // ANONYMOUS NAMESPACE ==========================================


// ==========================================================================
// Sith assassin Abilities
// ==========================================================================

struct shadow_assassin_attack_t : public attack_t
{
  shadow_assassin_attack_t( const char* n, shadow_assassin_t* p, int r=RESOURCE_NONE, const school_type s=SCHOOL_HOLY, int t=TREE_NONE ) :
    attack_t( n, p, r, s, t )
  {
    _init_shadow_assassin_attack_t();
  }

  void _init_shadow_assassin_attack_t()
  {
    may_crit   = true;
    may_glance = false;
  }
};

struct shadow_assassin_spell_t : public spell_t
{

  shadow_assassin_spell_t( const char* n, shadow_assassin_t* p, int r=RESOURCE_NONE, const school_type s=SCHOOL_HOLY, int t=TREE_NONE ) :
    spell_t( n, p, r, s, t )
  {
    _init_shadow_assassin_spell_t();
  }

  void _init_shadow_assassin_spell_t()
  {
    may_crit   = true;
    tick_may_crit = true;
  }

};

struct force_valor_t : public shadow_assassin_spell_t
{
  force_valor_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
    shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE )
  {
    parse_options( 0, options_str );
    base_cost = 0.0;
    harmful = false;
  }

  virtual void execute()
  {
    shadow_assassin_spell_t::execute();

    for ( player_t* p = sim -> player_list; p; p = p -> next )
    {
      if ( p -> ooc_buffs() )
        p -> buffs.force_valor -> override();
    }
  }

  virtual bool ready()
  {
    if ( player -> buffs.force_valor -> check() )
      return false;

    return shadow_assassin_spell_t::ready();
  }
};

struct project_t : public shadow_assassin_spell_t
{
  shadow_assassin_spell_t* upheaval;

  project_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str, bool is_upheaval = false ) :
    shadow_assassin_spell_t( ( n + std::string( is_upheaval ? "_upheaval" : "" ) ).c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC ),
    upheaval( 0 )
  {
    parse_options( 0, options_str );
    base_dd_min = 219.0; base_dd_max = 283.4;
    base_cost = 45.0;
    range = 10.0;
    direct_power_mod = 1.85;

    cooldown -> duration = timespan_t::from_seconds( 6.0 );

  }

};

struct telekinetic_throw_t : public shadow_assassin_spell_t
{

  telekinetic_throw_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
    shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC )
  {
    parse_options( 0, options_str );
    base_td = 127.2;
    base_cost = 10.0;
    if ( player -> set_bonus.indomitable -> two_pc() > 0 )
      base_cost -= 2.0;
    range = 30.0;
    tick_power_mod = 0.79;
    num_ticks = 3;
    base_tick_time = timespan_t::from_seconds( 1.0 );
    may_crit = false;
    channeled = true;
    tick_zero = true;

    cooldown -> duration = timespan_t::from_seconds( 6.0 );

  }
};


} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// shadow_assassin Character Definition
// ==========================================================================

// shadow_assassin_t::create_action ====================================================

action_t* shadow_assassin_t::create_action( const std::string& name,
                                 const std::string& options_str )
{
#if 0
  if ( type == SITH_ASSASSIN )
  {
    if ( name == "mark_of_power"      ) return new     mark_of_power_t( this, "mark_of_power", options_str );
    if ( name == "shock"              ) return new             shock_t( this, "shock", options_str );
    if ( name == "force_lightning"    ) return new   force_lightning_t( this, "force_lightning", options_str );
    if ( name == "crushing_darkness"  ) return new crushing_darkness_t( this, "crushing_darkness", options_str );
    if ( name == "death_field"        ) return new       death_field_t( this, "death_field", options_str );
    if ( name == "creeping_terror"    ) return new   creeping_terror_t( this, "creeping_terror", options_str );
    if ( name == "recklessness"       ) return new      recklessness_t( this, "recklessness", options_str );
    if ( name == "lightning_charge"   ) return new  lightning_charge_t( this, "lightning_charge", options_str );
    if ( name == "surging_charge"     ) return new    surging_charge_t( this, "surging_charge", options_str );
    if ( name == "low_slash"          ) return new         low_slash_t( this, "low_slash", options_str );
    if ( name == "voltaic_slash"      ) return new     voltaic_slash_t( this, "voltaic_slash", options_str );
    if ( name == "overcharge_saber"   ) return new  overcharge_saber_t( this, "overcharge_saber", options_str );
    if ( name == "assassinate"        ) return new       assassinate_t( this, "assassinate", options_str );
    if ( name == "lacerate"           ) return new          lacerate_t( this, "lacerate", options_str );
    if ( name == "blackout"           ) return new          blackout_t( this, "blackout", options_str );
    if ( name == "force_cloak"        ) return new       force_cloak_t( this, "force_cloak", options_str );
    if ( name == "discharge"          ) return new         discharge_t( this, "discharge", options_str );
    if ( name == "maul"               ) return new              maul_t( this, "maul", options_str );
    if ( name == "saber_strike"       ) return new      saber_strike_t( this, "saber_strike", options_str );
    if ( name == "thrash"             ) return new            thrash_t( this, "thrash", options_str );
  }
  else if ( type == JEDI_SHADOW )
  {
    if ( name == "force_valor"        ) return new        force_valor_t( this, "mark_of_power", options_str );
    if ( name == "project"            ) return new            project_t( this, "shock", options_str );
    if ( name == "telekinetic_throw"  ) return new  telekinetic_throw_t( this, "force_lightning", options_str );
    if ( name == "mind_crush"         ) return new         mind_crush_t( this, "crushing_darkness", options_str );
    if ( name == "force_in_balance"   ) return new   force_in_balance_t( this, "death_field", options_str );
    if ( name == "sever_force"        ) return new        sever_force_t( this, "creeping_terror", options_str );
    if ( name == "force_potency"      ) return new      force_potency_t( this, "recklessness", options_str );
    if ( name == "force_technique"    ) return new    force_technique_t( this, "lightning_charge", options_str );
    if ( name == "shadow_technique"   ) return new   shadow_technique_t( this, "surging_charge", options_str );
    if ( name == "low_slash"          ) return new          low_slash_t( this, "low_slash", options_str );
    if ( name == "clairvoyant_strike" ) return new clairvoyant_strike_t( this, "voltaic_slash", options_str );
    if ( name == "battle_readiness"   ) return new   battle_readiness_t( this, "overcharge_saber", options_str );
    if ( name == "spinning_strike"    ) return new    spinning_strike_t( this, "assassinate", options_str );
    if ( name == "whirling_blow"      ) return new      whirling_blow_t( this, "lacerate", options_str );
    if ( name == "blackout"           ) return new           blackout_t( this, "blackout", options_str );
    if ( name == "force_cloak"        ) return new        force_cloak_t( this, "force_cloak", options_str );
    if ( name == "force_breach"       ) return new       force_breach_t( this, "discharge", options_str );
    if ( name == "shadow_strike"      ) return new      shadow_strike_t( this, "maul", options_str );
    if ( name == "saber_strike"       ) return new       saber_strike_t( this, "saber_strike", options_str );
    if ( name == "double_strike"      ) return new      double_strike_t( this, "thrash", options_str );
  }
#endif

  return player_t::create_action( name, options_str );
}

// shadow_assassin_t::init_talents =====================================================

void shadow_assassin_t::init_talents()
{
  player_t::init_talents();

     // TREE_DARKNESS
    talents.thrashing_blades = find_talent( "Thrashing Blades" );
    talents.charge_mastery = find_talent( "Charge Mastery" );    

    // TREE_DECEPTION
    talents.insulation = find_talent( "Insulation" );
    talents.duplicity = find_talent( "Duplicity" );
    talents.dark_embrace = find_talent( "Dark Embrace" );
    talents.obfuscation = find_talent( "Obfuscation" );
    talents.recirculation = find_talent( "Recirculation" );
    talents.avoidance = find_talent( "Avoidance" );
    talents.induction = find_talent( "Induction" );
    talents.surging_charge = find_talent( "Surging Charge" );
    talents.darkswell = find_talent( "Darkswell" );
    talents.deceptive_power = find_talent( "Deceptive Power" );
    talents.entropic_field = find_talent( "Entropic Field" );
    talents.saber_conduct = find_talent( "Saber Conduct" );
    talents.fade = find_talent( "Fade" );
    talents.static_cling = find_talent( "Static Cling" );
    talents.resourcefulness = find_talent( "Resourcefulness" );
    talents.static_charges = find_talent( "Static Charges" );
    talents.low_slash = find_talent( "Low Slash" );
    talents.crackling_blasts = find_talent( "Crackling Blasts" );
    talents.voltaic_slash = find_talent( "Voltaic Slash" );
    
    // TREE_MADNESS
    talents.exploitive_strikes = find_talent( "Exploitive Strikes" );
    talents.sith_defiance = find_talent( "Sith Defiance" );
    talents.crackling_charge = find_talent( "Crackling Charge" );
    talents.oppressing_force = find_talent( "Oppressing Force" );
    talents.chain_shock = find_talent( "Chain Shock" );
    talents.parasitism = find_talent( "Parasitism" );
    talents.torment = find_talent( "Torment" );
    talents.death_field = find_talent( "Death Field" );
    talents.fanaticism = find_talent( "Fanaticism" );
    talents.claws_of_decay = find_talent( "Claws Of Decay" );
    talents.haunted_dreams = find_talent( "Haunted Dreams" );
    talents.corrupted_flesh = find_talent( "Corrupted Flesh" );
    talents.raze = find_talent( "Raze" );
    talents.deathmark = find_talent( "Deathmark" );
    talents.calculating_mind = find_talent( "Calculating Mind" );
    talents.unearthed_knowledge = find_talent( "Unearthed Knowledge" );
    talents.creeping_death = find_talent( "Creeping Death" );
    talents.devour = find_talent( "Devour" );
    talents.creeping_terror = find_talent( "Creeping Terror" );
  
 // talents.name = find_talent( "NAME" );

}

// shadow_assassin_t::init_base ========================================================

void shadow_assassin_t::init_base()
{
  player_t::init_base();

  base_gcd = timespan_t::from_seconds( 1.5 );

  attribute_base[ ATTR_WILLPOWER ] = 250;

  default_distance = 3;
  distance = default_distance;

  base_force_regen_per_second = 8.0;
  resource_base[  RESOURCE_FORCE  ] += 100;

  // FIXME: Add defensive constants
  //diminished_kfactor    = 0;
  //diminished_dodge_capi = 0;
  //diminished_parry_capi = 0;
}
// shadow_assassin_t::init_benefits =======================================================

void shadow_assassin_t::init_benefits()
{
  player_t::init_benefits();

}

// shadow_assassin_t::init_buffs =======================================================

void shadow_assassin_t::init_buffs()
{
  player_t::init_buffs();

  // buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

  // bool is_shadow = ( type == JEDI_SHADOW );


}

// shadow_assassin_t::init_gains =======================================================

void shadow_assassin_t::init_gains()
{
  player_t::init_gains();

}

// shadow_assassin_t::init_procs =======================================================

void shadow_assassin_t::init_procs()
{
  player_t::init_procs();

}

// shadow_assassin_t::init_rng =========================================================

void shadow_assassin_t::init_rng()
{
  player_t::init_rng();

}

// shadow_assassin_t::init_actions =====================================================

void shadow_assassin_t::init_actions()
{

//======================================================================================
//
//   Please Mirror all changes between Jedi Shadow and Sith Assassin!!!
//
//======================================================================================

  if ( action_list_str.empty() )
  {
    if ( type == JEDI_SHADOW )
    {
      switch ( primary_tree() )
      {
      case TREE_BALANCE:

      action_list_str += "stim,type=exotech_resolve";

      action_list_str += "/force_valor";

      action_list_str += "/snapshot_stats";

      break;


      case TREE_INFILTRATION:

      action_list_str += "stim,type=exotech_resolve";

      action_list_str += "/force_valor";

      action_list_str += "/snapshot_stats";


      break;

      default: break;
      }

      action_list_default = 1;
    }

    // Sith ASSASSIN
    else
    {
      switch ( primary_tree() )
      {
      case TREE_MADNESS:

      action_list_str += "stim,type=exotech_resolve";

      action_list_str += "/mark_of_power";

      action_list_str += "/snapshot_stats";

      break;


      case TREE_DECEPTION:

      action_list_str += "stim,type=exotech_resolve";

      action_list_str += "/mark_of_power";

      action_list_str += "/snapshot_stats";

      break;

      default: break;
      }

      action_list_default = 1;
    }
  }

  player_t::init_actions();
}

// shadow_assassin_t::primary_resource ==================================================

int shadow_assassin_t::primary_resource() const
{
  return RESOURCE_FORCE;
}

// shadow_assassin_t::primary_role ==================================================

int shadow_assassin_t::primary_role() const
{
  switch ( player_t::primary_role() )
  {
  case ROLE_TANK:
    return ROLE_TANK;
  case ROLE_DPS:
  case ROLE_HYBRID:
    return ROLE_HYBRID;
  default:
    if ( primary_tree() == TREE_KINETIC_COMBAT || primary_tree() == TREE_DARKNESS )
      return ROLE_TANK;
    break;
  }

  return ROLE_HYBRID;
}

// shadow_assassin_t::regen ==================================================

void shadow_assassin_t::regen( timespan_t periodicity )
{
  player_t::regen( periodicity );

}

// shadow_assassin_t::composite_spell_power ==================================================

double shadow_assassin_t::composite_force_damage_bonus() const
{
  double sp = player_t::composite_force_damage_bonus();


  return sp;
}

// shadow_assassin_t::composite_spell_alacrity ==================================================

double shadow_assassin_t::composite_spell_alacrity() const
{
  double sh = player_t::composite_spell_alacrity();

  return sh;
}

// shadow_assassin_t::create_talents ==================================================

void shadow_assassin_t::create_talents()
{
  // talent_trees[ tree_nr ].push_back( new talent_t( this, "TalentName", tree_nr,max_points ) );

  // eg.   talent_trees[ 0 ].push_back(  new talent_t( this, "Immutable Force", 0, 2 ) );


  player_t::create_talents();
}

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_shadow_assassin  ===================================================

player_t* player_t::create_jedi_shadow( sim_t* sim, const std::string& name, race_type r )
{
  return new shadow_assassin_t( sim, JEDI_SHADOW, name, r );
}

// player_t::create_sith_sorcerer  ===================================================

player_t* player_t::create_sith_assassin( sim_t* sim, const std::string& name, race_type r )
{
  return new shadow_assassin_t( sim, SITH_ASSASSIN, name, r );
}

// player_t::shadow_assassin_init ======================================================

void player_t::shadow_assassin_init( sim_t* sim )
{
  for ( unsigned int i = 0; i < sim -> actor_list.size(); i++ )
  {
    //player_t* p = sim -> actor_list[i];
    //p -> buffs.force_valor = new buff_t( p, "force_valor_mark_of_power", 1 );
  }
}

// player_t::shadow_assassin_combat_begin ==============================================

void player_t::shadow_assassin_combat_begin( sim_t* sim )
{
  for ( player_t* p = sim -> player_list; p; p = p -> next )
  {
    if ( p -> ooc_buffs() )
    {
      if ( sim -> overrides.force_valor )
        p -> buffs.force_valor -> override();
    }
  }
}

