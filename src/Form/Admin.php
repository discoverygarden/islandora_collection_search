<?php

namespace Drupal\islandora_collection_search\Form;

use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;

use Drupal\islandora\Utility\StateTrait;

/**
 * Admin form and submission handler.
 */
class Admin extends ConfigFormBase {

  use StateTrait;

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'islandora_collection_search_admin_form';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return ['islandora_collection_search.settings'];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('islandora_collection_search.settings');
    $form['ancestor_field'] = [
      '#title' => $this->t('Ancestor field'),
      '#type' => 'textfield',
      '#required' => TRUE,
      '#description' => $this->t('The Solr field that contains ancestor data.'),
      '#default_value' => $config->get('islandora_collection_search_ancestor_field'),
    ];
    $form['gsearch'] = [
      '#title' => $this->t('GSearch Config'),
      '#type' => 'fieldset',
      '#description' => $this->t('Some details about GSearch are required so we can reindex child objects when necessary (e.g. moving a collection from one collection to another).'),
      'islandora_collection_search_gsearch_endpoint' => [
        '#title' => $this->t('GSearch Endpoint'),
        '#type' => 'textfield',
        '#required' => TRUE,
        '#default_value' => static::stateGet("islandora_collection_search_gsearch_endpoint"),
      ],
      'islandora_collection_search_gsearch_user' => [
        '#title' => $this->t('GSearch User'),
        '#type' => 'textfield',
        '#required' => TRUE,
        '#default_value' => static::stateGet("islandora_collection_search_gsearch_user"),
      ],
      'islandora_collection_search_gsearch_password' => [
        '#title' => $this->t('GSearch Password'),
        '#type' => 'password',
        '#default_value' => static::stateGet("islandora_collection_search_gsearch_password"),
      ],
      'blank_password' => [
        '#type' => 'checkbox',
        '#title' => $this->t('Make password blank? Current password will be preserved if unchecked.'),
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
      '#title' => $this->t('Collections'),
      '#type' => 'fieldset',
      '#description' => $this->t('Collections selected will appear as selectable options within the search dropdown. Note that the current collection and the ability to search all collections will always be available regardless of configuration.'),
      '#collapsed' => FALSE,
      '#collapsible' => TRUE,
    ];
    $all_collections = islandora_basic_collection_get_collections();
    $header = [
      'label' => ['data' => $this->t('Label')],
      'pid' => ['data' => $this->t('PID')],
    ];
    $options = [];
    foreach ($all_collections as $collection_info) {
      $options[$collection_info['pid']] = [
        'label' => [
          'data' => $collection_info['label'],
        ],
        'pid' => ['data' => $collection_info['pid']],
      ];
    }
    $form['collections']['collection_selection'] = [
      '#type' => 'tableselect',
      '#header' => $header,
      '#options' => $options,
      '#empty' => $this->t('No collections available.'),
      '#default_value' => $config->get('islandora_collection_search_searchable_collections'),
    ];
    $form['submit'] = [
      '#type' => 'submit',
      '#value' => $this->t('Configure'),
    ];
    $form['collections']['all_pages'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Display collection selection on all pages?'),
      '#description' => $this->t("When selected this will display the collection selection box on all pages with the previously selected collection options"),
      '#default_value' => $config->get('islandora_collection_search_all_pages'),
    ];
    $form['collections']['collection_label'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Display collection label?'),
      '#description' => $this->t("When selected this will display the current collection's label for display as opposed to this collection"),
      '#default_value' => $config->get('islandora_collection_search_display_label'),
    ];
    $form['collections']['advanced_search_alter'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Display a searchable collection field in advanced search?'),
      '#default_value' => $config->get('islandora_collection_search_advanced_search_alter'),
    ];
    $form['collections']['retain_values_on_search_results'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Retain values searched for on result pages?'),
      '#default_value' => $config->get('islandora_collection_search_retain_search_values'),
    ];
    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    static::stateSet('islandora_collection_search_gsearch_endpoint', $form_state->getValue(['islandora_collection_search_gsearch_endpoint']));
    static::stateSet('islandora_collection_search_gsearch_user', $form_state->getValue(['islandora_collection_search_gsearch_user']));
    if ($form_state->getValue(['islandora_collection_search_gsearch_password']) || $form_state->getValue(['blank_password'])) {
      static::stateSet('islandora_collection_search_gsearch_password', $form_state->getValue(['islandora_collection_search_gsearch_password']));
    }

    $config = $this->config('islandora_collection_search.settings');
    $config->set('islandora_collection_search_ancestor_field', $form_state->getValue(['ancestor_field']));
    $config->set('islandora_collection_search_display_label', $form_state->getValue(['collection_label']));
    $config->set('islandora_collection_search_all_pages', $form_state->getValue(['all_pages']));
    $config->set('islandora_collection_search_searchable_collections', $form_state->getValue(['collection_selection']));
    $config->set('islandora_collection_search_advanced_search_alter', $form_state->getValue(['advanced_search_alter']));
    $config->set('islandora_collection_search_retain_search_values', $form_state->getValue(['retain_values_on_search_results']));

    $config->save();
  }

  /**
   * {@inheritdoc}
   */
  public static function stateDefaults() {
    return [
      'islandora_collection_search_gsearch_endpoint' => 'localhost:8080/fedoragsearch/rest',
      'islandora_collection_search_gsearch_user' => 'fedoraAdmin',
      'islandora_collection_search_gsearch_password' => 'fedoraAdmin',
    ];
  }

}
