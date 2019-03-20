<?php

namespace Drupal\islandora_collection_search\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;

/**
 * Form for searching within a given collection (or site wide).
 */
class Search extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'islandora_collection_search_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $form_state->loadInclude('islandora_collection_search', 'inc', 'includes/search.form');
    $form_state->loadInclude('islandora_collection_search', 'inc', 'includes/utilities');

    $form['simple'] = [
      '#type' => 'container',
      '#attributes' => [
        'class' => [
          'container-inline',
        ],
      ],
    ];

    $collection = islandora_collection_search_get_collection_pid();
    $options = [
      'all' => $this->t('All collections'),
    ];
    $default_search = 'all';
    if ($collection) {
      // See if the current object has children, and if so make it available for
      // searching otherwise get its parent.
      $qp = new IslandoraSolrQueryProcessor();
      $qp->buildQuery('*:*');
      $qp->solrParams['fq'][] = strtr(
        '!is_memberofcollection:"info:fedora/!pid" OR !is_member:"info:fedora/!pid"',
        [
          '!is_memberofcollection' => $this->config('islandora_solr.settings')->get('islandora_solr_member_of_collection_field'),
          '!is_memberof' => $this->config('islandora_solr.settings')->get('islandora_solr_member_of_field'),
          '!pid' => $collection,
        ]
      );

      $qp->executeQuery(FALSE);
      // If no children need to find the immediate parent to search for instead.
      if (!$qp->islandoraSolrResult['response']['numFound']) {
        module_load_include('inc', 'islandora_basic_collection', 'includes/utilities');
        $child_object = islandora_object_load($collection);
        $parents = islandora_basic_collection_get_parent_pids($child_object);
        $collections = array_filter(array_combine($parents, 'islandora_object_load', $parents));
      }
      $show_label = $this->config('islandora_collection_search.settings')->get('islandora_collection_search_display_label');
      if (!empty($collections)) {
        // If there is more than one possible parents we can't differentiate for
        // the "This collection" case, resort to using the labels.
        if (count($collections) > 1) {
          foreach ($collections as $object) {
            $options[$object->id] = $object->label;
          }
        }
        else {
          $object = reset($collections);
          $options[$object->id] = $show_label ? $object->label : $this->t('This collection');
        }
        $first_object = reset($collections);
        $default_search = $first_object->id;
      }
      else {
        $collection_object = islandora_object_load($collection);
        $options[$collection_object->id] = $show_label ? $collection_object->label : $this->t('This collection');
        $default_search = $collection_object->id;
      }
    }
    $options = islandora_collection_search_retrieve_searchable_collections($options, $collection);
    $all_pages = $this->config('islandora_collection_search.settings')->get('islandora_collection_search_all_pages');
    if ($all_pages) {
      if (!$collection) {
        $collection = $all_pages;
      }
    }
    $default_search_value = '';
    // Check if we're on a search results page.
    if ($this->config('islandora_collection_search.settings')->get('islandora_collection_search_retain_search_values') && strpos(Url::fromRoute('<current>')->toString(), 'islandora/search') === 0) {
      if (isset($_GET['cp'])) {
        $default_search = $_GET['cp'];
      }
      $default_search_value = arg(2);
    }
    $form['simple']['collection_select'] = [
      '#access' => $collection !== FALSE,
      '#type' => 'select',
      '#title' => $this->t('Select Collection'),
      '#title_display' => 'invisible',
      '#options' => $options,
      '#default_value' => $default_search,
    ];
    $form['simple']["islandora_simple_search_query"] = [
      '#size' => '15',
      '#type' => 'textfield',
      '#title' => $this->t('Search box'),
      '#title_display' => 'invisible',
      '#default_value' => $default_search_value,
    ];
    $form['simple']['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('search'),
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $form_state->setRebuild();
    $search_string = $form_state->getValue('islandora_simple_search_query');
    // Replace the slash so URL doesn't break.
    $search_string = islandora_solr_replace_slashes($search_string);
    $collection_select = isset($form_state->getValues()['collection_select']) ?
      $form_state->getValue('collection_select') :
      FALSE;

    // Using edismax by default.
    $query = ['type' => 'edismax'];
    if (isset($collection_select) && $collection_select !== 'all') {
      $query['cp'] = $collection_select;
    }
    $form_state->setRedirect(
      'islandora_solr.islandora_solr',
      ['query' => $search_string],
      ['query' => $query]
    );
  }

}
