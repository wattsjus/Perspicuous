<?php
//xdebug_start_code_coverage();
ini_set('display_errors','off');
require_once('globalFunctions.inc');
$debugging = true;
if($debugging) ob_start();
require_once('dbonly.inc');
$self = str_replace('/redirector.php', '', $_SERVER[PHP_SELF]);
$file = str_replace($self, '', $_SERVER[REQUEST_URI]);
if(strrpos($file,'?') > -1) {
$file = substr($file, 0, strrpos($file,'?'));
}
$ext = substr($file, strrpos($file,'.'));
if($ext == $file) { 
	$ext = '';
} else {
	$ext = substr($ext,1);
}
global $sb;
$sb = substr($_SERVER[HTTP_HOST], 0, strpos($_SERVER[HTTP_HOST],'.'));
if($sb == substr($_SERVER[HTTP_HOST], 0, strrpos($_SERVER[HTTP_HOST],'.'))) {
	$sb = '';
}
session_start();
if($sb != '') {
$result = Company::Where(Company::SubDomain()->Equals($sb))->Select();
if(count($result) > 0) {
$_SESSION[Company] = $result[0]->Company;
}
}
if(substr($file,0,4)=='/api') {
	if(strpos($file,'?') > 0) {
		$file = substr($file,0,strpos($file,'?'));
	}
	$_GET['base'] = 'api';
	$_GET['q'] = str_replace('api/','',substr($file,1));
	$_GET['p'] = 'Service/webapi.inc';
	require_once('Security/proxy.inc');
	if($debugging) ob_flush();
		die();
} else if(substr($file,0,16) == '/Content/Modules' || ((empty($ext) || $ext == '') && strpos($file,'Content') > -1)) {
	header('HTTP/10 404 Not Found');
	if($debugging) ob_flush();
	die();
}
$disable_view = false;
if(strpos($file,'/Content/') > -1) {
	header('Content-Type: '.ContentType($ext));
	if(file_exists(substr($file,1))) {
		GetCachedPage(substr($file,1));
	} else if(file_exists("../../Perspicuous$file")) {
		GetCachedPage("../../Perspicuous$file");
	} else {
		header('HTTP/1.0 404 Not Found');
	}
	if($debugging) ob_flush();
	die();
}
foreach($_SESSION as $key => $value) {
	if(is_a($_SESSION[$key],'DataObject')) {
		$_SESSION[$key]->ReNew();
	}
}
if(!empty($_SESSION[Company])) {
Config::$Theme = $_SESSION[Company]->Theme->value;
Config::$DateFormat = $_SESSION[Company]->DateFormat->value;
Config::$TimeFormat = $_SESSION[Company]->TimeFormat->value;
}
if(strrpos($file,'?') > -1) {
	$file = substr($file,0,strrpos($file,'?'));
}
$file = substr($file,1);
if($file == '') $file = 'index.php';
if(substr($file,-1) == '/') $file = substr($file,0,-1);
if (is_file($file.'.php')) {
	$file.='.php';
}
if($file == 'scp_globals.js') { ob_clean();require_once('scp_globals.inc'); die(); }
if(!empty($_SESSION[User]) && $_SESSION[User]->UserId->value == $_COOKIE[UserId]) {
if($_SESSION[User]->Expired->value == 'Y') {
$file = 'Private/settings/profile.php';
} else {
	if(!empty($sb) && is_file("Private/Clients/$sb/$file")) {
		$file = "Private/Clients/$sb/$file";
	}
	else if(!empty($sb) && is_file("Private/Clients/$sb/$file.php")) {
		$file = "Private/Clients/$sb/$file.php";
	}
	else if(!empty($sb) && is_file("Private/Clients/$sb/$file/index.php")) {
		$file = "Private/Clients/$sb/$file/index.php";
	} else {
		if(is_file("Private/Clients/Default/$file")) {
			$file = "Private/Clients/Default/$file";
		} else if(is_file("Private/Clients/Default/$file.php")) {
			$file = "Private/Clients/Default/$file.php";
		} else if(is_file("Private/Clients/Default/$file/index.php")) {
			$file = "Private/Clients/Default/$file/index.php";
		} else if(is_file("Private/$file")) {
			$file = "Private/$file";
		} else if(is_file("Private/$file.php")) {
			$file = "Private/$file.php";
		} else if(is_file("Private/$file/index.php")) {
			$file = "Private/$file/index.php";
		}
	}
}
} else if(!empty($sb) && is_file("Public/Clients/$sb/$file")) {
	$file = "Public/Clients/$sb/$file";
} else if(!empty($sb) && is_file("Public/Clients/$sb/$file.php")) {
	$file = "Public/Clients/$sb/$file.php";
} else if(!empty($sb) && is_file("Public/Clients/$sb/$file/index.php")) {
	$file = "Public/Clients/$sb/$file/index.php";
} else if(!empty($sb) && is_file("Public/Clients/Default/$file")) {
	$file = "Public/Clients/Default/$file";
} else if(!empty($sb) && is_file("Public/Clients/Default/$file.php")) {
	$file = "Public/Clients/Default/$file.php";
} else if(!empty($sb) && is_file("Public/Clients/Default/$file/index.php")) {
	$file = "Public/Clients/Default/$file/index.php";
} else {
	if(is_file("Public/$file")) {
		$file = "Public/$file";
	} else if(is_file("Public/$file.php")) {
		$file = "Public/$file.php";
	} else if(is_file("Public/$file/index.php")) {
		$file = "Public/$file/index.php";
	}
}
if(file_exists($file.'/index.php') && is_file($file.'/index.php')) {
	$file .= '/index.php';
}

if(!file_exists($file)) {
$file = 'Public/index2.php';
}
try {
require_once($file);
} catch(Exception $ex) {
echo $ex->getMessage();
}
if(empty($_GET[theme])) require_once('footer.inc');
if($disable_view) {
	if($debugging) ob_clean();
} else {
if(!empty(Config::$Theme) && Config::$Theme != '' && empty($_GET[theme])) {
global $body;
 $body = ob_get_contents();
ob_clean();
require_once('Content/Themes/'.Config::$Theme.'/index.inc');
} else {
	if($debugging) ob_flush();
}
}
function GetCachedPHPPage($page, $webcache = 3000) {
	$last_modified_time = filemtime($page); 
	$etag = md5_file($page);
	header("Pragma: public");
	$ts = gmdate("D, d M Y H:i:s", time() + $webcache) . " GMT";
	header("Expires: $ts");
	header("Pragma: cache");
	header("Cache-Control: private, max-age=$webcache");
	header("Last-Modified: ".gmdate("D, d M Y H:i:s", $last_modified_time)." GMT"); 
	header("Etag: $etag");
	if (@strtotime($_SERVER['HTTP_IF_MODIFIED_SINCE']) == $last_modified_time || 
	    @trim($_SERVER['HTTP_IF_NONE_MATCH']) == $etag) { 
	    header("HTTP/1.1 304 Not Modified"); 
	    die(); 
	} else {
		require_once($page);
	}
}
function GetCachedPage($page, $webcache = 300) {
	$last_modified_time = filemtime($page); 
	$etag = md5_file($page);
	header("Pragma: public");
	$ts = gmdate("D, d M Y H:i:s", time() + $webcache) . " GMT";
	header("Expires: $ts");
	header("Pragma: cache");
	header("Cache-Control: private, max-age=$webcache");
	header("Last-Modified: ".gmdate("D, d M Y H:i:s", $last_modified_time)." GMT"); 
	header("Etag: $etag");
	if (@strtotime($_SERVER['HTTP_IF_MODIFIED_SINCE']) == $last_modified_time || 
	    @trim($_SERVER['HTTP_IF_NONE_MATCH']) == $etag) { 
	    header("HTTP/1.1 304 Not Modified"); 
	    die(); 
	} else {
		echo file_get_contents($page);
	}
}
function ContentType($ext) {
	$mime_types = array("323" => "text/h323",
		"acx" => "application/internet-property-stream",
		"ai" => "application/postscript",
		"aif" => "audio/x-aiff",
		"aifc" => "audio/x-aiff",
		"aiff" => "audio/x-aiff",
		"asf" => "video/x-ms-asf",
		"asr" => "video/x-ms-asf",
		"asx" => "video/x-ms-asf",
		"au" => "audio/basic",
		"avi" => "video/x-msvideo",
		"axs" => "application/olescript",
		"bas" => "text/plain",
		"bcpio" => "application/x-bcpio",
		"bin" => "application/octet-stream",
		"bmp" => "image/bmp",
		"c" => "text/plain",
		"cat" => "application/vnd.ms-pkiseccat",
		"cdf" => "application/x-cdf",
		"cer" => "application/x-x509-ca-cert",
		"class" => "application/octet-stream",
		"clp" => "application/x-msclip",
		"cmx" => "image/x-cmx",
		"cod" => "image/cis-cod",
		"cpio" => "application/x-cpio",
		"crd" => "application/x-mscardfile",
		"crl" => "application/pkix-crl",
		"crt" => "application/x-x509-ca-cert",
		"csh" => "application/x-csh",
		"css" => "text/css",
		"dcr" => "application/x-director",
		"der" => "application/x-x509-ca-cert",
		"dir" => "application/x-director",
		"dll" => "application/x-msdownload",
		"dms" => "application/octet-stream",
		"doc" => "application/msword",
		"dot" => "application/msword",
		"dvi" => "application/x-dvi",
		"dxr" => "application/x-director",
		"eps" => "application/postscript",
		"etx" => "text/x-setext",
		"evy" => "application/envoy",
		"exe" => "application/octet-stream",
		"fif" => "application/fractals",
		"flr" => "x-world/x-vrml",
		"gif" => "image/gif",
		"gtar" => "application/x-gtar",
		"gz" => "application/x-gzip",
		"h" => "text/plain",
		"hdf" => "application/x-hdf",
		"hlp" => "application/winhlp",
		"hqx" => "application/mac-binhex40",
		"hta" => "application/hta",
		"htc" => "text/x-component",
		"htm" => "text/html",
		"html" => "text/html",
		"htt" => "text/webviewhtml",
		"ico" => "image/x-icon",
		"ief" => "image/ief",
		"iii" => "application/x-iphone",
		"ins" => "application/x-internet-signup",
		"isp" => "application/x-internet-signup",
		"jfif" => "image/pipeg",
		"jpe" => "image/jpeg",
		"jpeg" => "image/jpeg",
		"jpg" => "image/jpeg",
		"js" => "application/x-javascript",
		"latex" => "application/x-latex",
		"lha" => "application/octet-stream",
		"lsf" => "video/x-la-asf",
		"lsx" => "video/x-la-asf",
		"lzh" => "application/octet-stream",
		"m13" => "application/x-msmediaview",
		"m14" => "application/x-msmediaview",
		"m3u" => "audio/x-mpegurl",
		"man" => "application/x-troff-man",
		"mdb" => "application/x-msaccess",
		"me" => "application/x-troff-me",
		"mht" => "message/rfc822",
		"mhtml" => "message/rfc822",
		"mid" => "audio/mid",
		"mny" => "application/x-msmoney",
		"mov" => "video/quicktime",
		"movie" => "video/x-sgi-movie",
		"mp2" => "video/mpeg",
		"mp3" => "audio/mpeg",
		"mpa" => "video/mpeg",
		"mpe" => "video/mpeg",
		"mpeg" => "video/mpeg",
		"mpg" => "video/mpeg",
		"mpp" => "application/vnd.ms-project",
		"mpv2" => "video/mpeg",
		"ms" => "application/x-troff-ms",
		"mvb" => "application/x-msmediaview",
		"nws" => "message/rfc822",
		"oda" => "application/oda",
		"p10" => "application/pkcs10",
		"p12" => "application/x-pkcs12",
		"p7b" => "application/x-pkcs7-certificates",
		"p7c" => "application/x-pkcs7-mime",
		"p7m" => "application/x-pkcs7-mime",
		"p7r" => "application/x-pkcs7-certreqresp",
		"p7s" => "application/x-pkcs7-signature",
		"pbm" => "image/x-portable-bitmap",
		"pdf" => "application/pdf",
		"pfx" => "application/x-pkcs12",
		"pgm" => "image/x-portable-graymap",
		"pko" => "application/ynd.ms-pkipko",
		"pma" => "application/x-perfmon",
		"pmc" => "application/x-perfmon",
		"pml" => "application/x-perfmon",
		"pmr" => "application/x-perfmon",
		"pmw" => "application/x-perfmon",
		"pnm" => "image/x-portable-anymap",
		"pot" => "application/vnd.ms-powerpoint",
		"ppm" => "image/x-portable-pixmap",
		"pps" => "application/vnd.ms-powerpoint",
		"ppt" => "application/vnd.ms-powerpoint",
		"prf" => "application/pics-rules",
		"ps" => "application/postscript",
		"pub" => "application/x-mspublisher",
		"qt" => "video/quicktime",
		"ra" => "audio/x-pn-realaudio",
		"ram" => "audio/x-pn-realaudio",
		"ras" => "image/x-cmu-raster",
		"rgb" => "image/x-rgb",
		"rmi" => "audio/mid",
		"roff" => "application/x-troff",
		"rtf" => "application/rtf",
		"rtx" => "text/richtext",
		"scd" => "application/x-msschedule",
		"sct" => "text/scriptlet",
		"setpay" => "application/set-payment-initiation",
		"setreg" => "application/set-registration-initiation",
		"sh" => "application/x-sh",
		"shar" => "application/x-shar",
		"sit" => "application/x-stuffit",
		"snd" => "audio/basic",
		"spc" => "application/x-pkcs7-certificates",
		"spl" => "application/futuresplash",
		"src" => "application/x-wais-source",
		"sst" => "application/vnd.ms-pkicertstore",
		"stl" => "application/vnd.ms-pkistl",
		"stm" => "text/html",
		"svg" => "image/svg+xml",
		"sv4cpio" => "application/x-sv4cpio",
		"sv4crc" => "application/x-sv4crc",
		"t" => "application/x-troff",
		"tar" => "application/x-tar",
		"tcl" => "application/x-tcl",
		"tex" => "application/x-tex",
		"texi" => "application/x-texinfo",
		"texinfo" => "application/x-texinfo",
		"tgz" => "application/x-compressed",
		"tif" => "image/tiff",
		"tiff" => "image/tiff",
		"tr" => "application/x-troff",
		"trm" => "application/x-msterminal",
		"tsv" => "text/tab-separated-values",
		"txt" => "text/plain",
		"uls" => "text/iuls",
		"ustar" => "application/x-ustar",
		"vcf" => "text/x-vcard",
		"vrml" => "x-world/x-vrml",
		"wav" => "audio/x-wav",
		"wcm" => "application/vnd.ms-works",
		"wdb" => "application/vnd.ms-works",
		"wks" => "application/vnd.ms-works",
		"wmf" => "application/x-msmetafile",
		"wps" => "application/vnd.ms-works",
		"wri" => "application/x-mswrite",
		"wrl" => "x-world/x-vrml",
		"wrz" => "x-world/x-vrml",
		"xaf" => "x-world/x-vrml",
		"xbm" => "image/x-xbitmap",
		"xla" => "application/vnd.ms-excel",
		"xlc" => "application/vnd.ms-excel",
		"xlm" => "application/vnd.ms-excel",
		"xls" => "application/vnd.ms-excel",
		"xlt" => "application/vnd.ms-excel",
		"xlw" => "application/vnd.ms-excel",
		"xof" => "x-world/x-vrml",
		"xpm" => "image/x-xpixmap",
		"xwd" => "image/x-xwindowdump",
		"z" => "application/x-compress",
		"zip" => "application/zip");
	$type = $mime_types[$ext];
	return $type;
}
?>