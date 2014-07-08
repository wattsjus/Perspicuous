<?
interface iAttribute {

}
class Label implements iAttribute {
	public static function Validate($val) {
		return true;
	}
	public static function GetJSValidatorFunction($required) {
		return '';
	}
}
class Secure implements iAttribute {
public static function Validate($val) {
return true;
}
public static function GetJSValidatorFunction($required) {
return '';
}
}
class Required implements iAttribute {
	public static function Validate($val) {
		return !empty($val) || $val === '0';
	}
        public static function GetJSValidatorFunction($required) {
           return "function(val) { return (val != null) && (val.trim() != ''); }";
        }
}
class Int implements iAttribute {
	public static function Validate($val) {
		return ((int)$val).'' == $val;
	}
        public static function GetJSValidatorFunction($required) {
if($required == false) $notRequired = "if(val == undefined || val == null || val.trim() == '') return true;";
           return "function(val) { $notRequired return parseInt(val) == val; }";
        }
}
class Bool implements iAttribute {
public static function Validate($val) {
return true;
}
public static function GetJSValidatorFunction($required) {
return '';
}
}
class MoreThan implements iAttribute {
	public static function Validate($min, $val) {
		return $val > $min;
	}
        public static function GetJSValidatorFunction($min, $required) {
if($required == false) $notRequired = "if(val == undefined || val == null || val.trim() == '') return true;";
           return "function(val) { $notRequired return val > $min; }";
        }
}
class LessThan implements iAttribute {
	public static function Validate($max, $val) {
		return $val < $max;
	}
        public static function GetJSValidatorFunction($max, $required) {
if($required == false) $notRequired = "if(val == undefined || val == null || val.trim() == '') return true;";
           return "function(val) { $notRequired return val < $max; }";
        }
}
class NotEqual implements iAttribute {
	public static function Validate($noneql, $val) {
		return $val != $noneql;
	}
        public static function GetJSValidatorFunction($noneql, $required) {
if($required == false) $notRequired = "if(val == undefined || val == null || val.trim() == '') return true;";
           return "function(val) { $notRequired return val+'' != '$noneql'; }";
        }
}
class Decimal implements iAttribute {
	public static function Validate($regex, $val) {
		return preg_match($regex, $val);
	}
        public static function GetJSValidatorFunction($head, $prec, $required) {
return RegEx::GetJSValidatorFunction("^\d{$head}+(\.\d{1,$prec})?$",$required);
        }
}
class RegEx implements iAttribute {
	public static function Validate($regex, $val) {
		return preg_match($regex, $val);
	}
        public static function GetJSValidatorFunction($regex, $required) {
if($required == false) $notRequired = "if(val == undefined || val == null || val.trim() == '') return true;";
           return "function(val) { $notRequired var pattern = /$regex"."/g; return pattern.test(val); }";
        }
}
class Guid implements iAttribute {
	private static $_regex = '^[a-fA-F0-9]{32}$';
	public static function Validate($val) {
		return RegEx::Validate(Guid::$_regex, $val);
	}
	public static function GetJSValidatorFunction($required) {
		return RegEx::GetJSValidatorFunction(Guid::$_regex, $required);
	}
}
class File implements iAttribute {
public static function Validate($val) {
return is_array($val);
}
public static function GetJSValidatorFunction($required) {
if($required == false) $notRequired = "if(val == undefined || val == null || val.trim() == '') return true;";
return "function(val) { $notRequired return val+'' != ''; }";
}
}
class Char implements iAttribute {
	public static function Validate($val) {
		return substr($val, 0,1) == $val;
	}
        public static function GetJSValidatorFunction($required) {
if($required == false) $notRequired = "if(val == undefined || val == null || val.trim() == '') return true;";
           return "function(val) { $notRequired return val.substring(0,1) == val; }";
        }
}
class Date implements iAttribute {
	public static function Validate($val) {
		$t = strtotime($val);
		if(!empty($t) && $t != '' && $t != null && !preg_match('/[a-zA-Z]/', $val)) {
			$m = date('m',$t);
			$d = date('d',$t);
			$y = date('Y',$t);
			return checkdate ($m, $d, $y);
		} else {
			return false;
		}
	}
        public static function GetJSValidatorFunction($required) {
if($required == false) $notRequired = "if(val == undefined || val == null || val.trim() == '') return true;";
           return "function(val) { $notRequired return !isNaN(Date.parse(val)); }";
        }
}
class Length implements iAttribute {
	public static function Validate($min, $max, $val) {
		return strlen($val) >= $min && strlen($val) <= $max;
	}
        public static function GetJSValidatorFunction($min, $max, $required) {
if($required == false) $notRequired = "if(val == undefined || val == null || val.trim() == '') return true;";
           return "function(val) { $notRequired return val.length >= $min && val.length <= $max }";
        }
}
class StrictVals implements iAttribute {
	public static function Validate() {
                $args = func_get_args();
		$val = $args[count($args)-1];
		for($i = 0; $i < count($args) - 1; $i++) {
			if($val == $args[$i]) {
				return true;
			}
		}
		return false;
	}
        public static function GetJSValidatorFunction($required) {
if($required == false) $notRequired = "if(val == undefined || val == null || val.trim() == '') return true;";
		$args = func_get_args();
		$vals = array();
		foreach($args as $key => $val) {
		  $vals[] = $val;
		}
		$regex = implode('|', $vals);
		return "function(val) { $notRequired var pattern = /$regex/g; return pattern.test(val); }";
        }
}
class Custom implements iAttribute {
	public static function Validate() {
                $args = func_get_args();
                $functionName = $args[0];
                unset($args[0]);
		return call_user_func_array($functionName, $args);
	}
        public static function GetJSValidatorFunction($required) {
           return "function(val) { return true; }";
        }
}
class Hidden implements iAttribute {
public static function Validate() {
return true;
}
public static function GetJSValidatorFunction($required) {
return '';
}
}
?>