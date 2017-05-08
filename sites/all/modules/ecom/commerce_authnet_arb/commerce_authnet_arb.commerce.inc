<?php

/**
 * @file
 * Integration with a commerce framework.
 */

/**
 * Authnet ARB payment method settings form.
 */
function commerce_authnet_arb_commerce_settings_pane ($settings = NULL) {

  $arb_settings = NULL;
  if (!empty($settings)) {
    $arb_settings = array(
      'login' => $settings['settings']['commerce_authnet_arb_login_id'],
      'tran_key' => $settings['settings']['commerce_authnet_arb_tran_key'],
      'sandbox' => $settings['settings']['commerce_authnet_arb_sandbox'],
      'md5_hash' => $settings['settings']['commerce_authnet_arb_md5_hash'],
      'watchdog_all' => $settings['settings']['commerce_authnet_arb_debugging_watchdog_all'],
    );
  }

  $form = commerce_authnet_arb_settings_pane($arb_settings, TRUE);

  drupal_set_message(t('You may change ARB payment settings on !page page',
    array('!page' => l(t('ARB settings'), 'admin/config/commerce_authnet_arb'))),'warning');

  return $form;
}

/**
 * Payment method callback: submit form.
 *
 * @param array $payment_method
 *   Payment method.
 * @param mixed $pane_values
 *   Pane values.
 * @param mixed $checkout_pane
 *   Checkout pane.
 * @param array $order
 *   User order.
 *
 * @return array
 *   Payment pane form.
 */
function commerce_authnet_arb_payment_pane($payment_method, $pane_values, $checkout_pane, $order) {
  $form = array();
  module_load_include('inc', 'commerce_payment', 'includes/commerce_payment.credit_card');
  $credit_card = commerce_payment_credit_card_form(array('code' => ''));
  $form['credit_card'] = $credit_card['credit_card'];
  $recurring = array();

  // Allow hooks to alter the pane.
  $recurring = module_invoke_all('commerce_authnet_arb_commerce_set_recurring', $order);
  $form['recurring_options'] = commerce_authnet_arb_recurring_info_pane($recurring);
  $form['base_price'] = array(
    '#type' => 'hidden',
    '#value' => $order->commerce_order_total[LANGUAGE_NONE][0]['data']['components'][0]['price']['amount'],
  );

  return $form;
}

/**
 * Authnet ARB payment method settings form validation.
 */
function commerce_authnet_arb_payment_pane_validate($payment_method, $pane_form, $pane_values, $order, $form_parents = array()) {

  $valid = TRUE;

  // Validate the credit card fields.
  module_load_include('inc', 'commerce_payment', 'includes/commerce_payment.credit_card');
  $settings = array(
    'form_parents' => array_merge($form_parents, array('credit_card')),
  );
  if (!commerce_payment_credit_card_validate($pane_values['credit_card'], $settings)) {
    $valid = FALSE;
  }

  // Validating recurring settings.
  if(commerce_authnet_arb_recurring_info_pane_validation($pane_values['recurring_options'], $form_parents) === FALSE) {
    $valid = FALSE;
  }


  if ($valid === FALSE) {
    return FALSE;
  }

  // Validate card processing.
  $charge = commerce_payment_order_balance($order);

  // If the line items was not added yet, set amount to $0.01
  if (empty($charge['amount'])) {
    $charge['amount'] = '1';
  }

  if (!commerce_authnet_arb_verify_payment($charge, $pane_values['credit_card'])) {
    $prefix = implode('][', $form_parents) . '][';
    form_set_error($prefix, t('The card can\' be authorized.'));
    $valid =  FALSE;
  }

  return $valid;
}

/**
 * Authnet ARB payment method settings form submit.
 */
function commerce_authnet_arb_payment_pane_submit($payment_method, $pane_form, $pane_values, &$order, $charge) {
  if (isset($pane_values['base_price'])) {
    $charge['amount'] = $pane_values['base_price'];
  }
  return commerce_authnet_arb_create_subscription($pane_values['recurring_options'], $charge, $pane_values['credit_card'],
    array(), $order);
}
