<?php

/**
 * @defgroup dr_hooks Implements date_restrictions hooks.
 * @{
 */

/**
 * Implements hook_date_restrictions_settings().
 */
function date_restrictions_minmax_date_restrictions_settings() {
  $settings = array(
    'min' => array(
      'type' => 'disabled',
    ),
    'max' => array(
      'type' => 'disabled',
    ),
  );

  $info = date_restrictions_minmax_info();
  foreach (array('min', 'max') as $limit) {
    foreach ($info as $type => $data) {
      $settings[$limit][$type] = isset($data['settings'])?$data['settings']:null;
    }
  }

  return $settings;
}

/**
 * Implements hook_date_restrictions_instance_settings_form().
 */
function date_restrictions_minmax_date_restrictions_instance_settings_form($settings, $context) {
  // Fieldsets for minimum and maximum date constraints.
  $form['min'] = $form['max'] = array(
    '#type' => 'fieldset',
    '#collapsible' => true,
    '#collapsed' => true,
  );
  $form['min'] += array(
    '#title' => t('Minimum date'),
    '#description' => t('Choose whether to force a minimum allowed value in this field.'),
    '#weight' => 0,
  );
  $form['max'] += array(
    '#title' => t('Maximum date'),
    '#description' => t('Choose whether to force a maximum allowed value in this field.'),
    '#weight' => 1,
  );

  // Restriction type.
  $form['min']['type'] = $form['max']['type'] = array(
    '#type' => 'select',
    '#title' => t('Choose restriction type'),
  );

  $info = date_restrictions_minmax_info();
  foreach (array('min', 'max') as $limit) {
    $options = array();
    foreach ($info as $type => $data) {
      $options[$type] = $data['title'];
      if ($missing = date_restrictions_check_dependencies($data['dependencies'])) {
        $msg = '<span class="marker">'. t('This restriction type is not available. Required modules: @modules.', array('@modules' => implode(',', $missing))) .'</span>';
        $form[$limit][$type] = array(
          '#missing' => true,
          '#type' => 'item',
          '#markup' => $msg,
        );
      }
      else {
        $element = module_invoke($data['module'], 'minmax_settings_form', $type, $limit, $settings[$limit][$type], $context);
        $form[$limit][$type] = $element;
        $form[$limit][$type]['#missing'] = false;
      }
    }
    asort($options);
    $form[$limit]['type']['#options'] = array('disabled' => t('Do not restrict')) + $options;
  }

  // Set #states.
  foreach (array('min', 'max') as $limit) {
    $form[$limit]['type']['#default_value'] = $settings[$limit]['type'];

    // Set a css id and get selector to define #states below.
    $id = drupal_html_id('date-restrictions-minmax-' . $limit . '-type');
    $form[$limit]['type']['#id'] = $id;
    $selector = 'select#' . $id;

    foreach (array_keys($form[$limit]['type']['#options']) as $type) {
      if ($type != 'disabled') {
        $form[$limit][$type]['#states'] = array(
          'visible' => array(
            $selector => array('value' => $type),
          ),
        );
      }
    }
  }

  $form['#element_validate'] = array('date_restrictions_minmax_instance_settings_form_validate');

  return $form;
}

function date_restrictions_minmax_instance_settings_form_validate(&$form, &$form_state) {
  $settings = $form_state['values']['instance']['settings']['restrictions'];

  $info = date_restrictions_minmax_info();

  foreach (array('min', 'max') as $limit) {
    $type = $settings[$limit]['type'];
    if ($type != 'disabled') {
      if ($form[$limit][$type]['#missing']) {
        form_error($form[$limit], t('Invalid restriction type for %type.', array('%type' => $limit)));
      }
      else {
        module_invoke($info[$type]['module'], 'minmax_settings_validate', $type, $limit, $settings[$limit][$type], $form, $form_state);
      }
    }
  }
}

/**
 * Implements hook_date_restrictions_element_validate().
 */
function date_restrictions_minmax_date_restrictions_element_validate($date, $element, &$form_state, $form) {
  $date_ymd = $date->format(DATE_FORMAT_DATE);

  $restrictions = date_restrictions_settings($element['#restrictions']);
  $info = date_restrictions_minmax_info();

  foreach (array('min', 'max') as $limit) {
    $type = $restrictions[$limit]['type'];
    if ($type != 'disabled') {
      $module = $info[$type]['module'];
      $hook = 'minmax_restriction_date';
      if (module_hook($module, $hook)) {
        $function = $module . '_' . $hook;
        $value = $function($type, $restrictions[$limit][$type], $element, $form_state, $form);
        if ($value) {
          $value_ymd = $value->format(DATE_FORMAT_DATE);
          if ((($limit == 'min') && ($date_ymd < $value_ymd)) || (($limit == 'max') && ($date_ymd > $value_ymd))) {
            $vars = array('@date' => date_format_date($value, 'custom', $element['#date_format']));
            $error = ($limit == 'min') ? t('Dates before @date are not allowed.', $vars) : t('Dates after @date are not allowed.', $vars);
            form_error($element, $error);
            break;
          }
        }
      }
    }
  }
}

/**
 * @} End of "defgroup dr_hooks"
 */

/**
 * @defgroup dr_minmax_impl Implements minmax restriction types.
 * @{
 * Implements "fixed date" and "relative interval" minmax restrictions types.
 */

/**
 * Implements hook_date_restrictions_minmax_info().
 */
function date_restrictions_minmax_date_restrictions_minmax_info() {
  return array(
    'date' => array(
      'title' => t('Fixed date'),
    ),
    'interval' => array(
      'title' => t('Relative date'),
      'dependencies' => 'interval',
    ),
    'host_entity_date' => array(
      'title' => t('Host entity: get fixed date from a field in the host entity'),
      'settings' => array(
        'field' => null,
        'end_date' => false,
      ),
      'dependencies' => 'date_restrictions_host_entity',
    ),
    'host_entity_interval' => array(
      'title' => t('Host entity: get relative date from a field in the host entity'),
      'settings' => array(
        'field' => null,
      ),
      'dependencies' => array('date_restrictions_host_entity', 'interval'),
    ),
  );
}

/**
 * Implements minmax_settings_form callback.
 */
function date_restrictions_minmax_minmax_settings_form($type, $limit, $default_value, $context) {
  $label = ($limit == 'min') ? t('minimum') : t('maximum');
  switch ($type) {
    case 'date':
      $element = array(
        '#type' => 'date',
        '#title' => t('Fixed date'),
        '#description' => t('The @label value allowed in this field.', array('@label' => $label)),
        '#default_value' => $default_value,
      );
      break;
    case 'interval':
      $element = array(
        '#type' => 'interval',
        '#title' => t('Relative date'),
        '#description' => t('Interval to compute the @label value allowed in this field, relative to the day the field is created.', array('@label' => $label)),
        '#default_value' => $default_value,
        );
      break;
    case 'host_entity_date':
      $instance = $context['instance'];
      $bundles[$instance['entity_type']] = array($instance['bundle']);
      $options = date_restrictions_get_bundle_instances_as_options($bundles);
      // Exclude self.
      unset($options[$instance['entity_type']. ':' . $instance['bundle'] . ':' .$instance['field_name']]);
      $element = array(
        '#theme_wrappers' => array('container'),
        '#attributes' => array('class' => array('container-inline')),
      );
      $element['field'] = array(
        '#type' => 'select',
        '#title' => t('Field'),
        '#options' => $options,
        '#default_value' => $default_value['field'],
      );
      $element['end_date'] = array(
        '#type' => 'checkbox',
        '#title' => t("Use end date value. If not available, this restriction won't take effect."),
        '#default_value' => $default_value['end_date'],
      );
      break;
    case 'host_entity_interval':
      $instance = $context['instance'];
      $bundles[$instance['entity_type']] = array($instance['bundle']);
      $options = date_restrictions_get_bundle_instances_as_options($bundles, 'interval');
      $element = array(
        '#theme_wrappers' => array('container'),
        '#attributes' => array('class' => array('container-inline')),
      );
      $element['field'] = array(
        '#type' => 'select',
        '#title' => t('Field'),
        '#options' => $options,
        '#default_value' => $default_value['field'],
      );
      break;
  }

  return $element;
}

/**
 * Implements minmax_settings_validate callback.
 */
function date_restrictions_minmax_minmax_settings_validate($type, $limit, $value, $form) {
  if ($type == 'interval') {
    if (empty($value['interval'])) {
      $label = ($limit == 'min') ? t('Minimum date') : t('Maximum date');
      form_error($form[$limit]['interval'], t('@label: interval value is required.', array('@label' => $label)));
    }
  }
}

/**
 * Implements minmax_date_popup_settings callback.
 *
 * host-entity based restriction types need submitted values
 * that are not available at this stage of the form processing.
 * So this is done later, on after build.
 * @see date_restrictions_minmax_host_entity_after_build().
 *
 * For host-entity based restrictions, here we obtain the
 * values from the entity, just the first time the edit form is loaded.
 */
function date_restrictions_minmax_minmax_date_popup_settings($type, $limit, $settings, $element, &$form_state, $context) {
  $date = null;
  switch ($type) {
    case 'date':
      $date = date_restrictions_dateobject($settings, $element);
      break;
    case 'interval':
      $date = date_restrictions_dateobject('now', $element);
      interval_apply_interval($date, $settings);
      break;
    case 'host_entity_date':
      // Is this the first time the edit form is loaded?
      if (!$form_state['process_input']) {
        list($entity_type, $bundle, $field_name) = explode(':', $settings['field']);
        $form = $context['form'];
        $values = field_get_items($form['#entity_type'], $form['#entity'], $field_name);
        if ($values) {
          $value = (!$settings['end_date']) ? $values[0]['value'] : $values[0]['value2'];
          $date = date_restrictions_dateobject($value, $element);
        }
      }
      break;
    case 'host_entity_interval':
      // Is this the first time the edit form is loaded?
      if (!$form_state['process_input']) {
        list($entity_type, $bundle, $field_name) = explode(':', $settings['field']);
        $form = $context['form'];
        $value = field_get_items($form['#entity_type'], $form['#entity'], $field_name);
        if ($value) {
          $date = date_restrictions_dateobject('now', $element);
          interval_apply_interval($date, $value[0]);
        }
      }
      break;
  }

  $date_format = date_popup_date_format($element);
  return date_format_date($date, 'custom', $date_format);
}

/**
 * Implements hook_host_entity_after_build().
 *
 * Compute settings for min/max dates and attach js settings.
 *
 * This is an after_build, we receive the full date form element:
 * date_combo / date_popup, maybe multivalued.
 *
 * @see date_restrictions_host_entity_form_after_build().
 * @see date_restrictions_minmax_minmax_date_popup_settings().
 */
function date_restrictions_minmax_host_entity_after_build($element, $column, $restriction, $form_state) {
  $js_settings = array();

  $langcode = $element['#language'];
  foreach (element_children($element[$langcode]) as $key) {
    $child = $element[$langcode][$key][$column];
    if ($child['#type'] == 'date_popup') {
      $restrictions = ($column == 'value') ? $element[$langcode][$key]['#restrictions'] : $element[$langcode][$key]['#restrictions2'];
      $type = $restrictions[$restriction]['type'];
      if ($type != 'disabled') {
        $settings = $restrictions[$restriction][$type];
        $value = _date_restrictions_minmax_host_entity_after_build_date_popup_get_value($child, $type, $settings, $form_state);
        if ($value) {
          $js_settings[$child['date']['#id']]['settings'][$restriction . 'Date'] = $value;
        }
      }
    }
  }

  if (!empty($js_settings)) {
    drupal_add_js(array('datePopup' => $js_settings), 'setting');
  }
}

/**
 * Obtain the actual value of the source field and transform as needed.
 *
 * Helper function for date_restrictions_minmax_host_entity_after_build().
 */
function _date_restrictions_minmax_host_entity_after_build_date_popup_get_value($element, $type, $settings, $form_state) {
  $date = null;
  switch ($type) {
    case 'host_entity_date':
      list($entity_type, $bundle, $field_name) = explode(':', $settings['field']);
      if (isset($form_state[HOST_ENTITY_KEY]['values'][$field_name])) {
        $values = $form_state[HOST_ENTITY_KEY]['values'][$field_name];
        $value = (!$settings['end_date']) ? $values[0]['value'] : $values[0]['value2'];
        // 'value' is array('date' => '') if the date field is empty when editing.
        if ($value && (is_string($value))) {
          $date = date_restrictions_dateobject($value, $element);
        }
      }
      break;
    case 'host_entity_interval';
      list($entity_type, $bundle, $field_name) = explode(':', $settings['field']);
      if (isset($form_state[HOST_ENTITY_KEY]['values'][$field_name])) {
        $values = $form_state[HOST_ENTITY_KEY]['values'][$field_name];
        if (is_numeric($values[0]['interval'])) {
          $date = date_restrictions_dateobject('now', $element);
          interval_apply_interval($date, $value);
        }
      }
      break;
  }

  $date_format = date_popup_date_format($element);
  return date_format_date($date, 'custom', $date_format);
}

/**
 * Implements minmax_restriction_date callback.
 */
function date_restrictions_minmax_minmax_restriction_date($type, $settings, $element, &$form_state, $form) {
  $date = null;
  switch ($type) {
    case 'date':
      $date = date_restrictions_dateobject($settings, $element);
      break;
    case 'interval':
      $date = date_restrictions_dateobject('now', $element);
      interval_apply_interval($date, $settings);
      break;
    case 'host_entity_date':
      list($entity_type, $bundle, $field_name) = explode(':', $settings['field']);
      if (isset($form_state[HOST_ENTITY_KEY]['values'][$field_name])) {
        $values = $form_state[HOST_ENTITY_KEY]['values'][$field_name];
        $value = (!$settings['end_date']) ? $values[0]['value'] : $values[0]['value2'];
        // 'value' is array('date' => '') if the date field is empty when editing.
        if ($value && (is_string($value))) {
          $date = date_restrictions_dateobject($value, $element);
        }
      }
      break;
    case 'host_entity_interval':
      list($entity_type, $bundle, $field_name) = explode(':', $settings['field']);
      if (isset($form_state[HOST_ENTITY_KEY]['values'][$field_name])) {
        $values = $form_state[HOST_ENTITY_KEY]['values'][$field_name];
        if (is_numeric($values[0]['interval'])) {
          $date = date_restrictions_dateobject('now', $element);
          interval_apply_interval($date, $values[0]);
        }
      }
      break;
  }

  return $date;
}

/**
 * @} End of "defgroup dr_minmax_impl"
 */

