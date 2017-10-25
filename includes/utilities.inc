<?php 

/**
 * @file
 * Utilities functions for Islandora Jobs Monitor module.
 */

/**
 * This will return a human readable version of "how long ago" for a given timetamp.
 */
function islandora_jobs_monitor_timeago_from_timestamp($timestamp) {
  $dbDate = new DateTime($timestamp);
  $currDate = new DateTime(date('Ymd H:i:s'));
  $interval = $currDate->diff($dbDate);
  $ago_arr = array();
  if ($interval->y > 0) {
    $ago_arr[] = $interval->y . " yr" . ($interval->y == 1 ? '' : 's');
  }
  if ($interval->m > 0) {
    $ago_arr[] = $interval->m . " mo";
  }
  if ($interval->d > 0) {
    $ago_arr[] = $interval->d . " d";
  }
  if ($interval->h > 0) {
    $ago_arr[] = $interval->h . " h";
  }
  if ($interval->i > 0) {
    $ago_arr[] = $interval->i . " m";
  }
  if ($interval->s > 0) {
    $ago_arr[] = $interval->s . " s";
  }
  return implode(", ", $ago_arr) . ((count($ago_arr) > 0) ? ' ago' : '');
}

function islandora_jobs_monitor_command_text_of_master_command($current_master_command) {
  switch ($current_master_command) {
    case "stop_gearman_workers":
      $command_text = "Stop Gearman Workers";
      break;
    case "start_gearman_workers":
      $command_text = "Start Gearman Workers";
      break;
//    case "killall_php":
//      $command_text = "Kill PHP";
//      break;
    case "clear_tmp":
      $command_text = "Clear /tmp folder";
      break;
    case "stop_gearmand":
      $command_text = "Stop Gearmand";
      break;
    case "start_gearmand":
      $command_text = "Start Gearmand";
      break;
    default:
      $command_text = '';
      break;
  }
  return $command_text;
}