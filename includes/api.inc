<?php 

/**
 * @file
 * API functions for Islandora Jobs Monitor module.
 */

/**
 * This will handle /islandora/islandora_job_monitor/api/serverhealth posts.
 */
function islandora_jobs_monitor_api_serverhealth() {
  $args = func_get_arg();
  file_put_contents('/tmp/log1', print_r($args, true));
}
