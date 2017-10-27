#!/usr/bin/php
<?php

// Step 1. Get CPU %, Memory %, and Disk space, and then update `host_server_health`.
// Step 2. Post the results to the server that is reporting (specified by the
//         config value for server_monitor.

$server_id = get_config_value('islandora_jobs_monitor', 'server_id');
$server_monitor = get_config_value('islandora_jobs_monitor', 'server_monitor');


/**
 * Step 1. Get CPU %, Memory %, and Disk space, and then update `host_server_health`.
 */
$command = 'vmstat -s | awk  \' $0 ~ /total memory/ {total=$1 } $0 ~/free memory/ {free=$1} $0 ~/buffer memory/ {buffer=$1} $0 ~/cache/ {cache=$1} END{print (total-free-buffer-cache)/total*100}\'';
$mem = trim(shell_exec($command));

$command = '/opt/islandora_cron/cpu_check.sh';
$cpu = trim(shell_exec($command))/100;

$command = 'df -P | grep "/filestore" | gawk \'{ print $5 }\'';
$disk_used_pct = str_replace("%", "", trim(shell_exec($command)));


/**
 * Step 2. Post the results to the server that is reporting (specified by the
 *         config value for server_monitor.
 */
// I think that the "ip_login" module does not pass any posted variables to the
// redirected-to page and loses them when the /user login page is processed.  The
// easy way around this is to post the values as $_GET
$command = "wget '" . $server_monitor . "islandora/islandora_job_monitor/api/serverhealth/?cpu=" . $cpu . "&memory=" . $mem . "&disk_used_pct=" . $disk_used_pct . "' >/dev/null 2>&1";
$output = array();
exec($command, $output, $return);

exit(0);

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
