<?php

/**
 * Menu callback for per-user subscriptions.
 */
function chrome_notifications_user_notifications($user) {
  global $base_path;
  $manifest = array(
    '#tag' => 'link',
    '#attributes' => array(
      'rel' => 'manifest',
      'href' => $base_path . 'chrome_notifications/manifest',
    ),
  );
  drupal_add_html_head($manifest, 'chrome_manifest');
  $path = drupal_get_path('module', 'chrome_notifications');
  drupal_add_js($path . '/js/chrome_notifications.js', array('scope' => 'footer'));
  drupal_add_js(array(
    'chromeNotifications' => array(
      'modulePath' => $path,
    )
  ), 'setting');
  return '<button class="form-submit js-test-button" disabled>Send test notification</button><button class="form-submit js-push-button" disabled>enable</button>';
}

/**
 * Menu callback for generating a manifest.json file for the app.
 */
function chrome_notifications_manifest_json() {
  $path = drupal_get_path('module', 'chrome_notifications');
  // Create the json structure Chrome expects.
  $manifest = array(
    'name' => 'Push Drupal Demo',
    'short_name' => 'Push Drupal Demo',
    'icons' => array(
      array(
       'src' => $path . '/images/druplicon.png',
        'sizes' => '192x192',
        'type' => 'image/png',
      ) ,
    ),
    'start_url' => '/node',
    'display' => 'standalone',
    'gcm_sender_id' => variable_get('chrome_notifications_project_id', ''),
    'gcm_user_visible_only' => TRUE,
  );
  drupal_json_output($manifest);
}

/**
 * Menu callback for saving user subscriptions.
 */
function chrome_notifications_save_subscription() {
  global $user;
  if (!$user->uid) {
    // IF we have no user to save it on, screw it.
    return MENU_ACCESS_DENIED;
  }
  // Read POST data. Should be done properly.
  $data = file_get_contents('php://input', FALSE, NULL, -1, 10000);
  if (!$json = json_decode($data)) {
    return 'halla';
  }
  $data = array(
    'subscription_id' => $json->subscriptionId,
    'endpoint' => $json->endpoint,
  );
  // @todo. Should not just attach on user I guess.
  if (!$user->data) {
    $user->data = array();
  }
  $user->data['chrome_notifications'] = $data;
  user_save($user);
  drupal_json_output($user);
}

/**
 * Menu callback for testing subscription
 */
function chrome_notifications_test_subscription() {
  global $user;
  if (!$user->uid) {
    return MENU_ACCESS_DENIED;
  }
  if (empty($user->data)) {
    return MENU_NOT_FOUND;
  }
  if (empty($user->data['chrome_notifications']) || empty($user->data['chrome_notifications']['subscription_id'])) {
    return MENU_NOT_FOUND;
  }
  $sub_id = $user->data['chrome_notifications']['subscription_id'];
  $endpoint = $user->data['chrome_notifications']['endpoint'];
  $api_key = variable_get('chrome_notifications_api_key', '');
  $url = 'https://android.googleapis.com/gcm/send';
  $req = drupal_http_request($endpoint, array(
    'headers' => array(
      'Authorization' => 'key=' . $api_key,
      'Content-Type' => 'application/json',
    ),
    'method' => 'POST',
    'data' => json_encode(array(
      'registration_ids' => array(
        $sub_id,
      ),
    )),
  ));
  drupal_json_output($req);
}
