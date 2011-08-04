<?php

/**
 * @file
 * Contains all form manipulations to required by ThemeKey.
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 *
 * @author profix898
 *   @see http://drupal.org/user/35192
 */

require_once(drupal_get_path('module', 'themekey') .'/themekey_base.inc');
require_once(drupal_get_path('module', 'themekey') .'/themekey_build.inc');


/**
 * Form builder for the rule chain.
 *
 * The form will not be validated. All changes will be saved immediately.
 * Validation will happen when the form displays the stored configuration.
 * Otherwise all the drag'n'drop stuff will not work.
 *
 * @see themekey_form_alter()
 * @see themekey_rule_chain_form_submit()
 * @see themekey_rule_chain_form_set_error()
 *
 * @ingroup forms
 */
function themekey_rule_chain_form() {
  // scan for new properties provided by other modules
  themekey_rebuild();

  $properties = variable_get('themekey_properties', array());

  if (empty($properties)) {
    drupal_goto('admin/settings/themekey/settings');
  }

  $themes = themekey_theme_options(TRUE, TRUE);
  $attributes = variable_get('themekey_attributes', array());
  $operators = array(
    '=' => '=',
    '!' => '!',
    '<' => '<',
    '<=' => '<=',
    '>' => '>',
    '>=' => '>=',
    '~' => '~',
  );

  $form = array('#tree' => TRUE);

  $items = themekey_load_rules();
  $parent_options = array_merge(array(0), array_keys($items));
  $parent_options = array_combine($parent_options, $parent_options);
  foreach ($items as $item) {
    $form['old_items'][$item['id']]['depth'] = array(
      '#type' => 'value',
      '#value' => $item['depth'],
    );

    $form['old_items'][$item['id']]['id'] = array(
      '#type' => 'hidden',
      '#value' => $item['id'],
    );

    $property = $item['property'];
    $wildcard = '';
    $static = FALSE;

    if (!in_array($property, $properties)) {
      if (!empty($attributes[$property]['static'])) {
        $static = TRUE;

        $form['old_items'][$item['id']]['property'] = array(
          '#type' => 'hidden',
          '#default_value' => $property,
          '#value' => $property,
          '#prefix' => '<span class="themekey-fadeable">' . $property . '</span>',
        );

        $form['old_items'][$item['id']]['operator'] = array(
          '#type' => 'hidden',
          '#default_value' => '=',
          '#value' => '=',
        );

        $form['old_items'][$item['id']]['value'] = array(
          '#type' => 'hidden',
          '#default_value' => 'static',
          '#value' => 'static',
        );

        $form['old_items'][$item['id']]['theme'] = array(
          '#type' => 'select',
          '#default_value' => 'default',
          '#options' => array('default' => t('Triggered')),
        );
      }
      else {
        $property = 'drupal:path:wildcard';
        $wildcard = $item['property'];
      }
    }

    if (!isset($form['old_items'][$item['id']]['property'])) {
      $form['old_items'][$item['id']]['property'] = array(
        '#type' => 'select',
        '#default_value' => $property,
        '#options' => $properties,
      );
    }

    $form['old_items'][$item['id']]['wildcard'] = array(
      '#type' => 'textfield',
      '#default_value' => $wildcard,
      '#size' => 10,
      '#maxlength' => 255,
    );

    if (!isset($form['old_items'][$item['id']]['operator'])) {
      $form['old_items'][$item['id']]['operator'] = array(
        '#type' => 'select',
        '#default_value' => $item['operator'],
        '#options' => $operators,
      );
    }

    if (!isset($form['old_items'][$item['id']]['value'])) {
      $form['old_items'][$item['id']]['value'] = array(
        '#type' => 'textfield',
        '#default_value' => $item['value'],
        '#size' => 20,
        '#maxlength' => 255,
      );
    }

    $form['old_items'][$item['id']]['parent'] = array(
      '#type' => 'select',
      '#default_value' => $item['parent'],
      '#options' => $parent_options
    );

    if (!isset($form['old_items'][$item['id']]['theme'])) {
      $form['old_items'][$item['id']]['theme'] = array(
        '#type' => 'select',
        '#default_value' => $item['theme'],
        '#options' => $themes,
      );
    }

    $form['old_items'][$item['id']]['enabled'] = array(
      '#type' => 'checkbox',
      '#default_value' => $item['enabled'],
    );

    $form['old_items'][$item['id']]['weight'] = array(
      '#type' => 'weight',
      '#delta' => 50,
      '#default_value' => $item['weight'],
    );

    $form['old_items'][$item['id']]['delete'] = array(
      '#value' => $static ? '' : l(t('delete'), 'admin/settings/themekey/properties/delete/' . $item['id']),
    );
  }

  $form['new_item']['property'] = array(
    '#type' => 'select',
    '#default_value' => !empty($_GET['property']) ? check_plain($_GET['property']) : '',
    '#options' => $properties,
  );
  $form['new_item']['wildcard'] = array(
    '#type' => 'textfield',
    '#default_value' => '',
    '#size' => 10,
    '#maxlength' => 255,
  );
  $form['new_item']['operator'] = array(
    '#type' => 'select',
    '#default_value' => '=',
    '#options' => $operators,
  );
  $form['new_item']['value'] = array(
    '#type' => 'textfield',
    '#default_value' => !empty($_GET['value']) ? check_plain($_GET['value']) : '',
    '#size' => 25,
    '#maxlength' => 255
  );
  $form['new_item']['theme'] = array(
    '#type' => 'select',
    '#default_value' => 'default',
    '#options' => $themes,
  );
  $form['new_item']['enabled'] = array(
    '#type' => 'checkbox',
    '#default_value' => TRUE,
  );

  $form['buttons']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save configuration'),
  );

  return $form;
}


/**
 * Implements hook_form_alter().
 *
 * Alters
 * @see themekey_rule_chain_form()

 * Validation will happen here when the form displays the stored configuration.
 * Otherwise all the drag'n'drop stuff will not work.
 *
 * @see themekey_rule_chain_form()
 * @see themekey_rule_chain_form_set_error()
 */
function themekey_form_alter(&$form, $form_state, $form_id) {
  if ('themekey_rule_chain_form' == $form_id && empty($form_state['post'])) {
    module_invoke_all('themekey_load_validators');

    $attributes = variable_get('themekey_attributes', array());

    if (!empty($form['old_items'])) {
      foreach ($form['old_items'] as $key_1 => $value_1) {
        if ($value_1['enabled']['#default_value']) {
          if (!themekey_check_theme_enabled($value_1['theme']['#default_value'])) {
            themekey_rule_chain_form_set_error($form, $key_1, 'theme', t('Theme is not activated'));
          }

          if (0 == drupal_strlen($value_1['value']['#default_value'])) {
            themekey_rule_chain_form_set_error($form, $key_1, 'value', t('You must enter a value'));
          }
          elseif (!empty($attributes[$value_1['property']['#default_value']]['validator'])) {
            //validate rule with custom validator
            $validator = $attributes[$value_1['property']['#default_value']]['validator'];
            if (function_exists($validator)) {
              $rule = array(
                'property' => $value_1['property']['#default_value'],
                'wildcard' => $value_1['wildcard']['#default_value'],
                'operator' => $value_1['operator']['#default_value'],
                'value' => $value_1['value']['#default_value'],
              );
              $errors = $validator($rule);
              foreach ($errors as $element => $msg) {
                themekey_rule_chain_form_set_error($form, $key_1, $element, $msg);
              }
            }
            else {
              themekey_rule_chain_form_set_error($form, $key_1, 'property', t('ThemeKey requested an unknown validator called %validator to validate property %property', array('%validator' => $validator, '%property' => $value_1['property']['#default_value'])));
            }
          }

          foreach ($form['old_items'] as $key_2 => $value_2) {
            if ($key_2 == $key_1) {
              continue;
            }

            if ($value_2['enabled']['#default_value'] &&
              !empty($value_1['value']['#default_value']) &&
              $value_2['property']['#default_value'] == $value_1['property']['#default_value'] &&
              $value_2['operator']['#default_value'] == $value_1['operator']['#default_value'] &&
              $value_2['value']['#default_value'] == $value_1['value']['#default_value'] &&
              ($value_2['parent']['#default_value'] == $value_1['parent']['#default_value'] ||
              $value_2['parent']['#default_value'] == $value_1['id']['#value'])) {

              if ('drupal:path:wildcard' != $value_2['property']['#default_value'] ||
                  ('drupal:path:wildcard' == $value_2['property']['#default_value'] && $value_2['wildcard']['#default_value'] == $value_1['wildcard']['#default_value'])) {
                // We have two identical rules with the same 'indention' in a chain.
                // This is allowed only if the first rule has children and second has none and one isn't the parent of the other
                if (!$value_2['parent']['#default_value'] == $value_1['id']['#value']) {
                  $has_childs_1 = FALSE;
                  $has_childs_2 = FALSE;

                  foreach ($form['old_items'] as $key_3 => $value_3) {
                    if ($value_3['parent']['#default_value'] == $value_1['id']['#value']) {
                      $has_childs_1 = TRUE;
                    }

                    if ($value_3['parent']['#default_value'] == $value_2['id']['#value']) {
                      $has_childs_2 = TRUE;
                    }

                    if ($has_childs_1 && $has_childs_2) {
                      break;
                    }
                  }

                  if ((($value_1['weight']['#default_value'] < $value_2['weight']['#default_value']) && $has_childs_1 && !$has_childs_2) ||
                      (($value_1['weight']['#default_value'] > $value_2['weight']['#default_value']) && !$has_childs_1 && $has_childs_2)) {
                    // no error
                    continue;
                  }
                  elseif (($value_1['weight']['#default_value'] > $value_2['weight']['#default_value']) && $has_childs_1 && !$has_childs_2) {
                    themekey_rule_chain_form_set_error($form, $key_1, 'property', t('Theme switching rule could never be reached'));
                    continue;
                  }
                  elseif (($value_1['weight']['#default_value'] < $value_2['weight']['#default_value']) && !$has_childs_1 && $has_childs_2) {
                    themekey_rule_chain_form_set_error($form, $key_1, 'property', t('Theme switching rule hides a later one'));
                    continue;
                  }
                  elseif (($value_1['weight']['#default_value'] < $value_2['weight']['#default_value']) && $has_childs_1 && $has_childs_2) {
                    themekey_rule_chain_form_set_error($form, $key_1, 'property', t('Theme switching rule should be combined with an identical one below'));
                    continue;
                  }
                  elseif (($value_1['weight']['#default_value'] > $value_2['weight']['#default_value']) && $has_childs_1 && $has_childs_2) {
                    themekey_rule_chain_form_set_error($form, $key_1, 'property', t('Theme switching rule should be combined with an identical one above'));
                    continue;
                  }
                }

                themekey_rule_chain_form_set_error($form, $key_1, 'property', t('You entered two identical theme switching rules in the chain'));
                themekey_rule_chain_form_set_error($form, $key_2, 'property', t('You entered two identical theme switching rules in the chain'));
              }
            }
          }
        }
      }
    }

    // dump all errors
    themekey_rule_chain_form_set_error($form);
  }
}


/**
 * Implements an error stack for hemekey_rule_chain_form()
 *
 * @see hemekey_rule_chain_form()
 * @see themekey_form_alter()
 *
 * @param $form
 *   drupal form as reference
 *
 * @param $rule_id
 *   the id of a ThemeKey rule
 *
 * @param $element
 *   the id of an errounous form element as string
 *
 * @param $msg
 *   error message as string
 */
function themekey_rule_chain_form_set_error(&$form, $rule_id = 0, $element = '', $msg = '') {
  static $error_stack = array();

  if (!empty($rule_id)) {
    $error_stack[$rule_id][] = array('old_items][' . $rule_id . '][' . $element, $msg);
  }
  else {
    $error_counter = 0;
    foreach ($error_stack as $rule_id => $errors) {
      $error_counter++;
      $form['old_items'][$rule_id]['id']['#prefix'] = '<span class="error">' . $error_counter . '</span>';
      foreach ($errors as $error) {
        form_set_error($error[0], $error_counter .') ' . $error[1]);
      }
    }

    if (0 < $error_counter) {
      drupal_set_message(t("Your current configuration of Theme Switching Rules contains at least one logical error. Nevertheless this configuration is stored and active."), 'warning');
    }
  }
}

/**
 * Form submission handler for themekey_rule_chain_form().
 *
 * @see themekey_rule_chain_form()
 */
function themekey_rule_chain_form_submit($form, &$form_state) {
  $max_weight = 0;
  if (!empty($form_state['values']['old_items'])) {
    foreach ($form_state['values']['old_items'] as $id => $item) {
      if ($item['weight'] > $max_weight) {
        $max_weight = $item['weight'];
      }

      $item['id'] = $id;

      if ('drupal:path:wildcard' == $item['property']) {
        $item['property'] = $item['wildcard'];
      }

      unset($item['wildcard']);

      themekey_rule_set($item);
    }
  }

  if (!empty($form_state['values']['new_item']['value']) || '0' === $form_state['values']['new_item']['value']) {
    $item = $form_state['values']['new_item'];

    $item['parent'] = 0;
    $item['weight'] = $max_weight + 1;

    if ('drupal:path:wildcard' == $item['property']) {
      $item['property'] = $item['wildcard'];
    }

    unset($item['wildcard']);

    themekey_rule_set($item);
  }

  drupal_set_message(t('The configuration options have been saved.'));

  // fast deletion of page cache (truncate)
  cache_clear_all('*', 'cache_page', TRUE);
}


/**
 * Form builder for the ThemeKey settings form.
 *
 * @ingroup forms
 */
function themekey_settings_form() {
  $form['settings'] = array(
    '#type' => 'fieldset',
    '#title' => t('General Settings'),
    '#collapsible' => FALSE,
    '#collapsed' => FALSE
  );

  $form['settings']['themekey_path_case_sensitive'] = array(
    '#type' => 'checkbox',
    '#title' => t('Property drupal:path is case sensitive'),
    '#default_value' => variable_get('themekey_path_case_sensitive', 0),
    '#description' => t('Drupal paths are case-insensitive by default. Modules like Global Redirect might change this behavior.')
  );

  $form['settings']['themekey_allthemes'] = array(
    '#type' => 'checkbox',
    '#title' => t('Provide all themes for selection'),
    '#default_value' => variable_get('themekey_allthemes', 0),
    '#description' => t('Make all installed themes available for selection, not enabled ones only.')
  );

  $form['settings']['themekey_override_custom_theme'] = array(
    '#type' => 'checkbox',
    '#title' => t('Force custom theme overriding'),
    '#default_value' => variable_get('themekey_override_custom_theme', 0),
    '#description' => t('Select this option to force ThemeKey to set the custom theme. Otherwise, ThemeKey will set the custom theme only if it\'s not already set by a different module.<br /><b>Note: If you activate this feature it turns off theme switching of some other modules like system (administration theme) or organic groups.</b>'),
  );

  $form['settings']['themekey_theme_maintain'] = array(
    '#type' => 'checkbox',
    '#title' => t('Retain the theme until a new theme is set'),
    '#default_value' => variable_get('themekey_theme_maintain', 0),
    '#description' => t('Select this option to have users who are logged in stay in the same theme until they browse to a new page with an explicit theme set.'),
  );

  $form['settings']['themekey_theme_maintain_anonymous'] = array(
    '#type' => 'checkbox',
    '#title' => t('Retain the theme until a new theme is set for anonymous users'),
    '#default_value' => variable_get('themekey_theme_maintain_anonymous', 0),
    '#description' => t('Select this option to have anonymous users stay in the same theme until they browse to a new page with an explicit theme set.
<br /><b>Warning: This feature only works correctly if you turn off page caching or use a Drupal core that supports lazy session initialization like !cdclink or !pflink.</b>',
      array('!cdclink' => l(t('!path', array('!path' => 'Cocomore Drupal Core')), 'http://drupal.cocomore.com/node/175', array('attributes' => array('target' => '_blank'))), '!pflink' => l(t('!path', array('!path' => 'Pressflow')), 'https://launchpad.net/pressflow/6.x/', array('attributes' => array('target' => '_blank'))))),
  );

  if (module_exists('forum')) {
    $form['settings']['themekey_module_forum_triggers_taxonomy_vid'] = array(
      '#type' => 'checkbox',
      '#title' => t('Forum pages trigger property taxonomy:vid'),
      '#default_value' => variable_get('themekey_module_forum_triggers_taxonomy_vid', 0),
      '#description' => t('The taxonomy:vid property is set when a single node is shown (e.g. /node/17). If this option is selected, forum pages like /forum/28 will set taxonomy:vid as well.')
    );
  }

  $form['settings']['themekey_cron_page_cache'] = array(
    '#type' => 'checkbox',
    '#title' => t('Cron cleans up page cache'),
    '#default_value' => variable_get('themekey_cron_page_cache', 1),
    '#description' => t('Select this option if ThemeKey should check rules containing time based properties when cron runs. ThemeKey will carefully clean up the page cache if necessary to provide the right theme to anonymous users automatically, e.g. a Christmas theme.'),
  );

  return system_settings_form($form);
}


/**
 * Themes themekey_rule_chain_form() and adds drag'n'drop features.
 *
 * @ingroup themeable
 */
function theme_themekey_rule_chain_form($form) {
  themekey_admin_theme_warning();

  $output = '';

  $rows = array();

  if (!empty($form['old_items'])) {
    $num_childs = array();
    $parents_disabled = array();
    $attributes = variable_get('themekey_attributes', array());

    foreach ($form['old_items'] as $key => $item) {
      if (isset($item['property'])) {
        $num_childs[$key] = 0;
        $parents_disabled[$key] = FALSE;

        if (!empty($parents_disabled[$item['parent']['#value']])) {
          $form['old_items'][$key]['enabled']['#value'] = 0;
        }

        if (!$form['old_items'][$key]['enabled']['#value']) {
          $parents_disabled[$key] = TRUE;
        }
        elseif ($item['parent']['#value']) {
          $num_childs[$item['parent']['#value']]++;
        }
      }
    }

    foreach ($form['old_items'] as $key => $item) {
      if (isset($item['property'])) {
        $row = (isset($form['old_items'][$key]['#attributes']) && is_array($form['old_items'][$key]['#attributes'])) ? $form['old_items'][$key]['#attributes'] : array();

        // Add special classes to be used for tabledrag.js.
        $form['old_items'][$key]['id']['#attributes']['class'] = 'themekey-property-id';
        $form['old_items'][$key]['parent']['#attributes']['class'] = 'themekey-property-parent';
        $form['old_items'][$key]['weight']['#attributes']['class'] = 'themekey-property-weight';
        // Add special classes to be used for themekey-properties.js.
        $form['old_items'][$key]['property']['#attributes']['class'] = 'themekey-property-property themekey-fadeable';
        $form['old_items'][$key]['wildcard']['#attributes']['class'] = 'themekey-property-wildcard themekey-fadeable';
        $form['old_items'][$key]['operator']['#attributes']['class'] = 'themekey-fadeable';
        $form['old_items'][$key]['value']['#attributes']['class'] = 'themekey-fadeable';
        $form['old_items'][$key]['enabled']['#attributes']['class'] = 'themekey-property-enabled';
        $form['old_items'][$key]['theme']['#attributes']['class'] = 'themekey-property-theme themekey-fadeable';

        // form items of type markup don't have attributes
        $form['old_items'][$key]['delete']['#value'] = str_replace('<a', '<a class="themekey-rule-delete-link"', $form['old_items'][$key]['delete']['#value']);

        if ('drupal:path:wildcard' != $item['property']['#value']) {
          $form['old_items'][$key]['wildcard']['#attributes']['style'] = 'display: none';
        }

        if ($num_childs[$key]) {
          $form['old_items'][$key]['theme']['#attributes']['style'] = 'display: none';
          // form items of type markup don't have attributes
          $form['old_items'][$key]['delete']['#value'] = str_replace('<a', '<a style="display: none"', $form['old_items'][$key]['delete']['#value']);
        }

        if (!empty($parents_disabled[$item['parent']['#value']])) {
          $form['old_items'][$key]['enabled']['#attributes']['style'] = 'display: none';
          $form['old_items'][$key]['enabled']['#default_value'] = 0;
        }

        $elements = array();

        $description = htmlentities(strip_tags($attributes[$form['old_items'][$key]['property']['#default_value']]['description']));
        $description = str_replace('\'', '', $description);

        $elements[] = array('data' =>
          theme('indentation', $form['old_items'][$key]['depth']['#value']) .
            drupal_render($form['old_items'][$key]['id']) .
            drupal_render($form['old_items'][$key]['property']) .
            drupal_render($form['old_items'][$key]['wildcard']) .
            drupal_render($form['old_items'][$key]['operator']) .
            drupal_render($form['old_items'][$key]['value']) .
            '<a name="anchor-' . $key .'" href="#anchor-' . $key .'" id="' . form_clean_id('edit-old-items-' . $key . '-value-help') . '" class="themekey-fadeable" title="' . $description . '" onClick="alert(\'' . $description . '\')">?</a>',
          'class' => 'themekey-properties-row');

        $elements[] = array('data' =>
          drupal_render($form['old_items'][$key]['theme']));

        $elements[] = array('data' =>
          drupal_render($form['old_items'][$key]['enabled']));

        $elements[] = array('data' =>
          drupal_render($form['old_items'][$key]['delete']));

        $elements[] = array('data' =>
          drupal_render($form['old_items'][$key]['parent']));

        $elements[] = array('data' =>
          drupal_render($form['old_items'][$key]['weight']));

        $page_cache = $attributes[$form['old_items'][$key]['property']['#default_value']]['page cache'];
        if ($form['old_items'][$key]['enabled']['#value']) {
          themekey_page_cache_warning($page_cache);
        }

        $elements[] = array('data' =>
          '<div id="' . form_clean_id('edit-old-items-' . $key . '-page-cache-icon') . '">' .
          theme('themekey_page_cache_icon', $page_cache) .
          '</div>');

        $row['class'] = !empty($row['class']) ? $row['class'] .' draggable' : 'draggable';
        if (!$form['old_items'][$key]['enabled']['#value']) {
          $row['class'] .= ' themekey-fade-out';
        }
        if (!$form['old_items'][$key]['parent']['#value']) {
          $row['class'] .= ' themekey-top-level';
        }
        $row['id'] = 'themekey-properties-row-' . $key;
        $row['data'] = $elements;
        $rows[] = $row;
      }
    }
  }

  if (!empty($rows)) {
    if (empty($form['pager']['#value'])) {
      drupal_add_tabledrag('themekey-properties', 'match', 'parent', 'themekey-property-parent', 'themekey-property-parent', 'themekey-property-id', TRUE);
      drupal_add_tabledrag('themekey-properties', 'order', 'sibling', 'themekey-property-weight');
    }

    $header = array(
      array('data' => t('Theme Switching Rule Chain')),
      array('data' => t('Theme')),
      array('data' => t('Enabled')),
      array('data' => t('Operation')),
      array('data' => t('Parent')),
      array('data' => t('Weight')),
      array('data' => t('Page<br />Cache')),
      );

    $output .= theme('table', $header, $rows, array('id' => 'themekey-properties'));

    foreach ($num_childs as $parent => $num) {
      $output .= '<input id="themekey-num-childs-' . $parent . '" type="hidden" value="' . $num . '"></input>';
    }
  }

  $rows = array();

  if (!empty($form['new_item'])) {
    if (isset($form['new_item']['property'])) {
      $row = (isset($form['new_item']['#attributes']) && is_array($form['new_item']['#attributes'])) ? $form['new_item']['#attributes'] : array();

      // Add special classes to be used for themekey-properties.js.
      $form['new_item']['property']['#attributes']['class'] = 'themekey-property-property';
      $form['new_item']['wildcard']['#attributes']['class'] = 'themekey-property-wildcard';
      if ('drupal:path:wildcard' != $form['new_item']['property']['#value']) {
        $form['new_item']['wildcard']['#attributes']['style'] = 'display: none';
      }

      $elements = array();
      $elements[] = t('New Rule:');
      $elements[] = array('data' =>
        drupal_render($form['new_item']['property']) .
          drupal_render($form['new_item']['wildcard']) .
          drupal_render($form['new_item']['operator']) .
          drupal_render($form['new_item']['value']),
        'class' => 'themekey-properties-row');
      $elements[] = array('data' => drupal_render($form['new_item']['theme']));
      $elements[] = array('data' => drupal_render($form['new_item']['enabled']));

      $row['data'] = $elements;
      $rows[] = $row;
    }
  }

  if (!empty($rows)) {
    $header = array(
      '',
      array('data' => t('Theme Switching Rule')),
      array('data' => t('Theme')),
      array('data' => t('Enabled')),
    );

    $output .= '<a name="themekey_new_rule"></a>';
    $output .= theme('table', $header, $rows, array('id' => 'themekey-new-item'));
  }

  $output .= drupal_render($form);

  drupal_add_css(drupal_get_path('module', 'themekey') . '/themekey_properties.css');
  drupal_add_js(drupal_get_path('module', 'themekey') . '/themekey_properties.js');

  return $output;
}


/**
 * Detects if there's an active administration theme and displays a warning
 */
function themekey_admin_theme_warning() {
  if (variable_get('admin_theme', '0')) {
    drupal_set_message(t('%admin_theme is configured as the administration theme at !link. This setting is more powerful than a corresponding ThemeKey rule.',
      array('%admin_theme' => variable_get('admin_theme', '0'), '!link' => l(t('!path', array('!path' => 'admin/settings/admin')), 'admin/settings/admin'))), 'warning');
    if (variable_get('node_admin_theme', '0')) {
      drupal_set_message(t('As configured at !link adding or editing a node will use the administration theme %admin_theme.',
        array('%admin_theme' => variable_get('admin_theme', '0'), '!link' => l(t('!path', array('!path' => 'admin/settings/admin')), 'admin/settings/admin'))), 'warning');
    }
  }
}


/**
 * Detects if page cache is active and incompatible properties are used
 * in theme switching rule chain
 */
function themekey_page_cache_warning($page_cache_support) {
  static $warnings = array();

  if (variable_get('cache', CACHE_DISABLED)) {
    // CACHE_NORMAL, CACHE_AGGRESSIVE, CACHE_EXTERNAL
    switch ($page_cache_support) {
      case THEMEKEY_PAGECACHE_UNSUPPORTED:
        if (!isset($warnings[THEMEKEY_PAGECACHE_UNSUPPORTED])) {
          drupal_set_message(t('!page_cache_icon Your site uses !page_caching, but the Theme Switching Rule Chain contains properties that do not support page caching. You have to ensure that any rule using those properties are wrapped by another rule that excludes anonymous users like "user:uid > 0".',
            array('!page_caching' => l(t('page caching'), 'admin/settings/performance'), '!page_cache_icon' => theme('themekey_page_cache_icon', $page_cache_support))), 'warning');
          $warnings[THEMEKEY_PAGECACHE_UNSUPPORTED] = TRUE;
        }
        break;

      case THEMEKEY_PAGECACHE_TIMEBASED:
        if (!isset($warnings[THEMEKEY_PAGECACHE_TIMEBASED])) {
          if (variable_get('themekey_cron_page_cache', 1)) {
            drupal_set_message(t('!page_cache_icon Your site uses !page_caching and the Theme Switching Rule Chain contains properties that require your cron to run frequently enough. During the cron run, ThemeKey deletes the page cache if necessary, depending on your Theme Switching Rules.',
              array('!page_caching' => l(t('page caching'), 'admin/settings/performance'), '!page_cache_icon' => theme('themekey_page_cache_icon', $page_cache_support))), 'warning');
          }
          else {
            drupal_set_message(t('!page_cache_icon Your site uses !page_caching and the Theme Switching Rule Chain contains properties that require "!cron_page_cache" to be enabled, but it is disabled.',
              array('!page_caching' => l(t('page caching'), 'admin/settings/performance'), '!cron_page_cache' => l(t('Cron cleans up page cache'), 'admin/settings/themekey/settings'), '!page_cache_icon' => theme('themekey_page_cache_icon', $page_cache_support))), 'error');
          }
          $warnings[THEMEKEY_PAGECACHE_TIMEBASED] = TRUE;
        }
        break;
    }
  }
}


/**
 * Menu callback -- ask for confirmation of ThemeKey rule deletion
 */
function themekey_admin_delete_rule_confirm(&$form_state, $arg, $themekey_property_id) {
  $form['themekey_property_id'] = array(
    '#type' => 'value',
    '#value' => $themekey_property_id,
  );

  $title = themekey_format_rule_as_string($themekey_property_id);

  return confirm_form($form,
    t('Are you sure you want to delete ThemeKey rule %title?', array('%title' => $title)),
    'admin/settings/themekey/properties',
    t('This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );
}


/**
 * Execute ThemeKey rule deletion
 */
function themekey_admin_delete_rule_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    themekey_rule_del($form_state['values']['themekey_property_id']);
  }

  $form_state['redirect'] = 'admin/settings/themekey/properties';
}


function theme_themekey_page_cache_icon($page_cache_support) {
  static $module_path = '', $page_cache_support_desriptions = array();

  if (empty($module_path)) {
    $module_path = drupal_get_path('module', 'themekey');
    $page_cache_support_desriptions = themekey_get_page_cache_support_desriptions();
  }

  return '<img src="/' . $module_path .'/img/page_cache_' . $page_cache_support . '.png" width="16" height="16" title="' . htmlentities(strip_tags($page_cache_support_desriptions[$page_cache_support]), ENT_COMPAT, 'UTF-8') . '"></img>';

}


function themekey_get_page_cache_support_desriptions() {
  return array(
    THEMEKEY_PAGECACHE_UNSUPPORTED => t('Unsupported! If you enable page caching on your system you have to ensure that any rule using this property is wrapped by another rule that excludes anonymous users, e.g. "user:uid > 0".'),
    THEMEKEY_PAGECACHE_SUPPORTED => t('Supported'),
    THEMEKEY_PAGECACHE_TIMEBASED => t('Supported if you enable "Cron cleans up page cache" at !link and run your cron frequently enough. During the cron run, ThemeKey deletes the page cache if necessary, depending on your Theme Switching Rules.', array('!link' => l(t('!path', array('!path' => '/admin/settings/themekey/settings')), 'admin/settings/themekey/settings'))),
  );
}