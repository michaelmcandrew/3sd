<?php
// $Id: checkbox_formatter.inc,v 1.1.2.3 2009/06/02 20:31:23 maulwuff Exp $

/**
 * @file
 * The cck formatter part
 */

/**
 * Implementation of hook_field_formatter_info().
 */
function cck_inputs_checkbox_field_formatter_info() {
  return array(
    'default' => array(
      'label' => t('Default'),
      'field types' => array('cck_inputs_checkbox'),
      'multiple values' => CONTENT_HANDLE_CORE,
    ),
  );
}

/**
 * Theme function for 'default' text field formatter.
 */
function theme_cck_inputs_checkbox_formatter_default($element) {
  return drupal_render(cck_inputs_checkbox_builder($element));
}


