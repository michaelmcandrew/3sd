<!DOCTYPE HTML>
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<?php print $language->language ?>" lang="<?php print $language->language ?>" dir="<?php print $language->dir ?>">
	<head>
		<?php print $head; ?>
		<title><?php print $head_title; ?></title>
		<?php print $styles; ?>
		<?php print $scripts; ?>
		<script type="text/javascript"><?php /* Needed to avoid Flash of Unstyled Content in IE */ ?> </script>
	</head>

	<body>
		<div id="header">
			<div class="center_content">
				<h1 id="logo-text"><a href="<?php print $front_page; ?>"><?php print $site_name; ?>"</a></h1>
				<p id="slogan"><?php print $site_slogan; ?></p>
			</div>
		</div>

		<div id="nav">
			<div class="center_content">
				<?php print theme('links', $primary_links, array('class' => 'links primary-links')); ?>
			</div>
			<div class="clear"></div>
		</div>

		<!--wraps the main content area and the right-col-->
		<div id="content-wrapper">
			<div class="center_content">
				<div id="main">
					<?php if (!empty($title)) : ?><h1 class="title" id="page-title"><?php print $title; ?></h1><?php endif; ?>
					<!--tabs visible when logged in-->
					<?php if (!empty($tabs)): ?><div class="tabs"><?php print $tabs; ?></div><?php endif; ?>
					<?php if (!empty($messages)): print $messages; endif; ?>
					<?php if (!empty($help)): print $help; endif; ?>

					<!--main-content-->
					<div id="content-output">
						<?php if ($hp_feature): ?>
							<div id="hp-feature">
								<?php print $hp_feature; ?>
							</div>
						<?php endif; ?>
						<?php print $content; ?>
					</div>
					<!--end of main-content-->
				</div><!--end of main-->

				<?php if ($right): ?>
					<div id="sidebar-right">
						<?php print $right; ?>
					</div>
				<?php endif; ?>
				
				<div class="clear"></div>
			</div>
		</div><!--end of content-wrapper-->

		<div id="footer_blue">
			<div class="center_content">
				<?php print $footer_blue; ?>
			</div>
		</div>

		<div id="footer_lists">
			<div class="center_content">
				<?php print $footer_lists; ?>
			</div>
			<div class="clear"></div>
		</div>				

		<?php print $closure; ?>
	</body>
</html>


