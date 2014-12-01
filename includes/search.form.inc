<?php

/**
 * @file
 * Form submit handler and the like.
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
  module_load_include('inc', 'islandora_collection_search', 'includes/utilities');
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
  $options = array(
    'all' => t('All collections'),
  );
  $default_search = 'all';
  if ($collection) {
    // See if the current object has children, and if so make it available for
    // searching otherwise get its parent.
    $qp = new IslandoraSolrQueryProcessor();
    $qp->buildQuery('*:*');
    $qp->solrParams['fq'][] = format_string('!is_memberofcollection:"info:fedora/!pid" OR !is_member:"info:fedora/!pid"',
      array(
        '!is_memberofcollection' => variable_get('islandora_solr_member_of_collection_field', 'RELS_EXT_isMemberOfCollection_uri_ms'),
        '!is_memberof' => variable_get('islandora_solr_member_of_field', 'RELS_EXT_isMemberOf_uri_ms'),
        '!pid' => $collection,
      )
    );
    $qp->executeQuery(FALSE);
    // If no children need to find the immediate parent to search for instead.
    if (!$qp->islandoraSolrResult['response']['numFound']) {
      module_load_include('inc', 'islandora_basic_collection', 'includes/utilities');
      $child_object = islandora_object_load($collection);
      $parents = islandora_basic_collection_get_parent_pids($child_object);
      $collections = drupal_map_assoc($parents);
    }
    $show_label = variable_get('islandora_collection_search_display_label', FALSE);
    if (!empty($collections)) {
      // If there are more than one possible parents we can't differentiate for
      // the "This collection" case, resort to using the labels.
      if (count($collections) > 1) {
        foreach ($collections as $pid) {
          $object = islandora_object_load($pid);
          $options[$object->id] = $object->label;
        }
      }
      else {
        $pid = reset($collections);
        $object = islandora_object_load($pid);
        $options[$object->id] = $show_label ? $object->label : t('This collection');
      }
      $default_search = reset($collections);
    }
    else {
      $collection_object = islandora_object_load($collection);
      $options[$collection_object->id] = $show_label ? $collection_object->label : t('This collection');
      $default_search = $collection_object->id;
    }
  }
  $options = islandora_collection_search_retrieve_searchable_collections($options, $collection);
  $all_pages = variable_get('islandora_collection_search_all_pages', FALSE);
  if ($all_pages) {
    if (!$collection) {
      $collection = $all_pages;
    }
  }
  $default_search_value = '';
  // Check if we're on a search results page.
  if (variable_get('islandora_collection_search_retain_search_values', FALSE) && strpos(current_path(), 'islandora/search') === 0) {
    if (isset($_GET['cp'])) {
      $default_search = $_GET['cp'];
    }
    $default_search_value = arg(2);
  }
  $form['simple']['collection_select'] = array(
    '#access' => $collection !== FALSE,
    '#type' => 'select',
    '#options' => $options,
    '#default_value' => $default_search,
  );
  $form['simple']["islandora_simple_search_query"] = array(
    '#size' => '15',
    '#type' => 'textfield',
    '#title' => t('Search box'),
    '#title_display' => 'invisible',
    '#default_value' => $default_search_value,
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
  $collection_select = isset($form_state['values']['collection_select']) ?
    $form_state['values']['collection_select'] :
    FALSE;

  // Using edismax by default.
  $query = array('type' => 'edismax');
  if (isset($collection_select) && $collection_select !== 'all') {
    $query['cp'] = $collection_select;
  }
  drupal_goto('islandora/search/' . $search_string, array('query' => $query));
}

/**
 * Break out acquisition of the collection pid.
 *
 * @return bool|string
 *   FALSE if it's not a collection, the PID otherwise.
 */
function islandora_collection_search_get_collection_pid() {
  $params = $_GET;
  $hidden_filter = NULL;
  $rels_ext = array(
    variable_get('islandora_solr_member_of_collection_field', 'RELS_EXT_isMemberOfCollection_uri_ms'),
    variable_get('islandora_solr_member_of_field', 'RELS_EXT_isMemberOf_uri_ms'),
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
