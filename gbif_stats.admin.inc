<?php

function gbif_stats_admin_settingsform() {
  
  $form['gbif_stats_update_stats'] = array(
    '#type' => 'fieldset',
    '#title' => t('Update Statistics'),
  );
  $form['gbif_stats_update_stats']['update_options'] = array(
    '#title' => t('Available statistics sources: '),
    '#description' => t('Please choose statistics to update.'),
    '#type' => 'checkboxes',
    '#options' => drupal_map_assoc(array(
      'membership' => t('Statistics'),
      'geo' => t('Map'),
      'publication' => t('Publication')
    )),
  );
  $form['gbif_stats_update_stats']['update_stats'] = array(
    '#type' => 'submit',
    '#value' => t('Update'),
    '#submit' => array('gbif_stats_stats_update_submit'),
  );
  $form['gbif_stats_update_interval'] = array(
    '#title' => t('Update Interval'),
    '#type' => 'fieldset',
  );
  $interval = drupal_map_assoc(array(1800, 3600, 10800, 21600, 32400, 43200, 86400, 259200), 'format_interval');
  $interval[0] = '<' . t('none') . '>';
  $form['gbif_stats_update_interval']['gbif_stats_update_interval'] = array(
    '#title' => t('Please select update interval:'),
    '#description' => t('How frequently all statistics will be updated together with system cron tasks.'),
    '#type' => 'select',
    '#default_value' => variable_get('gbif_stats_update_interval', 3600),
    '#options' => $interval
  );
  $form['gbif_stats_client_mode'] = array(
    '#title' => t('Client Mode'),
    '#type' => 'fieldset',
  );
  $form['gbif_stats_client_mode']['gbif_stats_client_mode'] = array(
    '#title' => t('Run this module in client mode:'),
    '#description' => t('Client mode allow statistics retreival from GBIF Statistics module running at the GBIF Secretariat, instead of from directly connected databases.'),
    '#type' => 'checkbox',
    '#default_value' => variable_get('gbif_stats_client_mode', FALSE),
  );
  
  return system_settings_form($form);
}


function gbif_stats_stats_update_submit($form, &$form_state) {

  $client_mode = variable_get('gbif_stats_client_mode');

  $to_update = $form_state['values']['update_options'];
  $to_update_text = '';
  foreach ($to_update as $k => $v) {
    if (gettype($v) == 'integer') {
      unset($to_update[$k]);
    }
  }
  $length = count($to_update);
  if ($length > 0) {
    $i = 1;
    foreach ($to_update as $text) {
      $to_update_text .= $text;
      $to_update_text .= ($i < $length) ? ', ' : '';
      $i++;
    }
    $tasks = array(
      'Statistics' => 'gbif_stats_stats_update',
      'Map' => 'geojson_generate',
      'Publication' => 'publication_generate'
    );
    $tasks_to_run = array_intersect_key($tasks, $to_update);
    
    foreach ($tasks_to_run as $task) {
      $task($client_mode);
    }
    
    //drupal_set_message(t('Statistics of @chosen updated.', array('@chosen' => $to_update_text)));  
  } elseif ($length == 0) {
    drupal_set_message(t('No option was chosen.'), 'warning');
  }
}