<?php

/**
 * @file
 * Main page handler function for Islandora Daily Stats module.
 */

/**
 * This will control the variables that will make up any stats report.
 *
 * @return string
 *   Page markup html.
 */
function islandora_jobs_monitor_page() {
  $current_report =  islandora_jobs_monitor_get_report();

  return theme('islandora_jobs_monitor_page', array(
      'current_report' => $current_report,
  ));
}

/**
 * Create the report based on the available hosts.
 *
 * @return array
 *   'stats_table': Drupal themed table for stats report in HTML markup.
 */
function islandora_jobs_monitor_get_report() {
  // Get the list of available hosts.

  $query = db_select('islandora_stats_contentmodel_counts', 'mc');
  // Add a filter for last year...
  $query->where('mc.date >= DATE_SUB(NOW(), INTERVAL 1 YEAR)');
  $query->innerJoin('islandora_stats_models', 'm', 'm.model_id = mc.model_id');
  $query->addField('mc', 'date');
  $query->addField('m', 'model_desc');
  $query->addField('mc', 'pitt_count');
  $query->addField('mc', 'nonpitt_count');
  $query->orderBy('mc.date', 'DESC');
  $query->range(0, 500);
  $results = $query->execute()->fetchAll();
  $rows = array();
  $max_y = 0;
  $max_x = 0;
  $model_colors = array();
  $models = array();
  foreach ($results as $result_obj) {
    if ($result_obj->pitt_count > $max_y) {
      $max_y = $result_obj->pitt_count;
    }
    $rows[] = array(
      'date' => $result_obj->date,
      'model' => $result_obj->model_desc,
      'pitt_count' => $result_obj->pitt_count,
      'nonpitt_count' => $result_obj->nonpitt_count,
    );
    $max_x++;
  }

  $error_log = islandora_jobs_monitor_get_error_log();
  $headers = array('Date', 'Model', 'Pitt Count', 'Non-Pitt Count');
  $stats_table = '<h3>TEST ACCESS</h3><pre class="error_log">' . $error_log . "</textarea>" .
          theme('table', array('header' => $headers, 'rows' => $rows));

  return $stats_table;
}

function islandora_jobs_monitor_get_error_log() {
  $output = array();
  $command = 'cat /tmp/error_log_last100.txt';
  exec($command, $output, $ret);

  $output = htmlspecialchars(implode("\n", $output));

  return $output;
}