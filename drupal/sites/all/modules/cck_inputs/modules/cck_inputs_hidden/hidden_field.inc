<?php
// $Id: hidden_field.inc,v 1.1.2.1 2009/07/28 18:03:09 maulwuff Exp $

/**
 * @file
 * The cck field part
 */


/**
 * Implementation of hook_field_info().
 */
function cck_inputs_hidden_field_info() {
  return array(
    'cck_inputs_hidden' => array(
      'label' => t('Input Hidden Field'),
      'description' => t('Creates a hidden field for forms.'),
    ),
  );
}

/**
 * Implementation of hook_theme().
 */
function cck_inputs_hidden_theme() {
  return array(
    'cck_inputs_hidden_formatter_default' => array(
      'arguments' => array('element' => NULL),
    ),
    'hidden_widget' => array(
      'arguments' => array('element' => NULL),
    ),
  );
}



/**
 * Implementation of hook_field_settings().
 */
function cck_inputs_hidden_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      $form = array();

      return $form;

    case 'save':
      return array('hidden_name', 'hidden_value');

    case 'database columns':
      $columns['hidden_value'] = array('type' => 'varchar', 'length' => 255, 'not null' => FALSE, 'sortable' => FALSE, 'views' => FALSE);
      $columns['hidden_name'] = array('type' => 'varchar', 'length' => 255, 'not null' => FALSE);

      return $columns;

    case 'views data':
      break;
  }
}

/**
 * Implementation of hook_field().
 */
function cck_inputs_hidden_field($op, &$node, $field, &$items, $teaser, $page) {
  switch ($op) {
    case 'validate':
      if (is_array($items)) {
        foreach ($items as $delta => $item) {
          $error_element = isset($item['_error_element']) ? $item['_error_element'] : '';
          if (is_array($item) && isset($item['_error_element'])) unset($item['_error_element']);
        }
      }
      return $items;

    case 'sanitize':
      foreach ($items as $delta => $item) {
        $items[$delta]['hidden_value'] = check_plain($item['hidden_value']);
        $items[$delta]['hidden_name'] = check_plain($item['hidden_name']);

      }
  }
}

/**
 * Implementation of hook_content_is_empty().
 */
function cck_inputs_hidden_content_is_empty($item, $field) {
  if (empty($item['hidden_value']) && (string)$item['hidden_value'] !== '0') {
    return TRUE;
  }
  return FALSE;
}
