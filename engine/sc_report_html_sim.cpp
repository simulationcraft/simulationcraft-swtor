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
           sim -> elapsed_cpu.total_seconds() );
  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th>Speed Up:</th>\n"
           "\t\t\t\t\t\t\t\t<td>%.0f</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           sim -> iterations * sim -> simulation_length.mean / sim -> elapsed_cpu.total_seconds() );

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
           ( double )sim -> world_lag.total_millis(), ( double )sim -> world_lag_stddev.total_millis() );
  fprintf( file,
           "\t\t\t\t\t\t\t<tr class=\"left\">\n"
           "\t\t\t\t\t\t\t\t<th>Queue Lag:</th>\n"
           "\t\t\t\t\t\t\t\t<td>%.0f ms ( stddev = %.0f ms )</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           ( double )sim -> queue_lag.total_millis(), ( double )sim -> queue_lag_stddev.total_millis() );
  if ( sim -> strict_gcd_queue )
  {
    fprintf( file,
             "\t\t\t\t\t\t\t<tr class=\"left\">\n"
             "\t\t\t\t\t\t\t\t<th>GCD Lag:</th>\n"
             "\t\t\t\t\t\t\t\t<td>%.0f ms ( stddev = %.0f ms )</td>\n"
             "\t\t\t\t\t\t\t</tr>\n",
             ( double )sim -> gcd_lag.total_millis(), ( double )sim -> gcd_lag_stddev.total_millis() );
    fprintf( file,
             "\t\t\t\t\t\t\t<tr class=\"left\">\n"
             "\t\t\t\t\t\t\t\t<th>Channel Lag:</th>\n"
             "\t\t\t\t\t\t\t\t<td>%.0f ms ( stddev = %.0f ms )</td>\n"
             "\t\t\t\t\t\t\t</tr>\n",
             ( double )sim -> channel_lag.total_millis(), ( double )sim -> channel_lag_stddev.total_millis() );
    fprintf( file,
             "\t\t\t\t\t\t\t<tr class=\"left\">\n"
             "\t\t\t\t\t\t\t\t<th>Queue GCD Reduction:</th>\n"
             "\t\t\t\t\t\t\t\t<td>%.0f ms</td>\n"
             "\t\t\t\t\t\t\t</tr>\n",
             ( double )sim -> queue_gcd_reduction.total_millis() );
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
  std::vector<player_t*> player_list = ( dps ) ? sim -> players_by_dps : sim -> players_by_hps;
  int start = num * MAX_PLAYERS_PER_CHART;
  unsigned int end = start + MAX_PLAYERS_PER_CHART;

  for ( unsigned int i=0; i < player_list.size(); i++ )
  {
    player_t* p = player_list[ i ];
    if ( dps ? p -> dps.mean <= 0 : p -> hps.mean <=0 )
    {
      player_list.resize( i );
      break;
    }
  }

  if ( end > player_list.size() ) end = static_cast<unsigned>( player_list.size() );

  fprintf( file, "\t\t\tn = [" );
  for ( int i=end-1; i >= start; i-- )
  {
    fprintf( file, "\"%s\"", player_list[i] -> name_str.c_str() );
    if ( i != start ) fprintf( file, ", " );
  }
  fprintf( file, "];\n" );

  char imgid[32];
  util_t::snprintf( imgid, sizeof( imgid ), "%sIMG%d", ( dps ) ? "DPS" : "HPS", num );
  char mapid[32];
  util_t::snprintf( mapid, sizeof( mapid ), "%sMAP%d", ( dps ) ? "DPS" : "HPS", num );

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
           imgid, mapid, imgid, mapid, mapid );
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
  int prev_type = PLAYER_NONE;

  for ( int i=0; i < num_players; i++ )
  {
    player_t* p = sim -> players_by_name[ i ];

    if ( p -> type != prev_type )
    {
      prev_type = p -> type;

      fprintf( file,
               "\t\t\t\t\t<tr>\n"
               "\t\t\t\t\t\t<th class=\"left small\">Profile</th>\n" );
      for ( int j=0; j < STAT_MAX; j++ )
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
    for ( int j=0; j < STAT_MAX; j++ )
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
           "\t\t<div id=\"help-max\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>Max</h3>\n"
           "\t\t\t\t<p>Maximum crit damage over all iterations.</p>\n"
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
           sim -> max_time.total_seconds(),
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
             "\t\t\t#masthead {height: auto;padding-bottom: 20px;border: 0;-moz-border-radius: 20px;-khtml-border-radius: 20px;-webkit-border-radius: 20px;border-radius: 20px;-moz-box-shadow: 0px 0px 8px #FDD017;-webkit-box-shadow: 0px 0px 8px #FDD017;box-shadow: 0px 0px 8px #FDD017;text-align: left;color: #FDD017;background: #000 url(data:image/png;base64,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) 7px 13px no-repeat; }\n"
             "\t\t\t#masthead h1 {margin: 20px 0 0 355px; }\n"
             "\t\t\t#home #masthead h1 {margin-top: 0px; }\n"
             "\t\t\t#masthead h2 {margin-left: 355px; }\n"
             "\t\t\t#masthead ul.params {margin: 20px 0 0 345px; }\n"
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
             min_length.total_seconds(),
             max_length.total_seconds() );
  }
  else
  {
    fprintf( file,
             "\t\t\t\t<li><b>Fight Length:</b> %.0f</li>\n",
             sim -> max_time.total_seconds() );
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
