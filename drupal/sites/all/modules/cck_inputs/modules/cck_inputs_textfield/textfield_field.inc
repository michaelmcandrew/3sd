<?php
// $Id: textfield_field.inc,v 1.1.2.4 2009/10/25 20:47:36 maulwuff Exp $

/**
 * @file
 * The cck field part
 */


/**
 * Implementation of hook_field_info().
 */
function cck_inputs_textfield_field_info() {
  return array(
    'cck_inputs_textfield' => array(
      'label' => t('Input Textfield'),
      'description' => t('Shows a textfield for endusers.'),
    ),
  );
}

/**
 * Implementation of hook_theme().
 */
function cck_inputs_textfield_theme() {
  return array(
    'cck_inputs_textfield_formatter_default' => array(
      'arguments' => array('element' => NULL),
    ),
    'textfield_widget' => array(
      'arguments' => array('element' => NULL),
    ),
  );
}



/**
 * Implementation of hook_field_settings().
 */
function cck_inputs_textfield_field_settings($op, $field) {
  switch ($op) {
    case 'form':
      $form = array();

      return $form;

    case 'save':
      return array('textfield_name', 'textfield_value', 'textfield_label', 'textfield_required');

    case 'database columns':
      $columns['textfield_value'] = array('type' => 'varchar', 'length' => 255, 'not null' => FALSE, 'sortable' => TRUE, 'views' => TRUE);
      $columns['textfield_name'] = array('type' => 'varchar', 'length' => 255, 'not null' => FALSE);
      $columns['textfield_label'] = array('type' => 'varchar', 'length' => 255, 'not null' => FALSE, 'sortable' => TRUE, 'views' => TRUE);
      $columns['textfield_required'] = array('type' => 'int', 'length' => 1, 'size' => 'tiny', 'not null' => TRUE, 'default' => 0);

      return $columns;

    case 'views data':
      break;
  }
}

/**
 * Implementation of hook_field().
 */
function cck_inputs_textfield_field($op, &$node, $field, &$items, $teaser, $page) {
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
        $items[$delta]['textfield_value'] = check_plain($item['textfield_value']);
        $items[$delta]['textfield_name'] = check_plain($item['textfield_name']);
        $items[$delta]['textfield_label'] = check_plain($item['textfield_label']);
        $items[$delta]['textfield_required'] = check_plain($item['textfield_required']);
        $items[$delta]['builder'] = 'cck_inputs_textfield_builder';
      }
      break;

    case 'presave':
       if (is_array($items)) {
        foreach ($items as $delta => $item) {
          if(cck_inputs_textfield_content_is_empty($item, 'textfield_name')
              ) {
            //name is empty. use automatic fieldname
            $items[$delta]['textfield_name'] = 'textfield_name_'.$delta;
            $items[$delta]['textfield_value'] = $delta+1;
          }
        }
      }
      return $items;
  }
}

/**
 * Implementation of hook_content_is_empty().
 */
function cck_inputs_textfield_content_is_empty($item, $field) {
  if (empty($item['textfield_name']) && (string)$item['textfield_name'] !== '0') {
    return TRUE;
  }
  return FALSE;
}
