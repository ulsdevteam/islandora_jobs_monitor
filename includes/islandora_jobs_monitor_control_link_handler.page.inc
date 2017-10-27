<?php

/**
 * @file
 * Control link handler function for Islandora Jobs Stack Monitor module.
 */

/**
 * To display that the master_command value has been updated appropriately
 * according to the action performed.
 *
 * @return string
 *   Page markup html.
 */
function islandora_jobs_monitor_control_link_handler($server_id, $action) {
  module_load_include('inc', 'islandora_jobs_monitor', 'includes/db');

  islandora_jobs_monitor_put_master_command($server_id, $action);
  return '<h3>For server_id ' . $server_id . ', action = "' . $action . '"</h3>This will update the server record\'s ' .
         'master_command value -- which will be parsed during the CRON process ' .
         'in order to perform the specified action.';
}
