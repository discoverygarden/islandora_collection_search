<?php

/**
 * @file
 * Batch operation implementation.
 */

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
      if (!in_array($pid, $out_array)) {
        $out_array[] = $pid;
        islandora_collection_search_get_descendants($pid, $out_array);
      }
    }
  }
}

/**
 * A batch description which should reindex the children of $moving.
 */
function islandora_collection_search_reindex_descendants_batch($moving) {
  return array(
    'operations' => array(
      array('islandora_collection_search_get_descendants_batch_operation', array($moving)),
      array('islandora_collection_search_reindex_descendants_batch_operation', array()),
    ),
    'title' => t('Reindexing children...'),
    'init_message' => t('Preparing to reindex children...'),
    'progress_message' => t('Time elapsed: @elapsed <br/>Estimated time remaning @estimate.'),
    'error_message' => t('An error has occurred.'),
    'file' => drupal_get_path('module', 'islandora_collection_search') . '/includes/batch.inc',
  );
}

/**
 * A batch operation to get all descendants of a number of objects.
 *
 * @param array $moving
 *   An array of strings representing the PIDs for which to grab the
 *   descendants.
 * @param array $context
 *   We will place the objects discovered into
 *   $context['results']['to_reindex'].
 */
function islandora_collection_search_get_descendants_batch_operation($moving, &$context) {
  $sandbox = &$context['sandbox'];

  if (!isset($sandbox['offset'])) {
    // Initialize operation state.
    $sandbox['offset'] = 0;
    $sandbox['total'] = count($moving);
    $context['results']['to_reindex'] = array();
    if ($sandbox['total'] === 0) {
      return;
    }
  }

  $pid = $moving[$sandbox['offset']];
  $sandbox['offset']++;

  islandora_collection_search_get_descendants($pid, $context['results']['to_reindex']);

  $context['finished'] = $sandbox['offset'] / $sandbox['total'];
  $context['message'] = t('Discovered descendants of @pid.', array(
    '@pid' => $pid,
  ));
}

/**
 * A batch operation to reindex objects.
 *
 * @param array $context
 *   We expect there to be an array of PIDs to reindex in
 *   $context['results']['to_reindex'].
 */
function islandora_collection_search_reindex_descendants_batch_operation(&$context) {
  $sandbox = &$context['sandbox'];

  if (!isset($sandbox['offset'])) {
    // Initialize operation state.
    $sandbox['offset'] = 0;
    if (!isset($context['results']['to_reindex']) || !is_array($context['results']['to_reindex'])) {
      return;
    }
    $sandbox['total'] = count($context['results']['to_reindex']);
    if ($sandbox['total'] === 0) {
      return;
    }
  }

  $pid = $context['results']['to_reindex'][$sandbox['offset']];
  $sandbox['offset']++;

  islandora_collection_search_reindex_object($pid);

  $context['finished'] = $sandbox['offset'] / $sandbox['total'];
  $context['message'] = t('Reindexed @pid.', array(
    '@pid' => $pid,
  ));
}

/**
 * Trigger a reindex in GSearch.
 *
 * @param array|string $pids
 *   Either an array of strings representing multiple PIDs or a single string
 *   to reindex.
 */
function islandora_collection_search_reindex_object($pids) {
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

  foreach ((array) $pids as $pid) {
    // Poke gsearch with each pid.
    $url = url($gsearch_endpoint, array(
      'query' => $base_params + array(
        'value' => $pid,
      ),
    ));

    drupal_http_request($url, array('headers' => $headers));
  }
}
