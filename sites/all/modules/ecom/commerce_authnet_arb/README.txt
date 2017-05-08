COMMERCE AUTHNET ARB
--------------------

This module provide an API to Authorize.net payment integration for ARB (Automated Recurring Billing). To allow commerce_authnet_arb module to process silentpost requests from an Authorize.net you have configure a silentpost URL in your authorize.net account to http://<yourdomain>/authnet-arb-silentpost

The module has integration with commerce (drupal.org/project/commerce), it stores subscription information in $order->data and saves silentpost requests as a commerce_payment transactions attached to the order.


CONFIGURATION
-------------

Authorize.net SDK requirements:
  - cURL PHP Extension
  - PHP 5.2+
  - An Authorize.Net Merchant Account or Test Account. You can get a free test account at http://developer.authorize.net/testaccount/.

Installation steps
  - Place Authorize.Net SDK in directory 'sites/all/libraries'. You can access the file from https://github.com/AuthorizeNet/sdk-php.
  - Rename the sdk library directory name to 'anet_php_sdk'.
  - Run `composer update` command in the 'sites/all/libraries/anet_php_sdk' directory.
  - Now enable the module from sites/all/modules list.
  - Goto 'admin/config/commerce_authnet_arb' and put the Authorize.Net auth info (API Login ID & Transaction Key).
  - Please check the sandbox option for testing payments in the development phase.
  - To allow module to process silentpost requests from an Authorize.Net you have to configure a silentpost URL in your authorize.net account to
   'http://<yourdomain>/commerce-authnet-arb-silentpost'.
  - If 'commerce_customer' module is been enabled, please enable 'Billing information' pane in checkout settings ('admin/commerce/config/ checkout').

SILENTPOST TESTING
------------------
Authorize.net will send a silentpost request after you send a subscription only in a next day. Obviously this is make a debugging and testing mutch harder. To make it easier the module suggest you to request by browser a special url:http://<yoursite>/authnet-arb-silentpost-test
When you will request it (actually with no post data) the module will emulate a silentpost request by using not real but syntetic post data in format equal to ARB silentpost.

You may see the syntetic post data in authet_arb_get_sample_silentpost_post() function. If you want to change the data you can edit the function or pass parameters you want to change by GET, like: http://<yoursite>/authnet-arb-silentpost-test/?x_response_code=3&x_response_reason_code=4
