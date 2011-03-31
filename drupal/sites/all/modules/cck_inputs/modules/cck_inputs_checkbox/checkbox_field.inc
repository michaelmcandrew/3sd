<?php


// $Id: checkbox_field.inc,v 1.1.2.5 2009/10/25 20:47:35 maulwuff Exp $

/**
 * @file
 * The cck field part
 */

/**
 * Implementation of hook_field_info().
 */
function cck_inputs_checkbox_field_info() {
  return array (
    'cck_inputs_checkbox' => array (
      'label' => t('Input Checkbox'),
      'description' => t('Shows a checkbox for endusers.'),
    ),
  );
}

/**
 * Implementation of hook_theme().
 */
function cck_inputs_checkbox_theme() {
  return array (
    'cck_inputs_checkbox_formatter_default' => array (
      'arguments' => array (
        'element' => NULL
      ),
    ),
    'cck_inputs_checkbox_checkbox' => array (
      'arguments' => array (
        'element' => NULL
      ),
    ),
  );
}

/**
 * Implementation of hook_field_settings().
 */
function cck_inputs_checkbox_field_settings($op, $field) {
  switch ($op) {
    case 'form' :
      $form = array ();

      return $form;

    case 'save' :
      return array (
        'checkbox_name',
        'checkbox_value',
        'checkbox_label'
      );

    case 'database columns' :
      $columns['checkbox_value'] = array (
        'type' => 'varchar',
        'length' => 255,
        'not null' => FALSE,
        'sortable' => TRUE,
        'views' => TRUE
      );
      $columns['checkbox_name'] = array (
        'type' => 'varchar',
        'length' => 255,
        'not null' => FALSE
      );
      $columns['checkbox_label'] = array (
        'type' => 'varchar',
        'length' => 255,
        'not null' => FALSE,
        'sortable' => TRUE,
        'views' => TRUE
      );

      return $columns;

    case 'views data' :
      break;
  }
}

/**
 * Implementation of hook_field().
 */
function cck_inputs_checkbox_field($op, & $node, $field, & $items, $teaser, $page) {
  switch ($op) {
    case 'validate' :
      if (is_array($items)) {
        foreach ($items as $delta => $item) {
          $error_element = isset ($item['_error_element']) ? $item['_error_element'] : '';
          if (is_array($item) && isset ($item['_error_element']))
            unset ($item['_error_element']);
        }
      }
      return $items;

    case 'sanitize' :
      foreach ($items as $delta => $item) {
        $items[$delta]['checkbox_value'] = check_plain($item['checkbox_value']);
        $items[$delta]['checkbox_name'] = check_plain($item['checkbox_name']);
        $items[$delta]['checkbox_label'] = check_plain($item['checkbox_label']);
        $items[$delta]['builder'] = 'cck_inputs_checkbox_builder';
      }

    case 'presave' :
      if (is_array($items)) {
        foreach ($items as $delta => $item) {
          if (cck_inputs_checkbox_content_is_empty($item, 'checkbox_name') &&
              !empty($item['checkbox_value']) ) {
            //empty names. use automatic fieldname and value
            $items[$delta]['checkbox_name'] = 'checkbox_name_' . $delta;
          }
        }
      }
      return $items;
  }
}

/**
 * Implementation of hook_content_is_empty().
 */
function cck_inputs_checkbox_content_is_empty($item, $field) {
  if (empty ($item['checkbox_name']) && (string) $item['checkbox_name'] !== '0') {
    return TRUE;
  }
  return FALSE;
}
