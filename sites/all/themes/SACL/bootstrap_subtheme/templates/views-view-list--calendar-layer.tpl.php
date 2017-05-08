<?php

/**
 * @file
 * Default simple view template to display a list of rows.
 *
 * - $title : The title of this group of rows.  May be empty.
 * - $options['type'] will either be ul or ol.
 * @ingroup views_templates
 */
/* $view_calander = views_get_view('page_1');
/* $view->set_current_page(5);
$view->set_items_per_page(10); 
$view->execute();
$objects = $view->result;
print_r($objects);
$vars=array(); */
/* $vars['view']="calendar:page_1";
echo theme_calendar_stripe_legend($vars);

  $view = views_get_view('calendar');
  $view->set_display('page_1');
  $filter1['value'] =  array(
  'calendar_layer' => 'field_layers',
);
  $view->get_item('page_1', 'filter', 'filter_1');
  //$view->display_handler->display->display_options['filters']['field_layers']['value'] = 'calender_layers';
   $view->display_handler->display->display_options['filters']['field_layers']['default_value'] = 'calender_layers'; */
  $view = views_get_view('calendar');
  $view->set_display('page_1'); 
  $row_options = $view->display_handler->get_option('row_options');
  $legend_type = $row_options['colors']['legend'];
  $display_options = $row_options['colors']['calendar_colors_' . $legend_type];
  //print_r($display_options);
?>
<?php print $wrapper_prefix; ?>
  <?php if (!empty($title)) : ?>
    <h3><?php print $title; ?></h3>
  <?php endif; ?>
  <?php print $list_type_prefix; ?>
    <?php //foreach ($rows as $id => $row):
	//print_r($classes_array);
	 $id=1;
	?>
    <?php foreach ($display_options as $tid => $color): 
		$term = taxonomy_term_load($tid);
		if($term->vid == 6){
		//print_r($term);	
		?>
      <li class="<?php echo ($id%2==0)? 'views-row views-row-'.($id-1) . ' views-row-even':'views-row views-row-'.($id-1) . ' views-row-odd'; //print $classes_array[$id]; ?>"><div class="layer-box"><div class="layer-box-color" style="background-color:<?php echo $color; ?>">&nbsp;</div><div class="layer-box-text"><?php
echo $term->name;
	  
	  //print $row; ?></div></div></li>
    <?php $id++; } endforeach; ?>
  <?php print $list_type_suffix; ?>
<?php print $wrapper_suffix; ?>
