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
  module_load_include('inc', 'islandora_jobs_monitor', 'includes/db');
  $hosts = islandora_jobs_monitor_get_all_hosts();

  // Loop through the hosts data and call appropriate theme code to add
  // to themed_hosts array.
  $themed_hosts = array();
  foreach ($hosts as $host) {
    $worker = $host['is_worker'];
    // Unset this either way, it does not need to be used in the themed output.
    unset($host['is_worker']);
    if ($worker) {
      $themed_hosts['workers'][] = theme('islandora_jobs_monitor_themed_worker', array('host_record' => $host));
    }
    else {
      $themed_hosts['master'][] = theme('islandora_jobs_monitor_themed_master', array('host_record' => $host));
    }
  }

  $current_report = islandora_jobs_monitor_get_jobs_report();

  return theme('islandora_jobs_monitor_page', array(
      'themed_hosts' => $themed_hosts,
      'current_report' => $current_report,
  ));
}

/**
 * Create the report based on the available hosts.
 *
 * @return array
 *   'stats_table': Drupal themed table for stats report in HTML markup.
 */
function islandora_jobs_monitor_get_jobs_report() {
  $headers = array('Worker', 'PID', 'Worker Function Name', 'Job Started');
  $rows = islandora_jobs_monitor_get_jobs_rows();
  $stats_table = '<h3>Jobs Records</h3>' .
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