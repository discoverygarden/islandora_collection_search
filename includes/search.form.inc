<?php

/**
 * @file
 * Form helper.
 */

/**
 * Break out acquisition of the collection pid.
 *
 * @return bool|string
 *   FALSE if it's not a collection, the PID otherwise.
 */
function islandora_collection_search_get_collection_pid() {
  $query = \Drupal::request()->query;
  $hidden_filter = NULL;
  $rels_ext = [
    \Drupal::config('islandora_solr.settings')->get('islandora_solr_member_of_collection_field'),
    \Drupal::config('islandora_solr.settings')->get('islandora_solr_member_of_field'),
  ];

  // Find rels ext in parameters.
  if ($query->has('f')) {
    foreach ($query->get('f') as $filter) {
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
