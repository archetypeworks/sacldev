<?php

$plugin = array(
  // Default ctools properties.
  'title' => t('USPS'),
  'description' => t('USPS Plugin for Commerce Shipping.'),
  'handler' => array(
    'class' => 'CommerceShippingUSPS',
    'parent' => 'quote_base'
  ),
  // Special Commerce Shipping properties.
  // Display title, used for many frontend displays. Default value is the
  // title property.
  'display_title' => t('USPS shipping method'),
  // The shipping label for select form, default value is display_title.
  'shipping_label' => t('USPS'),
  // Boolean indicator indicating if a rule and action should be created.
  'create_rule' => TRUE,
  // Default settings as defined through the settings form on the rules action.
  'settings' => array(
    'shipping_price' => 42,
  ),
  // Define a price component that will be used for this shipping method.
  'price_component' => array(
    'title' => t('Shipping costs'),
    'display_title' => t('Shipping costs'),
    'weight' => -40,
  ),
);
