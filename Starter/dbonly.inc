<?php
require_once('config.inc');
require_once('../../Perspicuous/db.inc');
function __autoload($class_name) {
    include "Classes/$class_name.inc";
}
session_start();
?>