<?php

/**
 * @file
 * Hooks provided by the Date Restrictions module.
 */

/**
 * @defgroup dr_api Hooks to implement a module providing date restrictions.
 * @{
 */

/**
 * Default settings.
 */
function hook_date_restrictions_settings() {
  $settings['myrestrictions'] = array(
    'date_min' => array(
      'interval' => null,
      'period' => null,
    ),
  );
  return $settings;
}

/**
 * Settings form.
 */
function hook_date_restrictions_instance_settings_form($settings) {
  $form['date_min'] = array(
    '#type' => 'interval',
    '#title' => t('Minimum date'),
    '#default_value' => $settings['restrictions']['date_min'],
  );
  return $form;
}

/**
 * Javascript settings for datepicker.
 */
function hook_date_restrictions_date_popup_settings($element, $form_state, $context) {
  $date_min = $element['#restrictions']['date_min'];
  $date = date_restrictions_dateobject('now', $element);
  interval_apply_interval($date, $date_min);
  $settings['minDate'] = $date;
  // url to ask for day availability per year/month.
  $settings['checkDateCallbackUrl'] = url('date-restrictions-hook');
  return $settings;
}

/**
 * Validates input date.
 */
function hook_date_restrictions_element_validate($date, $element, $form_state, $form) {
  $min = new DateObject();
  interval_apply_interval($min, $element['#restrictions']['date_min']);
  if ($date < $min) {
    return form_error($element, t('Invalid date.'));
  }
}

/**
 * @} End of "defgroup dr_api"
 */
