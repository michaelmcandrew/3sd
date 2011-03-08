<?php
// $Id: themekey-debug-messages.tpl.php,v 1.2.2.1 2010/08/16 13:35:14 mkalkbrenner Exp $

/**
 * @file
 * template to format ThemeKey Debug Messages
 */
?>
<table border="1">
  <tr><th><?php print t('ThemeKey Debug Messages'); ?></th></tr>
  <?php foreach ($messages as $message) {?>
  <tr><td><?php print $message; ?></td></tr>
  <?php } ?>
</table>
