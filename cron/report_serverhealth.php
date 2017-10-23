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
$link = mysql_connect($db_host, $db_user, $db_pass);
if (!$link) {
    die('Not connected : ' . mysql_error());
}

$db_selected = mysql_select_db($db_name, $link);
if (!$db_selected) {
    die ('Can\'t use ' . $db_name . ' : ' . mysql_error());
}

/**
 * Step 1. Remove any `host_gearman_job` or `host_server_health` records for
 *         this server that are more than 1 hr old.
 */


/**
 * Step 2. Get CPU % and Memory % and update `host_server_health`.
 */
$cpu = 12.1;
$memory = 5.1;

/**
 * Step 3. Update the record in `host_error_log` (if the file is newer than the record).
 */

/**
 * Step 4. Post the results to the server that is reporting (specified by the
 *         config value for server_monitor.
 */
file_get_contents('http://dev.gamera.library.pitt.edu/islandora/islandora_job_monitor/api/serverhealth/?cpu=' . $cpu . '&memory=' . $memory);


exit(0);

?>
