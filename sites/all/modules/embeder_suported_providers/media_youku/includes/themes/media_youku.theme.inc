<?php

/**
 * @file media_youku/includes/themes/media_youku.theme.inc
 *
 * Theme and preprocess functions for Media: YouKu.
 */

/**
 * Preprocess function for theme('media_youku_video').
 */
function media_youku_preprocess_media_youku_video(&$variables) {
	// Build the URL for display.
	$uri = $variables['uri'];
	$wrapper = file_stream_wrapper_get_instance_by_uri($uri);
	$parts = $wrapper->get_parameters();
	$variables['video_id'] = check_plain($parts['v']);

	$variables['width'] = isset($variables['width']) ? $variables['width'] : media_youku_variable_get('width');
	$variables['height'] = isset($variables['height']) ? $variables['height'] : media_youku_variable_get('height');
	$variables['autoplay'] = isset($variables['autoplay']) ? $variables['autoplay'] : media_youku_variable_get('autoplay');
	$variables['fullscreen'] = isset($variables['fullscreen']) ? $variables['fullscreen'] : media_youku_variable_get('fullscreen');
	$variables['autoplay'] = $variables['autoplay'] ? 1 : 0;
	$variables['fullscreen'] = $variables['fullscreen'] ? 'true' : 'false';

	$variables['wrapper_id'] = 'media_youku_' . $variables['video_id'] . '_' . $variables['id'];

	// For users with JavaScript, these object and embed tags will be replaced
	// by an iframe, so that we can support users without Flash.
	$variables['output'] = <<<OUTPUT
    <object width="{$variables['width']}" height="{$variables['height']}">
      <param name="movie" value="http://www.youku.com/v/{$variables['video_id']}"></param>
      <param name="allowFullScreen" value="{$variables['fullscreen']}"></param>
      <param name="wmode" value="transparent" />
      <embed src="http://player.youku.com/player.php/sid/{$variables['video_id']}/v.swf" type="application/x-shockwave-flash" width="{$variables['width']}" height="{$variables['height']}" allowfullscreen="{$variables['fullscreen']}"></embed>
    </object>
OUTPUT;

	// @todo Replace this inline JavaScript with at least calls to
	//   drupal_add_js()/drupal_get_js(), and ideally, with a behavior. Keep
	//   in mind that the solution needs to work when inside a colorbox or
	//   otherwise in an AJAX response, but that should now be possible in D7.
	$iframe_id = drupal_json_encode($variables['wrapper_id'] .'_iframe');
	$wrapper_id = drupal_json_encode($variables['wrapper_id']);
	$JSObject = 'Drupal.settings.media_youku[' . $wrapper_id . ']';
	$variables['output'] .= <<<OUTPUT
    <script type="text/javascript">
      if (Drupal.settings && Drupal.media_youku) {
        Drupal.settings.media_youku = Drupal.settings.media_youku || {};
        $JSObject = {};
        $JSObject.width = {$variables['width']};
        $JSObject.height = {$variables['height']};
        $JSObject.video_id = "{$variables['video_id']}";
        $JSObject.fullscreen = {$variables['fullscreen']};
        $JSObject.id = $iframe_id;
        $JSObject.options = { autoplay: {$variables['autoplay']} };
        Drupal.media_youku.insertEmbed($wrapper_id);
      }
    </script>
OUTPUT;

        drupal_add_js(drupal_get_path('module', 'media_youku') . '/js/media_youku.js');
        drupal_add_css(drupal_get_path('module', 'media_youku') . '/css/media_youku.css');
        drupal_add_js(drupal_get_path('module', 'media_youku') . '/js/flash_detect_min.js');
}

function theme_media_youku_field_formatter_styles($variables) {
	$element = $variables['element'];
	$style = $variables['style'];
	$variables['file'] = $element['#item'];
	$variables['uri'] = $variables['file']['uri'];
	$variables['style_name'] = $style['name'];
	return theme('media_youku_embed', $variables);
}

/**
 * Preview for Styles UI.
 */
function theme_media_youku_preview_style($variables) {
	$variables['uri'] = media_youku_variable_get('preview_uri');
	$variables['field_type'] = 'file';
	$variables['object'] = file_uri_to_object($variables['uri']);

	return theme('styles', $variables);
}

/**
 * NOTE: Deprecated with Styles version 2.
 */
function theme_media_youku_styles($variables) {
	$style = $variables['style'];
	$variables['file'] = $variables['object'];
	$variables['uri'] = $variables['object']->uri;
	$variables['style_name'] = $style['name'];
	return theme('media_youku_embed', $variables);
}

/**
 * @todo: get this working
 *
 * This code is for embedding videos in WYSIYWG areas, not currently working.
 * NOTE: Deprecated with Styles version 2.
 */
function theme_media_youku_embed($variables) {
	$preset_name = $variables['preset_name'];
	$preset = styles_containers_available_styles('file', 'media_youku', $preset_name);
	$output = '';
	if (!empty($preset)) {
		// Build the URL for display.
		$uri = $variables['uri'];
		$wrapper = file_stream_wrapper_get_instance_by_uri($uri);
		$parts = $wrapper->get_parameters();

		$fullscreen_value = $autoplay = 'false';
		$in_browser = $thumbnail = FALSE;

		foreach ($preset['effects'] as $effect) {
			switch ($effect['name']) {
				case 'autoplay':
					$autoplay = $effect['data']['autoplay'] ? 'true' : 'false';
					break;
				case 'resize':
					$width = $effect['data']['width'];
					$height = $effect['data']['height'];
					break;
				case 'fullscreen':
					$fullscreen_value = $effect['data']['fullscreen'] ? 'true' : 'false';
					break;
				case 'thumbnail':
					$thumbnail = $effect['data']['thumbnail'];
			}
		}
		if (isset($variables['object']->override)) {
			$override = $variables['object']->override;
			if (isset($override['width'])) {
				$width = $override['width'];
			}
			if (isset($override['height'])) {
				$height = $override['height'];
			}
			if (isset($override['wysiwyg'])) {
				$thumbnail = TRUE;
			}
			if (isset($override['browser']) && $override['browser']) {
				$in_browser = TRUE;
				$thumbnail = TRUE;
			}
		}
		$width = isset($width) ? $width : media_youku_variable_get('width');
		$height = isset($height) ? $height : media_youku_variable_get('height');
		$video_id = check_plain($parts['v']);
		if ($thumbnail) {
			// @todo Clean this up.
			$image_variables = array(
        'path' => $wrapper->getOriginalThumbnailPath(),
        'alt' => $variables['alt'],
        'title' => $variables['title'],
        'getsize' => FALSE,
			);
			if (isset($preset['image_style'])) {
				$image_variables['path'] = $wrapper->getLocalThumbnailPath();
				$image_variables['style_name'] = $preset['image_style'];
				$output = theme('image_style', $image_variables);
			}
			else {
				// We need to add this style attribute here so that it doesn't get lost
				// If you resize a video in a node, save it, edit it, but don't adjust
				// the sizing of the video while editing, the size will revert to the
				// default.  Adding the specific size here retains the original resizing
				$WYSIWYG = isset($variables['object']->override['style']) ? $variables['object']->override['style'] : '';
				$image_variables['attributes'] = array('width' => $width, 'height' => $height, 'style' => $WYSIWYG);
				$output = theme('image', $image_variables);
			}
			if ($in_browser) {
				// Add an overlay that says 'YouKu' to media library browser thumbnails.
				$output .= '<span />';
			}
		}
		else {
			$output = theme('media_youku_video', array('uri' => $uri, 'width' => $width, 'height' => $height, 'autoplay' => ($autoplay == 'true' ? TRUE : NULL), 'fullscreen' => ($fullscreen_value == 'true' ? TRUE : NULL)));
		}
	}
	return $output;
}
