// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

namespace   // ANONYMOUS NAMESPACE ==========================================
{

struct token_t
{
  std::string full;
  std::string name;
  double value;
  std::string value_str;
};

// parse_tokens =============================================================

static int parse_tokens( std::vector<token_t>& tokens,
                         const std::string&    encoded_str )
{
  std::vector<std::string> splits;
  int num_splits = util_t::string_split( splits, encoded_str, "_" );

  tokens.resize( num_splits );
  for ( int i=0; i < num_splits; i++ )
  {
    token_t& t = tokens[ i ];
    t.full = splits[ i ];
    int index=0;
    while ( t.full[ index ] != '\0' &&
            t.full[ index ] != '%'  &&
            ! isalpha( t.full[ index ] ) ) index++;
    if ( index == 0 )
    {
      t.name = t.full;
      t.value = 0;
    }
    else
    {
      t.name = t.full.substr( index );
      t.value_str = t.full.substr( 0, index );
      t.value = atof( t.value_str.c_str() );
    }
  }

  return num_splits;
}

} // ANONYMOUS NAMESPACE ====================================================

// item_t::item_t ===========================================================

item_t::item_t( player_t* p, const std::string& o ) :
  sim( p -> sim ), player( p ), slot( SLOT_INVALID ), quality( QUALITY_NONE ),
  ilevel( 0 ), unique( false ), unique_enchant( false ), unique_addon( false ),
  is_heroic( false ), is_lfr( false ), is_ptr( p -> ptr ),
  options_str( o )
{}

// item_t::active ===========================================================

bool item_t::active() const
{
  if ( slot == SLOT_INVALID ) return false;
  if ( ! encoded_name_str.empty() ) return true;
  return false;
}

// item_t::heroic ===========================================================

bool item_t::heroic() const
{
  if ( slot == SLOT_INVALID ) return false;
  return is_heroic;
}

// item_t::lfr ==============================================================

bool item_t::lfr() const
{
  if ( slot == SLOT_INVALID ) return false;
  return is_lfr;
}

// item_t::ptr ==============================================================

bool item_t::ptr() const
{
  return is_ptr;
}

// item_t::name =============================================================

const char* item_t::name() const
{
  if ( ! encoded_name_str.empty() ) return encoded_name_str.c_str();
  if ( !  armory_name_str.empty() ) return  armory_name_str.c_str();
  return "inactive";
}

// item_t::weapon ===========================================================

weapon_t* item_t::weapon() const
{
  switch( slot )
  {
  case SLOT_MAIN_HAND: return &( player -> main_hand_weapon );
  case SLOT_OFF_HAND:  return &( player ->  off_hand_weapon );
  default:             return nullptr;
  }
}

// item_t::parse_options ====================================================

bool item_t::parse_options()
{
  if ( options_str.empty() ) return true;

  option_name_str = options_str;
  std::string remainder;

  std::string::size_type cut_pt = options_str.find( ',' );

  if ( cut_pt != options_str.npos )
  {
    remainder       = options_str.substr( cut_pt + 1 );
    option_name_str = options_str.substr( 0, cut_pt );
  }

  option_t options[] =
  {
    { "id",      OPT_STRING, &option_id_str            },
    { "stats",   OPT_STRING, &option_stats_str         },
    { "enchant", OPT_STRING, &option_enchant_str       },
    { "addon",   OPT_STRING, &option_addon_str         },
    { "equip",   OPT_STRING, &option_equip_str         },
    { "use",     OPT_STRING, &option_use_str           },
    { "weapon",  OPT_STRING, &option_weapon_str        },
    { "heroic",  OPT_STRING, &option_heroic_str        },
    { "lfr",     OPT_STRING, &option_lfr_str           },
    { "type",    OPT_STRING, &option_armor_type_str    },
    { "suffix",  OPT_STRING, &option_random_suffix_str },
    { "ilevel",  OPT_STRING, &option_ilevel_str        },
    { "quality", OPT_STRING, &option_quality_str       },
    { "source",  OPT_STRING, &option_data_source_str   },
    { NULL,      OPT_NONE,   NULL }
  };

  option_t::parse( sim, option_name_str, options, remainder );

  util_t::format_name( option_name_str );

  util_t::tolower( option_id_str            );
  util_t::tolower( option_stats_str         );
  util_t::tolower( option_enchant_str       );
  util_t::tolower( option_addon_str         );
  util_t::tolower( option_equip_str         );
  util_t::tolower( option_use_str           );
  util_t::tolower( option_weapon_str        );
  util_t::tolower( option_heroic_str        );
  util_t::tolower( option_lfr_str           );
  util_t::tolower( option_armor_type_str    );
  util_t::tolower( option_random_suffix_str );
  util_t::tolower( option_ilevel_str        );
  util_t::tolower( option_quality_str       );

  return true;
}

// item_t::encode_options ===================================================

void item_t::encode_options()
{
  // Re-build options_str for use in saved profiles

  std::string& o = options_str;

  o = encoded_name_str;

  if ( heroic() )                            { o += ",heroic=1";                                 }
  if ( lfr() )                               { o += ",lfr=1";                                    }
  if ( ! encoded_armor_type_str.empty() )    { o += ",type=";    o += encoded_armor_type_str;    }
  if ( ! encoded_ilevel_str.empty()        ) { o += ",ilevel=";  o += encoded_ilevel_str;        }
  if ( ! encoded_quality_str.empty()       ) { o += ",quality="; o += encoded_quality_str;       }
  if ( ! encoded_stats_str.empty()         ) { o += ",stats=";   o += encoded_stats_str;         }
  if ( ! encoded_enchant_str.empty()       ) { o += ",enchant="; o += encoded_enchant_str;       }
  if ( ! encoded_addon_str.empty()         ) { o += ",addon=";   o += encoded_addon_str;         }
  if ( ! encoded_equip_str.empty()         ) { o += ",equip=";   o += encoded_equip_str;         }
  if ( ! encoded_use_str.empty()           ) { o += ",use=";     o += encoded_use_str;           }
  if ( ! encoded_weapon_str.empty()        ) { o += ",weapon=";  o += encoded_weapon_str;        }
  if ( ! encoded_random_suffix_str.empty() ) { o += ",suffix=";  o += encoded_random_suffix_str; }
}

// item_t::init =============================================================

bool item_t::init()
{
  parse_options();

  encoded_name_str = armory_name_str;

  if ( ! option_name_str.empty() ) encoded_name_str = option_name_str;

  if ( ! option_id_str.empty() )
  {
    if ( ! item_t::download_item( *this, option_id_str ) )
    {
      return false;
    }

    if ( encoded_name_str != armory_name_str )
    {
      sim -> errorf( "Player %s at slot %s has inconsistency between name '%s' and '%s' for id '%s'\n",
                     player -> name(), slot_name(), option_name_str.c_str(), armory_name_str.c_str(), option_id_str.c_str() );

      encoded_name_str = armory_name_str;
    }
  }

  if ( encoded_name_str != "empty" &&
       encoded_name_str != "none" )
  {
    id_str                    = armory_id_str;
    encoded_stats_str         = armory_stats_str;
    encoded_enchant_str       = armory_enchant_str;
    encoded_addon_str         = armory_addon_str;
    encoded_weapon_str        = armory_weapon_str;
    encoded_heroic_str        = armory_heroic_str;
    encoded_lfr_str           = armory_lfr_str;
    encoded_armor_type_str    = armory_armor_type_str;
    encoded_ilevel_str        = armory_ilevel_str;
    encoded_quality_str       = armory_quality_str;
    encoded_random_suffix_str = armory_random_suffix_str;
  }

  if ( ! option_heroic_str.empty()  ) encoded_heroic_str  = option_heroic_str;

  if ( ! decode_heroic()  ) return false;

  if ( ! option_lfr_str.empty() ) encoded_lfr_str = option_lfr_str;

  if ( ! decode_lfr() ) return false;

  if ( ! option_armor_type_str.empty() ) encoded_armor_type_str = option_armor_type_str;

  if ( ! option_ilevel_str.empty() ) encoded_ilevel_str = option_ilevel_str;

  if ( ! decode_ilevel() ) return false;

  if ( ! option_quality_str.empty() ) encoded_quality_str = option_quality_str;

  if ( ! decode_quality() ) return false;

  unique_gear_t::get_equip_encoding( encoded_equip_str, encoded_name_str, heroic(), lfr(), player -> ptr, id_str );
  unique_gear_t::get_use_encoding  ( encoded_use_str,   encoded_name_str, heroic(), lfr(), player -> ptr, id_str );

  if ( ! option_stats_str.empty()   ) encoded_stats_str   = option_stats_str;
  if ( ! option_enchant_str.empty() ) encoded_enchant_str = option_enchant_str;
  if ( ! option_addon_str.empty()   ) encoded_addon_str   = option_addon_str;
  if ( ! option_weapon_str.empty()  ) encoded_weapon_str  = option_weapon_str;
  if ( ! option_random_suffix_str.empty() ) encoded_random_suffix_str = option_random_suffix_str;


  if ( ! decode_stats()         ) return false;
  if ( ! decode_enchant()       ) return false;
  if ( ! decode_addon()         ) return false;
  if ( ! decode_weapon()        ) return false;
#if 0
  if ( ! decode_random_suffix() ) return false;
#endif

  if ( ! option_equip_str.empty() ) encoded_equip_str = option_equip_str;
  if ( ! option_use_str.empty()   ) encoded_use_str   = option_use_str;

  if ( ! decode_special(   use, encoded_use_str   ) ) return false;
  if ( ! decode_special( equip, encoded_equip_str ) ) return false;

  encode_options();

  return true;
}

// item_t::decode_heroic ====================================================

bool item_t::decode_heroic()
{
  is_heroic = ! ( encoded_heroic_str.empty() || ( encoded_heroic_str == "0" ) || ( encoded_heroic_str == "no" ) );

  return true;
}

// item_t::decode_lfr =======================================================

bool item_t::decode_lfr()
{
  is_lfr = ! ( encoded_lfr_str.empty() || ( encoded_lfr_str == "0" ) || ( encoded_lfr_str == "no" ) );

  return true;
}

// item_t::decode_ilevel ====================================================

bool item_t::decode_ilevel()
{
  if ( encoded_ilevel_str.empty() ) return true;

  long ilvl = strtol( encoded_ilevel_str.c_str(), 0, 10 );

  if ( ilvl < 1 || ilvl == std::numeric_limits<long>::max() ) return false;

  ilevel = ilvl;

  return true;
}

// item_t::decode_quality ===================================================

bool item_t::decode_quality()
{
  if ( ! encoded_quality_str.empty() )
    quality = util_t::parse_quality_type( encoded_quality_str );
  return true;
}

// item_t::decode_stats =====================================================

bool item_t::decode_stats()
{
  if ( encoded_stats_str == "none" ) return true;

  std::vector<token_t> tokens;
  int num_tokens = parse_tokens( tokens, encoded_stats_str );

  for ( int i=0; i < num_tokens; i++ )
  {
    token_t& t = tokens[ i ];

    stat_type s = util_t::parse_stat_type( t.name );

    if ( s != STAT_NONE )
    {
      if ( s == STAT_ARMOR )
      {
        if ( slot != SLOT_HEAD      &&
             slot != SLOT_SHOULDERS &&
             slot != SLOT_CHEST     &&
             slot != SLOT_WAIST     &&
             slot != SLOT_LEGS      &&
             slot != SLOT_FEET      &&
             slot != SLOT_WRISTS    &&
             slot != SLOT_HANDS     &&
             slot != SLOT_BACK      &&
             slot != SLOT_OFF_HAND )
        {
          s = STAT_BONUS_ARMOR;
        }
      }
      base_stats.add_stat( s, t.value );
      stats.add_stat( s, t.value );
    }
    else
    {
      sim -> errorf( "Player %s has unknown 'stats=' token '%s' at slot %s\n", player -> name(), t.full.c_str(), slot_name() );
      return false;
    }
  }

  return true;
}

#if 0
// item_t::decode_random_suffix =============================================

bool item_t::decode_random_suffix()
{
  int                                       f = item_database_t::random_suffix_type( *this );

  if ( encoded_random_suffix_str.empty() ||
       encoded_random_suffix_str == "none" ||
       encoded_random_suffix_str == "0" )
    return true;

  // We need the ilevel/quality data, otherwise we cannot figure out
  // the random suffix point allocation.
  if ( ilevel == 0 || quality == 0 )
  {
    sim -> errorf( "Player %s with random suffix at slot %s requires both ilevel= and quality= information.\n", player -> name(), slot_name() );
    return true;
  }

  long rsid = abs( strtol( encoded_random_suffix_str.c_str(), 0, 10 ) );
  const random_prop_data_t& ilevel_data   = player -> dbc.random_property( ilevel );
  const random_suffix_data_t& suffix_data = player -> dbc.random_suffix( rsid );

  if ( ! suffix_data.id )
  {
    sim -> errorf( "Warning: Unknown random suffix identifier %ld at slot %s for item %s.\n",
                   rsid, slot_name(), name() );
    return true;
  }

  if ( sim -> debug )
  {
    log_t::output( sim, "random_suffix: item=%s suffix_id=%ld ilevel=%d quality=%d random_point_pool=%d",
                   name(), rsid, ilevel, quality, f );
  }

  std::vector<std::string>          stat_list;
  for ( int i = 0; i < 5; i++ )
  {
    unsigned                         enchant_id;
    if ( ! ( enchant_id = suffix_data.enchant_id[ i ] ) )
      continue;

    const item_enchantment_data_t& enchant_data = player -> dbc.item_enchantment( enchant_id );

    if ( ! enchant_data.id )
      continue;

    // Calculate amount of points
    double                          stat_amount;
    if ( quality == 4 ) // Epic
      stat_amount = ilevel_data.p_epic[ f ] * suffix_data.enchant_alloc[ i ] / 10000.0;
    else if ( quality == 3 ) // Rare
      stat_amount = ilevel_data.p_rare[ f ] * suffix_data.enchant_alloc[ i ] / 10000.0;
    else if ( quality == 2 ) // Uncommon
      stat_amount = ilevel_data.p_uncommon[ f ] * suffix_data.enchant_alloc[ i ] / 10000.0;
    else // Impossible choices, so bogus data, skip
      continue;

    // Loop through enchantment stats, adding valid ones to the stats of the item.
    // Typically (and for cata random suffixes), there seems to be only one stat per enchantment
    for ( int j = 0; j < 3; j++ )
    {
      if ( enchant_data.ench_type[ j ] != ITEM_ENCHANTMENT_STAT ) continue;

      stat_type stat = util_t::translate_item_mod( enchant_data.ench_prop[ j ] );

      if ( stat == STAT_NONE ) continue;

      // Make absolutely sure we do not add stats twice
      if ( base_stats.get_stat( stat ) == 0 )
      {
        base_stats.add_stat( stat, static_cast<int>( stat_amount ) );
        stats.add_stat( stat, static_cast<int>( stat_amount ) );

        std::string stat_str = util_t::stat_type_abbrev( stat );
        stat_list.push_back( ( boost::format( "%d%s" ) % static_cast<int>( stat_amount )
                               % stat_str ).str() );

        if ( sim -> debug )
          log_t::output( sim, "random_suffix: stat=%d (%s) stat_amount=%f", stat, stat_str.c_str(), stat_amount );
      }
      else
      {
        if ( sim -> debug )
        {
          log_t::output( sim, "random_suffix: Player %s item %s attempted to add base stat %d %d (%d) twice, due to random suffix.",
                         player -> name(), name(), j, stat, enchant_data.ench_type[ j ] );
        }
      }
    }
  }

  std::string name_str = suffix_data.suffix;
  util_t::format_name( name_str );

  if ( encoded_name_str.find( name_str ) == std::string::npos )
  {
    encoded_name_str += '_' + name_str;
  }


  // Append stats to the existing encoded stats string, as
  // a simple suffix will not tell the user anything about
  // the item
  if ( ! encoded_stats_str.empty() && stat_list.size() > 0 )
    encoded_stats_str += "_";

  for ( unsigned i = 0; i < stat_list.size(); i++ )
  {
    encoded_stats_str += stat_list[ i ];

    if ( i < stat_list.size() - 1 )
      encoded_stats_str += "_";
  }

  return true;
}
#endif

// item_t::decode_enchant ===================================================

bool item_t::decode_enchant()
{
  if ( encoded_enchant_str == "none" ) return true;

  if ( encoded_enchant_str == "berserking"       ||
       encoded_enchant_str == "executioner"      ||
       encoded_enchant_str == "mongoose"         ||
       encoded_enchant_str == "avalanche"        ||
       encoded_enchant_str == "elemental_slayer" ||
       encoded_enchant_str == "hurricane"        ||
       encoded_enchant_str == "landslide"        ||
       encoded_enchant_str == "power_torrent"    ||
       encoded_enchant_str == "windwalk"         ||
       encoded_enchant_str == "spellsurge"       ||
       encoded_enchant_str == "synapse_springs"  ||
       encoded_enchant_str == "gnomish_xray"      )
  {
    unique_enchant = true;
    return true;
  }

  if ( encoded_enchant_str == "lightweave_embroidery" ||
       encoded_enchant_str == "lightweave_embroidery_old" ||
       encoded_enchant_str == "lightweave" ||
       encoded_enchant_str == "lightweave_old" ||
       encoded_enchant_str == "swordguard_embroidery" ||
       encoded_enchant_str == "swordguard_embroidery_old" ||
       encoded_enchant_str == "darkglow_embroidery" ||
       encoded_enchant_str == "darkglow_embroidery_old" )
  {
    //stats.add_stat( STAT_SPIRIT, 1 );
  }

  std::string use_str;
  if ( unique_gear_t::get_use_encoding( use_str, encoded_enchant_str, heroic(), lfr(), player -> ptr ) )
  {
    unique_enchant = true;
    use.name_str = encoded_enchant_str;
    return decode_special( use, use_str );
  }

  std::string equip_str;
  if ( unique_gear_t::get_equip_encoding( equip_str, encoded_enchant_str, heroic(), lfr(), player -> ptr ) )
  {
    unique_enchant = true;
    enchant.name_str = encoded_enchant_str;
    return decode_special( enchant, equip_str );
  }

  std::vector<token_t> tokens;
  int num_tokens = parse_tokens( tokens, encoded_enchant_str );

  for ( int i=0; i < num_tokens; i++ )
  {
    token_t& t = tokens[ i ];

    stat_type s = util_t::parse_stat_type( t.name );
    if ( s != STAT_NONE )
      stats.add_stat( s, t.value );
    else
    {
      sim -> errorf( "Player %s has unknown 'enchant=' token '%s' at slot %s\n", player -> name(), t.full.c_str(), slot_name() );
      continue;
    }
  }

  return true;
}

// item_t::decode_addon =====================================================

bool item_t::decode_addon()
{
  if ( encoded_addon_str == "none" ) return true;

  if ( encoded_addon_str == "synapse_springs" )
  {
    unique_addon = true;
    return true;
  }

  std::string use_str;
  if ( unique_gear_t::get_use_encoding( use_str, encoded_addon_str, heroic(), lfr(), player -> ptr ) )
  {
    unique_addon = true;
    use.name_str = encoded_addon_str;
    return decode_special( use, use_str );
  }

  std::string equip_str;
  if ( unique_gear_t::get_equip_encoding( equip_str, encoded_addon_str, heroic(), lfr(), player -> ptr ) )
  {
    unique_addon = true;
    addon.name_str = encoded_addon_str;
    return decode_special( addon, equip_str );
  }

  std::vector<token_t> tokens;
  int num_tokens = parse_tokens( tokens, encoded_addon_str );

  for ( int i=0; i < num_tokens; i++ )
  {
    token_t& t = tokens[ i ];
    stat_type s = util_t::parse_stat_type( t.name );

    if ( s != STAT_NONE )
      stats.add_stat( s, t.value );
    else
    {
      sim -> errorf( "Player %s has unknown 'addon=' token '%s' at slot %s\n", player -> name(), t.full.c_str(), slot_name() );
      return false;
    }
  }

  return true;
}

// item_t::decode_special ===================================================

bool item_t::decode_special( special_effect_t& effect,
                             const std::string& encoding )
{
  if ( encoding == "custom" || encoding == "none" ) return true;

  std::vector<token_t> tokens;
  int num_tokens = parse_tokens( tokens, encoding );

  for ( int i=0; i < num_tokens; i++ )
  {
    token_t& t = tokens[ i ];
    stat_type s;
    school_type sc;

    if ( ( s = util_t::parse_stat_type( t.name ) ) != STAT_NONE )
    {
      effect.stat = s;
      effect.stat_amount = t.value;
    }
    else if ( ( sc = util_t::parse_school_type( t.name ) ) != SCHOOL_NONE )
    {
      effect.school = sc;
      effect.discharge_amount = t.value;

      std::vector<std::string> splits;
      if ( 2 == util_t::string_split( splits, t.value_str, "+" ) )
      {
        effect.discharge_amount  = atof( splits[ 0 ].c_str() );
        effect.discharge_scaling = atof( splits[ 1 ].c_str() ) / 100.0;
      }
    }
    else if ( t.name == "stacks" || t.name == "stack" )
    {
      effect.max_stacks = ( int ) t.value;
    }
    else if ( t.name == "%" )
    {
      effect.proc_chance = t.value / 100.0;
    }
    else if ( t.name == "ppm" )
    {
      effect.proc_chance = -t.value;
    }
    else if ( t.name == "duration" || t.name == "dur" )
    {
      effect.duration = from_seconds( t.value );
    }
    else if ( t.name == "cooldown" || t.name == "cd" )
    {
      effect.cooldown = from_seconds( t.value );
    }
    else if ( t.name == "tick" )
    {
      effect.tick = from_seconds( t.value );
    }
    else if ( t.full == "reverse" )
    {
      effect.reverse = true;
    }
    else if ( t.full == "chance" )
    {
      effect.chance_to_discharge = true;
    }
    else if ( t.name == "costrd" )
    {
      effect.cost_reduction = true;
      effect.no_refresh = true;
    }
    else if ( t.name == "nocrit" )
    {
      effect.no_crit = true;
    }
    else if ( t.name == "nobuffs" )
    {
      effect.no_player_benefits = true;
    }
    else if ( t.name == "nodebuffs" )
    {
      effect.no_player_benefits = true;
    }
    else if ( t.name == "norefresh" )
    {
      effect.no_refresh = true;
    }
    else if ( t.full == "ondamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = SCHOOL_ALL_MASK;
    }
    else if ( t.full == "onheal" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_HEAL;
      effect.trigger_mask = SCHOOL_ALL_MASK;
    }
    else if ( t.full == "ontickdamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_TICK_DAMAGE;
      effect.trigger_mask = SCHOOL_ALL_MASK;
    }
    else if ( t.full == "ondirectdamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DIRECT_DAMAGE;
      effect.trigger_mask = SCHOOL_ALL_MASK;
    }
    else if ( t.full == "onkineticdamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = bitmask( SCHOOL_KINETIC );
    }
    else if ( t.full == "onenergydamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = bitmask( SCHOOL_ENERGY );
    }
    else if ( t.full == "oninternaldamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = bitmask( SCHOOL_INTERNAL );
    }
    else if ( t.full == "onelementaldamage" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_DAMAGE;
      effect.trigger_mask = bitmask( SCHOOL_ELEMENTAL );
    }
    else if ( t.full == "ontick" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_TICK;
      effect.trigger_mask = RESULT_ALL_MASK;
    }
    else if ( t.full == "ontickhit" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_TICK;
      effect.trigger_mask = RESULT_HIT_MASK;
    }
    else if ( t.full == "ontickcrit" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_TICK;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onspellcast" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_SPELL;
      effect.trigger_mask = RESULT_NONE_MASK;
    }
    else if ( t.full == "onspellhit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_SPELL;
      effect.trigger_mask = RESULT_HIT_MASK;
    }
    else if ( t.full == "onspellcrit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_SPELL_AND_TICK;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onspelltickcrit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_TICK;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onspelldirectcrit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_SPELL;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onspellmiss" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_SPELL;
      effect.trigger_mask = RESULT_MISS_MASK;
    }
    else if ( t.full == "onharmfulspellcast" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HARMFUL_SPELL;
      effect.trigger_mask = RESULT_NONE_MASK;
    }
    else if ( t.full == "onharmfulspellhit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HARMFUL_SPELL;
      effect.trigger_mask = RESULT_HIT_MASK;
    }
    else if ( t.full == "onharmfulspellcrit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HARMFUL_SPELL;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onharmfulspellmiss" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HARMFUL_SPELL;
      effect.trigger_mask = RESULT_MISS_MASK;
    }
    else if ( t.full == "onhealcast" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HEAL_SPELL;
      effect.trigger_mask = RESULT_NONE_MASK;
    }
    else if ( t.full == "onhealhit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HEAL_SPELL;
      effect.trigger_mask = RESULT_HIT_MASK;
    }
    else if ( t.full == "onhealdirectcrit" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HEAL_SPELL;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onhealmiss" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_HEAL_SPELL;
      effect.trigger_mask = RESULT_MISS_MASK;
    }
    else if ( t.full == "onattack" )
    {
      effect.trigger_str = t.full;
      effect.trigger_type = PROC_ATTACK;
      effect.trigger_mask = RESULT_ALL_MASK;
    }
    else if ( t.full == "onattackhit" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_ATTACK;
      effect.trigger_mask = RESULT_HIT_MASK;
    }
    else if ( t.full == "onattackcrit" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_ATTACK;
      effect.trigger_mask = RESULT_CRIT_MASK;
    }
    else if ( t.full == "onattackmiss" )
    {
      effect.trigger_str  = t.full;
      effect.trigger_type = PROC_ATTACK;
      effect.trigger_mask = RESULT_MISS_MASK;
    }
    else
    {
      sim -> errorf( "Player %s has unknown 'use/equip=' token '%s' at slot %s\n", player -> name(), t.full.c_str(), slot_name() );
      return false;
    }
  }

  return true;
}

// item_t::decode_weapon ====================================================

bool item_t::decode_weapon()
{
  weapon_t* w = weapon();
  if ( ! w ) return true;

  std::vector<token_t> tokens;
  int num_tokens = parse_tokens( tokens, encoded_weapon_str );

  bool min_set=false, max_set=false;

  for ( int i=0; i < num_tokens; i++ )
  {
    token_t& t = tokens[ i ];
    weapon_type type;
    school_type school;

    if ( ( type = util_t::parse_weapon_type( t.name ) ) != WEAPON_NONE )
    {
      w -> type = type;
    }
    else if ( ( school = util_t::parse_school_type( t.name ) ) != SCHOOL_NONE )
    {
      w -> school = school;
    }
    else if ( t.name == "damage" || t.name == "dmg" )
    {
      w -> damage  = t.value;
      w -> min_dmg = t.value;
      w -> max_dmg = t.value;
    }
    else if ( t.name == "min" )
    {
      w -> min_dmg = t.value;
      min_set = true;

      if ( max_set )
      {
        w -> damage = ( w -> min_dmg + w -> max_dmg ) / 2;
      }
      else
      {
        w -> max_dmg = w -> min_dmg;
      }
    }
    else if ( t.name == "max" )
    {
      w -> max_dmg = t.value;
      max_set = true;

      if ( min_set )
      {
        w -> damage = ( w -> min_dmg + w -> max_dmg ) / 2;
      }
      else
      {
        w -> min_dmg = w -> max_dmg;
      }
    }
    else
    {
      sim -> errorf( "Player %s has unknown 'weapon=' token '%s' at slot %s\n", player -> name(), t.full.c_str(), slot_name() );
      return false;
    }
  }

  if ( ! max_set || ! min_set )
  {
    w -> max_dmg = w -> damage;
    w -> min_dmg = w -> damage;
  }

  return true;
}

// item_t::download_slot ====================================================

bool item_t::download_slot( item_t& item,
                            const std::string& /* item_id */,
                            const std::string& /* enchant_id */,
                            const std::string& /* addon_id */,
                            const std::string& /* reforge_id */,
                            const std::string& /* rsuffix_id */ )
{
  const cache::behavior_t cb = cache::items();
  bool success = false;

  if ( cb != cache::CURRENT )
  {
    bool has_local = false;

    for ( unsigned i = 0; ! success && i < item.sim -> item_db_sources.size(); i++ )
    {
      const std::string& src = item.sim -> item_db_sources[ i ];
      if ( src == "local" )
        has_local = true;
#if 0
      else if ( src == "wowhead" )
        success = wowhead_t::download_slot( item, item_id, enchant_id, addon_id, reforge_id,
                                            rsuffix_id, gem_ids, item.player -> dbc.ptr, cache::ONLY );
      else if ( src == "ptrhead" )
        success = wowhead_t::download_slot( item, item_id, enchant_id, addon_id, reforge_id,
                                            rsuffix_id, gem_ids, ! item.player -> dbc.ptr, cache::ONLY );
      else if ( src == "mmoc" )
        success = mmo_champion_t::download_slot( item, item_id, enchant_id, addon_id, reforge_id,
                                                 rsuffix_id, gem_ids, cache::ONLY );
      else if ( src == "armory" )
        success = armory_t::download_slot( item, item_id, cache::ONLY );
      else if ( src == "bcpapi" )
        success = bcp_api::download_slot( item, item_id, enchant_id, addon_id, reforge_id,
                                          rsuffix_id, gem_ids, cache::ONLY );
#endif
    }

    if ( ! success && has_local )
      assert( 0 );
      /*success = item_database_t::download_slot( item, item_id, enchant_id, addon_id, reforge_id,
                                                rsuffix_id, gem_ids );*/
  }

#if 0
  if ( cb != cache::ONLY )
  {
    // Download in earnest from a data source
    for ( unsigned i = 0; ! success && i < item.sim -> item_db_sources.size(); i++ )
    {
      const std::string& src = item.sim -> item_db_sources[ i ];
      if ( src == "wowhead" )
        success = wowhead_t::download_slot( item, item_id, enchant_id, addon_id, reforge_id,
                                            rsuffix_id, gem_ids, item.player -> dbc.ptr, cb );
      else if ( src == "ptrhead" )
        success = wowhead_t::download_slot( item, item_id, enchant_id, addon_id, reforge_id,
                                            rsuffix_id, gem_ids, ! item.player -> dbc.ptr, cb );
      else if ( src == "mmoc" )
        success = mmo_champion_t::download_slot( item, item_id, enchant_id, addon_id, reforge_id,
                                                 rsuffix_id, gem_ids, cb );
      else if ( src == "armory" )
        success = armory_t::download_slot( item, item_id, cache::items() );
      else if ( src == "bcpapi" )
        success = bcp_api::download_slot( item, item_id, enchant_id, addon_id, reforge_id,
                                          rsuffix_id, gem_ids, cb );
    }
  }
#endif

  return success;
}

// item_t::download_item ====================================================

bool item_t::download_item( item_t& /* item */, const std::string& /* item_id */ )
{
  std::vector<std::string> source_list;
#if 0
  if ( ! item_database_t::initialize_item_sources( item, source_list ) )
  {
    item.sim -> errorf( "Your item-specific data source string \"%s\" contained no valid sources to download item id %s.\n",
                        item.option_data_source_str.c_str(), item_id.c_str() );
    return false;
  }
#endif

  bool success = false;
  if ( cache::items() != cache::CURRENT )
  {
    bool has_local = false;

    // Check data source caches, except local
    for ( unsigned i = 0; ! success && i < source_list.size(); i++ )
    {
      if ( source_list[ i ] == "local" )
        has_local = true;
#if 0
      else if ( source_list[ i ] == "wowhead" )
        success = wowhead_t::download_item( item, item_id, false, cache::ONLY );
      else if ( source_list[ i ] == "ptrhead" )
        success = wowhead_t::download_item( item, item_id, true, cache::ONLY );
      else if ( source_list[ i ] == "mmoc" )
        success = mmo_champion_t::download_item( item, item_id, cache::ONLY );
      else if ( source_list[ i ] == "armory" )
        success = armory_t::download_item( item, item_id, cache::ONLY );
      else if ( source_list[ i ] == "bcpapi" )
        success = bcp_api::download_item( item, item_id, cache::ONLY );
#endif
    }

    if ( ! success && has_local )
      assert( 0 );
      //success = item_database_t::download_item( item, item_id );
  }

#if 0
  if ( cache::items() != cache::ONLY )
  {
    // Download in earnest from a data source
    for ( unsigned i = 0; ! success && i < source_list.size(); i++ )
    {
      if ( source_list[ i ] == "wowhead" )
        success = wowhead_t::download_item( item, item_id, false );
      else if ( source_list[ i ] == "ptrhead" )
        success = wowhead_t::download_item( item, item_id, true );
      else if ( source_list[ i ] == "mmoc" )
        success = mmo_champion_t::download_item( item, item_id );
      else if ( source_list[ i ] == "armory" )
        success = armory_t::download_item( item, item_id );
      else if ( source_list[ i ] == "bcpapi" )
        success = bcp_api::download_item( item, item_id );
    }
  }
#endif

  return success;
}

#if 0
// item_t::download_glyph ===================================================

bool item_t::download_glyph( player_t* player, std::string& glyph_name, const std::string& glyph_id )
{
  bool success = false;

  if ( cache::items() != cache::CURRENT )
  {
    bool has_local = false;

    // Check data source caches, except local
    for ( unsigned i = 0; ! success && i < player -> sim -> item_db_sources.size(); i++ )
    {
      const std::string& src = player -> sim -> item_db_sources[ i ];
      if ( src == "local" )
        has_local = true;
      else if ( src == "wowhead" )
        success = wowhead_t::download_glyph( player, glyph_name, glyph_id, false, cache::ONLY );
      else if ( src == "ptrhead" )
        success = wowhead_t::download_glyph( player, glyph_name, glyph_id, true, cache::ONLY );
      else if ( src == "mmoc" )
        success = mmo_champion_t::download_glyph( player, glyph_name, glyph_id, cache::ONLY );
      else if ( src == "bcpapi" )
        success = bcp_api::download_glyph( player, glyph_name, glyph_id, cache::ONLY );
    }

    if ( ! success && has_local )
      success = item_database_t::download_glyph( player, glyph_name, glyph_id );
  }

  if ( cache::items() != cache::ONLY )
  {
    // Download in earnest from a data source
    for ( unsigned i = 0; ! success && i < player -> sim -> item_db_sources.size(); i++ )
    {
      const std::string& src = player -> sim -> item_db_sources[ i ];
      if ( src == "wowhead" )
        success = wowhead_t::download_glyph( player, glyph_name, glyph_id );
      else if ( src == "ptrhead" )
        success = wowhead_t::download_glyph( player, glyph_name, glyph_id, true );
      else if ( src == "mmoc" )
        success = mmo_champion_t::download_glyph( player, glyph_name, glyph_id );
      else if ( src == "bcpapi" )
        success = bcp_api::download_glyph( player, glyph_name, glyph_id );
    }
  }

  return success;
}

// item_t::parse_gem ========================================================

int item_t::parse_gem( item_t&            item,
                       const std::string& gem_id )
{
  if ( gem_id.empty() || gem_id == "0" )
    return GEM_NONE;

  std::vector<std::string> source_list;
  if ( ! item_database_t::initialize_item_sources( item, source_list ) )
  {
    item.sim -> errorf( "Your item-specific data source string \"%s\" contained no valid sources to download gem id %s.\n",
                        item.option_data_source_str.c_str(), gem_id.c_str() );
    return GEM_NONE;
  }

  int gem_type = GEM_NONE;

  if ( cache::items() != cache::CURRENT )
  {
    // Check data source caches, except local
    bool has_local = false;

    for ( unsigned i = 0; gem_type == GEM_NONE && i < source_list.size(); i++ )
    {
      if ( source_list[ i ] == "local" )
        has_local = true;
      else if ( source_list[ i ] == "wowhead" )
        gem_type = wowhead_t::parse_gem( item, gem_id, false, cache::ONLY );
      else if ( source_list[ i ] == "ptrhead" )
        gem_type = wowhead_t::parse_gem( item, gem_id, true, cache::ONLY );
      else if ( source_list[ i ] == "mmoc" )
        gem_type = mmo_champion_t::parse_gem( item, gem_id, cache::ONLY );
      else if ( source_list[ i ] == "bcpapi" )
        gem_type = bcp_api::parse_gem( item, gem_id, cache::ONLY );
    }

    if ( gem_type == GEM_NONE && has_local )
      gem_type = item_database_t::parse_gem( item, gem_id );
  }

  if ( cache::items() != cache::ONLY )
  {
    // Nothing found from a cache, nor local item db. Let's fetch, again honoring our source list
    for ( unsigned i = 0; gem_type == GEM_NONE && i < source_list.size(); i++ )
    {
      if ( source_list[ i ] == "wowhead" )
        gem_type = wowhead_t::parse_gem( item, gem_id );
      else if ( source_list[ i ] == "ptrhead" )
        gem_type = wowhead_t::parse_gem( item, gem_id, true );
      else if ( source_list[ i ] == "mmoc" )
        gem_type = mmo_champion_t::parse_gem( item, gem_id );
      else if ( source_list[ i ] == "bcpapi" )
        gem_type = bcp_api::parse_gem( item, gem_id );
    }
  }

  return gem_type;
}
#endif // 0
