// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

namespace mrrobot { // ======================================================

namespace { // ANONYMOUS ====================================================

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

const bool DEBUG_ITEMS = false;

// parse_skills =============================================================

bool parse_skills( player_t* p, js_node_t* profile )
{
  std::string skill_string;
  if ( !js_t::get_value( skill_string, profile, "SkillString" ) )
  {
    p -> sim -> errorf( "Player '%s' has no skills.\n", p -> name() );
    return false;
  }

  std::vector<std::string> tree_strings;
  util_t::string_split( tree_strings, skill_string, "-" );

  for ( unsigned tree = 0; tree < MAX_TALENT_TREES && tree < tree_strings.size(); ++tree )
  {
    for ( unsigned i = 0; i < tree_strings[ tree ].length() && i < p -> talent_trees[ tree ].size(); ++i )
    {
      signed char c = tree_strings[ tree ][ i ] - '0';

      if ( unlikely( c < 0 || c > 5 ) )
      {
        // FIXME: Report something.
        continue;
      }

      p -> talent_trees[ tree ][ i ] -> set_rank( c );
    }
  }

  return true;
}

slot_type translate_slot_name( const std::string& name )
{
  static const char* const slot_map[] =
  {
    "head",
    "ear",
    0,
    0,
    "chest",
    "waist",
    "legs",
    "feet",
    "wrist",
    "hands",
    "implant1",
    "implant2",
    "relic1",
    "relic2",
    0,
    "mainhand",
    "offhand",
  };

  for ( unsigned i = 0; i < sizeof_array( slot_map ); ++i )
  {
    if ( slot_map[ i ] && util_t::str_compare_ci( name, slot_map[ i ] ) )
      return static_cast<slot_type>( i );
  }

  return SLOT_NONE;
}

// parse_items ==============================================================

bool parse_items( player_t* p, js_node_t* items )
{
  if ( !items ) return true;

  std::vector<js_node_t*> nodes;
  js_t::get_children( nodes, items );

  for ( unsigned i = 0; i < nodes.size(); ++i )
  {
    std::stringstream item_encoding;
    js_node_t* item = nodes[ i ];

    std::string slot_name;
    if ( ! js_t::get_value( slot_name, item, "CharacterSlot" ) )
    {
      // FIXME: Report weirdness.
      continue;
    }

    slot_type slot = translate_slot_name( slot_name );
    if ( slot == SLOT_NONE )
    {
      // FIXME: Report weirdness.
      continue;
    }

    std::string name;
    if ( ! js_t::get_value( name, item, "Name" ) )
    {
      // FIXME: Report weirdness.
      continue;
    }
    armory_t::format( name );
    item_encoding << name;

    std::string weapon_type_str;
    if ( js_t::get_value( weapon_type_str, item, "WeaponType" ) )
    {
      // FIXME: Do something.
    }

    js_node_t* stats = js_t::get_child( item, "Stats" );
    if ( ! stats )
    {
      // FIXME: Report weirdness.
      continue;
    }

    std::vector<js_node_t*> stat_nodes;
    js_t::get_children( stat_nodes, stats );

    item_encoding << ",stats=";

    for ( size_t j = 0; j < stat_nodes.size(); ++j )
    {
      std::string stat_name;
      int stat_value;
      if ( ! js_t::get_value( stat_name, stat_nodes[ j ], "Stat" ) ||
           ! js_t::get_value( stat_value, stat_nodes[ j ], "Value" ) )
      {
        // FIXME: Report weirdness.
        continue;
      }

      if ( j )
        item_encoding << '_';
      item_encoding << stat_value << stat_name;
    }

    p -> items[ slot ].options_str = item_encoding.str();
  }

  return true;
}

#if 0
// parse_profession =========================================================

void parse_profession( std::string& professions_str, js_node_t* profile, int index )
{
  std::string key = "professions/primary/" + util_t::to_string( index );
  if ( js_node_t* profession = js_t::get_node( profile, key ) )
  {
    int id;
    std::string rank;
    if ( js_t::get_value( id, profession, "id" ) && js_t::get_value( rank, profession, "rank" ) )
    {
      if ( professions_str.length() > 0 )
        professions_str += '/';
      professions_str += util_t::profession_type_string( util_t::translate_profession_id( id ) );
      professions_str += '=' + rank;
    }
  }
}

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

  js_node_t* js = download_id( item.sim, item.sim -> default_region_str, item_id, caching );
  if ( ! js )
  {
    if ( caching != cache::ONLY )
    {
      item.sim -> errorf( "BCP API: Player '%s' unable to download item id '%s' at slot %s.\n",
                          item.player -> name(), item_id.c_str(), item.slot_name() );
    }
    return false;
  }
  if ( item.sim -> debug ) js_t::print( js, item.sim -> output_file );

  try
  {
    {
      int id;
      if ( ! js_t::get_value( id, js, "id" ) ) throw( "id" );
      item_data.id = id;
    }

    if ( ! js_t::get_value( item_data.name_str, js, "name" ) ) throw( "name" );
    item_data.name = item_data.name_str.c_str();

    if ( js_t::get_value( item_data.icon_str, js, "icon" ) )
      item_data.icon = item_data.icon_str.c_str();

    if ( ! js_t::get_value( item_data.level, js, "itemLevel" ) ) throw( "level" );

    js_t::get_value( item_data.req_level, js, "requiredLevel" );
    js_t::get_value( item_data.req_skill, js, "requiredSkill" );
    js_t::get_value( item_data.req_skill_level, js, "requiredSkillRank" );

    if ( ! js_t::get_value( item_data.quality, js, "quality" ) ) throw( "quality" );

    if ( ! js_t::get_value( item_data.inventory_type, js, "inventoryType" ) ) throw( "inventory type" );
    if ( ! js_t::get_value( item_data.item_class, js, "itemClass" ) ) throw( "item class" );
    if ( ! js_t::get_value( item_data.item_subclass, js, "itemSubClass" ) ) throw( "item subclass" );
    js_t::get_value( item_data.bind_type, js, "itemBind" );

    if ( js_node_t* w = js_t::get_child( js, "weaponInfo" ) )
    {
      int minDmg, maxDmg;
      double speed;
      if ( ! js_t::get_value( speed, w, "weaponSpeed" ) ) throw( "weapon speed" );
      if ( ! js_t::get_value( minDmg, w, "damage/minDamage" ) ) throw( "weapon minimum damage" );
      if ( ! js_t::get_value( maxDmg, w, "damage/maxDamage" ) ) throw( "weapon maximum damage" );
      item_data.delay = speed * 1000.0;
      item_data.dmg_range = maxDmg - minDmg;
    }

    if ( js_node_t* classes = js_t::get_child( js, "allowableClasses" ) )
    {
      std::vector<js_node_t*> nodes;
      js_t::get_children( nodes, classes );
      for ( size_t i = 0, n = nodes.size(); i < n; ++i )
      {
        int cid;
        if ( js_t::get_value( cid, nodes[ i ] ) )
          item_data.class_mask |= 1 << ( cid - 1 );
      }
    }
    else
      item_data.class_mask = -1;

    if ( js_node_t* races = js_t::get_child( js, "allowableRaces" ) )
    {
      std::vector<js_node_t*> nodes;
      js_t::get_children( nodes, races );
      for ( size_t i = 0, n = nodes.size(); i < n; ++i )
      {
        int rid;
        if ( js_t::get_value( rid, nodes[ i ] ) )
          item_data.race_mask |= 1 << ( rid - 1 );
      }
    }
    else
      item_data.race_mask = -1;

    if ( js_node_t* stats = js_t::get_child( js, "bonusStats" ) )
    {
      std::vector<js_node_t*> nodes;
      js_t::get_children( nodes, stats );
      for ( size_t i = 0, n = std::min( nodes.size(), sizeof_array( item_data.stat_type ) ); i < n; ++i )
      {
        if ( ! js_t::get_value( item_data.stat_type[ i ], nodes[ i ], "stat" ) ) throw( "bonus stat" );
        if ( ! js_t::get_value( item_data.stat_val[ i ], nodes[ i ], "amount" ) ) throw( "bonus stat amount" );
      }
    }

    if ( js_node_t* sockets = js_t::get_node( js, "socketInfo/sockets" ) )
    {
      std::vector<js_node_t*> nodes;
      js_t::get_children( nodes, sockets );
      for ( size_t i = 0, n = std::min( nodes.size(), sizeof_array( item_data.socket_color ) ); i < n; ++i )
      {
        std::string color;
        if ( js_t::get_value( color, nodes[ i ], "type" ) )
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

    js_t::get_value( item_data.id_set, js, "itemSet" );

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

js_node_t* download_roster( sim_t* sim,
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
  js_node_t* js = js_t::create( sim, result );
  if ( ! js || ! ( js = js_t::get_child( js, "members" ) ) )
  {
    sim -> errorf( "BCP API: Unable to get members of guild %s|%s|%s.\n", region.c_str(), server.c_str(), name.c_str() );
    return 0;
  }

  return js;
}
#endif // 0

void canonical_class_name( std::string& name )
{
  armory_t::format( name );
  name = util_t::player_type_string( util_t::translate_class_str( name ) );
}

void canonical_race_name( std::string& name )
{
  armory_t::format( name );
  name = util_t::race_type_string( util_t::parse_race_type( name ) );
}
} // ANONYMOUS namespace ====================================================

// mrrobot::download_player =================================================

player_t* download_player( sim_t*             sim,
                           const std::string& id,
                           cache::behavior_t  caching )
{
  // Check form validity of the provided profile id before even starting to access the profile
  if ( id.empty() ) return 0;

  sim -> current_name = id;
  sim -> current_slot = 0;

  std::string url = "http://swtor.askmrrobot.com/api/character/" + id;
  std::string result;
  if ( id == "test" )
  {
    result =
        "{\n"
        "        \"ProfileId\":\"9674b96a-d94a-47c8-b268-17e65c432915\",\n"
        "        \"ProfileName\":\"Yellowsix of Juyo (US)\",\n"
        "        \"LastUpdated\":\"2012-03-05T21:27:35\",\n"
        "        \"Region\":\"US\",\n"
        "        \"Server\":\"Juyo\",\n"
        "        \"Name\":\"Yellowsix\",\n"
        "        \"Guild\":\"GH-AMR\",\n"
        "        \"Faction\":\"Empire\",\n"
        "        \"AdvancedClass\":\"Sorcerer\",\n"
        "        \"Level\":45,\n"
        "        \"Gender\":\"Female\",\n"
        "        \"Race\":\"Chiss\",\n"
        "        \"Alignment\":\"Neutral\",\n"
        "        \"SocialLevel\":\"None\",\n"
        "        \"ValorRank\":0,\n"
        "        \"CraftingCrewSkill\":\"None\",\n"
        "        \"CrewSkill2\":\"None\",\n"
        "        \"CrewSkill3\":\"None\",\n"
        "        \"SkillString\":\"0000000000000000000-00000000000000000000-000000000000000000\",\n"
        "        \"Gear\":[\n"
        "          {\n"
        "            \"CharacterSlot\":\"Chest\",\n"
        "            \"Id\":\"30045\",\n"
        "            \"VariantId\":0,\n"
        "            \"Mods\":[\n"
        "              {\n"
        "                \"Slot\":\"Mod\",\n"
        "                \"Id\":\"5890\",\n"
        "                \"Name\":\"Advanced Deflecting Mod 25B\",\n"
        "                \"Stats\":[\n"
        "                  {\"Stat\":\"Aim\",\"Value\":61},\n"
        "                  {\"Stat\":\"Endurance\",\"Value\":37},\n"
        "                  {\"Stat\":\"Defense\",\"Value\":11}\n"
        "                ]\n"
        "              },\n"
        "              {\n"
        "                \"Slot\":\"Enhancement\",\n"
        "                \"Id\":\"9207\",\n"
        "                \"Name\":\"Advanced Astute Enhancement 25\",\n"
        "                \"Stats\":[\n"
        "                  {\"Stat\":\"Endurance\",\"Value\":40},\n"
        "                  {\"Stat\":\"Accuracy\",\"Value\":51},\n"
        "                  {\"Stat\":\"Defense\",\"Value\":20}\n"
        "                ]\n"
        "              }\n"
        "            ],\n"
        "            \"Name\":\"Battlemaster Supercommando's Body Armor\",\n"
        "            \"ArmorType\":\"Heavy\",\n"
        "            \"WeaponType\":null,\n"
        "            \"ShieldType\":null,\n"
        "            \"Stats\":[\n"
        "             {\"Stat\":\"Aim\",\"Value\":94},\n"
        "              {\"Stat\":\"Endurance\",\"Value\":108},\n"
        "              {\"Stat\":\"Expertise\",\"Value\":50},\n"
        "              {\"Stat\":\"Accuracy\",\"Value\":51},\n"
        "              {\"Stat\":\"Defense\",\"Value\":31}\n"
        "            ],\n"
        "            \"ItemSetId\":null\n"
        "          }\n"
        "        ],\n"
        "        \"Datacrons\":[]\n"
        "      }";
  }

  else if ( ! http_t::get( result, url, caching ) )
  {
    sim -> errorf( "Unable to download player from '%s'\n", url.c_str() );
    return 0;
  }

  // if ( sim -> debug ) util_t::fprintf( sim -> output_file, "%s\n%s\n", url.c_str(), result.c_str() );
  js_node_t* profile = js_t::create( sim, result );
  if ( ! profile )
  {
    sim -> errorf( "Unable to parse player profile from '%s'\n", url.c_str() );
    return 0;
  }
  if ( sim -> debug ) js_t::print( profile, sim -> output_file );

  std::string name;
  if ( ! js_t::get_value( name, profile, "Name"  ) )
  {
    sim -> errorf( "Unable to extract player name from '%s'.\n", url.c_str() );
    return 0;
  }
  if ( ! name.empty() )
    sim -> current_name = name;

  int level;
  if ( ! js_t::get_value( level, profile, "Level"  ) )
  {
    sim -> errorf( "Unable to extract player level from '%s'.\n", url.c_str() );
    return 0;
  }

  std::string class_name;
  if ( ! js_t::get_value( class_name, profile, "AdvancedClass" ) )
  {
    sim -> errorf( "Unable to extract player class from '%s'.\n", url.c_str() );
    return 0;
  }
  canonical_class_name( class_name );

  std::string race_name;
  if ( ! js_t::get_value( race_name, profile, "Race" ) )
  {
    sim -> errorf( "Unable to extract player race from '%s'.\n", url.c_str() );
    return 0;
  }
  race_type race = util_t::parse_race_type( race_name );

  player_t* p = player_t::create( sim, class_name, name, race );
  sim -> active_player = p;
  if ( ! p )
  {
    sim -> errorf( "Unable to build player with class '%s' and name '%s' from '%s'.\n",
                   class_name.c_str(), name.c_str(), url.c_str() );
    return 0;
  }

  p -> level = level;

  js_t::get_value( p -> server_str, profile, "Server" );
  js_t::get_value( p -> region_str, profile, "Region" );

  p -> origin_str = url;

  std::string crafting_skill;
  if ( js_t::get_value( crafting_skill, profile, "CraftingCrewSkill" ) )
  {
    // FIXME: Do something.
    ;
  }

  if ( ! parse_skills( p, profile ) )
    return 0;

  if ( ! parse_items( p, js_t::get_child( profile, "Gear" ) ) )
    return 0;

  if ( js_node_t* datacrons = js_t::get_child( profile, "Datacrons" ) )
  {
    std::vector<js_node_t*> datacron_list;
    js_t::get_children( datacron_list, datacrons );

    for ( size_t i = 0; i < datacron_list.size(); ++i )
    {
      // FIXME: Do something.
    }
  }

  return p;
}

// mrrobot::parse_talents ===================================================

bool parse_talents( player_t& p, const std::string& talent_string )
{
  // format: [tree_1]-[tree_2]-[tree_3] where each tree is a
  // sum over all talents of [# of points] * 6 ^ [0-based talent index]
  // in base 36.

  int encoding[ MAX_TALENT_SLOTS ];
  fill( encoding, 0 );

  std::vector<std::string> tree_strings;
  util_t::string_split( tree_strings, talent_string, "-" );

  for ( unsigned tree=0; tree < MAX_TALENT_TREES && tree < tree_strings.size(); ++tree )
  {
    unsigned count = 0;
    for ( unsigned j=0; j < tree; j++ )
      count += p.talent_trees[ j ].size();
    unsigned tree_size = p.talent_trees[ tree ].size();
    std::string::size_type pos = tree_strings[ tree ].length();
    unsigned tree_count = 0;
    while ( pos-- > 0 )
    {
      try
      {
        base36_t::pair_t point_pair = decoder( tree_strings[ tree ][ pos ] );
        if ( ++tree_count >= tree_size )
          break;
        encoding[ count++ ] = point_pair.second;
        if ( ++tree_count >= tree_size )
          break;
        encoding[ count++ ] = point_pair.first;
      }

      catch ( base36_t::bad_char bc )
      {
        p.sim -> errorf( "Player %s has malformed wowhead talent string. Translation for '%c' unknown.\n",
                         p.name(), bc.c );
        return false;
      }
    }
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
  js_node_t* js = download_roster( sim, region, server, name, caching );
  if ( !js ) return false;

  std::vector<std::string> names;
  std::vector<js_node_t*> characters;
  js_t::get_children( characters, js );

  for ( std::size_t i = 0, n = characters.size(); i < n; ++i )
  {
    int level;
    if ( ! js_t::get_value( level, characters[ i ], "character/level" ) || level < 85 )
      continue;

    int cid;
    if ( ! js_t::get_value( cid, characters[ i ], "character/class" ) ||
         ( player_filter != PLAYER_NONE && player_filter != util_t::translate_class_id( cid ) ) )
      continue;

    int rank;
    if ( ! js_t::get_value( rank, characters[ i ], "rank" ) ||
         ( ( max_rank > 0 ) && ( rank > max_rank ) ) ||
         ( ! ranks.empty() && range::find( ranks, rank ) == ranks.end() ) )
      continue;

    std::string cname;
    if ( ! js_t::get_value( cname, characters[ i ], "character/name" ) ) continue;
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
