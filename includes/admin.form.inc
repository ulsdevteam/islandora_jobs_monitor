<?php

/**
 * @file
 * Administration forms and submit handling for islandora_jobs_monitor module.
 */

/**
 * Administration and configuration form for islandora_jobs_monitor.
 *
 * @ingroup forms
 *
 * @todo break out non-settings elements into their own form.
 *
 * @return array
 *   The Drupal form definition.
 */
function islandora_jobs_monitor_admin_form() {
  global $user;
  $islandora_job_monitor_link = user_access(ISLANDORA_USE_STACK_MONITOR, $user) ?
      l('Islandora Job Monitor', '/islandora/islandora_job_monitor') : 'This ' .
        'account does not have permission to use the Islandora Jobs Monitor.';

  $password_field_description = (variable_get('islandora_jobs_monitor_ftp_password') ?
      '<span style="color:red">' . t('There is a value for the password, but it is not displayed.  
         Only enter text here if the password value needs to change.') . '</span>':
      t('There is NOT any password value.  FTP will not work without a valid password.'));

  $form = array(
//    'settings_file' => array(
//      '#type' => 'textfield',
//      '#title' => t('settings.php File'),
//      '#description' => t('Full path to the site\'s settings.php file.  Usually this would ' .
//        'be located under the /var/www/drupal7/sites/default/settings.php, but for a site ' .
//        'that has a specific "sites" folder, that must be provided here.  This is ' .
//        'needed in order to read the database settings for the ' .
//        '`islandora_jobs_monitor` database.'),
//      '#default_value' => variable_get('islandora_jobs_monitor_settings_file', ''),
//    ),
    'submit' => array(
      '#type' => 'submit',
      '#value' => t('Save configuration'),
    ),
  );
  if ($islandora_job_monitor_link) {
    $form['islandora_job_monitor_link'] = array(
      '#type' => 'fieldset',
      '#title' => 'Islandora Job Monitor',
      '#description' => $islandora_job_monitor_link,
    );
  }

  return $form;
}

/**
 * Function that sets the Drupal variables with user's input.
 *
 * @param array $form
 *   The Drupal form definition.
 * @param array $form_state
 *   The Drupal form state.
 */
function islandora_jobs_monitor_admin_form_submit(array $form, array &$form_state) {
  $id = $form_state['triggering_element']['#id'];
  switch ($id) {
    case 'edit-submit':
      // variable_set('islandora_jobs_monitor_settings_file', $form_state['values']['settings_file']);
      break;

    case 'edit-reset':
      // variable_del('islandora_jobs_monitor_settings_file');
      break;
  }
  // Set this message after the folders may be created in case there are any error messages.
  drupal_set_message(t('The settings have been updated!'));
}
