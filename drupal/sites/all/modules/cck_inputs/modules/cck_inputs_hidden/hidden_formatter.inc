<?php
// $Id: hidden_formatter.inc,v 1.1.2.1 2009/07/28 18:03:09 maulwuff Exp $

/**
 * @file
 * The cck formatter part
 */

/**
 * Implementation of hook_field_formatter_info().
 */
function cck_inputs_hidden_field_formatter_info() {
  return array(
    'default' => array(
      'label' => t('Default'),
      'field types' => array('cck_inputs_hidden'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
  );
}

/**
 * Theme function for 'default' text field formatter.
 */
function theme_cck_inputs_hidden_formatter_default($element) {
#print_r($element);exit;

  $hidden = array(
    '#type' => 'hidden',
    '#name' => $element['#item']['hidden_name'],
    '#value' => $element['#item']['hidden_value'],
    //#default_value to support javascript changing

  );
  return drupal_render($hidden);

}

