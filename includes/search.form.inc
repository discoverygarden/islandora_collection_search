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
  $options = array(
    0 => t('All collections'),
    1 => t('This collection'),
  );
  $form['simple'] = array(
    '#type' => 'container',
    '#attributes' => array(
      'class' => array(
        'container-inline',
      ),
    ),
  );
  $form['simple']['hidden_filter'] = array(
    '#type' => 'hidden',
    '#value' => $hidden_filter,
  );
  if (!empty($hidden_filter)) {
    $form['simple']['collection_select'] = array(
      '#type' => 'select',
      '#options' => $options,
      '#default_value' => (!empty($hidden_filter)) ? 1 : 0,
    );
  }
  $form['simple']["islandora_simple_search_query"] = array(
    '#size' => '15',
    '#type' => 'textfield',
    '#title' => '',
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
 *
 * @param array $form_state
 *   Associative array with form state information.
 */
function islandora_collection_search_form_submit($form, &$form_state) {
  $form_state['rebuild'] = TRUE;
  $search_string = $form_state['values']['islandora_simple_search_query'];
  // Replace the slash so url doesn't break.
  $search_string = str_replace('/', '~slsh~', $search_string);
  preg_match('/:"(.*?)"/', $form_state['values']['hidden_filter'], $hidden_match);
  $hidden_filter = $hidden_match[1];
  $collection_pid = $hidden_filter;
  if (isset($form_state['values']['collection_select'])) {
    $collection_select = $form_state['values']['collection_select'];
  }

  // Using edismax by default.
  $query = array('type' => 'edismax');
  if (!empty($hidden_filter) && isset($collection_select) && $collection_select == 1) {
    $query['cp'] = $collection_pid;
  }
  drupal_goto('islandora/search/' . $search_string, array('query' => $query));
}
