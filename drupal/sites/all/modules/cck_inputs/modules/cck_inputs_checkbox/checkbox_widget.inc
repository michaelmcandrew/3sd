<?php
// $Id: checkbox_widget.inc,v 1.1.2.5 2009/12/27 17:00:33 maulwuff Exp $

/**
 * @file
 * The cck widget part
 */


/**
 * Implementation of hook_widget_info().
 * define available widgets.
 * cck_inputs_checkbox needed as prefix for form_alter
 */
function cck_inputs_checkbox_widget_info() {
  return array(
    'cck_inputs_checkbox_checkbox' => array(
      'label' => t('Checkbox'),
      'field types' => array('cck_inputs_checkbox'),
      'multiple values' => CONTENT_HANDLE_CORE,
      'callbacks' => array(
        'default value' => CONTENT_CALLBACK_CUSTOM,
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
function cck_inputs_checkbox_widget(&$form, &$form_state, $field, $items, $delta = 0) {
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
 *
 * processors for widget
 */
function cck_inputs_checkbox_elements() {
  return array(
    'cck_inputs_checkbox_checkbox' => array(
      '#input' => TRUE,
      '#process' => array('cck_inputs_checkbox_checkbox_process'),
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
function cck_inputs_checkbox_checkbox_process($element, $edit, &$form_state, $form) {
#kpr($element);exit;

  $defaults = $element['#value'];
  $field = $form['#field_info'][$element['#field_name']];

  #$element['#after_build'][] = 'cck_inputs_checkbox_checkbox_process_afterbuild';
  $element['checkbox_value'] = array(
    '#title' => t( 'Checkbox value' ),
    '#type' => 'textfield',
    '#default_value' => $defaults['checkbox_value'],
    '#weight' => 4,
    '#attributes' => array('class' => 'mytext'),
    // The following values were set by the content module and need
    // to be passed down to the nested element.
    '#description' => $element['#description'],
    '#required' => $element['#required'],
    '#field_name' => $element['#field_name'],
    '#type_name' => $element['#type_name'],
    '#delta' => $element['#delta'],
    '#columns' => $element['#columns'],
  );

  $element['checkbox_label'] = array(
    '#title' => t( 'Checkbox label' ),
    '#type' => 'textfield',
    '#default_value' => $defaults['checkbox_label'],
    '#weight' => 2,
    '#attributes' => array('class' => 'mytext'),
    // The following values were set by the content module and need
    // to be passed down to the nested element.
    '#description' => $element['#description'],
    '#required' => $element['#required'],
    '#field_name' => $element['#field_name'],
    '#type_name' => $element['#type_name'],
    '#delta' => $element['#delta'],
    '#columns' => $element['#columns'],
  );

  $element['checkbox_name'] = array(
    '#title' => t( 'Checkbox name' ),
    '#type' => 'textfield',
    '#default_value' => $defaults['checkbox_name'],
    '#weight' => 3,
    '#attributes' => array('class' => 'mytext'),
    // The following values were set by the content module and need
    // to be passed down to the nested element.
    '#description' => t('Leave this field empty for automatic naming').'<br>'.$element['#description'],
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
function cck_inputs_checkbox_widget_settings($op, $widget) {
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
function theme_cck_inputs_checkbox_checkbox($element) {
  return '<fieldset class=" collapsible collapsed">' .
      '     <legend>'.t($element['#title']).' '.($element['#delta']+1).'</legend>'
     .$element['#children'].
          '</fieldset>';
}


/**
 * hook_default_value
 * To get this hook actually called, make sure to set   'default value' =>
 * CONTENT_CALLBACK_CUSTOM on hook_widget_info.
 * returns $items
 */
function cck_inputs_checkbox_default_value(&$form, &$form_state, $field, $delta){
  $items = array();

  $field_name = $field['field_name'];
#kpr($form);exit;
  switch ($field['multiple']) {
    case 0:
      $max = 1;
      break;
    case 1:
      $max =  isset($form_state['item_count'][$field_name]) ? $form_state['item_count'][$field_name] : 1;
      break;
    default:
      $max = $field['multiple'];
      break;
  }

  for ($delta = 0; $delta < $max; $delta++) {
    $items[$delta] = array(
      'checkbox_label' => 'Seat '.($delta+1),
      'checkbox_name' => 'checkbox_name_'.($delta+1),
      'checkbox_value' => ($delta+1),
     );
  }
  return $items;
}