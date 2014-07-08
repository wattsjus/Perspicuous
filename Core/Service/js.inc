<?
if($_GET['src'] == 'js' || $_GET['src'] == 'jsonp') {
header("Pragma: public");
header("Expires: 0");
header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
header("Cache-Control: public"); 
$last_modified_time = filemtime($base.$_GET['base'].'/'.$controller.'.inc'); 
$etag = md5_file($base.$_GET['base'].'/'.$controller.'.inc');
header("Last-Modified: ".gmdate("D, d M Y H:i:s", $last_modified_time)." GMT"); 
header("Etag: $etag");
if ($_SERVER['Etag'] == $etag) { 
    header("HTTP/1.1 304 Not Modified"); 
    die(); 
}
header('Content-Type: application/javascript');
$protocol = 'http';
 if ($_SERVER["HTTPS"] == "on") {$protocol .= "s";}
$methods = get_class_methods(new $controller);
?>var <?= $controller; ?>={<? $reflector = new ReflectionClass($controller);
foreach($methods as $methodName) { 
	if(!HasMethod($controller, $methodName, "WebPrivate")) {
		$parameters = $reflector->getMethod($methodName)->getParameters();
		$params = '';
		$params2 = '';
		$data = '{';
		foreach($parameters as $param) {
			$params .= $param->name.',';
			$params2 .= '"+'.$param->name.'+"/';
			$data .= "\"$param->name\":$param->name,";
		}
		$data = substr($data,0,-1).'}';
		echo "$methodName"; ?>:function(<?= $params; ?>func,extra){if(typeof extra==="undefined")extra='';<? 
		$mode = 'Get';
		if(HasMethod($controller, $methodName, "Method")) {
			$mode = GetMethod($controller, $methodName, "Method");
			$mode = $mode['Params'][0];
		}
		foreach($parameters as $param) {
			if($mode == 'Get') {
				echo "if(typeof $param->name === 'object') $param->name=JSON.stringify($param->name);";
			}
		} 
		$data = "contentType:'application/json',data:JSON.stringify($data),dataType:'json'";
		if($mode == 'Get') {
			if($params2 != '') $params2 = '/'.$params2;
				$data = '';
			} else { 
				$params2 = '';
				$data = ', '.$data;
			}
			if($methodName == $mode) $methodName = '';
			else $methodName = '/'.$methodName;
$url = '/'.$_GET['base'].'/'.$controller. $methodName.$params2;
if($_GET['src'] == 'js') {
?>jQuery.ajax("<?= $url ?>?"+extra,{type:"<?= $mode; ?>",success:func<?= $data; ?>})},<? 
} else {
?>jQuery.getJSON("<?= $url ?>?"+extra, func)},<?
}
		} 
	} ?>}<? 
die();
}
?>