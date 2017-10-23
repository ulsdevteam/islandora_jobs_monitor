<?php 

/**
 * @file
 * API functions for Islandora Jobs Monitor module.
 */

/**
 * This will handle /islandora/islandora_job_monitor/api/serverhealth posts.
 */
function islandora_jobs_monitor_api_serverhealth() {
  $args = func_get_args();
  file_put_contents('/tmp/log1', 'args
' . print_r($args, true));


  global $user;

  file_put_contents('/tmp/log1', 'user 
' . print_r($user, true), FILE_APPEND);

  
  file_put_contents('/tmp/log1', 'request
' . print_r($_REQUEST, true), FILE_APPEND);

  echo "accessed";
}
