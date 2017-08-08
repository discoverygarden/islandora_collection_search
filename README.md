# Islandora Collection Search

Islandora Collection Search is a Drupal module that allows for searching within a collection using Solr. It provides a more robust replacement for islandora_solr_search's 'Simple Search' block, and it automatically handles appropriate Solr re-indexes when records are moved between collections in Fedora.

## Dependencies

This module requires the following modules/libraries:
* [Islandora](https://github.com/Islandora/islandora)
* [Islandora Solr Search](https://github.com/Islandora/islandora_solr_search)
* [Islandora Solution Pack Collection](https://github.com/Islandora/islandora_solution_pack_collection)

## Installation

  1. Install as usual, see [this](https://drupal.org/documentation/install/modules-themes/modules-7) for further information.
  2. Ensure that your xslt used by gsearch will index the `ancestors_ms` field.  If you are using discoverygarden's [basic-solr-config](https://github.com/discoverygarden/basic-solr-config?source=c), make the following changes in your `foxmlToSolr.xslt`:
      1. Find the section that looks like [this](https://github.com/discoverygarden/basic-solr-config/blob/e161a73abc5bfb0186747174d17a80dcfdc49b4b/foxmlToSolr.xslt#L27-L30), and make it look like this:
      ```
      xmlns:dgi-e="xalan://ca.discoverygarden.gsearch_extensions"
      xmlns:sparql="http://www.w3.org/2001/sw/DataAccess/rf1/result"
      xmlns:xalan="http://xml.apache.org/xalan">
      ```
      2. Find the section that looks like [this](https://github.com/discoverygarden/basic-solr-config/blob/e161a73abc5bfb0186747174d17a80dcfdc49b4b/foxmlToSolr.xslt#L114-L120), and uncomment the code. Leave the comments as comments.
      3. Find the section that looks like [this](https://github.com/discoverygarden/basic-solr-config/blob/e161a73abc5bfb0186747174d17a80dcfdc49b4b/foxmlToSolr.xslt#L277-L287), and uncomment it.
      4. After making these changes, Fedora will need to be restarted, and re-indexed.
  3. Set up the 'Islandora Collection Search' block. It can replace the standard 'Islandora simple search' block.

## Configuration

### Configuring the Module

The configuration page for the module can be found at `yoursite.com/admin/islandora/tools/collection_search`.
* The 'Ancestor field' is set to 'ancestors_ms' by default.
* Configure the 'GSearch Config' section with the proper credentials.
* The options in the 'Collections' section of that page provide ways to configure the UI, such as having a drop-down menu of certain collections that can be searched from anywhere on your site where the 'Collection Search' block is enabled.

### Configuring the Block

The configuration page for the 'Islandora Collection Search' block can be found at `yoursite.com/admin/structure/block/manage/islandora_collection_search/islandora_collection_search/configure`. The main configuration you might want to do is to define which pages display the block for which Roles and/or Users. That can be managed in the 'Visibility settings' section of this block's configuration page.

## Usage

This module provides a block that works like the 'Simple Search' block, but it can narrow the search to objects within a given collection. However, it also has the option to search all collections, which would result in functionality identical to the 'Simple Search' block.

If all the [Installation](#installation) and [Configuration](#configuration) steps have been successfully completed, the powerful and user friendly 'Collection Search' block should now be available.

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
