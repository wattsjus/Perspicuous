<?
header('Content-Type: application/json');
$from_webapi = true;
$q = explode('/',rtrim($_GET["q"],'/'));
$controller = $q[0];
$base = str_replace('/Security/proxy.php','',$_SERVER['SCRIPT_FILENAME']).'/';
$base = str_replace('/redirector.php','',$base);
foreach (glob("$base/Classes/*.inc") as $filename)
{
    require_once($filename);
}
foreach($_SESSION as $key => $val) {
	if(is_object($_SESSION[$key]) && is_a($_SESSION[$key], 'DataObject')) {
		$_SESSION[$key]->ReNew();
	}
}
require_once("$base"."config.inc");
require_once($perspicuous_path.'/db.inc');
session_start();
if(function_exists('Noop')) Noop();
require_once($base.$_GET['base'].'/'.$controller.'.inc');
require_once('js.inc');
try {
$_AppCache = new TableSession();
	$method = $q[1];
	$a = array();
	foreach($q as $t) {
		$obj = json_decode($t.'');
		$a[] = empty($obj) ? $t : $obj;
	}
	unset($a[0]);
	$p = file_get_contents("php://input");
	if(!empty($p) && trim($p) != '') {
		foreach(json_decode($p) as $key => $val) {
			$a[] = $val;
		}
	}
	if(method_exists(new $controller(),$method) === true) {
		unset($a[1]);
		$offset = 2;
	} else {
		$method = ucwords($_SERVER["REQUEST_METHOD"]);
		$offset = 1;
	}
	$params = array();
	$refmethod = new ReflectionMethod($controller, $method);
	$methodParams = $refmethod->getParameters();
	foreach($a as $key => $value) {
		$name = $methodParams[$key - $offset]->name;
		$$name = $value;
	}
	unset($name);
for($i = 0; $i <= count($methodParams) - count($a); $i++) {
$a[] = '';
}
	$methods = GetMethods($controller, $method);
	$webcache = 0;
	foreach($methods as $meth) {
		$res = GetFunctionAndParams(trim($meth));
		if($res['FunctionName'] == 'Redirect') {
			$val= $res['Params'][0];
			if(is_object($val) && property_exists(get_class($val),'value')) {
				$val = $val->value;
			}
			$val2 = $res['Params'][1];
			if(is_object($val2) && property_exists(get_class($val2),'value')) {
				$val2 = $val2->value;
			}
			if(json_encode($val) == json_encode($val2)) {
				$method = $res['Params'][2];
			}
		} else if($res['FunctionName'] == 'WebPrivate') {
			$text = 'Unauthorized';
			if(!empty($res['Params'][0])) $text = $res['Params'][0];
			throw new Exception($text);
		} else if($res['FunctionName'] == 'Test' && json_encode($res['Params'][0]) != json_encode($res['Params'][1])) {
			$text = 'Unauthorized';
			if(!empty($res['Params'][2])) $text = $res['Params'][2];
			throw new Exception($text);
		} else if($res['FunctionName'] == 'ForceType') {
			$param_order = $res['Params'][0];
			$type = $res['Params'][1];
			$obj = new $type();
			foreach(json_decode($a[$param_order+$offset]) as $key => $val) {
				$obj->$key = $val;
			}
			$a[$param_order+$offset] = $obj;
		} else if($res['FunctionName'] == 'WebCache') {
			$webcache = (int)$res['Params'][0];
		} else if($res['FunctionName'] == 'DataCache') {
			global $datacache;
			$datacache = (int)$res['Params'][0];
		} else if($res[FunctionName] == 'SecurityTest') {
			if(function_exists('SecurityTest')) { 
				if(SecurityTest($res['Params'][0],$res['Params'][1]) == false) {
					$text = 'Unauthorized';
					if(!empty($res['Params'][2])) $text = $res['Params'][2];
					throw new Exception($text);
				}
			}
		}
	}
	if(!method_exists($controller,$method)) {
		throw new Exception("Method $method not implemented");
	}
	if($datacache > 0) {
		$hash = "$controller_$method_";
		foreach($a as $key => $val) {
			$hash .= "$key=$val_";
		}
		$hash = md5($hash);
$cache = $_AppCache->$hash;
	} else {
$cache = null;
}
	if($cache == null) {
		$result = call_user_func_array(array($controller,$method),$a);
		if(is_object($result) && method_exists($result, 'ToJson')) {
			$result = json_decode($result->ToJson());
		}
		if($datacache > 0) {
			$_AppCache->$hash = $result;
		}
	} else {
		$result = $cache;
	}
	$result = json_encode($result);
	if($webcache > 0) {
		header("Pragma: public");
		$ts = gmdate("D, d M Y H:i:s", time() + $webcache) . " GMT";
		header("Expires: $ts");
		header("Pragma: cache");
		header("Cache-Control: max-age=$webcache");
		header("Cache-Control: public"); 
		$etag = md5($result);
		header("Etag: $etag");
		if ($_SERVER['Etag'] == $etag) { 
		    header("HTTP/1.1 304 Not Modified"); 
		    die(); 
		}
	}
	echo $result;
} catch(Exception $ex) {
	echo '{"error":'.json_encode($ex->getMessage()).'}';
}
function HasMethod($controller, $method, $find) {
	return GetMethod($controller, $method, $find) != false;
}
function GetMethod($controller, $method, $find) {
	foreach(GetMethods($controller, $method) as $meth) {
		$parts = GetFunctionAndParams(trim($meth));
		if($parts['FunctionName'] == $find) return $parts;
	}
	return false;
}
function GetMethods($controller, $method) {
	$Reflection = new ReflectionMethod($controller, $method);
	$classRef = new ReflectionClass($controller);
	$classComment = $classRef->getDocComment();
	if(!empty($classComment) && trim($classComment) != '') {
		$val = trim(substr($classComment,3,-2));
	}
	$val .= "\n". trim(substr($Reflection->getDocComment(),3,-2));
	return explode("\n",$val);
}
?>