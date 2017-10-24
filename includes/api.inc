<?php 

/**
 * @file
 * API functions for Islandora Jobs Monitor module.
 */

/**
 * This will handle /islandora/islandora_job_monitor/api/serverhealth posts.
 */
function islandora_jobs_monitor_api_serverhealth() {
  global $user;

  // The server_ip is the same as the user record's hostname.
  $ip_address = $user->hostname;
  $args = func_get_args();
  if (count($args) > 0) {
    $get_args = urldecode($args[0]);
    list($junk, $keep) = explode("?", $get_args, 2);
    // get_args should be something like: ?cpu=12.1&memory=5.1
    $kv_pairs = explode("&", $keep);
    if (count($kv_pairs) > 0) {
      module_load_include('inc', 'islandora_jobs_monitor', 'includes/db');
      $save_values = array();
      foreach ($kv_pairs as $kv_pair) {
        @list($key, $value) = explode("=", $kv_pair, 2);
        $save_values[$key] = $value;
      }
      islandora_jobs_monitor_save_server_healthrecord($ip_address, $save_values);
    }
  }
}
