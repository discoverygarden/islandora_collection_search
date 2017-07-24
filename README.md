# Islandora Collection Search

A drupal module that allows for searching within a collection using Solr.  Provides a replacement for islandora_solr_search's simple search block and automatically handles appropriate solr re-indexes when records are moved between collections in Fedora.

## Dependencies

This module requires the following modules/libraries:
* [Islandora](https://github.com/Islandora/islandora)
* [Islandora Solr Search](https://github.com/Islandora/islandora_solr_search)
* [Islandora Solution Pack Collection](https://github.com/Islandora/islandora_solution_pack_collection)

## Installation

  1. Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.
  1. Ensure that your xslt used by gsearch will index the ancestor_ms field.  If you are using discoverygarden's [basic-solr-config](https://github.com/discoverygarden/basic-solr-config?source=c), this [gist](https://gist.github.com/daniel-dgi/6001819) will point you in the right direction.
  1. Set up the 'Islandora Collection Search' block. It can replace the standard 'Islandora simple search' block.
  
## Troubleshooting/Issues

Having problems or solved a problem? Contact [discoverygarden](http://support.discoverygarden.ca).
  
## Maintainers/Sponsors
Current maintainers:

* [discoverygarden](https://github.com/discoverygarden)

## Development

If you would like to contribute to this module, please check out our helpful
[Documentation for Developers](https://github.com/Islandora/islandora/wiki#wiki-documentation-for-developers)
info, [Developers](http://islandora.ca/developers) section on Islandora.ca, and
contact [discoverygarden](http://support.discoverygarden.ca).

## License

[GPLv3](http://www.gnu.org/licenses/gpl-3.0.txt)
