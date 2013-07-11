<?php

/**
 * @file
 * 
 */

/**
 * Form for searching within a given collection (or site wide).
 *
 * @param array $form
 *   Associative array with Drupal Form API elements.
 *
 * @param array $form_state
 *   Associative array with form state information.
 */
function islandora_collection_search_form($form, &$form_state) {
  form_load_include($form_state, 'inc', 'islandora_collection_search', 'includes/search.form');

  $form['simple'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'container-inline',
      ),
    ),
  );

  $collection = islandora_collection_search_get_collection_pid();
  $form['simple']['collection_pid'] = array(
    '#type' => 'hidden',
    '#value' => $collection,
  );
  $form['simple']['collection_select'] = array(
    '#access' => $collection !== FALSE,
    '#type' => 'select',
    '#options' => array(
      0 => t('All collections'),
      1 => t('This collection'),
    ),
    '#default_value' => ($collection ? 1 : 0),
  );
  $form['simple']["islandora_simple_search_query"] = array(
    '#size' => '15',
    '#type' => 'textfield',
    '#title' => t('Search box'),
    '#title_display' => 'invisible',
    // @TODO: should this be the searched value?
    '#default_value' => '',
  );
  $form['simple']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('search'),
  );
  return $form;
}

/**
 * Submit handler for collection search form.
 *
 * @param array $form
 *   Associative array with Drupal Form API elements.
 * @param array $form_state
 *   Associative array with form state information.
 */
function islandora_collection_search_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  $search_string = $form_state['values']['islandora_simple_search_query'];
  // Replace the slash so url doesn't break.
  module_load_include('inc', 'islandora_solr', 'includes/utilities');
  $search_string = islandora_solr_replace_slashes($search_string);

  $collection_pid = $form_state['values']['collection_pid'];

  $collection_select = isset($form_state['values']['collection_select']) ?
    $form_state['values']['collection_select'] :
    FALSE;

  // Using edismax by default.
  $query = array('type' => 'edismax');
  if (!empty($collection_pid) && isset($collection_select) && $collection_select == 1) {
    $query['cp'] = $collection_pid;
  }
  drupal_goto('islandora/search/' . $search_string, array('query' => $query));
}

/**
 * Break out acquisition of the collection pid.
 */
function islandora_collection_search_get_collection_pid() {
  $params = $_GET;
  $hidden_filter = NULL;
  $rels_ext = array(
    'RELS_EXT_isMemberOfCollection_uri_ms',
    'RELS_EXT_isMemberOf_uri_ms',
  );
  // Find rels ext in parameters.
  if (isset($params['f'])) {
    foreach ($params['f'] as $filter) {
      $filter_arr = explode(':', $filter);
      foreach ($rels_ext as $rels) {
        if (strpos($filter_arr[0], $rels) === 0) {
          $hidden_filter = $filter;
          break;
        }
      }
    }
  }
  $hidden_match = array();
  if (preg_match('/:"(.*?)"/', $hidden_filter, $hidden_match)) {
    return $hidden_match[1];
  }

  $test_object = menu_get_object('islandora_object', 2);
  if ($test_object) {
    return $test_object->id;
  }

  return FALSE;
}

