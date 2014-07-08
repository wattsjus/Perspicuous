<?php 
require_once('AttributeClasses.inc');
require_once('Form.inc');
require_once('HTMLControls.inc');
require_once('TableSession.inc');
$_AppSession = new TableSession();
$datacache = 0;
class DataObject {
	private $_Null = false;
	private $_History = null;
	private $_OtherValues = array();
	public static $AddOnFunctions = array();
	public $ControlId = 0;
	public static $NextId = 0;
	private static $DBObject = null;
	public static function GetProperties($justNames = false) {
		$reflection = new ReflectionClass(get_called_class());
		$props = array();
		foreach($reflection->getProperties() as $key => $val) {
			if($val->class <> 'DataObject') {
				$Reflection = new ReflectionProperty($val->class, $val->name);
				if($justNames == true) {
					$props[] = $val->name;
				} else {
					$rules = self::SplitValidationRules($comment);
					$r = array();
					foreach($rules as $key2 => $val2) {
						$temp= GetFunctionAndParams($val2);
						$r[$temp['FunctionName']] = $temp['Params'];
					}
					$props[$val->name] = $r;
				}
			}
		}
		return $props;
	}
	public static function GetPropertyLabels() {
		$reflection = new ReflectionClass(get_called_class());
		$props = array();
		foreach($reflection->getProperties() as $key => $val) {
			if($val->class == get_called_class()) {
				$Reflection = new ReflectionProperty(get_called_class(), $val->name);
				$rules = self::SplitValidationRules($Reflection->getDocComment());
				$props[$val->name] = $val->name;
				foreach($rules as $key2 => $val2) {
					$temp= GetFunctionAndParams($val2);
					if($temp['FunctionName'] == 'Label') {
						$props[$val->name] = $temp['Params'][0];
					}
				}
			}
		}
		return $props;
	}
	public function ReNew() {
		foreach(get_object_vars($this) as $key => $val) {
			if($key != '_History' && $key != '_OtherValues' && $key != '_Null' && $key != '_ValidationRules' && $key != 'AddOnFunctions' && $key != 'ControlId' && $key != 'NextId') {
				unset($this->$key);
			}
		}
	}
	public function __construct() {
		$this->_History = new History();
		foreach(get_object_vars($this) as $key => $val) {
			if($key != '_History' && $key != '_OtherValues' && $key != '_Null' && $key != '_ValidationRules' && $key != 'AddOnFunctions' && $key != 'ControlId' && $key != 'NextId') {
				$this->_History->OldValues[$key] = $val;
				$Reflection = new ReflectionProperty(get_called_class(), $key);
				//$this->_ValidationRules[$key] = $Reflection->getDocComment();
				unset($this->$key);
			}
		}
		DataObject::$NextId += 1;
                $this->ControlId = DataObject::$NextId;
	}
	function __call($method, $params) {
		if(array_search($method, self::GetProperties(true)) !== false) {
			return self::__callStatic($method, $params);
                } else if(array_key_exists($method, DataObject::$AddOnFunctions)) {
                        $newParams = array();
                        $newParams[] = $this; // need to pass reference of this to addon function
                        foreach($params as $key => $val) {
                            $newParams[] = & $params[$key];
                        }
                        return call_user_func_array(DataObject::$AddOnFunctions[$method], $newParams);
                } else {
                	return true; //if the addin doesn't exist for the particular function process should still move on.
                }
        }
       	public static function __callStatic($name, $params) {
		if(array_search($name, self::GetProperties(true)) !== false) {
			return new StaticProperty(get_called_class(), $name);
		} else if(array_search($name,get_class_methods(get_called_class()))!== false) {
			$c = get_called_class();
			$r = call_user_func_array("$c::$name",$params);
			return $r;
		} else {
			return null;
		}
	}
	function __set($name, $val) {
		if(is_object($val) && get_class($val) == 'Property') $val = $val->value;
		if(array_key_exists($name, $this->_History->OldValues)) {
			$this->_History->NewValues[$name] = $val;
		} else {
			$this->_OtherValues[$name] = $val;
		}
	}
	function __get($name) {
		if(array_key_exists($name, $this->_OtherValues)) {
			return $this->_OtherValues[$name];
		} else {
			if(array_key_exists($name, $this->_History->OldValues) === false ) return null;
			$val = isset($this->_History->NewValues[$name]) ? $this->_History->NewValues[$name] : $this->_History->OldValues[$name];
			return new Property($this, $name, $val);
		}
	}
	public static function GetDBObject() {
		if(self::$DBObject == null) {
			return new Config::$DbType();
		} else {
			return self::$DBObject;
		}
	}
	public function GetHistory() {
		return $this->_History;
	}
	public static function GetValidationRules($prop) {
		$rules = array();
		$reflection = new ReflectionClass(get_called_class());
		foreach($reflection->getProperties() as $key => $val) {
			if($val->class <> 'DataObject' && $val->name.'' == $prop) {
				$Reflection = new ReflectionProperty(get_called_class(), $val->name);
				return $Reflection->getDocComment();
			}
		}
	}
	public function NewToOld() {
		foreach($this->_History->NewValues as $key => $value) {
			$this->_History->OldValues[$key] = $value;
			unset($this->_History->NewValues[$key]);
		}
	}
	private static function encrypt($str, $key)
	{
		$block = mcrypt_get_block_size('des', 'ecb');
		if (($pad = $block - (strlen($str) % $block)) < $block) {
		$str .= str_repeat(chr($pad), $pad);
		}
		
		return mcrypt_encrypt(MCRYPT_DES, $key, $str, MCRYPT_MODE_ECB);
	}
	
	private static function decrypt($str, $key)
	{   
		$str = mcrypt_decrypt(MCRYPT_DES, $key, $str, MCRYPT_MODE_ECB);
		
		# Strip padding out.
		$block = mcrypt_get_block_size('des', 'ecb');
		$pad = ord($str[($len = strlen($str)) - 1]);
		if ($pad && $pad < $block && preg_match('/' . chr($pad) . '{' . $pad . '}$/', $str)) {
			return substr($str, 0, strlen($str) - $pad);
		}
		return $str;
	}
	public function Save($validate = false) {
		if(!$validate || $this->IsValid()) {
			if($this->HasDataChanges()) {
				if(is_callable(array($this, 'PreSave'))) {
					$this->PreSave();
				}
				$temp = json_decode($this->ToJSON(true));
				$ClassName = get_class($this);
				$PrimaryField = $ClassName."Id";
				$primVal = $temp->$PrimaryField;
				if(($primVal === 0) || empty($primVal) || ($primVal == '')) {
					$keys = array();
					$values = array();
					foreach($temp as $key => $val) {
						if($this->$key->Has('Guid')) {
							if($key == $PrimaryField) {
								$this->$PrimaryField = self::GetDBObject()->NewBinaryGuid();
								$val = $this->$PrimaryField;
							}
							$values[] = self::GetDBObject()->FormatBinaryGuid($val);
							$keys[] = self::GetDBObject()->FormatField($key);
						} else if($this->$key->Has('Date')) {
							$values[] = self::GetDBObject()->FormatDate($val.'');
							$keys[] = self::GetDBObject()->FormatField($key);
						} else if($this->$key->Has('Secure')) {
							$param = $this->$key->AttributeParams('Secure');
							$values[] = self::GetDBObject()->FormatString(self::encrypt($val, $param[0]));
							$keys[] = self::GetDBObject()->FormatField($key);
						} else if($this->$key->Has('Bool')) {
							$values[] = self::FormatString($val == true ? '1' : '0');
							$keys[] = self::GetDBObject()->FormatField($key);
						} else {
							if($key != $PrimaryField) {
								$values[] = self::GetDBObject()->FormatString($val);
								$keys[] = self::GetDBObject()->FormatField($key);
							}
						}
					}
					$fields = implode(",", $keys);
					$vals = implode(",", $values);
					$sql = self::GetDBObject()->CompileInsert($fields, $vals, $ClassName);
				} else {
					$set = array();
					foreach($this->GetDataChanges() as $change) {
						$field = $change['Field'];
						if($field != $PrimaryField) {
							if($this->$field->Has('Date')) {
								$set[] = self::GetDBObject()->FormatField($field).' = ' . self::GetDBObject()->FormatDate($change['NewValue']);
							} else if($this->$field->Has('Guid')) {
								$set[] = self::GetDBObject()->FormatField($field).' = ' . self::GetDBObject()->FormatBinaryGuid($change['NewValue']);
							} else if($this->$field->Has('Secure')) {
								$param = $this->$field->AttributeParams('Secure');
								$set[] = self::GetDBObject()->FormatField($field).' = ' . self::GetDBObject()->FormatString(self::encrypt($change['NewValue'], $param[0]));
							} else if($this->$field->Has('Bool')) {
								$set[] = self::GetDBObject()->FormatField($field).' = ' . self::GetDBObject()->FormatString($change['NewValue'] == false ? '0' : '1');
							} else {
								$set[] = self::GetDBObject()->FormatField($field).' = ' . self::GetDBObject()->FormatString($change['NewValue']);
							}
						}
					}
					$sets = implode(",", $set);
					if($this->$PrimaryField->Has('Guid')) {
						$sql = self::GetDBObject()->CompileUpdate($sets, $PrimaryField, 'unhex(\''.$this->$PrimaryField.'\')', $ClassName);
					} else {
						$sql = self::GetDBObject()->CompileUpdate($sets, $PrimaryField, $this->$PrimaryField, $ClassName);
					}
				}
				$result = self::GetDBObject()->Query($sql);
				if($result->insert_id > 0) {
					$obj->$PrimaryField = $result->insert_id;
				}
				$this->NewToOld();
			}
			return true; //if you want to save the object and the data is the same then there is no need to save.
		} else {
			return false;
		}
	}
	public function Delete() {
		if(get_class($this) == "DataObject") { //checking to see if the call was from a static method
			return self::DeleteIn(get_called_class(), func_get_args());
		} else {
			$temp = json_decode($this->ToJSON());
			$ClassName = get_class($this);
			$PrimaryField = $ClassName."Id";
			if(!empty($temp->$PrimaryField)) {
				$val = $temp->$PrimaryField;
				if($this->$PrimaryField->Has('Guid')) {
					$val = self::GetDBObject()->FormatBinaryGuid($temp->$PrimaryField);
				}
				self::GetDBObject()->Query($this->GetDBObject()->CompileDelete($PrimaryField, $val, $ClassName));
			} else {
				return false;
			}
			return true;
		}
	}
	public static function DeleteIn($table, $args) {
		$ClassName = $table;
		$PrimaryField = $ClassName."Id";
		$fields = self::GetProperties();
		if(isset($fields[$PrimaryField]['Guid'])) {
			foreach($args as $key => $val) {
				$args[$key] = self::GetDBObject()->FormatBinaryGuid($val);	
			}
		}
		$fields = implode(',',$args);
		self::GetDBObject()->Query(self::GetDBOBject()->CompileDeleteIn($PrimaryField, $fields, $ClassName));
		return true; 
	}
	public static function Get() {
		$args = func_get_args();
		$table = get_called_class();
		$ClassName = $table;
		$PrimaryField = $ClassName.'Id';
		if(count($args) > 1) {
			$temp = self::GetAllIn($args);
		} else {
			$temp = new $table();
			if($temp->$PrimaryField->Has('Guid')) $primaryGuid = true;
			$temp = json_decode($temp->ToJSON(true));
			$id = $args[0];
			if(!empty($id)) {
				$field = array();
				foreach($temp as $key => $val) {
					$field[] = self::GetDBObject()->FormatField($key);
				}
				$fields = implode(',', $field);
				if($primaryGuid == true) { $id = self::GetDbObject()->FormatBinaryGuid($id); }
				$sql = self::GetDBObject()->CompileSelect($fields, $ClassName). ' '.self::GetDBObject()->CompileWhereIn($PrimaryField, $id);
				$result = self::GetDBObject()->Query($sql);
				$result = $result->fetch_object($table);
				if(empty($result)) {
					return null;
				} else {
					return self::UnpackResult($result);
				}
			}
			if($args[0] != $temp->$PrimaryField->value) { $temp = new $ClassName(); $temp->_Null = true; }
		}
		return $temp;
	}
	private static function UnpackResult($result) {
		foreach(json_decode($result->ToJSON(true)) as $key => $val) {
			if($result->$key->Has('Guid')) {
				$result->$key = self::str2hex($result->$key->value);
			if($result->$key->value == '00000000000000000000000000000000') {
				$result->$key = null;
			}
			} else if($result->$key->Has('Date')) {
				if($result->$key->value != null) {
					$result->$key = date(Config::$DateFormat.' '.Config::$TimeFormat, strtotime($result->$key->value));
				}
			} else if($result->$key->Has('Secure')) {
				$param = $result->$key->AttributeParams('Secure');
				$result->$key = self::decrypt($result->$key->value, $param[0]);
			} else {
				$result->$key = $val;
			}
		}
		$result->NewToOld();
		return $result;
	}
	private static function str2hex($string)
	{
	    $hex='';
	    for ($i=0; $i < strlen($string); $i++)
	    {
		$val = dechex(ord($string[$i]));
		if(strlen($val) == 1) $val = "0$val";
	        $hex .= $val;
	    }
	    return $hex;
	}
	public static function GetAll($page = 0, $pagelength = 0) {
		$table = get_called_class();
		$ClassName = $table;
		$PrimaryField = $ClassName . "Id";
		$objs = new DataObjectArray();
		$field = array();
		$temp = $table::GetProperties(true);
		foreach($temp as $key => $val) {
			$field[] = self::GetDBObject()->FormatField($val);
		}
		$fields = implode(',', $field);
		if($pagelength == 0) {
			$sql = self::GetDBObject()->CompileSelect($fields, $ClassName);
		} else {
                        $order = $page > 0 ? "ASC" : "DESC";
                        $result = self::GetDBObject()->Query(self::GetDbObject()->CompileSelect("COUNT(*) C", $ClassName));
                        $row = $result->fetch_assoc();
                        $totalRecords = $row["C"];
                        $totalPages = ceil($totalRecords / $pagelength);
                        $page = $page > 0 ? $page : $totalPages - ($page + 1);
			$sql = self::GetDBObject()->CompileSelect($fields, $ClassName). ' ' . self::GetDBObject()->CompileOrderBy($PrimaryField, $order) . ' ' . self::GetDBObject()->CompileLimit(($pagelength * ($page - 1)), $pagelength);
		}
		$result = self::GetDBObject()->Query($sql);
		while($obj = $result->fetch_object($table)) {
			$objs[] = self::UnpackResult($obj);
		}
                $objs->Sort();
                foreach($objs as $key => $val) {
                        $val->NewToOld();
                }
                return $objs;
	}
	public static function GetPage($page, $pagelength = 10) {
		if($pagelength > 0) {
			return self::GetAll($page, $pagelength);
		} else {
			return null;
		}
	}
	private static function GetAllIn() {
		if(is_array($arr)) {
			$table = get_called_class();
			$ClassName = $table;
			$PrimaryField = $ClassName . "Id";
			$objs = new DataObjectArray();
			$field = array();
		$temp = $table::GetProperties(true);
		foreach($temp as $key => $val) {
			$field[] = self::GetDBObject()->FormatField($val);
		}
			$fields = implode(',', $field);
			if($temp->$PrimaryField->Has('Guid')) {
				foreach($arr as $key => $val) {
					$arr[$key] = self::GetDBObject()->FormatBinaryGuid($val);
				}
			}
			$Ids = implode(',', $arr);
			$result = self::GetDBObject()->Query(self::GetDBObject()->CompileSelect($fields,$ClassName).' '.self::GetDBObject()->CompileWhereIn($PrimaryField,$Ids).' '.self::GetDBObject()->CompileOrderBy($PrimaryField));
			while($obj = $result->fetch_object($table)) {
				$objs[] = self::UnpackResult($obj);
			}
			return $objs;	
		} else {
			return null;
		}
                foreach($temp as $key => $val) {
                        $val->NewToOld();
                }
                return $temp;
	}
	public static function Where() {
		$ss = new SqlStatement();
		foreach(func_get_args() as $key => $val) {
			$ss->AddWhere($val);
		}
		return $ss;
	}
	public static function Join() {
		$ss = new SqlStatement();
		foreach(func_get_args() as $key => $val) {
		$ss->AddJoin(new Join(get_called_class(), $val->rightfield->class, $val));
		$val->rightfield = $val->rightfield->propertyName;
		}
		return $ss;
	}
	public static function On($leftfield, $rightfield = '') {
		if(empty($rightfield)) $rightfield = $leftfield;
		if(!(is_object($leftfield) && get_class($leftfield) == 'StaticProperty')) {
			$leftfield = call_user_func(get_called_class()."::$leftfield");
		}
		return new On($leftfield,$rightfield);
	}
	public static function OrderBy($orderby) {
		$ss = new SqlStatement();
		$ss->OrderBy($orderby);
		$class = get_called_class();
		$PrimaryField = $class.'Id';
		$ss->Where(new Where($class::$PrimaryField(), $class::$PrimaryField(), "Equals"));
		return $ss;
	}
	public function GetDataChanges() {
		$changes = array();
		foreach($this->_History->OldValues as $key => $val) {
			if(isset($this->_History->NewValues[$key]) && $this->_History->NewValues[$key].'' != $val) {
				$changes[] = array("Field" => $key, "OldValue" => $val, "NewValue" => $this->_History->NewValues[$key]); 
			}
		}
		return $changes;
	}
	public function HasDataChanges() {
		return count($this->GetDataChanges()) > 0;
	}
	public function ToJSON($all = false) {
                if($this->_Null) {
                        return "{}";
                } else {
			foreach($this->_History->OldValues as $key => $val) {
				if($all == true || !($this->$key->Has('Secure') || $this->$key->Has('Hidden'))) {
					if($this->$key->Has('Bool')) $json->$key = $this->$key->value == '1' ? true : false;
					else $json->$key = $this->$key->value;
				}
			}
			if($all == true) {
				foreach($this->_OtherValues as $key => $val) {
					$json->$key = $this->$key;
				}
			}
			return json_encode($json);
                }
	}
	public function FromJSON($json) {
		$temp = json_decode($json);
		foreach($temp as $key => $val) {
		  	if($key != '_History' && $key != '_OtherValues' && $key != '_Null' && $key != '_ValidationRules' && $key != 'AddOnFunctions' && $key != 'ControlId' && $key != 'NextId') {
				if($this->$key->Has('Bool')) $this->$key = $val == true ? 1 : 0;
				else $this->$key = $val;
			}
		}
	}
	public static function GetJSValidatorFunctions($property) {
		$functions = array();
		$rules = self::SplitValidationRules(self::GetValidationRules($property));
		$hasRequired = false;
		foreach($rules as $key => $val) {
			$result = GetFunctionAndParams($val);
			if($result['FunctionName'] == 'Required') {
				$hasRequired = true;
				break;
			}
		}
	        foreach($rules as $key => $val) {
	                $result = GetFunctionAndParams($val);
	                $functionName = $result['FunctionName'];
	                if(is_array($result['Params'])) {
	                	$func_reflection = new ReflectionMethod("$functionName::GetJSValidatorFunction");
				$num_of_params = $func_reflection->getNumberOfParameters();
				if($num_of_params - 1 < count($result['Params'])) {
					$error = $result['Params'][count($result['Params'])-1];
					unset($result['Params'][$num_of_params-1]);
				}
				$result['Params'][] = $hasRequired;
				$func = call_user_func_array("$functionName::GetJSValidatorFunction", $result["Params"]);
				if(!empty($func) && !empty($error)) {
					$functions[] = "function(val) { var func = $func; var ret = func(val); if(ret) { return true; } else { InvalidatedMessages.push('".addslashes($error)."'); return false; } }";
				} else {
	                		$functions[] = $func;
	                	}
	                } else {
		                $functions[] = call_user_func("$functionName::GetJSValidatorFunction");
	                }
	        }
	        return $functions;
	}
	public static function SplitValidationRules($rules) {
		$e = explode("\n", trim(substr($rules,3,-2)));
		$ret = array();
		foreach($e as $key => $val) {
			$val = trim($val);
			if(isset($val) && $val != '' && $val != null) $ret[] = $val;
		}
		return $ret;
	}
	public function IsValid() {
		foreach($this->GetHistory()->OldValues as $key => $val) {
			if($this->IsPropertyValid($key) == false) return false;
		}
		return true;
	}
	public function IsPropertyValid($prop) {
		foreach(self::SplitValidationRules(self::GetValidationRules($prop)) as $key => $val) {
			$result = GetFunctionAndParams($val);
			$result["Params"][] = $this->$prop."";
	                $functionName = $result["FunctionName"];
			$v = call_user_func_array("$functionName::Validate", $result["Params"]);
			if($v == false) return false;
		}
		return true;
	}
	function __toString() {
		return $this->ToJSON();
	}
}
function GetFunctionAndParams($val) {
	foreach($GLOBALS as $key => $val2) {
		if($key != 'val') {
			$$key = $val2;
		}
	}
        $functionName = '';
	$params = array();
	$firstParam = strpos($val,'(');
	if($firstParam > -1) {
		$functionName = trim(substr($val, 0, $firstParam));
		$params = substr($val, $firstParam + 1, -1);
		$params = eval("return array($params);");
	} else {
		$functionName = trim($val);
	}
        return array("FunctionName" => $functionName, "Params" => $params);
}
class SqlFunction {
public $property;
public $function;
public function __construct($property, $function) {
$this->property = $property;
$this->function = $function;
}
}
class StaticProperty {
	public $class;
	public $propertyName;
	public function __construct($class, $name) {
		$this->class = $class;
		$this->propertyName = $name;
	}
	function __toString() {
		return $this->value.'';
	}
	public function In() {
		$vals = func_get_args();
if(count($vals) == 1 && is_array($vals[0])) {
$vals = $vals[0];
}
		foreach($vals as $key => $val) {
			if(is_object($val) && get_class($val) == 'Property') { $vals[$key] = $val->value; }
			else if($val == null) { $vals[$key] = 'NULL'; }
			else {$vals[$key] = $val;}
		}
		return new Where($this, $vals, "In");
	}
	public function Equals($obj) {
		return new Where($this, $obj, "Equals");
	}
	public function NotEquals($obj) {
		return new Where($this, $obj, "Not Equals");
	}
	public function GreaterThan($obj) {
		return new Where($this, $obj, "Greater Than");
	}
	public function LessThan($obj) {
		return new Where($this, $obj, "Less Than");
	}
	public function Like($obj) {
		return new Where($this, $obj, "Like");
	}
public function Sum() {
return new SqlFunction($this, "Sum");
}
public function Min() {
return new SqlFunction($this, "Min");
}
public function Max() {
return new SqlFunction($this, "Max");
}
	public function Has($obj) {
		$c = $this->class;
		$rules = $c::GetValidationRules($this->propertyName);
		$split = $c::SplitValidationRules($rules);
		foreach($split as $key => $val) {
			$result = GetFunctionAndParams($val);
			if(trim($result['FunctionName']) == trim($obj)) return true;
		}
	}
	public function AttributeParams($property) {
		$c = $this->class;
		foreach($c::SplitValidationRules($c::GetValidationRules($this->propertyName)) as $key => $val) {
			$result = GetFunctionAndParams($val);
			if($result['FunctionName'] == $property) return $result['Params'];
		}
	}
}
class Property {
	public $class;
	public $propertyName;
	public $value;
	public function __construct($class, $name, $value) {
		$this->class = $class;
		$this->propertyName = $name;
		$this->value = $value;
	}
	function __toString() {
		return $this->value.'';
	}
	public function Has($obj) {
		$c = $this->class;
		foreach($c::SplitValidationRules($c::GetValidationRules($this->propertyName)) as $key => $val) {
			$result = GetFunctionAndParams($val);
			if($result['FunctionName'] == $obj) return true;
		}
	}
	public function Equals($obj) {
		return new Where($this, $obj, "Equals");
	}
	public function Like($obj) {
		return new Where($this, $obj, "Like");
	}
	public function NotEquals($obj) {
		return new Where($this, $obj, "Not Equals");
	}
	public function GreaterThan($obj) {
		return new Where($this, $obj, "Greater Than");
	}
	public function LessThan($obj) {
		return new Where($this, $obj, "Less Than");
	}
	public function AttributeParams($property) {
		$c = $this->class;
		foreach($c::SplitValidationRules($c::GetValidationRules($this->propertyName)) as $key => $val) {
			$result = GetFunctionAndParams($val);
			if($result['FunctionName'] == $property) return $result['Params'];
		}
	}
}
class History {
	public $OldValues = array();
	public $NewValues = array();
}
class DataObjectArray extends ArrayObject {
	public function ReNew() {
		foreach($this as $obj) {
			$obj->ReNew();
		}
	}
private function isAssoc($arr)
{
    return array_keys($arr->getArrayCopy()) !== range(0, count($arr) - 1);
}
	public function ToJSON($all = false) {
		$ret = array();
if($this->isAssoc($this)) {
		foreach($this as $key => $val) {
			$ret[$key] = json_decode($val->ToJSON($all));
		}
} else {
		foreach($this as $obj) {
			$ret[] = json_decode($obj->ToJSON($all));
		}
}
		return json_encode($ret);
	}
        public function Sort() {
            $copy = $this->getArrayCopy();
            rsort($copy);
            foreach($this as $key => $val) {
              $this[$key] = $copy[$key];
            }
        }
        function __unset($name) {
		unset($this[$name]);
	}
	function __toString() {
		return $this->ToJSON();
	}
	public function Save($validate = false) {
		foreach($this as $key => $val) {
			if(!$val->Save($validate)) return false;
		}
		return true;
	}
	public function Where() {
		$wheres = func_get_args();
		$ret = new DataObjectArray();
		foreach($this as $key => $val) {
			$match = true;
			foreach($wheres as $key2 => $val2) {
				$leftclass = $val2->leftfield->class;
				$leftfield = $val2->leftfield->propertyName;
				$fieldval = $val2->rightfield;
				if($val->action == 'Equals') {
					if($val->$leftclass->$leftfield != $fieldval) {
						$match = false;
						break;
					}
				} else if($val->action == 'Not Equals') {
					if($val->$leftclass->$leftfield == $fieldval) {
						$match = false;
						break;
					}
				} else if($val->action == 'Greater Than') {
					if($val->$leftclass->$leftfield > $fieldval) {
						$match = false;
						break;
					}
				} else if($val->action == 'Less Than') {
					if($val->$leftclass->$leftfield < $fieldval) {
						$match = false;
						break;
					}
				}
			}
			if($match) {
				$ret[] = $val;
			}
		}
		return $ret;
	}
}
class DataResult {
	private $_objects = array();
	public function AddResultObject($obj) {
		if(!array_key_exists(get_class($obj).$obj->ControlId, $this->_objects)) {
			$this->_objects[get_class($obj)] = $obj;
		}
	}
	public function ReNew() {
		foreach($this->_objects as $key => $val) {
			$val->ReNew();
		}
	}
	function __get($name) {
		$posibilities = array();
		foreach($this->_objects as $key => $val) {
			if($key == $name) {
				return $val;
			}
			foreach($val->GetHistory()->OldValues as $key2 => $val2) {
				if($key2 == $name) {
					$posibilities[] = $this->_objects[$key]->$key2;
				}
			}
		}
		if(count($posibilities) < 2) {
			return $posibilities[0];
		} else {
			throw new Exception("Ambiguous field name $name in the result set.");
		}
	}
	function __set($name, $val3) {
		$posibilities = 0;
		foreach($this->_objects as $key => $val) {
			foreach($val->GetHistory()->OldValues as $key2 => $val2) {
				if($key2 == $name) {
					$posibilities++;
				}
			}
		}
		if($posibilities <= 1) {
			foreach($this->_objects as $key => $val) {
				foreach($val->GetHistory()->OldValues as $key2 => $val2) {
					if($key2 == $name) {
						$val->$key2 = $val3;
					}
				}
			}
		} else {
			throw new Exception("Ambiguous field name $name in the result set.");
		}
	}
	private function getAllKeys($obj) {
		$keys = array();
		foreach($obj->GetHistory()->OldValues as $key => $value) {
			$keys[] = $key;
		}
		foreach($obj as $key => $value) {
			if($key != 'ControlId') {
				$keys[] = $key;
			}
		}
		return $keys;
	}
	public function ToJSON($all = false) {
		$collisions = array();
		foreach($this->_objects as $key => $val) {
			$class = get_class($val);
			foreach($this->_objects as $key2 => $val2) {
				if($class != get_class($val2)) {
					foreach($this->getAllKeys($val) as $key3 => $val3) {
						foreach($this->getAllKeys($val2) as $key4 => $val4) {
							if($val3 == $val4 && !in_array($val3,$collisions)) {
								$collisions[] = $val3;
							}
						}
					}
				}
			}
		}
		foreach($this->_objects as $key => $val) {
			$class = get_class($val);
			foreach($this->getAllKeys($val) as $key2 => $val2) {
				if(in_array($val2,$collisions)) {
					if($all == true || !($this->$class->$val2->Has('Secure') || $this->$class->$val2->Has('Hidden'))) {
						if($this->$class->$val2->Has('Bool')) $obj->$class->$val2 = $this->$class->$val2->value == '1' ? true : false;
						else $obj->$class->$val2 = $this->$class->$val2->value;
					}
				} else {
					if($all == true || !($this->$val2->Has('Secure') || $this->$val2->Has('Hidden'))) {
						if($this->$val2->Has('Bool')) $obj->$val2 = $this->$val2->value == '1' ? true : false;
						else $obj->$val2 = $val->$val2->value;
					}
				}
			}
		}
		return json_encode($obj);
	}
	function __toString() {
		return $this->ToJSON();
	}
	public function Save($validate = false) {
		foreach($this->_objects as $key => $val) {
			if(!$val->Save($validate)) return false;
		}
		return true;
	}
}
class SqlStatement {
	private $joins = array();
	private $wheres = array();
	private $orderby = '';
	public function AddJoin($join) {
		$this->joins[] = $join;
	}
	public function AddWhere($where) {
		$this->wheres[] = $where;
	}
	public function OrderBy($orderby) {
		$this->orderby = $orderby;
		return $this;
	}
	public function Delete() {
		$sql = $this->BaseDelete();
		return DataObject::GetDBObject()->Query($sql);
	}
private function _Select($sql) {
global $datacache;
		global $_AppSession;
		if($datacache > 0 && empty($from_webapi)) {
			$query_key = md5($sql);
			if($_AppSession->$query_key) {
				return $_AppSession->$query_key;
			}
		}
		$result = DataObject::GetDBObject()->Query($sql);
		$ret = new DataObjectArray();
		while(null !== ($val = $result->fetch_assoc())) {
			$objs = array();
			foreach($val as $key2 => $val2) {
				$parts = explode('.',$key2);
				if(!array_key_exists($parts[0], $objs)) {
					$objs[$parts[0]] = new $parts[0]();
				}
				$k = $parts[1];
				$objs[$parts[0]]->$k = $val2;
			}
			$row = new DataResult();
			foreach($objs as $key2 => $val2) {
				$val2->NewToOld();
				$val2 = DataObject::UnpackResult($val2);
				$row->AddResultObject($val2);

			}
			$ret[] = $row;
		}
		if($datacache > 0) {
			$prevDatacache = $_AppSession->TimeOut;
			$_AppSession->TimeOut = $datacache;
			$_AppSession->$query_key = $ret;
			$_AppSession->TimeOut = $prevDatacache;
		}
		return $ret;
}
public function SelectFields() {
	$fields = func_get_args();
return $this->_Select($this->BaseSelect($fields));
}
	public function Select($start = 0, $count = 10) {
		$sql = $this->BaseSelect();
		if($start > 0) {
			$sql .= " LIMIT $start, $count";
		}
		return $this->_Select($sql);
	}
	private function CompileWhere($val) {
		$leftfield = $val->leftfield;
		$rightfield = $val->rightfield;
		if(is_object($val->leftfield) && get_class($val->leftfield) == 'StaticProperty') {
			$class = $leftfield->class;
			$leftfield = DataObject::GetDBObject()->FormatTableName($class).'.'.DataObject::GetDBObject()->FormatField($leftfield->propertyName);
			$this->j[$class] = $class::GetProperties(true);
		} else if($val->leftfield == null && $val->leftfield !== 0) {
			$leftfield = 'NULL';
		} else {
			if(is_array($leftfield)) {
			} else if(is_object($val->rightfield) && get_class($val->rightfield) == 'StaticProperty' && $val->rightfield->Has('Guid')) {
				$leftfield = DataObject::GetDBObject()->FormatBinaryGuild($leftfield);
			} else if(is_object($val->rightfield) && get_class($val->rightfield) == 'StaticProperty' && $val->rightfield->Has('Date')) {
				$leftfield = DataObject::GetDBObject()->FormatDate($leftfield);
			} else if($leftfield == null && $val->leftfield !== 0) {
				$leftfield = 'NULL';
			} else {
				if(is_object($val->rightfield) && get_class($val->rightfield) == 'StaticProperty' && $val->rightfield->Has('Secure')) {
					$params = $rightfield->AttributeParams('Secure');
					$leftfield = DataObject::encrypt($leftfield, $params[0]);
				}
				$leftfield = DataObject::GetDBObject()->FormatString($leftfield);
			}
		}
		if(is_object($val->rightfield) && get_class($val->rightfield) == 'StaticProperty') {
			$class = $rightfield->class;
			$rightfield = DataObject::GetDBObject()->FormatTableName($class).'.'.DataObject::GetDBObject()->FormatField($rightfield->propertyName);
			$this->j[$class] = $class::GetProperties(true);
		} else if($val->rightfield == null && $val->rightfield !== 0) {
			$rightfield = 'NULL';
		} else {
			if(is_array($rightfield)) {
			} else if(is_object($val->leftfield) && get_class($val->leftfield) == 'StaticProperty' && $val->leftfield->Has('Guid')) {
				$rightfield = DataObject::GetDBObject()->FormatBinaryGuid($rightfield);
			} else if(is_object($val->leftfield) && get_class($val->leftfield) == 'StaticProperty' && $val->leftfield->Has('Date')) {
				$rightfield = DataObject::GetDBObject()->FormatDate($rightfield);
			} else if($val->rightfield == null && $val->rightfield !== 0) {
				$rightfield = 'NULL';
			} else {
				if(is_object($val->leftfield) && get_class($val->leftfield) == 'StaticProperty' && $val->leftfield->Has('Secure')) {
					$params = $val->leftfield->AttributeParams('Secure');
					$rightfield = DataObject::encrypt($rightfield, $params[0]);
				}
				$rightfield = DataObject::GetDBObject()->FormatString($rightfield);
			}
		}
		if($rightfield == 'NULL') {
			$where = "($leftfield IS $rightfield OR $leftfield = UNHEX('00000000000000000000000000000000'))";
		} else if($leftfield == 'NULL') {
			$where = "($leftfield IS $rightfield OR $rightfield = UNHEX('00000000000000000000000000000000'))";
		} else if($val->action == 'Equals') {
			$where = "$leftfield = $rightfield";
		} else if($val->action == 'Not Equals') {
			$where = "$leftfield != $rightfield";
		} else if($val->action == 'Greater Than') {
			$where = "$leftfield > $rightfield";
		} else if($val->action == 'Less Than') {
			$where = "$leftfield < $rightfield";
		} else if($val->action == 'Like') {
			$where = "$leftfield LIKE '%".substr($rightfield,1,-1)."%'";
		} else if($val->action == 'In') {
			$hadNull = '';
			foreach($rightfield as $key => $val2) {
				if($val2 == 'NULL') {
					$hadNull = " $leftfield IS NULL";
					unset($rightfield[$key]);
				} else {
					if($val->leftfield->Has('Guid')) {
						$rightfield[$key] = DataObject::GetDBObject()->FormatBinaryGuid($val2);
					} else if($val->leftfield->Has('Date')) {
						$rightfield[$key] = DataObject::GetDBObject()->FormatDate($val2);
					} else {
						$rightfield[$key] = DataObject::GetDBObject()->FormatString($val2);
					}
				}
			}
			if(count($rightfield) > 0) {
				$rightfields = implode(',',$rightfield);
				$where = "$leftfield IN ($rightfields)";
			}
			if(!empty($where) && !empty($hadNull)) {
				$or = ' OR ';
			}
			if(isset($val->or)) {
				return "(($where $or $hadNull) OR " . $this->CompileWhere($val->or).')';
			} else {
				return "($where $or $hadNull)";
			}
		}
		if(isset($val->or)) {
			return "($where OR " . $this->CompileWhere($val->or).')';
		} else {
			return $where;
		}
	}
	private function CompileWheres() {
		$where = array();
		foreach($this->wheres as $key => $val) {
			$where[] = $this->CompileWhere($val);	
		}
		return $where;
	}
	public function BaseDelete() {
		$wheres = $this->CompileWheres();
		$flefttable = array_keys($this->j);
		$flefttable = DataObject::GetDBObject()->FormatTableName($flefttable[0]);
		return "DELETE FROM $flefttable WHERE ".implode(' AND ', $wheres);
	}
	private $j;
	public function BaseSelect($fields = null) {
		$this->j = array();
		foreach($this->joins as $key => $val) {
			$lefttable = $val->lefttable;
			$righttable = $val->righttable;
			$flefttable = DataObject::GetDBObject()->FormatTableName($val->lefttable);
			$frighttable = DataObject::GetDBObject()->FormatTableName($val->righttable);
			$leftfield = DataObject::GetDBObject()->FormatField($val->on->leftfield);
			$rightfield = DataObject::GetDBObject()->FormatField($val->on->rightfield);
			if(count($this->j) == 0) {
				$joins = "$flefttable LEFT JOIN $frighttable ON $flefttable.$leftfield = $frighttable.$rightfield";
			} else {
				$joins .= " LEFT JOIN $frighttable ON $flefttable.$leftfield = $frighttable.$rightfield";
			}
			if(count($val->on->cases) > 0) {
				foreach($val->on->cases as $key2 => $val2) {
					$joins .= " AND ". $this->CompileWhere($val2);
				}
			}
if($fields == null) {
			$this->j[$lefttable] = $lefttable::GetProperties(true);
			$this->j[$righttable] = $righttable::GetProperties(true);
}
		}
		$field = array();
if($fields != null) {
foreach($fields as $key => $val) {
if(get_class($val) == 'SqlFunction') {
$field[] = $val->function.'(`'.$val->property->class.'`.`'.$val->property->propertyName.'`) AS '.$val->property->propertyName.$val->function;
} else {
$field[] = '`'.$val->property->class.'`.`'.$val->property->propertyName.'` AS `'.$val->property->class.'.'.$val->property->propertyName.'`';
}
}
}
		$where = $this->CompileWheres();
		$wheres = implode(' AND ', $where);
if($fields == null) {
		foreach($this->j as $key => $val) {
			foreach($val as $key2 => $val2) {
				$field[] = "`$key`.`$val2` AS `$key.$val2`";
			}
		}
}
		$fields = implode(',',$field);
		if(!empty($where) && $where != '') {
			$wheres = "WHERE $wheres";
		}
		if(count($joins)>0) {
			$sql = "SELECT $fields FROM $joins $wheres";
		} else {
			$class = $this->wheres[0]->leftfield->class;
			$sql = "SELECT $fields FROM `$class` $wheres";
		}
		if(empty($this->orderby) || $this->orderby == '') {
			return $sql;
		} else {
			return $sql." ORDER BY $this->orderby";
		}
	}
	public function Where() {
		foreach(func_get_args() as $key => $val) {
			$this->AddWhere($val);
		}
		return $this;
	}
}
class Where {
	public $leftfield;
	public $rightfield;
	public $action;
	public $or;
	function __construct($leftfield, $rightfield, $action) {
		$this->action = $action;
		$this->leftfield = $leftfield;
		$this->rightfield = $rightfield;
	}
	public function Either($where) {
		$this->or = $where;
		return $this;
	}
}
class On {
	public $leftfield;
	public $rightfield;
	public $cases;
	function __construct($rightfield, $leftfield = null) {
		$this->cases = array();
		if($leftfield == null) {
			if(is_object($rightfield) && get_class($rightfield) == 'StaticProperty') {
				$this->leftfield = $rightfield->propertyName;
			} else {
				$this->leftfield = $rightfield;
			}
		} else {
			if(is_object($leftfield) && get_class($leftfield) == 'StaticProperty') {
				$this->leftfield = $leftfield->propertyName;
			} else {
				$this->leftfield = $leftfield;
			}
		}
		$this->rightfield=$rightfield;
	}
	public function AddCase($where) {
		$this->cases[] = $where;
		return $this;
	}
}
class Join {
	public $lefttable;
	public $righttable;
	public $on;
	function __construct($lefttable, $righttable, $on) {
		$this->lefttable = $lefttable;
		$this->righttable = $righttable;
		$this->on = $on;
	}
}
interface DBType {
	public function FormatTableName($name);
	public function FormatField($name);
	public function FormatBinaryGuid($value);
	public function FormatDate($value);
	public function FormatString($value);
	public function NewBinaryGuid();
	public function CompileSelect($fields, $table, $start = 0, $end = 0);
	public function CompileLimit($start, $end);
	public function CompileOrderBy($Field, $Direction = 'ASC');
	public function CompileWhere($PKName, $PKValue);
	public function CompileWhereIn($PKName, $PKValues);
	public function CompileInsert($fields, $values, $table);
	public function CompileUpdate($sets, $PKName, $PKValue, $table);
	public function CompileDelete($PKName, $PKValue, $table);
	public function CompileDeleteIn($PKName, $PKValues, $table);
	public function Query($sql);
	public static function IsDBDate($string);
}
$path = dirname(__FILE__);
if ($handle = opendir("$path/Plugins")) {
    while (false !== ($entry = readdir($handle))) {
        if(substr($entry, -4) == ".inc") {
            require_once("$path/Plugins/$entry");
        }
    }
    closedir($handle);
}
?>