<?php

/**
 * @file
 * Utilities functions for Islandora Jobs Stack Monitor module.
 */

/**
 * This will use the mysqli_real_escape_string function.
 * 
 * @param type $link
 * @param type $in
 * @return string
 *   Safe string value to use in a mysql query.
 */
function islandora_jobs_monitor_safe_qstring($link, $in) {
  return mysqli_real_escape_string($link, urldecode(trim($in)));
}

/**
 * Get general info for all servers.
 *
 * @return array
 *   rows of data pertaining to all hosts
 */
function islandora_jobs_monitor_get_all_hosts() {
  $hosts = array();
  $link = islandora_jobs_monitor_get_databaselink('islandora_jobs_monitor');

  $sql = "SELECT s.server_id, s.name, s.is_worker, s.hostname, s.ip_address, " .
         " (SELECT h.disk_used_pct FROM host_server_health h WHERE h.server_id = s.server_id ORDER BY timestamp DESC LIMIT 1) `disk_used_pct`, " .
         " (SELECT h.disk_bytes_avail FROM host_server_health h WHERE h.server_id = s.server_id ORDER BY timestamp DESC LIMIT 1) `disk_bytes_avail`, " .
         " (SELECT h.error_log_errors_in_last_100 FROM host_server_health h WHERE h.server_id = s.server_id ORDER BY timestamp DESC LIMIT 1) `errors_in_last_100` " .
         "FROM servers s " .
         "LEFT OUTER JOIN host_error_log e ON (e.server_id = s.server_id)";
  $result = mysqli_query($link, $sql);
  if (!$result) {
    islandora_jobs_monitor_sql_error_die($link, $sql);
  }
  while ($row = mysqli_fetch_assoc($result)) {
    $hosts[] = $row;
  }

  mysqli_close($link);
  return $hosts;
}

/**
 * This will get the health metrics for a given server.
 *
 * @param integer $server_id
 * @return array
 *   The values for the specific host's health from the host_server_health
 * table.
 */
function islandora_jobs_monitor_get_health_metrics($server_id) {
  $rows = array(
    'cpu_percentages' => array(),
    'memory_percentages' => array(),
  );
  $link = islandora_jobs_monitor_get_databaselink('islandora_jobs_monitor');
  
  $sql = "SELECT timestamp, cpu_percentage `cpu_percentages`, memory_percentage `memory_percentages` " .
         "FROM host_server_health WHERE server_id = " . $server_id . " " .
         "ORDER BY timestamp DESC LIMIT 20";
  $result = mysqli_query($link, $sql);
  if (!$result) {
    islandora_jobs_monitor_sql_error_die($link, $sql);
  }
  while ($row = mysqli_fetch_assoc($result)) {
    $rows['cpu_percentages'][$row['timestamp']] = $row['cpu_percentages'];
    $rows['memory_percentages'][$row['timestamp']] = $row['memory_percentages'];
  }

  $rows['cpu_percentages'] = array_reverse($rows['cpu_percentages']);
  $rows['memory_percentages'] = array_reverse($rows['memory_percentages']);
  mysqli_close($link);
  return $rows;
}


/**
 * Gets all the jobs rows from the host_gearman_job table.
 *
 * @return array
 *   This array contains the rows that come from the host_gearman_job table for
 * all servers that have is_worker = 1.
 */
function islandora_jobs_monitor_get_jobs_rows() {
  module_load_include('inc', 'islandora_jobs_monitor', 'includes/utilities');
  $rows = array();

  $link = islandora_jobs_monitor_get_databaselink('islandora_jobs_monitor');

  $sql = "SELECT s.name, j.PID, j.job_name, j.started " .
         "FROM servers s " . 
         "LEFT OUTER JOIN host_gearman_job j ON (j.server_id = s.server_id) " .
         "WHERE s.is_worker = 1 and j.completed IS NULL" ;
  $result = mysqli_query($link, $sql);
  if (!$result) {
    islandora_jobs_monitor_sql_error_die($link, $sql);
  }
  while ($row = mysqli_fetch_assoc($result)) {
    $row['started'] = islandora_jobs_monitor_timeago_from_timestamp($row['started']);
    $rows[] = $row;
  }

  mysqli_close($link);
  return $rows;
}

/**
 * This will return the server_id from the islandora_jobs_monitor.servers table.
 *
 * If the ip_address value is not found, this will return a value of 0.
 *
 * @param string $ip_address
 *
 * @return integer
 *   The server_id value.
 */
function islandora_jobs_monitor_get_server_id_of_ip($ip_address) {
  $link = islandora_jobs_monitor_get_databaselink('islandora_jobs_monitor');
  $server_id = 0;
  
  $sql = "SELECT server_id FROM servers WHERE ip_address = '" . islandora_jobs_monitor_safe_qstring($link, $ip_address) . "'";
  $result = mysqli_query($link, $sql);
  if (!$result) {
    islandora_jobs_monitor_sql_error_die($link, $sql);
  }
  if ($row = mysqli_fetch_assoc($result)) {
    $server_id = $row['server_id'];
  }
  mysqli_close($link);
  return $server_id;
}

/**
 * This will add the record to the `host_gearman_job` table for the current job.
 *
 * @param string $server_id
 *   The server_id of the server record
 * @param string $pid
 *   The PID value for the object being processed
 * @param string $jobname
 *   The name of the job that was just started.
 */
function islandora_jobs_monitor_add_job_record($server_id, $pid, $jobname) {
  $link = islandora_jobs_monitor_get_databaselink('islandora_jobs_monitor');

  $sql = "INSERT INTO `host_gearman_job` (`server_id`, `PID`, `job_name`, `started`) VALUES (" .
      $server_id . ", '" . islandora_jobs_monitor_safe_qstring($link, $pid) .
      "', '" . islandora_jobs_monitor_safe_qstring($link, $jobname) . "', now())";

  $result = mysqli_query($link, $sql);
  mysqli_close($link);
}

/**
 * This will add the record to the `host_gearman_job` table for the current job.
 *
 * @param string $server_id
 *   The server_id of the server record
 * @param string $pid
 *   The PID value for the object being processed
 * @param string $jobname
 *   The name of the job that was just started.
 */
function islandora_jobs_monitor_completed_job_record($server_id, $pid, $jobname) {
  $link = islandora_jobs_monitor_get_databaselink('islandora_jobs_monitor');

  $sql = "UPDATE `host_gearman_job` SET `completed` = now() WHERE `server_id` = " . $server_id .
      " AND  `PID` = '" . islandora_jobs_monitor_safe_qstring($link, $pid) . "' AND " .
      "`job_name` = '" . islandora_jobs_monitor_safe_qstring($link, $jobname) . "'";

  $result = mysqli_query($link, $sql);
  mysqli_close($link);
}

function islandora_jobs_monitor_save_server_healthrecord($ip_address, $save_values) {
  $server_id = islandora_jobs_monitor_get_server_id_of_ip($ip_address);
  if ($server_id) {
    islandora_jobs_monitor_delete_old_records($server_id);
    $link = islandora_jobs_monitor_get_databaselink('islandora_jobs_monitor');
    $cpu_percentage = isset($save_values['cpu']) ? $save_values['cpu'] : 0;
    $memory_percentage = isset($save_values['memory']) ? $save_values['memory'] : 0;
    $error_log_errors_in_last_100 = isset($save_values['errors']) ? $save_values['errors'] : 0;
    $disk_used_pct = isset($save_values['disk_used_pct']) ? $save_values['disk_used_pct'] : 0;
    $disk_bytes_avail = isset($save_values['disk_bytes_avail']) ? $save_values['disk_bytes_avail'] : 0;


    $sql = "INSERT INTO `host_server_health` (`server_id`, `timestamp`, `cpu_percentage`, `memory_percentage`, `error_log_errors_in_last_100`, `disk_used_pct`, `disk_bytes_avail`) VALUES (" .
        $server_id . ", now(), " . $cpu_percentage . ", " . $memory_percentage . ", " . $error_log_errors_in_last_100 . ", '" . $disk_used_pct . "', " . $disk_bytes_avail . ")";
    $result = mysqli_query($link, $sql);
    mysqli_close($link);
  }
}

/**
 * This will remove any old `host_gearman_job` or `host_server_health`.
 *
 * @param string $server_id
 */
function islandora_jobs_monitor_delete_old_records($server_id) {
  $link = islandora_jobs_monitor_get_databaselink('islandora_jobs_monitor');
  $sql = "DELETE FROM `host_server_health` WHERE server_id = " . $server_id . " AND timestamp < DATE_SUB(NOW(), INTERVAL 105 MINUTE)";
  mysqli_query($link, $sql);
  mysqli_close($link);
}

/**
 * Gets the error_log_last_100_lines value related to the provided server_id.
 *
 * @param type $server_id
 * @return string
 */
function islandora_jobs_monitor_get_error_log($server_id) {
  $link = islandora_jobs_monitor_get_databaselink('islandora_jobs_monitor');
  $sql = "SELECT e.error_log_last_100_lines `error_log`, s.name `host_name` " .
         "FROM servers s LEFT OUTER JOIN host_error_log e ON (e.server_id = s.server_id) " .
         "WHERE s.server_id = " . islandora_jobs_monitor_safe_qstring($link, $server_id);
  $result = mysqli_query($link, $sql);
  if (!$result) {
    islandora_jobs_monitor_sql_error_die($link, $sql);
  }
  if ($row = mysqli_fetch_assoc($result)) {
    $error_log_record = $row;
  }
  else {
    $error_log_record = array(
      'error_log' => '',
      'host_name' => 'unknown',
    );
  }

  mysqli_close($link);
  return $error_log_record;
}

/**
 * Gets the current master_command record value.
 *
 * @param type $server_id
 * @return type
 */
function islandora_jobs_monitor_get_master_command($server_id) {
  $link = islandora_jobs_monitor_get_databaselink('islandora_jobs_monitor');
  $sql = "SELECT master_command FROM servers " .
         "WHERE server_id = " . islandora_jobs_monitor_safe_qstring($link, $server_id);
  $result = mysqli_query($link, $sql);
  if (!$result) {
    islandora_jobs_monitor_sql_error_die($link, $sql);
  }

  mysqli_close($link);
  return ($row = mysqli_fetch_assoc($result)) ?
    $row['master_command'] :
    '';
}

/**
 * Puts the current master_command record value for the provided server.
 * 
 * @param integer $server_id
 * @param string $command
 */
function islandora_jobs_monitor_put_master_command($server_id, $command) {
  $link = islandora_jobs_monitor_get_databaselink('islandora_jobs_monitor');
  $sql = "UPDATE servers SET master_command = '" . islandora_jobs_monitor_safe_qstring($link, $command) . "' " .
         "WHERE server_id = " . islandora_jobs_monitor_safe_qstring($link, $server_id);
  $result = mysqli_query($link, $sql);
  if (!$result) {
    islandora_jobs_monitor_sql_error_die($link, $sql);
  }

  mysqli_close($link);
}
