// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// ==========================================================================
// Jediconsular
// ==========================================================================

struct jedi_consular_t : public player_t
{

  // Buffs
  //buff_t* buffs_<buffname>;

  // Gains

  // Procs
  //proc_t* procs_<procname>;


protected:
  jedi_consular_t( sim_t* sim, player_type pt, const std::string& name, race_type r ) :
    player_t( sim, pt, name, ( r == RACE_NONE ) ? RACE_HUMAN : r )
  {

    create_options();
  }
public:
  // Character Definition
  virtual action_t* create_action( const std::string& name, const std::string& options );
  virtual void      init_spells();
  virtual void      init_scaling();
  virtual void      init_buffs();
  virtual void      init_gains();
  virtual void      init_procs();
  virtual void      init_rng();
  virtual void      init_resources( bool force=false );
  virtual int       primary_resource() const;
};

namespace { // ANONYMOUS NAMESPACE ==========================================

// ==========================================================================
// jediconsular Abilities
// ==========================================================================

struct jedi_consular_attack_t : public attack_t
{

  jedi_consular_attack_t( const char* n, uint32_t id, jedi_consular_t* p, int t=TREE_NONE, bool special = true ) :
    attack_t( n, id, p, t, special )
  {
    _init_jedi_consular_attack_t();
  }
  void _init_jedi_consular_attack_t()
  {
    may_crit   = true;
    may_glance = false;
  }

  virtual bool   ready();
};

struct jedi_consular_spell_t : public spell_t
{
  jedi_consular_spell_t( const char* n, jedi_consular_t* p, int r=RESOURCE_NONE, const school_type s=SCHOOL_HOLY, int t=TREE_NONE ) :
    spell_t( n, p, r, s, t )
  {
    _init_jedi_consular_spell_t();
  }

  void _init_jedi_consular_spell_t()
  {
    may_crit   = true;
  }

};

struct project_t : public jedi_consular_spell_t
{
  project_t( jedi_consular_t* p, const std::string& options_str ) :
    jedi_consular_spell_t( "project", p, RESOURCE_FORCE )
  {
    parse_options( 0, options_str );
    base_dd_min=3558.0; base_dd_max=4424.0;
    base_cost = 45.0;
    range=30.0;
    direct_power_mod = 1.85;

    cooldown -> duration = 6.0;
  }
};

struct telekinetic_throw_t : public jedi_consular_spell_t
{
  telekinetic_throw_t( jedi_consular_t* p, const std::string& options_str ) :
    jedi_consular_spell_t( "telekinetic_throw", p, RESOURCE_FORCE )
  {
    parse_options( 0, options_str );
    base_td = 4089.0 / 3.0;
    base_cost = 30.0;
    range=10.0;
    tick_power_mod = 0.79;

    num_ticks = 3;

    base_tick_time = 1.0;

    channeled = true;

    cooldown -> duration = 6.0;
  }
};


} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// jediconsular Character Definition
// ==========================================================================

// jedi_consular_t::create_action ====================================================

action_t* jedi_consular_t::create_action( const std::string& name,
                                 const std::string& options_str )
{
  if ( name == "project"           ) return new            project_t( this, options_str );
  if ( name == "telekinetic_throw" ) return new  telekinetic_throw_t( this, options_str );

  return player_t::create_action( name, options_str );
}


// jedi_consular_t::init_spells ======================================================

void jedi_consular_t::init_spells()
{
  player_t::init_spells();

  // Add Spells & Glyphs

}

// jedi_consular_t::init_scaling =====================================================

void jedi_consular_t::init_scaling()
{
  player_t::init_scaling();

}

// jedi_consular_t::init_buffs =======================================================

void jedi_consular_t::init_buffs()
{
  player_t::init_buffs();

  // buff_t( player, name, max_stack, duration, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

}

// jedi_consular_t::init_gains =======================================================

void jedi_consular_t::init_gains()
{
  player_t::init_gains();

}

// jedi_consular_t::init_procs =======================================================

void jedi_consular_t::init_procs()
{
  player_t::init_procs();

}

// jedi_consular_t::init_rng =========================================================

void jedi_consular_t::init_rng()
{
  player_t::init_rng();

}

// jedi_consular_t::reset ==================================================

void jedi_consular_t::init_resources( bool force )
{
  player_t::init_resources( force);

}

// jedi_consular_t::primary_role ==================================================

int jedi_consular_t::primary_resource() const
{
  return RESOURCE_FORCE;
}

// ==========================================================================
// Jedi Sage
// ==========================================================================

struct jedi_sage_t : public jedi_consular_t
{

  // Buffs
  //buff_t* buffs_<buffname>;

  // Gains

  // Procs
  //proc_t* procs_<procname>;

  // Talents
  struct talents_t
  {
    // TREE_jedi_consular_tANK
    //talent_t* <talentname>;

    // TREE_jediconsular_DAMAGE

    // TREE_jediconsular_HEAL

    talents_t() { memset( ( void* ) this, 0x0, sizeof( talents_t ) ); }
  };
  talents_t talents;

  // Passives
  struct passives_t
  {
    // TREE_jedi_consular_tANK
    // spell_id_t* mastery/passive spells

    // TREE_jediconsular_DAMAGE

    // TREE_jediconsular_HEAL

    passives_t() { memset( ( void* ) this, 0x0, sizeof( passives_t ) ); }
  };
  passives_t passives;

  // Glyphs
  struct glyphs_t
  {
    // Prime
    //glyph_t* <glyphname>;

    // Major

    glyphs_t() { memset( ( void* ) this, 0x0, sizeof( glyphs_t ) ); }
  };
  glyphs_t glyphs;

  jedi_sage_t( sim_t* sim, const std::string& name, race_type r = RACE_NONE ) :
    jedi_consular_t( sim, JEDI_SAGE, name, ( r == RACE_NONE ) ? RACE_HUMAN : r )
  {

    create_talents();
    create_glyphs();
    create_options();
  }

  // Character Definition
  virtual action_t* create_action( const std::string& name, const std::string& options );
  virtual void      init_talents();
  virtual void      init_spells();
  virtual void      init_base();
  virtual void      init_scaling();
  virtual void      init_buffs();
  virtual void      init_gains();
  virtual void      init_procs();
  virtual void      init_rng();
  virtual void      init_actions();
  virtual int       primary_role() const;
};

namespace { // ANONYMOUS NAMESPACE ==========================================

// ==========================================================================
// jediconsular Abilities
// ==========================================================================

struct jedi_sage_attack_t : public attack_t
{

  jedi_sage_attack_t( const char* n, uint32_t id, jedi_sage_t* p, int t=TREE_NONE, bool special = true ) :
    attack_t( n, id, p, t, special )
  {
    _init_jedi_sage_attack_t();
  }
  void _init_jedi_sage_attack_t()
  {
    may_crit   = true;
    may_glance = false;
  }

  virtual bool   ready();
};

struct jedi_sage_spell_t : public spell_t
{
  jedi_sage_spell_t( const char* n, jedi_sage_t* p, int r=RESOURCE_NONE, const school_type s=SCHOOL_HOLY, int t=TREE_NONE ) :
    spell_t( n, p, r, s, t )
  {
    _init_jedi_sage_spell_t();
  }

  void _init_jedi_sage_spell_t()
  {
    may_crit   = true;
  }

};

struct disturbance_t : public jedi_sage_spell_t
{
  disturbance_t( jedi_sage_t* p, const std::string& options_str ) :
    jedi_sage_spell_t( "disturbance", p, RESOURCE_FORCE )
  {
    parse_options( 0, options_str );
    base_dd_min=698.0; base_dd_max=762.0;
    base_execute_time = 1.5;
    base_cost = 30.0;
    range=30.0;
    direct_power_mod = 1.32;
  }
};

struct mind_crush_t : public jedi_sage_spell_t
{
  mind_crush_t( jedi_sage_t* p, const std::string& options_str ) :
    jedi_sage_spell_t( "mind_crush", p, RESOURCE_FORCE )
  {
    parse_options( 0, options_str );
    base_dd_min=3160.0; base_dd_max=4199.0;
    base_td = 5295.0 / 6.0;
    base_tick_time = 2.0;
    num_ticks = 6;
    base_cost = 40.0;
    range=30.0;
    direct_power_mod = 1.23;
    tick_power_mod = 0.295;

    cooldown -> duration = 15.0;
  }
};


} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// jediconsular Character Definition
// ==========================================================================

// jedi_consular_t::create_action ====================================================

action_t* jedi_sage_t::create_action( const std::string& name,
                                 const std::string& options_str )
{
  if ( name == "disturbance" ) return new  disturbance_t( this, options_str );
  if ( name == "mind_crush"  ) return new   mind_crush_t( this, options_str );

  return jedi_consular_t::create_action( name, options_str );
}

// jedi_consular_t::init_talents =====================================================

void jedi_sage_t::init_talents()
{
  jedi_consular_t::init_talents();


  // TREE_jedi_consular_tANK
  //talents.<name>        = find_talent( "<talentname>" );

  // TREE_jediconsular_DAMAGE

  // TREE_jediconsular_HEAL

}

// jedi_consular_t::init_spells ======================================================

void jedi_sage_t::init_spells()
{
  jedi_consular_t::init_spells();

  // Add Spells & Glyphs

}

// jedi_consular_t::init_base ========================================================

void jedi_sage_t::init_base()
{
  jedi_consular_t::init_base();


  default_distance = 10;
  distance = default_distance;

  base_gcd = 1.5; // FIXME: assumption

  resource_base[  RESOURCE_FORCE  ] = 500; // FIXME: placeholder

  base_force_regen_per_second = 8.0; // FIXME: placeholder ( identical to rogue )

  // FIXME: Add defensive constants
  //diminished_kfactor    = 0;
  //diminished_dodge_capi = 0;
  //diminished_parry_capi = 0;
}

// jedi_consular_t::init_scaling =====================================================

void jedi_sage_t::init_scaling()
{
  jedi_consular_t::init_scaling();

}

// jedi_consular_t::init_buffs =======================================================

void jedi_sage_t::init_buffs()
{
  jedi_consular_t::init_buffs();

  // buff_t( player, name, max_stack, duration, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

}

// jedi_consular_t::init_gains =======================================================

void jedi_sage_t::init_gains()
{
  jedi_consular_t::init_gains();

}

// jedi_consular_t::init_procs =======================================================

void jedi_sage_t::init_procs()
{
  jedi_consular_t::init_procs();

}

// jedi_consular_t::init_rng =========================================================

void jedi_sage_t::init_rng()
{
  jedi_consular_t::init_rng();

}

// jedi_consular_t::init_actions =====================================================

void jedi_sage_t::init_actions()
{
  if ( action_list_str.empty() )
  {

    // Flask

    action_list_str += "/snapshot_stats";

    action_list_str += "/mind_crush";

    action_list_str += "/project";

    action_list_str += "/disturbance";

    action_list_default = 1;
  }

  jedi_consular_t::init_actions();
}

// jedi_consular_t::primary_role ==================================================

int jedi_sage_t::primary_role() const
{
  return ROLE_SPELL;
}

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_jediconsular  ===================================================

player_t* player_t::create_jedi_sage( sim_t* sim, const std::string& name, race_type r )
{
  return new jedi_sage_t( sim, name, r );

}

// player_t::jediconsular_init ======================================================

void player_t::jedi_sage_init( sim_t* /* sim */ )
{
  //for ( unsigned int i = 0; i < sim -> actor_list.size(); i++ )
  //{}
}

// player_t::jediconsular_combat_begin ==============================================

void player_t::jedi_sage_combat_begin( sim_t* /* sim */ )
{
  //for ( player_t* p = sim -> player_list; p; p = p -> next )
  //{}
}

