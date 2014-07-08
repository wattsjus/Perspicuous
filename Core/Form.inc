<?
class Form {
	public static $Name;
        public static $ControlClasses = array();
	public static function Start ($name, $page = '', $method='POST') {
		Form::$ControlClasses = array();
		Form::$Name = $name;
if($page != '') $page="action='$page'";
		echo "<form method='$method' $page name='$name' id='$name'><script>var ".Form::$Name."Validator = [];</script>";
	}
	public static function End() {
	        $fields = implode('|', Form::$ControlClasses);
	        echo "<input type='hidden' value='$fields' name='Controls' />";
		Form::$Name = '';
		echo "</form>";
		Form::$ControlClasses = array();
	}
        private static function AddControlClass($instance) {
                Form::$ControlClasses[$instance->ControlId] = $instance->ControlId.'~'.get_class($instance);
        }
	public static function Password($property, $elementname = '') {
		if(is_object($property) && (get_class($property) == 'Property' || get_class($property) == 'StaticProperty')) {
			$controlId = $property->class->ControlId;
			if(empty($elementname)) $elementname = "$property->propertyName$controlId";
			$name = "$controlId#$property->propertyName";
			Form::AddControlClass($property->class);
			if($property->Has('Length')) {
				$size = $property->AttributeParams('Length');
				$size = $size[1];
			} else {
				$size = 0;
			}
			$c = new PasswordControl($name, $elementname, $property, Form::GetValidators($property, "#$elementname"));
			return $c->SetSize($size);
		} else {
			$c = new PasswordControl($property->ElementName,$property->ElementId, $property, '');
			return $c->SetSize($property->Size);
		}
	}
	public static function CorrectionBox() {
		return new CorrectionBox(Form::$Name);
	}
	public static function Label($property) {
        	$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		return new LabelControl($elementname, $property);
	}
        public static function Span($property, $class = 'spanLabel') {
        	$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		return new SpanControl($elementname, $property, $class);
        }
        public static function Div($property) {
        	$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		return new DivControl($elementname, $property);
        }
	public static function LabelFor($property, $htmlcontrol = null) {
		return new LabelForControl($property, $htmlcontrol);
	}
        public static function Text($property,$elementname='') {
		$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		$name = "$controlId#$property->propertyName";
                Form::AddControlClass($property->class);
		if($property->Has('Length')) {
			$size = $property->AttributeParams('Length');
			$size = $size[1];
		} else {
			$size = 0;
		}
		$c = new TextControl($name, $elementname, $property, Form::GetValidators($property, "#$elementname"));
                return $c->SetSize($size);
        }
        public static function Decimal($property,$elementname='') {
		$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		$name = "$controlId#$property->propertyName";
                Form::AddControlClass($property->class);
		$c = new DecimalControl($name, $elementname, $property, Form::GetValidators($property, "#$elementname"));
        }
        public static function ComboBox($property,$textandvals = null, $enabled = true, $elementname='') {
		$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		$name = "$controlId#$property->propertyName";
		Form::AddControlClass($property->class);
                return new ComboBoxControl($name, $elementname, $property, $textandvals, $enabled, Form::GetValidators($property, "#$elementname"));
        }
        public static function Hidden($property, $elementname = '') {
if(!is_object($property)) {
$name = $property;
$value = $elementname;
$elementname=$name;
} else {
		$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		$name = "$controlId#$property->propertyName";
                Form::AddControlClass($property->class);
$value = $property;
}
                return new HiddenControl($name, $elementname, $value);
        }
        public static function RadioGroup($property, $textandvals = null, $enabled = true, $elementname ='') {
		$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		$name = "$controlId#$property->propertyName";
                Form::AddControlClass($property->class);
                return new RadioGroupControl($name, $elementname, $property, $textandvals, $enabled, Form::GetValidators($property, "input[type='radio'][name='$name']:checked"));
        }
        public static function Phone($property, $mask = '(999) 999-9999', $elementname = '') {
		$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		$name = "$controlId#$property->propertyName";
                Form::AddControlClass($property->class);
		return new PhoneControl($name, $elementname, $property, $mask, Form::GetValidators($property, "#$elementname"));
	}
        public static function Email($property, $elementname = '') {
		$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		$name = "$controlId#$property->propertyName";
                Form::AddControlClass($property->class);
		if($property->Has('Length')) {
			$size = $property->AttributeParams('Length');
			$size = $size[1];
		} else {
			$size = 0;
		}
		$c = new EmailControl($name, $elementname, $property, Form::GetValidators($property, "#$elementname"));
                return $c->SetSize($size);
        }
        public static function CheckBox($property, $text, $onvalue = '1', $enabled = true, $elementname='') {
		$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		$name = "$controlId#$property->propertyName";
                Form::AddControlClass($property->class);
                return new CheckBoxControl($name, $elementname, $property, $text, $onvalue, $enabled, Form::GetValidators($property, "#$elementname"));
        }
        public static function Number($property, $min, $max, $elementname='') {
		$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		$name = "$controlId#$property->propertyName";
                Form::AddControlClass($property->class);
                return new NumberControl($name, $elementname, $property, Form::GetValidators($property, "#$elementname"));
        }
        public static function Date($property, $format = 'm/d/Y', $mask = '99/99/9999', $elementname='') {
		$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		$name = "$controlId#$property->propertyName";
                Form::AddControlClass($property->class);
                return new DateControl($name, $elementname, $property, $format, $mask, Form::GetValidators($property, "#$elementname"));
        }
	public static function Area($property, $Columns = 42, $Rows = 6, $elementname = '') {
		$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		$name = "$controlId#$property->propertyName";
		Form::AddControlClass($property->class);
		return new AreaControl($name, $elementname, $property, $Columns, $Rows, Form::GetValidators($property, "#$elementname"));
	}
	public static function DateTime($property, $format = 'Y-m-d\TH:i:s', $mask = '99/99/9999 99:99', $elementname = '') {
		$controlId = $property->class->ControlId;
		if(empty($elementname)) $elementname = "$property->propertyName$controlId";
		$name = "$controlId#$property->propertyName";
		Form::AddControlClass($property->class);
		return new DateTimeControl($name, $elementname, $property, $format, $mask, Form::GetValidators($property, "#$elementname"));
	}
        public static function SubmitButton($text='Send') {
		return new SubmitButtonControl('','btnSubmit'.Form::$Name, '', $text, Form::$Name); 
        }
	private static function GetValidators($property, $elementname) {
		$vals = "<script>";
$class = $property->class;
		foreach($class::GetJSValidatorFunctions($property->propertyName) as $key => $val) {
			if(!empty($val)) {
				$vals .= Form::$Name."Validator.push({'Element':\"$elementname\",'Func':$val});";
			}
		}
		return $vals."</script>";
	}
        private static function PrintValidators($property, $elementname) {
                echo Form::GetValidators($property, $elementname);
        }
        public static function Relation($parent, $child) {
        	$parentControlId = $parent->class->ControlId;
        	$childControlId = $child->class->ControlId;
        	echo "<input type='hidden' name='Relation[]' value='$childControlId#$child->propertyName|$parentControlId#$parent->propertyName' />";
        }
	public static function PrintValue($property) {
		$db = Config::$DbType;
		if($db::IsDBDate($property->value)) {
			$value = date(Config::$DateFormat, strtotime($property->value));
		} else {
			$value = $property->value;
		}
		echo "<span>$value</span>";
	}
        public static function GetForm() {
               	if(isset($_GET["Controls"])) return $_GET;
        	if(isset($_POST["Controls"])) return $_POST;
        }
        public static function GetFormObjects() {
		$form = Form::GetForm();
		if(!empty($form)) { 
	        	$controls = explode('|', $form["Controls"]);
	        	$objs = new DataObjectArray();
	        	foreach($controls as $key => $val) {
		        	if($val != '') {
		        		$obj = explode('~', $val);
		        		$classId = $obj[0];
		        		$class = $obj[1];
		        		$PrimaryField = $class."Id";
		        		if(!empty($form[$classId.'#'.$PrimaryField])) {
		        			$obj = $class::Get($form[$classId.'#'.$PrimaryField]);
		        		} else {
		        			$obj = new $class();
		        		}
		        		if($obj == null) {
		        			$obj = new $class();
		        		}
		        		$db = Config::$DbType;
		        		foreach($obj->GetHistory()->OldValues as $key2 => $val2) {
		        			if(isset($form[$classId.'#'.$key2]) && $form[$classId.'#'.$key2] != $obj->$key2) {
						if($obj->$key2->Has('Date')) {
							$form[$classId.'#'.$key2] = substr(DataObject::GetDBObject()->FormatDate($form[$classId.'#'.$key2]),1,-1);
						}
		       					$obj->$key2 = $form[$classId.'#'.$key2];
		        			}
		        		}
		        		$objs[] = $obj;
	        		}
	        	}
	        	return $objs;
        	}
        }
        public static function SortFormObjectsByRelations($objects) {
        	$form = Form::GetForm();
        	$relations = $form['Relation'];
        	if(!empty($relations)) {
	        	foreach($relations as $key => $val) {
	        		$relation = explode('|', $val);
	        		$parent = explode('#', $relation[1]);
	        		$child = explode('#', $relation[0]);
	        		$parentControlId = $parent[0];
	        		$childControlId = $child[0];
	        		$parentId =Form::ControlOrdinalIdByControlId(& $objects, $parentControlId.'');
	        		$childId = Form::ControlOrdinalIdByControlId(& $objects, $childControlId.'');
	        		if(($parendId == '0' ? -1 : intval($parentId)) > ($childId == '0' ? -1 : intval($childId))) {
	        			$newObjects = new DataObjectArray();
	        			foreach($objects as $key => $val) {
	        				if($val->ControlId == $parentControlId) {
	        				} else if($val->ControlId == $childControlId) {
	        					$newObjects[] = $objects[Form::ControlOrdinalIdByControlId(& $objects, $parentControlId.'')];
	        					$newObjects[] = $objects[Form::ControlOrdinalIdByControlId(& $objects, $val->ControlId.'')];
	        				} else {
	        					$newObjects[] = $objects[Form::ControlOrdinalIdByControlId(& $objects, $val->ControlId.'')];
	        				}
	        			}
	        			$objects = $newObjects;
	        		}
	        	}
        	}
        	return $objects;
        }
        public static function SaveFormObjects() {
	        $objects = Form::SortFormObjectsByRelations(Form::GetFormObjects());
        	$form = Form::GetForm();
        	$relations = $form['Relation'];
        	$rel = array();
        	if(!empty($relations)) {
	        	foreach($relations as $key => $val) {
	        		$relation = explode('|', $val);
	        		$parent = explode('#', $relation[1]);
	        		$child = explode('#', $relation[0]);
	        		$parentControlId = $parent[0];
	        		$childControlId = $child[0];
	        		$rel[] = array("Parent" => $parentControlId, "Child" => $childControlId, "ParentField" => $parent[1], "ChildField" => $child[1]);
	        	}
        	}
        	if(!empty($objects)) {
	        	foreach($objects as $key => $val) {
	        	/*	$className = get_class($val);
	        		$PrimaryField = $className."Id";*/
	        		$val->Save();
	        		foreach($rel as $key2 => $val2) {
	        			if($val->ControlId == $val2['Parent']) {
	        				foreach($objects as $key3 => $val3) {
	        					if($val3->ControlId == $val2['Child']) {
	        						$parentField = $val2['ParentField'];
	        						$childField = $val2['ChildField'];
	        						$objects[$key3]->$childField = $val->$parentField->value;
	        					}
	        				}
	        			}
	        		}
	        	}
        	}
		return $objects;
        }
        private static function ControlOrdinalIdByControlId(& $objects, $controlId) {
        	$id = -1;
        	$i = 0;
        	foreach($objects as $key => $val) {
        		if($val->ControlId == $controlId) return $i;
        		$i++;
        	}
        }
        private static function ControlInOrder(& $objects, $parent, $child) {
        	$parentId = Form::ControlOrdinalIdByControlId(& $objects, $parent);
        	$childId = Form::ControlOrdinalIdByControlId(& $objects, $child);
        	return $parentId < $childId;
        }
}
?>