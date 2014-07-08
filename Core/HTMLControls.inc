<?
function getValue($value) {
if(is_object($value) && (get_class($value) == 'StaticProperty' || get_class($value) == 'Property')) {
return $value->value;
} else {
return $value;
}
}
function getDataBind($value, $prop = 'value') {
if(is_object($value) && (get_class($value) == 'StaticProperty' || get_class($value) == 'Property')) {
return "data-bind='$prop: $value->propertyName'";
} else {
return '';
}
}
class HTMLControl {
	public $ElementName;
	public $ElementId;
	public $Value;
	public $Validators;
	public $LabelOnLeft = true;
	public $TextAlignRight = true;
public $Size = 0;
	public function __construct($ElementName, $ElementId, $Value, $Validators = '') {
		$this->ElementName = $ElementName;
		$this->ElementId = $ElementId;
		$this->Value = $Value;
		$this->Validators = $Validators;
	}
	function __toString() {
		return "<span id='$this->ElementId' name='$this->ElementName'>$this->Value</span>\r\n$this->Validators\r\n";
	}
public function SetSize($size) {
$this->Size = $size;
return $this;
}
}
class CorrectionBox extends HTMLControl {
private $formname;
function __construct($formname) {
parent::__construct('','','');
$this->formname = $formname;
}
function __toString() {
return "<div class='".$this->formname."CorrectionBox CorrectionBox'></div>";
}
}
class PasswordControl extends HTMLControl {
function __construct($ElementName, $ElementId, $Value, $Validators = '') {
parent::__construct($ElementName, $ElementId, $Value, $Validators);
}
function __toString() {
if(is_object($this->Value) && get_class($this->Value) == 'PasswordControl') {
return "<input type='password' id='$this->ElementId"."Confirm' name='$this->ElementName"."Confirm' /><script>".Form::$Name."Validator.push({'Element':\"#$this->ElementId"."Confirm\",'Func':function(val) { var func = function(val) { return val == $('#$this->ElementId').val(); }; var ret = func(val); if(ret) { return true; } else { InvalidatedMessages.push('Passwords must match.'); return false; } }});</script>";
} else {
return "<input type='password' id='$this->ElementId' name='$this->ElementName' />\r\n$this->Validators\r\n";
}
}
}
class TextControl extends HTMLControl {
	function __construct($ElementName, $ElementId, $Value, $Validators = '') {
		parent::__construct($ElementName, $ElementId, $Value, $Validators);
	}
	function __toString() {
		$val = addslashes(getValue($this->Value));
$dbind = getDataBind($this->Value);
		$size = $this->Size > 0 ? "maxlength='$this->Size'" : '';
		return "<input type='text' $dbind id='$this->ElementId' $size name='$this->ElementName' value='$val' />\r\n$this->Validators\r\n";
	}
}
class FileControl extends HTMLControl {
function __construct($ElementName, $ElementId, $Value, $Validators = '') {
parent::__construct($ElementName, $ElementId, $Value, $Validators);
}
function __toString() {
return "<input type='file' id='$this->ElementId' name='$this->ElementName' />\r\n$this->Validators\r\n";
}
}
class ComboBoxControl extends HTMLControl {
	public $TextAndVals;
	public $Enabled = true;
	public function __construct($ElementName, $ElementId, $Value, $textandvals = null, $enabled = true, $Validators = '') {
		parent::__construct($ElementName, $ElementId, $Value, $Validators);
		$this->TextAndVals = $textandvals;
		$this->Enabled = $enabled;
	}
	function __toString() {
$dbind = getDataBind($this->Value);
		if($this->Enabled == true) {
			$html = "<select $dbind name='$this->ElementName' id='$this->ElementId'>";
			if($this->TextAndVals != null) {
				foreach($this->TextAndVals as $key => $val) {
					if($val == getValue($this->Value)) {
						$html .= "<option value='$val' selected>$key</option>";
					} else {
						$html .= "<option value='$val'>$key</option>";
					}
				}
			}
			return $html."</select>\r\n$this->Validators";
		} else {
			if($this->TextAndVals != null) {
				foreach($this->TextAndVals as $key => $val) {
					if($this->Value == $val) {
return "<span $dbind class='spanLabel' id='$this->ElementId'>$key</span>";
					}
				}
			}
return '';
		}
	}
}
class HiddenControl extends HTMLControl {
	public function __construct($ElementName, $ElementId, $Value) {
		parent::__construct($ElementName, $ElementId, $Value);
	}
	function __toString() {
$dbind = getDataBind($this->Value);
		return "<input $dbind type='hidden' id='$this->ElementId' name='$this->ElementName' value='$this->Value' />\r\n";
	}
}
class RadioGroupControl extends HTMLControl {
	public $TextAndVals;
	public $Enabled = true;
	public function __construct($ElementName, $ElementId, $Value, $textandvals = null, $enabled = true, $Validators = '') {
		parent::__construct($ElementName, $ElementId, $Value, $Validators);
		$this->TextAndVals = $textandvals;
		$this->Enabled = $enabled;
	}
	function __toString() {
		$html = '';
		if($this->Enabled == true) {
		       	if($this->TextAndVals != null) {
		       		foreach($this->TextAndVals as $key => $val) {
		       			$dbind = 'data-bind="checked: '.$this->Value->propertyName.'"';
			       		$html .= "<input type='radio' value='$val' $dbind style='float:left' /><label style='float:left' for='$this->ElementId$key'>$key</label>";
		        	}
		        }
		        $html .= "\r\n$this->Validators";
		} else {
			if($this->TextAndVals != null) {
				foreach($this->TextAndVals as $key => $val) {
					if($this->Value == $val) { $html = "<span id='$this->ElementName'>$key</span>"; break; }
				}
			}
		}
		return "$html\r\n";
	}
}
class EmailControl extends HTMLControl {
	public function __construct($ElementName, $ElementId, $Value, $Validators = '') {
		parent::__construct($ElementName, $ElementId, $Value, $Validators);
	}
	function __toString() {
$size = $this->Size > 0 ? "maxlength='$this->Size'" : '';
$val = getValue($this->Value);
$dbind = getDataBind($this->Value);
		return "<input $dbind type='email' id='$this->ElementId' $size name='$this->ElementName' value='$val' />\r\n$this->Validators\r\n";
	}
}
class CheckBoxControl extends HTMLControl {
	public $OnValue = '1';
	public $Enabled = true;
	public $Label = '';
	public function __construct($ElementName, $ElementId, $Value, $Label, $onvalue = '1', $enabled = true, $Validators = '') {
		parent::__construct($ElementName, $ElementId, $Value, $Validators);
		$this->OnValue = $onvalue;
		$this->Enabled = $enabled;
		$this->Label = $Label;
		$this->LabelOnLeft = false;
	}
	function __toString() {
		//if(getValue($this->Value) == $this->OnValue) $selected = 'CHECKED';
		if($this->Enabled == false) $enabled = 'DISABLED';
$dbind = 'data-bind="checked: '.$this->Value->propertyName.'() == \''.$this->OnValue.'\'"';
		return "<input $dbind type='checkbox' name='$this->ElementName' id='$this->ElementId' $enabled/>\r\n$this->Validators\r\n";
	}
}
class NumberControl extends HTMLControl {
	public $Min;
	public $Max;
	public function __construct($ElementName, $ElementId, $Value, $Min, $Max, $Validators = '') {
		parent::__construct($ElementName, $ElementId, $Value, $Validators);
		$this->Min = $Min;
		$this->Max = $Max;
	}
	function __toString() {
$val = getValue($this->Value);
$dbind = getDataBind($this->Value);
		return "<input $dbind type='number' name='$this->ElementName' id='$this->ElementId' min='$this->Min' max='$this->Max' value='$val' />\r\n$this->Validators\r\n";
	}
}
class PhoneControl extends HTMLControl {
public $Mask;
public function __construct($ElementName, $ElementId, $Value, $Mask, $Validators = '') {
parent::__construct($ElementName,$ElementId,$Value,$Validators);
$this->Mask = $Mask;
}
function __toString() {
$size = $this->Size > 0 ? "maxlength='$this->Size'" : '';
$val = getValue($this->Value);
$dbind = getDataBind($this->Value);
return "<input $dbind type='text' name='$this->ElementName' $size id='$this->ElementId' value='$val' /><script>\$('#$this->ElementId').mask('$this->Mask');</script>\r\n$this->Validators\r\n";
}
}
class DateControl extends HTMLControl {
	public $Format;
public $Mask;
	public function __construct($ElementName, $ElementId, $Value, $Format, $Mask, $Validators = '') {
		parent::__construct($ElementName, $ElementId, $Value, $Validators);
		$this->Format = $Format;
$this->Mask = $Mask;
	}
	function __toString() {
                $date = date($this->Format, strtotime(getValue($this->Value)));
$dbind= getDataBind($this->value);
		$html = "<input $dbind type='date' name='$this->ElementName' id='$this->ElementId' value='$date' />";
		//$html .= "<script>$('#$this->ElementId').mask('$this->Mask');$('#$this->ElementId').datepicker({dateFormat:'mm/dd/yy'});</script>";
		return "$html\r\n$this->Validators\r\n";
	}
}
class AreaControl extends HTMLControl {
	public $Columns;
	public $Rows;
	public function __construct($ElementName, $ElementId, $Value, $Columns = 40, $Rows = 4, $Validators = '') {
		parent::__construct($ElementName, $ElementId, $Value, $Validators);
		$this->Columns = $Columns;
		$this->Rows = $Rows;
		$this->TextAlignRight = false;
	}
	function __toString() {
$val = getValue($this->Value);
$dbind = getDataBind($this->Value);
		return "<textarea $dbind rows='$this->Rows' cols='$this->Columns' id='$this->ElementId' name='$this->ElementName'>$val</textarea>\r\n$this->Validators\r\n";
	}
}
class DateTimeControl extends HTMLControl {
	public $Format;
public $Mask;
	public function __construct($ElementName, $ElementId, $Value, $Format, $Mask, $Validators = '') {
		parent::__construct($ElementName, $ElementId, $Value, $Validators);
		$this->Format = $Format;
$this->Mask = $Mask;
	}
	function __toString() {
		$date = date($this->Format, strtotime(getValue($this->Value)));
$dbind = getDataBind($this->Value);
		$html = "<input $dbind type='datetime-local' name='$this->ElementName' id='$this->ElementId' value='$date' />\r\n";
		//$html .= "\r\n<script>$('#$this->ElementId').mask('$this->Mask');$('#$this->ElementId').datetimepicker({timeFormat: 'hh:mm TT',ampm: true });</script>\r\n$this->Validators\r\n";
		return $html;
	}
}
class SubmitButtonControl extends HTMLControl {
	public $Text;
	public $FormName;
	public function __construct($ElementName, $ElementId, $Value, $Text, $FormName) {
		parent::__construct($ElementName, $ElementId, $Value);
		$this->Text = $Text;
		$this->FormName = $FormName;
	}
	function __toString() {
	        $html = "<button type='submit' name='submit' id='$this->ElementId' value='submit-value'>$this->Text</button>\r\n";
        	$html .= "<script>$(function() { buttonEvent('#$this->ElementId', {$this->FormName}Validator, '$this->FormName'); });</script>\r\n";
        	return $html;
	}
}
class LabelForControl extends HTMLControl {
	public $Text;
	public $Control;
	public function __construct($Text, $Control = null) {
		$this->Text = $Text;
		$this->Control = $Control;
	}
	function __toString() {
		$html = ''; 
		if(is_object($this->Text)) {
			$text = $this->Text->propertyName;
			if($this->Text->Has('Label')) {
				$text = $this->Text->AttributeParams('Label');
				$text = $text[0];
			}
		} else {
			$text = $this->Text;
		}
		if(is_object($this->Control)) {
			$align = $this->Control->TextAlignRight ? '' : 'style="text-align:left"';
			$elementid = $this->Control->ElementId;
			if($this->Control->LabelOnLeft == true) {
				$html .= "<label class='leftLabel' $align for='$elementid'>$text</label>";
			}
			$html .= $this->Control.'';
			if($this->Control->LabelOnLeft == false) {
				$html = "<label class='leftLabel' style='visibility:hidden'>&nbsp;</label>$html<label for='$elementid' class='rightLabel'>$text</label></span>";
			}
		} else {
if($this->Control == null) {
$html .= "<span class='spanLabel'>$text</span>";
} else {
			$html .= "<label class='leftLabel'>$text</label><span class='spanLabel'>$this->Control</span>";
}
		}
		return "$html\r\n";
	}
}
class LabelControl extends HTMLControl {
public function __construct($ElementId, $Value) {
parent::__construct('',$ElementId, $Value);
}
function __toString() {
$propName = $this->Value->propertyName;
$dbind = "data-bind='text:$propName'";
$db = Config::$DbType;
if($db::IsDBDate(getValue($this->Value))) {
$value = date(Config::$DateFormat, strtotime($this->Value));
} else {
$value = getValue($this->Value);
}
return "<span $dbind class='spanLabel' id=$this->ElementId'>$value</span>";
}
}
class DivControl extends HTMLControl {
public function __construct($ElementId, $Value) {
parent::__construct('', $ElementId, $Value);
}
function __toString() {
$val = getValue($this->Value);
return "<div class='divLabel' id='$this->ElementId'>$val</div>";
}
}
class SpanControl extends HTMLControl {
public $Class;
	public function __construct($ElementId, $Value, $class = 'spanLabel') {
if(get_class($Value) == 'StaticProperty' && $Value->Has('Label')) {
$p = $Value->AttributeParams('Label');
$Value = $p[0];
}
		parent::__construct('', $ElementId, $Value);
$this->Class = $class;
	}
	function __toString() {
$val = getValue($this->Value);
		return "<span class='$this->Class' id='$this->ElementId'>$val</span>";
	}
}
?>