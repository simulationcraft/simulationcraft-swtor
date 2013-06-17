// ==========================================================================
// SimulationCraft for Star Wars: The Old Republic
// http://code.google.com/p/simulationcraft-swtor/
// ==========================================================================

#include "simulationcraft.hpp"

namespace { // ANONYMOUS NAMESPACE ==========================================

static void print_html_sample_data( FILE* file, player_t* p, sample_data_t& data, const std::string& name )
{
  // Print Statistics of a Sample Data Container

  fprintf( file,
           "\t\t\t\t\t\t\t<table>\n"
           "\t\t\t\t\t\t\t\t<tr>\n"
           "\t\t\t\t\t\t\t\t\t<th class=\"left small\"><a href=\"#\" class=\"toggle-details\" rel=\"sample=%s\">%s</a></th>\n"
           "\t\t\t\t\t\t\t\t\t<th></th>\n"
           "\t\t\t\t\t\t\t\t</tr>\n", name.c_str(), name.c_str() );

  if ( data.basics_analyzed() )
  {
    fprintf( file,
             "\t\t\t\t\t\t\t\t<tr class=\"details hide\">\n"
             "\t\t\t\t\t\t\t\t<td colspan=\"2\" class=\"filler\">\n" );

    fprintf( file,
             "\t\t\t\t\t\t\t<table class=\"details\">\n" );

    fprintf( file,
             "\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"left\"><b>Sample Data</b></td>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"right\"></td>\n"
             "\t\t\t\t\t\t\t\t</tr>\n" );

    fprintf( file,
             "\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"left\">Count</td>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"right\">%d</td>\n"
             "\t\t\t\t\t\t\t\t</tr>\n",
             data.size() );

    fprintf( file,
             "\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"left\">Mean</td>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f</td>\n"
             "\t\t\t\t\t\t\t\t</tr>\n",
             data.mean );

    if ( !data.simple || data.min_max )
    {

      fprintf( file,
               "\t\t\t\t\t\t\t\t<tr>\n"
               "\t\t\t\t\t\t\t\t\t<td class=\"left\">Minimum</td>\n"
               "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f</td>\n"
               "\t\t\t\t\t\t\t\t</tr>\n",
               data.min );

      fprintf( file,
               "\t\t\t\t\t\t\t\t<tr>\n"
               "\t\t\t\t\t\t\t\t\t<td class=\"left\">Maximum</td>\n"
               "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f</td>\n"
               "\t\t\t\t\t\t\t\t</tr>\n",
               data.max );

      fprintf( file,
               "\t\t\t\t\t\t\t\t<tr>\n"
               "\t\t\t\t\t\t\t\t\t<td class=\"left\">Spread ( max - min )</td>\n"
               "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f</td>\n"
               "\t\t\t\t\t\t\t\t</tr>\n",
               data.max - data.min );

      fprintf( file,
               "\t\t\t\t\t\t\t\t<tr>\n"
               "\t\t\t\t\t\t\t\t\t<td class=\"left\">Range [ ( max - min ) / 2 * 100%% ]</td>\n"
               "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
               "\t\t\t\t\t\t\t\t</tr>\n",
               data.mean ? ( ( data.max - data.min ) / 2 ) * 100 / data.mean : 0 );
      if ( data.variance_analyzed() )
      {
        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\">Standard Deviation</td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.4f</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 data.std_dev );

        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\">5th Percentile</td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 data.percentile( 0.05 ) );

        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\">95th Percentile</td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 data.percentile( 0.95 ) );

        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\">( 95th Percentile - 5th Percentile )</td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 data.percentile( 0.95 ) - data.percentile( 0.05 ) );

        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\"><b>Mean Distribution</b></td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\"></td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n" );

        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\">Standard Deviation</td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.4f</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 data.mean_std_dev );

        double mean_error = data.mean_std_dev * p -> sim -> confidence_estimator;
        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\">%.2f%% Confidence Intervall</td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\">( %.2f - %.2f )</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 p -> sim -> confidence * 100.0,
                 data.mean - mean_error,
                 data.mean + mean_error );

        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\">Normalized %.2f%% Confidence Intervall</td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\">( %.2f%% - %.2f%% )</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 p -> sim -> confidence * 100.0,
                 data.mean ? 100 - mean_error * 100 / data.mean : 0,
                 data.mean ? 100 + mean_error * 100 / data.mean : 0 );



        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\"><b>Approx. Iterations needed for ( always use n>=50 )</b></td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\"></td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n" );

        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\">1%% Error</td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\">%i</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 ( int ) ( data.mean ? ( ( mean_error * mean_error * ( ( float ) data.size() ) / ( 0.01 * data.mean * 0.01 * data.mean ) ) ) : 0 ) );

        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\">0.1%% Error</td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\">%i</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 ( int ) ( data.mean ? ( ( mean_error * mean_error * ( ( float ) data.size() ) / ( 0.001 * data.mean * 0.001 * p -> dps.mean ) ) ) : 0 ) );

        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\">0.1 Scale Factor Error with Delta=60</td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\">%i</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 ( int ) ( 2.0 * mean_error * mean_error * ( ( float ) data.size() ) / ( 6 * 6 ) ) );

        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\">0.05 Scale Factor Error with Delta=60</td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\">%i</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 ( int ) ( 2.0 * mean_error * mean_error * ( ( float ) data.size() ) / ( 3 * 3 ) ) );

        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\">0.01 Scale Factor Error with Delta=60</td>\n"
                 "\t\t\t\t\t\t\t\t\t<td class=\"right\">%i</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 ( int ) (  2.0 * mean_error * mean_error * ( ( float ) data.size() ) / ( 0.6 * 0.6 ) ) );
      }
    }

  }
  fprintf( file,
           "\t\t\t\t\t\t\t\t</table>\n" );
  fprintf( file,
           "\t\t\t\t\t\t\t\t</td>\n"
           "\t\t\t\t\t\t\t</tr>\n" );
  fprintf( file,
           "\t\t\t\t\t\t\t\t</table>\n" );

}

template <typename Range>
double mean_damage( const Range& results )
{
  double mean = 0;
  int count = 0;

  for ( auto const& result : results )
  {
    mean  += result.actual_amount.sum;
    count += result.actual_amount.size();
  }

  if ( count > 0 )
    mean /= count;

  return mean;
}

// print_html_action_damage =================================================

static void print_html_action_damage( FILE* file, stats_t* s, player_t* p, int j )
{
  int id = 0;

  fprintf( file,
           "\t\t\t\t\t\t\t<tr" );
  if ( j & 1 )
  {
    fprintf( file, " class=\"odd\"" );
  }
  fprintf( file, ">\n" );

  for ( action_t* a = s -> player -> action_list; a; a = a -> next )
  {
    if ( a -> stats != s ) continue;
    id = a -> id;
    if ( ! a -> background ) break;
  }

  fprintf( file,
           "\t\t\t\t\t\t\t\t<td class=\"left small\">" );
  if ( p -> sim -> report_details )
    fprintf( file,
             "<a href=\"http://knotor.com/abilities/%i\" class=\"toggle-details\">%s</a></td>\n",
             id,
             s -> name_str.c_str() );
  else
    fprintf( file,
             "%s</td>\n",
             s -> name_str.c_str() );

  fprintf( file,
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.1f%%</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.1f</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.2fsec</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.1f%%</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.1f%%</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.1f%%</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.1f%%</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.1f%%</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.1f%%</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           s -> portion_aps.mean,
           s -> portion_amount * 100,
           s -> num_executes,
           s -> frequency,
           s -> ape,
           s -> apet,
           s -> direct_results[ RESULT_HIT  ].actual_amount.mean,
           s -> direct_results[ RESULT_CRIT ].actual_amount.mean,
           mean_damage( s -> direct_results ),
           s -> direct_results[ RESULT_CRIT ].pct,
           s -> direct_results[ RESULT_MISS ].pct +
           s -> direct_results[ RESULT_AVOID  ].pct,
           s -> direct_results[ RESULT_BLOCK  ].pct,
           s -> num_ticks,
           s -> tick_results[ RESULT_HIT  ].actual_amount.mean,
           s -> tick_results[ RESULT_CRIT ].actual_amount.mean,
           mean_damage( s -> tick_results ),
           s -> tick_results[ RESULT_CRIT ].pct,
           s -> tick_results[ RESULT_MISS ].pct +
           s -> tick_results[ RESULT_AVOID ].pct,
           100 * to_seconds( s -> total_tick_time ) / s -> player -> fight_length.mean );

  if ( p -> sim -> report_details )
  {
    std::string timeline_stat_aps_str                    = "";
    if ( ! s -> timeline_aps_chart.empty() )
    {
      timeline_stat_aps_str = "<img src=\"" + s -> timeline_aps_chart + "\" alt=\"APS Timeline Chart\" />\n";
    }
    std::string aps_distribution_str                    = "";
    if ( ! s -> aps_distribution_chart.empty() )
    {
      aps_distribution_str = "<img src=\"" + s -> aps_distribution_chart + "\" alt=\"APS Distribution Chart\" />\n";
    }
    fprintf( file,
             "\t\t\t\t\t\t\t<tr class=\"details hide\">\n"
             "\t\t\t\t\t\t\t\t<td colspan=\"20\" class=\"filler\">\n" );

    // Stat Details
    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t<h4>Stats details: %s </h4>\n", s -> name_str.c_str() );

    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t<table class=\"details\">\n"
              "\t\t\t\t\t\t\t\t\t\t<tr>\n" );
    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Executes</th>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Direct Results</th>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Ticks</th>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Tick Results</th>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Execute Time per Execution</th>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Tick Time per  Tick</th>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Actual Amount</th>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Total Amount</th>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Overkill %%</th>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Amount per Total Time</th>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Amount per Total Execute Time</th>\n" );
    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t\t</tr>\n"
              "\t\t\t\t\t\t\t\t\t\t<tr>\n" );
    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.4f</td>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.4f</td>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
              "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n",
              s -> num_executes,
              s -> num_direct_results,
              s -> num_ticks,
              s -> num_tick_results,
              s -> etpe,
              s -> ttpt,
              s -> actual_amount.mean,
              s -> total_amount.mean,
              s -> overkill_pct,
              s -> aps,
              s -> apet );
    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t\t</tr>\n"
              "\t\t\t\t\t\t\t\t\t</table>\n" );
    if ( ! s -> portion_aps.simple || ! s -> actual_amount.simple )
    {
      print_html_sample_data( file, p, s -> actual_amount, "Actual Amount" );

      print_html_sample_data( file, p, s -> portion_aps, "portion Amount per Second ( pAPS )" );

        if ( ! s -> portion_aps.simple && p -> sim -> scaling -> has_scale_factors() )
        {
          int colspan = 0;
          fprintf( file,
                   "\t\t\t\t\t\t<table class=\"details\">\n" );
          fprintf( file,
                   "\t\t\t\t\t\t\t<tr>\n"
                   "\t\t\t\t\t\t\t\t<th><a href=\"#help-scale-factors\" class=\"help\">?</a></th>\n" );
          for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
            if ( p -> scales_with[ i ] )
            {
              fprintf( file,
                       "\t\t\t\t\t\t\t\t<th>%s</th>\n",
                       util_t::stat_type_abbrev( i ) );
              colspan++;
            }
          if ( p -> sim -> scaling -> scale_lag )
          {
            fprintf( file,
                     "\t\t\t\t\t\t\t\t<th>ms Lag</th>\n" );
            colspan++;
          }
          fprintf( file,
                   "\t\t\t\t\t\t\t</tr>\n" );
          fprintf( file,
                   "\t\t\t\t\t\t\t<tr>\n"
                   "\t\t\t\t\t\t\t\t<th class=\"left\">Scale Factors</th>\n" );
          for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
            if ( p -> scales_with[ i ] )
              fprintf( file,
                       "\t\t\t\t\t\t\t\t<td>%.*f</td>\n",
                       p -> sim -> report_precision,
                       s -> scaling.get_stat( i ) );
          fprintf( file,
                   "\t\t\t\t\t\t\t</tr>\n" );
          fprintf( file,
                   "\t\t\t\t\t\t\t<tr>\n"
                   "\t\t\t\t\t\t\t\t<th class=\"left\">Scale Deltas</th>\n" );
          for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
            if ( p -> scales_with[ i ] )
              fprintf( file,
                       "\t\t\t\t\t\t\t\t<td>%.0f</td>\n",
                       p -> sim -> scaling -> stats.get_stat( i ) );
          fprintf( file,
                   "\t\t\t\t\t\t\t</tr>\n" );
          fprintf( file,
                   "\t\t\t\t\t\t\t<tr>\n"
                   "\t\t\t\t\t\t\t\t<th class=\"left\">Error</th>\n" );
          for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
            if ( p -> scales_with[ i ] )
              fprintf( file,
                       "\t\t\t\t\t\t\t\t<td>%.*f</td>\n",
                       p -> sim -> report_precision,
                       s -> scaling_error.get_stat( i ) );
          fprintf( file,
                   "\t\t\t\t\t\t\t</tr>\n" );



          fprintf( file,
                   "\t\t\t\t\t\t</table>\n" );

          std::string scale_factor_str;
          chart_t::scale_factors( scale_factor_str, p, s->name_str, s -> scaling, s -> scaling_error );

          if ( ! scale_factor_str.empty() )
          {
            scale_factor_str = "<img src=\"" + scale_factor_str + "\" alt=\"DPS Error Chart\" />\n";
            fprintf ( file, "\t\t\t\t\t\t\t\t\t%s\n",
                    scale_factor_str.c_str() );
          }
        }
    }


    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t<table  class=\"details\">\n" );
    if ( s -> num_direct_results > 0 )
    {
      // Direct Damage
      fprintf ( file,
                "\t\t\t\t\t\t\t\t\t\t<tr>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Direct Results</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Count</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Pct</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Mean</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Min</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Max</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Average per Iteration</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Average Min</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Average Max</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Actual Amount</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Total Amount</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Overkill %%</th>\n"
                "\t\t\t\t\t\t\t\t\t\t</tr>\n" );
      for ( result_type i = RESULT_MAX; --i >= RESULT_NONE; )
      {
        if ( s -> direct_results[ i ].count.mean )
        {
          fprintf( file,
                   "\t\t\t\t\t\t\t\t\t\t<tr" );
          if ( i & 1 )
          {
            fprintf( file, " class=\"odd\"" );
          }
          fprintf( file, ">\n" );
          fprintf ( file,
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"left small\">%s</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f%%</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t</tr>\n",
                    util_t::result_type_string( i ),
                    s -> direct_results[ i ].count.mean,
                    s -> direct_results[ i ].pct,
                    s -> direct_results[ i ].actual_amount.mean,
                    s -> direct_results[ i ].actual_amount.min,
                    s -> direct_results[ i ].actual_amount.max,
                    s -> direct_results[ i ].avg_actual_amount.mean,
                    s -> direct_results[ i ].avg_actual_amount.min,
                    s -> direct_results[ i ].avg_actual_amount.max,
                    s -> direct_results[ i ].fight_actual_amount.mean,
                    s -> direct_results[ i ].fight_total_amount.mean,
                    s -> direct_results[ i ].overkill_pct );
        }
      }

    }

    if ( s -> num_tick_results > 0 )
    {
      // Tick Damage
      fprintf ( file,
                "\t\t\t\t\t\t\t\t\t\t<tr>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Tick Results</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Count</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Pct</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Mean</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Min</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Max</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Average per Iteration</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Average Min</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Average Max</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Actual Amount</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Total Amount</th>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<th class=\"small\">Overkill %%</th>\n"
                "\t\t\t\t\t\t\t\t\t\t</tr>\n" );
      for ( result_type i = RESULT_MAX; --i >= RESULT_NONE; )
      {
        if ( s -> tick_results[ i ].count.mean )
        {
          fprintf( file,
                   "\t\t\t\t\t\t\t\t\t\t<tr" );
          if ( i & 1 )
          {
            fprintf( file, " class=\"odd\"" );
          }
          fprintf( file, ">\n" );
          fprintf ( file,
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"left small\">%s</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.1f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f%%</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.0f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t\t<td class=\"right small\">%.2f</td>\n"
                    "\t\t\t\t\t\t\t\t\t\t</tr>\n",
                    util_t::result_type_string( i ),
                    s -> tick_results[ i ].count.mean,
                    s -> tick_results[ i ].pct,
                    s -> tick_results[ i ].actual_amount.mean,
                    s -> tick_results[ i ].actual_amount.min,
                    s -> tick_results[ i ].actual_amount.max,
                    s -> tick_results[ i ].avg_actual_amount.mean,
                    s -> tick_results[ i ].avg_actual_amount.min,
                    s -> tick_results[ i ].avg_actual_amount.max,
                    s -> tick_results[ i ].fight_actual_amount.mean,
                    s -> tick_results[ i ].fight_total_amount.mean,
                    s -> tick_results[ i ].overkill_pct );
        }
      }


    }
    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t</table>\n" );

    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t<div class=\"clear\"></div>\n" );

    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t%s\n",
              timeline_stat_aps_str.c_str() );
    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t%s\n",
              aps_distribution_str.c_str() );

    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t<div class=\"clear\"></div>\n" );
    // Action Details
    std::vector<std::string> processed_actions;
    size_t size = s -> action_list.size();
    for ( size_t i = 0; i < size; i++ )
    {
      action_t* a = s -> action_list[ i ];

      bool found = false;
      size_t size_processed = processed_actions.size();
      for ( size_t k = 0; k < size_processed && !found; k++ )
        if ( processed_actions[ k ] == a -> name() )
          found = true;
      if ( found ) continue;
      processed_actions.push_back( a -> name() );

      fprintf ( file,
                "\t\t\t\t\t\t\t\t\t<h4>Action details: %s </h4>\n", a -> name() );
      fprintf ( file,
                "\t\t\t\t\t\t\t\t\t<div class=\"float\">\n"
                "\t\t\t\t\t\t\t\t\t\t<h5>Static Values</h5>\n"
                "\t\t\t\t\t\t\t\t\t\t<ul>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">id:</span>%i</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">attack-policy:</span>%s</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">school:</span>%s</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">resource:</span>%s</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">range:</span>%.1f</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">travel_speed:</span>%.4f</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">trigger_gcd:</span>%.4f</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">min_gcd:</span>%.4f</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">base_cost:</span>%.1f</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">cooldown:</span>%.2f</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">base_execute_time:</span>%.2f</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">base_crit:</span>%.2f</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">base_accuracy:</span>%.2f</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">target:</span>%s</li>\n"
                "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">harmful:</span>%s</li>\n"
                "\t\t\t\t\t\t\t\t\t\t</ul>\n"
                "\t\t\t\t\t\t\t\t\t</div>\n"
                "\t\t\t\t\t\t\t\t\t<div class=\"float\">\n",
                a -> id,
                a -> attack_policy -> name().c_str(),
                util_t::school_type_string( a-> school ),
                util_t::resource_type_string( a -> resource ),
                a -> range,
                a -> travel_speed,
                to_seconds( a -> trigger_gcd ),
                to_seconds( a -> min_gcd ),
                a -> base_cost,
                to_seconds( a -> cooldown -> duration ),
                to_seconds( a -> base_execute_time ),
                a -> base_crit,
                a -> base_accuracy,
                a -> target ? a -> target -> name() : "",
                a -> harmful ? "true" : "false" );
      if ( a -> dd.power_mod || a -> dd.base_min || a -> dd.base_max )
      {
        fprintf ( file,
                  "\t\t\t\t\t\t\t\t\t\t<h5>Direct Damage</h5>\n"
                  "\t\t\t\t\t\t\t\t\t\t<ul>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">may_crit:</span>%s</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">dd.power_mod:</span>%.6f</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">dd.standardhealthpercentmin:</span>%.3f</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">dd.standardhealthpercentmax:</span>%.3f</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">dd.base_min:</span>%.2f</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">dd.base_max:</span>%.2f</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t</ul>\n",
                  a -> may_crit ? "true" : "false",
                  a -> dd.power_mod,
                  a -> dd.standardhealthpercentmin,
                  a -> dd.standardhealthpercentmax,
                  a -> dd.base_min,
                  a -> dd.base_max );
      }
      if ( a -> num_ticks )
      {
        fprintf ( file,
                  "\t\t\t\t\t\t\t\t\t\t<h5>Damage Over Time</h5>\n"
                  "\t\t\t\t\t\t\t\t\t\t<ul>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">tick_may_crit:</span>%s</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">tick_zero:</span>%s</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">td.power_mod:</span>%.6f</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">td.standardhealthpercentmin:</span>%.3f</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">td.standardhealthpercentmax:</span>%.3f</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">td.base_min:</span>%.2f</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">td.base_max:</span>%.2f</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">num_ticks:</span>%i</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">base_tick_time:</span>%.2f</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">hasted_ticks:</span>%s</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">dot_behavior:</span>%s</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t</ul>\n",
                  a -> tick_may_crit?"true":"false",
                  a -> tick_zero?"true":"false",
                  a -> td.power_mod,
                  a -> td.standardhealthpercentmin,
                  a -> td.standardhealthpercentmax,
                  a -> td.base_min,
                  a -> td.base_max,
                  a -> num_ticks,
                  to_seconds( a -> base_tick_time ),
                  a -> hasted_ticks?"true":"false",
                  a -> dot_behavior==DOT_REFRESH?"DOT_REFRESH":a -> dot_behavior==DOT_CLIP?"DOT_CLIP":"DOT_WAIT" );
      }
      if ( a -> weapon )
      {
        fprintf ( file,
                  "\t\t\t\t\t\t\t\t\t\t<h5>Weapon</h5>\n"
                  "\t\t\t\t\t\t\t\t\t\t<ul>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">normalized:</span>%s</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">weapon_multiplier:</span>%.2f</li>\n"
                  "\t\t\t\t\t\t\t\t\t\t</ul>\n",
                  a -> normalize_weapon_speed ? "true" : "false",
                  a -> weapon_multiplier );
      }
      fprintf ( file,
                "\t\t\t\t\t\t\t\t\t</div>\n"
                "\t\t\t\t\t\t\t\t\t<div class=\"clear\"></div>\n" );
    }


    fprintf( file,
             "\t\t\t\t\t\t\t\t</td>\n"
             "\t\t\t\t\t\t\t</tr>\n" );
  }
}

// print_html_action_resource ===============================================

static void print_html_action_resource( FILE* file, stats_t* s, int j )
{

  fprintf( file,
           "\t\t\t\t\t\t\t<tr" );
  if ( j & 1 )
  {
    fprintf( file, " class=\"odd\"" );
  }
  fprintf( file, ">\n" );

  for ( action_t* a = s -> player -> action_list; a; a = a -> next )
  {
    if ( a -> stats != s ) continue;
    if ( ! a -> background ) break;
  }

  fprintf( file,
           "\t\t\t\t\t\t\t\t<td class=\"left small\">%s</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"left small\">%s</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.1f%%</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.1f</td>\n"
           "\t\t\t\t\t\t\t\t<td class=\"right small\">%.1f</td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           s -> name_str.c_str(),
           util_t::resource_type_string( s -> resource ),
           s -> resource_portion * 100,
           s -> apr,
           s -> rpe );
}

// print_html_gear ==========================================================

static void print_html_gear ( FILE* file, player_t* a )
{
  if ( a -> fight_length.mean > 0 )
  {
    fprintf( file,
             "\t\t\t\t\t\t<div class=\"player-section gear\">\n"
             "\t\t\t\t\t\t\t<h3 class=\"toggle\">Gear</h3>\n"
             "\t\t\t\t\t\t\t<div class=\"toggle-content hide\">\n"
             "\t\t\t\t\t\t\t\t<table class=\"sc\">\n"
             "\t\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t\t<th></th>\n"
             "\t\t\t\t\t\t\t\t\t\t<th>Average Item Level: %.2f</th>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> avg_ilvl );

    for ( int i=0; i < SLOT_MAX; i++ )
    {
      item_t& item = a -> items[ i ];

      fprintf( file,
               "\t\t\t\t\t\t\t\t\t<tr>\n"
               "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">%s</th>\n"
               "\t\t\t\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n"
               "\t\t\t\t\t\t\t\t\t</tr>\n",
               item.slot_name(),
               item.active() ? item.options_str.c_str() : "empty" );
    }

    fprintf( file,
             "\t\t\t\t\t\t\t\t</table>\n"
             "\t\t\t\t\t\t\t</div>\n"
             "\t\t\t\t\t\t</div>\n" );
  }
}

// print_html_profile =======================================================

std::string text_to_html( const std::string& text )
{
  std::string html;

  for ( auto p = text.begin(), e = text.end(); p != e; ++p )
  {
    switch( *p )
    {
    case '&':
      html += "&amp;";
      break;
    case '<':
      html += "&lt;";
      break;
    case '>':
      html += "&gt;";
      break;
    case '\n':
      html += "<br>\n";
      break;
    default:
      html += *p;
      break;
    }
  }

  return html;
}

static void print_html_profile( FILE* file, player_t* a )
{
  if ( a -> fight_length.mean > 0 )
  {
    fprintf( file,
             "\t\t\t\t\t\t<div class=\"player-section profile\">\n"
             "\t\t\t\t\t\t\t<h3 class=\"toggle\">Profile</h3>\n"
             "\t\t\t\t\t\t\t<div class=\"toggle-content hide\">\n"
             "\t\t\t\t\t\t\t\t<div class=\"subsection force-wrap\">\n"
             "\t\t\t\t\t\t\t\t\t<p>%s</p>\n"
             "\t\t\t\t\t\t\t\t</div>\n"
             "\t\t\t\t\t\t\t</div>\n"
             "\t\t\t\t\t\t</div>\n",
             text_to_html( a -> create_profile( SAVE_ALL ) ).c_str() );
  }
}


// print_html_stats =========================================================

static void print_html_stats( FILE* file, player_t* a )
{
  std::string n = a -> name();
  util_t::format_text( n, true );

  if ( a -> fight_length.mean > 0 )
  {
    fprintf( file,
             "\t\t\t\t\t\t<div class=\"player-section stats\">\n"
             "\t\t\t\t\t\t\t<h3 class=\"toggle\">Stats</h3>\n"
             "\t\t\t\t\t\t\t<div class=\"toggle-content hide\">\n"
             "\t\t\t\t\t\t\t\t<table class=\"sc\">\n"
             "\t\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t\t<th></th>\n"
             "\t\t\t\t\t\t\t\t\t\t<th>Raid-Buffed</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<th>Unbuffed</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<th>Gear Amount</th>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n" );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Strength</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.attribute[ ATTR_STRENGTH  ],
             a -> strength(),
             a -> stats.attribute[ ATTR_STRENGTH  ] );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Aim</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.attribute[ ATTR_AIM ],
             a -> aim(),
             a -> stats.attribute[ ATTR_AIM ] );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Cunning</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.attribute[ ATTR_CUNNING ],
             a -> cunning(),
             a -> stats.attribute[ ATTR_CUNNING ] );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Willpower</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.attribute[ ATTR_WILLPOWER  ],
             a -> willpower(),
             a -> stats.attribute[ ATTR_WILLPOWER  ] );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Endurance</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.attribute[ ATTR_ENDURANCE ],
             a -> endurance(),
             a -> stats.attribute[ ATTR_ENDURANCE ] );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Presence</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.attribute[ ATTR_PRESENCE ],
             a -> presence(),
             a -> stats.attribute[ ATTR_PRESENCE ] );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Health</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.resource[ RESOURCE_HEALTH ],
             a -> resource_max[ RESOURCE_HEALTH ],
             0.0 );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Power</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.power,
             a -> composite_power(),
             a -> stats.power );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Force Power</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.force_power,
             a -> composite_force_power(),
             a -> stats.force_power );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Tech Power</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.tech_power,
             a -> composite_tech_power(),
             a -> stats.tech_power );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Surge</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             100 * a -> buffed.surge,
             100 * a -> surge_bonus,
             a -> stats.surge_rating );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Alacrity</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             100 * ( a -> buffed.alacrity - 1 ),
             100 * ( a -> alacrity() - 1 ),
             a -> stats.alacrity_rating );

    // Melee
    if ( a -> report_attack_type( action_t::melee_policy ) )
      {
    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\"></th>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n" );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Melee Damage Bonus</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\"></td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.melee_damage_bonus,
             a -> melee_damage_bonus() );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Melee Accuracy</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             100 * a -> buffed.melee_accuracy,
             100 * a -> melee_accuracy_chance(),
             a -> stats.accuracy_rating );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Melee Crit</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             100 * a -> buffed.melee_crit,
             100 * a -> melee_crit_chance(),
             a -> stats.crit_rating );
      }
    // Range
    if ( a -> report_attack_type( action_t::range_policy ) )
      {
    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\"></th>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n" );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Range Damage Bonus</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\"></td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.range_damage_bonus,
             a -> range_damage_bonus() );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Range Accuracy</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             100 * a -> buffed.range_accuracy,
             100 * a -> range_accuracy_chance(),
             a -> stats.accuracy_rating  );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Range Crit</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             100 * a -> buffed.range_crit,
             100 * a -> range_crit_chance(),
             a -> stats.crit_rating );
      }

    // Force
    if ( a -> report_attack_type( action_t::force_policy ) )
      {
    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\"></th>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n" );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Force Damage Bonus</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\"></td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.force_damage_bonus,
             a -> force_damage_bonus() );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Force Accuracy</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             100 * a -> buffed.force_accuracy,
             100 * a -> force_accuracy_chance(),
             a -> stats.accuracy_rating  );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Force Crit</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             100 * a -> buffed.force_crit,
             100 * a -> force_crit_chance(),
             a -> stats.crit_rating );
      }
    // Tech
    if ( a -> report_attack_type( action_t::tech_policy ) )
      {
    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\"></th>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n" );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Tech Damage Bonus</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\"></td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.tech_damage_bonus,
             a -> tech_damage_bonus() );
    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Tech Accuracy</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             100 * a -> buffed.tech_accuracy,
             100 * a -> tech_accuracy_chance(),
             a -> stats.accuracy_rating  );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Tech Crit</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             100 * a -> buffed.tech_crit,
             100 * a -> tech_crit_chance(),
             a -> stats.crit_rating );
      }


    // Force Heal
    if ( a -> report_attack_type( action_t::force_heal_policy ) )
      {
    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\"></th>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n" );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Force Healing Bonus</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\"></td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.force_healing_bonus,
             a -> force_healing_bonus() );
      }

    // Tech Heal
    if ( a -> report_attack_type( action_t::tech_heal_policy ) )
      {
    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\"></th>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n" );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Tech Healing Bonus</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\"></td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.tech_healing_bonus,
             a -> tech_healing_bonus() );
      }

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\"></th>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n" );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Armor</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.armor,
             a -> composite_armor(),
             ( a -> stats.armor + a -> stats.bonus_armor ) );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Armor Penetration Debuff</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\"></td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> buffed.armor_penetration_debuff * 100.0,
             a -> armor_penetration_debuff() * 100.0 );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr class=\"odd\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Shield</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             100 * a -> buffed.shield_chance,
             100 * a -> shield_chance(),
             a -> stats.shield_rating );

    fprintf( file,
             "\t\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">Absorb</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"right\">%.0f</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             100 * a -> buffed.shield_absorb,
             100 * a -> shield_absorb(),
             a -> stats.absorb_rating );

    fprintf( file,
             "\t\t\t\t\t\t\t\t</table>\n"
             "\t\t\t\t\t\t\t</div>\n"
             "\t\t\t\t\t\t</div>\n" );
  }
}


// print_html_talents_player ================================================

static void print_html_talents( FILE* file, player_t* p )
{
  std::string n = p -> name();
  util_t::format_text( n, true );

  if ( p -> fight_length.mean > 0 )
  {
    fprintf( file,
             "\t\t\t\t\t\t<div class=\"player-section talents\">\n"
             "\t\t\t\t\t\t\t<h3 class=\"toggle\">Talents</h3>\n"
             "\t\t\t\t\t\t\t<div class=\"toggle-content hide\">\n" );

    for ( int i = 0; i < MAX_TALENT_TREES; i++ )
    {
      size_t tree_size = p -> talent_trees[ i ].size();

      if ( tree_size == 0 )
        continue;

      fprintf( file,
               "\t\t\t\t\t\t\t\t<div class=\"float\">\n"
               "\t\t\t\t\t\t\t\t\t<table class=\"sc\">\n"
               "\t\t\t\t\t\t\t\t\t\t<tr>\n"
               "\t\t\t\t\t\t\t\t\t\t\t<th class=\"left\">%s</th>\n"
               "\t\t\t\t\t\t\t\t\t\t\t<th>Rank</th>\n"
               "\t\t\t\t\t\t\t\t\t\t</tr>\n",
               util_t::talent_tree_string( p -> tree_type[ i ], false ) );



      for ( size_t j=0; j < tree_size; j++ )
      {
        talent_t* t = p -> talent_trees[ i ][ j ];

        fprintf( file, "\t\t\t\t\t\t\t\t\t\t<tr%s>\n", ( ( j&1 ) ? " class=\"odd\"" : "" ) );
        fprintf( file, "\t\t\t\t\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n", t -> name().c_str() );
        fprintf( file, "\t\t\t\t\t\t\t\t\t\t\t<td>%d</td>\n", t -> rank() );
        fprintf( file, "\t\t\t\t\t\t\t\t\t\t</tr>\n" );
      }
      fprintf( file,
               "\t\t\t\t\t\t\t\t\t</table>\n"
               "\t\t\t\t\t\t\t\t</div>\n" );
    }

    fprintf( file,
             "\t\t\t\t\t\t\t\t<div class=\"clear\"></div>\n" );

    fprintf( file,
             "\t\t\t\t\t\t\t</div>\n"
             "\t\t\t\t\t\t</div>\n" );
  }
}


// print_html_player_scale_factors ==========================================

static void print_html_player_scale_factors( FILE* file, sim_t* sim, player_t* p )
{

  if ( !p -> is_pet() )
  {
    if ( p -> sim -> scaling -> has_scale_factors() )
    {
      int colspan = 0;
      fprintf( file,
               "\t\t\t\t\t\t<table class=\"sc mt\">\n" );
      fprintf( file,
               "\t\t\t\t\t\t\t<tr>\n"
               "\t\t\t\t\t\t\t\t<th><a href=\"#help-scale-factors\" class=\"help\">?</a></th>\n" );
      for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
        if ( p -> scales_with[ i ] )
        {
          fprintf( file,
                   "\t\t\t\t\t\t\t\t<th>%s</th>\n",
                   util_t::stat_type_abbrev( i ) );
          colspan++;
        }
      if ( p -> sim -> scaling -> scale_lag )
      {
        fprintf( file,
                 "\t\t\t\t\t\t\t\t<th>ms Lag</th>\n" );
        colspan++;
      }
      fprintf( file,
               "\t\t\t\t\t\t\t</tr>\n" );
      fprintf( file,
               "\t\t\t\t\t\t\t<tr>\n"
               "\t\t\t\t\t\t\t\t<th class=\"left\">Scale Factors</th>\n" );
      for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
        if ( p -> scales_with[ i ] )
          fprintf( file,
                   "\t\t\t\t\t\t\t\t<td>%.*f</td>\n",
                   p -> sim -> report_precision,
                   p -> scaling.get_stat( i ) );
      if ( p -> sim -> scaling -> scale_lag )
        fprintf( file,
                 "\t\t\t\t\t\t\t\t<td>%.*f</td>\n",
                 p -> sim -> report_precision,
                 p -> scaling_lag );
      fprintf( file,
               "\t\t\t\t\t\t\t</tr>\n" );
      fprintf( file,
               "\t\t\t\t\t\t\t<tr>\n"
               "\t\t\t\t\t\t\t\t<th class=\"left\">Normalized</th>\n" );
      for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
        if ( p -> scales_with[ i ] )
          fprintf( file,
                   "\t\t\t\t\t\t\t\t<td>%.*f</td>\n",
                   p -> sim -> report_precision,
                   p -> scaling_normalized.get_stat( i ) );
      fprintf( file,
               "\t\t\t\t\t\t\t</tr>\n" );
      fprintf( file,
               "\t\t\t\t\t\t\t<tr>\n"
               "\t\t\t\t\t\t\t\t<th class=\"left\">Scale Deltas</th>\n" );
      for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
        if ( p -> scales_with[ i ] )
          fprintf( file,
                   "\t\t\t\t\t\t\t\t<td>%.0f</td>\n",
                   p -> sim -> scaling -> stats.get_stat( i ) );
      if ( p -> sim -> scaling -> scale_lag )
        fprintf( file,
                 "\t\t\t\t\t\t\t\t<td>100</td>\n" );
      fprintf( file,
               "\t\t\t\t\t\t\t</tr>\n" );
      fprintf( file,
               "\t\t\t\t\t\t\t<tr>\n"
               "\t\t\t\t\t\t\t\t<th class=\"left\">Error</th>\n" );
      for ( stat_type i = STAT_NONE; i < STAT_MAX; i++ )
        if ( p -> scales_with[ i ] )
          fprintf( file,
                   "\t\t\t\t\t\t\t\t<td>%.*f</td>\n",
                   p -> sim -> report_precision,
                   p -> scaling_error.get_stat( i ) );
      if ( p -> sim -> scaling -> scale_lag )
        fprintf( file,
                 "\t\t\t\t\t\t\t\t<td>%.*f</td>\n",
                 p -> sim -> report_precision,
                 p -> scaling_lag_error );
      fprintf( file,
               "\t\t\t\t\t\t\t</tr>\n" );
#if 0
// Disabled until Torhead or others bring back gear weight interfaces
      fprintf( file,
               "\t\t\t\t\t\t\t<tr class=\"left\">\n"
               "\t\t\t\t\t\t\t\t<th>Gear Ranking</th>\n"
               "\t\t\t\t\t\t\t\t<td colspan=\"%i\" class=\"filler\">\n"
               "\t\t\t\t\t\t\t\t\t<ul class=\"float\">\n"
               "\t\t\t\t\t\t\t\t\t\t<li><a href=\"%s\" class=\"ext\">wowhead</a></li>\n"
               "\t\t\t\t\t\t\t\t\t\t<li><a href=\"%s\" class=\"ext\">lootrank</a></li>\n"
               "\t\t\t\t\t\t\t\t\t</ul>\n"
               "\t\t\t\t\t\t\t\t</td>\n"
               "\t\t\t\t\t\t\t</tr>\n",
               colspan,
               p -> gear_weights_wowhead_link.c_str(),
               p -> gear_weights_lootrank_link.c_str() );
      fprintf( file,
               "\t\t\t\t\t\t\t<tr class=\"left\">\n"
               "\t\t\t\t\t\t\t\t<th>Optimizers</th>\n"
               "\t\t\t\t\t\t\t\t<td colspan=\"%i\" class=\"filler\">\n"
               "\t\t\t\t\t\t\t\t\t<ul class=\"float\">\n"
               "\t\t\t\t\t\t\t\t\t\t<li><a href=\"%s\" class=\"ext\">wowreforge</a></li>\n"
               "\t\t\t\t\t\t\t\t\t</ul>\n"
               "\t\t\t\t\t\t\t\t</td>\n"
               "\t\t\t\t\t\t\t</tr>\n",
               colspan,
               p -> gear_weights_wowreforge_link.c_str() );
#endif
      fprintf( file,
               "\t\t\t\t\t\t\t<tr class=\"left\">\n"
               "\t\t\t\t\t\t\t\t<th>Stat Ranking</th>\n"
               "\t\t\t\t\t\t\t\t<td colspan=\"%i\" class=\"filler\">\n"
               "\t\t\t\t\t\t\t\t\t<ul class=\"float\">\n"
               "\t\t\t\t\t\t\t\t\t\t<li>",
               colspan );
      size_t num_scaling_stats = p -> scaling_stats.size();
      for ( size_t i=0; i < num_scaling_stats; i++ )
      {
        if ( i > 0 )
        {
          if ( ( ( p -> scaling.get_stat( p -> scaling_stats[ i - 1 ] ) - p -> scaling.get_stat( p -> scaling_stats[ i ] ) )
                 > sqrt ( p -> scaling_compare_error.get_stat( p -> scaling_stats[ i - 1 ] ) * p -> scaling_compare_error.get_stat( p -> scaling_stats[ i - 1 ] ) / 4 + p -> scaling_compare_error.get_stat( p -> scaling_stats[ i ] ) * p -> scaling_compare_error.get_stat( p -> scaling_stats[ i ] ) / 4 ) * 2 ) )
            fprintf( file, " > " );
          else
            fprintf( file, " = " );
        }

        fprintf( file, "%s", util_t::stat_type_abbrev( p -> scaling_stats[ i ] ) );

      }
      fprintf( file, "</li>\n"
               "\t\t\t\t\t\t\t\t\t</ul>\n"
               "\t\t\t\t\t\t\t\t</td>\n"
               "\t\t\t\t\t\t\t</tr>\n" );

      fprintf( file,
               "\t\t\t\t\t\t</table>\n" );
      if ( sim -> iterations < 10000 )
        fprintf( file,
                 "\t\t\t\t<div class=\"alert\">\n"
                 "\t\t\t\t\t<h3>Warning</h3>\n"
                 "\t\t\t\t\t<p>Scale Factors generated using less than 10,000 iterations may vary significantly from run to run.</p>\n"
                 "\t\t\t\t</div>\n" );
    }
  }
  fprintf( file,
           "\t\t\t\t\t</div>\n"
           "\t\t\t\t</div>\n" );
}

// print_html_player_action_priority_list ===================================

static void print_html_player_action_priority_list( FILE* file, sim_t* sim, player_t* p )
{

  fprintf( file,
           "\t\t\t\t\t\t<div class=\"player-section action-priority-list\">\n"
           "\t\t\t\t\t\t\t<h3 class=\"toggle\">Action Priority List</h3>\n"
           "\t\t\t\t\t\t\t<div class=\"toggle-content hide\">\n"
           "\t\t\t\t\t\t\t\t<table class=\"sc\">\n"
           "\t\t\t\t\t\t\t\t\t<tr>\n"
           "\t\t\t\t\t\t\t\t\t\t<th class=\"right\">#</th>\n"
           "\t\t\t\t\t\t\t\t\t\t<th class=\"left\">action,conditions</th>\n"
           "\t\t\t\t\t\t\t\t\t</tr>\n" );
  int i = 1;
  for ( action_t* a = p -> action_list; a; a = a -> next )
  {
    if ( a -> signature_str.empty() || ! a -> marker ) continue;
    fprintf( file,
             "\t\t\t\t\t\t\t\t<tr" );
    if ( !( i & 1 ) )
    {
      fprintf( file, " class=\"odd\"" );
    }
    fprintf( file, ">\n" );
    fprintf( file,
             "\t\t\t\t\t\t\t\t\t\t<th class=\"right\">%c</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n",
             a -> marker,
             report_t::encode_html( a -> signature_str ).c_str() );
    i++;
  }
  fprintf( file,
           "\t\t\t\t\t\t\t\t</table>\n" );

  if ( ! p -> action_sequence.empty() )
  {
    std::string& seq = p -> action_sequence;
    if ( seq.size() > 0 )
    {
      fprintf( file,
               "\t\t\t\t\t\t\t\t<div class=\"subsection subsection-small\">\n"
               "\t\t\t\t\t\t\t\t\t<h4>Sample Sequence</h4>\n"
               "\t\t\t\t\t\t\t\t\t<div class=\"force-wrap mono\">\n"
               "                    %s\n"
               "\t\t\t\t\t\t\t\t\t</div>\n"
               "\t\t\t\t\t\t\t\t</div>\n",
               seq.c_str() );

      fprintf( file,
               "\t\t\t\t\t\t\t\t<div class=\"subsection subsection-small\">\n"
               "\t\t\t\t\t\t\t\t\t<h4>Labels</h4>\n"
               "\t\t\t\t\t\t\t\t\t<div class=\"force-wrap mono\">\n"
               "                    %s\n"
               "\t\t\t\t\t\t\t\t\t</div>\n"
               "\t\t\t\t\t\t\t\t</div>\n",
               p -> print_action_map( sim -> iterations, 2 ).c_str() );
    }
  }

  fprintf( file,
           "\t\t\t\t\t\t\t</div>\n"
           "\t\t\t\t\t\t</div>\n" );

}

// print_html_player_statistics =============================================

static void print_html_player_statistics( FILE* file, player_t* p )
{

// Statistics & Data Analysis

  fputs( "\t\t\t\t\t<div class=\"player-section gains\">\n"
         "\t\t\t\t\t\t<h3 class=\"toggle\">Statistics & Data Analysis</h3>\n"
         "\t\t\t\t\t\t<div class=\"toggle-content hide\">\n"
         "\t\t\t\t\t\t\t<table  class=\"sc\">\n"
         "\t\t\t\t\t\t\t\t<tr>\n"
         "\t\t\t\t\t\t\t\t<td>\n", file );

  print_html_sample_data( file, p, p -> fight_length, "Fight Length" );

  print_html_sample_data( file, p, p -> dps, "DPS" );

  print_html_sample_data( file, p, p -> dpse, "DPS(e)" );

  print_html_sample_data( file, p, p -> dmg, "Damage" );

  print_html_sample_data( file, p, p -> dtps, "DTPS" );

  print_html_sample_data( file, p, p -> hps, "HPS" );

  print_html_sample_data( file, p, p -> hpse, "HPS(e)" );

  print_html_sample_data( file, p, p -> heal, "Heal" );

  print_html_sample_data( file, p, p -> htps, "HTPS" );

  print_html_sample_data( file, p, p -> executed_foreground_actions, "#Executed Foreground Actions" );

  if ( ! p -> dps_error_chart.empty() )
    fprintf( file, "<img src=\"%s\" alt=\"DPS Error Chart\" />\n", p -> dps_error_chart.c_str() );

  fputs( "\t\t\t\t\t\t\t</td>\n"
         "\t\t\t\t\t\t\t</tr>\n"
         "\t\t\t\t\t\t\t</table>\n"
         "\t\t\t\t\t\t\t</div>\n"
         "\t\t\t\t\t\t</div>\n", file );
}

// print_html_player_resources ==============================================

static void print_html_player_resources( FILE* file, player_t* p )
{
// Resources Section

  fprintf( file,
           "\t\t\t\t<div class=\"player-section gains\">\n"
           "\t\t\t\t\t<h3 class=\"toggle\">Resources</h3>\n"
           "\t\t\t\t\t<div class=\"toggle-content hide\">\n"
           "\t\t\t\t\t\t<table class=\"sc mt\">\n"
           "\t\t\t\t\t\t\t<tr>\n"
           "\t\t\t\t\t\t\t\t<th class=\"left small\">Resource Usage</th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\">Type</th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\">Res%%</th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-dpr\" class=\"help\">DPR</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\">RPE</th>\n"
           "\t\t\t\t\t\t\t</tr>\n" );

  fprintf( file,
           "\t\t\t\t\t\t\t<tr>\n"
           "\t\t\t\t\t\t\t\t<th class=\"left small\">%s</th>\n"
           "\t\t\t\t\t\t\t\t<td colspan=\"4\" class=\"filler\"></td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           p -> name() );

  int i = 0;
  for ( stats_t* s = p -> stats_list; s; s = s -> next )
  {
    if ( s -> rpe > 0 )
    {
      print_html_action_resource( file, s, i );
      i++;
    }
  }

  for ( pet_t* pet = p -> pet_list; pet; pet = pet -> next_pet )
  {
    bool first=true;

    i = 0;
    for ( stats_t* s = pet -> stats_list; s; s = s -> next )
    {
      if ( s -> rpe > 0 )
      {
        if ( first )
        {
          first = false;
          fprintf( file,
                   "\t\t\t\t\t\t\t<tr>\n"
                   "\t\t\t\t\t\t\t\t<th class=\"left small\">pet - %s</th>\n"
                   "\t\t\t\t\t\t\t\t<td colspan=\"4\" class=\"filler\"></td>\n"
                   "\t\t\t\t\t\t\t</tr>\n",
                   pet -> name_str.c_str() );
        }
        print_html_action_resource( file, s, i );
        i++;
      }
    }
  }

  fprintf( file,
           "\t\t\t\t\t\t</table>\n" );

// Resource Gains Section
  fprintf( file,
           "\t\t\t\t\t\t<table class=\"sc\">\n"
           "\t\t\t\t\t\t\t<tr>\n"
           "\t\t\t\t\t\t\t\t<th>Resource Gains</th>\n"
           "\t\t\t\t\t\t\t\t<th>Type</th>\n"
           "\t\t\t\t\t\t\t\t<th>Count</th>\n"
           "\t\t\t\t\t\t\t\t<th>Total</th>\n"
           "\t\t\t\t\t\t\t\t<th>Average</th>\n"
           "\t\t\t\t\t\t\t\t<th colspan=\"2\">Overflow</th>\n"
           "\t\t\t\t\t\t\t</tr>\n" );
  i = 1;
  for ( gain_t* g = p -> gain_list; g; g = g -> next )
  {
    if ( g -> actual > 0 || g -> overflow > 0 )
    {
      double overflow_pct = 100.0 * g -> overflow / ( g -> actual + g -> overflow );
      fprintf( file,
               "\t\t\t\t\t\t\t\t<tr" );
      if ( !( i & 1 ) )
      {
        fprintf( file, " class=\"odd\"" );
      }
      fprintf( file, ">\n" );
      fprintf( file,
               "\t\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n"
               "\t\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n"
               "\t\t\t\t\t\t\t\t<td class=\"right\">%.1f</td>\n"
               "\t\t\t\t\t\t\t\t<td class=\"right\">%.1f</td>\n"
               "\t\t\t\t\t\t\t\t<td class=\"right\">%.1f</td>\n"
               "\t\t\t\t\t\t\t\t<td class=\"right\">%.1f</td>\n"
               "\t\t\t\t\t\t\t\t<td class=\"right\">%.1f%%</td>\n"
               "\t\t\t\t\t\t\t</tr>\n",
               g -> name(),
               util_t::resource_type_string( g -> type ),
               g -> count,
               g -> actual,
               g -> actual / g -> count,
               g -> overflow,
               overflow_pct );
      i++;
    }
  }
  for ( pet_t* pet = p -> pet_list; pet; pet = pet -> next_pet )
  {
    if ( pet -> fight_length.mean <= 0 ) continue;
    bool first = true;
    for ( gain_t* g = pet -> gain_list; g; g = g -> next )
    {
      if ( g -> actual > 0 || g -> overflow > 0 )
      {
        if ( first )
        {
          first = false;
          fprintf( file,
                   "\t\t\t\t\t\t\t<tr>\n"
                   "\t\t\t\t\t\t\t\t<th>pet - %s</th>\n"
                   "\t\t\t\t\t\t\t\t<td colspan=\"6\" class=\"filler\"></td>\n"
                   "\t\t\t\t\t\t\t</tr>\n",
                   pet -> name_str.c_str() );
        }
        double overflow_pct = 100.0 * g -> overflow / ( g -> actual + g -> overflow );
        fprintf( file,
                 "\t\t\t\t\t\t\t<tr>\n"
                 "\t\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n"
                 "\t\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n"
                 "\t\t\t\t\t\t\t\t<td class=\"right\">%.1f</td>\n"
                 "\t\t\t\t\t\t\t\t<td class=\"right\">%.1f</td>\n"
                 "\t\t\t\t\t\t\t\t<td class=\"right\">%.1f</td>\n"
                 "\t\t\t\t\t\t\t\t<td class=\"right\">%.1f</td>\n"
                 "\t\t\t\t\t\t\t\t<td class=\"right\">%.1f%%</td>\n"
                 "\t\t\t\t\t\t\t</tr>\n",
                 g -> name(),
                 util_t::resource_type_string( g -> type ),
                 g -> count,
                 g -> actual,
                 g -> actual / g -> count,
                 g -> overflow,
                 overflow_pct );
      }
    }
  }
  fprintf( file,
           "\t\t\t\t\t\t</table>\n" );

  fprintf( file,
           "\t\t\t\t\t\t<div class=\"charts charts-left\">\n" );
  for ( resource_type i = RESOURCE_NONE; i < RESOURCE_MAX; ++i )
  {
    double total_gain=0;
    for ( gain_t* g = p -> gain_list; g; g = g -> next )
    {
      if ( g -> actual > 0 && g -> type == i )
        total_gain += g -> actual;
    }

    if ( total_gain > 0 )
    {
      chart_t::gains( p -> gains_chart, p, i );
      if ( ! p -> gains_chart.empty() )
      {
        fprintf( file,
                 "\t\t\t\t\t\t\t<img src=\"%s\" alt=\"Resource Gains Chart\" />\n",
                 p -> gains_chart.c_str() );
      }
    }
  }
  fprintf( file,
           "\t\t\t\t\t\t</div>\n" );


  fprintf( file,
           "\t\t\t\t\t\t<div class=\"charts\">\n" );
  for ( resource_type j = RESOURCE_NONE; ++j < RESOURCE_MAX; )
  {
    if ( p -> resource_max[ j ] > 0 && ! p -> timeline_resource_chart[ j ].empty() )
    {
      fprintf( file,
               "\t\t\t\t\t\t<img src=\"%s\" alt=\"Resource Timeline Chart\" />\n",
               p -> timeline_resource_chart[ j ].c_str() );
    }
  }
  fprintf( file,
           "\t\t\t\t\t\t</div>\n"
           "\t\t\t\t\t<div class=\"clear\"></div>\n" );

  fprintf( file,
           "\t\t\t\t\t</div>\n"
           "\t\t\t\t</div>\n" );
}

// print_html_player_charts =================================================

static void print_html_player_charts( FILE* file, sim_t* sim, player_t* p )
{
  const size_t num_players = sim -> players_by_name.size();

  fputs( "\t\t\t\t<div class=\"player-section\">\n"
         "\t\t\t\t\t<h3 class=\"toggle open\">Charts</h3>\n"
         "\t\t\t\t\t<div class=\"toggle-content\">\n"
         "\t\t\t\t\t\t<div class=\"charts charts-left\">\n", file );

  if ( ! p -> action_dpet_chart.empty() )
  {
    const char* fmt;
    if ( num_players == 1 )
      fmt = "\t\t\t\t\t\t\t<img src=\"%s\" alt=\"Action DPET Chart\" />\n";
    else
      fmt = "\t\t\t\t\t\t\t<span class=\"chart-action-dpet\" title=\"Action DPET Chart\">%s</span>\n";
    fprintf( file, fmt, p -> action_dpet_chart.c_str() );
  }

  if ( ! p -> action_dmg_chart.empty() )
  {
    const char* fmt;
    if ( num_players == 1 )
      fmt = "\t\t\t\t\t\t\t<img src=\"%s\" alt=\"Action Damage Chart\" />\n";
    else
      fmt = "\t\t\t\t\t\t\t<span class=\"chart-action-dmg\" title=\"Action Damage Chart\">%s</span>\n";
    fprintf( file, fmt, p -> action_dmg_chart.c_str() );
  }

  if ( ! p -> scaling_dps_chart.empty() )
  {
    const char* fmt;
    if ( num_players == 1 )
      fmt = "\t\t\t\t\t\t\t<img src=\"%s\" alt=\"Scaling DPS Chart\" />\n";
    else
      fmt = "\t\t\t\t\t\t\t<span class=\"chart-scaling-dps\" title=\"Scaling DPS Chart\">%s</span>\n";
    fprintf( file, fmt, p -> scaling_dps_chart.c_str() );
  }

  fputs( "\t\t\t\t\t\t</div>\n"
         "\t\t\t\t\t\t<div class=\"charts\">\n", file );

  if ( ! p -> reforge_dps_chart.empty() )
  {
    const char* fmt;
    if ( p -> sim -> reforge_plot -> reforge_plot_stat_indices.size() == 2 )
    {
      if ( num_players == 1 )
        fmt = "\t\t\t\t\t\t\t<img src=\"%s\" alt=\"Reforge DPS Chart\" />\n";
      else
        fmt = "\t\t\t\t\t\t\t<span class=\"chart-reforge-dps\" title=\"Reforge DPS Chart\">%s</span>\n";
    }
    else
    {
      if ( true )
        fmt = "\t\t\t\t\t\t\t%s";
      else
      {
        if ( num_players == 1 )
          fmt = "\t\t\t\t\t\t\t<iframe>%s</iframe>\n";
        else
          fmt = "\t\t\t\t\t\t\t<span class=\"chart-reforge-dps\" title=\"Reforge DPS Chart\">%s</span>\n";
      }
    }
    fprintf( file, fmt, p -> reforge_dps_chart.c_str() );
  }

  if ( ! p -> scale_factors_chart.empty() )
  {
    const char* fmt;
    if ( num_players == 1 )
      fmt = "\t\t\t\t\t\t\t<img src=\"%s\" alt=\"Scale Factors Chart\" />\n";
    else
      fmt = "\t\t\t\t\t\t\t<span class=\"chart-scale-factors\" title=\"Scale Factors Chart\">%s</span>\n";
    fprintf( file, fmt, p -> scale_factors_chart.c_str() );
  }

  if ( ! p -> timeline_dps_chart.empty() )
  {
    const char* fmt;
    if ( num_players == 1 )
      fmt = "\t\t\t\t\t\t\t<img src=\"%s\" alt=\"DPS Timeline Chart\" />\n";
    else
      fmt = "\t\t\t\t\t\t\t<span class=\"chart-timeline-dps\" title=\"DPS Timeline Chart\">%s</span>\n";
    fprintf( file, fmt, p -> timeline_dps_chart.c_str() );
  }

  if ( ! p -> distribution_dps_chart.empty() )
  {
    const char* fmt;
    if ( num_players == 1 )
      fmt = "\t\t\t\t\t\t\t<img src=\"%s\" alt=\"DPS Distribution Chart\" />\n";
    else
      fmt = "\t\t\t\t\t\t\t<span class=\"chart-distribution-dps\" title=\"DPS Distribution Chart\">%s</span>\n";
    fprintf( file, fmt, p -> distribution_dps_chart.c_str() );
  }

  if ( ! p -> time_spent_chart.empty() )
  {
    const char* fmt;
    if ( num_players == 1 )
      fmt = "\t\t\t\t\t\t\t<img src=\"%s\" alt=\"Time Spent Chart\" />\n";
    else
      fmt = "\t\t\t\t\t\t\t<span class=\"chart-time-spent\" title=\"Time Spent Chart\">%s</span>\n";
    fprintf( file, fmt, p -> time_spent_chart.c_str() );
  }

  fputs( "\t\t\t\t\t\t</div>\n"
         "\t\t\t\t\t\t<div class=\"clear\"></div>\n"
         "\t\t\t\t\t</div>\n"
         "\t\t\t\t</div>\n", file );
}


// print_html_player_buffs ==================================================

static inline bool buff_comp( const buff_t* i, const buff_t* j )
{
  // Aura & Buff / Pet
  if ( ( ! i -> player || ! i -> player -> is_pet() ) && j -> player && j -> player -> is_pet() )
    return true;
  // Pet / Aura & Buff
  else if ( i -> player && i -> player -> is_pet() && ( ! j -> player || ! j -> player -> is_pet() ) )
    return false;
  // Pet / Pet
  else if ( i -> player && i -> player -> is_pet() && j -> player && j -> player -> is_pet() )
  {
    if ( i -> player -> name_str.compare( j -> player -> name_str ) == 0 )
      return ( i -> name_str.compare( j -> name_str ) < 0 );
    else
      return ( i -> player -> name_str.compare( j -> player -> name_str ) < 0 );
  }

  return ( i -> name_str.compare( j -> name_str ) < 0 );
}

static void print_html_player_buffs( FILE* file, player_t* p )
{
  int i=0;
  // Buff Section
  fprintf( file,
           "\t\t\t\t<div class=\"player-section buffs\">\n"
           "\t\t\t\t\t<h3 class=\"toggle open\">Buffs</h3>\n"
           "\t\t\t\t\t<div class=\"toggle-content\">\n" );

  // Dynamic Buffs table
  fprintf( file,
           "\t\t\t\t\t\t<table class=\"sc mb\">\n"
           "\t\t\t\t\t\t\t<tr>\n"
           "\t\t\t\t\t\t\t\t<th class=\"left\"><a href=\"#help-dynamic-buffs\" class=\"help\">Dynamic Buffs</a></th>\n"
           "\t\t\t\t\t\t\t\t<th>Start</th>\n"
           "\t\t\t\t\t\t\t\t<th>Refresh</th>\n"
           "\t\t\t\t\t\t\t\t<th>Interval</th>\n"
           "\t\t\t\t\t\t\t\t<th>Trigger</th>\n"
           "\t\t\t\t\t\t\t\t<th>Up-Time</th>\n"
           "\t\t\t\t\t\t\t\t<th>Benefit</th>\n"
           "\t\t\t\t\t\t\t</tr>\n" );

  std::vector< buff_t* > dynamic_buffs;
  for ( buff_t* b = p -> buff_list; b; b = b -> next )
    if ( ! b -> quiet && b -> start_count && ! b -> constant )
      dynamic_buffs.push_back( b );
  for ( pet_t* pet = p -> pet_list; pet; pet = pet -> next_pet )
    for ( buff_t* b = pet -> buff_list; b; b = b -> next )
      if ( ! b -> quiet && b -> start_count && ! b -> constant )
        dynamic_buffs.push_back( b );
  for ( buff_t* b = p -> sim -> buff_list; b; b = b -> next )
    if ( ! b -> quiet && b -> start_count && ! b -> constant )
      dynamic_buffs.push_back( b );

  boost::sort( dynamic_buffs, buff_comp );

  for ( i=0; i < ( int ) dynamic_buffs.size(); i++ )
  {
    buff_t* b = dynamic_buffs[ i ];

    std::string buff_name;
    if ( b -> player && b -> player -> is_pet() )
    {
      buff_name += b -> player -> name_str + '-';
    }
    buff_name += b -> name();

    fprintf( file,
             "\t\t\t\t\t\t\t<tr" );
    if ( i & 1 )
    {
      fprintf( file, " class=\"odd\"" );
    }
    fprintf( file, ">\n" );
    if ( p -> sim -> report_details )
      fprintf( file,
               "\t\t\t\t\t\t\t\t<td class=\"left\"><a href=\"#\" class=\"toggle-details\">%s</a></td>\n",
               buff_name.c_str() );
    else
      fprintf( file,
               "\t\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n",
               buff_name.c_str() );
    fprintf( file,
             "\t\t\t\t\t\t\t\t<td class=\"right\">%.1f</td>\n"
             "\t\t\t\t\t\t\t\t<td class=\"right\">%.1f</td>\n"
             "\t\t\t\t\t\t\t\t<td class=\"right\">%.1fsec</td>\n"
             "\t\t\t\t\t\t\t\t<td class=\"right\">%.1fsec</td>\n"
             "\t\t\t\t\t\t\t\t<td class=\"right\">%.0f%%</td>\n"
             "\t\t\t\t\t\t\t\t<td class=\"right\">%.0f%%</td>\n"
             "\t\t\t\t\t\t\t</tr>\n",
             b -> avg_start,
             b -> avg_refresh,
             b -> avg_start_interval,
             b -> avg_trigger_interval,
             b -> uptime_pct.mean,
             ( b -> benefit_pct > 0 ? b -> benefit_pct : b -> uptime_pct.mean ) );

    if ( p -> sim -> report_details )
    {
      fprintf( file,
               "\t\t\t\t\t\t\t<tr class=\"details hide\">\n"
               "\t\t\t\t\t\t\t\t<td colspan=\"7\" class=\"filler\">\n"
               "\t\t\t\t\t\t\t\t\t<h4>Database details</h4>\n"
               "\t\t\t\t\t\t\t\t\t<ul>\n"
               "\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">cooldown name:</span>%s</li>\n"
               "\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">max_stacks:</span>%.i</li>\n"
               "\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">duration:</span>%.2f</li>\n"
               "\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">cooldown:</span>%.2f</li>\n"
               "\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">default_chance:</span>%.2f%%</li>\n"
               "\t\t\t\t\t\t\t\t\t</ul>\n"
               "\t\t\t\t\t\t\t\t</td>\n",
               b -> cooldown -> name_str.c_str(),
               b -> max_stack,
               to_seconds( b -> buff_duration ),
               to_seconds( b -> cooldown -> duration ),
               b -> default_chance * 100 );

      fprintf( file,
               "\t\t\t\t\t\t\t\t<td colspan=\"7\" class=\"filler\">\n"
               "\t\t\t\t\t\t\t\t\t<h4>Stack Uptimes</h4>\n"
               "\t\t\t\t\t\t\t\t\t<ul>\n" );
      for ( size_t j = 0; j < b -> stack_uptime.size(); j++ )
      {
        double uptime = b -> stack_uptime[ j ].uptime;
        if ( uptime > 0 )
        {
          fprintf( file,
                   "\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">%s_%d:</span>%.1f%%</li>\n",
                   b -> name(), j,
                   uptime * 100.0 );
        }
      }
      fprintf( file,
               "\t\t\t\t\t\t\t\t\t</ul>\n"
               "\t\t\t\t\t\t\t\t</td>\n"
               "\t\t\t\t\t\t\t</tr>\n" );
    }
  }
  fprintf( file,
           "\t\t\t\t\t\t\t</table>\n" );

  // constant buffs
  if ( !p -> is_pet() )
  {
    fprintf( file,
             "\t\t\t\t\t\t\t<table class=\"sc\">\n"
             "\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t<th class=\"left\"><a href=\"#help-constant-buffs\" class=\"help\">Constant Buffs</a></th>\n"
             "\t\t\t\t\t\t\t\t</tr>\n" );
    i = 1;
    std::vector< buff_t* > constant_buffs;

    for ( buff_t* b = p -> buff_list; b; b = b -> next )
      if ( ! b -> quiet && b -> start_count && b -> constant )
        constant_buffs.push_back( b );
    for ( buff_t* b = p -> sim -> buff_list; b; b = b -> next )
      if ( ! b -> quiet && b -> start_count && b -> constant )
        constant_buffs.push_back( b );

    boost::sort( constant_buffs, buff_comp );

    for ( auto const& b : constant_buffs | boost::adaptors::indirected )
    {
      fprintf( file,
               "\t\t\t\t\t\t\t<tr" );
      if ( !( i & 1 ) )
      {
        fprintf( file, " class=\"odd\"" );
      }
      fprintf( file, ">\n" );
      if ( p -> sim -> report_details )
      {
        fprintf( file,
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\"><a href=\"#\" class=\"toggle-details\">%s</a></td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 b.name() );


        fprintf( file,
                 "\t\t\t\t\t\t\t\t<tr class=\"details hide\">\n"
                 "\t\t\t\t\t\t\t\t\t<td>\n"
                 "\t\t\t\t\t\t\t\t\t\t<h4>Database details</h4>\n"
                 "\t\t\t\t\t\t\t\t\t\t<ul>\n"
                 "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">cooldown name:</span>%s</li>\n"
                 "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">max_stacks:</span>%.i</li>\n"
                 "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">duration:</span>%.2f</li>\n"
                 "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">cooldown:</span>%.2f</li>\n"
                 "\t\t\t\t\t\t\t\t\t\t\t<li><span class=\"label\">default_chance:</span>%.2f%%</li>\n"
                 "\t\t\t\t\t\t\t\t\t\t</ul>\n"
                 "\t\t\t\t\t\t\t\t\t</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 b.cooldown -> name_str.c_str(),
                 b.max_stack,
                 to_seconds( b.buff_duration ),
                 to_seconds( b.cooldown -> duration ),
                 b.default_chance * 100 );
      }
      else
        fprintf( file,
                 "\t\t\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n"
                 "\t\t\t\t\t\t\t\t</tr>\n",
                 b.name() );

      i++;
    }
    fprintf( file,
             "\t\t\t\t\t\t\t</table>\n" );
  }


  fprintf( file,
           "\t\t\t\t\t\t</div>\n"
           "\t\t\t\t\t</div>\n" );

}

// print_html_player ========================================================

static void print_html_player_description( FILE* file, sim_t* sim, player_t* p, int j, std::string& n )
{

  int num_players = ( int ) sim -> players_by_name.size();

  // Player Description
  fprintf( file,
           "\t\t<div id=\"%s\" class=\"player section",
           n.c_str() );
  if ( num_players > 1 && j == 0 && ! sim -> scaling -> has_scale_factors() && p -> type != ENEMY && p -> type != ENEMY_ADD )
  {
    fprintf( file, " grouped-first" );
  }
  else if ( ( p -> type == ENEMY || p -> type == ENEMY_ADD ) && j == ( int ) sim -> targets_by_name.size() - 1 )
  {
    fprintf( file, " final grouped-last" );
  }
  else if ( num_players == 1 )
  {
    fprintf( file, " section-open" );
  }
  fprintf( file, "\">\n" );

  if ( ! p -> thumbnail_url.empty()  )
    fprintf( file,
             "\t\t\t<a href=\"%s\" class=\"toggle-thumbnail%s\"><img src=\"%s\" alt=\"%s\" class=\"player-thumbnail\"/></a>\n",
             p -> origin_str.c_str(), ( num_players == 1 ) ? "" : " hide",
             p -> thumbnail_url.c_str(), p -> name_str.c_str() );

  fprintf( file,
           "\t\t\t<h2 class=\"toggle" );
  if ( num_players == 1 )
  {
    fprintf( file, " open" );
  }

  if ( p -> dps.mean >= p -> hps.mean )
    fprintf( file, "\">%s&nbsp;:&nbsp;%.0f dps</h2>\n",
             n.c_str(),
             p -> dps.mean );
  else
    fprintf( file, "\">%s&nbsp;:&nbsp;%.0f hps</h2>\n",
             n.c_str(),
             p -> hps.mean );

  fprintf( file,
           "\t\t\t<div class=\"toggle-content" );
  if ( num_players > 1 )
  {
    fprintf( file, " hide" );
  }
  fprintf( file, "\">\n" );

  fprintf( file,
           "\t\t\t\t<ul class=\"params\">\n"
           "\t\t\t\t\t<li><b>Race:</b> %s</li>\n"
           "\t\t\t\t\t<li><b>Class:</b> %s</li>\n",
           p -> race_str.c_str(),
           p -> is_pet() ? util_t::pet_type_string( p -> cast_pet() -> pettype ) :util_t::player_type_string( p -> type )
         );

  if ( p -> primary_tree() != TREE_NONE )
    fprintf( file,
             "\t\t\t\t\t<li><b>Tree:</b> %s</li>\n",
             util_t::talent_tree_string( p -> primary_tree() ) );

  fprintf( file,
           "\t\t\t\t\t<li><b>Level:</b> %d</li>\n"
           "\t\t\t\t\t<li><b>Role:</b> %s</li>\n"
           "\t\t\t\t\t<li><b>Position:</b> %s</li>\n"
           "\t\t\t\t</ul>\n"
           "\t\t\t\t<div class=\"clear\"></div>\n",
           p -> level, util_t::role_type_string( p -> primary_role() ), p -> position_str.c_str() );

}

// print_html_player_results_spec_gear ========================================================

static void print_html_player_results_spec_gear( FILE* file, sim_t* sim, player_t* p )
{

  // Main player table
  fprintf( file,
           "\t\t\t\t<div class=\"player-section results-spec-gear mt\">\n" );
  if ( p -> is_pet() )
  {
    fprintf( file,
             "\t\t\t\t\t<h3 class=\"toggle open\">Results</h3>\n" );
  }
  else
  {
    fprintf( file,
             "\t\t\t\t\t<h3 class=\"toggle open\">Results, Spec and Gear</h3>\n" );
  }
  fprintf( file,
           "\t\t\t\t\t<div class=\"toggle-content\">\n"
           "\t\t\t\t\t\t<table class=\"sc\">\n"
           "\t\t\t\t\t\t\t<tr>\n" );
  // Damage
  if ( p -> dps.mean > 0 )
    util_t::fprintf( file,
                     "\t\t\t\t\t\t\t\t<th><a href=\"#help-dps\" class=\"help\">DPS</a></th>\n"
                     "\t\t\t\t\t\t\t\t<th><a href=\"#help-dpse\" class=\"help\">DPS(e)</a></th>\n"
                     "\t\t\t\t\t\t\t\t<th><a href=\"#help-error\" class=\"help\">DPS Error</a></th>\n"
                     "\t\t\t\t\t\t\t\t<th><a href=\"#help-range\" class=\"help\">DPS Range</a></th>\n"
                     "\t\t\t\t\t\t\t\t<th><a href=\"#help-dpr\" class=\"help\">DPR</a></th>\n" );
  // Heal
  if ( p -> hps.mean > 0 )
    util_t::fprintf( file,
                     "\t\t\t\t\t\t\t\t<th><a href=\"#help-dps\" class=\"help\">HPS</a></th>\n"
                     "\t\t\t\t\t\t\t\t<th><a href=\"#help-dpse\" class=\"help\">HPS(e)</a></th>\n"
                     "\t\t\t\t\t\t\t\t<th><a href=\"#help-error\" class=\"help\">HPS Error</a></th>\n"
                     "\t\t\t\t\t\t\t\t<th><a href=\"#help-range\" class=\"help\">HPS Range</a></th>\n"
                     "\t\t\t\t\t\t\t\t<th><a href=\"#help-dpr\" class=\"help\">HPR</a></th>\n" );
  util_t::fprintf( file,
                   "\t\t\t\t\t\t\t\t<th><a href=\"#help-rps-out\" class=\"help\">RPS Out</a></th>\n"
                   "\t\t\t\t\t\t\t\t<th><a href=\"#help-rps-in\" class=\"help\">RPS In</a></th>\n"
                   "\t\t\t\t\t\t\t\t<th>Resource</th>\n"
                   "\t\t\t\t\t\t\t\t<th><a href=\"#help-waiting\" class=\"help\">Waiting</a></th>\n"
                   "\t\t\t\t\t\t\t\t<th><a href=\"#help-apm\" class=\"help\">APM</a></th>\n"
                   "\t\t\t\t\t\t\t\t<th>Active</th>\n"
                   "\t\t\t\t\t\t\t</tr>\n"
                   "\t\t\t\t\t\t\t<tr>\n" );
  // Damage
  if ( p -> dps.mean > 0 )
  {
    double range = ( p -> dps.percentile( 0.95 ) - p -> dps.percentile( 0.05 ) ) / 2;
    util_t::fprintf( file,
                     "\t\t\t\t\t\t\t\t<td>%.1f</td>\n"
                     "\t\t\t\t\t\t\t\t<td>%.1f</td>\n"
                     "\t\t\t\t\t\t\t\t<td>%.2f / %.2f%%</td>\n"
                     "\t\t\t\t\t\t\t\t<td>%.0f / %.1f%%</td>\n"
                     "\t\t\t\t\t\t\t\t<td>%.1f</td>\n",
                     p -> dps.mean,
                     p -> dpse.mean,
                     p -> dps_error,
                     p -> dps.mean ? p -> dps_error * 100 / p -> dps.mean : 0,
                     range,
                     p -> dps.mean ? range / p -> dps.mean * 100.0 : 0,
                     p -> dpr );
  }
  // Heal
  if ( p -> hps.mean > 0 )
  {
    double range = ( p -> hps.percentile( 0.95 ) - p -> hps.percentile( 0.05 ) ) / 2;
    util_t::fprintf( file,
                     "\t\t\t\t\t\t\t\t<td>%.1f</td>\n"
                     "\t\t\t\t\t\t\t\t<td>%.1f</td>\n"
                     "\t\t\t\t\t\t\t\t<td>%.2f / %.2f%%</td>\n"
                     "\t\t\t\t\t\t\t\t<td>%.0f / %.1f%%</td>\n"
                     "\t\t\t\t\t\t\t\t<td>%.1f</td>\n",
                     p -> hps.mean,
                     p -> hpse.mean,
                     p -> hps_error,
                     p -> hps.mean ? p -> hps_error * 100 / p -> hps.mean : 0,
                     range,
                     p -> hps.mean ? range / p -> hps.mean * 100.0 : 0,
                     p -> hpr );
  }
  util_t::fprintf( file,
                   "\t\t\t\t\t\t\t\t<td>%.1f</td>\n"
                   "\t\t\t\t\t\t\t\t<td>%.1f</td>\n"
                   "\t\t\t\t\t\t\t\t<td>%s</td>\n"
                   "\t\t\t\t\t\t\t\t<td>%.2f%%</td>\n"
                   "\t\t\t\t\t\t\t\t<td>%.1f</td>\n"
                   "\t\t\t\t\t\t\t\t<td>%.1f%%</td>\n"
                   "\t\t\t\t\t\t\t</tr>\n"
                   "\t\t\t\t\t\t</table>\n",
                   p -> rps_loss,
                   p -> rps_gain,
                   util_t::resource_type_string( p -> primary_resource() ),
                   p -> fight_length.mean ? 100.0 * p -> waiting_time.mean / p -> fight_length.mean : 0,
                   p -> fight_length.mean ? 60.0 * p -> executed_foreground_actions.mean / p -> fight_length.mean : 0,
                   sim -> simulation_length.mean ? p -> fight_length.mean / sim -> simulation_length.mean * 100.0 : 0 );

  // Spec and gear
  if ( ! p -> is_pet() )
  {
    fprintf( file,
             "\t\t\t\t\t\t<table class=\"sc mt\">\n" );
    if ( p -> origin_str.compare( "unknown" ) )
    {
      std::string enc_url = p -> origin_str;
      util_t::urldecode( enc_url );
      fprintf( file,
               "\t\t\t\t\t\t\t<tr class=\"left\">\n"
               "\t\t\t\t\t\t\t\t<th><a href=\"#help-origin\" class=\"help\">Origin</a></th>\n"
               "\t\t\t\t\t\t\t\t<td><a href=\"%s\" class=\"ext\">%s</a></td>\n"
               "\t\t\t\t\t\t\t</tr>\n",
               p -> origin_str.c_str(),
               report_t::encode_html( enc_url ).c_str() );
    }
    if ( ! p -> talents_str.empty() )
    {
      std::string enc_url = report_t::encode_html( p -> talents_str );
      fprintf( file,
               "\t\t\t\t\t\t\t<tr class=\"left\">\n"
               "\t\t\t\t\t\t\t\t<th>Talents</th>\n"
               "\t\t\t\t\t\t\t\t<td><a href=\"%s\" class=\"ext\">%s</a></td>\n"
               "\t\t\t\t\t\t\t</tr>\n",
               enc_url.c_str(),
               enc_url.c_str() );
    }

#if 0
    std::vector<std::string> glyph_names;
    int num_glyphs = util_t::string_split( glyph_names, p -> glyphs_str, ",/" );
    if ( num_glyphs )
    {
      fprintf( file,
               "\t\t\t\t\t\t\t<tr class=\"left\">\n"
               "\t\t\t\t\t\t\t\t<th>Glyphs</th>\n"
               "\t\t\t\t\t\t\t\t<td>\n"
               "\t\t\t\t\t\t\t\t\t<ul class=\"float\">\n" );
      for ( int i=0; i < num_glyphs; i++ )
      {
        fprintf( file,
                 "\t\t\t\t\t\t\t\t\t\t<li>%s</li>\n",
                 glyph_names[ i ].c_str() );
      }
      fprintf( file,
               "\t\t\t\t\t\t\t\t\t</ul>\n"
               "\t\t\t\t\t\t\t\t</td>\n"
               "\t\t\t\t\t\t\t</tr>\n" );
    }
#endif

    fprintf( file,
             "\t\t\t\t\t\t</table>\n" );
  }
}

// print_html_player_abilities ========================================================

static void print_html_player_abilities( FILE* file, sim_t* sim, player_t* p, std::string& n )
{

  // Abilities Section
  fprintf( file,
           "\t\t\t\t<div class=\"player-section\">\n"
           "\t\t\t\t\t<h3 class=\"toggle open\">Abilities</h3>\n"
           "\t\t\t\t\t<div class=\"toggle-content\">\n"
           "\t\t\t\t\t\t<table class=\"sc\">\n"
           "\t\t\t\t\t\t\t<tr>\n"
           "\t\t\t\t\t\t\t\t<th class=\"left small\">Damage Stats</th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-dps\" class=\"help\">DPS</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-dps-pct\" class=\"help\">DPS%%</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-count\" class=\"help\">Count</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-interval\" class=\"help\">Interval</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-dpe\" class=\"help\">DPE</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-dpet\" class=\"help\">DPET</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-hit\" class=\"help\">Hit</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-crit\" class=\"help\">Crit</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-avg\" class=\"help\">Avg</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-crit-pct\" class=\"help\">Crit%%</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-miss-pct\" class=\"help\">Avoid%%</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-block-pct\" class=\"help\">B%%</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-ticks\" class=\"help\">Ticks</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-ticks-hit\" class=\"help\">T-Hit</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-ticks-crit\" class=\"help\">T-Crit</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-ticks-avg\" class=\"help\">T-Avg</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-ticks-crit-pct\" class=\"help\">T-Crit%%</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-ticks-miss-pct\" class=\"help\">T-Avoid%%</a></th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"small\"><a href=\"#help-ticks-uptime\" class=\"help\">Up%%</a></th>\n"
           "\t\t\t\t\t\t\t</tr>\n" );

  fprintf( file,
           "\t\t\t\t\t\t\t<tr>\n"
           "\t\t\t\t\t\t\t\t<th class=\"left small\">%s</th>\n"
           "\t\t\t\t\t\t\t\t<th class=\"right small\">%.0f</th>\n"
           "\t\t\t\t\t\t\t\t<td colspan=\"18\" class=\"filler\"></td>\n"
           "\t\t\t\t\t\t\t</tr>\n",
           n.c_str(),
           p -> dps.mean );

  int i = 0;
  for ( stats_t* s = p -> stats_list; s; s = s -> next )
  {
    if ( s -> num_executes > 1 || s -> compound_amount > 0 || sim -> debug )
    {
      print_html_action_damage( file, s, p, i );
      i++;
    }
  }

  for ( pet_t* pet = p -> pet_list; pet; pet = pet -> next_pet )
  {
    bool first=true;

    i = 0;
    for ( stats_t* s = pet -> stats_list; s; s = s -> next )
    {
      if ( s -> num_executes || s -> compound_amount > 0 || sim -> debug )
      {
        if ( first )
        {
          first = false;
          fprintf( file,
                   "\t\t\t\t\t\t\t<tr>\n"
                   "\t\t\t\t\t\t\t\t<th class=\"left small\">pet - %s</th>\n"
                   "\t\t\t\t\t\t\t\t<th class=\"right small\">%.0f / %.0f</th>\n"
                   "\t\t\t\t\t\t\t\t<td colspan=\"18\" class=\"filler\"></td>\n"
                   "\t\t\t\t\t\t\t</tr>\n",
                   pet -> name_str.c_str(),
                   pet -> dps.mean,
                   pet -> dpse.mean );
        }
        print_html_action_damage( file, s, p, i );
        i++;
      }
    }
  }

  fprintf( file,
           "\t\t\t\t\t\t</table>\n"
           "\t\t\t\t\t</div>\n"
           "\t\t\t\t</div>\n" );
}

// print_html_player_benefits_uptimes ========================================================

static void print_html_player_benefits_uptimes( FILE* file, player_t* p )
{
  fprintf( file,
           "\t\t\t\t\t<div class=\"player-section benefits\">\n"
           "\t\t\t\t\t\t<h3 class=\"toggle\">Benefits & Uptimes</h3>\n"
           "\t\t\t\t\t\t<div class=\"toggle-content hide\">\n"
           "\t\t\t\t\t\t\t<table class=\"sc\">\n"
           "\t\t\t\t\t\t\t\t<tr>\n"
           "\t\t\t\t\t\t\t\t\t<th>Benefits</th>\n"
           "\t\t\t\t\t\t\t\t\t<th>%%</th>\n"
           "\t\t\t\t\t\t\t\t</tr>\n" );
  int i = 1;
  for ( benefit_t* u = p -> benefit_list; u; u = u -> next )
  {
    if ( u -> ratio > 0 )
    {
      fprintf( file,
               "\t\t\t\t\t\t\t\t<tr" );
      if ( !( i & 1 ) )
      {
        fprintf( file, " class=\"odd\"" );
      }
      fprintf( file, ">\n" );
      fprintf( file,
               "\t\t\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n"
               "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.1f%%</td>\n"
               "\t\t\t\t\t\t\t\t</tr>\n",
               u -> name(),
               u -> ratio * 100.0 );
      i++;
    }
  }
  fprintf( file,
           "\t\t\t\t\t\t\t\t<tr>\n"
           "\t\t\t\t\t\t\t\t\t<th>Uptimes</th>\n"
           "\t\t\t\t\t\t\t\t\t<th>%%</th>\n"
           "\t\t\t\t\t\t\t\t</tr>\n" );
  for ( uptime_t* u = p -> uptime_list; u; u = u -> next )
  {
    if ( u -> uptime > 0 )
    {
      fprintf( file,
               "\t\t\t\t\t\t\t\t<tr" );
      if ( !( i & 1 ) )
      {
        fprintf( file, " class=\"odd\"" );
      }
      fprintf( file, ">\n" );
      fprintf( file,
               "\t\t\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n"
               "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.1f%%</td>\n"
               "\t\t\t\t\t\t\t\t</tr>\n",
               u -> name(),
               u -> uptime * 100.0 );
      i++;
    }
  }

  fprintf( file,
           "\t\t\t\t\t\t\t</table>\n"
           "\t\t\t\t\t\t</div>\n"
           "\t\t\t\t\t</div>\n" );
}

// print_html_player_procs ========================================================

static void print_html_player_procs( FILE* file, player_t* p )
{
  // Procs Section
  fprintf( file,
           "\t\t\t\t\t<div class=\"player-section procs\">\n"
           "\t\t\t\t\t\t<h3 class=\"toggle\">Procs</h3>\n"
           "\t\t\t\t\t\t<div class=\"toggle-content hide\">\n"
           "\t\t\t\t\t\t\t<table class=\"sc\">\n"
           "\t\t\t\t\t\t\t\t<tr>\n"
           "\t\t\t\t\t\t\t\t\t<th></th>\n"
           "\t\t\t\t\t\t\t\t\t<th>Count</th>\n"
           "\t\t\t\t\t\t\t\t\t<th>Interval</th>\n"
           "\t\t\t\t\t\t\t\t</tr>\n" );
  int i = 1;
  for ( proc_t* proc = p -> proc_list; proc; proc = proc -> next )
  {
    if ( proc -> count > 0 )
    {
      fprintf( file,
               "\t\t\t\t\t\t\t\t<tr" );
      if ( !( i & 1 ) )
      {
        fprintf( file, " class=\"odd\"" );
      }
      fprintf( file, ">\n" );
      fprintf( file,
               "\t\t\t\t\t\t\t\t\t<td class=\"left\">%s</td>\n"
               "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.1f</td>\n"
               "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.1fsec</td>\n"
               "\t\t\t\t\t\t\t\t</tr>\n",
               proc -> name(),
               proc -> count,
               proc -> frequency );
      i++;
    }
  }
  fprintf( file,
           "\t\t\t\t\t\t\t</table>\n"
           "\t\t\t\t\t\t</div>\n"
           "\t\t\t\t\t</div>\n" );



}

// print_html_player_deaths ========================================================

static void print_html_player_deaths( FILE* file, player_t* p )
{
  // Death Analysis

  if ( p -> deaths.size() > 0 )
  {
    std::string distribution_deaths_str                = "";
    if ( ! p -> distribution_deaths_chart.empty() )
    {
      distribution_deaths_str = "<img src=\"" + p -> distribution_deaths_chart + "\" alt=\"Deaths Distribution Chart\" />\n";
    }

    fprintf( file,
             "\t\t\t\t\t<div class=\"player-section gains\">\n"
             "\t\t\t\t\t\t<h3 class=\"toggle\">Deaths</h3>\n"
             "\t\t\t\t\t\t<div class=\"toggle-content hide\">\n"
             "\t\t\t\t\t\t\t<table class=\"sc\">\n"
             "\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t<th></th>\n"
             "\t\t\t\t\t\t\t\t\t<th></th>\n"
             "\t\t\t\t\t\t\t\t</tr>\n" );

    fprintf( file,
             "\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"left\">death count</td>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"right\">%i</td>\n"
             "\t\t\t\t\t\t\t\t</tr>\n",
             ( int ) p -> deaths.size() );

    fprintf( file,
             "\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"left\">death count pct</td>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f%%</td>\n"
             "\t\t\t\t\t\t\t\t</tr>\n",
             ( double ) p -> deaths.size() / p -> sim -> iterations * 100.0);
    fprintf( file,
             "\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"left\">avg death time</td>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f</td>\n"
             "\t\t\t\t\t\t\t\t</tr>\n",
             p -> deaths.mean );
    fprintf( file,
             "\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"left\">min death time</td>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f</td>\n"
             "\t\t\t\t\t\t\t\t</tr>\n",
             p -> deaths.min );
    fprintf( file,
             "\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"left\">max death time</td>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f</td>\n"
             "\t\t\t\t\t\t\t\t</tr>\n",
             p -> deaths.max );
    fprintf( file,
             "\t\t\t\t\t\t\t\t<tr>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"left\">dmg taken</td>\n"
             "\t\t\t\t\t\t\t\t\t<td class=\"right\">%.2f</td>\n"
             "\t\t\t\t\t\t\t\t</tr>\n",
             p -> dmg_taken.mean );

    fprintf( file,
             "\t\t\t\t\t\t\t\t</table>\n" );

    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t<div class=\"clear\"></div>\n" );

    fprintf ( file,
              "\t\t\t\t\t\t\t\t\t%s\n",
              distribution_deaths_str.c_str() );

    fprintf ( file,
              "\t\t\t\t\t\t\t</div>\n"
              "\t\t\t\t\t\t</div>\n" );
  }
}

// print_html_player_gear_weights ========================================================

static void print_html_player_gear_weights( FILE* file, player_t* p )
{
  if ( p -> sim -> scaling -> has_scale_factors() && !p -> is_pet() )
  {
    fprintf( file,
             "\t\t\t\t\t\t<div class=\"player-section gear-weights\">\n"
             "\t\t\t\t\t\t\t<h3 class=\"toggle\">Gear Weights</h3>\n"
             "\t\t\t\t\t\t\t<div class=\"toggle-content hide\">\n"
             "\t\t\t\t\t\t\t\t<table class=\"sc mb\">\n"
             "\t\t\t\t\t\t\t\t\t<tr class=\"left\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th>Pawn Standard</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td>%s</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n"
             "\t\t\t\t\t\t\t\t\t<tr class=\"left\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th>Zero Accuracy/Expertise</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td>%s</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n"
             "\t\t\t\t\t\t\t\t</table>\n",
             p -> gear_weights_pawn_std_string.c_str(),
             p -> gear_weights_pawn_alt_string.c_str() );

    std::string rhada_std = p -> gear_weights_pawn_std_string;
    std::string rhada_alt = p -> gear_weights_pawn_alt_string;

    if ( rhada_std.size() > 10 ) rhada_std.replace( 2, 8, "RhadaTip" );
    if ( rhada_alt.size() > 10 ) rhada_alt.replace( 2, 8, "RhadaTip" );

    fprintf( file,
             "\t\t\t\t\t\t\t\t<table class=\"sc\">\n"
             "\t\t\t\t\t\t\t\t\t<tr class=\"left\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th>RhadaTip Standard</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td>%s</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n"
             "\t\t\t\t\t\t\t\t\t<tr class=\"left\">\n"
             "\t\t\t\t\t\t\t\t\t\t<th>Zero Accuracy/Expertise</th>\n"
             "\t\t\t\t\t\t\t\t\t\t<td>%s</td>\n"
             "\t\t\t\t\t\t\t\t\t</tr>\n"
             "\t\t\t\t\t\t\t\t</table>\n",
             rhada_std.c_str(),
             rhada_alt.c_str() );
    fprintf( file,
             "\t\t\t\t\t\t\t</div>\n"
             "\t\t\t\t\t\t</div>\n" );
  }
}

// print_html_player_ ========================================================

static void print_html_player_( FILE* file, sim_t* sim, player_t* p, int j=0 )
{
  std::string n = p -> name();
  util_t::format_text( n, true );


  print_html_player_description( file, sim, p, j, n );

  print_html_player_results_spec_gear( file, sim, p );

  print_html_player_scale_factors( file, sim, p );

  print_html_player_charts( file, sim, p );

  print_html_player_abilities( file, sim, p, n );

  print_html_player_resources( file, p );

  print_html_player_buffs( file, p );

  print_html_player_benefits_uptimes( file, p );

  print_html_player_procs( file, p );

  print_html_player_deaths( file, p );

  print_html_player_statistics( file, p );

  print_html_player_action_priority_list( file, sim, p );

  print_html_stats( file, p );

  print_html_gear( file, p );

  print_html_talents( file, p );

  print_html_profile( file, p );

  print_html_player_gear_weights( file, p );


  fprintf( file,
           "\t\t\t\t\t</div>\n"
           "\t\t\t\t</div>\n\n" );
}

} // ANONYMOUS NAMESPACE ====================================================

// report_t::print_html_player ====================================================

void report_t::print_html_player( FILE* file, player_t* p, int j=0 )
{
  print_html_player_( file, p -> sim, p, j );
}
