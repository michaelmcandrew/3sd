<?php
// $Id: textfield_formatter.inc,v 1.1.2.2 2009/10/18 20:41:44 maulwuff Exp $

/**
 * @file
 * The cck formatter part
 */

/**
 * Implementation of hook_field_formatter_info().
 */
function cck_inputs_textfield_field_formatter_info() {
  return array(
    'default' => array(
      'label' => t('Default'),
      'field types' => array('cck_inputs_textfield'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
  );
}

/**
 * Theme function for 'default' text field formatter.
 */
function theme_cck_inputs_textfield_formatter_default($element) {
  return drupal_render(cck_inputs_textfield_builder($element));
}

