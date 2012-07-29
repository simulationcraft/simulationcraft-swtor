// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

namespace { // ANONYMOUS NAMESPACE ==========================================


// print_html_contents ======================================================

static void print_html_contents( FILE*  file, sim_t* sim )
{
  size_t c = 2;     // total number of TOC entries
  if ( sim -> scaling -> has_scale_factors() )
    ++c;

  const int num_players = ( int ) sim -> players_by_name.size();
  c += num_players;
  if ( sim -> report_targets )
    c += sim -> targets_by_name.size();

  if ( sim -> report_pets_separately )
  {
    for ( int i=0; i < num_players; i++ )
    {
      for ( pet_t* pet = sim -> players_by_name[ i ] -> pet_list; pet; pet = pet -> next_pet )
      {
        if ( pet -> summoned )
          ++c;
      }
    }
  }

  fputs( "\t\t<div id=\"table-of-contents\" class=\"section grouped-first grouped-last\">\n"
         "\t\t\t<h2 class=\"toggle\">Table of Contents</h2>\n"
         "\t\t\t<div class=\"toggle-content hide\">\n", file );

  // set number of columns
  int n;         // number of columns
  const char* toc_class; // css class
  if ( c < 6 )
  {
    n = 1;
    toc_class = "toc-wide";
  }
  else if ( sim -> report_pets_separately )
  {
    n = 2;
    toc_class = "toc-wide";
  }
  else if ( c < 9 )
  {
    n = 2;
    toc_class = "toc-narrow";
  }
  else
  {
    n = 3;
    toc_class = "toc-narrow";
  }

  int pi = 0;    // player counter
  int ab = 0;    // auras and debuffs link added yet?
  for ( int i=0; i < n; i++ )
  {
    int cs;    // column size
    if ( i == 0 )
    {
      cs = ( int ) ceil( 1.0 * c / n );
    }
    else if ( i == 1 )
    {
      if ( n == 2 )
      {
        cs = ( int ) ( c - ceil( 1.0 * c / n ) );
      }
      else
      {
        cs = ( int ) ceil( 1.0 * c / n );
      }
    }
    else
    {
      cs = ( int ) ( c - 2 * ceil( 1.0 * c / n ) );
    }

    fprintf( file,
             "\t\t\t\t<ul class=\"toc %s\">\n",
             toc_class );

    int ci = 1;    // in-column counter
    if ( i == 0 )
    {
      fputs( "\t\t\t\t\t<li><a href=\"#raid-summary\">Raid Summary</a></li>\n", file );
      ci++;
      if ( sim -> scaling -> has_scale_factors() )
      {
        fputs( "\t\t\t\t\t<li><a href=\"#raid-scale-factors\">Scale Factors</a></li>\n", file );
        ci++;
      }
    }
    while ( ci <= cs )
    {
      if ( pi < ( int ) sim -> players_by_name.size() )
      {
        player_t* p = sim -> players_by_name[ pi ];
        fprintf( file,
                 "\t\t\t\t\t<li><a href=\"#%s\">%s</a>",
                 p -> name(), p -> name() );
        ci++;
        if ( sim -> report_pets_separately )
        {
          fputs( "\n\t\t\t\t\t\t<ul>\n", file );
          for ( pet_t* pet = sim -> players_by_name[ pi ] -> pet_list; pet; pet = pet -> next_pet )
          {
            if ( pet -> summoned )
            {
              fprintf( file,
                       "\t\t\t\t\t\t\t<li><a href=\"#%s\">%s</a></li>\n",
                       pet -> name(), pet -> name() );
              ci++;
            }
          }
          fputs( "\t\t\t\t\t\t</ul>", file );
        }
        fputs( "</li>\n", file );
        pi++;
      }
      if ( pi == ( int ) sim -> players_by_name.size() )
      {
        if ( ab == 0 )
        {
          fputs( "\t\t\t\t\t\t<li><a href=\"#auras-buffs\">Auras/Buffs</a></li>\n", file );
          ab = 1;
        }
        ci++;
        fputs( "\t\t\t\t\t\t<li><a href=\"#sim-info\">Simulation Information</a></li>\n", file );
        ci++;
      }
      if ( sim -> report_targets && ab > 0 )
      {
        if ( ab == 1 )
        {
          pi = 0;
          ab = 2;
        }
        while ( ci <= cs )
        {
          if ( pi < ( int ) sim -> targets_by_name.size() )
          {
            player_t* p = sim -> targets_by_name[ pi ];
            fprintf( file,
                     "\t\t\t\t\t<li><a href=\"#%s\">%s</a></li>\n",
                     p -> name(), p -> name() );
          }
          ci++;
          pi++;
        }
      }
    }
    fputs( "\t\t\t\t</ul>\n", file );
  }

  fputs( "\t\t\t\t<div class=\"clear\"></div>\n"
         "\t\t\t</div>\n\n"
         "\t\t</div>\n\n", file );
}



// print_html_sim_summary ===================================================

static void print_html_sim_summary( FILE*  file, sim_t* sim )
{

  fprintf( file,
           "\t\t\t\t<div id=\"sim-info\" class=\"section\">\n" );

  fprintf( file,
           "\t\t\t\t\t<h2 class=\"toggle\">Simulation & Raid Information</h2>\n"
           "\t\t\t\t\t\t<div class=\"toggle-content hide\">\n" );

  fprintf( file,
           "\t\t\t\t\t\t<table class=\"sc mt\">\n" );

  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th>Iterations:</th>\n"
           "\t\t\t\t\t\t\t\t<td>%d</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           sim -> iterations );
  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th>Threads:</th>\n"
           "\t\t\t\t\t\t\t\t<td>%d</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           sim -> threads < 1 ? 1 : sim -> threads );
  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th>Confidence:</th>\n"
           "\t\t\t\t\t\t\t\t<td>%.2f%%</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           sim -> confidence * 100.0 );
  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th>Fight Length:</th>\n"
           "\t\t\t\t\t\t\t\t<td>%.0f - %.0f ( %.1f )</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           sim -> simulation_length.min,
           sim -> simulation_length.max,
           sim -> simulation_length.mean );
  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th><h2>Performance:</h2></th>\n"
           "\t\t\t\t\t\t\t\t<td></td>\n"
           "\t\t\t\t\t\t\t</tr>\n" );
  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th>Total Events Processed:</th>\n"
           "\t\t\t\t\t\t\t\t<td>%ld</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           ( long ) sim -> total_events_processed );
  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th>Max Event Queue:</th>\n"
           "\t\t\t\t\t\t\t\t<td>%ld</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           ( long ) sim -> max_events_remaining );
  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th>Sim Seconds:</th>\n"
           "\t\t\t\t\t\t\t\t<td>%.0f</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           sim -> iterations * sim -> simulation_length.mean );
  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th>CPU Seconds:</th>\n"
           "\t\t\t\t\t\t\t\t<td>%.4f</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           to_seconds( sim -> elapsed_cpu ) );
  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th>Speed Up:</th>\n"
           "\t\t\t\t\t\t\t\t<td>%.0f</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           sim -> iterations * sim -> simulation_length.mean / to_seconds( sim -> elapsed_cpu ) );

  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th><h2>Settings:</h2></th>\n"
           "\t\t\t\t\t\t\t\t<td></td>\n"
           "\t\t\t\t\t\t\t</tr>\n" );
  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th>World Lag:</th>\n"
           "\t\t\t\t\t\t\t\t<td>%.0f ms ( stddev = %.0f ms )</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           to_millis( sim -> world_lag ), to_millis( sim -> world_lag_stddev ) );
  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th>Queue Lag:</th>\n"
           "\t\t\t\t\t\t\t\t<td>%.0f ms ( stddev = %.0f ms )</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           to_millis( sim -> queue_lag ), to_millis( sim -> queue_lag_stddev ) );
  if ( sim -> strict_gcd_queue )
  {
    fprintf( file,
             "\t\t\t\t\t\t\t<tr class=\"left\">\n"
             "\t\t\t\t\t\t\t\t<th>GCD Lag:</th>\n"
             "\t\t\t\t\t\t\t\t<td>%.0f ms ( stddev = %.0f ms )</td>\n"
             "\t\t\t\t\t\t\t</tr>\n",
             to_millis( sim -> gcd_lag ), to_millis( sim -> gcd_lag_stddev ) );
    fprintf( file,
             "\t\t\t\t\t\t\t<tr class=\"left\">\n"
             "\t\t\t\t\t\t\t\t<th>Channel Lag:</th>\n"
             "\t\t\t\t\t\t\t\t<td>%.0f ms ( stddev = %.0f ms )</td>\n"
             "\t\t\t\t\t\t\t</tr>\n",
             to_millis( sim -> channel_lag ), to_millis( sim -> channel_lag_stddev ) );
    fprintf( file,
             "\t\t\t\t\t\t\t<tr class=\"left\">\n"
             "\t\t\t\t\t\t\t\t<th>Queue GCD Reduction:</th>\n"
             "\t\t\t\t\t\t\t\t<td>%.0f ms</td>\n"
             "\t\t\t\t\t\t\t</tr>\n",
             to_millis( sim -> queue_gcd_reduction ) );
  }


  fprintf( file,
           "\t\t\t\t\t\t</table>\n" );

  // Left side charts: dps, gear, timeline, raid events

  fprintf( file,
           "\t\t\t\t<div class=\"charts charts-left\">\n" );
  // Timeline Distribution Chart
  if ( sim -> iterations > 1 && ! sim -> timeline_chart.empty() )
  {
    fprintf( file,
             "\t\t\t\t\t<a href=\"#help-timeline-distribution\" class=\"help\"><img src=\"%s\" alt=\"Timeline Distribution Chart\" /></a>\n",
             sim -> timeline_chart.c_str() );
  }
  // Gear Charts
  int count = ( int ) sim -> gear_charts.size();
  for ( int i=0; i < count; i++ )
  {
    fprintf( file,
             "\t\t\t\t\t<img src=\"%s\" alt=\"Gear Chart\" />\n",
             sim -> gear_charts[ i ].c_str() );
  }
  // Raid Downtime Chart
  if ( ! sim -> downtime_chart.empty() )
  {
    fprintf( file,
             "\t\t\t\t\t<img src=\"%s\" alt=\"Player Downtime Chart\" />\n",
             sim -> downtime_chart.c_str() );
  }

  fprintf( file,
           "\t\t\t\t</div>\n" );

  // Right side charts: dpet
  fprintf( file,
           "\t\t\t\t<div class=\"charts\">\n" );
  count = ( int ) sim -> dpet_charts.size();
  for ( int i=0; i < count; i++ )
  {
    fprintf( file,
             "\t\t\t\t\t<img src=\"%s\" alt=\"DPET Chart\" />\n",
             sim -> dpet_charts[ i ].c_str() );
  }



  fprintf( file,
           "\t\t\t\t</div>\n" );


  // closure
  fprintf( file,
           "\t\t\t\t<div class=\"clear\"></div>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n\n" );
}

// print_html_raid_summary ==================================================

static void print_html_raid_summary( FILE*  file, sim_t* sim )
{
  fprintf( file,
           "\t\t<div id=\"raid-summary\" class=\"section section-open\">\n\n" );
  fprintf( file,
           "\t\t\t<h2 class=\"toggle open\">Raid Summary</h2>\n" );
  fprintf( file,
           "\t\t\t<div class=\"toggle-content\">\n" );
  fprintf( file,
           "\t\t\t<ul class=\"params\">\n" );
  fprintf( file,
           "\t\t\t\t<li><b>Raid Damage:</b> %.0f</li>\n",
           sim -> total_dmg.mean );
  fprintf( file,
           "\t\t\t\t<li><b>Raid DPS:</b> %.0f</li>\n",
           sim -> raid_dps.mean );
  if ( sim -> total_heal.mean > 0 )
  {
    fprintf( file,
             "\t\t\t\t<li><b>Raid Heal:</b> %.0f</li>\n",
             sim -> total_heal.mean );
    fprintf( file,
             "\t\t\t\t<li><b>Raid HPS:</b> %.0f</li>\n",
             sim -> raid_hps.mean );
  }
  fprintf( file,
           "\t\t\t</ul><p>&nbsp;</p>\n" );

  // Left side charts: dps, raid events
  fprintf( file,
           "\t\t\t\t<div class=\"charts charts-left\">\n" );
  int count = ( int ) sim -> dps_charts.size();
  for ( int i=0; i < count; i++ )
  {
    fprintf( file, "\t\t\t\t\t<map id='DPSMAP%d' name='DPSMAP%d'></map>\n", i, i );
    fprintf( file,
             "\t\t\t\t\t<img id='DPSIMG%d' src=\"%s\" alt=\"DPS Chart\" />\n",
             i, sim -> dps_charts[ i ].c_str() );
  }

  if ( ! sim -> raid_events_str.empty() )
  {
    fprintf( file,
             "\t\t\t\t\t<table>\n"
             "\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t<th></th>\n"
             "\t\t\t\t\t\t\t<th class=\"left\">Raid Event List</th>\n"
             "\t\t\t\t\t\t</tr>\n" );
    std::vector<std::string> raid_event_names;
    int num_raid_events = util_t::string_split( raid_event_names, sim -> raid_events_str, "/" );
    for ( int i=0; i < num_raid_events; i++ )
    {
      fprintf( file,
               "\t\t\t\t\t\t<tr" );
      if ( ( i & 1 ) )
      {
        fprintf( file, " class=\"odd\"" );
      }
      fprintf( file, ">\n" );
      fprintf( file,
               "\t\t\t\t\t\t\t<th class=\"right\">%d</th>\n"
               "\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n"
               "\t\t\t\t\t\t</tr>\n",
               i,
               raid_event_names[ i ].c_str() );
    }
    fprintf( file,
             "\t\t\t\t\t</table>\n" );
  }
  fprintf( file,
           "\t\t\t\t</div>\n" );

  // Right side charts: hps
  fprintf( file,
           "\t\t\t\t<div class=\"charts\">\n" );

  count = ( int ) sim -> hps_charts.size();
  for ( int i=0; i < count; i++ )
  {
    fprintf( file, "\t\t\t\t\t<map id='HPSMAP%d' name='HPSMAP%d'></map>\n", i, i );
    fprintf( file,
             "\t\t\t\t\t<img id='HPSIMG%d' src=\"%s\" alt=\"HPS Chart\" />\n",
             i, sim -> hps_charts[ i ].c_str() );
  }
  // RNG chart
  if ( sim -> report_rng )
  {
    fprintf( file,
             "\t\t\t\t\t<ul>\n" );
    for ( int i=0; i < ( int ) sim -> players_by_name.size(); i++ )
    {
      player_t* p = sim -> players_by_name[ i ];
      double range = ( p -> dps.percentile( 0.95 ) - p -> dps.percentile( 0.05 ) ) / 2.0;
      fprintf( file,
               "\t\t\t\t\t\t<li>%s: %.1f / %.1f%%</li>\n",
               p -> name(),
               range,
               p -> dps.mean ? ( range * 100 / p -> dps.mean ) : 0 );
    }
    fprintf( file,
             "\t\t\t\t\t</ul>\n" );
  }

  fprintf( file,
           "\t\t\t\t</div>\n" );

  // closure
  fprintf( file,
           "\t\t\t\t<div class=\"clear\"></div>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n\n" );

}

// print_html_raid_imagemaps ==================================================

static void print_html_raid_imagemap( FILE* file, sim_t* sim, int num, bool dps )
{
  std::vector<player_t*> player_list;

  if ( dps )
    std::copy_if( sim -> players_by_dps.begin(), sim -> players_by_dps.end(),
                  std::back_inserter( player_list ),
                  []( const player_t* p ){ return p -> dps.mean > 0; } );
  else
    std::copy_if( sim -> players_by_hps.begin(), sim -> players_by_hps.end(),
                  std::back_inserter( player_list ),
                  []( const player_t* p ){ return p -> hps.mean > 0; } );

  int start = num * MAX_PLAYERS_PER_CHART;
  int end = std::min( start + MAX_PLAYERS_PER_CHART,
                      static_cast<int>( player_list.size() ) );

  fprintf( file, "\t\t\tn = [" );
  for ( int i=end-1; i >= start; i-- )
  {
    fprintf( file, "\"%s\"", player_list[i] -> name_str.c_str() );
    if ( i != start ) fprintf( file, ", " );
  }
  fprintf( file, "];\n" );

  std::string imgid = ( boost::format( "%sIMG%d" ) % ( dps ? "DPS" : "HPS" ) % num ).str();
  std::string mapid = ( boost::format( "%sMAP%d" ) % ( dps ? "DPS" : "HPS" ) % num ).str();

  fprintf( file,
           "\t\t\tu = document.getElementById('%s').src;\n"
           "\t\t\tgetMap(u, n, function(mapStr) {\n"
           "\t\t\t\tdocument.getElementById('%s').innerHTML += mapStr;\n"
           "\t\t\t\t$j('#%s').attr('usemap','#%s');\n"
           "\t\t\t\t$j('#%s area').click(function(e) {\n"
           "\t\t\t\t\tanchor = $j(this).attr('href');\n"
           "\t\t\t\t\ttarget = $j(anchor).children('h2:first');\n"
           "\t\t\t\t\topen_anchor(target);\n"
           "\t\t\t\t});\n"
           "\t\t\t});\n\n",
           imgid.c_str(), mapid.c_str(), imgid.c_str(), mapid.c_str(), mapid.c_str() );
}

static void print_html_raid_imagemaps( FILE*  file, sim_t* sim )
{

  fprintf( file,
           "\t\t<script type=\"text/javascript\">\n"
           "\t\t\tvar $j = jQuery.noConflict();\n"
           "\t\t\tfunction getMap(url, names, mapWrite) {\n"
           "\t\t\t\t$j.getJSON(url + '&chof=json&callback=?', function(jsonObj) {\n"
           "\t\t\t\t\tvar area = false;\n"
           "\t\t\t\t\tvar chart = jsonObj.chartshape;\n"
           "\t\t\t\t\tvar mapStr = '';\n"
           "\t\t\t\t\tfor (var i = 0; i < chart.length; i++) {\n"
           "\t\t\t\t\t\tarea = chart[i];\n"
           "\t\t\t\t\t\tarea.coords[2] = 523;\n"
           "\t\t\t\t\t\tmapStr += \"\\n  <area name='\" + area.name + \"' shape='\" + area.type + \"' coords='\" + area.coords.join(\",\") + \"' href='#\" + names[i] + \"'  title='\" + names[i] + \"'>\";\n"
           "\t\t\t\t\t}\n"
           "\t\t\t\t\tmapWrite(mapStr);\n"
           "\t\t\t\t});\n"
           "\t\t\t}\n\n" );

  int count = ( int ) sim -> dps_charts.size();
  for ( int i=0; i < count; i++ )
  {
    print_html_raid_imagemap( file, sim, i, true );
  }

  count = ( int ) sim -> hps_charts.size();
  for ( int i=0; i < count; i++ )
  {
    print_html_raid_imagemap( file, sim, i, false );
  }

  fprintf( file, "\t\t</script>\n" );

}

// print_html_scale_factors =================================================

static void print_html_scale_factors( FILE*  file, sim_t* sim )
{
  if ( ! sim -> scaling -> has_scale_factors() ) return;

  if ( sim -> report_precision < 0 )
    sim -> report_precision = 2;

  fprintf( file,
           "\t\t<div id=\"raid-scale-factors\" class=\"section grouped-first\">\n\n"
           "\t\t\t<h2 class=\"toggle\">DPS Scale Factors (dps increase per unit stat)</h2>\n"
           "\t\t\t<div class=\"toggle-content hide\">\n" );

  fprintf( file,
           "\t\t\t\t<table class=\"sc\">\n" );

  std::string buffer;
  int num_players = ( int ) sim -> players_by_name.size();
  player_type prev_type = PLAYER_NONE;

  for ( int i=0; i < num_players; i++ )
  {
    player_t* p = sim -> players_by_name[ i ];

    if ( p -> type != prev_type )
    {
      prev_type = p -> type;

      fprintf( file,
               "\t\t\t\t\t<tr>\n"
               "\t\t\t\t\t\t<th class=\"left small\">Profile</th>\n" );
      for ( stat_type j = STAT_NONE; j < STAT_MAX; j++ )
      {
        if ( sim -> scaling -> stats.get_stat( j ) != 0 )
        {
          fprintf( file,
                   "\t\t\t\t\t\t<th class=\"small\">%s</th>\n",
                   util_t::stat_type_abbrev( j ) );
        }
      }
      fprintf( file,
               "\t\t\t\t\t\t<th class=\"small\">wowhead</th>\n"
               "\t\t\t\t\t\t<th class=\"small\">lootrank</th>\n"
               "\t\t\t\t\t</tr>\n" );
    }

    fprintf( file,
             "\t\t\t\t\t<tr" );
    if ( ( i & 1 ) )
    {
      fprintf( file, " class=\"odd\"" );
    }
    fprintf( file, ">\n" );
    fprintf( file,
             "\t\t\t\t\t\t<td class=\"left small\">%s</td>\n",
             p -> name() );
    for ( stat_type j = STAT_NONE; j < STAT_MAX; j++ )
    {
      if ( sim -> scaling -> stats.get_stat( j ) != 0 )
      {
        if ( p -> scaling.get_stat( j ) == 0 )
        {
          fprintf( file, "\t\t\t\t\t\t<td class=\"small\">-</td>\n" );
        }
        else
        {
          fprintf( file,
                   "\t\t\t\t\t\t<td class=\"small\">%.*f</td>\n",
                   sim -> report_precision,
                   p -> scaling.get_stat( j ) );
        }
      }
    }
    fprintf( file,
             "\t\t\t\t\t\t<td class=\"small\"><a href=\"%s\"> wowhead </a></td>\n"
             "\t\t\t\t\t\t<td class=\"small\"><a href=\"%s\"> lootrank</a></td>\n"
             "\t\t\t\t\t</tr>\n",
             p -> gear_weights_wowhead_link.c_str(),
             p -> gear_weights_lootrank_link.c_str() );
  }
  fprintf( file,
           "\t\t\t\t</table>\n" );
  if ( sim -> iterations < 10000 )
    fprintf( file,
             "\t\t\t\t<div class=\"alert\">\n"
             "\t\t\t\t\t<h3>Warning</h3>\n"
             "\t\t\t\t\t<p>Scale Factors generated using less than 10,000 iterations will vary from run to run.</p>\n"
             "\t\t\t\t</div>\n" );
  fprintf( file,
           "\t\t\t</div>\n"
           "\t\t</div>\n\n" );
}

// print_html_help_boxes ====================================================

static void print_html_help_boxes( FILE*  file, sim_t* sim )
{
  fprintf( file,
           "\t\t<!-- Help Boxes -->\n" );

  fprintf( file,
           "\t\t<div id=\"help-apm\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>APM</h3>\n"
           "\t\t\t\t<p>Average number of actions executed per minute.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-constant-buffs\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Constant Buffs</h3>\n"
           "\t\t\t\t<p>Buffs received prior to combat and present the entire fight.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-count\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Count</h3>\n"
           "\t\t\t\t<p>Average number of times an action is executed per iteration.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-crit\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Crit</h3>\n"
           "\t\t\t\t<p>Average crit damage.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-crit-pct\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Crit%%</h3>\n"
           "\t\t\t\t<p>Percentage of executes that resulted in critical strikes.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-dodge-pct\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Dodge%%</h3>\n"
           "\t\t\t\t<p>Percentage of executes that resulted in dodges.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-dpe\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>DPE</h3>\n"
           "\t\t\t\t<p>Average damage per execution of an individual action.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-dpet\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>DPET</h3>\n"
           "\t\t\t\t<p>Average damage per execute time of an individual action; the amount of damage generated, divided by the time taken to execute the action, including time spent in the GCD.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-dpr\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>DPR</h3>\n"
           "\t\t\t\t<p>Average damage per resource point spent.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-dps\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>DPS</h3>\n"
           "\t\t\t\t<p>Average damage per active player duration.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-dpse\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Effective DPS</h3>\n"
           "\t\t\t\t<p>Average damage per fight duration.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-dps-pct\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>DPS%%</h3>\n"
           "\t\t\t\t<p>Percentage of total DPS contributed by a particular action.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-dynamic-buffs\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Dynamic Buffs</h3>\n"
           "\t\t\t\t<p>Temporary buffs received during combat, perhaps multiple times.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-error\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Error</h3>\n"
           "\t\t\t\t<p>Estimator for the %.2f%% confidence interval.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n",
           sim -> confidence * 100.0 );

  fprintf( file,
           "\t\t<div id=\"help-block-pct\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>G%%</h3>\n"
           "\t\t\t\t<p>Percentage of executes that resulted in blocking blows.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-hit\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Hit</h3>\n"
           "\t\t\t\t<p>Average non-crit damage.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-interval\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Interval</h3>\n"
           "\t\t\t\t<p>Average time between executions of a particular action.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-avg\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Avg</h3>\n"
           "\t\t\t\t<p>Average direct damage per execution.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-miss-pct\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>M%%</h3>\n"
           "\t\t\t\t<p>Percentage of executes that resulted in misses, dodges or parries.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-origin\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Origin</h3>\n"
           "\t\t\t\t<p>The player profile from which the simulation script was generated. The profile must be copied into the same directory as this HTML file in order for the link to work.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-parry-pct\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Parry%%</h3>\n"
           "\t\t\t\t<p>Percentage of executes that resulted in parries.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-range\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Range</h3>\n"
           "\t\t\t\t<p>( dps.percentile( 0.95 ) - dps.percentile( 0.05 ) / 2</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-rps-in\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>RPS In</h3>\n"
           "\t\t\t\t<p>Average resource points generated per second.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-rps-out\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>RPS Out</h3>\n"
           "\t\t\t\t<p>Average resource points consumed per second.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-scale-factors\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Scale Factors</h3>\n"
           "\t\t\t\t<p>DPS gain per unit stat increase except for <b>Hit/Expertise</b> which represent <b>DPS loss</b> per unit stat <b>decrease</b>.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-ticks\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Ticks</h3>\n"
           "\t\t\t\t<p>Average number of periodic ticks per iteration. Spells that do not have a damage-over-time component will have zero ticks.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-ticks-crit\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>T-Crit</h3>\n"
           "\t\t\t\t<p>Average crit tick damage.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-ticks-crit-pct\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>T-Crit%%</h3>\n"
           "\t\t\t\t<p>Percentage of ticks that resulted in critical strikes.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-ticks-hit\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>T-Hit</h3>\n"
           "\t\t\t\t<p>Average non-crit tick damage.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-ticks-miss-pct\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>T-M%%</h3>\n"
           "\t\t\t\t<p>Percentage of ticks that resulted in misses, dodges or parries.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-ticks-uptime\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>UpTime%%</h3>\n"
           "\t\t\t\t<p>Percentage of total time that DoT is ticking on target.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-ticks-avg\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>T-Avg</h3>\n"
           "\t\t\t\t<p>Average damage per tick.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-timeline-distribution\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Timeline Distribution</h3>\n"
           "\t\t\t\t<p>The simulated encounter's duration can vary based on the health of the target and variation in the raid DPS. This chart shows how often the duration of the encounter varied by how much time.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<div id=\"help-fight-length\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Fight Length</h3>\n"
           "\t\t\t\t<p>Fight Length: %.0f<br />\n"
           "\t\t\t\tVary Combat Length: %.2f</p>\n"
           "\t\t\t\t<p>Fight Length is the specified average fight duration. If vary_combat_length is set, the fight length will vary by +/- that portion of the value. See <a href=\"http://code.google.com/p/simulationcraft-swtor/wiki/Options#Combat_Length\" class=\"ext\">Combat Length</a> in the wiki for further details.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n",
           to_seconds( sim -> max_time ),
           sim -> vary_combat_length );

  fprintf( file,
           "\t\t<div id=\"help-waiting\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Waiting</h3>\n"
           "\t\t\t\t<p>This is the percentage of time in which no action can be taken other than autoattacks. This can be caused by resource starvation, lockouts, and timers.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

  fprintf( file,
           "\t\t<!-- End Help Boxes -->\n" );
}

// print_html_styles ====================================================

static void print_html_styles( FILE*  file, sim_t* sim )
{
  // Styles
  // If file is being hosted on simulationcraft.org, link to the local
  // stylesheet; otherwise, embed the styles.
  if ( sim -> hosted_html )
  {
    fprintf( file,
             "\t\t<style type=\"text/css\" media=\"screen\">\n"
             "\t\t\t@import url('http://www.simulationcraft.org/css/swtor_styles.css');\n"
             "\t\t</style>\n"
             "\t\t<style type=\"text/css\" media=\"print\">\n"
             "\t\t  @import url('http://www.simulationcraft.org/css/swtor_styles-print.css');\n"
             "\t\t</style>\n" );
  }
  else if ( sim -> print_styles )
  {
    fprintf( file,
             "\t\t<style type=\"text/css\" media=\"all\">\n"
             "\t\t\t* {border: none;margin: 0;padding: 0; }\n"
             "\t\t\tbody {padding: 5px 25px 25px 25px;font-family: \"Lucida Grande\", Arial, sans-serif;font-size: 14px;background-color: #f9f9f9;color: #333;text-align: center; }\n"
             "\t\t\tp {margin: 1em 0 1em 0; }\n"
             "\t\t\th1, h2, h3, h4, h5, h6 {width: auto;color: #777;margin-top: 1em;margin-bottom: 0.5em; }\n"
             "\t\t\th1, h2 {margin: 0;padding: 2px 2px 0 2px; }\n"
             "\t\t\th1 {font-size: 24px; }\n"
             "\t\t\th2 {font-size: 18px; }\n"
             "\t\t\th3 {margin: 0 0 4px 0;font-size: 16px; }\n"
             "\t\t\th4 {font-size: 12px; }\n"
             "\t\t\th5 {font-size: 10px; }\n"
             "\t\t\ta {color: #666688;text-decoration: none; }\n"
             "\t\t\ta:hover, a:active {color: #333; }\n"
             "\t\t\tul, ol {padding-left: 20px; }\n"
             "\t\t\tul.float, ol.float {padding: 0;margin: 0; }\n"
             "\t\t\tul.float li, ol.float li {display: inline;float: left;padding-right: 6px;margin-right: 6px;list-style-type: none;border-right: 2px solid #eee; }\n"
             "\t\t\t.clear {clear: both; }\n"
             "\t\t\t.hide, .charts span {display: none; }\n"
             "\t\t\t.center {text-align: center; }\n"
             "\t\t\t.float {float: left; }\n"
             "\t\t\t.mt {margin-top: 20px; }\n"
             "\t\t\t.mb {margin-bottom: 20px; }\n"
             "\t\t\t.force-wrap {word-wrap: break-word; }\n"
             "\t\t\t.mono {font-family: \"Lucida Console\", Monaco, monospace;font-size: 12px; }\n"
             "\t\t\t.toggle {cursor: pointer; }\n"
             "\t\t\th2.toggle {padding-left: 16px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAeCAIAAACT/LgdAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAD1SURBVHja7JQ9CoQwFIT9LURQG3vBwyh4XsUjWFtb2IqNCmIhkp1dd9dsfIkeYKdKHl+G5CUTvaqqrutM09Tk2rYtiiIrjuOmaeZ5VqBBEADVGWPTNJVlOQwDyYVhmKap4zgGJp7nJUmCpQoOY2Mv+b6PkkDz3IGevQUOeu6VdxrHsSgK27azLOM5AoVwPqCu6wp1ApXJ0G7rjx5oXdd4YrfQtm3xFJdluUYRBFypghb32ve9jCaOJaPpDpC0tFmg8zzn46nq6/rSd2opAo38IHMXrmeOdgWHACKVFx3Y/c7cjys+JkSP9HuLfYR/Dg1icj0EGACcXZ/44V8+SgAAAABJRU5ErkJggg==) 0 -10px no-repeat; }\n"
             "\t\t\th2.open {margin-bottom: 10px;background-position: 0 9px; }\n"
             "\t\t\th3.toggle {padding-left: 16px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAwAAAAaCAIAAAAMmCo2AAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAEfSURBVHjazJPLjkRAGIXbdSM8ACISvWeDNRYeGuteuL2EdMSGWLrOmdExaCO9nLOq+vPV+S9VRTwej6IoGIYhCOK21zzPfd/f73da07TiRxRFbTkQ4zjKsqyqKoFN27ZhGD6fT5ZlV2IYBkVRXNflOI5ESBAEz/NEUYT5lnAcBwQi307L6aZpoiiqqgprSZJwbCF2EFTXdRAENE37vr8SR2jhAPE8vw0eoVORtw/0j6Fpmi7afEFlWeZ5jhu9grqui+M4SZIrCO8Eg86y7JT7LXx5TODSNL3qDhw6eOeOIyBJEuUj6ZY7mRNmAUvQa4Q+EEiHJizLMgzj3AkeMLBte0vsoCULPHRd//NaUK9pmu/EywDCv0M7+CTzmb4EGADS4Lwj+N6gZgAAAABJRU5ErkJggg==) 0 -11px no-repeat; }\n"
             "\t\t\th3.open {background-position: 0 7px; }\n"
             "\t\t\th4.toggle {margin: 0 0 8px 0;padding-left: 12px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAVCAIAAADw0OikAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAD8SURBVHjavJHLjkRAGIUbRaxd3oAQ8QouifDSFmysPICNIBZ2EhuJuM6ZMdFR3T3LOYtKqk79/3/qKybLsrZteZ5/3DXPs67rxLbtvu+bprluHMexrqumaZZlMdhM05SmaVVVhBBst20zDMN1XRR822erJEnKsmQYxjRNz/M4jsM5ORsKguD7/r7vqHAc5/Sg3+orDsuyGHGd3OxXsY8/9R92XdfjOH60i6IAODzsvQ0sgApw1I0nAZACVGAAPlEU6WigDaLoEcfxleNN8mEY8Id0c2hZFlmWgyDASlefXhiGqqrS0eApihJFkSRJt0nHj/I877rueNGXAAMAKcaTc/aCM/4AAAAASUVORK5CYII=) 0 -8px no-repeat; }\n"
             "\t\t\th4.open {background-position: 0 6px; }\n"
             "\t\t\ta.toggle-details {margin: 0 0 8px 0;padding-left: 12px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAkAAAAXCAYAAADZTWX7AAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAADiSURBVHjaYvz//z/DrFmzGBkYGLqBeG5aWtp1BjTACFIEAkCFZ4AUNxC7ARU+RlbEhMT+BMQaQLwOqEESlyIYMIEqlMenCAQsgLiakKILQNwF47AgSfyH0leA2B/o+EfYTOID4gdA7IusAK4IGk7ngNgPqOABut3I1uUDFfzA5kB4YOIDTAxEgOGtiAUY2vlA2hCIf2KRZwXie6AQPwzEFUAsgUURSGMQEzAqQHFmB8R30BS8BWJXoPw2sJuAjNug2Afi+1AFH4A4DCh+GMXhQIEboHQExKeAOAbI3weTAwgwAIZTQ9CyDvuYAAAAAElFTkSuQmCC) 0 4px no-repeat; }\n"
             "\t\t\ta.open {background-position: 0 -11px; }\n"
             "\t\t\ttd.small a.toggle-details {background-position: 0 2px; }\n"
             "\t\t\ttd.small a.open {background-position: 0 -13px; }\n"
             "\t\t\t#active-help, .help-box {display: none; }\n"
             "\t\t\t#active-help {position: absolute;width: 350px;padding: 3px;background: #fff;z-index: 10; }\n"
             "\t\t\t#active-help-dynamic {padding: 6px 6px 18px 6px;background: #eeeef5;outline: 1px solid #ddd;font-size: 13px; }\n"
             "\t\t\t#active-help .close {position: absolute;right: 10px;bottom: 4px; }\n"
             "\t\t\t.help-box h3 {margin: 0 0 5px 0;font-size: 16px; }\n"
             "\t\t\t.help-box {border: 1px solid #ccc;background-color: #fff;padding: 10px; }\n"
             "\t\t\ta.help {cursor: help; }\n"
             "\t\t\t.section {position: relative;width: 1150px;padding: 8px;margin-left: auto;margin-right: auto;margin-bottom: -1px;border: 1px solid #ccc;background-color: #fff;-moz-box-shadow: 4px 4px 4px #bbb;-webkit-box-shadow: 4px 4px 4px #bbb;box-shadow: 4px 4px 4px #bbb;text-align: left; }\n"
             "\t\t\t.section-open {margin-top: 25px;margin-bottom: 35px;-moz-border-radius: 15px;-khtml-border-radius: 15px;-webkit-border-radius: 15px;border-radius: 15px; }\n"
             "\t\t\t.grouped-first {-moz-border-radius-topright: 15px;-moz-border-radius-topleft: 15px;-khtml-border-top-right-radius: 15px;-khtml-border-top-left-radius: 15px;-webkit-border-top-right-radius: 15px;-webkit-border-top-left-radius: 15px;border-top-right-radius: 15px;border-top-left-radius: 15px; }\n"
             "\t\t\t.grouped-last {-moz-border-radius-bottomright: 15px;-moz-border-radius-bottomleft: 15px;-khtml-border-bottom-right-radius: 15px;-khtml-border-bottom-left-radius: 15px;-webkit-border-bottom-right-radius: 15px;-webkit-border-bottom-left-radius: 15px;border-bottom-right-radius: 15px;border-bottom-left-radius: 15px; }\n"
             "\t\t\t.section .toggle-content {padding: 0; }\n"
             "\t\t\t.player-section .toggle-content {padding-left: 16px;margin-bottom: 20px; }\n"
             "\t\t\t.subsection {background-color: #ccc;width: 1000px;padding: 8px;margin-bottom: 20px;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px;font-size: 12px; }\n"
             "\t\t\t.subsection-small {width: 500px; }\n"
             "\t\t\t.subsection h4 {margin: 0 0 10px 0; }\n"
             "\t\t\t.profile .subsection p {margin: 0; }\n"
             "\t\t\t#raid-summary .toggle-content {padding-bottom: 0px; }\n"
             "\t\t\tul.params {padding: 0;margin: 4px 0 0 6px; }\n"
             "\t\t\tul.params li {float: left;padding: 2px 10px 2px 10px;margin-left: 10px;list-style-type: none;background: #eeeef5;font-family: \"Lucida Grande\", Arial, sans-serif;font-size: 11px; }\n"
             "\t\t\t#masthead ul.params {margin-left: 4px; }\n"
             "\t\t\t#masthead ul.params li {margin-left: 0px;margin-right: 10px; }\n"
             "\t\t\t.player h2 {margin: 0; }\n"
             "\t\t\t.player ul.params {position: relative;top: 2px; }\n"
             "\t\t\t#masthead h2 {margin: 10px 0 5px 0; }\n"
             "\t\t\t#notice {border: 1px solid #ddbbbb;background: #ffdddd;font-size: 12px; }\n"
             "\t\t\t#notice h2 {margin-bottom: 10px; }\n"
             "\t\t\t.alert {width: 800px;padding: 10px;margin: 10px 0 10px 0;background-color: #ddd;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px; }\n"
             "\t\t\t.alert p {margin-bottom: 0px; }\n"
             "\t\t\t.section .toggle-content {padding-left: 18px; }\n"
             "\t\t\t.player > .toggle-content {padding-left: 0; }\n"
             "\t\t\t.toc {float: left;padding: 0; }\n"
             "\t\t\t.toc-wide {width: 560px; }\n"
             "\t\t\t.toc-narrow {width: 375px; }\n"
             "\t\t\t.toc li {margin-bottom: 10px;list-style-type: none; }\n"
             "\t\t\t.toc li ul {padding-left: 10px; }\n"
             "\t\t\t.toc li ul li {margin: 0;list-style-type: none;font-size: 13px; }\n"
             "\t\t\t.charts {float: left;width: 541px;margin-top: 10px; }\n"
             "\t\t\t.charts-left {margin-right: 40px; }\n"
             "\t\t\t.charts img {padding: 8px;margin: 0 auto;margin-bottom: 20px;border: 1px solid #ccc;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px;-moz-box-shadow: inset 1px 1px 4px #ccc;-webkit-box-shadow: inset 1px 1px 4px #ccc;box-shadow: inset 1px 1px 4px #ccc; }\n"
             "\t\t\t.talents div.float {width: auto;margin-right: 50px; }\n"
             "\t\t\ttable.sc {border: 0;background-color: #eee; }\n"
             "\t\t\ttable.sc tr {background-color: #fff; }\n"
             "\t\t\ttable.sc tr.head {background-color: #aaa;color: #fff; }\n"
             "\t\t\ttable.sc tr.odd {background-color: #f3f3f3; }\n"
             "\t\t\ttable.sc th {padding: 2px 4px 4px 4px;text-align: center;background-color: #aaa;color: #fcfcfc; }\n"
             "\t\t\ttable.sc th.small {padding: 2px 2px;font-size: 12px; }\n"
             "\t\t\ttable.sc th a {color: #fff;text-decoration: underline; }\n"
             "\t\t\ttable.sc th a:hover, table.sc th a:active {color: #f1f1ff; }\n"
             "\t\t\ttable.sc td {padding: 2px;text-align: center;font-size: 13px; }\n"
             "\t\t\ttable.sc th.left, table.sc td.left, table.sc tr.left th, table.sc tr.left td {text-align: left; }\n"
             "\t\t\ttable.sc th.right, table.sc td.right, table.sc tr.right th, table.sc tr.right td {text-align: right;padding-right: 4px; }\n"
             "\t\t\ttable.sc th.small {padding: 2px 2px 3px 2px;font-size: 11px; }\n"
             "\t\t\ttable.sc td.small {padding: 2px 2px 3px 2px;font-size: 11px; }\n"
             "\t\t\ttable.sc tr.details td {padding: 0 0 15px 15px;text-align: left;background-color: #fff;font-size: 11px; }\n"
             "\t\t\ttable.sc tr.details td ul {padding: 0;margin: 4px 0 8px 0; }\n"
             "\t\t\ttable.sc tr.details td ul li {clear: both;padding: 2px;list-style-type: none; }\n"
             "\t\t\ttable.sc tr.details td ul li span.label {display: block;padding: 2px;float: left;width: 145px;margin-right: 4px;background: #f3f3f3; }\n"
             "\t\t\ttable.sc tr.details td ul li span.tooltip {display: block;float: left;width: 190px; }\n"
             "\t\t\ttable.sc tr.details td ul li span.tooltip-wider {display: block;float: left;width: 350px; }\n"
             "\t\t\ttable.sc tr.details td div.float {width: 350px; }\n"
             "\t\t\ttable.sc tr.details td div.float h5 {margin-top: 4px; }\n"
             "\t\t\ttable.sc tr.details td div.float ul {margin: 0 0 12px 0; }\n"
             "\t\t\ttable.sc td.filler {background-color: #ccc; }\n"
             "\t\t\ttable.sc .dynamic-buffs tr.details td ul li span.label {width: 120px; }\n"
             "\t\t</style>\n" );
  }
  else
  {
    fprintf( file,
             "\t\t<style type=\"text/css\" media=\"all\">\n"
             "\t\t\t* {border: none;margin: 0;padding: 0; }\n"
             "\t\t\tbody {padding: 5px 25px 25px 25px;font-family: \"Lucida Grande\", Arial, sans-serif;font-size: 14px;background: #261307;color: #FFF;text-align: center; }\n"
             "\t\t\tp {margin: 1em 0 1em 0; }\n"
             "\t\t\th1, h2, h3, h4, h5, h6 {width: auto;color: #FDD017;margin-top: 1em;margin-bottom: 0.5em; }\n"
             "\t\t\th1, h2 {margin: 0;padding: 2px 2px 0 2px; }\n"
             "\t\t\th1 {font-size: 28px;text-shadow: 0 0 3px #FDD017; }\n"
             "\t\t\th2 {font-size: 18px; }\n"
             "\t\t\th3 {margin: 0 0 4px 0;font-size: 16px; }\n"
             "\t\t\th4 {font-size: 12px; }\n"
             "\t\t\th5 {font-size: 10px; }\n"
             "\t\t\ta {color: #FDD017;text-decoration: none; }\n"
             "\t\t\ta:hover, a:active {text-shadow: 0 0 1px #FDD017; }\n"
             "\t\t\tul, ol {padding-left: 20px; }\n"
             "\t\t\tul.float, ol.float {padding: 0;margin: 0; }\n"
             "\t\t\tul.float li, ol.float li {display: inline;float: left;padding-right: 6px;margin-right: 6px;list-style-type: none;border-right: 2px solid #333; }\n"
             "\t\t\t.clear {clear: both; }\n"
             "\t\t\t.hide, .charts span {display: none; }\n"
             "\t\t\t.center {text-align: center; }\n"
             "\t\t\t.float {float: left; }\n"
             "\t\t\t.mt {margin-top: 20px; }\n"
             "\t\t\t.mb {margin-bottom: 20px; }\n"
             "\t\t\t.force-wrap {word-wrap: break-word; }\n"
             "\t\t\t.mono {font-family: \"Lucida Console\", Monaco, monospace;font-size: 12px; }\n"
             "\t\t\t.toggle {cursor: pointer; }\n"
             "\t\t\th2.toggle {padding-left: 18px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAeCAIAAACT/LgdAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAFaSURBVHjaYoz24a9N51aVZ2PADT5//VPS+5WRk51RVZ55STu/tjILVnV//jLEVn1cv/cHMzsb45OX/+48/muizSoiyISm7vvP/yn1n1bs+AE0kYGbkxEiaqDOcn+HyN8L4nD09aRYhCcHRBakDK4UCKwNWM+sEIao+34aoQ6LUiCwMWR9sEMETR12pUBgqs0a5MKOJohdKVYAVMbEQDQYVUq6UhlxZmACIBwNQNJCj/XVQVFjLVbCsfXrN4MwP9O6fn4jTVai3Ap0xtp+fhMcZqN7S06CeU0fPzBxERUCshLM6ycKmOmwEhVYkiJMa/oE0HyJM1zffvj38u0/wkq3H/kZU/nxycu/yIJY8v65678LOj8DszsBt+4+/iuo8COmOnSlh87+Ku///PjFXwIRe2qZkKggE56IZebnZfn56x8nO9P5m/+u3vkNLHBYWdARExMjNxczQIABACK8cxwggQ+oAAAAAElFTkSuQmCC) 0 -10px no-repeat; }\n"
             "\t\t\th2.toggle:hover {text-shadow: 0 0 2px #FDD017; }\n"
             "\t\t\th2.open {margin-bottom: 10px;background-position: 0 9px; }\n"
             "\t\t\t#home-toc h2.open {margin-top: 20px; }\n"
             "\t\t\th3.toggle {padding-left: 16px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAwAAAAaCAYAAACD+r1hAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAD/SURBVHjaYvx7QdyTgYGhE4iVgfg3A3bACsRvgDic8f///wz/Lkq4ADkrgVgIh4bvIMVM+i82M4F4QMYeIBUAxE+wKP4IxCEgxWC1MFGgwGEglQnEj5EUfwbiaKDcNpgA2EnIAOg8VyC1Cog5gDgMZjJODVBNID9xABVvQZdjweHJO9CQwQBYbcAHmBhIBMNBAwta+MtgSx7A+MBpgw6pTloKxBGkaOAB4vlAHEyshu/QRLcQlyZ0DYxQmhuIFwNxICnBygnEy4DYg5R4AOW2D8RqACXxMCA+QYyG20CcAcSHCGUgTmhxEgPEp4gJpetQZ5wiNh7KgXg/vlAACDAAkUxCv8kbXs4AAAAASUVORK5CYII=) 0 -11px no-repeat; }\n"
             "\t\t\th3.toggle:hover {text-shadow: 0 0 2px #CDB007; }\n"
             "\t\t\th3.open {background-position: 0 7px; }\n"
             "\t\t\th4.toggle {margin: 0 0 8px 0;padding-left: 12px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAVCAIAAADw0OikAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAD8SURBVHjavJHLjkRAGIUbRaxd3oAQ8QouifDSFmysPICNIBZ2EhuJuM6ZMdFR3T3LOYtKqk79/3/qKybLsrZteZ5/3DXPs67rxLbtvu+bprluHMexrqumaZZlMdhM05SmaVVVhBBst20zDMN1XRR822erJEnKsmQYxjRNz/M4jsM5ORsKguD7/r7vqHAc5/Sg3+orDsuyGHGd3OxXsY8/9R92XdfjOH60i6IAODzsvQ0sgApw1I0nAZACVGAAPlEU6WigDaLoEcfxleNN8mEY8Id0c2hZFlmWgyDASlefXhiGqqrS0eApihJFkSRJt0nHj/I877rueNGXAAMAKcaTc/aCM/4AAAAASUVORK5CYII=) 0 -8px no-repeat; }\n"
             "\t\t\th4.open {background-position: 0 6px; }\n"
             "\t\t\ta.toggle-details {margin: 0 0 8px 0;padding-left: 12px;background: url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAAoAAAAWCAYAAAD5Jg1dAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAADpSURBVHjaYvx7QdyLgYGhH4ilgfgPAypgAuIvQBzD+P//f4Z/FyXCgJzZQMyHpvAvEMcx6b9YBlYIAkDFAUBqKRBzQRX9AuJEkCIwD6QQhoHOCADiX0D8F4hjkeXgJsIA0OQYIMUGNGkesjgLAyY4AsTM6IIYJuICTAxEggFUyIIULIpA6jkQ/0AxSf8FhoneQKxJjNVxQLwFiGUJKfwOxFJAvBmakgh6Rh+INwCxBDG+NoEq1iEmeK4A8Rt8iQIEpgJxPjThYpjIhKSoFFkRukJQQK8D4gpoCDDgSo+Tgfg0NDNhAIAAAwD5YVPrQE/ZlwAAAABJRU5ErkJggg==) 0 -9px no-repeat; }\n"
             "\t\t\ta.open {background-position: 0 6px; }\n"
             "\t\t\ttd.small a.toggle-details {background-position: 0 -10px; }\n"
             "\t\t\ttd.small a.open {background-position: 0 5px; }\n"
             "\t\t\t#active-help, .help-box {display: none;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px; }\n"
             "\t\t\t#active-help {position: absolute;width: auto;padding: 3px;background: transparent;z-index: 10; }\n"
             "\t\t\t#active-help-dynamic {max-width: 400px;padding: 8px 8px 20px 8px;background: #333;font-size: 13px;text-align: left;border: 1px solid #222;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px;-moz-box-shadow: 4px 4px 10px #000;-webkit-box-shadow: 4px 4px 10px #000;box-shadow: 4px 4px 10px #000; }\n"
             "\t\t\t#active-help .close {display: block;height: 14px;width: 14px;position: absolute;right: 12px;bottom: 7px;background: #000 url(data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAA4AAAAOCAYAAAAfSC3RAAAABGdBTUEAANbY1E9YMgAAABl0RVh0U29mdHdhcmUAQWRvYmUgSW1hZ2VSZWFkeXHJZTwAAAE8SURBVHjafNI/KEVhGMfxc4/j33BZjK4MbkmxnEFiQFcZlMEgZTAZDbIYLEaRUMpCuaU7yCCrJINsJFkUNolSBnKJ71O/V69zb576LOe8v/M+73ueVBzH38HfesQ5bhGiFR2o9xdFidAm1nCFop7VoAvTGHILQy9kCw+0W9F7/o4jHPs7uOAyZrCL0aC05rCgd/uu1Rus4g6VKKAa2wrNKziCPTyhx4InClkt4RNbardFoWG3E3WKCwteJ9pawSt28IEcDr33b7gPy9ysVRZf2rWpzPso0j/yax2T6EazzlynTgL9z2ykBe24xAYm0I8zqdJF2cUtog9tFsxgFs8YR68uwFVeLec1DDYEaXe+MZ1pIBFyZe3WarJKRq5CV59Wiy9IoQGDmPpvVq3/Tg34gz5mR2nUUPzWjwADAFypQitBus+8AAAAAElFTkSuQmCC) no-repeat; }\n"
             "\t\t\t#active-help .close:hover {background-color: #1d1d1d; }\n"
             "\t\t\t.help-box h3 {margin: 0 0 12px 0;font-size: 14px;color: #C68E17; }\n"
             "\t\t\t.help-box p {margin: 0 0 10px 0; }\n"
             "\t\t\t.help-box {background-color: #000;padding: 10px; }\n"
             "\t\t\ta.help {color: #C68E17;cursor: help; }\n"
             "\t\t\ta.help:hover {text-shadow: 0 0 1px #C68E17; }\n"
             "\t\t\t.section {position: relative;width: 1150px;padding: 8px;margin-left: auto;margin-right: auto;margin-bottom: -1px;border: 0;-moz-box-shadow: 0px 0px 8px #FDD017;-webkit-box-shadow: 0px 0px 8px #FDD017;box-shadow: 0px 0px 8px #FDD017;color: #fff;background-color: #000;text-align: left; }\n"
             "\t\t\t.section-open {margin-top: 25px;margin-bottom: 35px;-moz-border-radius: 15px;-khtml-border-radius: 15px;-webkit-border-radius: 15px;border-radius: 15px; }\n"
             "\t\t\t.grouped-first {-moz-border-radius-topright: 15px;-moz-border-radius-topleft: 15px;-khtml-border-top-right-radius: 15px;-khtml-border-top-left-radius: 15px;-webkit-border-top-right-radius: 15px;-webkit-border-top-left-radius: 15px;border-top-right-radius: 15px;border-top-left-radius: 15px; }\n"
             "\t\t\t.grouped-last {-moz-border-radius-bottomright: 15px;-moz-border-radius-bottomleft: 15px;-khtml-border-bottom-right-radius: 15px;-khtml-border-bottom-left-radius: 15px;-webkit-border-bottom-right-radius: 15px;-webkit-border-bottom-left-radius: 15px;border-bottom-right-radius: 15px;border-bottom-left-radius: 15px; }\n"
             "\t\t\t.section .toggle-content {padding: 0; }\n"
             "\t\t\t.player-section .toggle-content {padding-left: 16px; }\n"
             "\t\t\t#home-toc .toggle-content {margin-bottom: 20px; }\n"
             "\t\t\t.subsection {background-color: #333;width: 1000px;padding: 8px;margin-bottom: 20px;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px;font-size: 12px; }\n"
             "\t\t\t.subsection-small {width: 500px; }\n"
             "\t\t\t.subsection h4 {margin: 0 0 10px 0;color: #fff; }\n"
             "\t\t\t.profile .subsection p {margin: 0; }\n"
             "\t\t\t#raid-summary .toggle-content {padding-bottom: 0px; }\n"
             "\t\t\tul.params {padding: 0;margin: 4px 0 0 6px; }\n"
             "\t\t\tul.params li {float: left;padding: 2px 10px 2px 10px;margin-left: 10px;list-style-type: none;background: #2f2f2f;color: #ddd;font-family: \"Lucida Grande\", Arial, sans-serif;font-size: 11px;-moz-border-radius: 8px;-khtml-border-radius: 8px;-webkit-border-radius: 8px;border-bottom-radius: 8px; }\n"
             "\t\t\tul.params li.linked:hover {background: #393939; }\n"
             "\t\t\tul.params li a {color: #ddd; }\n"
             "\t\t\tul.params li a:hover {text-shadow: none; }\n"
             "\t\t\t.player h2 {margin: 0; }\n"
             "\t\t\t.player ul.params {position: relative;top: 2px; }\n"
             "\t\t\t#masthead {height: auto;padding-bottom: 35px;border: 0;-moz-border-radius: 15px;-khtml-border-radius: 15px;-webkit-border-radius: 15px;border-radius: 15px;-moz-box-shadow: 0px 0px 8px #FDD017;-webkit-box-shadow: 0px 0px 8px #FDD017;box-shadow: 0px 0px 8px #FDD017;text-align: left;color: #FDD017;background: #000 url(data:image/jpg;base64,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IHhtbG5zOnRpZmY9Imh0dHA6Ly9ucy5hZG9iZS5jb20vdGlmZi8xLjAvIiB4bWxuczpleGlmPSJodHRwOi8vbnMuYWRvYmUuY29tL2V4aWYvMS4wLyIgeG1sbnM6cGhvdG9zaG9wPSJodHRwOi8vbnMuYWRvYmUuY29tL3Bob3Rvc2hvcC8xLjAvIiBkYzpmb3JtYXQ9ImltYWdlL2pwZWciIHhtcDpDcmVhdG9yVG9vbD0iQWRvYmUgUGhvdG9zaG9wIENTMyBXaW5kb3dzIiB4bXA6Q3JlYXRlRGF0ZT0iMjAxMi0wNy0yOFQxNDo1ODoyNC0wNzowMCIgeG1wOk1vZGlmeURhdGU9IjIwMTItMDctMjlUMTc6MzI6NTArMTA6MDAiIHhtcDpNZXRhZGF0YURhdGU9IjIwMTItMDctMjhUMTQ6NTg6MjQtMDc6MDAiIHhtcE1NOkRvY3VtZW50SUQ9InV1aWQ6QTMyN0RENDRGRkQ4RTExMUJEMEVDQjFCRDEzRjQ2NTAiIHhtcE1NOkluc3RhbmNlSUQ9InV1aWQ6QTQyN0RENDRGRkQ4RTExMUJEMEVDQjFCRDEzRjQ2NTAiIHRpZmY6WFJlc29sdXRpb249IjcyMDAwMC8xMDAwMCIgdGlmZjpZUmVzb2x1dGlvbj0iNzIwMDAwLzEwMDAwIiB0aWZmOlJlc29sdXRpb25Vbml0PSIyIiB0aWZmOk5hdGl2ZURpZ2VzdD0iMjU2LDI1NywyNTgsMjU5LDI2MiwyNzQsMjc3LDI4NCw1MzAsNTMxLDI4MiwyODMsMjk2LDMwMSwzMTgsMzE5LDUyOSw1MzIsMzA2LDI3MCwyNzEsMjcyLDMwNSwzMTUsMzM0MzI7NzgyNDI1NEU1NzdDOTZDODE4QUY4N0U5MUVDOTQyRjQiIGV4aWY6Q29sb3JTcGFjZT0iMSIgZXhpZjpOYXRpdmVEaWdlc3Q9IjM2ODY0LDQwOTYwLDQwOTYxLDM3MTIxLDM3MTIyLDQwOTYyLDQwOTYzLDM3NTEwLDQwOTY0LDM2ODY3LDM2ODY4LDMzNDM0LDMzNDM3LDM0ODUwLDM0ODUyLDM0ODU1LDM0ODU2LDM3Mzc3LDM3Mzc4LDM3Mzc5LDM3MzgwLDM3MzgxLDM3MzgyLDM3MzgzLDM3Mzg0LDM3Mzg1LDM3Mzg2LDM3Mzk2LDQxNDgzLDQxNDg0LDQxNDg2LDQxNDg3LDQxNDg4LDQxNDkyLDQxNDkzLDQxNDk1LDQxNzI4LDQxNzI5LDQxNzMwLDQxOTg1LDQxOTg2LDQxOTg3LDQxOTg4LDQxOTg5LDQxOTkwLDQxOTkxLDQxOTkyLDQxOTkzLDQxOTk0LDQxOTk1LDQxOTk2LDQyMDE2LDAsMiw0LDUsNiw3LDgsOSwxMCwxMSwxMiwxMywxNCwxNSwxNiwxNywxOCwyMCwyMiwyMywyNCwyNSwyNiwyNywyOCwzMDswN0VDQkY1OTU1QUQ3QzYxQUM3MzgxRTdDRUU5QUIwOCIgZXhpZjpEYXRlVGltZU9yaWdpbmFsPSIyMDEyLTA3LTI5VDE3OjI5OjMwKzEwOjAwIiBwaG90b3Nob3A6Q29sb3JNb2RlPSIzIiBwaG90b3Nob3A6SUNDUHJvZmlsZT0ic1JHQiBJRUM2MTk2Ni0yLjEiIHBob3Rvc2hvcDpIaXN0b3J5PSIiPiA8ZGM6Y3JlYXRvcj4gPHJkZjpTZXE+IDxyZGY6bGk+UGljYXNhPC9yZGY6bGk+IDwvcmRmOlNlcT4gPC9kYzpjcmVhdG9yPiA8eG1wTU06RGVyaXZlZEZyb20gc3RSZWY6aW5zdGFuY2VJRD0idXVpZDowRENGNzc5RUZFRDhFMTExQkQwRUNCMUJEMTNGNDY1MCIgc3RSZWY6ZG9jdW1lbnRJRD0idXVpZDo1M0FGMzY1QkY2RDhFMTExQkQwRUNCMUJEMTNGNDY1MCIvPiA8L3JkZjpEZXNjcmlwdGlvbj4gPC9yZGY6UkRGPiA8L3g6eG1wbWV0YT4gICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICAgICA8P3hwYWNrZXQgZW5kPSJ3Ij8+/+0t5lBob3Rvc2hvcCAzLjAAOEJJTQQEAAAAAAA+HAEAAAIABBwBWgADGyVHHAIAAAIABBwCNwAIMjAxMjA3MjkcAjwACzE3MjkzMCsxMDAwHAJQAAZQaWNhc2E4QklNBCUAAAAAABAyK9UVxoRNjBTjLaXn33SeOEJJTQPtAAAAAAAQAEgAAAABAAEASAAAAAEAAThCSU0EJgAAAAAADgAAAAAAAAAAAAA/gAAAOEJJTQQNAAAAAAAEAAAAeDhCSU0EGQAAAAAABAAAAB44QklNA/MAAAAAAAkAAAAAAAAAAAEAOEJJTQQKAAAAAAABAAA4QklNJxAAAAAAAAoAAQAAAAAAAAACOEJJTQP1AAAAAABIAC9mZgABAGxmZgAGAAAAAAABAC9mZgABAKGZmgAGAAAAAAABADIAAAABAFoAAAAGAAAAAAABADUAAAABAC0AAAAGAAAAAAABOEJJTQP4AAAAAABwAAD/////////////////////////////A+gAAAAA/////////////////////////////wPoAAAAAP////////////////////////////8D6AAAAAD/////////////////////////////A+gAADhCSU0ECAAAAAAAEAAAAAEAAAJAAAACQAAAAAA4QklNBB4AAAAAAAQAAAAAOEJJTQQaAAAAAANJAAAABgAAAAAAAAAAAAABDwAAAUUAAAAKAHMAaQBtAGMAdABvAHIAMwAyADUAAAABAAAAAAAAAAAAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAUUAAAEPAAAAAAAAAAAAAAAAAAAAAAEAAAAAAAAAAAAAAAAAAAAAAAAAEAAAAAEAAAAAAABudWxsAAAAAgAAAAZib3VuZHNPYmpjAAAAAQAAAAAAAFJjdDEAAAAEAAAAAFRvcCBsb25nAAAAAAAAAABMZWZ0bG9uZwAAAAAAAAAAQnRvbWxvbmcAAAEPAAAAAFJnaHRsb25nAAABRQAAAAZzbGljZXNWbExzAAAAAU9iamMAAAABAAAAAAAFc2xpY2UAAAASAAAAB3NsaWNlSURsb25nAAAAAAAAAAdncm91cElEbG9uZwAAAAAAAAAGb3JpZ2luZW51bQAAAAxFU2xpY2VPcmlnaW4AAAANYXV0b0dlbmVyYXRlZAAAAABUeXBlZW51bQAAAApFU2xpY2VUeXBlAAAAAEltZyAAAAAGYm91bmRzT2JqYwAAAAEAAAAAAABSY3QxAAAABAAAAABUb3AgbG9uZwAAAAAAAAAATGVmdGxvbmcAAAAAAAAAAEJ0b21sb25nAAABDwAAAABSZ2h0bG9u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) 27px 13px no-repeat scroll; }\n"
             "\t\t\t#masthead h1 {margin: 50px 0 0 285px; }\n"
             "\t\t\t#home #masthead h1 {margin-top: 0px; }\n"
             "\t\t\t#masthead h2 {margin: 10px 0 0 285px; }\n"
             "\t\t\t#masthead ul.params {margin: 20px 0 0 275px; }\n"
             "\t\t\t#masthead p {color: #fff;margin: 20px 20px 0 20px; }\n"
             "\t\t\t#notice {font-size: 12px;-moz-box-shadow: 0px 0px 8px #E41B17;-webkit-box-shadow: 0px 0px 8px #E41B17;box-shadow: 0px 0px 8px #E41B17; }\n"
             "\t\t\t#notice h2 {margin-bottom: 10px; }\n"
             "\t\t\t.alert {width: 800px;padding: 10px;margin: 10px 0 10px 0;background-color: #333;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px;-moz-box-shadow: inset 0px 0px 6px #C11B17;-webkit-box-shadow: inset 0px 0px 6px #C11B17;box-shadow: inset 0px 0px 6px #C11B17; }\n"
             "\t\t\t.alert p {margin-bottom: 0px; }\n"
             "\t\t\t.section .toggle-content {padding-left: 18px; }\n"
             "\t\t\t.player > .toggle-content {padding-left: 0; }\n"
             "\t\t\t.toc {float: left;padding: 0; }\n"
             "\t\t\t.toc-wide {width: 560px; }\n"
             "\t\t\t.toc-narrow {width: 375px; }\n"
             "\t\t\t.toc li {margin-bottom: 10px;list-style-type: none; }\n"
             "\t\t\t.toc li ul {padding-left: 10px; }\n"
             "\t\t\t.toc li ul li {margin: 0;list-style-type: none;font-size: 13px; }\n"
             "\t\t\t.charts {float: left;width: 541px;margin-top: 10px; }\n"
             "\t\t\t.charts-left {margin-right: 40px; }\n"
             "\t\t\t.charts img {background-color: #333;padding: 5px;margin-bottom: 20px;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px; }\n"
             "\t\t\t.talents div.float {width: auto;margin-right: 50px; }\n"
             "\t\t\ttable.sc {background-color: #333;padding: 4px 2px 2px 2px;margin: 10px 0 20px 0;-moz-border-radius: 6px;-khtml-border-radius: 6px;-webkit-border-radius: 6px;border-radius: 6px; }\n"
             "\t\t\ttable.sc tr {color: #fff;background-color: #1a1a1a; }\n"
             "\t\t\ttable.sc tr.head {background-color: #aaa;color: #fff; }\n"
             "\t\t\ttable.sc tr.odd {background-color: #222; }\n"
             "\t\t\ttable.sc th {padding: 2px 4px 4px 4px;text-align: center;background-color: #333;color: #fff; }\n"
             "\t\t\ttable.sc td {padding: 2px;text-align: center;font-size: 13px; }\n"
             "\t\t\ttable.sc th.left, table.sc td.left, table.sc tr.left th, table.sc tr.left td {text-align: left; }\n"
             "\t\t\ttable.sc th.right, table.sc td.right, table.sc tr.right th, table.sc tr.right td {text-align: right;padding-right: 4px; }\n"
             "\t\t\ttable.sc th.small {padding: 2px 2px 3px 2px;font-size: 11px; }\n"
             "\t\t\ttable.sc td.small {padding: 2px 2px 3px 2px;font-size: 11px; }\n"
             "\t\t\ttable.sc tr.details td {padding: 0 0 15px 15px;text-align: left;background-color: #333;font-size: 11px; }\n"
             "\t\t\ttable.sc tr.details td ul {padding: 0;margin: 4px 0 8px 0; }\n"
             "\t\t\ttable.sc tr.details td ul li {clear: both;padding: 2px;list-style-type: none; }\n"
             "\t\t\ttable.sc tr.details td ul li span.label {display: block;padding: 2px;float: left;width: 145px;margin-right: 4px;background: #222; }\n"
             "\t\t\ttable.sc tr.details td ul li span.tooltip {display: block;float: left;width: 190px; }\n"
             "\t\t\ttable.sc tr.details td ul li span.tooltip-wider {display: block;float: left;width: 350px; }\n"
             "\t\t\ttable.sc tr.details td div.float {width: 350px; }\n"
             "\t\t\ttable.sc tr.details td div.float h5 {margin-top: 4px; }\n"
             "\t\t\ttable.sc tr.details td div.float ul {margin: 0 0 12px 0; }\n"
             "\t\t\ttable.sc td.filler {background-color: #333; }\n"
             "\t\t\ttable.sc .dynamic-buffs tr.details td ul li span.label {width: 120px; }\n"
             "\t\t\ttr.details td table.details {padding: 0px;margin: 5px 0 10px 0; }\n"
             "\t\t\ttr.details td table.details tr th {background-color: #222; }\n"
             "\t\t\ttr.details td table.details tr td {background-color: #2d2d2d; }\n"
             "\t\t\ttr.details td table.details tr.odd td {background-color: #292929; }\n"
             "\t\t\ttr.details td table.details tr td {padding: 1px 3px 1px 3px; }\n"
             "\t\t\ttr.details td table.details tr td.right {text-align: right; }\n"
             "\t\t\t.player-thumbnail {float: right;margin: 8px;border-radius: 12px;-moz-border-radius: 12px;-webkit-border-radius: 12px;-khtml-border-radius: 12px; }\n"
             "\t\t</style>\n" );
  }
}

// print_html_masthead ====================================================

static void print_html_masthead( FILE*  file, sim_t* sim )
{
  // Begin masthead section
  fprintf( file,
           "\t\t<div id=\"masthead\" class=\"section section-open\">\n\n" );

  fprintf( file,
           "\t\t\t<h1><a href=\"http://code.google.com/p/simulationcraft-swtor/\">SimulationCraft %s-%s</a></h1>\n"
           "\t\t\t<h2>for Star Wars: The Old Republic %s %s</h2>\n\n",
           SC_MAJOR_VERSION, SC_MINOR_VERSION, sim -> ptr ? SWTOR_VERSION_PTR : SWTOR_VERSION_LIVE, ( sim -> ptr ? "PTR" : "Live" ) );

  time_t rawtime;
  time ( &rawtime );

  fprintf( file,
           "\t\t\t<ul class=\"params\">\n" );
  fprintf( file,
           "\t\t\t\t<li><b>Timestamp:</b> %s</li>\n",
           ctime( &rawtime ) );
  fprintf( file,
           "\t\t\t\t<li><b>Iterations:</b> %d</li>\n",
           sim -> iterations );

  if ( sim -> vary_combat_length > 0.0 )
  {
    timespan_t min_length = sim -> max_time * ( 1 - sim -> vary_combat_length );
    timespan_t max_length = sim -> max_time * ( 1 + sim -> vary_combat_length );
    fprintf( file,
             "\t\t\t\t<li class=\"linked\"><a href=\"#help-fight-length\" class=\"help\"><b>Fight Length:</b> %.0f - %.0f</a></li>\n",
             to_seconds( min_length ),
             to_seconds( max_length ) );
  }
  else
  {
    fprintf( file,
             "\t\t\t\t<li><b>Fight Length:</b> %.0f</li>\n",
             to_seconds( sim -> max_time ) );
  }
  fprintf( file,
           "\t\t\t\t<li><b>Fight Style:</b> %s</li>\n",
           sim -> fight_style.c_str() );
  fprintf( file,
           "\t\t\t</ul>\n" );
  fprintf( file,
           "\t\t\t<div class=\"clear\"></div>\n\n"
           "\t\t</div>\n\n" );
  // End masthead section
}

} // ANONYMOUS NAMESPACE ====================================================

// report_t::print_html =====================================================
void report_t::print_html( sim_t* sim )
{
  int num_players = ( int ) sim -> players_by_name.size();

  if ( num_players == 0 ) return;
  if ( sim -> simulation_length.mean == 0 ) return;
  if ( sim -> html_file_str.empty() ) return;

  FILE* file = fopen( sim -> html_file_str.c_str(), "w" );
  if ( ! file )
  {
    sim -> errorf( "Unable to open html file '%s'\n", sim -> html_file_str.c_str() );
    return;
  }

  fprintf( file,
           "<!DOCTYPE html>\n\n" );
  fprintf( file,
           "<html>\n\n" );

  fprintf( file,
           "\t<head>\n\n" );
  fprintf( file,
           "\t\t<title>Simulationcraft Results</title>\n\n" );
  fprintf( file,
           "\t\t<meta http-equiv=\"Content-Type\" content=\"text/html; charset=UTF-8\" />\n\n" );

  print_html_styles( file, sim );

  fprintf( file,
           "\t</head>\n\n" );

  fprintf( file,
           "\t<body>\n\n" );

  if ( ! sim -> error_list.empty() )
  {
    fprintf( file,
             "\t\t<pre>\n" );
    size_t num_errors = sim -> error_list.size();
    for ( size_t i=0; i < num_errors; i++ )
      fprintf( file,
               "      %s\n", sim -> error_list[ i ].c_str() );
    fprintf( file,
             "\t\t</pre>\n\n" );
  }

  // Prints div wrappers for help popups
  fprintf( file,
           "\t\t<div id=\"active-help\">\n"
           "\t\t\t<div id=\"active-help-dynamic\">\n"
           "\t\t\t\t<div class=\"help-box\"></div>\n"
           "\t\t\t\t<a href=\"#\" class=\"close\"><span class=\"hide\">close</span></a>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n\n" );

  print_html_masthead( file, sim );

#if SC_BETA
  fprintf( file,
           "\t\t<div id=\"notice\" class=\"section section-open\">\n" );
  fprintf( file,
           "\t\t\t<h2>Beta Release</h2>\n" );
  int ii = 0;
  if ( beta_warnings[ 0 ] )
    fprintf( file,
             "\t\t\t<ul>\n" );
  while ( beta_warnings[ ii ] )
  {
    fprintf( file,
             "\t\t\t\t<li>%s</li>\n",
             beta_warnings[ ii ] );
    ii++;
  }
  if ( beta_warnings[ 0 ] )
    fprintf( file,
             "\t\t\t</ul>\n" );
  fprintf( file,
           "\t\t</div>\n\n" );
#endif

  if ( num_players > 1 )
  {
    print_html_contents( file, sim );
  }

  if ( num_players > 1 )
  {
    print_html_raid_summary( file, sim );
    print_html_scale_factors( file, sim );
  }

  // Players
  for ( int i=0; i < num_players; i++ )
  {
    report_t::print_html_player( file, sim -> players_by_name[ i ], i );

    // Pets
    if ( sim -> report_pets_separately )
    {
      for ( pet_t* pet = sim -> players_by_name[ i ] -> pet_list; pet; pet = pet -> next_pet )
      {
        if ( pet -> summoned )
          report_t::print_html_player( file, pet, 1 );
      }
    }
  }

  // Sim Summary
  print_html_sim_summary( file, sim );

  // Report Targets
  if ( sim -> report_targets )
  {
    for ( int i=0; i < ( int ) sim -> targets_by_name.size(); i++ )
    {
      report_t::print_html_player( file, sim -> targets_by_name[ i ], i );

      // Pets
      if ( sim -> report_pets_separately )
      {
        for ( pet_t* pet = sim -> targets_by_name[ i ] -> pet_list; pet; pet = pet -> next_pet )
        {
          //if ( pet -> summoned )
          report_t::print_html_player( file, pet, 1 );
        }
      }
    }
  }

  // Help Boxes
  print_html_help_boxes( file, sim );

  // jQuery
  fprintf ( file,
            "\t\t<script type=\"text/javascript\" src=\"http://ajax.googleapis.com/ajax/libs/jquery/1.7.1/jquery.min.js\"></script>\n" );

  // Toggles, image load-on-demand, etc. Load from simulationcraft.org if
  // hosted_html=1, otherwise embed
  if ( sim -> hosted_html )
  {
    fprintf( file,
             "\t\t<script type=\"text/javascript\" src=\"http://www.simulationcraft.org/js/ga.js\"></script>\n"
             "\t\t<script type=\"text/javascript\" src=\"http://www.simulationcraft.org/js/rep.js\"></script>\n"
             "\t\t<script type=\"text/javascript\" src=\"http://i4.knocdn.com/assets/tooltips.js\"></script>\n" );
  }
  else
  {
    fprintf( file,
             "\t\t<script type=\"text/javascript\">\n"
             "\t\t\tfunction load_images(containers) {\n"
             "\t\t\t\tvar $j = jQuery.noConflict();\n"
             "\t\t\t\tcontainers.each(function() {\n"
             "\t\t\t\t\t$j(this).children('span').each(function() {\n"
             "\t\t\t\t\t\tvar j = jQuery.noConflict();\n"
             "\t\t\t\t\t\timg_class = $j(this).attr('class');\n"
             "\t\t\t\t\t\timg_alt = $j(this).attr('title');\n"
             "\t\t\t\t\t\timg_src = $j(this).html().replace(/&amp;/g, '&');\n"
             "\t\t\t\t\t\tvar img = new Image();\n"
             "\t\t\t\t\t\t$j(img).attr('class', img_class);\n"
             "\t\t\t\t\t\t$j(img).attr('src', img_src);\n"
             "\t\t\t\t\t\t$j(img).attr('alt', img_alt);\n"
             "\t\t\t\t\t\t$j(this).replaceWith(img);\n"
             "\t\t\t\t\t\t$j(this).load();\n"
             "\t\t\t\t\t});\n"
             "\t\t\t\t});\n"
             "\t\t\t}\n"
             "\t\t\tfunction open_anchor(anchor) {\n"
             "\t\t\t\tvar img_id = '';\n"
             "\t\t\t\tvar src = '';\n"
             "\t\t\t\tvar target = '';\n"
             "\t\t\t\tanchor.addClass('open');\n"
             "\t\t\t\tvar section = anchor.parent('.section');\n"
             "\t\t\t\tsection.addClass('section-open');\n"
             "\t\t\t\tsection.removeClass('grouped-first');\n"
             "\t\t\t\tsection.removeClass('grouped-last');\n"
             "\t\t\t\tif (!(section.next().hasClass('section-open'))) {\n"
             "\t\t\t\t\tsection.next().addClass('grouped-first');\n"
             "\t\t\t\t}\n"
             "\t\t\t\tif (!(section.prev().hasClass('section-open'))) {\n"
             "\t\t\t\t\tsection.prev().addClass('grouped-last');\n"
             "\t\t\t\t}\n"
             "\t\t\t\tanchor.next('.toggle-content').show(150);\n"
             "\t\t\t\tchart_containers = anchor.next('.toggle-content').find('.charts');\n"
             "\t\t\t\tload_images(chart_containers);\n"
             "\t\t\t\tsetTimeout(\"var ypos=0;var e=document.getElementById('\" + section.attr('id') + \"');while( e != null ) {ypos += e.offsetTop;e = e.offsetParent;}window.scrollTo(0,ypos);\", 500);\n"
             "\t\t\t}\n"
             "\t\t\tjQuery.noConflict();\n"
             "\t\t\tjQuery(document).ready(function($) {\n"
             "\t\t\t\tvar chart_containers = false;\n"
             "\t\t\t\tvar anchor_check = document.location.href.split('#');\n"
             "\t\t\t\tif (anchor_check.length > 1) {\n"
             "\t\t\t\t\tvar anchor = anchor_check[anchor_check.length - 1];\n"
             "\t\t\t\t}\n"
             "\t\t\t\t$('a.ext').mouseover(function() {\n"
             "\t\t\t\t\t$(this).attr('target', '_blank');\n"
             "\t\t\t\t});\n"
             "\t\t\t\t$('.toggle').click(function(e) {\n"
             "\t\t\t\t\tvar img_id = '';\n"
             "\t\t\t\t\tvar src = '';\n"
             "\t\t\t\t\tvar target = '';\n"
             "\t\t\t\t\te.preventDefault();\n"
             "\t\t\t\t\t$(this).toggleClass('open');\n"
             "\t\t\t\t\tvar section = $(this).parent('.section');\n"
             "\t\t\t\t\tif (section.attr('id') != 'masthead') {\n"
             "\t\t\t\t\t\tsection.toggleClass('section-open');\n"
             "\t\t\t\t\t}\n"
             "\t\t\t\t\tif (section.attr('id') != 'masthead' && section.hasClass('section-open')) {\n"
             "\t\t\t\t\t\tsection.removeClass('grouped-first');\n"
             "\t\t\t\t\t\tsection.removeClass('grouped-last');\n"
             "\t\t\t\t\t\tif (!(section.next().hasClass('section-open'))) {\n"
             "\t\t\t\t\t\t\tsection.next().addClass('grouped-first');\n"
             "\t\t\t\t\t\t}\n"
             "\t\t\t\t\t\tif (!(section.prev().hasClass('section-open'))) {\n"
             "\t\t\t\t\t\t\tsection.prev().addClass('grouped-last');\n"
             "\t\t\t\t\t\t}\n"
             "\t\t\t\t\t} else if (section.attr('id') != 'masthead') {\n"
             "\t\t\t\t\t\tif (section.hasClass('final') || section.next().hasClass('section-open')) {\n"
             "\t\t\t\t\t\t\tsection.addClass('grouped-last');\n"
             "\t\t\t\t\t\t} else {\n"
             "\t\t\t\t\t\t\tsection.next().removeClass('grouped-first');\n"
             "\t\t\t\t\t\t}\n"
             "\t\t\t\t\t\tif (section.prev().hasClass('section-open')) {\n"
             "\t\t\t\t\t\t\tsection.addClass('grouped-first');\n"
             "\t\t\t\t\t\t} else {\n"
             "\t\t\t\t\t\t\tsection.prev().removeClass('grouped-last');\n"
             "\t\t\t\t\t\t}\n"
             "\t\t\t\t\t}\n"
             "\t\t\t\t\t$(this).next('.toggle-content').toggle(150);\n"
             "\t\t\t\t\t$(this).prev('.toggle-thumbnail').toggleClass('hide');\n"
             "\t\t\t\t\tchart_containers = $(this).next('.toggle-content').find('.charts');\n"
             "\t\t\t\t\tload_images(chart_containers);\n"
             "\t\t\t\t});\n"
             "\t\t\t\t$('.toggle-details').click(function(e) {\n"
             "\t\t\t\t\te.preventDefault();\n"
             "\t\t\t\t\t$(this).toggleClass('open');\n"
             "\t\t\t\t\t$(this).parents().next('.details').toggleClass('hide');\n"
             "\t\t\t\t});\n"
             "\t\t\t\t$('.toggle-db-details').click(function(e) {\n"
             "\t\t\t\t\te.preventDefault();\n"
             "\t\t\t\t\t$(this).toggleClass('open');\n"
             "\t\t\t\t\t$(this).parent().next('.toggle-content').toggle(150);\n"
             "\t\t\t\t});\n"
             "\t\t\t\t$('.help').click(function(e) {\n"
             "\t\t\t\t\te.preventDefault();\n"
             "\t\t\t\t\tvar target = $(this).attr('href') + ' .help-box';\n"
             "\t\t\t\t\tvar content = $(target).html();\n"
             "\t\t\t\t\t$('#active-help-dynamic .help-box').html(content);\n"
             "\t\t\t\t\t$('#active-help .help-box').show();\n"
             "\t\t\t\t\tvar t = e.pageY - 20;\n"
             "\t\t\t\t\tvar l = e.pageX - 20;\n"
             "\t\t\t\t\t$('#active-help').css({top:t,left:l});\n"
             "\t\t\t\t\t$('#active-help').show(250);\n"
             "\t\t\t\t});\n"
             "\t\t\t\t$('#active-help a.close').click(function(e) {\n"
             "\t\t\t\t\te.preventDefault();\n"
             "\t\t\t\t\t$('#active-help').toggle(250);\n"
             "\t\t\t\t});\n"
             "\t\t\t\tif (anchor) {\n"
             "\t\t\t\t\tanchor = '#' + anchor;\n"
             "\t\t\t\t\ttarget = $(anchor).children('h2:first');\n"
             "\t\t\t\t\topen_anchor(target);\n"
             "\t\t\t\t}\n"
             "\t\t\t\t$('ul.toc li a').click(function(e) {\n"
             "\t\t\t\t\tanchor = $(this).attr('href');\n"
             "\t\t\t\t\ttarget = $(anchor).children('h2:first');\n"
             "\t\t\t\t\topen_anchor(target);\n"
             "\t\t\t\t});\n"
             "\t\t\t});\n"
             "\t\t</script>\n\n" );
  }

  if ( num_players > 1 ) print_html_raid_imagemaps( file, sim );

  fprintf( file,
           "\t</body>\n\n"
           "</html>\n" );

  fclose( file );
}
