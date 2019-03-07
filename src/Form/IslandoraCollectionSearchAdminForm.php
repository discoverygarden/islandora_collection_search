<?php

namespace Drupal\islandora_collection_search\Form;

use Drupal\Core\Form\FormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Render\Element;

/**
 * @file
 * Admin form and submission handler.
 */
class IslandoraCollectionSearchAdminForm extends FormBase {

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'islandora_collection_search_admin_form';
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    form_load_include($form_state, 'inc', 'islandora_collection_search', 'includes/admin.form');
    $form['ancestor_field'] = [
      '#title' => t('Ancestor field'),
      '#type' => 'textfield',
      '#required' => TRUE,
      '#description' => t('The Solr field that contains ancestor data.'),
      '#default_value' => \Drupal::config('islandora_collection_search.settings')->get('islandora_collection_search_ancestor_field'),
    ];
    $form['gsearch'] = [
      '#title' => t('GSearch Config'),
      '#type' => 'fieldset',
      '#description' => t('Some details about GSearch are required so we can reindex child objects when necessary (e.g. moving a collection from one collection to another).'),
      'islandora_collection_search_gsearch_endpoint' => [
        '#title' => t('GSearch Endpoint'),
        '#type' => 'textfield',
        '#required' => TRUE,
        '#default_value' => \Drupal::config('islandora_collection_search.settings')->get("islandora_collection_search_gsearch_endpoint"),
      ],
      'islandora_collection_search_gsearch_user' => [
        '#title' => t('GSearch User'),
        '#type' => 'textfield',
        '#required' => TRUE,
        '#default_value' => \Drupal::config('islandora_collection_search.settings')->get("islandora_collection_search_gsearch_user"),
      ],
      'islandora_collection_search_gsearch_password' => [
        '#title' => t('GSearch Password'),
        '#type' => 'password',
        '#default_value' => \Drupal::config('islandora_collection_search.settings')->get("islandora_collection_search_gsearch_password"),
      ],
      'blank_password' => [
        '#type' => 'checkbox',
        '#title' => t('Make password blank? Current password will be preserved if unchecked.'),
        '#states' => [
          'visible' => [
            'input[name=islandora_collection_search_gsearch_password]' => [
              'value' => '',
              ],
            ],
          ],
        '#default_value' => FALSE,
      ],
    ];
    $form['collections'] = [
      '#title' => t('Collections'),
      '#type' => 'fieldset',
      '#description' => t('Collections selected will appear as selectable options within the search dropdown. Note that the current collection and the ability to search all collections will always be available regardless of configuration.'),
      '#collapsed' => FALSE,
      '#collapsible' => TRUE,
    ];
    $all_collections = islandora_basic_collection_get_collections();
    $header = [
      'label' => ['data' => t('Label')],
      'pid' => ['data' => t('PID')],
    ];
    $options = [];
    foreach ($all_collections as $collection_info) {
      $options[$collection_info['pid']] = [
        'label' => [
          'data' => $collection_info['label']
          ],
        'pid' => ['data' => $collection_info['pid']],
      ];
    }
  // @FIXME
// Could not extract the default value because it is either indeterminate, or
// not scalar. You'll need to provide a default value in
// config/install/islandora_collection_search.settings.yml and config/schema/islandora_collection_search.schema.yml.
    $form['collections']['collection_selection'] = [
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#empty' => t('No collections available.'),
      '#default_value' => \Drupal::config('islandora_collection_search.settings')->get('islandora_collection_search_searchable_collections'),
    ];
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => t('Configure'),
    ];
    $form['collections']['all_pages'] = [
      '#type' => 'checkbox',
      '#title' => t('Display collection selection on all pages?'),
      '#description' => t("When selected this will display the collection selection box on all pages with the previously selected collection options"),
      '#default_value' => \Drupal::config('islandora_collection_search.settings')->get('islandora_collection_search_all_pages'),
    ];
    $form['collections']['collection_label'] = [
      '#type' => 'checkbox',
      '#title' => t('Display collection label?'),
      '#description' => t("When selected this will display the current collection's label for display as opposed to this collection"),
      '#default_value' => \Drupal::config('islandora_collection_search.settings')->get('islandora_collection_search_display_label'),
    ];
    $form['collections']['advanced_search_alter'] = [
      '#type' => 'checkbox',
      '#title' => t('Display a searchable collection field in advanced search?'),
      '#default_value' => \Drupal::config('islandora_collection_search.settings')->get('islandora_collection_search_advanced_search_alter'),
    ];
    $form['collections']['retain_values_on_search_results'] = [
      '#type' => 'checkbox',
      '#title' => t('Retain values searched for on result pages?'),
      '#default_value' => \Drupal::config('islandora_collection_search.settings')->get('islandora_collection_search_retain_search_values'),
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    \Drupal::configFactory()->getEditable('islandora_collection_search.settings')->set('islandora_collection_search_ancestor_field', $form_state->getValue(['ancestor_field']));
    \Drupal::configFactory()->getEditable('islandora_collection_search.settings')->set('islandora_collection_search_gsearch_endpoint', $form_state->getValue(['islandora_collection_search_gsearch_endpoint']));
    \Drupal::configFactory()->getEditable('islandora_collection_search.settings')->set('islandora_collection_search_gsearch_user', $form_state->getValue(['islandora_collection_search_gsearch_user']));
    \Drupal::configFactory()->getEditable('islandora_collection_search.settings')->set('islandora_collection_search_display_label', $form_state->getValue(['collection_label']));
    if ($form_state->getValue(['islandora_collection_search_gsearch_password']) || $form_state->getValue(['blank_password'])) {
      \Drupal::configFactory()->getEditable('islandora_collection_search.settings')->set('islandora_collection_search_gsearch_password', $form_state->getValue(['islandora_collection_search_gsearch_password']));
    }
    \Drupal::configFactory()->getEditable('islandora_collection_search.settings')->set('islandora_collection_search_all_pages', $form_state->getValue(['all_pages']));
    \Drupal::configFactory()->getEditable('islandora_collection_search.settings')->set('islandora_collection_search_searchable_collections', $form_state->getValue(['collection_selection']));
    \Drupal::configFactory()->getEditable('islandora_collection_search.settings')->set('islandora_collection_search_advanced_search_alter', $form_state->getValue(['advanced_search_alter']));
    \Drupal::configFactory()->getEditable('islandora_collection_search.settings')->set('islandora_collection_search_retain_search_values', $form_state->getValue(['retain_values_on_search_results']));
  }

}
