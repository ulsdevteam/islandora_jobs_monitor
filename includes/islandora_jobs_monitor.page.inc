<?php

/**
 * @file
 * Main page handler function for Islandora Jobs Stack Monitor module.
 */

/**
 * Controller for the Islandora Jobs Stack Monitor main page.
 *
 * @return string
 *   Page markup html.
 */
function islandora_jobs_monitor_page() {
  module_load_include('inc', 'islandora_jobs_monitor', 'includes/db');
  module_load_include('inc', 'islandora_jobs_monitor', 'includes/utilities');
  $hosts = islandora_jobs_monitor_get_all_hosts();

  // Loop through the hosts data and call appropriate theme code to add
  // to themed_hosts array.
  $themed_hosts = array();
  $err_log_link_options = array(
    'attributes' => array(
      'target'=>'_blank',
      'title' => ''
    ),
  );

  foreach ($hosts as $host) {
    $metrics = islandora_jobs_monitor_get_health_metrics($host['server_id']);
    // Convert the errors_in_last_1000 value into a link that will serve up
    // the related last 1000 lines from that server's error_log file.
    $err_log_link_options['attributes']['title'] = 'View "' . $host['name'] . '" error log';
    $host['errors_in_last_1000'] = l($host['errors_in_last_1000'], '/islandora/islandora_job_monitor/error_log/' . $host['server_id'], $err_log_link_options);

    $worker = $host['is_worker'];
    // Unset these either way, they do not need to be used in the themed output.
    $server_id = $host['server_id'];
    unset($host['server_id']);
    unset($host['is_worker']);
    $keys = array_keys($metrics['cpu_percentages']);
    $first_key = array_pop($keys);
    // Add the percent sign to the end of the disk_used_pct value.
    if (isset($host['disk_used_pct'])) {
      $host['disk_used_pct'] = $host['disk_used_pct'] . '% (' . islandora_jobs_monitor_bytes_to_human_readable($host['disk_bytes_avail']) . ' free)';
      unset($host['disk_bytes_avail']);
    }
    if ($worker) {
      $themed_hosts['workers'][] = theme('islandora_jobs_monitor_themed_worker', array(
        'host_record' => $host,
        'control_links' => theme('islandora_jobs_monitor_themed_control_links', array(
          'server_control_links' => islandora_jobs_monitor_get_worker_control_links($server_id),
        )),
        'cpu_values' => theme('islandora_jobs_monitor_themed_0_to_100_metrics', array(
          'label' => 'CPU (' . ($first_key ? $metrics['cpu_percentages'][$first_key] . '%' : '?' ) . ')',
          'values' => $metrics['cpu_percentages'],
        )),
        'memory_values' => theme('islandora_jobs_monitor_themed_0_to_100_metrics', array(
          'label' => 'Memory (' . ($first_key ? $metrics['memory_percentages'][$first_key] . '%' : '?' ) . ')',
          'values' => $metrics['memory_percentages'],
        )),
      ));
    }
    else {
      $themed_hosts['master'][] = theme('islandora_jobs_monitor_themed_master', array(
        'host_record' => $host,
        'control_links' => ($server_id < 100) ? theme('islandora_jobs_monitor_themed_control_links', array(
          'server_control_links' => islandora_jobs_monitor_get_master_control_links($server_id),
        )) : '',
        'cpu_values' => theme('islandora_jobs_monitor_themed_0_to_100_metrics', array(
          'label' => 'CPU (' . ($first_key ? $metrics['cpu_percentages'][$first_key] . '%' : '?' ) . ')',
          'values' => $metrics['cpu_percentages'],
        )),
        'memory_values' => theme('islandora_jobs_monitor_themed_0_to_100_metrics', array(
          'label' => 'Memory (' . ($first_key ? $metrics['memory_percentages'][$first_key] . '%' : '?' ) . ')',
          'values' => $metrics['memory_percentages'],
        ))));
    }
  }

  $current_report = islandora_jobs_monitor_get_jobs_report();

  return theme('islandora_jobs_monitor_page', array(
      'themed_hosts' => $themed_hosts,
      'current_report' => $current_report,
  ));
}

/**
 * Create the report based on the available hosts.
 *
 * @return array
 *   'jobs_table': Drupal themed table of gearman jobs in HTML markup.
 */
function islandora_jobs_monitor_get_jobs_report() {
  $headers = array('Worker', 'PID', 'Worker Function Name', 'Job Started');
  $rows = islandora_jobs_monitor_get_jobs_rows();
  $jobs_table = '<h3>Jobs Records</h3>' .
          theme('table', array('header' => $headers, 'rows' => $rows));

  return $jobs_table;
}

/**
 * Gets the worker links relative to the server_id provided.
 *
 * @param type $server_id
 * @return array
 *   Links that will update the master_command value for the server's record.
 */
function islandora_jobs_monitor_get_worker_control_links($server_id) {
  $links = array();
  $current_master_command = islandora_jobs_monitor_get_master_command($server_id);

  if ($current_master_command) {
    module_load_include('inc', 'islandora_jobs_monitor', 'includes/utilities');
    $command_text = islandora_jobs_monitor_command_text_of_master_command($current_master_command);
    $links[] = '<em>Currently processing "' . $command_text . '" command.</em>';
  }
  else {
    $links[] = l('Stop Gearman Workers', '/islandora/islandora_job_monitor/command_handler/' . $server_id . '/stop_gearman_workers');
    $links[] = l('Start Gearman Workers', '/islandora/islandora_job_monitor/command_handler/' . $server_id . '/start_gearman_workers');
    // $links[] = l('Kill PHP', '/islandora/islandora_job_monitor/command_handler/' . $server_id . '/killall_php');
    $links[] = l('Clear /tmp folder', '/islandora/islandora_job_monitor/command_handler/' . $server_id . '/clear_tmp');
  }

  return $links;
}

function islandora_jobs_monitor_get_master_control_links($server_id) {
  $links = array();
  $current_master_command = islandora_jobs_monitor_get_master_command($server_id);

  if ($current_master_command) {
    module_load_include('inc', 'islandora_jobs_monitor', 'includes/utilities');
    $command_text = islandora_jobs_monitor_command_text_of_master_command($current_master_command);
    $links[] = '<em>Currently processing "' . $command_text . '" command.</em>';
  } 
  else {
    $links[] = l('Stop Gearmand', '/islandora/islandora_job_monitor/command_handler/' . $server_id . '/stop_gearmand');
    $links[] = l('Start Gearmand', '/islandora/islandora_job_monitor/command_handler/' . $server_id . '/start_gearmand');
    $links[] = l('Clear /tmp folder', '/islandora/islandora_job_monitor/command_handler/' . $server_id . '/clear_tmp');
  }

  return $links;
}
