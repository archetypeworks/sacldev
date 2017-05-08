<?php
/**
 * @file
 * Template to display a view as a calendar month.
 * 
 * @see template_preprocess_calendar_month.
 *
 * $day_names: An array of the day of week names for the table header.
 * $rows: An array of data for each day of the week.
 * $view: The view.
 * $calendar_links: Array of formatted links to other calendar displays - year, month, week, day.
 * $display_type: year, month, day, or week.
 * $block: Whether or not this calendar is in a block.
 * $min_date_formatted: The minimum date for this calendar in the format YYYY-MM-DD HH:MM:SS.
 * $max_date_formatted: The maximum date for this calendar in the format YYYY-MM-DD HH:MM:SS.
 * $date_id: a css id that is unique for this date, 
 *   it is in the form: calendar-nid-field_name-delta
 * 
 */
//dsm('Display: '. $display_type .': '. $min_date_formatted .' to '. $max_date_formatted);
?>
<div class="calendar-calendar"><div class="month-view">
<table class="full">
  <thead>
    <tr>
      <?php foreach ($day_names as $id => $cell): ?>
        <th class="<?php print $cell['class']; ?>" id="<?php print $cell['header_id'] ?>">
          <?php print $cell['data']; ?>
        </th>
      <?php endforeach; ?>
    </tr>
  </thead>
  <tbody>
    <?php 
      foreach ((array) $rows as $row) {
        print $row['data'];
      } ?>
  </tbody>
</table>
</div></div>
<style>
/**************Date select ******* filename : <calendar module>/theme/calender-month.tpl.php******************/
/***********************Hello Michael you will be modify this section to design for select date*******Starting point****************/ 
/***********Michael, You can also move this part at /public_html/sites/all/themes/SACL/bootstrap_subtheme/css/style.css file also where you working****************/
.calendar-calendar .month-view .full tr td.date-select, .calendar-calendar .month-view .full tr.odd td.date-select, .calendar-calendar .month-view .full tr.even td.date-select{
    background: rgba(0, 0, 0, 0) none repeat scroll 0 0;
    border-left: 2px solid #424242;
    border-right: 2px solid #424242;
	z-index:2000;
	background: rgba(0, 0, 0, 0) none repeat scroll 0 0;
}
.calendar-calendar .month-view .full td.date-box.date-select {
    border-color: #424242;
    border-style: solid;
    border-width: 2px 2px 0;
}
.calendar-calendar .month-view .full tr td.single-day.date-select {
    border-bottom: 2px solid #424242;
}

.view-calendar .view-content{position: relative; top: -46px;}
/***********************Hello Michael you will be modify this section to design for select date*******Ending point****************/ 
</style>
<script>
/**************Date select ******* filename : <calendar module>/theme/calender-month.tpl.php******************/
jQuery(document).ready(function(){
 	jQuery(".calendar-calendar").find("td").click(function(){
//alert('[data-date="'+jQuery(this).attr("data-date")+'"]');
	jQuery('[data-date="'+jQuery(this).attr("data-date")+'"]').toggleClass( "date-select" )
	}); 
});
try {
  // ie hack to make the single day row expand to available space
  if ($.browser.msie ) {
    var multiday_height = $('tr.multi-day')[0].clientHeight; // Height of a multi-day row
    $('tr[iehint]').each(function(index) {
      var iehint = this.getAttribute('iehint');
      // Add height of the multi day rows to the single day row - seems that 80% height works best
      var height = this.clientHeight + (multiday_height * .8 * iehint); 
      this.style.height = height + 'px';
    });
  }
}catch(e){
  // swallow 
}
</script>