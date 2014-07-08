<?php
require_once('../../Perspicuous/config.inc');

Config::$UserName = '';
Config::$Password = '';
Config::$Database = '';
Config::$DateFormat = 'm/d/Y';
Config::$TimeFormat = 'h:i A';
Config::$Theme = 'halcyonic';
Config::$Live = false;
class GlobalValidation {
	public static $Email = '^\w+@[a-zA-Z_]+?\.[a-zA-Z]{2,3}$';
	public static $Zip = '(^\d{5}$)|(^\d{5}-\d{4}$)';
}
?>