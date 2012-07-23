#ifndef WARR_KNIGHT_HPP
#define WARR_KNIGHT_HPP

#include "../simulationcraft.hpp"

namespace warr_knight {

class class_t : public player_t
{
  public:
    class_t( sim_t* sim, player_type pt, const std::string& name, race_type rt ) :
      player_t( sim, pt, name, rt )
    {
      primary_attribute   = ATTR_STRENGTH;
      secondary_attribute = ATTR_WILLPOWER;
    }

  virtual resource_type primary_resource() const
  { return RESOURCE_RAGE; }

  virtual void init_base()
  {
    player_t::init_base();

    distance = default_distance = 3;

    std::cout << "base rage 12\n";
    resource_base[ RESOURCE_RAGE ] = 12;
  }

};

class targetdata_t : public ::targetdata_t
{
  public:
    targetdata_t( class_t& source, player_t& target );
};

// put this in sc_warr_knight.cpp
targetdata_t::targetdata_t( class_t& source, player_t& target ) :
  ::targetdata_t( source, target )
{}
// ^^^

class action_t : public ::action_t
{
  typedef ::action_t base_t;
public:
  action_t( const std::string& n, class_t* player,
            attack_policy_t policy, resource_type r, school_type s ) :
  base_t( ACTION_ATTACK, n, player, policy, r, s )
  {
    harmful = false;
  }

  targetdata_t* targetdata() const { return static_cast<targetdata_t*>( base_t::targetdata() ); }
  class_t* p() const { return static_cast<class_t*>( player ); }
  class_t* cast() const { return p(); }
};

}


#endif // WARR_KNIGHT_HPP
