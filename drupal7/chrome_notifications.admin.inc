<?php

/**
 * Page callback for the admin form.
 */
function chrome_notifications_admin_form() {
  $form = array();
  $form['chrome_notifications_project_id'] = array(
    '#title' => t('Chrome notifications project ID'),
    '#type' => 'textfield',
    '#default_value' => variable_get('chrome_notifications_project_id', ''),
    '#description' => t('The ID for your Google developer console project. Should be numeric.'),
  );
  $form['chrome_notifications_api_key'] = array(
    '#title' => t('Chrome notifications API key'),
    '#type' => 'textfield',
    '#default_value' => variable_get('chrome_notifications_api_key', ''),
    '#description' => t('Your server API credentials for your Google developer console project.'),
  );
  return system_settings_form($form);
}
