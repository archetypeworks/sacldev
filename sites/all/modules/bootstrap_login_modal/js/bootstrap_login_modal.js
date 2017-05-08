(function ($) {
  Drupal.behaviors.bootstrap_login_modal = {
      attach: function (context) {
        $("#login-modal", context).appendTo("body");
        $("#register-modal", context).appendTo("body");
		$("#pass-reset-modal", context).appendTo("body");
      }
  };
})(jQuery);