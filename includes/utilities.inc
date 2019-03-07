<?php

/**
 * @file
 * Utility functions for the collection search module.
 */

/**
 * Retrieves all searchable collections.
 *
 * @param array $options
 *   The default set of options to be appended to.
 * @param bool|string $collection
 *   Bool denoting whether we are on a collection when searching, the PID
 *   otherwise.
 *
 * @return array
 *   An associative array where the key is the pid of the collection object
 *   and the value is the label.
 */
function islandora_collection_search_retrieve_searchable_collections(array $options, $collection = FALSE) {
  // @FIXME
// Could not extract the default value because it is either indeterminate, or
// not scalar. You'll need to provide a default value in
// config/install/islandora_collection_search.settings.yml and config/schema/islandora_collection_search.schema.yml.
$searchable_collections = \Drupal::config('islandora_collection_search.settings')->get('islandora_collection_search_searchable_collections');
  $searchable_collections = array_filter($searchable_collections);
  foreach ($searchable_collections as $collection_pid) {
    // Don't want to add the current collection twice or inaccessible
    // collections.
    $search_object = islandora_object_load($collection_pid);
    if ($search_object && islandora_object_access(ISLANDORA_VIEW_OBJECTS, $search_object) && $collection_pid != $collection) {
      $options[$collection_pid] = $search_object->label;
    }
  }
  return $options;
}
