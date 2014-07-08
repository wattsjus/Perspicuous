<?
class GuidUpdateAndCreate extends DataObject {
	/**
	Required
	Date
	Hidden
	*/
	public $CreateDate = null;
	/**
	Required
	Guid
	Hidden
	*/
	public $CreateUserId = '';
	/**
	Required
	Date
	Hidden
	*/
	public $UpdateDate = null;
	/**
	Required
	Guid
	Hidden
	*/
	public $UpdateUserId = '';
	/**
	Required
	RegEx("^[AI]$")
Hidden
	*/
	public $Status = 'A';
	public function PreSave() {
		$Key = get_called_class().'Id';
		$dbType = Config::$DbType;
		$currDate = date($dbType::$DBDateFormat, time());
		if(!isset($this->$Key->value) || $this->$Key->value == '') {
			$this->CreateUserId = $_SESSION[User]->UserId->value;
			$this->CreateDate = $currDate;
		}
		$this->UpdateUserId = $_SESSION[User]->UserId->value;
		$this->UpdateDate = $currDate;
	}
}
?>