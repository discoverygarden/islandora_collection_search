<?php

/**
 * @file
 * Admin form and submission handler.
 */

/**
 * Form building function.
 */
function islandora_collection_search_admin_form($form, &$form_state) {
  form_load_include($form_state, 'inc', 'islandora_collection_search', 'includes/admin.form');

  $form['wrapper'] = array(
    '#title' => t('GSearch Config'),
    '#type' => 'fieldset',
    '#description' => t('Some details about GSearch are required, in order to reindex child objects.'),
    'islandora_collection_search_gsearch_endpoint'] => array(
      '#title' => t('GSearch Endpoint'),
      '#type' => 'textfield',
      '#required' => TRUE,
      '#default_value' => variable_get("islandora_collection_search_gsearch_endpoint", "http://localhost:8080/fedoragsearch/rest"),
    ),
    'islandora_collection_search_gsearch_user' => array(
      '#title' => t('GSearch User'),
      '#type' => 'textfield',
      '#required' => TRUE,
      '#default_value' => variable_get("islandora_collection_search_gsearch_user", "fedoraAdmin"),
    ),
    'islandora_collection_search_gsearch_password' => array(
      '#title' => t('GSearch Password')
      '#type' => 'password',
      '#default_value' => variable_get("islandora_collection_search_gsearch_password", "fedoraAdmin"),
    ),
  );

  system_settings_form($form);
}
