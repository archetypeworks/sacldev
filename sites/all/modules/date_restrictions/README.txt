Description
===========

Restricts allowed values in date fields, and datepicker popup widget.

Installation
============

As usual.

Configuration and Usage
=======================

 * Create or edit a date field.
 * Configure under "More settings and values" > "Restrictions".

Code example
============

$date_format = date_limit_format(variable_get('date_format_short'), array('year', 'month', 'day'));
$form['date'] = array(
  '#type' => 'date_popup',
  '#flavour' => 'inline', // Requires date_datepicker_inline module.
  '#date_format' => $date_format,
  '#default_value' => $date->format(DATE_FORMAT_DATE),
  '#restrictions' => array(
    'min' => array(
      'type' => 'date',
      'date' => $date_range[0]['value'],
    ),
    'max' => array(
      'type' => 'interval', // Requires interval module.
      'interval' => array('interval' => 3, 'period' => 'months'),
    ),
    'allowed_values' => array(
      'type' => 'weekdays', // Requires weekdays module.
      'weekdays' => array('SA', 'SU'),
    ),
  ),
);

