// ==========================================================================
// Dedmonwakeen's Raid DPS/TPS Simulator.
// Send questions to natehieter@gmail.com
// ==========================================================================

#include "simulationcraft.h"

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
           "\t\t\t\t<p>Estimator for the %.2f%% confidence intervall.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n",
           sim -> confidence * 100.0 );

  fprintf( file,
           "\t\t<div id=\"help-glance-pct\">\n"
           "\t\t\t<div class=\"help-box\">\n"
           "\t\t\t\t<h3>G%%</h3>\n"
           "\t\t\t\t<p>Percentage of executes that resulted in glancing blows.</p>\n"
           "\t\t\t</div>\n"
           "\t\t</div>\n" );

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
             "\t\t\t@import url('http://www.simulationcraft.org/css/styles.css');\n"
             "\t\t</style>\n"
             "\t\t<style type=\"text/css\" media=\"print\">\n"
             "\t\t  @import url('http://www.simulationcraft.org/css/styles-print.css');\n"
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
           "\t\t\t<h2>for Star Wars: The Old Republic %s %s (build level %s)</h2>\n\n",
           SC_MAJOR_VERSION, SC_MINOR_VERSION, dbc_t::wow_version( sim -> dbc.ptr ), ( sim -> dbc.ptr ? "PTR" : "Live" ), dbc_t::build_level( sim -> dbc.ptr ) );

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
             "\t\t<script type=\"text/javascript\" src=\"http://static.torhead.com/widgets/power.js\"></script>\n" );
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
