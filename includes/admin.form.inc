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
  $form['collections'] = array(
    '#title' => t('Collections'),
    '#type' => 'fieldset',
    '#description' => t('Collections selected will appear as selectable options within the search dropdown. Note that the current collection and the ability to search all collections will always be available regardless of configuration.'),
    '#collapsed' => FALSE,
    '#collapsible' => TRUE,
  );
  $all_collections = islandora_basic_collection_get_collections();
  $header = array(
    'label' => array('data' => t('Label')),
    'pid' => array('data' => t('PID')),
  );
  $options = array();
  foreach ($all_collections as $collection_info) {
    $options[$collection_info['pid']] = array(
      'label' => array(
        'data' => $collection_info['label'],
      ),
      'pid' => array(
        'data' => $collection_info['pid'],
      ),
    );
  }
  $form['collections']['collection_selection'] = array(
    '#type' => 'tableselect',
    '#header' => $header,
    '#options' => $options,
    '#empty' => t('No collections available.'),
    '#default_value' => variable_get('islandora_collection_search_searchable_collections', array()),
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Configure'),
  );
  $form['collections']['all_pages'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display collection selection on all pages?'),
    '#description' => t("When selected this will display the collection selection box on all pages with the previously selected collection options"),
    '#default_value' => variable_get('islandora_collection_search_all_pages', FALSE),
  );
  $form['collections']['collection_label'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display collection label?'),
    '#description' => t("When selected this will display the current collection's label for display as opposed to this collection"),
    '#default_value' => variable_get('islandora_collection_search_display_label', FALSE),
  );
  $form['collections']['advanced_search_alter'] = array(
    '#type' => 'checkbox',
    '#title' => t('Display a searchable collection field in advanced search?'),
    '#default_value' => variable_get('islandora_collection_search_advanced_search_alter', FALSE),
  );
  $form['collections']['retain_values_on_search_results'] = array(
    '#type' => 'checkbox',
    '#title' => t('Retain values searched for on result pages?'),
    '#default_value' => variable_get('islandora_collection_search_retain_search_values', FALSE),
  );
  return $form;
}

/**
 * Form submission handler.
 */
function islandora_collection_search_admin_form_submit(&$form, &$form_state) {
  variable_set('islandora_collection_search_gsearch_endpoint', $form_state['values']['islandora_collection_search_gsearch_endpoint']);
  variable_set('islandora_collection_search_gsearch_user', $form_state['values']['islandora_collection_search_gsearch_user']);
  variable_set('islandora_collection_search_display_label', $form_state['values']['collection_label']);
  if ($form_state['values']['islandora_collection_search_gsearch_password'] || $form_state['values']['blank_password']) {
    variable_set('islandora_collection_search_gsearch_password', $form_state['values']['islandora_collection_search_gsearch_password']);
  }
  variable_set('islandora_collection_search_all_pages', $form_state['values']['all_pages']);
  variable_set('islandora_collection_search_searchable_collections', $form_state['values']['collection_selection']);
  variable_set('islandora_collection_search_advanced_search_alter', $form_state['values']['advanced_search_alter']);
  variable_set('islandora_collection_search_retain_search_values', $form_state['values']['retain_values_on_search_results']);
}
