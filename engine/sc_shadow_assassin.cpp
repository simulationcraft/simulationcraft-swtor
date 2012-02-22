// ==========================================================================
// Dedmonwakeen's DPS-DPM Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

// ==========================================================================
// Tyrus ToDo List
// ==========================================================================
// ----------
// Abilities |
// ----------
//
// Discharge : Add Dark Discharge
// Stealth : NYI
//
// ----------
// Procs     |
// ----------
//
// Add Dark Charge Ability
//
// ----------
// Talents   |
// ----------
//
// MADNESS Tree mostly done - Double check needed on some talents and proc type (melee vs spells procs)
// DECEPTION: NYI (only Exploit Weakness implemented so far
// DARKNESS : Thrashing Blades and Charge mastery only
// ==========================================================================

#include "simulationcraft.h"

enum charge_type_t
{
    CHARGE_NONE=0,
    LIGHTNING_CHARGE,
    SURGING_CHARGE,
    DARK_CHARGE
};

struct shadow_assassin_targetdata_t : public targetdata_t
{
    dot_t* dots_crushing_darkness;
    dot_t* dots_creeping_terror;
    dot_t* dots_lightning_discharge;

    shadow_assassin_targetdata_t( player_t* source, player_t* target )
        : targetdata_t( source, target )
    {}
};

void register_shadow_assassin_targetdata( sim_t* sim )
{
    player_type t = SITH_ASSASSIN;
    typedef shadow_assassin_targetdata_t type;

    REGISTER_DOT( crushing_darkness, crushing_darkness );
    REGISTER_DOT( creeping_terror, creeping_terror );
    REGISTER_DOT( lightning_discharge, lightning_discharge );
}


// ==========================================================================
// Jedi Shadow | Sith Assassin
// ==========================================================================

struct shadow_assassin_t : public player_t
{
    //Active
    struct actives_t
    {
      int charge;
    } actives;

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
        buff_t* recklessness;
        buff_t* deathmark;
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
        proc_t* exploit_weakness;
        proc_t* lightning_charge;
        proc_t* surging_charge;
    } procs;

    // RNGs
    struct rngs_t
    {
        rng_t* chain_shock;
        rng_t* raze;
        rng_t* exploitive_strikes;
        rng_t* lightning_charge;
        rng_t* surging_charge;
        rng_t* static_charges;
    } rngs;

    // Benefits
    struct benefits_t
    {
        benefit_t* discharge;
        benefit_t* crushing_darkness;
        benefit_t* creeping_terror;
    } benefits;

    // Cooldowns
    struct cooldowns_t
    { cooldown_t* crushing_darkness;
    } cooldowns;

    // Talents
    struct talents_t
    {
        // Darkness|Kinetic Combat
        talent_t* thrashing_blades;
        talent_t* charge_mastery;

        // Deception|Infiltration
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
        talent_t* saber_conduit;
        talent_t* fade;
        talent_t* static_cling;
        talent_t* resourcefulness;
        talent_t* static_charges;
        talent_t* low_slash;
        talent_t* crackling_blasts;
        talent_t* voltaic_slash;

        // Madness|Balance
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
    } talents;

    shadow_assassin_t( sim_t* sim, player_type pt, const std::string& name, race_type r = RACE_NONE ) :
        player_t( sim, pt == SITH_ASSASSIN ? SITH_ASSASSIN : JEDI_SHADOW, name, ( r == RACE_NONE ) ? RACE_HUMAN : r )
    {
        if ( pt == SITH_ASSASSIN )
        {
            tree_type[ SITH_ASSASSIN_DARKNESS ]   = TREE_DARKNESS;
            tree_type[ SITH_ASSASSIN_DECEPTION ]  = TREE_DECEPTION;
            tree_type[ SITH_ASSASSIN_MADNESS ]    = TREE_MADNESS;
            cooldowns.crushing_darkness = get_cooldown( "crushing_darkness");
        }
        else
        {
            tree_type[ JEDI_SHADOW_KINETIC_COMBAT ] = TREE_KINETIC_COMBAT;
            tree_type[ JEDI_SHADOW_INFILTRATION ]   = TREE_INFILTRATION;
            tree_type[ JEDI_SHADOW_BALANCE ]        = TREE_BALANCE;
            cooldowns.crushing_darkness = get_cooldown( "mind_crush");
        }

        primary_attribute   = ATTR_WILLPOWER;
        secondary_attribute = ATTR_STRENGTH;


        create_talents();
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
    virtual void      init_spells();
    virtual int       primary_resource() const;
    virtual int       primary_role() const;
    virtual void      regen( timespan_t periodicity );
    virtual double    composite_force_damage_bonus() const;
    virtual double    composite_spell_alacrity() const;
    virtual void      reset();
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
        may_crit   = true;
    }

    virtual void impact( player_t* t, int impact_result, double travel_dmg )
    {
        attack_t::impact( t, impact_result, travel_dmg );

        shadow_assassin_t* p = player -> cast_shadow_assassin();
        shadow_assassin_targetdata_t* td = targetdata() -> cast_shadow_assassin();
        if ( result_is_hit( impact_result ) )
        {

            if ( p -> talents.raze -> rank() > 0 && td -> dots_lightning_discharge -> ticking )
            {
                p -> buffs.raze -> trigger();
                if ( p -> buffs.raze -> up() )
                    p -> cooldowns.crushing_darkness -> reset();
            }

        }
    }

    virtual void player_buff()
    {
        attack_t::player_buff();

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        if ( p -> buffs.exploitive_strikes -> up() )
            player_crit += p -> talents.exploitive_strikes -> rank() * 0.03;
        if ( p -> buffs.unearthed_knowledge -> up() )
            dd.player_multiplier *= 1.10;
    }

   virtual void execute()
   {
      attack_t::execute();
   }


   virtual double armor() const
   {
     double a = attack_t::armor();

     shadow_assassin_t* p = player -> cast_shadow_assassin();

     if ( p -> talents.charge_mastery -> rank() )
     {
       a *= 1.0 - p -> talents.charge_mastery -> rank() * 0.03;
     }

     return a;
   }

};

struct shadow_assassin_spell_t : public spell_t
{
    shadow_assassin_spell_t( const char* n, shadow_assassin_t* p, int r=RESOURCE_NONE, const school_type s=SCHOOL_HOLY, int t=TREE_NONE ) :
        spell_t( n, p, r, s, t )
    {
        may_crit   = true;
        tick_may_crit = true;
    }

    virtual void init()
    {
        spell_t::init();

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        if ( td.base_min > 0 && !channeled )
            crit_bonus += p -> talents.creeping_death -> rank() * 0.1;
    }

    virtual void execute()
    {
        spell_t::execute();

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        if ( dd.base_min > 0 )
            p -> buffs.recklessness -> decrement();
    }

    virtual void player_buff()
    {
        spell_t::player_buff();

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        if ( dd.base_min > 0 && p -> buffs.recklessness -> up() )
            player_crit += 0.60;
    }

    virtual void target_debuff( player_t* t, int dmg_type )
    {
        spell_t::target_debuff( t, dmg_type );

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        // This method is in target_debuff so that it is checked before every dot tick

        // Assume channeled spells don't profit
        if ( p -> talents.deathmark -> rank() > 0 && td.base_min > 0 && !channeled )
            if ( p -> buffs.deathmark -> up() )
                td.target_multiplier *= 1.20;
    }

    virtual void tick( dot_t* d )
    {
        spell_t::tick( d );

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        if ( tick_dmg > 0 && !channeled && p ->buffs.deathmark -> up() )
        {
            p -> buffs.deathmark -> decrement();
            p -> resource_gain( RESOURCE_FORCE, p -> talents.calculating_mind -> rank() , p -> gains.calculating_mind );
        }

        if ( result == RESULT_CRIT  && p -> talents.parasitism -> rank() > 0 )
        {
            double hp = p -> resource_max[ RESOURCE_HEALTH ] * p -> talents.parasitism -> rank() * 0.005 * (1 + p -> talents.devour -> rank() * 0.5);
            p -> resource_gain( RESOURCE_HEALTH, hp , p -> gains.parasitism );
        }
    }

    virtual void assess_damage( player_t* t, double dmg_amount, int dmg_type, int dmg_result )
    {
        spell_t::assess_damage( t, dmg_amount, dmg_type, dmg_result );

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        if ( dmg_result == RESULT_CRIT && p -> talents.exploitive_strikes -> rank() > 0 )
        {
            p -> buffs.exploitive_strikes -> trigger();
        }
    }
};

// Mark of Power | Force Valor =======================

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
                p -> buffs.force_valor -> trigger();
        }
    }

    virtual bool ready()
    {
        if ( player -> buffs.force_valor -> check() )
            return false;

        return shadow_assassin_spell_t::ready();
    }
};

// Shock | Project =======================

struct shock_t : public shadow_assassin_spell_t
{
    shock_t* chain_shock;

    shock_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str, bool is_chain_shock = false ) :
        shadow_assassin_spell_t( ( n + std::string( is_chain_shock ? "_chain_shock" : "" ) ).c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC ),
        chain_shock( 0 )
    {
        static const int ranks[] = { 1, 4, 7, 11, 14, 17, 23, 34, 47, 50 };
        range::copy( ranks, std::back_inserter( rank_level_list ) );

        parse_options( 0, options_str );

        range = 10.0;

        crit_bonus += p -> talents.crackling_blasts -> rank() * 0.10;

        if ( is_chain_shock )
        {
          dd.standardhealthpercentmin = .073;
          dd.standardhealthpercentmax = .113;
          dd.power_mod = 0.925;
          background = true;
        }
        else
        {
            dd.standardhealthpercentmin = .165;
            dd.standardhealthpercentmax = .205;
            dd.power_mod = 1.85;
            base_cost = 45.0 - p -> talents.torment -> rank() * 3;

            cooldown -> duration = timespan_t::from_seconds( 6.0 );

            if ( p -> talents.chain_shock -> rank() > 0 )
            {
                chain_shock = new shock_t( p, n, options_str, true );
                add_child( chain_shock );
            }
        }
    }

    virtual void player_buff()
    {
      shadow_assassin_spell_t::player_buff();

      shadow_assassin_t* p = player -> cast_shadow_assassin();

      player_multiplier += p -> buffs.voltaic_slash -> stack() * 0.15;
    }

    virtual double cost() const
    {
      double c = shadow_assassin_spell_t::cost();

      if ( c <= 0 )
        return 0;

      shadow_assassin_t* p = player -> cast_shadow_assassin();

      c *= 1.0 - p -> buffs.induction -> stack() * 0.25;

      return c;
    }

    virtual void execute()
    {
      shadow_assassin_spell_t::execute();

      shadow_assassin_t* p = player -> cast_shadow_assassin();

      p -> buffs.unearthed_knowledge -> trigger();

      if ( chain_shock )
      {
        if ( p -> rngs.chain_shock -> roll( p -> talents.chain_shock -> rank() * 0.15 ) )
          chain_shock -> execute();
      }
    }
};

// Force Lightning | Telekinetic Throw ======

struct force_lightning_t : public shadow_assassin_spell_t
{
    force_lightning_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
        shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC )
    {
        static const int ranks[] = { 2, 5, 8, 11, 14, 19, 27, 39, 50 };
        range::copy( ranks, std::back_inserter( rank_level_list ) );

        parse_options( 0, options_str );

        td.standardhealthpercentmin = td.standardhealthpercentmax = .079;
        td.power_mod = 0.79;

        base_cost = 30.0;
        if ( player -> set_bonus.rakata_force_masters -> two_pc() )
            base_cost -= 2.0;
        range = 10.0;
        num_ticks = 3;
        base_tick_time = timespan_t::from_seconds( 1.0 );
        may_crit = false;
        channeled = true;
        tick_zero = true;
        cooldown -> duration = timespan_t::from_seconds( 6.0 );
    }
};

// Crushing Darkness | Mind Crush =====================

struct crushing_darkness_t : public shadow_assassin_spell_t
{
    struct crushing_darkness_dot_t : public shadow_assassin_spell_t
    {
        crushing_darkness_dot_t( shadow_assassin_t* p, const std::string& n ) :
            shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC )
        {
            static const int ranks[] = { 14, 19, 30, 41, 50 };
            range::copy( ranks, std::back_inserter( rank_level_list ) );

            td.standardhealthpercentmin = td.standardhealthpercentmax = .0295;
            td.power_mod = 0.295;

            base_tick_time = timespan_t::from_seconds( 1.0 );
            num_ticks = 6;
            range = 10.0;
            background = true;
            may_crit = false;
        }
    };

    crushing_darkness_dot_t* dot_spell;

    crushing_darkness_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
        shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC ),
        dot_spell( new crushing_darkness_dot_t( p, n + "_dot" ) )
    {
        static const int ranks[] = { 14, 19, 30, 41, 50 };
        range::copy( ranks, std::back_inserter( rank_level_list ) );

        parse_options( 0, options_str );

        dd.standardhealthpercentmin = .103;
        dd.standardhealthpercentmax = .143;
        dd.power_mod = 1.23;

        base_execute_time = timespan_t::from_seconds( 2.0 );
        base_cost = 40.0;
        range = 10.0;
        cooldown -> duration = timespan_t::from_seconds( 15.0 );

        add_child( dot_spell );
    }

    virtual void execute()
    {
        shadow_assassin_spell_t::execute();

        dot_spell -> execute();
    }

    virtual double cost() const
    {
        shadow_assassin_t* p = player -> cast_shadow_assassin();

        if ( p -> buffs.raze -> check() > 0 )
            return 0.0;

        return shadow_assassin_spell_t::cost();
    }

    virtual void consume_resource()
    {
        shadow_assassin_spell_t::consume_resource();

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        p -> buffs.raze -> up();
    }

    virtual timespan_t execute_time() const
    {
        timespan_t et = shadow_assassin_spell_t::execute_time();

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        if ( p -> buffs.raze -> up() )
            et = timespan_t::zero;

        return et;
    }

    virtual void update_ready()
    {
        shadow_assassin_spell_t::update_ready();

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        p -> buffs.raze -> expire();
    }
};

// Death Field | Force in Balance =======================

struct death_field_t : public shadow_assassin_spell_t
{
    death_field_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
        shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL )
    {
        check_talent( p -> talents.death_field -> rank() );

        parse_options( 0, options_str );

        dd.standardhealthpercentmin = .167;
        dd.standardhealthpercentmax = .207;
        dd.power_mod = 1.87;

        crit_bonus += p -> talents.creeping_death -> rank() * 0.1 ;

        ability_lag = timespan_t::from_seconds( 0.2 );
        base_cost = 50.0 * ( 1 - p -> talents.fanaticism -> rank() * 0.25 );
        range = 30.0;
        aoe = 3;

        cooldown -> duration = timespan_t::from_seconds( 15.0 );
    }

    virtual void execute()
    {
        shadow_assassin_spell_t::execute();

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        // ToDo: Move buff to targetdata and buff trigger to impact

        p -> buffs.deathmark -> trigger( 10 );
    }
};

// Creeping Terror | Sever Force ==================================

struct creeping_terror_t : public shadow_assassin_spell_t
{
    creeping_terror_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
        shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL )
    {
        check_talent( p -> talents.creeping_terror -> rank() );

        parse_options( 0, options_str );

        td.standardhealthpercentmin = td.standardhealthpercentmax = .031;
        td.power_mod = 0.311;

        base_tick_time = timespan_t::from_seconds( 3.0 );
        num_ticks = 6;
        base_cost = 20.0;
        range = 30.0;
        may_crit = false;
        cooldown -> duration = timespan_t::from_seconds( 9.0 );
        tick_zero = true;
    }

    virtual void target_debuff( player_t* t, int dmg_type )
    {
        shadow_assassin_spell_t::target_debuff( t, dmg_type );

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        if ( p -> talents.deathmark -> rank() > 0 )
            p -> benefits.creeping_terror -> update( p -> buffs.deathmark -> check() > 0 );
    }
};

// Recklessness | Force Potency ==================

struct recklessness_t : public shadow_assassin_spell_t
{
    recklessness_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
        shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL )
    {
        parse_options( 0, options_str );
        cooldown -> duration = timespan_t::from_seconds( 90.0 );
        harmful = false;

        trigger_gcd = timespan_t::zero;
    }

    virtual void execute()
    {
        shadow_assassin_spell_t::execute();

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        p -> buffs.recklessness -> trigger( 2 );
    }
};



// Discharge ======================================

struct discharge_t : public shadow_assassin_spell_t
{
  struct lightning_discharge_t : public shadow_assassin_spell_t
  {
      lightning_discharge_t(shadow_assassin_t* p, const std::string& n ) :
          shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_ENERGY)
      {
          td.standardhealthpercentmin = td.standardhealthpercentmax = .038;
          td.power_mod = 0.38;

          base_tick_time = timespan_t::from_seconds( 3.0 );
          num_ticks = 6;
          may_crit = false;
          tick_zero = true;
          background = true;

          base_multiplier *= 1 + p -> talents.crackling_charge -> rank() * 0.08;
          crit_bonus += p -> talents.crackling_blasts -> rank() * 0.10;
      }

      virtual void target_debuff( player_t* t, int dmg_type )
      {
          shadow_assassin_spell_t::target_debuff( t, dmg_type );

          shadow_assassin_t* p = player -> cast_shadow_assassin();

          if ( p -> talents.deathmark -> rank() > 0 )
              p -> benefits.discharge -> update( p -> buffs.deathmark -> check() > 0 );
      }
  };


  struct surging_discharge_t : public shadow_assassin_spell_t
  {
      surging_discharge_t(shadow_assassin_t* p, const std::string& n ) :
          shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL)
      {
          dd.standardhealthpercentmin = .154;
          dd.standardhealthpercentmax = .194;
          dd.power_mod = 1.74;

          base_tick_time = timespan_t::from_seconds( 3.0 );

          background= true;

          base_multiplier *= 1 + p -> talents.crackling_charge -> rank() * 0.08;
          crit_bonus += p -> talents.crackling_blasts -> rank() * 0.10;
      }
  };

  struct dark_discharge_t : public shadow_assassin_spell_t
  {
      dark_discharge_t(shadow_assassin_t* p, const std::string& n ) :
          shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL)
      {
          background= true;

          // FIME: Implement Dark Discharge
          crit_bonus += p -> talents.crackling_blasts -> rank() * 0.10;
      }
  };

  shadow_assassin_spell_t* lightning_discharge;
  shadow_assassin_spell_t* surging_discharge;
  shadow_assassin_spell_t* dark_discharge;

  discharge_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
    shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE )
  {
    parse_options( 0, options_str );

    range = 10.0;
    base_cost = 20.0;



    lightning_discharge = new lightning_discharge_t( p, "lightning_discharge" );
    surging_discharge   = new surging_discharge_t( p, "surging_discharge" );
    dark_discharge      = new dark_discharge_t( p, "dark_discharge" );

    add_child( lightning_discharge );
    add_child( surging_discharge );
    add_child( dark_discharge );
  }

  shadow_assassin_spell_t* choose_charge()
  {
    shadow_assassin_t* p = player -> cast_shadow_assassin();

    if ( p -> actives.charge == LIGHTNING_CHARGE )
      return lightning_discharge;
    else if ( p -> actives.charge == SURGING_CHARGE )
      return surging_discharge;
    else if ( p -> actives.charge == DARK_CHARGE )
      return dark_discharge;

    return 0;
  }

  virtual void execute()
  {
    shadow_assassin_t* p = player -> cast_shadow_assassin();

    cooldown -> duration = timespan_t::from_seconds( 15.0 - p -> talents.recirculation -> rank() * 1.5 );

    if ( p -> actives.charge == LIGHTNING_CHARGE && p -> talents.crackling_charge -> rank() > 0 )
        cooldown -> duration -= cooldown -> duration * p -> talents.crackling_charge -> rank() * 0.25 ;

    shadow_assassin_spell_t::execute();

    shadow_assassin_spell_t* charge_action = choose_charge();

    if ( ! charge_action )
      return;

    charge_action -> execute();
  }
};

// Apply Charge ======================================

struct apply_charge_t : public shadow_assassin_spell_t
{
    int charge;

    apply_charge_t(shadow_assassin_t* p, const std::string& options_str ) :
      shadow_assassin_spell_t( "apply_charge", p, RESOURCE_FORCE ),
        charge( CHARGE_NONE )
    {
      std::string type_str;
      option_t options[] =
      {
        { "type",       OPT_STRING, &( type_str      ) },
        { NULL, OPT_UNKNOWN, NULL }
      };

      parse_options( options, options_str );

      // Maybe add a cleaner way to set charges, possibly on shadow_assassin_t level if it's needed in other places too
      if ( type_str == "lightning" )
        charge = LIGHTNING_CHARGE;
      else if ( type_str =="surging" && p -> talents.surging_charge -> rank() > 0 )
        charge = SURGING_CHARGE;
      else if ( type_str == "dark")
        charge = DARK_CHARGE;

      if ( charge == CHARGE_NONE )
      {
        sim -> errorf( "%s %s: No valid charge selected. charge_str '%s'.\n", player -> name(), name(), type_str.c_str() );
      }

      base_cost = 100.0;

      harmful = false;

    }

    virtual void execute()
    {
      shadow_assassin_spell_t::execute();

      shadow_assassin_t* p = player -> cast_shadow_assassin();

      p -> actives.charge = charge;
    }

    virtual bool ready()
    {
      shadow_assassin_t* p = player -> cast_shadow_assassin();

      if ( charge == p -> actives.charge )
        return false;

      return shadow_assassin_spell_t::ready();
    }
};

// Low Slash =====================================

struct low_slash_t : public shadow_assassin_attack_t
{
  low_slash_t( shadow_assassin_t* p,const std::string& n,const std::string& options_str) :
    shadow_assassin_attack_t( n.c_str(),p,RESOURCE_FORCE, SCHOOL_PHYSICAL, TREE_NONE )
  {
    parse_options( 0, options_str );

    dd.standardhealthpercentmin = dd.standardhealthpercentmax = .132;
    dd.power_mod = 1.32;

    weapon = &( player -> main_hand_weapon );
    weapon_multiplier = -0.12;

    base_cost = 30;
    range = 4.0;
    cooldown -> duration = timespan_t::from_seconds( 15 );
  }
};

// Voltaic Slash | Clairvoyant Strike ===============

struct voltaic_slash_t : public shadow_assassin_attack_t
{
  voltaic_slash_t* second_strike;

  voltaic_slash_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str, bool is_second_strike = false) :
    shadow_assassin_attack_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_PHYSICAL ),
    second_strike( 0 )
  {
    parse_options( 0, options_str );

    dd.standardhealthpercentmin = dd.standardhealthpercentmax = .08;
    dd.power_mod = .8;

    weapon = &( player -> main_hand_weapon );
    weapon_multiplier = -0.465;

    range = 4.0;

    if ( is_second_strike )
    {
      background = true;
      dual = true;
    }
    else
    {
      check_talent( p -> talents.voltaic_slash -> rank() );

      base_cost = 25.0;

      second_strike = new voltaic_slash_t( p, n, options_str, true );
      add_child( second_strike );
    }
  }

  virtual void execute()
  {
    shadow_assassin_attack_t::execute();

    shadow_assassin_t* p = player -> cast_shadow_assassin();

    // FIXME: Do both hits give a stack of Induction, or only one?
    if ( p -> actives.charge == SURGING_CHARGE )
      p -> buffs.induction -> trigger();

    if ( second_strike )
    {
      p -> buffs.voltaic_slash -> trigger( 1 );
      second_strike -> execute();
    }
  }
};

// Overcharge Saber | Battle Readiness ================

struct overcharge_saber_t : public shadow_assassin_spell_t
{
  overcharge_saber_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
    shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL )
  {
    parse_options( 0, options_str );
    cooldown -> duration = timespan_t::from_seconds( 120.0 );
    harmful = false;

    trigger_gcd = timespan_t::zero;
  }

  virtual void execute()
  {
    shadow_assassin_spell_t::execute();

    //shadow_assassin_t* p = player -> cast_shadow_assassin();

    //p -> buffs.overcharge_saber -> trigger();
  }
};

// Assassinate | Spinning Strike ==============================================

struct assassinate_t : public shadow_assassin_attack_t
{

  assassinate_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
    shadow_assassin_attack_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_PHYSICAL )
  {
    parse_options( 0, options_str );

    dd.standardhealthpercentmin = dd.standardhealthpercentmax = .309;
    dd.power_mod = 3.09;

    weapon = &( player -> main_hand_weapon );
    weapon_multiplier = 1.06;

    base_cost = 25.0;
    range = 4.0;
    cooldown -> duration = timespan_t::from_seconds( 6.0 );
  }

  virtual void execute()
  {
    shadow_assassin_attack_t::execute();

    shadow_assassin_t* p = player -> cast_shadow_assassin();

    if ( p -> actives.charge == SURGING_CHARGE )
      p -> buffs.induction -> trigger();
  }

  virtual bool ready()
  {
    if ( target -> health_percentage() >= 30 )
        return false;

    return shadow_assassin_attack_t::ready();
  }
};

// Lacerate | Whirling Blow ==================================

struct lacerate_t : public shadow_assassin_attack_t
{

  lacerate_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
    shadow_assassin_attack_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_PHYSICAL )
  {
    parse_options( 0, options_str );

    dd.standardhealthpercentmin = dd.standardhealthpercentmax = .071;
    dd.power_mod = 0.71;

    weapon = &( player -> main_hand_weapon );
    weapon_multiplier = -0.52;

    base_cost = 40;
    range = 4.0;

    base_multiplier *= 1.0 + p -> talents.thrashing_blades -> rank() * 0.03;
  }
};

// Blackout ================================

struct blackout_t : public shadow_assassin_spell_t
{
    blackout_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
        shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL )
    {
        parse_options( 0, options_str );
        cooldown -> duration = timespan_t::from_seconds( 60.0 );
        harmful = false;
        trigger_gcd = timespan_t::zero;
    }

    virtual void execute()
    {
        shadow_assassin_spell_t::execute();

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        p -> buffs.dark_embrace -> trigger( 1 );
    }
};

// Force Cloak ==========================================

struct force_cloak_t : public shadow_assassin_spell_t
{
  force_cloak_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str ) :
    shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL )
  {
    parse_options( 0, options_str );
    cooldown -> duration = timespan_t::from_seconds( 180.0 );
    harmful = false;

    trigger_gcd = timespan_t::zero;
  }

  virtual void execute()
  {
    shadow_assassin_spell_t::execute();

    shadow_assassin_t* p = player -> cast_shadow_assassin();

    p -> buffs.dark_embrace -> trigger( 1 );
  }
};


// Maul | Shadow Strike ===================

struct maul_t : public shadow_assassin_attack_t
{
    maul_t( shadow_assassin_t* p,const std::string& n,const std::string& options_str) :
        shadow_assassin_attack_t(n.c_str(), p, RESOURCE_FORCE, SCHOOL_PHYSICAL )
    {
        parse_options( 0, options_str );

        dd.standardhealthpercentmin = .236;
        dd.standardhealthpercentmax = .236;
        dd.power_mod = 2.37;

        weapon = &( player -> main_hand_weapon );
        weapon_multiplier = 0.58;

        base_cost = 50.0;
        range = 4.0;
    }

    virtual void execute()
    {
      shadow_assassin_attack_t::execute();

      shadow_assassin_t* p = player -> cast_shadow_assassin();

      p -> buffs.exploit_weakness -> expire();
    }

    virtual double cost() const
    {
      double c = shadow_assassin_attack_t::cost();

      shadow_assassin_t* p = player -> cast_shadow_assassin();

      if ( p -> buffs.exploit_weakness -> up() )
      {
        c *= 0.5;
      }

      return c;
    }

    virtual double armor() const
    {
      double a = shadow_assassin_attack_t::armor();

      shadow_assassin_t* p = player -> cast_shadow_assassin();

      if ( p -> buffs.exploit_weakness -> up() )
      {
        a *= 0.5;
      }

      return a;
    }
};

// Saber Strike ==================================

struct saber_strike_t : public shadow_assassin_attack_t
{
  saber_strike_t* second_strike;
  saber_strike_t* third_strike;

  saber_strike_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str, bool is_consequent_strike = false ) :
    shadow_assassin_attack_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_PHYSICAL ),
    second_strike( 0 ), third_strike( 0 )
  {
    parse_options( 0, options_str );

    base_cost = 0;
    range = 4.0;

    if ( is_consequent_strike )
    {
      weapon_multiplier = -.033;
      dd.power_mod = .33;
      background = true;
      dual = true;
    }
    else
    {
      weapon_multiplier = -.066;
      dd.power_mod = .66;

      second_strike = new saber_strike_t( p, n, options_str, true );
      third_strike = new saber_strike_t( p, n, options_str, true );

      add_child( second_strike );
      add_child( third_strike );
    }
  }

  virtual void execute()
  {
    shadow_assassin_attack_t::execute();

    if ( second_strike )
    {
      second_strike -> execute();
    }
    if ( third_strike )
    {
      third_strike -> execute();
    }
  }
};

// Thrash | Double Strike ==================================

struct thrash_t : public shadow_assassin_attack_t
{
  thrash_t* second_strike;

  thrash_t( shadow_assassin_t* p, const std::string& n, const std::string& options_str, bool is_second_strike = false ) :
    shadow_assassin_attack_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_PHYSICAL ),
    second_strike( 0 )
 {
    parse_options( 0, options_str );

    dd.standardhealthpercentmin = dd.standardhealthpercentmax = .074;
    dd.power_mod = 0.74;

    weapon = &( player -> main_hand_weapon );
    weapon_multiplier = -0.505;

    range = 4.0;

    base_multiplier *= 1.0 + p -> talents.thrashing_blades -> rank() * 0.03;
    crit_bonus += p -> talents.claws_of_decay -> rank() * 0.25;

    if ( is_second_strike )
    {
      background = true;
      dual = true;
    }
    else
    {
      base_cost = 25 - p -> talents.torment -> rank() * 1.0;

      second_strike = new thrash_t( p, n, options_str, true );
      add_child( second_strike );
    }
  }

  virtual void execute()
  {
    shadow_assassin_attack_t::execute();

    shadow_assassin_t* p = player -> cast_shadow_assassin();

    if ( p -> actives.charge == SURGING_CHARGE )
      p -> buffs.induction -> trigger();

    if ( second_strike )
    {
      second_strike -> execute();
    }
  }
};

// Action Callbacks ( Charge procs )

// Lightning Charge | Force Technique ===========

struct lightning_charge_callback_t : public action_callback_t
{

  struct lightning_charge_spell_t : public shadow_assassin_spell_t
  {
    lightning_charge_spell_t(shadow_assassin_t* p, const std::string& n ) :
      shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC )
    {

      dd.standardhealthpercentmin = dd.standardhealthpercentmax = .017;
      dd.power_mod = 0.165;

      proc = true;
      background = true;
      cooldown -> duration = timespan_t::from_seconds( 1.5 );

      base_crit *= 1.0 + p -> talents.charge_mastery -> rank() * 0.01;
      base_multiplier *= 1.0 + p -> talents.charge_mastery -> rank() * 0.06;
    }

  };

  rng_t* rng_lightning_charge;
  shadow_assassin_spell_t* lightning_charge_damage_proc;

  lightning_charge_callback_t( shadow_assassin_t* p ) :
    action_callback_t( p -> sim, p ), lightning_charge_damage_proc( 0 )
  {
    rng_lightning_charge = p -> get_rng( "lightning_charge" );
    lightning_charge_damage_proc = new lightning_charge_spell_t( p, p -> type == SITH_ASSASSIN ? "lightning_charge" : "force_technique" );
  }

  virtual void trigger( action_t* /* a */, void* /* call_data */ )
  {
    //weapon_t* w = a -> weapon;
    //if ( ! w ) return;

    if ( ! rng_lightning_charge -> roll( 0.5 ) )
      return;

    shadow_assassin_t* p = listener -> cast_shadow_assassin();

    if ( p -> actives.charge != LIGHTNING_CHARGE )
      return;

    if ( lightning_charge_damage_proc -> cooldown -> remains() > timespan_t::zero )
      return;

    lightning_charge_damage_proc -> execute();
  }
};

// Surging Charge | Shadow Technique ========================

struct surging_charge_callback_t : public action_callback_t
{

  struct surging_charge_spell_t : public shadow_assassin_spell_t
  {
      surging_charge_spell_t( shadow_assassin_t* p, const std::string& n ) :
          shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_INTERNAL )
      {
          dd.standardhealthpercentmin = dd.standardhealthpercentmax = .034;
          dd.power_mod = 0.344;

          proc = true;
          background = true;
          cooldown -> duration = timespan_t::from_seconds( 1.5 );
      }

      virtual void player_buff()
      {
        shadow_assassin_spell_t::player_buff();

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        player_multiplier += p -> buffs.static_charges -> stack() * 0.06;
      }

      virtual void execute()
      {
        shadow_assassin_spell_t::execute();

        shadow_assassin_t* p = player -> cast_shadow_assassin();

        p -> buffs.static_charges -> trigger( 1 );
      }
  };

  rng_t* rng_surging_charge;
  shadow_assassin_spell_t* surging_charge_damage_proc;

  surging_charge_callback_t( shadow_assassin_t* p ) :
    action_callback_t( p -> sim, p ), surging_charge_damage_proc( 0 )
  {
    rng_surging_charge = p -> get_rng( "surging_charge" );
    surging_charge_damage_proc = new surging_charge_spell_t( p, p -> type == SITH_ASSASSIN ? "surging_charge" : "shadow_technique" );
  }

  virtual void trigger( action_t* /* a */, void* /* call_data */ )
  {
    //weapon_t* w = a -> weapon;
    //if ( ! w ) return;

    if ( ! rng_surging_charge -> roll( 0.25 ) )
      return;

    shadow_assassin_t* p = listener -> cast_shadow_assassin();

    if ( p -> actives.charge != SURGING_CHARGE )
      return;

    if ( surging_charge_damage_proc -> cooldown -> remains() > timespan_t::zero )
      return;

    surging_charge_damage_proc -> execute();
  }
};

// Lightning Charge | Force Technique ===========

struct dark_charge_callback_t : public action_callback_t
{

  struct dark_charge_spell_t : public shadow_assassin_spell_t
  {
    dark_charge_spell_t(shadow_assassin_t* p, const std::string& n ) :
      shadow_assassin_spell_t( n.c_str(), p, RESOURCE_FORCE, SCHOOL_KINETIC )
    {
      // FIXME: ADD correct values and implement heal and other effects
      //dd_standardhealthpercentmin = dd_standardhealthpercentmax = .017;
      //direct_power_mod = 0.165;

      proc = true;
      background = true;
      cooldown -> duration = timespan_t::from_seconds( 4.5 );

      base_crit *= 1.0 + p -> talents.charge_mastery -> rank() * 0.01;
      base_multiplier *= 1.0 + p -> talents.charge_mastery -> rank() * 0.06;
    }

  };

  rng_t* rng_dark_charge;
  shadow_assassin_spell_t* dark_charge_damage_proc;

  dark_charge_callback_t( shadow_assassin_t* p ) :
    action_callback_t( p -> sim, p ), dark_charge_damage_proc( 0 )
  {
    rng_dark_charge = p -> get_rng( "dark_charge" );
    dark_charge_damage_proc = new dark_charge_spell_t( p, p -> type == SITH_ASSASSIN ? "dark_charge" : "force_technique" );
  }

  virtual void trigger( action_t* /* a */, void* /* call_data */ )
  {
    //weapon_t* w = a -> weapon;
    //if ( ! w ) return;

    if ( ! rng_dark_charge -> roll( 0.5 ) )
      return;

    shadow_assassin_t* p = listener -> cast_shadow_assassin();

    if ( p -> actives.charge != DARK_CHARGE )
      return;

    if ( dark_charge_damage_proc -> cooldown -> remains() > timespan_t::zero )
      return;

    dark_charge_damage_proc -> execute();
  }
};

struct duplicity_callback_t : action_callback_t
{
  duplicity_callback_t( shadow_assassin_t* p ) :
    action_callback_t( p -> sim, p )
  {

  }

  virtual void trigger( action_t* /* a */, void* /* call_data */ )
  {
    shadow_assassin_t* p = listener -> cast_shadow_assassin();

    p -> buffs.exploit_weakness -> trigger();
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
    if ( type == SITH_ASSASSIN )
    {
        if ( name == "mark_of_power"      ) return new       mark_of_power_t( this, "mark_of_power", options_str );
        if ( name == "shock"              ) return new               shock_t( this, "shock", options_str );
        if ( name == "force_lightning"    ) return new     force_lightning_t( this, "force_lightning", options_str );
        if ( name == "crushing_darkness"  ) return new   crushing_darkness_t( this, "crushing_darkness", options_str );
        if ( name == "death_field"        ) return new         death_field_t( this, "death_field", options_str );
        if ( name == "creeping_terror"    ) return new     creeping_terror_t( this, "creeping_terror", options_str );
        if ( name == "recklessness"       ) return new        recklessness_t( this, "recklessness", options_str );
        if ( name == "low_slash"          ) return new           low_slash_t( this, "low_slash", options_str );
        if ( name == "voltaic_slash"      ) return new       voltaic_slash_t( this, "voltaic_slash", options_str );
        if ( name == "overcharge_saber"   ) return new    overcharge_saber_t( this, "overcharge_saber", options_str );
        if ( name == "assassinate"        ) return new         assassinate_t( this, "assassinate", options_str );
        if ( name == "lacerate"           ) return new            lacerate_t( this, "lacerate", options_str );
        if ( name == "blackout"           ) return new            blackout_t( this, "blackout", options_str );
        if ( name == "force_cloak"        ) return new         force_cloak_t( this, "force_cloak", options_str );
        if ( name == "discharge"          ) return new           discharge_t( this, "discharge", options_str );
        if ( name == "maul"               ) return new                maul_t( this, "maul", options_str );
        if ( name == "saber_strike"       ) return new        saber_strike_t( this, "saber_strike", options_str );
        if ( name == "thrash"             ) return new              thrash_t( this, "thrash", options_str );
    }
    else if ( type == JEDI_SHADOW )
    {
        if ( name == "force_valor"        ) return new       mark_of_power_t( this, "force_valor", options_str );
        if ( name == "project"            ) return new               shock_t( this, "project", options_str );
        if ( name == "telekinetic_throw"  ) return new     force_lightning_t( this, "telekinetic_throw", options_str );
        if ( name == "mind_crush"         ) return new   crushing_darkness_t( this, "mind_crush", options_str );
        if ( name == "force_in_balance"   ) return new         death_field_t( this, "force_in_balance", options_str );
        if ( name == "sever_force"        ) return new     creeping_terror_t( this, "sever_force", options_str );
        if ( name == "force_potency"      ) return new        recklessness_t( this, "force_potency", options_str );
        if ( name == "low_slash"          ) return new           low_slash_t( this, "low_slash", options_str );
        if ( name == "clairvoyant_strike" ) return new       voltaic_slash_t( this, "clairvoyant_strike", options_str );
        if ( name == "battle_readiness"   ) return new    overcharge_saber_t( this, "battle_readiness", options_str );
        if ( name == "spinning_strike"    ) return new         assassinate_t( this, "spinning_strike", options_str );
        if ( name == "whirling_blow"      ) return new            lacerate_t( this, "whirling_blow", options_str );
        if ( name == "blackout"           ) return new            blackout_t( this, "blackout", options_str );
        if ( name == "force_cloak"        ) return new         force_cloak_t( this, "force_cloak", options_str );
        if ( name == "force_breach"       ) return new           discharge_t( this, "force_breach", options_str );
        if ( name == "shadow_strike"      ) return new                maul_t( this, "shadow_strike", options_str );
        if ( name == "saber_strike"       ) return new        saber_strike_t( this, "saber_strike", options_str );
        if ( name == "double_strike"      ) return new              thrash_t( this, "double_strike", options_str );
    }

    if ( name == "apply_charge"      ) return new              apply_charge_t( this, options_str );

    return player_t::create_action( name, options_str );
}

// shadow_assassin_t::init_talents =====================================================

void shadow_assassin_t::init_talents()
{
    player_t::init_talents();

    // Darkness|Kinetic Combat
    talents.thrashing_blades      = find_talent( "Thrashing Blades" );
    talents.charge_mastery        = find_talent( "Charge Mastery" );

    // Deception|Infiltration
    talents.insulation            = find_talent( "Insulation" );
    talents.duplicity             = find_talent( "Duplicity" );
    talents.dark_embrace          = find_talent( "Dark Embrace" );
    talents.obfuscation           = find_talent( "Obfuscation" );
    talents.recirculation         = find_talent( "Recirculation" );
    talents.avoidance             = find_talent( "Avoidance" );
    talents.induction             = find_talent( "Induction" );
    talents.surging_charge        = find_talent( "Surging Charge" );
    talents.darkswell             = find_talent( "Darkswell" );
    talents.deceptive_power       = find_talent( "Deceptive Power" );
    talents.entropic_field        = find_talent( "Entropic Field" );
    talents.saber_conduit         = find_talent( "Saber Conduit" );
    talents.fade                  = find_talent( "Fade" );
    talents.static_cling          = find_talent( "Static Cling" );
    talents.resourcefulness       = find_talent( "Resourcefulness" );
    talents.static_charges        = find_talent( "Static Charges" );
    talents.low_slash             = find_talent( "Low Slash" );
    talents.crackling_blasts      = find_talent( "Crackling Blasts" );
    talents.voltaic_slash         = find_talent( "Voltaic Slash" );

    // Madness|Balance
    talents.exploitive_strikes    = find_talent( "Exploitive Strikes" );
    talents.sith_defiance         = find_talent( "Sith Defiance" );
    talents.crackling_charge      = find_talent( "Crackling Charge" );
    talents.oppressing_force      = find_talent( "Oppressing Force" );
    talents.chain_shock           = find_talent( "Chain Shock" );
    talents.parasitism            = find_talent( "Parasitism" );
    talents.torment               = find_talent( "Torment" );
    talents.death_field           = find_talent( "Death Field" );
    talents.fanaticism            = find_talent( "Fanaticism" );
    talents.claws_of_decay        = find_talent( "Claws of Decay" );
    talents.haunted_dreams        = find_talent( "Haunted Dreams" );
    talents.corrupted_flesh       = find_talent( "Corrupted Flesh" );
    talents.raze                  = find_talent( "Raze" );
    talents.deathmark             = find_talent( "Deathmark" );
    talents.calculating_mind      = find_talent( "Calculating Mind" );
    talents.unearthed_knowledge   = find_talent( "Unearthed Knowledge" );
    talents.creeping_death        = find_talent( "Creeping Death" );
    talents.devour                = find_talent( "Devour" );
    talents.creeping_terror       = find_talent( "Creeping Terror" );
}

// shadow_assassin_t::init_base ========================================================

void shadow_assassin_t::init_base()
{
    player_t::init_base();

    attribute_base[ ATTR_WILLPOWER ] = 250;

    default_distance = 3;
    distance = default_distance;

    base_force_regen_per_second = 8.0;
    resource_base[  RESOURCE_FORCE  ] += 100 + talents.deceptive_power -> rank() * 10;

    // FIXME: Add defensive constants
    //diminished_kfactor    = 0;
    //diminished_dodge_capi = 0;
    //diminished_parry_capi = 0;
}

// shadow_assassin_t::init_benefits =======================================================

void shadow_assassin_t::init_benefits()
{
    player_t::init_benefits();

    if ( type == SITH_ASSASSIN )
    {
        benefits.discharge           = get_benefit( "Discharge ticks with Deathmark"        );
        benefits.crushing_darkness   = get_benefit( "Crushing Darkness ticks with Deathmark");
        benefits.creeping_terror     = get_benefit( "Creeping Terror ticks with Deathmark"  );
    }
    else
    {
        benefits.discharge           = get_benefit( "Weaken Mind ticks with Force Suppression");
        benefits.crushing_darkness   = get_benefit( "Mind Crush ticks with Force Suppression" );
        benefits.creeping_terror     = get_benefit( "Sever Force ticks with Force Suppression");
    }
}

// shadow_assassin_t::init_buffs =======================================================

void shadow_assassin_t::init_buffs()
{
    player_t::init_buffs();

    // buff_t( player, name, max_stack, duration, cd=-1, chance=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
    // buff_t( player, id, name, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )
    // buff_t( player, name, spellname, chance=-1, cd=-1, quiet=false, reverse=false, rng_type=RNG_CYCLIC, activated=true )

    bool is_shadow = ( type == JEDI_SHADOW );

    buffs.exploit_weakness = new buff_t( this, "exploit_weakness", 1, timespan_t::from_seconds( 10.0 ), timespan_t::from_seconds( 10.0 ), talents.duplicity -> rank () * 0.1 );
    buffs.dark_embrace = new buff_t( this, "dark_embrace", 1, timespan_t::from_seconds( 6.0 ), timespan_t::zero );
    buffs.induction = new buff_t( this, "induction", 2, timespan_t::from_seconds( 10.0 ), timespan_t::zero, talents.induction -> rank() * 0.5 );
    buffs.voltaic_slash = new buff_t( this, "voltaic_slash", 2, timespan_t::from_seconds( 10.0 ), timespan_t::zero );
    buffs.static_charges = new buff_t( this, "static_charges", 5, timespan_t::from_seconds( 30.0 ), timespan_t::zero );
    buffs.exploitive_strikes = new buff_t( this, "exploitive_strikes", 1, timespan_t::from_seconds( 10.0 ), timespan_t::zero );
    buffs.raze = new buff_t( this, "raze", 1, timespan_t::from_seconds( 15.0 ), timespan_t::from_seconds( 7.5 ), talents.raze -> rank() * 0.6 );
    buffs.unearthed_knowledge = new buff_t( this, "unearthed_knowledge", 1, timespan_t::from_seconds( 20.0 ), timespan_t::zero, talents.unearthed_knowledge -> rank() * 0.5 );
    buffs.recklessness = new buff_t( this, is_shadow ? "force_potency" : "recklessness", 2, timespan_t::from_seconds( 20.0 ) );
    buffs.deathmark = new buff_t( this, "deathmark", 10, timespan_t::from_seconds( 30.0 ), timespan_t::zero );

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

    procs.exploitive_strikes  = get_proc( "exploitive_strikes" );
    procs.exploit_weakness    = get_proc( "exploit_weakness"   );
    procs.raze                = get_proc( "raze"               );
    procs.lightning_charge    = get_proc( "lightning_charge"   );
    procs.surging_charge      = get_proc( "surging_charge"     );
}

// shadow_assassin_t::init_rng =========================================================

void shadow_assassin_t::init_rng()
{
    player_t::init_rng();

    rngs.chain_shock        = get_rng( "chain_shock"         );
    rngs.raze               = get_rng( "raze"                );
    rngs.exploitive_strikes = get_rng( "exploitive_strikes " );
    rngs.lightning_charge   = get_rng( "lightning_charge"    );
    rngs.surging_charge     = get_rng( "surging_charge"      );
    rngs.static_charges     = get_rng( "static_charges"      );
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
            action_list_str += "stim,type=exotech_resolve";
            action_list_str += "/force_valor";
            action_list_str += "/snapshot_stats";

            switch ( primary_tree() )
            {
            case TREE_BALANCE:

                // Balance Actions

                break;


            case TREE_INFILTRATION:

                // Infiltration Actions

                break;

            default: break;
            }

            action_list_default = 1;
        }

        // Sith ASSASSIN
        else
        {
            action_list_str += "stim,type=exotech_resolve";
            action_list_str += "/mark_of_power";
            action_list_str += "/snapshot_stats";

            switch ( primary_tree() )
            {
            case TREE_MADNESS:
              action_list_str += "/apply_charge,type=lightning";
              action_list_str += "/power_potion";
              action_list_str += "/recklessness";
              action_list_str += "/death_field";
              action_list_str += "/crushing_darkness,if=buff.raze.react";
              action_list_str += "/discharge,if=!dot.lightning_discharge.ticking";
              action_list_str += "/shock,if=buff.unearthed_knowledge.down";
              action_list_str += "/assassinate,if=target.health_pct<=30";
              action_list_str += "/maul,if=buff.exploit_weakness.react";
              action_list_str += "/creeping_terror,if=!ticking";
              action_list_str += "/thrash";
              action_list_str += "/saber_strike";

                break;


            case TREE_DECEPTION:
              action_list_str += "/apply_charge,type=surging";
              action_list_str += "/power_potion";
              action_list_str += "/recklessness";
              action_list_str += "/assassinate";
              action_list_str += "/maul,if=buff.exploit_weakness.react";
              action_list_str += "/discharge";
              action_list_str += "/shock,if=buff.induction.stack=2";
              action_list_str += "/voltaic_slash";
              action_list_str += "/saber_strike";

                break;

            default: break;
            }

            action_list_default = 1;
        }
    }

    player_t::init_actions();
}

// shadow_assassin_t::init_spells ==================================================

void shadow_assassin_t::init_spells()
{
  player_t::register_callbacks();

  action_callback_t* lc = new lightning_charge_callback_t( this );
  action_callback_t* sc = new surging_charge_callback_t( this );
  action_callback_t* dc = new dark_charge_callback_t( this );

  register_attack_callback( RESULT_HIT_MASK, lc );
  register_attack_callback( RESULT_HIT_MASK, sc );
  register_attack_callback( RESULT_HIT_MASK, dc );

  register_spell_callback( RESULT_HIT_MASK, lc );
  register_spell_callback( RESULT_HIT_MASK, sc );
  register_spell_callback( RESULT_HIT_MASK, dc );

  register_attack_callback( RESULT_HIT_MASK, new duplicity_callback_t( this ) );
  register_spell_callback( RESULT_HIT_MASK, new duplicity_callback_t( this ) );
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

void shadow_assassin_t::reset()
{
  player_t::reset();

  actives.charge = CHARGE_NONE;
}

// shadow_assassin_t::create_talents ==================================================

void shadow_assassin_t::create_talents()
{
    static const struct
    {
        const char* name;
        int maxpoints;
    } talent_descriptions[] = {
        // Darkness|Kinetic Combat
        { "Thrashing Blades", 2 }, { "Lightning Reflexes", 2 }, { "Charge Mastery", 3 },
        { "Shroud of Darkness", 3 }, { "Lightning Recovery", 2 }, { "Swelling Shadows", 2 }, { "Electric Execution", 3 },
        { "Disjunction", 1 }, { "Energize", 1 }, { "Dark Ward", 1 }, { "Premonition", 2 },
        { "Hollow", 2 }, { "Blood of Sith", 3 },
        { "Electrify", 1 }, { "Eye of the Storm", 1 }, { "Force Pull", 1 }, { "Nerve Wracking", 3 },
        { "Harnessed Darkness", 2 }, { "Mounting Darkness", 3 },
        { "Wither", 1 },
        { 0, 0 },

        // Deception|Infiltration
        { "Insulation", 2 }, { "Duplicity", 3 }, { "Dark Embrace", 2 },
        { "Obfuscation", 3 }, { "Recirculation", 2 }, { "Avoidance", 2 },
        { "Induction", 2 }, { "Surging Charge", 1 }, { "Darkswell", 1 }, { "Deceptive Power", 1 },
        { "Entropic Field", 2 }, { "Saber Conduit", 3 }, { "Fade", 2 }, { "Static Cling", 2 },
        { "Resourcefulness", 2 }, { "Static Charges", 2 }, { "Low Slash", 1 },
        { "Crackling Blasts", 5 },
        { "Voltaic Slash", 1 },
        { 0, 0 },

        // Madness|Balance
        { "Exploitive Strikes", 3 }, { "Sith Defiance", 2 }, { "Crackling Charge", 2 },
        { "Oppressing Force", 2 }, { "Chain Shock", 3 }, { "Parasitism", 2 }, { "Torment", 2 },
        { "Death Field", 1 }, { "Fanaticism", 2 }, { "Claws of Decay", 2 },
        { "Haunted Dreams", 2 }, { "Corrupted Flesh", 2 }, { "Raze", 1 },
        { "Deathmark", 1 }, { "Calculating Mind", 2 }, { "Unearthed Knowledge", 2 },
        { "Creeping Death", 3 }, { "Devour", 2 },
        { "Creeping Terror", 1 },
        { 0, 0 },
    };

    unsigned i = 0;
    for ( unsigned tree = 0; tree < 3; ++tree )
    {
        for(; talent_descriptions[ i ].name != 0; ++i )
        {
            talent_trees[ tree ].push_back( new talent_t( this, talent_descriptions[ i ].name, tree,
                                                          talent_descriptions[ i ].maxpoints ) );
        }
        ++i;
    }

    player_t::create_talents();
}

// ==========================================================================
// PLAYER_T EXTENSIONS
// ==========================================================================

// player_t::create_jedi_shadow  ===================================================

player_t* player_t::create_jedi_shadow( sim_t* sim, const std::string& name, race_type r )
{
    return new shadow_assassin_t( sim, JEDI_SHADOW, name, r );
}

// player_t::create_sith_assassin  ===================================================

player_t* player_t::create_sith_assassin( sim_t* sim, const std::string& name, race_type r )
{
    return new shadow_assassin_t( sim, SITH_ASSASSIN, name, r );
}

// player_t::shadow_assassin_init ======================================================

void player_t::shadow_assassin_init( sim_t* /* sim */ )
{
#if 0
    for ( unsigned int i = 0; i < sim -> actor_list.size(); i++ )
    {
        //player_t* p = sim -> actor_list[i];
        // Force Valor || Mark of Power constructed in sage_sorcerer_init !!!
    }
#endif
}

// player_t::shadow_assassin_combat_begin ==============================================

void player_t::shadow_assassin_combat_begin( sim_t* /* sim */ )
{
#if 0
    for ( player_t* p = sim -> player_list; p; p = p -> next )
    {
        if ( p -> ooc_buffs() )
        {
            // Force Valor || Mark of Power is taken care of in sage_sorcerer_combat_begin !!!
        }
    }
#endif
}


