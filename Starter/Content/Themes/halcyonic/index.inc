<!DOCTYPE HTML>
<html>
	<head><meta http-equiv="Content-Type" content="text/html; charset=windows-31j">
		<base href="http://<?= $_SERVER[HTTP_HOST]; ?>"></base>
<meta name="viewport" content="width=320; initial-scale=1.0; maximum-scale=1.0; user-scalable=0;"/>
		<title><?= empty($_SESSION[Company]) ? 'Welcome' : $_SESSION[Company]->Name->value; ?></title>
		
		<noscript>
			<link rel="stylesheet" href="/Content/Themes/halcyonic/css/skel-noscript.css" />
			<link rel="stylesheet" href="/Content/Themes/halcyonic/css/style.css" />
			<link rel="stylesheet" href="/Content/Themes/halcyonic/css/style-desktop.css" />
		</noscript>
		<!--[if lte IE 9]><link rel="stylesheet" href="/Content/Themes/halcyonic/css/ie9.css" /><![endif]-->
<link rel="stylesheet" type="text/css" href="/Content/css/main.css"></link>
<link rel="stylesheet" type="text/css" href="/Content/css/datetimepicker.css"></link>
<link rel="stylesheet" type="text/css" href="/Content/css/jquery-ui-1.10.3.custom.css"></link>
<link rel="stylesheet" type="text/css" media="print" href="/Content/Themes/halcyonic/css/printer.css" />
		<script src="/Content/js/jquery.min.js"></script>
<script src="/Content/js/jquery-ui.min.js"></script>
<script src="/Content/js/jquery-ui-timepicker-addon.js"></script>
		<script src="/Content/Themes/halcyonic/js/config.js"></script>
		<script src="/Content/Themes/halcyonic/js/skel.min.js"></script>
		<script src="/Content/Themes/halcyonic/js/skel-panels.min.js"></script>
		<!--[if lte IE 8]><script src="/Content/Themes/halcyonic/js/html5shiv.js"></script><![endif]-->
<? Section(includes); ?>
	</head>
	<body>
			<div id="header-wrapper">
				<header id="header" class="container">
					<div class="row">
						<div class="12u">
							<h1><a href="/" id="logo"><? Section(logo); ?></a></h1>
							<nav id="nav">
							<? Section(menu); ?>
							</nav>
						</div>
					</div>
<div class="row">
<div class="12u">
<? Section(todo); ?>
</div>
</div>
				</header>
				<div id="banner">
					<div class="container">
						<div class="row">
							<? Section(login); ?>
						</div>
					</div>
				</div>
			</div>
			<div id="features-wrapper">
				<div id="features">
					<div class="container">
						<div class="row">
<? Section(breadcrumbs); ?>
						</div>
					</div>
				</div>
			</div>
			<div id="content-wrapper">
				<div id="content">
					<div class="container">
						<div class="row">
							<? GetPageContent(); ?>
						</div>
					</div>
				</div>
			</div>
			<div id="footer-wrapper">
				<footer id="footer" class="container">
					<div class="row">
						
					</div>
				</footer>
			</div>
			<div id="copyright">
				&copy; Untitled. All rights reserved. | Design: <a href="http://html5up.net">HTML5 UP</a> | Images: <a href="http://fotogrph.com">fotogrph</a>
			</div>
<script>
if (history && history.pushState) {
var loc2 = document.location+'';
var loc = loc2.replace($("base").attr("href"),"");
history.pushState({path:loc},"halcyonic",loc);
$(window).bind("popstate",function(event) {
if(event.originalEvent.state) {
changePage(event.originalEvent.state.path, false);
}
});
}
</script>
	</body>
</html>