<?php

/**
 * @file
 * Extends the MediaInternetBaseHandler class to handle YouTube videos.
 */

/**
 * Implementation of MediaInternetBaseHandler.
 *
 * @see hook_media_internet_providers().
 */
class MediaInternetFaceBookHandler extends MediaInternetBaseHandler {
  public function parse($embedCode) {
    // http://youtube.com/watch/*
    // http://youtube.com/embed/*
    // http://youtube.com/v/*
    // http://youtube.com/?v=*
    // http://youtu.be/*
    // http://gdata.youtube.com/feeds/api/videos/*
    $patterns = array(
      '@youtube\.com/watch[#\?].*?v=([^"\& ]+)@i',
      '@youtube\.com/embed/([^"\&\? ]+)@i',
      '@youtube\.com/v/([^"\&\? ]+)@i',
      '@youtube\.com/\?v=([^"\& ]+)@i',
      '@youtu\.be/([^"\&\? ]+)@i',
      '@gdata\.youtube\.com/feeds/api/videos/([^"\&\? ]+)@i',
    );
preg_match_all('#(\d{8,})#', $embedCode,$matches);
//print_r($matches);
//die;
      // @TODO: Parse is called often. Refactor so that valid ID is checked
      // when a video is added, but not every time the embedCode is parsed.
      if (isset($matches[1]) && self::validId(end($matches[1]))) {
        return file_stream_wrapper_uri_normalize('facebook://v/' . end($matches[1]));
      }
    
  }

  public function claim($embedCode) {
    if ($this->parse($embedCode)) {
      return TRUE;
    }
  }

  public function getFileObject() {
    $uri = $this->parse($this->embedCode);
    $file = file_uri_to_object($uri, TRUE);

    // Try to default the file name to the video's title.
    if (empty($file->fid) && $info = $this->getOEmbed()) {
      $file->filename = truncate_utf8($info['title'], 255);
    }

    return $file;
  }

  /**
   * Returns information about the media.
   *
   * See http://www.oembed.com.
   *
   * @return
   *   If oEmbed information is available, an array containing 'title', 'type',
   *   'url', and other information as specified by the oEmbed standard.
   *   Otherwise, NULL.
   */
  public function getOEmbed() {
    $uri = $this->parse($this->embedCode);
    $external_url = file_create_url($uri);
	//preg_match_all('#(\d{8,})#', $this->embedCode , $matches);
	preg_match_all('#(\d{8,})#', $this->embedCode,$matches);
    $oembed_url = 'https://graph.facebook.com/'.end($matches[1]);
    $response = drupal_http_request($oembed_url);

    if (!isset($response->error)) {
      return drupal_json_decode($response->data);
    }
    else {
      throw new Exception("Error Processing Request. (Error: {$response->code}, {$response->error})");
      return;
    }
  }

  /**
   * Check if a YouTube video ID is valid.
   *
   * @return boolean
   *   TRUE if the video ID is valid, or throws a
   *   MediaInternetValidationException otherwise.
   */
  static public function validId($id) {
    $uri = file_stream_wrapper_uri_normalize('facebook://v/' . check_plain($id));

    $oembed_url = 'https://graph.facebook.com/'.$id;
	//echo $oembed_url; die;
    $response = drupal_http_request($oembed_url, array('method' => 'HEAD'));

    if ($response->code == 401) {
      throw new MediaInternetValidationException('Embedding has been disabled for this YouTube video.');
    }
    elseif ($response->code != 200) {
      throw new MediaInternetValidationException('The YouTube video ID is invalid or the video was deleted.');
    }

    return TRUE;
  }
}
