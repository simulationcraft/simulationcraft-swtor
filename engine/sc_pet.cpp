// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

// ==========================================================================
// Pet
// ==========================================================================

// pet_t::pet_t =============================================================

void pet_t::init_pet_t_()
{
  target = owner -> target;
  level = owner -> level;
  full_name_str = owner -> name_str + '_' + name_str;
  expiration = 0;

  pet_t** last = &( owner -> pet_list );
  while ( *last ) last = &( ( *last ) -> next_pet );
  *last = this;
  next_pet = 0;

  endurance_per_owner = 0.75;

  party = owner -> party;

  // Inherit owner's dbc state
  ptr = owner -> ptr;
}
pet_t::pet_t( sim_t*             s,
              player_t*          o,
              const std::string& n,
              bool               g ) :
              player_t( s, g ? PLAYER_GUARDIAN : PLAYER_PET, n ), owner( o ), next_pet( 0 ), summoned( false ), pet_type( PET_NONE )

{
  init_pet_t_();
}

pet_t::pet_t( sim_t*             s,
              player_t*          o,
              const std::string& n,
              pet_type_t         pt,
              player_type        type) :
  player_t( s, type, n ), owner( o ), next_pet( 0 ), summoned( false ), pet_type( pt )
{
  init_pet_t_();
}

// pet_t::endurance =========================================================

double pet_t::endurance() const
{
  double a = composite_attribute_multiplier( ATTR_ENDURANCE ) * ( endurance_per_owner * owner -> endurance() );

  return player_t::endurance() + a;
}

// player_t::id =============================================================

const char* pet_t::id()
{
  if ( id_str.empty() )
  {
    // create artifical unit ID, format type+subtype+id= TTTSSSSSSSIIIIII
    id_str = ( boost::format( "0xF140601FC5%06X,\"%s\",0x1111" ) % index % name_str ).str();
  }

  return id_str.c_str();
}

// pet_t::init_base =========================================================

void pet_t::init_base()
{}

// pet_t::init_target =======================================================

void pet_t::init_target()
{
  if ( ! target_str.empty() )
    player_t::init_target();
  else
    target = owner -> target;
}

void pet_t::init_talents()
{
  specialization = primary_tab();
}
// pet_t::reset =============================================================

void pet_t::reset()
{
  player_t::reset();
  expiration = 0;
}

// pet_t::summon ============================================================

void pet_t::summon( timespan_t duration )
{
  if ( sim -> log )
  {
    log_t::output( sim, "%s summons %s. for %.2fs", owner -> name(), name(),
                   to_seconds( duration ) );
  }

  distance = owner -> distance;

  owner -> active_pets++;

  summoned = true;

  // Take care of remaining expiration
  if ( expiration )
  {
    event_t::cancel( expiration );
    expiration = 0;
  }

  if ( duration > timespan_t::zero() )
  {
    struct expiration_t : public event_t
    {
      expiration_t( sim_t* sim, pet_t* p, timespan_t duration ) : event_t( sim, p )
      {
        sim -> add_event( this, duration );
      }

      virtual void execute()
      {
        player -> cast_pet() -> expiration = 0;
        if ( ! player -> sleeping ) player -> cast_pet() -> dismiss();
      }
    };
    expiration = new ( sim ) expiration_t( sim, this, duration );
  }

  arise();
}

// pet_t::dismiss ===========================================================

void pet_t::dismiss()
{
  if ( sim -> log ) log_t::output( sim, "%s dismisses %s", owner -> name(), name() );

  owner -> active_pets--;

  if ( expiration )
  {
    event_t::cancel( expiration );
    expiration = 0;
  }

  demise();
}

// pet_t::assess_damage =====================================================

double pet_t::assess_damage( double            amount,
                             const school_type school,
                             int               dmg_type,
                             int               result,
                             action_t*         action )
{
  if ( ! action || action -> aoe )
    amount *= 0.10;

  return player_t::assess_damage( amount, school, dmg_type, result, action );
}

// pet_t::combat_begin ======================================================

void pet_t::combat_begin()
{
  // By default, only report statistics in the context of the owner
  quiet = ! sim -> report_pets_separately;

  player_t::combat_begin();
}

// companion_t::companion_t


companion_t::companion_t( sim_t*             s,
                          player_t*          o,
                          const std::string& n,
                          pet_type_t         pt ) :
  pet_t( s, o, n, pt, PLAYER_COMPANION )
{
}

void companion_t::summon( timespan_t duration )
{
  owner -> active_companion = this;
  pet_t::summon( duration );
}

void companion_t::dismiss()
{
  pet_t::dismiss();
  owner -> active_companion = 0;
}
