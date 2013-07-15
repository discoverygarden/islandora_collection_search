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

  $form['gsearch'] = array(
    '#title' => t('GSearch Config'),
    '#type' => 'fieldset',
    '#description' => t('Some details about GSearch are required so we can reindex child objects when necessary (e.g. moving a collection from one collection to another).'),
    'islandora_collection_search_gsearch_endpoint' => array(
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
      '#title' => t('GSearch Password'),
      '#type' => 'password',
      '#default_value' => variable_get("islandora_collection_search_gsearch_password", "fedoraAdmin"),
    ),
    'blank_password' => array(
      '#type' => 'checkbox',
      '#title' => t('Make password blank? Current password will be preserved if unchecked.'),
      '#states' => array(
        'visible' => array('input[name=islandora_collection_search_gsearch_password]' => array('value' => '')),
      ),
      '#default_value' => FALSE,
    ),
  );

  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Submit'),
  );
  return $form;
}

/**
 * Form submission handler.
 */
function islandora_collection_search_admin_form_submit(&$form, &$form_state) {
  variable_set('islandora_collection_search_gsearch_endpoint', $form_state['values']['islandora_collection_search_gsearch_endpoint']);
  variable_set('islandora_collection_search_gsearch_user', $form_state['values']['islandora_collection_search_gsearch_user']);

  if ($form_state['values']['islandora_collection_search_gsearch_password'] || $form_state['values']['blank_password']) {
    variable_set('islandora_collection_search_gsearch_password', $form_state['values']['islandora_collection_search_gsearch_password']);
  }
}
