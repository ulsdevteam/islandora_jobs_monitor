#!/usr/bin/php
<?php

// Step 1. Check for a "master_command" for the current server, and run that
//         process and update the record to remove that task.
// Step 2. Remove any `host_gearman_job` or `host_server_health` records for
//         this server that are more than 1 hr old.
// Step 3. Get CPU % and Memory % and update `host_server_health`.
// Step 4. Update the record in `host_error_log` (if the file is newer than the record).
// Step 5. Post the results to the server that is reporting (specified by the
//         config value for server_monitor.


/**
 * Connect to mysql
 */

$server_id = get_config_value('islandora_jobs_monitor', 'server_id');
$server_monitor = get_config_value('islandora_jobs_monitor', 'server_monitor');

$db_host = get_config_value('mysql','host');
$db_user = get_config_value('mysql','username');
$db_pass = get_config_value('mysql','password');
$db_name = get_config_value('mysql', 'jobs_monitor_database');
$link = mysqli_connect($db_host, $db_user, $db_pass, $db_name);
if (!$link) {
    die('Not connected : ' . mysql_error());
}

/**
 * Step 1. Check for a "master_command" for the current server, and run that
 *         process and update the record to remove that task.
 */
$sql = "SELECT master_command FROM `servers` WHERE server_id = " . $server_id;
$result = mysqli_query($link, $sql);
if ($row = mysqli_fetch_assoc($result)) {
  $master_command = $row['master_command'];
  if ($master_command) {
    _handle_master_command($link, $master_command, $server_id);
  }
}


/**
 * Step 2. Remove any `host_gearman_job` or `host_server_health` records for
 *         this server that are more than 1 hr old.
 */
$sql = "DELETE FROM `host_server_health` WHERE server_id = " . $server_id . " AND timestamp < (UNIX_TIMESTAMP() - 3600)";
mysqli_query($link, $sql);

/**
 * Step 3. Get CPU % and Memory % and update `host_server_health`.
 */
$command = 'vmstat -s | awk  \' $0 ~ /total memory/ {total=$1 } $0 ~/free memory/ {free=$1} $0 ~/buffer memory/ {buffer=$1} $0 ~/cache/ {cache=$1} END{print (total-free-buffer-cache)/total*100}\'';
$mem = trim(shell_exec($command));

$command = '/opt/islandora_cron/cpu_check.sh';
// $command = 'mpstat | grep all | awk \'{print 100-$12}\'';
$cpu = trim(shell_exec($command) / 100);

/**
 * Step 4. Update the record in `host_error_log` (if the file is newer than the record)
 *         and save this error_log section to the database.
 */
$host_error_log = file_get_contents('/tmp/error_log_last100.txt');
// Save this value directly to the `host_error_log` table
$host_error_log_errors_count = substr_count(strtolower($host_error_log), 'error');
$sql = "REPLACE INTO host_error_log (`server_id`, `error_log_last_100_lines`) VALUES (" . 
       $server_id . ", '" . mysqli_real_escape_string($link, addslashes($host_error_log)) . "')";
$result = mysqli_query($link, $sql);


/**
 * Step 5. Post the results to the server that is reporting (specified by the
 *         config value for server_monitor.
 */
// I think that the "ip_login" module does not pass any posted variables to the 
// redirected-to page and loses them when the /user login page is processed.  The
// easy way around this is to post the values as $_GET 
if ($server_id > 0) {
  $command = 'df -P | grep "lv_root" | gawk \'{ print $4,$5 }\'';
  $disk_fields = str_replace("%", "", trim(shell_exec($command)));
  @list($blocks, $disk_used_pct) = explode(" ", $disk_fields);
  $bytes_avail = 1024 * $blocks;

  $command = "wget '" . $server_monitor . "islandora/islandora_job_monitor/api/serverhealth/?cpu=" . $cpu . "&memory=" . $mem . "&errors=" . $host_error_log_errors_count . "&disk_used_pct=" . $disk_used_pct . "&disk_bytes_avail=" . $bytes_avail . "' >/dev/null 2>&1";
  $output = array();
  exec($command, $output, $return);
}
else {
  $command = 'df -P | grep "/ingest/tmp" | gawk \'{ print $5 }\'';
  $ingest_tmp_disk_fields = str_replace("%", "", trim(shell_exec($command)));
  @list($blocks, $ingest_tmp_disk_used_pct) = explode(" ", $ingest_tmp_disk_fields);
  $bytes_avail = 1024 * $blocks;

  // this is gamera -- must save the record with SQL.
  $sql = "INSERT INTO `host_server_health` (`server_id`, `timestamp`, `cpu_percentage`, `memory_percentage`, `error_log_errors_in_last_100`, `disk_used_pct`, `disk_bytes_avail`) VALUES (" .
        $server_id . ", now(), " . $cpu . ", " . $mem . ", " . $host_error_log_errors_count . ", '" . $ingest_tmp_disk_used_pct . "'," . $bytes_avail . ")";
  $result = mysqli_query($link, $sql);
  if (!$result) {
    die('Invalid query: ' . mysqli_error($link) );
  }
}

@mysqli_close($link);

exit(0);

/**
 * This will handle the possible master commands on the current server.  The
 * command will be executed and the servers record will be updated to clear the
 * master_command value for the current server_id.
 *
 * @param type $link
 * @param type $master_command
 * @param type $server_id
 */
function _handle_master_command($link, $master_command, $server_id) {
  switch ($master_command) {
    case "stop_gearman_workers":
      $command_text = "service gearman-workers stop";
      break;
    case "start_gearman_workers":
      $command_text = "service gearman-workers start";
      break;
//    case "killall_php":
//      $command_text = "Kill PHP";
//      break;
    case "clear_tmp":
      $command_text = "rm -rf /tmp/tuque*;rm -rf /tmp/pitt*;rm -rf curlcookie*";
      break;
    case "stop_gearmand":
      $command_text = "service gearmand stop";
      break;
    case "start_gearmand":
      $command_text = "service gearmand start";
      break;
    default:
      $command_text = '';
      break;
  }

  if ($command_text) {
    $null = shell_exec($command_text);
  }

  $sql = "UPDATE servers SET master_command = '' WHERE server_id = " . $server_id;
  $result = mysqli_query($link, $sql);
}

/**
 * Will get the value from the config file.
 * 
 * @param type $section
 * @param type $key
 * @return string
 *   The value from the config for the given section and key.
 */
function get_config_value($section,$key) {
  if (file_exists('/opt/islandora_cron/islandora_jobs_monitor.ini')) {
    $ini_array = parse_ini_file('/opt/islandora_cron/islandora_jobs_monitor.ini', true);
    if (isset($ini_array[$section][$key])) {
      $value = $ini_array[$section][$key];
      return($value);
    } else {
      return("");
    }
  } else {
    return(0);
  }
}
