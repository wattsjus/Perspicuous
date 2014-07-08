<?php

function Section($name) {
	require_once("Content/Sections/$name.inc");
}
function GetPageContent() {
	global $body;
	echo $body;
	unset($body);
}
function GetService($name) {
  require_once("api/$name.inc");
  return new $name();
}
function Logger($Event, $Description = '') {
$log = new Log();
$log->Event = $Event;
$log->Description = $Description;
$log->Save();
}

?>