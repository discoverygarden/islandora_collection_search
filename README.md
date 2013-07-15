Islandora Collection Search
===========================

A drupal module that allows for searching within a collection using Solr.  Provides a replacement for islandora_solr_search's simple search block and automatically handles appropriate solr re-indexes when records are moved between collections in Fedora.

Dependencies
------------
  - islandora
  - islandora_solr
  - islandora_basic_collection

Installation
------------
  1. Download and install all dependencies.
  1. Download this module and place it in the appropriate directory for your site (sites/all/modules is a safe bet if you don't know).
  1. Enable this module through drupal's admin interface or via drush (drush en islandora_collection_search).
  1. Ensure that your xslt used by gsearch will index the ancestor_ms field.  If you are using discoverygarden's [basic-solr-config], this [gist] will point you in the right direction.
  1. Set up the 'Islandora Collection Search' block as a replacement for the standard 'Islandora simple search' block.

[basic-solr-config]: https://github.com/discoverygarden/basic-solr-config?source=c
[gist]: https://gist.github.com/daniel-dgi/6001819
