<?php

/**
 * @file
 * Handle the 'profile2 view' override task.
 *
 * This plugin overrides %profile-type/%profile2_by_uid and reroutes it to the page manager, where
 * a list of tasks can be used to service this request based upon criteria
 * supplied by access plugins.
 */

/**
 * Specialized implementation of hook_page_manager_task_tasks(). See api-task.html for
 * more information.
 */
function profile2_panels_profile2_view_page_manager_tasks() {
  return array(
    // This is a 'page' task and will fall under the page admin UI
    'task type' => 'page',
    'title' => t('Profile2 template'),
    'admin title' => t('Profile2 template'),
    'admin description' => t('When enabled, this overrides the default Drupal behavior for displaying profile2s at <em>%profile-type/%profile2_by_uid</em>. If you add variants, you may use selection criteria such as profile2 type or language or user access to provide different views of profile2s. If no variant is selected, the default Drupal profile2 view will be used. This page only affects profile2s viewed as pages, it will not affect profile2s viewed in lists or at other locations. Also please note that if you are using pathauto, aliases may make a profile2 to be somewhere else, but as far as Drupal is concerned, they are still at %profile-type/%profile2_by_uid.'),
    'admin path' => '%profile-type/%profile2_by_uid',
    // Menu hooks so that we can alter the %profile-type/%profile2_by_uid menu entry to point to us.
    'hook menu alter' => 'profile2_panels_profile2_view_menu_alter',
    // This is task uses 'context' handlers and must implement these to give the
    // handler data it needs.
    'handler type' => 'context',
    'get arguments' => 'profile2_panels_profile2_view_get_arguments',
    'get context placeholders' => 'profile2_panels_profile2_view_get_contexts',
    // Allow this to be enabled or disabled:
    'disabled' => variable_get('profile2_panels_profile2_view_disabled', TRUE),
    'enable callback' => 'profile2_panels_profile2_view_enable',
    'access callback' => 'profile2_panels_profile2_view_access_check',
  );
}

/**
 * Callback defined by profile2_panels_view_page_manager_tasks().
 *
 * Alter the profile2 view input so that profile2 view comes to us rather than the
 * normal profile2 view process.
 */
function profile2_panels_profile2_view_menu_alter(&$items, $task) {
  if (variable_get('profile2_panels_profile2_view_disabled', TRUE)) {
    return;
  }

  // Override the profile2 view handler for our purpose.
  foreach (profile2_get_types() as $type_name => $type) {
    if (!empty($type->data['use_page'])) {
      $path = profile2_page_get_base_path($type);

      $callback = $items[$path . '/%profile2_by_uid']['page callback'];
      if ($callback == 'profile2_page_view' || variable_get('page_manager_override_anyway', FALSE)) {
        $items[$path . '/%profile2_by_uid']['page callback'] = 'profile2_panels_profile2_view_page';
        $items[$path . '/%profile2_by_uid']['file path'] = $task['path'];
        $items[$path . '/%profile2_by_uid']['file'] = $task['file'];
      }
    }
  }
}

/**
 * Entry point for our overridden profile2 view.
 *
 * This function asks its assigned handlers who, if anyone, would like
 * to run with it. If no one does, it passes through to Profile2's page view,
 * which is profile2_page_view().
 */
function profile2_panels_profile2_view_page($profile2) {
  // Save empty profile's. Not clean, but we need a ID later.
  if (isset($profile2->is_new) && empty($profile2->pid)) {
    $profile2->save();
    if (empty($profile2->pid)) {
      drupal_set_message("Could not load profile2", 'error');
      return "";
    }
  }

  // Prep the profile2 to be displayed so all of the regular hooks are triggered.
  // Also save the output for later, in case it is needed.
  module_load_include('inc', 'profile2_page');
  $default_output = profile2_page_view($profile2);

  // Load my task plugin
  $task = page_manager_get_task('profile2_view');

  // Load the profile2 into a context.
  ctools_include('context');
  ctools_include('context-task-handler');

  // Load all contexts.
  $contexts = ctools_context_handler_get_task_contexts($task, '', array($profile2));

  // Build the full output using the configured CTools plugin.
  $output = ctools_context_handler_render($task, '', $contexts, array($profile2->pid));
  if ($output !== FALSE) {
    return $output;
  }

  // Try loading an override plugin.
  foreach (module_implements('page_manager_override') as $module) {
    $call = $module . '_page_manager_override';
    if (($rc = $call('profile2_page_view')) && function_exists($rc)) {
      return $rc($profile2);
    }
  }

  // Otherwise, fall back to the default output generated by profile2_page_view().
  return $default_output;
}

/**
 * Callback to get arguments provided by this task handler.
 *
 * Since this is the profile2 view and there is no UI on the arguments, we
 * create dummy arguments that contain the needed data.
 */
function profile2_panels_profile2_view_get_arguments($task, $subtask_id) {
  return array(
    array(
      'keyword' => 'profile2',
      'identifier' => t('Profile2 being viewed'),
      'id' => 1,
      'name' => 'entity_id:profile2',
      'settings' => array(),
    ),
  );
}

/**
 * Callback to get context placeholders provided by this handler.
 */
function profile2_panels_profile2_view_get_contexts($task, $subtask_id) {
  return ctools_context_get_placeholders_from_argument(profile2_panels_profile2_view_get_arguments($task, $subtask_id));
}

/**
 * Callback to enable/disable the page from the UI.
 */
function profile2_panels_profile2_view_enable($cache, $status) {
  variable_set('profile2_panels_profile2_view_disabled', $status);

  // Set a global flag so that the menu routine knows it needs
  // to set a message if enabling cannot be done.
  if (!$status) {
    $GLOBALS['page_manager_enabling_profile2_view'] = TRUE;
  }
}

/**
 * Callback to determine if a page is accessible.
 *
 * @param $task
 *   The task plugin.
 * @param $subtask_id
 *   The subtask id
 * @param $contexts
 *   The contexts loaded for the task.
 * @return
 *   TRUE if the current user can access the page.
 */
function profile2_panels_profile2_view_access_check($task, $subtask_id, $contexts) {
  $context = reset($contexts);
  return profile2_access('view', $context->data);
}
