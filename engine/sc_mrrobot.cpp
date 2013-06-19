// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"
#include <boost/uuid/string_generator.hpp>

namespace mrrobot { // ======================================================

namespace { // ANONYMOUS ====================================================

const bool USE_TEST_API = false;

// Encoding used by askmrrobot's talent builder.
const base36_t::encoding_t talent_encoding =
{
  '0', '1', '2', '3', '4', '5',
  '6', '7', '8', '9', 'a', 'b',
  'c', 'd', 'e', 'f', 'g', 'h',
  'i', 'j', 'k', 'l', 'm', 'n',
  'o', 'p', 'q', 'r', 's', 't',
  'u', 'v', 'w', 'x', 'y', 'z',
};

const base36_t decoder( talent_encoding );


inline char encode_pair( int first, int second=0 )
{
  assert( 0 <= first  && first <= 5 );
  assert( 0 <= second && second <= 5 );
  return talent_encoding[ 6 * first + second ];
}

std::string encode_tree( const std::vector<talent_t*>& tree )
{
  std::string result;

  bool first_talent_reached = false;
  int i = tree.size() - 1;
  while( i >= 0 )
  {
    int first = 0;
    if ( tree[ i ] )
      first = tree[ i ] -> rank();
    --i;

    if ( first || first_talent_reached )
    {
      first_talent_reached = true;
      result += first + '0';
    }
  }

  return result.size() > 0 ? result : "0";
}
void parse_profession( js::node_t* profile,
                       const std::string& path,
                       std::string& player_profession_string )
{
  std::string crafting_skill;
  if ( profile -> get( path, crafting_skill ) )
  {
    util_t::format_name( crafting_skill );
    if ( ! player_profession_string.empty() )
      player_profession_string += '/';
    player_profession_string += crafting_skill;
  }
}
// parse_skills =============================================================

void parse_skill_tree( std::vector<talent_t*>& tree, std::string& s )
{
  for ( size_t i = 0; i < s.size() && i < tree.size(); ++i )
  {
    signed char c = s[ s.size() - i - 1 ] - '0';

    if ( unlikely( c < 0 || c > 5 ) )
    {
      // FIXME: Report something.
      continue;
    }

    tree[ i ] -> set_rank( c );
  }
}

void parse_skills( player_t* p, js::node_t* profile )
{
  std::string skill_string;
  if ( !profile -> get( "SkillString", skill_string ) )
    return;

  auto tree_strings = split( skill_string, '-' );
  size_t max = std::min( static_cast<size_t>( MAX_TALENT_TREES ),
                         tree_strings.size() );

  for ( size_t tree = 0; tree < max; ++tree )
    parse_skill_tree( p -> talent_trees[ tree ], tree_strings[ tree ] );
}

slot_type translate_slot_name( const std::string& name )
{
  static const struct {
    const char* name;
    slot_type slot;
  } slot_map[] = {
    { "Helm",     SLOT_HEAD },
    { "Ears",     SLOT_EAR },
    { "Chest",    SLOT_CHEST },
    { "Belt",     SLOT_WAIST },
    { "Leg",      SLOT_LEGS },
    { "Feet",     SLOT_FEET },
    { "Wrist",    SLOT_WRISTS },
    { "Glove",    SLOT_HANDS },
    { "Implant1", SLOT_IMPLANT_1 },
    { "Implant2", SLOT_IMPLANT_2 },
    { "Relic1",   SLOT_RELIC_1 },
    { "Relic2",   SLOT_RELIC_2 },
    { "MainHand", SLOT_MAIN_HAND },
    { "OffHand",  SLOT_OFF_HAND  },
  };

  for ( auto const& i : slot_map )
    if ( util_t::str_compare_ci( name, i.name ) )
      return i.slot;

  return SLOT_INVALID;
}

// decode_weapon_type =======================================================

weapon_type decode_weapon_type( const std::string& s )
{
  static const struct {
    const char* name;
    weapon_type type;
  } weapon_map[] = {
    { "LightSaber",    WEAPON_LIGHTSABER },
    { "Pistol",        WEAPON_BLASTER_PISTOL },
    { "PoleSaber",     WEAPON_DOUBLE_BLADED_LIGHTSABER },
    { "BlasterRifle",  WEAPON_BLASTER_RIFLE },
    { "VibroKnife",    WEAPON_VIBROKNIFE },
    { "VibroSword",    WEAPON_VIBROSWORD },
    { "TrainingSaber", WEAPON_TRAININGSABER },
    { "ScatterGun",    WEAPON_SCATTERGUN },
    { "AssaultCannon", WEAPON_ASSAULT_CANNON },
    { "SniperRifle",   WEAPON_SNIPER_RIFLE },
    { "ElectroStaff",  WEAPON_ELECTROSTAFF },
    { "TechBlade",     WEAPON_TECHBLADE },
    { "TechStaff",     WEAPON_TECHSTAFF },
  };

  for ( auto const& i : weapon_map )
    if ( util_t::str_compare_ci( s, i.name ) )
      return i.type;

  return WEAPON_NONE;
}

// decode_setbonus =============================================================

std::string decode_armoring( js::node_t* node )
{
  std::string value;
  std::string armor_name;

  for ( js::node_t* mod : *node )
    {
      // It's an Armoring if there is no slot defined of there is a slot defined as 'Armoring'.
      // Depends if you are using the test API (defined) or not (no slot).
      // So it's NOT an armoring if there is a slot defined that's not set to 'Armoring'.
      if (( mod -> get ( "Slot" , value ) ) && ( value != "Armoring" ))  //
        continue;
      mod -> get ( "Name", armor_name );
    }

  return util_t::format_name( armor_name );
}

// decode_stats =============================================================

std::string decode_stats( js::node_t* node )
{
  static const struct {
    const char* name;
    const char* abbrv;
  } stat_mapping[] = {
    { "Armor",      "armor" },
#if 0
    // These are handled specially for weapons in Simc.
    { "MinDamage",  "min" },
    { "MaxDamage",  "max" },
#endif
    { "Endurance",  "endurance" },
    { "Strength",   "strength" },
    { "Aim",        "aim" },
    { "Cunning",    "cunning" },
    { "Will",       "willpower" },
    { "Presence",   "presence" },
    { "Expertise",  "expertise" },
    { "Power",      "power" },
    { "ForcePower", "forcepower" },
    { "TechPower",  "techpower" },
    { "Defense",    "def" },
    { "Shielding",  "shield" },
    { "Absorb",     "abs" },
    { "Accuracy",   "accuracy" },
    { "Crit",       "crit" },
    { "Surge",      "surge" },
    { "Alacrity",   "alacrity" },
  };

  std::stringstream ss;
  bool first = true;

  for ( auto const& i : stat_mapping )
  {
    int value;
    if ( ! node -> get( i.name, value ) )
      continue;
    if ( first )
      first = false;
    else
      ss << '_';
    ss << value << i.abbrv;
  }

  return ss.str();
}

// parse_items ==============================================================

void parse_items( player_t* p, js::node_t* items )
{
  if ( !items ) return;

  for ( js::node_t* item : *items )
  {
    std::stringstream item_encoding;

    std::string slot_name = item ->name();
    slot_type slot = translate_slot_name( slot_name );
    if ( slot == SLOT_INVALID )
    {
      // FIXME: Report weirdness.
      continue;
    }

    std::string name;
    if ( ! item -> get( "Name", name ) )
    {
      // FIXME: Report weirdness.
      continue;
    }
    item_encoding << util_t::format_name( name );

#if 0
    int id;
    if ( item -> get( "Id", level ) )
      item_encoding << ",id=" << id;
#endif

    int level;
    if ( item -> get( "ItemLevel", level ) )
      item_encoding << ",ilevel=" << level;

    std::string quality;
    if ( item -> get( "Quality", quality ) )
      item_encoding << ",quality=" << util_t::format_name( quality );

    if ( slot == SLOT_MAIN_HAND || slot == SLOT_OFF_HAND )
    {
      std::string weapon_type_str;
      weapon_type wt = WEAPON_NONE;
      if ( item -> get( "WeaponType", weapon_type_str ) )
      {
        wt = decode_weapon_type( weapon_type_str );
        if ( wt != WEAPON_NONE )
        {
          item_encoding << ",weapon=" << util_t::weapon_type_string( wt );
          int value;
          js::node_t* dmg = item -> find( "Stats/MinDamage" );
          if ( dmg && dmg -> get( value ) )
            item_encoding << '_' << value << "min";
          dmg = item -> find( "Stats/MaxDamage" );
          if ( dmg && dmg -> get( value ) )
            item_encoding << '_' << value << "max";
        }
      }
    }

    if ( js::node_t* stats = item -> get_child( "Stats" ) )
    {
      std::string s = decode_stats( stats );
      if ( ! s.empty() )
        item_encoding << ",stats=" << s;
    }


    if ( ! ( DEFAULT_SET_BONUS_SLOT_MASK != 0 &&
         ( DEFAULT_SET_BONUS_SLOT_MASK & bitmask( slot ) ) == 0 ) )
      {
        //we have a valid set bonus item

        if ( js::node_t* mods = item -> get_child( "Mods" ) )
          {
            std::string a = decode_armoring( mods );

            if ( ! a.empty() )
              {
                // have the armor name, now need to see if it matches a set bonus
                std::string s = p->get_armoring_set_bonus_name( a );

                if ( s.empty() )
                  {
                    //armoring doesn't have bonus, check the shell
                    s = p->get_shell_set_bonus_name( name );
                    if ( ! s.empty() )
                      item_encoding << ",setbonus=" << s;
                  }
                else
                  {
                    item_encoding << ",setbonus=" << s;
                  }
              }
          }
      }

    p -> items[ slot ].options_str = item_encoding.str();
  }
}

// parse_companion_bonuses ==================================================
void parse_companion_bonuses( player_t* p, js::node_t* profile )
{
  std::string has_human;
  profile -> get( "LegacyHasHuman", has_human );
  //std::cout << "has human" << has_human;
  //std::stringstream ss;
  if ( js::node_t* bonuses = profile -> get_child("LegacyCompanionBonuses") )
  {
    std::string bonus;

    for ( js::node_t* node : *bonuses )
    {
      node -> get ( bonus );
      if ( util_t::str_compare_ci ( "MeleeDamage", bonus ) )
      {
        p -> bonus_surge_pc_ = 1;
        //ss << "bonus_surge_pc=1\n";
      }
      else if ( util_t::str_compare_ci ( "RangedTank", bonus ) )
      {
        p -> bonus_health_pc_ = 1;
        //ss << "bonus_health_pc=1\n";
      }
      else if ( util_t::str_compare_ci ( "RangedDamage", bonus ) )
      {
        p -> bonus_crit_pc_ = 1;
        //ss << "bonus_crit_pc=1\n";
      }
      else if ( util_t::str_compare_ci ( "MeleeTank", bonus ) )
      {
        p -> bonus_accuracy_pc_ = 1;
        //ss << "bonus_accuracy_pc=1\n";
      }
      else if ( util_t::str_compare_ci ( "Healer", bonus ) )
      {
        //ss << "bonus_heal_pc=1\n";
      }
    }
  }
  //std::cout << ss.str() << std::endl;
}

// parse_datacrons ==========================================================

void parse_datacrons( player_t* p, js::node_t* datacrons )
{
  static const struct datacron_stats
  {
    int id;
    attribute_type stat;
    int amount;
  } all_datacrons[] = {
    { 0, ATTR_AIM, 4 },
    { 1, ATTR_PRESENCE, 3 },
    { 2, ATTR_WILLPOWER, 3 },
    { 3, ATTR_STRENGTH, 4 },
    { 4, ATTR_ENDURANCE, 3 },
    { 5, ATTR_STRENGTH, 2 },
    { 6, ATTR_AIM, 2 },
    { 7, ATTR_WILLPOWER, 2 },
    { 8, ATTR_CUNNING, 2 },
    { 10, ATTR_AIM, 4 },
    { 11, ATTR_CUNNING, 4 },
    { 12, ATTR_PRESENCE, 4 },
    { 13, ATTR_ENDURANCE, 3 },
    { 14, ATTR_WILLPOWER, 4 },
    { 15, ATTR_WILLPOWER, 4 },
    { 16, ATTR_ENDURANCE, 4 },
    { 17, ATTR_PRESENCE, 4 },
    { 18, ATTR_AIM, 4 },
    { 20, ATTR_STRENGTH, 4 },
    { 21, ATTR_WILLPOWER, 4 },
    { 23, ATTR_PRESENCE, 4 },
    { 24, ATTR_AIM, 4 },
    { 25, ATTR_CUNNING, 4 },
    { 27, ATTR_WILLPOWER, 4 },
    { 28, ATTR_PRESENCE, 2 },
    { 30, ATTR_STRENGTH, 2 },
    { 31, ATTR_ENDURANCE, 2 },
    { 32, ATTR_CUNNING, 2 },
    { 33, ATTR_STRENGTH, 2 },
    { 34, ATTR_PRESENCE, 2 },
    { 35, ATTR_CUNNING, 2 },
    { 37, ATTR_ENDURANCE, 2 },
    { 38, ATTR_PRESENCE, 4 },
    { 40, ATTR_ENDURANCE, 4 },
    { 41, ATTR_CUNNING, 4 },
    { 42, ATTR_STRENGTH, 4 },
    { 43, ATTR_AIM, 2 },
    { 45, ATTR_PRESENCE, 2 },
    { 46, ATTR_AIM, 4 },
    { 47, ATTR_ENDURANCE, 4 },
    { 49, ATTR_WILLPOWER, 4 },
    { 51, ATTR_WILLPOWER, 2 },
    { 52, ATTR_ENDURANCE, 2 },
    { 54, ATTR_AIM, 3 },
    { 55, ATTR_PRESENCE, 3 },
    { 56, ATTR_CUNNING, 3 },
    { 57, ATTR_STRENGTH, 3 },
    { 59, ATTR_AIM, 3 },
    { 60, ATTR_PRESENCE, 3 },
    { 61, ATTR_STRENGTH, 3 },
    { 63, ATTR_PRESENCE, 2 },
    { 64, ATTR_AIM, 2 },
    { 65, ATTR_CUNNING, 4 },
    { 66, ATTR_ENDURANCE, 4 },
    { 67, ATTR_STRENGTH, 4 },
    { 68, ATTR_STRENGTH, 2 },
    { 69, ATTR_PRESENCE, 4 },
    { 70, ATTR_AIM, 2 },
    { 71, ATTR_ENDURANCE, 3 },
    { 72, ATTR_CUNNING, 2 },
    { 73, ATTR_AIM, 4 },
    { 74, ATTR_WILLPOWER, 2 },
    { 76, ATTR_WILLPOWER, 4 },
    { 77, ATTR_CUNNING, 4 },
    { 78, ATTR_CUNNING, 3 },
    { 79, ATTR_WILLPOWER, 3 },
    { 80, ATTR_AIM, 3 },
    { 82, ATTR_STRENGTH, 3 },
    { 83, ATTR_CUNNING, 3 },
    { 84, ATTR_ENDURANCE, 2 },
    { 85, ATTR_WILLPOWER, 2 },
    { 87, ATTR_WILLPOWER, 4 },
    { 88, ATTR_PRESENCE, 4 },
    { 89, ATTR_ENDURANCE, 4 },
    { 90, ATTR_STRENGTH, 4 },
    { 91, ATTR_CUNNING, 4 },
    { 92, ATTRIBUTE_NONE, 10 },
    { 93, ATTRIBUTE_NONE, 10 },
  };

  if ( ! datacrons ) return;

  std::array<int,ATTRIBUTE_MAX> attributes;
  boost::fill( attributes, 0 );

  for ( js::node_t* node : *datacrons )
  {
    int id;
    if ( node -> get( id ) )
    {
      for ( auto const& d : all_datacrons )
      {
        if ( d.id == id )
        {
          if ( d.stat == ATTRIBUTE_NONE )
          {
            for ( attribute_type i = ATTRIBUTE_NONE; ++i < ATTRIBUTE_MAX; )
              attributes[i] += d.amount;
          }
          else
            attributes[ d.stat ] += d.amount;
          break;
        }
      }
    }
  }

  bool first = true;
  std::stringstream ss;
  ss << "datacrons,stats=";
  for ( attribute_type i = ATTRIBUTE_NONE; ++i < ATTRIBUTE_MAX; )
  {
    if ( ! attributes[ i ] ) continue;
    if ( first )
      first = false;
    else
      ss << '_';
    ss << attributes[ i ] << util_t::attribute_type_string( i );
  }

  if ( ! first )
    p -> items[ SLOT_SHIRT ].options_str = ss.str();
}

#if 0
struct item_info_t : public item_data_t
{
  std::string name_str, icon_str;
  item_info_t() : item_data_t() {}
};

bool download_item_data( item_t& item, item_info_t& item_data,
                         const std::string& item_id, cache::behavior_t caching )
{
  // BCP API doesn't currently provide enough information to describe items completely.
  if ( ! DEBUG_ITEMS )
    return false;

  js::node_t* js = download_id( item.sim, item.sim -> default_region_str, item_id, caching );
  if ( ! js )
  {
    if ( caching != cache::ONLY )
    {
      item.sim -> errorf( "BCP API: Player '%s' unable to download item id '%s' at slot %s.\n",
                          item.player -> name(), item_id.c_str(), item.slot_name() );
    }
    return false;
  }
  if ( item.sim -> debug ) js::print( js, item.sim -> output_file );

  try
  {
    {
      int id;
      if ( ! js -> get( "id", id ) ) throw( "id" );
      item_data.id = id;
    }

    if ( ! js -> get( "name", item_data.name_str ) ) throw( "name" );
    item_data.name = item_data.name_str.c_str();

    if ( js -> get( "icon", item_data.icon_str ) )
      item_data.icon = item_data.icon_str.c_str();

    if ( ! js -> get( "itemLevel", item_data.level ) ) throw( "level" );

    js -> get( "requiredLevel", item_data.req_level );
    js -> get( "requiredSkill", item_data.req_skill );
    js -> get( "requiredSkillRank", item_data.req_skill_level );

    if ( ! js -> get( "quality", item_data.quality ) ) throw( "quality" );

    if ( ! js -> get( "inventoryType", item_data.inventory_type ) ) throw( "inventory type" );
    if ( ! js -> get( "itemClass", item_data.item_class ) ) throw( "item class" );
    if ( ! js -> get( "itemSubClass", item_data.item_subclass ) ) throw( "item subclass" );
    js -> get( "itemBind", item_data.bind_type );

    if ( js::node_t* w = js::get_child( js, "weaponInfo" ) )
    {
      int minDmg, maxDmg;
      double speed;
      if ( ! w -> get( "weaponSpeed", speed ) ) throw( "weapon speed" );
      if ( ! w -> get( "damage/minDamage", minDmg ) ) throw( "weapon minimum damage" );
      if ( ! w -> get( "damage/maxDamage", maxDmg ) ) throw( "weapon maximum damage" );
      item_data.delay = speed * 1000.0;
      item_data.dmg_range = maxDmg - minDmg;
    }

    if ( js::node_t* classes = js::get_child( js, "allowableClasses" ) )
    {
      std::vector<js::node_t*> nodes;
      js::get_children( nodes, classes );
      for ( size_t i = 0, n = nodes.size(); i < n; ++i )
      {
        int cid;
        if ( js::get_value( cid, nodes[ i ] ) )
          item_data.class_mask |= 1 << ( cid - 1 );
      }
    }
    else
      item_data.class_mask = -1;

    if ( js::node_t* races = js::get_child( js, "allowableRaces" ) )
    {
      std::vector<js::node_t*> nodes;
      js::get_children( nodes, races );
      for ( size_t i = 0, n = nodes.size(); i < n; ++i )
      {
        int rid;
        if ( js::get_value( rid, nodes[ i ] ) )
          item_data.race_mask |= 1 << ( rid - 1 );
      }
    }
    else
      item_data.race_mask = -1;

    if ( js::node_t* stats = js::get_child( js, "bonusStats" ) )
    {
      std::vector<js::node_t*> nodes;
      js::get_children( nodes, stats );
      for ( size_t i = 0, n = std::min( nodes.size(), sizeof_array( item_data.stat_type ) ); i < n; ++i )
      {
        if ( ! nodes[ i ] -> get( "stat", item_data.stat_type[ i ] ) ) throw( "bonus stat" );
        if ( ! nodes[ i ] -> get( "amount", item_data.stat_val[ i ] ) ) throw( "bonus stat amount" );
      }
    }

    if ( js::node_t* sockets = js::get_node( js, "socketInfo/sockets" ) )
    {
      std::vector<js::node_t*> nodes;
      js::get_children( nodes, sockets );
      for ( size_t i = 0, n = std::min( nodes.size(), sizeof_array( item_data.socket_color ) ); i < n; ++i )
      {
        std::string color;
        if ( nodes[ i ] -> get( "type", color ) )
        {
          if ( color == "META" )
            item_data.socket_color[ i ] = SOCKET_COLOR_META;
          else if ( color == "RED" )
            item_data.socket_color[ i ] = SOCKET_COLOR_RED;
          else if ( color == "YELLOW" )
            item_data.socket_color[ i ] = SOCKET_COLOR_RED;
          else if ( color == "BLUE" )
            item_data.socket_color[ i ] = SOCKET_COLOR_BLUE;
          else if ( color == "PRISMATIC" )
            item_data.socket_color[ i ] = SOCKET_COLOR_PRISMATIC;
          else if ( color == "COGWHEEL" )
            item_data.socket_color[ i ] = SOCKET_COLOR_COGWHEEL;
        }
      }
    }

    js -> get( "itemSet", item_data.id_set );

    // heroic tag is not available from BCP API.
    {
      // FIXME: set item_data.flags_1 to ITEM_FLAG_HEROIC as appropriate.
    }

    // FIXME: LFR tag is not available from BCP API.

    // socket bonus is not available from BCP API.
    {
      // FIXME: set item_data.id_socket_bonus appropriately.
    }
  }
  catch ( const char* fieldname )
  {
    item.sim -> errorf( "BCP API: Player '%s' unable to parse item '%s' %s at slot '%s'.\n",
                        item.player -> name(), item_id.c_str(), fieldname, item.slot_name() );
    return false;
  }

  assert( 0 );
  return false;
}

// download_roster ==========================================================

js::node_t* download_roster( sim_t* sim,
                            const std::string& region,
                            const std::string& server,
                            const std::string& name,
                            cache::behavior_t  caching )
{
  std::string url = "http://" + region + ".battle.net/api/wow/guild/" + server + '/' +
                    name + "?fields=members";

  std::string result;
  if ( ! http_t::get( result, url, caching, "\"members\"" ) )
  {
    sim -> errorf( "BCP API: Unable to download guild %s|%s|%s.\n", region.c_str(), server.c_str(), name.c_str() );
    return 0;
  }
  js::node_t* js = js::create( sim, result );
  if ( ! js || ! ( js = js::get_child( js, "members" ) ) )
  {
    sim -> errorf( "BCP API: Unable to get members of guild %s|%s|%s.\n", region.c_str(), server.c_str(), name.c_str() );
    return 0;
  }

  return js;
}
#endif // 0

void canonical_class_name( std::string& name )
{
  util_t::format_name( name );
  name = util_t::player_type_string( util_t::translate_class_str( name ) );
}

void canonical_race_name( std::string& name )
{
  util_t::format_name( name );
  name = util_t::race_type_string( util_t::parse_race_type( name ) );
}
} // ANONYMOUS namespace ====================================================

// mrrobot::download_player =================================================

player_t* download_player( sim_t*             sim,
                           const std::string& key,
                           cache::behavior_t  caching )
{
  auto parts = split( key, '|' );
  auto id = strip_whitespace( parts[ 0 ] );

  // Check form validity of the provided profile id before even starting to access the profile
  try { boost::uuids::string_generator()( id ); }
  catch( std::runtime_error& )
  {
    sim -> errorf( "'%s' is not a valid Mr. Robot profile identifier.\n", id.c_str() );
    return nullptr;
  }

  if ( parts.size() > 1 )
  {
    player_type pt = util_t::translate_class_str( parts[ 1 ] );
    if ( pt == PLAYER_NONE )
    {
      sim -> errorf( "'%s' is not a valid advanced class.\n", parts[ 1 ].c_str() );
      return nullptr;
    }
  }

  sim -> current_name = id;
  sim -> current_slot = 0;

  std::string url = "http://swtor.askmrrobot.com/api/";
  if ( USE_TEST_API ) url += "test/";
  url += "character/v1/" + id;

  std::string result;

  if ( ! http_t::get( result, url, caching ) )
  {
    sim -> errorf( "Unable to download player from '%s'\n", url.c_str() );
    return nullptr;
  }

  // if ( sim -> debug ) util_t::fprintf( sim -> output_file, "%s\n%s\n", url.c_str(), result.c_str() );
  js::handle profile = js::create( sim, result );
  if ( ! profile )
  {
    sim -> errorf( "Unable to parse player profile from '%s'\n", url.c_str() );
    return nullptr;
  }
  if ( sim -> debug ) profile -> print( sim -> output_file );

  std::string name;
  if ( ! profile -> get( "Name", name ) &&
       ! profile -> get( "ProfileName", name ) )
  {
    sim -> errorf( "Unable to extract player name from '%s'.\n", url.c_str() );
    return nullptr;
  }
  if ( ! name.empty() )
    sim -> current_name = name;

  int level;
  if ( ! profile -> get( "Level", level ) )
  {
    sim -> errorf( "Unable to extract player level from '%s'.\n", url.c_str() );
    return nullptr;
  }

  std::string class_name;
  if ( parts.size() > 1 )
    class_name = parts[ 1 ];
  else if ( ! profile -> get( "AdvancedClass", class_name ) )
  {
    sim -> errorf( "Unable to extract player class from '%s'.\n", url.c_str() );
    return nullptr;
  }
  canonical_class_name( class_name );

  std::string race_name;
  race_type race = RACE_NONE;
  if ( profile -> get( "Race", race_name ) )
    race = util_t::parse_race_type( race_name );

  player_t* p = player_t::create( sim, class_name, name, race );
  sim -> active_player = p;
  if ( ! p )
  {
    sim -> errorf( "Unable to build player with class '%s' and name '%s' from '%s'.\n",
                   class_name.c_str(), name.c_str(), url.c_str() );
    return nullptr;
  }

  p -> level = level;

  profile -> get( "Server", p -> server_str );
  profile -> get( "Region", p -> region_str );

  p -> origin_str = "http://swtor.askmrrobot.com/character/" + id;

  parse_profession( profile, "CraftingCrewSkill", p -> professions_str );
  if ( false )
  {
    parse_profession( profile, "CrewSkill2",      p -> professions_str );
    parse_profession( profile, "CrewSkill3",      p -> professions_str );
  }

  parse_skills( p, profile );

  parse_items( p, profile -> get_child( "GearSet" ) );

  parse_datacrons( p, profile -> get_child( "Datacrons" ) );

  parse_companion_bonuses( p, profile );

  return p;
}

// mrrobot::parse_talents ===================================================

bool parse_talents( player_t& p, const std::string& talent_string )
{
  // format: [tree_1]-[tree_2]-[tree_3] where each tree is a
  // string of numbers which, from right to left, represent the
  // rank of each skill from bottom left to right and on up.

  int encoding[ MAX_TALENT_SLOTS ];
  boost::fill( encoding, 0 );

  auto tree_strings = split( talent_string, '-' );

  for ( size_t tree=0; tree < MAX_TALENT_TREES && tree < tree_strings.size(); ++tree )
  {
    size_t count = 0;
    for ( size_t j=0; j < tree; j++ )
      count += p.talent_trees[ j ].size();
    size_t tree_size = p.talent_trees[ tree ].size();
    std::string::size_type pos = tree_strings[ tree ].length();
    size_t tree_count = 0;
    while (( pos-- > 0 ) && ( tree_count++ < tree_size ))
      encoding[ count++ ] = tree_strings[ tree ][ pos ] - '0';
  }

  if ( p.sim -> debug )
  {
    std::string str_out;
    for ( size_t i = 0; i < MAX_TALENT_SLOTS; ++i )
      str_out += '0' + encoding[i];
    std::string::size_type pos = str_out.find_last_not_of( '0' );
    if ( pos != str_out.npos )
    {
      str_out.resize( pos );
      log_t::output( p.sim, "%s MrRobot talent string translation: %s\n", p.name(), str_out.c_str() );
    }
  }

  return p.parse_talent_trees( encoding );
}


std::string encode_talents( const player_t& p )
{
  std::string encoding;

  if ( const char* ac_code = util_t::player_type_string_short( p.type ) )
  {
    std::stringstream ss;

    ss << "http://swtor.askmrrobot.com/skills/" << ac_code << "#";

    // This is necessary because sometimes the talent trees change shape between live/ptr.
    for ( size_t i = 0; i < sizeof_array( p.talent_trees ); ++i )
    {
      if ( i > 0 ) ss << '-';
      ss << encode_tree( p.talent_trees[ i ] );
    }

    encoding = ss.str();
  }

  return encoding;
}

#if 0
// bcp_api::download_item() =================================================

bool download_item( item_t& item, const std::string& item_id, cache::behavior_t caching )
{
  // BCP API doesn't currently provide enough information to describe items completely.
  if ( ! DEBUG_ITEMS )
    return false;

  item_info_t item_data;
  return download_item_data( item, item_data, item_id, caching );
}

// bcp_api::download_slot() =================================================

bool download_slot( item_t& item,
                    const std::string& item_id,
                    const std::string& enchant_id,
                    const std::string& addon_id,
                    const std::string& reforge_id,
                    const std::string& rsuffix_id,
                    const std::string gem_ids[ 3 ],
                    cache::behavior_t caching )
{
  // BCP API doesn't currently provide enough information to describe items completely.
  if ( ! DEBUG_ITEMS )
    return false;

  item_info_t item_data;
  if ( ! download_item_data( item, item_data, item_id, caching ) )
    return false;

  if ( ! enchant_t::download_addon( item, addon_id ) )
  {
    item.sim -> errorf( "Player %s unable to parse addon id %s for item \"%s\" at slot %s.\n",
                        item.player -> name(), addon_id.c_str(), item.name(), item.slot_name() );
  }

  if ( ! enchant_t::download_reforge( item, reforge_id ) )
  {
    item.sim -> errorf( "Player %s unable to parse reforge id %s for item \"%s\" at slot %s.\n",
                        item.player -> name(), reforge_id.c_str(), item.name(), item.slot_name() );
  }

  if ( ! enchant_t::download_rsuffix( item, rsuffix_id ) )
  {
    item.sim -> errorf( "Player %s unable to determine random suffix '%s' for item '%s' at slot %s.\n",
                        item.player -> name(), rsuffix_id.c_str(), item.name(), item.slot_name() );
  }

  return true;
}

// mrrobot::download_guild ==================================================

bool download_guild( sim_t* sim, const std::string& region, const std::string& server, const std::string& name,
                     const std::vector<int>& ranks, int player_filter, int max_rank, cache::behavior_t caching )
{
  js::node_t* js = download_roster( sim, region, server, name, caching );
  if ( !js ) return false;

  std::vector<std::string> names;
  std::vector<js::node_t*> characters;
  js::get_children( characters, js );

  for ( std::size_t i = 0, n = characters.size(); i < n; ++i )
  {
    int level;
    if ( ! characters[ i ] -> get( "character/level", level ) || level < 85 )
      continue;

    int cid;
    if ( ! characters[ i ] -> get( "character/class", cid ) ||
         ( player_filter != PLAYER_NONE && player_filter != util_t::translate_class_id( cid ) ) )
      continue;

    int rank;
    if ( ! characters[ i ] -> get( "rank", rank ) ||
         ( ( max_rank > 0 ) && ( rank > max_rank ) ) ||
         ( ! ranks.empty() && range::find( ranks, rank ) == ranks.end() ) )
      continue;

    std::string cname;
    if ( ! characters[ i ] -> get( "character/name", cname ) ) continue;
    names.push_back( cname );
  }

  if ( names.empty() ) return true;

  range::sort( names );

  for ( std::size_t i = 0, n = names.size(); i < n; ++i )
  {
    const std::string& cname = names[ i ];
    util_t::printf( "Downloading character: %s\n", cname.c_str() );
    player_t* player = download_player( sim, region, server, cname, "active", caching );
    if ( !player )
    {
      sim -> errorf( "BCP API: Failed to download player '%s'\n", cname.c_str() );
      // Just ignore invalid players
    }
  }

  return true;
}
#endif // 0

} // namespace mrrobot ======================================================
