<?php

/**
 * @file
 * Provides some node attributes as ThemeKey properties.
 *
 * @author Markus Kalkbrenner | Cocomore AG
 *   @see http://drupal.org/user/124705
 *
 * @author profix898
 *   @see http://drupal.org/user/35192
 */


/**
 * Implements hook_themekey_properties().
 *
 * Provides additional properties for module ThemeKey:
 * - locale:language
 *
 * @return
 *   array of themekey properties and mapping functions
 */
function themekey_locale_themekey_properties() {
  // Attributes for properties
  $attributes = array();
  $attributes['locale:language'] = array(
    'description' => t('Locale: Language - The code of the current site language, formatted like "en" or "de". See !link to find the codes for your enabled languages',
      array('!link' => l(t('!path', array('!path' => 'admin/settings/language')), 'admin/settings/language'))),
    'validator' => 'themekey_validator_language',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );

  return array('attributes' => $attributes);
}


/**
 * Implements hook_themekey_paths().
 */
function themekey_locale_themekey_global() {
  global $language;

  $parameters = array();
  $parameters['locale:language'] = !empty($language->language) ? $language->language : NULL;

  return $parameters;
}
