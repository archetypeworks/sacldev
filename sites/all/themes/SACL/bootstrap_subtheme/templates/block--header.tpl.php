<?php

/**
 * @file
 * Default theme implementation to display a region.
 *
 * Available variables:
 * - $content: The content for this region, typically blocks.
 * - $classes: String of classes that can be used to style contextually through
 *   CSS. It can be manipulated through the variable $classes_array from
 *   preprocess functions. The default values can be one or more of the following:
 *   - region: The current template type, i.e., "theming hook".
 *   - region-[name]: The name of the region with underscores replaced with
 *     dashes. For example, the page_top region would have a region-page-top class.
 * - $region: The name of the region variable as defined in the theme's .info file.
 *
 * Helper variables:
 * - $classes_array: Array of html class attribute values. It is flattened
 *   into a string within the variable $classes.
 * - $is_admin: Flags true when the current user is an administrator.
 * - $is_front: Flags true when presented in the front page.
 * - $logged_in: Flags true when the current user is a logged-in member.
 *
 * @see template_preprocess()
 * @see template_preprocess_region()
 * @see template_process()
 *
 * @ingroup themeable
 */
 global $base_url;
?>
<?php global $theme_path; if($block->delta=="login" && $block->module=="user" ){ ?> 
<div class="container">
        <div class="member-login">
	<!--<div id="<?php print $block_html_id; ?>" class="<?php print $classes; ?>"<?php print $attributes; ?>>-->

	  <?php print render($title_prefix); ?>
	<?php if ($block->subject): ?>
	<h3><span class="circle"><img alt="login-icon" src="<?php echo $base_url."/".$theme_path; ?>/bootstrap_subtheme/images/login-icon.png"></span><?php print $block->subject ?><strong>Members login only</strong></h3>
	 <!-- <h2 <?php print $title_attributes; ?>><?php print $block->subject ?></h2>-->
	<?php endif;?>
	  <?php print render($title_suffix); ?>

	 <!-- <div class="content"<?php print $content_attributes; ?>>-->
		<?php print $content; ?>
	 <!-- </div>-->
	<!--</div>-->
	</div>
</div> <?php
}else{ ?>
	<!--<div id="<?php print $block_html_id; ?>" class="<?php print $classes; ?>"<?php print $attributes; ?>>-->

	  <?php print render($title_prefix); ?>
	<?php if ($block->subject): ?>
	  <h2 <?php print $title_attributes; ?>><?php print $block->subject ?></h2>
	<?php endif;?>
	  <?php print render($title_suffix); ?>

	 <!-- <div class="content"<?php print $content_attributes; ?>>-->
		<?php print $content; ?>
	 <!-- </div>-->
	<!--</div>-->
<?php } ?>