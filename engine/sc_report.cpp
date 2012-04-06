// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"
#include <fstream>

namespace { // ANONYMOUS NAMESPACE ==========================================

#if SC_BETA
// beta warning messages
static const char* beta_warnings[] =
{
  "Beta! Beta! Beta! Beta! Beta! Beta!",
  "All classes are supported.",
  "Some class models still need tweaking.",
  "Some class action lists need tweaking.",
  "Some class BiS gear setups need tweaking.",
  "Some trinkets not yet implemented.",
  "Constructive feedback regarding our output will shorten the Beta phase dramatically.",
  "Beta! Beta! Beta! Beta! Beta! Beta!",
  0
};
#endif

} // ANONYMOUS NAMESPACE ====================================================

// ==========================================================================
// Report
// ==========================================================================

// report_t::encode_html ====================================================

std::string report_t::encode_html( std::string buffer )
{
  util_t::replace_all( buffer, '&', "&amp;" );
  util_t::replace_all( buffer, '<', "&lt;" );
  util_t::replace_all( buffer, '>', "&gt;" );
  return buffer;
}

// report_t::print_profiles =================================================

void report_t::print_profiles( sim_t* sim )
{
  int k = 0;
  for ( unsigned int i = 0; i < sim -> actor_list.size(); i++ )
  {
    player_t& p = *sim -> actor_list[i];
    if ( p.is_pet() ) continue;

    k++;

    auto maybe_save_profile = [sim,&p]( const std::string& filename,
                                        save_type st, const char* errorname )
    {
      if ( filename.empty() ) return;

      std::ofstream file( filename, std::ios::trunc );
      if ( ! file )
        sim -> errorf( "Unable to save%s%s profile %s for player %s\n",
                       ( errorname ? " " : "" ), errorname,
                       filename.c_str(), p.name() );
      else
        p.create_profile( file, st );
    };

    maybe_save_profile( p.save_gear_str, SAVE_GEAR, "gear" );
    maybe_save_profile( p.save_talents_str, SAVE_TALENTS, "talents" );
    maybe_save_profile( p.save_actions_str, SAVE_ACTIONS, "actions" );

    std::string file_name = p.save_str;

    if ( file_name.empty() )
    {
      if ( ! sim -> save_profiles )
        continue;

      file_name = sim -> save_prefix_str + p.name_str;
      if ( sim -> save_talent_str )
        file_name += '_' + p.primary_tree_name();
      file_name += sim -> save_suffix_str + ".simc";
      util_t::urlencode( util_t::format_text( file_name, sim -> input_is_utf8 ) );
    }

    maybe_save_profile( file_name, SAVE_ALL, nullptr );
  }

  // Save overview file for Guild downloads
  //if ( /* guild parse */ )
  if ( sim -> save_raid_summary )
  {
    std::string filename = "Raid_Summary.simc";
    std::ofstream file( filename, std::ios::trunc );
    if ( ! file )
      sim -> errorf( "Unable to save overview profile %s\n", filename.c_str() );
    else
    {
      file << "#Raid Summary\n"
              "# Contains " << k << " Players.\n";

      for ( unsigned int i = 0; i < sim -> actor_list.size(); i++ )
      {
        player_t* p = sim -> actor_list[ i ];
        if ( p -> is_pet() ) continue;

        file << '\n';

        if ( ! p -> save_str.empty() )
          file << p -> save_str << '\n';

        else if ( sim -> save_profiles )
        {
          file << "# Player: " << p -> name_str << " Spec: " << p -> primary_tree_name()
               << " Role: " << util_t::role_type_string( p -> primary_role() ) << '\n';

          std::string profile_name = sim -> save_prefix_str + p -> name_str;
          if ( sim -> save_talent_str )
            profile_name += '_' + p -> primary_tree_name();
          profile_name += sim -> save_suffix_str + ".simc";
          util_t::urlencode( util_t::format_text( profile_name, sim -> input_is_utf8 ) );

          file << profile_name << '\n';
        }
      }
    }
  }
}

void report_t::print_json_profiles( sim_t* sim )
{
  for ( player_t* p : sim -> actor_list )
  {
    if ( p -> is_pet() ) continue;

    std::string file_name = p -> save_json_str;
    if ( file_name.empty() )
    {
      if ( ! sim -> save_profiles )
      continue;

      file_name = sim -> save_prefix_str + p -> name_str;
      if ( sim -> save_talent_str )
        file_name += '_' + p -> primary_tree_name();
      file_name += sim -> save_suffix_str + ".simc";
      util_t::urlencode( util_t::format_text( file_name, sim -> input_is_utf8 ) );
    }

    std::ofstream file( file_name, std::ios::out | std::ios::trunc );
    if ( ! file )
    {
      sim -> errorf( "Unable to save profile %s for player %s\n", file_name.c_str(), p -> name() );
      continue;
    }
    p -> create_json_profile( file );
  }
}

// report_t::print_spell_query ==============================================
/*
void report_t::print_spell_query( sim_t* sim )
{
  spell_data_expr_t* sq = sim -> spell_query;
  assert( sq );

  for ( std::vector<uint32_t>::const_iterator i = sq -> result_spell_list.begin(); i != sq -> result_spell_list.end(); i++ )
  {
    if ( sq -> data_type == DATA_TALENT )
    {
      util_t::fprintf( sim -> output_file, "%s", spell_info_t::talent_to_str( sim, sim -> dbc.talent( *i ) ).c_str() );
    }
    else if ( sq -> data_type == DATA_EFFECT )
    {
      std::ostringstream sqs;
      const spell_data_t* spell = sim -> dbc.spell( sim -> dbc.effect( *i ) -> spell_id() );
      if ( spell )
      {
        spell_info_t::effect_to_str( sim,
                                     spell,
                                     sim -> dbc.effect( *i ),
                                     sqs );
      }
      util_t::fprintf( sim -> output_file, "%s", sqs.str().c_str() );
    }
    else
    {
      const spell_data_t* spell = sim -> dbc.spell( *i );
      util_t::fprintf( sim -> output_file, "%s", spell_info_t::to_str( sim, spell ).c_str() );
    }
  }
}
*/
// report_t::print_suite ====================================================

void report_t::print_suite( sim_t* sim )
{
  report_t::print_text( sim -> output_file, sim, sim -> report_details != 0 );
  report_t::print_html( sim );
  report_t::print_xml( sim );
  report_t::print_profiles( sim );
}
