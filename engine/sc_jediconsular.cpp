// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

// ==========================================================================
// Jediconsular
// ==========================================================================

struct jediconsular_t : public player_t
{

  // Buffs
  //buff_t* buffs_<buffname>;

  // Gains

  // Procs
  //proc_t* procs_<procname>;

  // Talents
  struct talents_t
  {
    // TREE_jediconsular_TANK
    //talent_t* <talentname>;

    // TREE_jediconsular_DAMAGE

    // TREE_jediconsular_HEAL

    talents_t() { memset( ( void* ) this, 0x0, sizeof( talents_t ) ); }
  };
  talents_t talents;

  // Passives
  struct passives_t
  {
    // TREE_jediconsular_TANK
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

  jediconsular_t( sim_t* sim, const std::string& name, race_type r = RACE_NONE ) :
    player_t( sim, JEDI_CONSULAR, name, ( r == RACE_NONE ) ? RACE_HUMAN : r )
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
  virtual void      init_resources( bool force=false );
  virtual double    matching_gear_multiplier( const attribute_type attr ) const;
  virtual int       decode_set( item_t& item );
  virtual int       primary_resource() const;
  virtual int       primary_role() const;
};

namespace { // ANONYMOUS NAMESPACE ==========================================

// ==========================================================================
// jediconsular Abilities
// ==========================================================================

struct jediconsular_attack_t : public attack_t
{

  jediconsular_attack_t( const char* n, uint32_t id, jediconsular_t* p, int t=TREE_NONE, bool special = true ) :
    attack_t( n, id, p, t, special )
  {
    _init_jediconsular_attack_t();
  }
  void _init_jediconsular_attack_t()
  {
    may_crit   = true;
    may_glance = false;
  }

  virtual bool   ready();
};

struct jediconsular_spell_t : public spell_t
{
  jediconsular_spell_t( const char* n, jediconsular_t* p, int r=RESOURCE_NONE, const school_type s=SCHOOL_HOLY, int t=TREE_NONE ) :
    spell_t( n, p, r, s, t )
  {
    _init_jediconsular_spell_t();
  }

  void _init_jediconsular_spell_t()
  {
    may_crit   = true;
  }

};


struct disturbance_t : public jediconsular_spell_t
{
  disturbance_t( jediconsular_t* p, const std::string& options_str ) :
  jediconsular_spell_t( "disturbance", p, RESOURCE_FORCE )
  {
    parse_options( 0, options_str );
    base_dd_min=698.0; base_dd_max=762.0;
    base_cost = 30.0;
    range=30.0;
  }
};




} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// jediconsular Character Definition
// ==========================================================================

// jediconsular_t::create_action ====================================================

action_t* jediconsular_t::create_action( const std::string& name,
                                 const std::string& options_str )
{
  if ( name == "disturbance" ) return new  disturbance_t( this, options_str );

  return player_t::create_action( name, options_str );
}

// jediconsular_t::init_talents =====================================================

void jediconsular_t::init_talents()
{
  player_t::init_talents();

  // TREE_jediconsular_TANK
  //talents.<name>        = find_talent( "<talentname>" );

  // TREE_jediconsular_DAMAGE

  // TREE_jediconsular_HEAL

}

// jediconsular_t::init_spells ======================================================

void jediconsular_t::init_spells()
{
  player_t::init_spells();

  // Add Spells & Glyphs

}

// jediconsular_t::init_base ========================================================

void jediconsular_t::init_base()
{
  player_t::init_base();


  default_distance = 30;
  distance = default_distance;

  base_gcd = 1.5; // FIXME: assumption

  resource_base[  RESOURCE_FORCE  ] = 500; // FIXME: placeholder

  base_force_regen_per_second = 8.0; // FIXME: placeholder ( identical to rogue )

  // FIXME: Add defensive constants
  //diminished_kfactor    = 0;
  //diminished_dodge_capi = 0;
  //diminished_parry_capi = 0;
}

// jediconsular_t::init_scaling =====================================================

void jediconsular_t::init_scaling()
{
  player_t::init_scaling();

}

// jediconsular_t::init_buffs =======================================================

void jediconsular_t::init_buffs()
{
  player_t::init_buffs();

  // buff_t( player, name, max_stack, duration, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
  // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

}

// jediconsular_t::init_gains =======================================================

void jediconsular_t::init_gains()
{
  player_t::init_gains();

}

// jediconsular_t::init_procs =======================================================

void jediconsular_t::init_procs()
{
  player_t::init_procs();

}

// jediconsular_t::init_rng =========================================================

void jediconsular_t::init_rng()
{
  player_t::init_rng();

}

// jediconsular_t::init_actions =====================================================

void jediconsular_t::init_actions()
{
  if ( action_list_str.empty() )
  {

    // Flask

    action_list_str += "/snapshot_stats";

    action_list_str += "/disturbance";

    action_list_default = 1;
  }

  player_t::init_actions();
}

// jediconsular_t::reset ==================================================

void jediconsular_t::init_resources( bool force )
{
  player_t::init_resources( force);

}

// jediconsular_t::matching_gear_multiplier =========================================

double jediconsular_t::matching_gear_multiplier( const attribute_type /* attr */ ) const
{
  return 0.0;
}

// jediconsular_t::decode_set =======================================================

int jediconsular_t::decode_set( item_t& item )
{
  if ( item.slot != SLOT_HEAD      &&
       item.slot != SLOT_SHOULDERS &&
       item.slot != SLOT_CHEST     &&
       item.slot != SLOT_HANDS     &&
       item.slot != SLOT_LEGS      )
  {
    return SET_NONE;
  }

  //const char* s = item.name();

  //if ( strstr( s, "<setname>"      ) ) return SET_T14_TANK;
  //if ( strstr( s, "<setname>"      ) ) return SET_T14_MELEE;
  //if ( strstr( s, "<setname>"      ) ) return SET_T14_HEAL;

  return SET_NONE;
}

// jediconsular_t::primary_role ==================================================

int jediconsular_t::primary_resource() const
{
  return RESOURCE_FORCE;
}

// jediconsular_t::primary_role ==================================================

int jediconsular_t::primary_role() const
{
  return ROLE_SPELL;
}

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_jediconsular  ===================================================

player_t* player_t::create_jediconsular( sim_t* sim, const std::string& name, race_type r )
{
  return new jediconsular_t( sim, name, r );

}

// player_t::jediconsular_init ======================================================

void player_t::jediconsular_init( sim_t* /* sim */ )
{
  //for ( unsigned int i = 0; i < sim -> actor_list.size(); i++ )
  //{}
}

// player_t::jediconsular_combat_begin ==============================================

void player_t::jediconsular_combat_begin( sim_t* /* sim */ )
{
  //for ( player_t* p = sim -> player_list; p; p = p -> next )
  //{}
}
