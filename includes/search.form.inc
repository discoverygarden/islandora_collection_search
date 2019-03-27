<?php

/**
 * @file
 * Form helper.
 */

use AbstractObject;

/**
 * Break out acquisition of the collection pid.
 *
 * @return bool|string
 *   FALSE if it's not a collection, the PID otherwise.
 */
function islandora_collection_search_get_collection_pid() {
  $params = \Drupal::request()->query->all();
  $hidden_filter = NULL;
  $rels_ext = [
    \Drupal::config('islandora_solr.settings')->get('islandora_solr_member_of_collection_field'),
    \Drupal::config('islandora_solr.settings')->get('islandora_solr_member_of_field'),
  ];

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
  $hidden_match = [];
  if (preg_match('@.*:"(?:info\\\:fedora\\\/)?(.*)"@', $hidden_filter, $hidden_match)) {
    $pid = str_replace('\\', '', $hidden_match[1]);
    return $pid;
  }

  $test_object = \Drupal::routeMatch()->getParameter('object');
  if ($test_object instanceof AbstractObject) {
    return $test_object->id;
  }

  return FALSE;
}
