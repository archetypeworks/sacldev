<?php

/**
 *  @file
 *  Create a Vzaar Stream Wrapper class for the Media/Resource module.
 */

/**
 *  Create an instance like this:
 *  $vzaar = new ResourceVzaarStreamWrapper('vzaar://?v=[video-code]');
 */
class MediaVzaarStreamWrapper extends MediaReadOnlyStreamWrapper {
  protected $base_url = 'http://vzaar.com/watch';

  function getTarget($f) {
    return FALSE;
  }

  static function getMimeType($uri, $mapping = NULL) {
    return 'video/vzaar';
  }

  function getOriginalThumbnailPath() {
    $parts = $this->get_parameters();
    return 'http://view.vzaar.com/'. check_plain($parts['v']) .'/image';
  }

  function getLocalThumbnailPath() {
    $parts = $this->get_parameters();
    $local_path = 'public://media-vzaar/' . check_plain($parts['v']) . '.jpg';
    if (!file_exists($local_path)) {
      $dirname = drupal_dirname($local_path);
      file_prepare_directory($dirname, FILE_CREATE_DIRECTORY | FILE_MODIFY_PERMISSIONS);
      @copy($this->getOriginalThumbnailPath(), $local_path);
    }
    return $local_path;
  }
}
