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
         " (SELECT h.error_log_errors_in_last_1000 FROM host_server_health h WHERE h.server_id = s.server_id ORDER BY timestamp DESC LIMIT 1) `errors_in_last_1000`, " .
         " (SELECT h.gearman_running FROM host_server_health h WHERE h.server_id = s.server_id ORDER BY timestamp DESC LIMIT 1) `gearman_running` " .
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
  
  $sql = "SELECT timestamp, cpu_percentage `cpu_percentages`, memory_percentage `memory_percentages`, gearman_running `gearman_running` " .
         "FROM host_server_health WHERE server_id = " . $server_id . " " .
         "ORDER BY timestamp DESC LIMIT 20";
  $result = mysqli_query($link, $sql);
  if (!$result) {
    islandora_jobs_monitor_sql_error_die($link, $sql);
  }
  while ($row = mysqli_fetch_assoc($result)) {
    $rows['cpu_percentages'][$row['timestamp']] = $row['cpu_percentages'];
    $rows['memory_percentages'][$row['timestamp']] = $row['memory_percentages'];
    $rows['gearman_running'][$row['timestamp']] = $row['gearman_running'];
  }

  $rows['cpu_percentages'] = array_reverse($rows['cpu_percentages']);
  $rows['memory_percentages'] = array_reverse($rows['memory_percentages']);
  $rows['gearman_running'] = array_reverse($rows['gearman_running']);
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
         "WHERE s.is_worker = 1 and j.completed IS NULL " .
         "ORDER BY j.started DESC LIMIT 500";
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

function islandora_jobs_monitor_get_gearman_queue_rows() {
  module_load_include('inc', 'islandora_jobs_monitor', 'includes/utilities');
  $rows = array();

  $link = islandora_jobs_monitor_get_databaselink('gearman');

  // Must give the row "data" a different alias because 'data' is a special
  // directive for the table theme code.
  $sql = "SELECT unique_key, function_name, data `PID` " .
         "FROM gearman_queue";
  $result = mysqli_query($link, $sql);
  if (!$result) {
    islandora_jobs_monitor_sql_error_die($link, $sql);
  }
  while ($row = mysqli_fetch_assoc($result)) {
    $row['PID'] = islandora_jobs_monitor_data_to_pid($row['PID']);
    $rows[] = $row;
  }
  mysqli_close($link);
  return $rows;
}

function islandora_jobs_monitor_get_completed_object_rows() {
  module_load_include('inc', 'islandora_jobs_monitor', 'includes/utilities');
  $rows = array();

  $link = islandora_jobs_monitor_get_databaselink('islandora_jobs_monitor');

  $sql = "SELECT PID, started FROM host_gearman_job WHERE PID NOT LIKE '%-%' " .
         "GROUP BY PID ORDER BY started DESC LIMIT 500";
  $result = mysqli_query($link, $sql);
  if (!$result) {
    islandora_jobs_monitor_sql_error_die($link, $sql);
  }
  $link_options = array(
    'attributes' => array('target'=>'_blank'),
  );
  while ($row = mysqli_fetch_assoc($result)) {
    $row['PID'] = l($row['PID'], '/islandora/object/' . $row['PID'], $link_options);
    $rows[] = $row;
  }
  mysqli_close($link);
  return $rows;
}

function islandora_jobs_monitor_get_recent_transaction_rows() {
  module_load_include('inc', 'islandora_jobs_monitor', 'includes/utilities');
  $rows = array();

  $link = islandora_jobs_monitor_get_databaselink('mysql_new_workflow');

  $sql = 'SELECT t.timestamp, ta.description, i.do_id ' .
         'FROM transaction t ' .
         'JOIN transaction_actions ta ON (ta.id = t.transaction_action_id) ' .
         'JOIN item i ON (i.id = t.item_id) ' .
         'WHERE t.transaction_action_id IN (3, 147, 145, 146, 128,126, 127, 117) ' .
         'ORDER BY t.timestamp DESC LIMIT 1000';
  $result = mysqli_query($link, $sql);
  if (!$result) {
    islandora_jobs_monitor_sql_error_die($link, $sql);
  }
  while ($row = mysqli_fetch_assoc($result)) {
    $rows[] = $row;
  }
  mysqli_close($link);
  return $rows;
}

// Strip down the `data` (PID) field so that only the PID remains
// {"func":"islandora_job_jobs_islandora_do_derivatives","args":["pitt:31735070049287-0003",[],2],"site":"http:\/\/gamera.library.pitt.edu","uid":"1"}
function islandora_jobs_monitor_data_to_pid($data) {
  $parts = explode('"args":', $data);
  if (isset($parts[1])) {
    $args = explode(",", $parts[1]);
    return str_replace(array("[", '"'), "", $args[0]);
  }
  else {
    return "? <i>$data</i>";
  }
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
  $server_id = FALSE;
  
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

  $sql = "UPDATE `host_gearman_job` SET `completed` = now() WHERE " . // `server_id` = " . $server_id .
      " AND  `PID` = '" . islandora_jobs_monitor_safe_qstring($link, $pid) . "' AND " .
      "`job_name` = '" . islandora_jobs_monitor_safe_qstring($link, $jobname) . "' AND " .
      "`completed` IS NULL";

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
    $error_log_errors_in_last_1000 = isset($save_values['errors']) ? $save_values['errors'] : 0;
    $disk_used_pct = isset($save_values['disk_used_pct']) ? $save_values['disk_used_pct'] : 0;
    $disk_bytes_avail = isset($save_values['disk_bytes_avail']) ? $save_values['disk_bytes_avail'] : 0;
    $disk_size = isset($save_values['disk_size']) ? $save_values['disk_size'] : 0;
    $gearman_running = isset($save_values['gearman_running']) ? $save_values['gearman_running'] : 0;

    $sql = "INSERT INTO `host_server_health` (`server_id`, `timestamp`, " .
           "`cpu_percentage`, `memory_percentage`, `error_log_errors_in_last_1000`, " .
           "`disk_used_pct`, `disk_bytes_avail`, `disk_size`, `gearman_running`) " .
           "VALUES (" . $server_id . ", now(), " . $cpu_percentage . ", " .
           $memory_percentage . ", " . $error_log_errors_in_last_1000 . ", '" .
           $disk_used_pct . "', " . $disk_bytes_avail . ", " . $disk_size . ", " . $gearman_running . ")";
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
 * Gets the error_log_last_1000_lines value related to the provided server_id.
 *
 * @param type $server_id
 * @return string
 */
function islandora_jobs_monitor_get_error_log($server_id) {
  $link = islandora_jobs_monitor_get_databaselink('islandora_jobs_monitor');
  $sql = "SELECT e.error_log_last_1000_lines `error_log`, s.name `host_name` " .
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

