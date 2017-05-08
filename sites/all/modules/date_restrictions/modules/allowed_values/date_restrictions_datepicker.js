(function ($) {
  Drupal.behaviors.DateRestrictionsDatePopup = {
    attach: function (context) {
      $('.date-restrictions-datepicker').each(function() {
        // Initialize DateRestrictionsAllowedValues for this popup.
        if ((this.id in Drupal.settings.datePopup) && (Drupal.settings.datePopup[this.id].settings.checkDateCallbackUrl)) {
          if (!('allowedValues' in Drupal.settings.datePopup[this.id])) {
            Drupal.settings.datePopup[this.id].allowedValues = new Array();
          }
          Drupal.settings.datePopup[this.id].settings.beforeShowDay = dateRestrictionsCheckDate;
        }
      });
    }
  };

  function dateRestrictionsCheckDate(date) {
    // Extract and normalize date parts.
    var day = ('0' + date.getDate().toString()).slice(-2);
    var month = ('0' + (date.getMonth() + 1).toString()).slice(-2)
    var year = date.getFullYear();

    // We will work internally with yyyy-mm-dd.
    date = year + '-' + month + '-' + day;

    // Index for results cache.
    var index = year + '-' + month;

    var datePopup = Drupal.settings.datePopup[this.id];
    // If we don't have the month-year dates for the requested date,
    // ask them to Drupal.
    if (!(index in datePopup.allowedValues)) {
      var callbackUrl = datePopup.settings.checkDateCallbackUrl + '/' + year + '/' + month;
        datePopup.allowedValues[index] = (function () {
        var val = null;
        $.ajax({
          'async': false,
          'global': false,
          'url': callbackUrl,
          'success': function (data) {
            val = data;
          }
        });
        return val;
      })();
    }

    // Simple array of available dates.
    if (Array.isArray(datePopup.allowedValues[index])) {
      if (datePopup.allowedValues[index].indexOf(date) == -1) {
        return [false, ''];
      }
      return [true, ''];
    }
    // Object with each day availability info.
    else {
      return datePopup.allowedValues[index][date];
    }
  }
})(jQuery);