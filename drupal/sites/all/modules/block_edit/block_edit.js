// $Id: block_edit.js,v 1.1.2.6 2009/05/26 21:24:02 psynaptic Exp $

$(document).ready(function() {

  var regexp = new RegExp(/block-(.+?)-(.+?)/mi);
  var checkp = new RegExp(/block-.*?-.+/mi);

  $("div.block").each(function (i) {
    var block_id = $(this).attr('id');
    if (block_id.match(checkp)) {
      var block_path = block_id.replace(regexp, '$1/$2');
      block_id = block_id.replace(regexp, '$1_$2');
      var block_link = '<div id="block-edit-link-' + block_id + '" class="block-edit-link"><a href="' + Drupal.settings.basePath + 'admin/build/block/configure/' + block_path + '?' + Drupal.settings.block_edit.destination + '">[Configure]</a></div>';
      $(this).prepend(block_link);
    }
  });

  $("div.block").mouseover(function() {
    var block_id = $(this).attr('id');
    if (block_id.match(checkp)) {
      block_id = block_id.replace(regexp, '$1_$2');
      $('div#block-edit-link-' + block_id).css('display', 'block');
    }
  });

  $("div.block").mouseout(function() {
    var block_id = $(this).attr('id');
    if (block_id.match(checkp)) {
      block_id = block_id.replace(regexp, '$1_$2');
      $('div#block-edit-link-' + block_id).css('display', 'none');
    }
  });
});
