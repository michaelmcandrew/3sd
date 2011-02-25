// $Id: qamodules.js,v 1.1 2008/02/22 01:25:13 starbow Exp $

/**
 * We are activating the module, so activate all the parents it depends on.
 */
Drupal.qamodulesActivate = function (title, modules) {
  modules[title].self.attr('checked', true);
  jQuery.each(modules[title].dependencies, function(i, val) {
    Drupal.qamodulesActivate(val, modules);
  });
}

/**
 * We are deactivating the module, so deactivate all the modules that depend on it.
 */
Drupal.qamodulesDeactivate = function (title, modules) {
  modules[title].self.attr('checked', false);
  jQuery.each(modules[title].requirements, function(i, val) {
    Drupal.qamodulesDeactivate(val, modules);
  });
}

/**
 * Parse a module's Required by, or Depends on string.
 *
 * @param: str - ex: 'Required by: Event Views (disabled), Views UI (disabled)'
 * @return: array - ex: ['Event Views', 'Views UI']
 */
Drupal.qamodulesParse = function(str) {
  var result = [];
  if (str) {
    str = str.substr(str.indexOf(':')+1); // Remove 'Required by:'
    str = str.replace( /\([^)]*\)/g, ''); // Remove (stuff).
    result = jQuery.map(str.split(','), function(n,i) {
      return jQuery.trim(n);
    });
  }
  return result;
}

if( Drupal.jsEnabled ) {
  $(document).ready(function(){ 
	  var modules = [];
	  $('#system-modules input:checkbox').each( function() {
	    var $row = $(this).parents('tr:first');
	    var $title = $row.find('label:last').text();
	    var $depend = Drupal.qamodulesParse($row.find('.admin-dependencies').text());
	    var $required = Drupal.qamodulesParse($row.find('.admin-required').text());
	    modules[$title] = { self: $(this), dependencies : $depend, requirements : $required }
	  });
	  $('#system-modules input:checkbox').click( function() {
	    var title = $(this).parents('tr:first').find('label:last').text();
	    var checked = $(this).attr('checked');
	    if (checked) { 
	      Drupal.qamodulesActivate(title, modules);
	    }
	    else {
	      Drupal.qamodulesDeactivate(title, modules);    
	    }
	  });
	  $(':checkbox:disabled').attr('disabled', false); // Allow parents to be turned off.
  });
}
