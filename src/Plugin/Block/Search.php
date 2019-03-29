<?php

namespace Drupal\islandora_collection_search\Plugin\Block;

use Drupal\islandora\Plugin\Block\AbstractFormBlockBase;
use Drupal\Core\Access\AccessResult;
use Drupal\Core\Session\AccountInterface;

/**
 * Block for searching collections.
 *
 * @Block(
 *   id = "islandora_collection_search",
 *   admin_label = @Translation("Islandora Collection Search"),
 * )
 */
class Search extends AbstractFormBlockBase {

  /**
   * {@inheritdoc}
   */
  public function build() {
    return \Drupal::formBuilder()->getForm('\Drupal\islandora_collection_search\Form\Search');
  }

  /**
   * {@inheritdoc}
   */
  protected function blockAccess(AccountInterface $account) {
    return AccessResult::allowedIfHasPermission($account, 'search islandora solr');
  }

}
