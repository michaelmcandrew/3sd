<?php

/**
 * @file
 * Provides some taxonomy stuff as ThemeKey properties.
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
 * - taxonomy:vid
 * - taxonomy:tid
 * - taxonomy:tid_and_childs
 *
 * @return
 *   array of themekey properties and mapping functions
 */
function themekey_taxonomy_themekey_properties() {
  // Attributes for properties ;)
  $attributes = array();
  $attributes['taxonomy:vid'] = array(
    'description' => t('Taxonomy: Vocabulary - The vocabulary id (vid) of a taxonomy vocabulary. See !link for your vocabularies.',
      array('!link' => l(t('!path', array('!path' => 'admin/content/taxonomy')), 'admin/content/taxonomy'))),
    'validator' => 'themekey_validator_ctype_digit',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );
  $attributes['taxonomy:tid'] = array(
    'description' => t('Taxonomy: Term - The term id (tid) of a taxonomy term.'),
    'validator' => 'themekey_validator_ctype_digit',
    'page cache' => THEMEKEY_PAGECACHE_SUPPORTED,
  );
  $attributes['taxonomy:tid_and_childs'] = array(
    'description' => t('Taxonomy: Term and its children - The term id (tid) of a taxonomy term. If set, all child terms of this term will be used too.'),
    'validator' => 'themekey_validator_ctype_digit',
    'page cache' => THEMEKEY_PAGECACHE_UNSUPPORTED,
  );

  // Mapping functions
  $maps = array();
  $maps[] = array('src'       => 'taxonomy:tid',
                  'dst'       => 'taxonomy:vid',
                  'callback'  => 'themekey_taxonomy_tid2vid');
  $maps[] = array('src'       => 'taxonomy:tid',
                  'dst'       => 'taxonomy:tid_and_childs',
                  'callback'  => 'themekey_taxonomy_tid2tid_and_parents');
  $maps[] = array('src'       => 'node:vid',
                  'dst'       => 'taxonomy:vid',
                  'callback'  => 'themekey_taxonomy_vid2vid');
  $maps[] = array('src'       => 'node:vid',
                  'dst'       => 'taxonomy:tid',
                  'callback'  => 'themekey_taxonomy_vid2tid');
  $maps[] = array('src'       => 'node:vid',
                  'dst'       => 'taxonomy:tid_and_childs',
                  'callback'  => 'themekey_taxonomy_vid2tid_and_parents');

  return array('attributes' => $attributes, 'maps' => $maps);
}


/**
 * Implements hook_themekey_paths().
 */
function themekey_taxonomy_themekey_paths() {
  $paths = array();
  $paths[] = array('path' => 'taxonomy/term/#taxonomy:tid');

  // Add support for 'forum' paths
  if (module_exists('forum') && variable_get('themekey_module_forum_triggers_taxonomy_vid', 0)) {
    $paths[] = array('path' => 'forum/#taxonomy:vid');
  }

  return $paths;
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: taxonomy:tid
 * dst: taxonomy:vid
 *
 * @param $tids
 *   array of taxonomy term ids or a single term id
 *
 * @return
 *   array of taxonomy vocabulary ids
 *   or NULL if no value could be mapped
 */
function themekey_taxonomy_tid2vid($tids) {
  $vid = array();
  // Use SQL instead taxonomy API because this code runs during hook_init() stage.
  // Using taxonomy API will require to load the node using node_load() which is not allowed in this stage.
  $tids = is_array($tids) ? $tids : array($tids);
  foreach ($tids as $tid) {
    $vid[] = db_result(db_query('SELECT vid FROM {term_data} WHERE tid = %d', $tid));
  }

  return count($vid) ? $vid : NULL;
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: node:vid
 * dst: taxonomy:vid
 *
 * @param $node_vid
 *   node version id
 *
 * @return
 *   array of taxonomy vocabulary ids
 *   or NULL if no value could be mapped
 */
function themekey_taxonomy_vid2vid($node_vid) {
  $vid = array();
  // Use SQL instead taxonomy API because this code runs during hook_init() stage.
  // Using taxonomy API will require to load the node using node_load() which is not allowed in this stage.
  $result = db_query('SELECT td.vid FROM {term_data} td INNER JOIN {term_node} tn ON td.tid = tn.tid WHERE tn.vid = %d', $node_vid);
  while ($term = db_fetch_object($result)) {
    $vid[] = $term->vid;
  }

  return count($vid) ? $vid : NULL;
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: node:vid
 * dst: taxonomy:tid
 *
 * @param $node_vid
 *   node version id
 *
 * @return
 *   array of taxonomy term ids
 *   or NULL if no value could be mapped
 */
function themekey_taxonomy_vid2tid($node_vid) {
  $tid = array();
  // Use SQL instead taxonomy API because this code runs during hook_init() stage.
  // Using taxonomy API will require to load the node using node_load() which is not allowed in this stage.
  $result = db_query('SELECT tid FROM {term_node} WHERE vid = %d', $node_vid);
  while ($term = db_fetch_object($result)) {
    $tid[] = $term->tid;
  }

  return count($tid) ? $tid : NULL;
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: node:vid
 * dst: taxonomy:tid_and_parents
 *
 * @param $node_vid
 *   node version id
 *
 * @return
 *   array of taxonomy term ids
 *   or NULL if no value could be mapped
 */
function themekey_taxonomy_vid2tid_and_parents($node_vid) {
  $node_tids = themekey_taxonomy_vid2tid($node_vid);
  if (!is_array($node_tids)) {
    return FALSE;
  }
  $tids = $node_tids;
  foreach ($node_tids as $tid) {
    $parent_terms = taxonomy_get_parents_all($tid);
    foreach ($parent_terms as $parent_term) {
      $tids[] = $parent_term->tid;
    }
  }

  return count($tids) ? array_unique($tids) : NULL;
}


/**
 * ThemeKey mapping function to set a
 * ThemeKey property's value (destination)
 * with the aid of another ThemeKey property (source).
 *
 * src: taxonomy:tid
 * dst: taxonomy:tid_and_parents
 *
 * @param $tids
 *   array of taxonomy term ids or a single term id
 *
 * @return
 *   array of taxonomy term ids
 *   or NULL if no value could be mapped
 */
function themekey_taxonomy_tid2tid_and_parents($tids) {
  $tids = is_array($tids) ? $tids : array($tids);
  foreach ($tids as $tid) {
    // note that taxonomy_get_parents_all() returns the term itself
    $parent_terms = taxonomy_get_parents_all($tid);
    $parents = array();
    foreach ($parent_terms as $parent_term) {
      $parents[] = $parent_term->tid;
    }
  }

  return count($parents) ? array_unique($parents) : NULL;
}
