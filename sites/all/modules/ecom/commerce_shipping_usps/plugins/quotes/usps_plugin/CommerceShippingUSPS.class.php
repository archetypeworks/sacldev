<?php

class CommerceShippingUSPS extends CommerceShippingQuote {
  /**
   * Settings form to configure USPS quoting service
   */
  public function settings_form(&$form, $rules_settings) {
    $form['store-info'] = array(
      '#title' => 'Store information',
      '#type' => 'fieldset'
    );

    $form['store-info']['name'] = array(
      '#type' => 'textfield',
      '#title' => t('Name'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['store-info']['name']) ? $rules_settings['store-info']['name'] : '',
    );

    $form['store-info']['owner'] = array(
      '#type' => 'textfield',
      '#title' => t('Owner'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['store-info']['owner']) ? $rules_settings['store-info']['owner'] : '',
    );

    $form['store-info']['email'] = array(
      '#type' => 'textfield',
      '#title' => t('Email Address'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['store-info']['email']) ? $rules_settings['store-info']['email'] : '',
    );

    $form['store-info']['phone'] = array(
      '#type' => 'textfield',
      '#title' => t('Phone'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['store-info']['phone']) ? $rules_settings['store-info']['phone'] : '',
    );

    $form['store-info']['fax'] = array(
      '#type' => 'textfield',
      '#title' => t('Fax'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['store-info']['fax']) ? $rules_settings['store-info']['fax'] : '',
    );

    $form['store-info']['street1'] = array(
      '#type' => 'textfield',
      '#title' => t('Street #1'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['store-info']['street1']) ? $rules_settings['store-info']['street1'] : '',
    );

    $form['store-info']['street2'] = array(
      '#type' => 'textfield',
      '#title' => t('Street #2'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['store-info']['street2']) ? $rules_settings['store-info']['street2'] : '',
    );

    $form['store-info']['city'] = array(
      '#type' => 'textfield',
      '#title' => t('City'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['store-info']['city']) ? $rules_settings['store-info']['city'] : '',
    );

    $form['store-info']['zone'] = array(
      '#type' => 'textfield',
      '#title' => t('State/Province'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['store-info']['zone']) ? $rules_settings['store-info']['zone'] : '',
    );

    $form['store-info']['postal_code'] = array(
      '#type' => 'textfield',
      '#title' => t('Postal Code'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['store-info']['postal_code']) ? $rules_settings['store-info']['postal_code'] : '',
    );

    $form['store-info']['country'] = array(
      '#type' => 'textfield',
      '#title' => t('Country'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['store-info']['country']) ? $rules_settings['store-info']['country'] : 'US',
    );

    $form['shipment-settings'] = array(
      '#type' => 'fieldset',
      '#title' => 'Shipment Settings',
    );


    $form['shipment-settings']['usps_services'] = array(
      '#type' => 'checkboxes',
      '#title' => t('USPS Services'),
      '#description' => t('Select the USPS services that are available to customers.'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['shipment-settings']['usps_services']) ? $rules_settings['shipment-settings']['usps_services'] : array(),
      '#options' => _commerce_shipping_usps_service_list(),
    );

    $form['shipment-settings']['usps-markup-type'] = array(
      '#type' => 'select',
      '#title' => t('Markup type'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['shipment-settings']['usps-markup-type']) ? $rules_settings['shipment-settings']['usps-markup-type'] : 'percentage',
      '#options' => array(
        'percentage' => t('Percentage (%)'),
        'multiplier' => t('Multiplier (×)'),
        'currency' => t('Addition ($)'),
      ),
    );

    $form['shipment-settings']['usps-markup'] = array(
      '#type' => 'textfield',
      '#title' => t('Shipping rate markup'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['shipment-settings']['usps-markup']) ? $rules_settings['shipment-settings']['usps-markup'] : '0',
      '#description' => t('Markup shipping rate quote by currency amount, percentage, or multiplier.'),
    );

    $form['shipment-settings']['currency_code'] = array(
      '#type' => 'textfield',
      '#title' => t('Currency Code'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['shipment-settings']['currency_code']) ? $rules_settings['shipment-settings']['currency_code'] : 'USD',
    );

    $form['connection-settings'] = array(
      '#type' => 'fieldset',
      '#title' => 'USPS Connection Settings',
    );

    $form['connection-settings']['usps_connection_address'] = array(
      '#type' => 'textfield',
      '#title' => t('Connection Address'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['connection-settings']['usps_connection_address']) ? $rules_settings['connection-settings']['usps_connection_address'] : 'http://Production.ShippingAPIs.com/ShippingAPI.dll',
    );

    $form['connection-settings']['usps_user_name'] = array(
      '#type' => 'textfield',
      '#title' => t('User name'),
      '#default_value' => is_array($rules_settings) && isset($rules_settings['connection-settings']['usps_user_name']) ? $rules_settings['connection-settings']['usps_user_name'] : '',
      '#description' => t('The user name for your USPS account.'),
    );
  }

  /**
   * Checkout Form for selecting USPS shipment method
   */
  public function submit_form($pane_values, $checkout_pane, $order = NULL) {
    if (empty($order)) {
      $order = $this->order;
    }
    $form = parent::submit_form($pane_values, $checkout_pane, $order);

    // Merge in values from the order.
    if (!empty($order->data['commerce_shipping_usps'])) {
      $pane_values += $order->data['commerce_shipping_usps'];
    }
    /* TODO: Find a better way with some nice array function */
    $method = NULL;
    $all_methods = _commerce_shipping_usps_service_list();
    $methods = array();
    foreach ($this->settings['shipment-settings']['usps_services'] as $key => $service) {
      if (!$method) 
        $method = $key;
      if ($service !== 0) {
        $methods[$key] = $all_methods[$key];
      }
    }

    // Merge in default values.
    $pane_values += array(
      'method' => $method,
    );

    $form['method'] = array(
      '#type' => 'radios',
      '#title' => t('Shipment Method'),
      '#default_value' => $method,
      '#options' => $methods,
    );

    return $form;
  }

  /**
   * Validation form.
   */
  public function submit_form_validate($pane_form, $pane_values, $form_parents = array(), $order = NULL) {
  }

  /**
   * Calculate Quote
   */
  public function calculate_quote($currency_code, $form_values = array(), $order = NULL, $pane_form = NULL, $pane_values = NULL) {
    $method = $form_values['method'];
    $shipping_address = $pane_values['values']['customer_profile_shipping']['commerce_customer_address'][LANGUAGE_NONE][0];
    $rate = $this->usps_quote($order, $method, $shipping_address);

    $all_methods = _commerce_shipping_usps_service_list();

    if (empty($order)) {
      $order = $this->order;
    }
    $settings = $this->settings;
    $shipping_line_items = array();
    $shipping_line_items[] = array(
      'amount' => commerce_currency_decimal_to_amount($rate, $currency_code),
      'currency_code' => $currency_code,
      'label' => t($all_methods[$method]),
    );

    return $shipping_line_items;
  }

  /**
   * Callback for retrieving a USPS shipping quote.
   *
   * Request a quote for the requested USPS Service.
   *
   * @param $order
   *   Cart Order.
   *
   * @param $method
   *   The Shipping Method to Quote For
   *
   * @param $shipping_address
   *   The Address to Ship to
   *
   * @return
   *   An array of hotness
   */
  private function usps_quote($order, $method, $shipping_address) {
    $store_info = $this->settings['store-info'];
    $shipment_settings = $this->settings['shipment-settings'];
    $connection_settings = $this->settings['connection-settings'];

    $shipment_weight = 0;
    foreach ($order->commerce_line_items[LANGUAGE_NONE] as $order_line) {
      $line_item_id = $order_line['line_item_id'];
      $line_item = commerce_line_item_load($line_item_id);
      $product = commerce_product_load($line_item->commerce_product[LANGUAGE_NONE][0]['product_id']);

      $product_weight = isset($product->field_weight[LANGUAGE_NONE]) ? $product->field_weight[LANGUAGE_NONE][0]['value'] : 0;
      $weight = $product_weight * $line_item->quantity;
      $shipment_weight += $weight;
    }

    $ounces = $shipment_weight - floor($shipment_weight);
    $ounces = 16 * $ounces;
    $pounds = floor($shipment_weight);

    $shipto_zip = $shipping_address['postal_code'];
    $shipfrom_zip = $store_info['postal_code'];
    $data = "<Package ID=\"1ST\"><Service>{$method}</Service><ZipOrigination>{$shipfrom_zip}</ZipOrigination><ZipDestination>{$shipto_zip}</ZipDestination><Pounds>{$pounds}</Pounds><Ounces>{$ounces}</Ounces><Size>REGULAR</Size><Machinable>TRUE</Machinable></Package>";
    $request = $this->usps_access_request($data);
    $resp = drupal_http_request($this->settings['connection-settings']['usps_connection_address'], array('method' => 'POST', 'data' => $request));

    /**
     * <?xml version="1.0"?> 
     * <RateV3Response>
     *   <Package ID="1ST">
     *     <ZipOrigination>37214</ZipOrigination>
     *     <ZipDestination>37214</ZipDestination>
     *     <Pounds>5</Pounds>
     *     <Ounces>0</Ounces>
     *     <Container></Container>
     *     <Size>REGULAR</Size>
     *     <Zone>1</Zone>
     *     <Postage CLASSID="3">
     *       <MailService>Express Mail&amp;lt;sup&amp;gt;&amp;amp;reg;&amp;lt;/sup&amp;gt;</MailService>
     *       <Rate>19.60</Rate>
     *     </Postage>
     *   </Package>
     * </RateV3Response>
     */

    if ($resp->code == '200' & !empty($resp->data)) {
      $quote = new SimpleXMLElement($resp->data);
      $rate = (Float)$quote->Package->Postage->Rate;
      
      return $rate;
    } else {
      return NULL;
    }
  }



  /**
   * Return XML access request to be prepended to all requests to the USPS webservice.
   */
  private function usps_access_request($data) {
    $user_name = $this->settings['connection-settings']['usps_user_name'];

    return "API=RateV3&XML=<RateV3Request USERID=\"$user_name\">". $data ."</RateV3Request>";
  }

  /**
   * Modify the rate received from USPS before displaying to the customer.
   */
  private function usps_markup($rate) {
    $markup = $this->settings['shipment-settings']['usps-markup'];
    $type = $this->settings['shipment-settings']['usps-markup-type'];
    if (is_numeric(trim($markup))) {
      switch ($type) {
        case 'percentage':
          return $rate + $rate * floatval(trim($markup)) / 100;
        case 'multiplier':
          return $rate * floatval(trim($markup));
        case 'currency':
          return $rate + floatval(trim($markup));
      }
    }
    else {
      return $rate;
    }
  }

}

/** Kinda Lifted from Ubercart **/

/**
 * Convenience function to get USPS codes for their services.
 */
function _commerce_shipping_usps_service_list() {
  return array(
    'FIRST CLASS' => 'First Class',
    'FIRST CLASS COMMERCIAL' => 'First Class Commercial',
    'FIRST CLASS HFP COMMERCIAL' => 'First Class HFP Commercial',
    'PRIORITY' => 'Priority',
    'PRIORITY COMMERCIAL' => 'Priority Commercial',
    'PRIORITY HFP COMMERCIAL' => 'Priority HFP Commercial',
    'EXPRESS' => 'Express',
    'EXPRESS COMMERCIAL' => 'Express Commercial',
    'EXPRESS SH' => 'Express SH',
    'EXPRESS SH COMMERCIAL' => 'Express SH Commercial',
    'EXPRESS HFP' => 'Express HFP',
    'EXPRESS HFP COMMERCIAL' => 'Express HFP Commercial',
    'PARCEL' => 'Parcel',
    'MEDIA' => 'Media',
    'LIBRARY' => 'Library',
    'ALL' => 'All',
    'ONLINE ' => 'Online '
  );
}

/**
 * Pseudo-constructor to set default values of a package.
 */
function _commerce_shipping_usps_new_package() {
  $package = new stdClass();

  $package->weight = 0;
  $package->price = 0;

  $package->length = 0;
  $package->width = 0;
  $package->height = 0;

  $package->length_units = 'in';
  $package->weight_units = 'lb';
  $package->qty = 1;
  $package->pkg_type = '02';

  return $package;
}

