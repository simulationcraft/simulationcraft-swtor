// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

struct shadow_assassin_targetdata_t : public targetdata_t
{
  dot_t* dots_crushing_darkness;
  dot_t* dots_creeping_terror;
  dot_t* dots_discharge;

  shadow_assassin_targetdata_t( player_t* source, player_t* target )
    : targetdata_t( source, target )
  {
  }
};

void register_shadow_assassin_targetdata( sim_t* sim )
{
  player_type t = SITH_ASSASSIN;
  typedef shadow_assassin_targetdata_t type;

  REGISTER_DOT( crushing_darkness );
  REGISTER_DOT( creeping_terror );
  REGISTER_DOT( discharge );

}


// ==========================================================================
// Shadow Assassin
// ==========================================================================

struct shadow_assassin_t : public player_t
{
  // Buffs
  struct buffs_t
  {
    buff_t* exploit_weakness;
    buff_t* dark_embrace;
    buff_t* induction;
    buff_t* voltaic_slash;
    buff_t* static_charges;
    buff_t* exploitive_strikes;
    buff_t* raze;
    buff_t* unearthed_knowledge;
  } buffs;

  // Gains
  struct gains_t
  {
    gain_t* parasitism;
    gain_t* dark_embrace;
    gain_t* calculating_mind;
  } gains;

  // Procs
  struct procs_t
  {
    proc_t* exploitive_strikes;
    proc_t* raze;
    proc_t* exploitive_weakness;
  } procs;

  // RNGs
  struct rngs_t
  {
    rng_t* chain_shock;
  } rngs;

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

struct mark_of_power_t : public shadow_assassin_spell_t
{
  mark_of_power_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
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

struct shock_t : public shadow_assassin_spell_t
{
  shadow_assassin_spell_t* chain_shock;

  shock_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str, bool is_chain_shock = false ) :
    shadow_assassin_spell_t( ( n + std::string( is_chain_shock ? "_chain_shock" : "" ) ).c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC ),
    chain_shock( 0 )
  {
    parse_options( 0, options_str );
    base_dd_min = 254.93; base_dd_max = 316.73;
    base_cost = 45.0;
    range = 10.0;
    direct_power_mod = 1.85;

    cooldown -> duration = timespan_t::from_seconds( 6.0 );

    if ( player -> is_shadow_assassin() )
    {
      shadow_assassin_t* p = player -> cast_shadow_assassin();

      if ( !is_chain_shock && p -> talents.chain_shock -> rank() > 0 )
      {
        chain_shock = new shock_t( p, n, options_str, true );
        chain_shock -> base_multiplier *= 0.50;
        chain_shock -> base_cost = 0.0;
        chain_shock -> background = true;
        add_child( chain_shock );
      }
    }
  }

  virtual void execute()
  {
    shadow_assassin_spell_t::execute();

    if ( chain_shock )
    {
      if ( player -> is_shadow_assassin() )
      {
        shadow_assassin_t* p = player -> cast_shadow_assassin();

        if ( p -> rngs.chain_shock -> roll( p -> talents.chain_shock -> rank() * 0.15 ) )
          chain_shock -> execute();
      }
    }
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

 buffs.exploit_weakness = new buff_t( this, "exploit_weakness", 1, timespan_t::from_seconds( 10.0 ), timespan_t::from_seconds( 10.0 ) );
 buffs.induction = new buff_t( this, "induction", 2, timespan_t::from_seconds( 10.0 ), timespan_t::zero );
 buffs.voltaic_slash = new buff_t( this, "voltaic_slash", 2, timespan_t::from_seconds( 10.0 ), timespan_t::zero );
 buffs.static_charges = new buff_t( this, "static_charges", 5, timespan_t::from_seconds( 30.0 ), timespan_t::zero );
 buffs.exploitive_strikes = new buff_t( this, "exploitive_strikes", 1, timespan_t::from_seconds( 10.0 ), timespan_t::zero );
 buffs.raze = new buff_t( this, "raze", 1, timespan_t::from_seconds( 15.0 ), timespan_t::from_seconds( 7.5 ), talents.raze -> rank() * 0.6 );
 buffs.unearthed_knowledge = new buff_t( this, "unearthed_knowledge", 1, timespan_t::from_seconds( 20.0 ), timespan_t::zero, talents.unearthed_knowledge -> rank() * 0.5 );

}

// shadow_assassin_t::init_gains =======================================================

void shadow_assassin_t::init_gains()
{
  player_t::init_gains();

  gains.dark_embrace     = get_gain( "dark_embrace"     );
  gains.parasitism       = get_gain( "parasitism"       );
  gains.calculating_mind = get_gain( "calculating_mind" );

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

  rngs.chain_shock = get_rng( "chain_shock" );
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

  // TREE DARKNESS
  talent_trees[ 0 ].push_back(  new talent_t( this, "Thrashing Blades", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Lightning Reflexes", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Charge Mastery", 0, 3 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Shroud of Darkness", 0, 3 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Lightning Recovery", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Swelling Shadows", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Electric Execution", 0, 3 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Disjunction", 0, 1 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Energize", 0, 1 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Dark Ward", 0, 1 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Premonition", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Hollow", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Blood of Sith", 0, 3 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Electrify", 0, 1 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Eye of the Storm", 0, 1 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Force Pull", 0, 1 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Nerve Wracking", 0, 3 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Harnessed Darkness", 0, 2 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Mounting Darkness", 0, 3 ) );
  talent_trees[ 0 ].push_back(  new talent_t( this, "Wither", 0, 1 ) );

  // TREE DECEPTION
  talent_trees[ 1 ].push_back(  new talent_t( this, "Insulation", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Duplicity", 1, 3 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Dark Embrace", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Obfuscation", 1, 3 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Recirculation", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Avoidance", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Induction", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Surging Charge", 1, 1 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Darkswell", 1, 1 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Deceptive Power", 1, 1 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Entropic Field", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Saber Conduit", 1, 3 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Fade", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Static Cling", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Resourcefulness", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Static Charges", 1, 2 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Low Slash", 1, 1 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Crackling Blasts", 1, 5 ) );
  talent_trees[ 1 ].push_back(  new talent_t( this, "Voltaic Slash", 1, 1 ) );

  // TREE MADNESS
  talent_trees[ 2 ].push_back(  new talent_t( this, "Exploitive Strikes", 2, 3 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Sith Defiance", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Crackling Charge", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Oppressing Force", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Chain Shock", 2, 3 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Parasitism", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Torment", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Death Field", 2, 1 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Fanaticism", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Claws of Decay", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Haunted Dreams", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Corrupted Flesh", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Raze", 2, 1 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Deathmark", 2, 1 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Calculating Mind", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Unearthed Knowledge", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Creeping Death", 2, 3 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Devour", 2, 2 ) );
  talent_trees[ 2 ].push_back(  new talent_t( this, "Creeping Terror", 2, 1 ) );

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

