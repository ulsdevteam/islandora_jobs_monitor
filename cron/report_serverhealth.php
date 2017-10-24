#!/usr/bin/php
<?php

// Step 1. Remove any `host_gearman_job` or `host_server_health` records for
//         this server that are more than 1 hr old.
// Step 2. Get CPU % and Memory % and update `host_server_health`.
// Step 3. Update the record in `host_error_log` (if the file is newer than the record).
// Step 4. Post the results to the server that is reporting (specified by the
//         config value for server_monitor.

// Load our own Library.
require_once('/opt/islandora_cron/uls-tuque-lib.php');

/* IF ANY TUQUE COMMANDS ARE NEEDED, UNCOMENT THIS BLOCK --
 *
// Setup Tuque
$path_to_tuque = get_config_value('tuque','path_to_tuque');

if (file_exists($path_to_tuque)) {
        require_once($path_to_tuque . 'Cache.php');
        require_once($path_to_tuque . 'FedoraApi.php');
        require_once($path_to_tuque . 'FedoraApiSerializer.php');
        require_once($path_to_tuque . 'Object.php');
        require_once($path_to_tuque . 'Repository.php');
        require_once($path_to_tuque . 'RepositoryConnection.php');
} else {
        print "Error - \"" . $path_to_tuque . "\" Invalid path to Tuque.\n";
        exit(1);
}

$connection = getRepositoryConnection();
$repository = getRepository($connection);

 -- IF ANY TUQUE COMMANDS ARE NEEDED, UNCOMENT THIS BLOCK */


/**
 * Connect to mysql
 */

$server_id = get_config_value('islandora_jobs_monitor', 'server_id');
$server_monitor = get_config_value('islandora_jobs_monitor', 'server_monitor');

$db_host = get_config_value('mysql','host');
$db_user = get_config_value('mysql','username');
$db_pass = get_config_value('mysql','password');
$db_name = get_config_value('mysql','database');
$link = mysqli_connect($db_host, $db_user, $db_pass, $db_name);
if (!$link) {
    die('Not connected : ' . mysql_error());
}

/**
 * Step 1. Remove any `host_gearman_job` or `host_server_health` records for
 *         this server that are more than 1 hr old.
 */
$exec_free = explode("\n", trim(shell_exec('free')));
$get_mem = preg_split("/[\s]+/", $exec_free[1]);
$mem = trim(round($get_mem[2]/$get_mem[1], 5));

/**
 * Step 2. Get CPU % and Memory % and update `host_server_health`.
 */
$command = 'mpstat | grep all | awk \'{print $12}\'';
$cpu = trim(shell_exec($command));

/**
 * Step 3. Update the record in `host_error_log` (if the file is newer than the record)
 *         and save this error_log section to the database.
 */
$host_error_log = file_get_contents('/tmp/error_log_last100.txt');
// Save this value directly to the `host_error_log` table
$host_error_log_errors_count = substr_count(strtolower($host_error_log), 'error');
$sql = "REPLACE INTO host_error_log (`server_id`, `error_log_last_100_lines`) VALUES (" . 
       $server_id . ", '" . mysqli_real_escape_string($link, addslashes($host_error_log)) . "')";
$result = mysqli_query($link, $sql);
@mysqli_close($link);


/**
 * Step 4. Post the results to the server that is reporting (specified by the
 *         config value for server_monitor.
 */

// file_get_contents to the api endpoint causes 20 redirects and an error.

// I think that the "ip_login" module does not pass any posted variables to the 
// redirected-to page and loses them when the /user login page is processed.  The
// easy way around this is to post the values as $_GET 
$command = "wget '" . $server_monitor . "islandora/islandora_job_monitor/api/serverhealth/?cpu=" . $cpu . "&memory=" . $mem . "&errors=" . $host_error_log_errors_count . "' >/dev/null 2>&1";
$output = array();
exec($command, $output, $return);


exit(0);

?>
