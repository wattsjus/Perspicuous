<?
require_once('config.inc'); 
$type = array('css'=>'text/css','js'=>'text/javascript');
$ext = substr($_GET['p'], strrpos($_GET['p'],'.')+1);
header('Content-Type: '.$type[$ext]);
$last_modified_time = filemtime($perspicuous_path.'/'.$_GET['p']); 
$etag = md5_file($perspicuous_path.'/'.$_GET['p']);
header("Last-Modified: ".gmdate("D, d M Y H:i:s", $last_modified_time)." GMT"); 
header("Etag: $etag");
if (@strtotime($_SERVER['HTTP_IF_MODIFIED_SINCE']) == $last_modified_time || 
    @trim($_SERVER['HTTP_IF_NONE_MATCH']) == $etag) { 
    header("HTTP/1.1 304 Not Modified"); 
    die();
} else {
require_once($perspicuous_path.'/'.$_GET['p']);
}
?>