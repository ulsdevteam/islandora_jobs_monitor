<?php

/**
 * @file
 * Main page handler function for Islandora Jobs Stack Monitor module.
 */

/**
 * Controller for the error_log display page.
 *
 * @return string
 *   Page markup html.
 */
function islandora_jobs_monitor_error_log_page($server_id) {
  module_load_include('inc', 'islandora_jobs_monitor', 'includes/db');

  $error_log_record = islandora_jobs_monitor_get_error_log($server_id);
  return theme('islandora_jobs_monitor_error_log', array(
    'error_log' => $error_log_record['error_log'],
    'host_name' => $error_log_record['host_name'],
  ));

}
