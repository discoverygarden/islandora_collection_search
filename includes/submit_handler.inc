<?php

/**
 * @file
 * Submit handler to reindex children being migrated and associated functions.
 */


/**
 * Re-indexes all children of the object(s) manipulated by a form.
 *
 * @param array $form
 *   Array of drupal form api elements.
 * @param array $form_state
 *   Array representing the form state.
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
  $gsearch_endpoint = variable_get("islandora_collection_search_gsearch_endpoint", "http://localhost:8080/fedoragsearch/rest");
  $gsearch_user = variable_get("islandora_collection_search_gsearch_user", "fedoraAdmin");
  $gsearch_password = variable_get("islandora_collection_search_gsearch_password", "fedoraAdmin");


  $base_params = array(
    'operation' => 'updateIndex',
    'action' => 'fromPid',
  );
  $headers = array(
    'Authorization' => 'Basic: ' . base64_encode(rawurlencode($gsearch_user) . ':' . rawurlencode($gsearch_password)),
  );

  // Poke gsearch with each pid.
  foreach ($to_reindex as $pid) {
    $url = url($gsearch_url, array('query' => $base_params + array(
      'value' => $pid,
    )));

    drupal_http_request($url, array('headers' => $headers));
  }
}

/**
 * Recursively constructs a list of descendants for the given pid.
 *
 *
 * @param string $pid
 *   The pid whose descendants we wish to obtain.
 * @param array $out_array
 *   An array passed by reference throughout the whole recursive walk.  It
 *   is filled with the descendants from each recursive call of the function.
 *   If this breaks memory constraints, we could make this write to a file...
 *   Might then have to contend with time constraints?
 */
function islandora_collection_search_get_descendants($pid, &$out_array) {
  // SPARQL query to get all children for a given pid.
  $query = "PREFIX fre: <info:fedora/fedora-system:def/relations-external#>
            PREFIX fm: <info:fedora/fedora-system:def/model#>
            SELECT ?obj
            FROM <#ri>
            WHERE {
              {
                ?obj fre:isMemberOfCollection ?sub
              }
              UNION
              {
                ?obj fre:isMemberOf ?sub
              }
              ?obj fm:state fm:Active .
              ?sub fm:state fm:Active
              FILTER(sameTerm(?sub, <info:fedora/$pid>))
            }";

  // Use tuque to get a connection to Fedora and execute the SPARQL query.
  $tuque = islandora_get_tuque_connection();
  $repo = $tuque->repository;
  $results = $repo->ri->sparqlQuery($query);

  if (!empty($results)) {
    foreach ($results as $result) {
      $pid = $result['obj']['value'];
      $out_array[] = $pid;
      islandora_collection_search_get_descendants($pid, $out_array);
    }
  }
}
