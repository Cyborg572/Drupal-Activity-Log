Drupal.behaviors.activityLogAdmin = function (context) {
  // Make sure we can run context.find().
  var ctxt = $(context);
  var handle = function() {
    var val = ctxt.find('input:radio[name="settings[grouping][group_method]"]:checked').val();
    if (val == 'target_action') {
      ctxt.find('#edit-settings-grouping-group-interval-wrapper').hide();
      ctxt.find('#edit-settings-grouping-group-max-wrapper').hide();
      ctxt.find('#edit-settings-grouping-group-summary-wrapper').hide();
      ctxt.find('#edit-settings-grouping-collapse-method-wrapper').hide();
      ctxt.find('#edit-settings-grouping-group-template-wrapper').show();
      ctxt.find('#edit-settings-grouping-translation-templates-fieldset').show();
      ctxt.find('#activity-log-admin-description').hide();
    }
    else if (val == 'action' || val == 'user_action') {
      ctxt.find('#edit-settings-grouping-group-interval-wrapper').show();
      ctxt.find('#edit-settings-grouping-group-max-wrapper').show();
      ctxt.find('#edit-settings-grouping-group-summary-wrapper').show();
      ctxt.find('#edit-settings-grouping-collapse-method-wrapper').show();
      ctxt.find('#edit-settings-grouping-group-template-wrapper').show();
      ctxt.find('#edit-settings-grouping-translation-templates-fieldset').show();
      ctxt.find('#activity-log-admin-description').show();
    }
    else if (val == 'none') {
      ctxt.find('#edit-settings-grouping-group-interval-wrapper').hide();
      ctxt.find('#edit-settings-grouping-group-max-wrapper').hide();
      ctxt.find('#edit-settings-grouping-group-summary-wrapper').hide();
      ctxt.find('#edit-settings-grouping-collapse-method-wrapper').hide();
      ctxt.find('#edit-settings-grouping-group-template-wrapper').hide();
      ctxt.find('#edit-settings-grouping-translation-templates-fieldset').hide();
    }
  };
  handle();
  ctxt.find('input:radio[name="settings[grouping][group_method]"]').change(handle);
}
