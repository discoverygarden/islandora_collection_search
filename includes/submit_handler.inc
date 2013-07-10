<?php

/**
 * @file
 * A submit handler to reindex children being migrated.
 */


/**
 * Re-indexes all children of the object(s) manipulated by a form.
 *
 * @param array $form
 *   Array of drupal form api elements.
 *
 * @param array $form_state
 *   Array representing the form state.
 *
 * @param string $form_id
 *   The name of the form.
 */
function islandora_collection_search_migrate_children_form_submit($form, &$form_state, $form_id) {
  // Grab the initial list of children that are being migrated from the form.
  $children = array_keys(array_filter($form_state['values']['children']));

  // Recursively construct the pid list of children to re-index in solr.
  $to_reindex = array();
  foreach ($children as $child) {
    islandora_collection_search_get_descendants($child, $to_reindex);
  }
  $to_reindex = array_unique($to_reindex);

  // Construct the url to ping gsearch to perform the re-index.
  $base_url = variable_get("islandora_collection_search_gsearch_endpoint", "http://localhost:8080/fedoragsearch/rest");
  $base_url .= "?operation=updateIndex&action=fromPid&value=";

  //
  // RESUME WORK HERE
  //
  // Need to fiddle with the default value for the variable_get so it's
  // reasonable.  Also need to set up an admin page so the user can edit
  // the 'islandora_collection_search_gsearch_endpoint' variable.
  //
  // Right now the gsearch endpoing isn't working for me locally, but i'm sure
  // it's a pretty simple DERP to fix.  It would be really nice if we could
  // simply put the message on the queue programatically, but i'm not sure
  // how we'd do that in PHPland.

  // Poke gsearch with each pid.
  foreach ($to_reindex as $pid) {
    $route = $base_url . $pid;
    dd($route);
    drupal_http_request($route);
  }
}
