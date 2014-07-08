<?
class TableSession {
public $TimeOut = 300; //default of five minutes
	public function __get($name) {
		$name = DataObject::GetDBObject()->FormatString($name);
		$result = DataObject::GetDBObject()->Query("SELECT `Value`, `ExpireDate` FROM `Session` WHERE `Key` = $name AND `ExpireDate` > '".date( 'Y-m-d H:i:s')."'");
		$row = $result->fetch_assoc();
		if(isset($row['Value'])) {
			if(strtotime($row['ExpireDate']) > strtotime(date('Y-m-d H:i:s'))) {
				$result = unserialize(gzuncompress($row['Value']));
if(method_exists($result, 'ReNew')) $result->ReNew();
return $result;
			} else {
				DataObject::GetDBObject()->Query("DELETE FROM `Session` WHERE `Key` = $name");
			}
		}
		return null;
	}
	public function __set($name, $value) {
		$name = DataObject::GetDBObject()->FormatString($name);
		$value = DataObject::GetDBObject()->FormatString(gzcompress(serialize($value),9));
		$result = DataObject::GetDBObject()->Query("SELECT COUNT(*) C FROM `Session` WHERE `key` = $name");
$row = $result->fetch_assoc();
		if($row['C'] > 0) {
			DataObject::GetDBObject()->Query("UPDATE `Session` SET `Value` = $value, `ExpireDate` = NOW() + INTERVAL $this->TimeOut SECOND WHERE `Key` = $name");
		} else {
DataObject::GetDBObject()->Query("INSERT INTO `Session` (`Value`, `Key`, `ExpireDate`) VALUES($value,$name, NOW() + INTERVAL $this->TimeOut SECOND)");
}
	}
}
?>