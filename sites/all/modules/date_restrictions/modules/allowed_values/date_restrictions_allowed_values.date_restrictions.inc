<?php

/**
 * @defgroup dr_hooks Implements date_restrictions hooks.
 * @{
 */

/**
 * Implements hook_date_restrictions_settings().
 */
function date_restrictions_allowed_values_date_restrictions_settings() {
  $settings = array(
    'allowed_values' => array(
      'type' => 'disabled',
    ),
  );

  $info = date_restrictions_allowed_values_info();
  foreach ($info as $type => $data) {
    $settings['allowed_values'][$type] = isset($data['settings'])?$data['settings']:null;
  }

  return $settings;
}

/**
 * Implements hook_date_restrictions_instance_settings_form().
 */
function date_restrictions_allowed_values_date_restrictions_instance_settings_form($settings, $context) {
  // Fieldset for restriction types.
  $form['allowed_values'] = array(
    '#type' => 'fieldset',
    '#collapsible' => true,
    '#collapsed' => true,
    '#title' => t('Allowed values'),
  );
  // Select restriction type.
  $id = drupal_html_id('date-restrictions-allowed-values');
  $form['allowed_values']['type'] = array(
    '#id' => $id,
    '#type' => 'select',
    '#title' => t('Choose restriction type'),
    '#default_value' => $settings['allowed_values']['type'],
  );
  // Restriction type elements.
  $options = array();
  $info = date_restrictions_allowed_values_info();
  foreach ($info as $type => $data) {
    $options[$type] = $data['title'];
    if ($missing = date_restrictions_check_dependencies($data['dependencies'])) {
      $msg = '<span class="marker">'. t('This restriction type is not available. Required modules: @modules.', array('@modules' => implode(',', $missing))) .'</span>';
      $form['allowed_values'][$type] = array(
        '#missing' => true,
        '#type' => 'item',
        '#markup' => $msg,
      );
    }
    else {
      $element = module_invoke($data['module'], 'allowed_values_settings_form', $type, $settings['allowed_values'][$type], $context);
      $form['allowed_values'][$type] = $element;
      $form['allowed_values'][$type]['#missing'] = false;
    }
  }
  asort($options);
  $form['allowed_values']['type']['#options'] = array('disabled' => t('Do not restrict')) + $options;

  // Set #states.
  $selector = 'select#' . $id;
  foreach (array_keys($form['allowed_values']['type']['#options']) as $type) {
    if ($type != 'disabled') {
      $form['allowed_values'][$type]['#states'] = array(
        'visible' => array(
          $selector => array('value' => $type),
        ),
      );
    }
  }

  $form['#element_validate'] = array('date_restrictions_allowed_values_instance_settings_form_validate');

  return $form;
}

function date_restrictions_allowed_values_instance_settings_form_validate(&$form, &$form_state) {
  $settings = $form_state['values']['instance']['settings']['restrictions'];
  $info = date_restrictions_allowed_values_info();

  $type = $settings['allowed_values']['type'];
  if ($type != 'disabled') {
    if ($form['allowed_values'][$type]['#missing']) {
      form_error($form['allowed_values'], t('Invalid restriction type for %type.', array('%type' => 'allowed_values')));
    }
    else {
      $module = $info[$type]['module'];
      $hook = 'allowed_values_settings_validate';
      if (module_hook($module, $hook)) {
        $function = $module . '_' . $hook;
        $function($type, $settings['allowed_values'][$type], $form, $form_state);
      }
    }
  }
}

/**
 * Implements hook_date_restrictions_element_validate().
 */
function date_restrictions_allowed_values_date_restrictions_element_validate($date, $element, $form_state, $form) {
  $restrictions = date_restrictions_settings($element['#restrictions']);
  $type = $restrictions['allowed_values']['type'];
  if ($type != 'disabled') {
    $info = date_restrictions_allowed_values_info();
    if (!module_invoke($info[$type]['module'], 'allowed_values_valid_date', $date, $type, $restrictions['allowed_values'][$type], $element, $form_state, $form)) {
      $error = t('Invalid date @date', array('@date' => date_format_date($date, 'custom', $element['#date_format'])));
      form_error($element, $error);
    }
  }
}

/**
 * @} End of "defgroup dr_hooks"
 */

