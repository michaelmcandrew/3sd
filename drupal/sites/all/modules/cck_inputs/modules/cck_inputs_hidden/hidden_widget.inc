<?php
// $Id: hidden_widget.inc,v 1.1.2.1 2009/07/28 18:03:09 maulwuff Exp $

/**
 * @file
 * The cck widget part
 */


/**
 * Implementation of hook_widget_info().
 *
 * Here we indicate that the content module will handle
 * the default value and multiple values for these widgets.
 *
 * Callbacks can be omitted if default handing is used.
 * They're included here just so this module can be used
 * as an example for custom modules that might do things
 * differently.
 */
function cck_inputs_hidden_widget_info() {
  return array(
    'hidden_widget' => array(
      'label' => t('Hidden Field'),
      'field types' => array('cck_inputs_hidden'),
      'multiple values' => CONTENT_HANDLE_CORE,
      'callbacks' => array(
      'default value' => CONTENT_CALLBACK_DEFAULT,
      ),
    ),
  );
}


/**
 * Implementation of hook_widget().
 *
 * Attach a single form element to the form. It will be built out and
 * validated in the callback(s) listed in hook_elements. We build it
 * out in the callbacks rather than here in hook_widget so it can be
 * plugged into any module that can provide it with valid
 * $field information.
 *
 * Content module will set the weight, field name and delta values
 * for each form element. This is a change from earlier CCK versions
 * where the widget managed its own multiple values.
 *
 * If there are multiple values for this field, the content module will
 * call this function as many times as needed.
 *
 * @param $form
 *   the entire form array, $form['#node'] holds node information
 * @param $form_state
 *   the form_state, $form_state['values'][$field['field_name']]
 *   holds the field's form values.
 * @param $field
 *   the field array
 * @param $items
 *   array of default values for this field
 * @param $delta
 *   the order of this item in the array of subelements (0, 1, 2, etc)
 *
 * @return
 *   the form item for a single element for this field
 */
function cck_inputs_hidden_widget(&$form, &$form_state, $field, $items, $delta = 0) {
  $element = array(
    '#type' => $field['widget']['type'],
    '#default_value' => isset($items[$delta]) ? $items[$delta] : '',
  );
  return $element;
}



/**
 * Implementation of FAPI hook_elements().
 *
 * Any FAPI callbacks needed for individual widgets can be declared here,
 * and the element will be passed to those callbacks for processing.
 *
 * Drupal will automatically theme the element using a theme with
 * the same name as the hook_elements key.
 *
 * Autocomplete_path is not used by text_widget but other widgets can use it
 * (see nodereference and userreference).
 */
function cck_inputs_hidden_elements() {
  return array(
    'hidden_widget' => array(
      '#input' => TRUE,
      '#process' => array('cck_inputs_hidden_widget_process'),
    ),
  );
}


/**
 * Process an individual element.
 *
 * Build the form element. When creating a form using FAPI #process,
 * note that $element['#value'] is already set.
 *
 * The $fields array is in $form['#field_info'][$element['#field_name']].
 */
function cck_inputs_hidden_widget_process($element, $edit, &$form_state, $form) {
#print_r($element);

  $defaults = $element['#value'];
  #$field = content_fields($element['#field_name'], $element['#type_name']);
  $field = $form['#field_info'][$element['#field_name']];
  $field_key = $element['#columns'][0];
  $delta = $element['#delta'];

  $element['hidden_value'] = array(
    '#title' => t( 'hidden field value' ),
    '#type' => 'textfield',
    '#default_value' => $defaults['hidden_value'],
    '#weight' => 4,
    '#attributes' => array('class' => 'intputhidden'),
    // The following values were set by the content module and need
    // to be passed down to the nested element.
    '#description' => $element['#description'],
    '#required' => $element['#required'],
    '#field_name' => $element['#field_name'],
    '#type_name' => $element['#type_name'],
    '#delta' => $element['#delta'],
    '#columns' => $element['#columns'],
  );

//  $element['textfield_label'] = array(
//    '#title' => t( 'textfield label' ),
//    '#type' => 'textfield',
//    '#default_value' => $defaults['textfield_label'],
//    '#weight' => 3,
//    '#attributes' => array('class' => 'intputtextfield'),
//    // The following values were set by the content module and need
//    // to be passed down to the nested element.
//    '#description' => $element['#description'],
//    '#required' => $element['#required'],
//    '#field_name' => $element['#field_name'],
//    '#type_name' => $element['#type_name'],
//    '#delta' => $element['#delta'],
//    '#columns' => $element['#columns'],
//  );

  $element['hidden_name'] = array(
    '#title' => t( 'hidden field name' ),
    '#type' => 'textfield',
    '#default_value' => $defaults['hidden_name'],
    '#weight' => 3,
    '#attributes' => array('class' => 'intputhidden'),
    // The following values were set by the content module and need
    // to be passed down to the nested element.
    '#description' => $element['#description'],
    '#required' => $element['#required'],
    '#field_name' => $element['#field_name'],
    '#type_name' => $element['#type_name'],
    '#delta' => $element['#delta'],
    '#columns' => $element['#columns'],
  );

  return $element;


}

/**
 * Implementation of hook_widget_settings().
 */
function cck_inputs_hidden_widget_settings($op, $widget) {
//nothing to do
}


/**
 * FAPI theme for an individual text elements.
 *
 * The textfield or textarea is already rendered by the
 * textfield or textarea themes and the html output
 * lives in $element['#children']. Override this theme to
 * make custom changes to the output.
 *
 * $element['#field_name'] contains the field name
 * $element['#delta]  is the position of this element in the group
 */
function theme_hidden_widget($element) {
  #print_r($element);exit;
  return $element['#children'];
  #return theme( 'form_element', $element, $element['#children'] );
}