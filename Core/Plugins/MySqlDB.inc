<?php
class MySqlDB implements DBType {
	public static $DB = null;
	public static $DBDateFormat = 'Y-m-d H:i:s';
	public static function IsDBDate($string) {
		return preg_match('/^(\d{4})(-)(\d{2})(-)(\d{2})/', $string);
	}
	public function FormatTableName($name) {
		return "`$name`";
	}
	public function FormatBinaryGuid($value) {
		return "unhex('$value')";
	}
	public function FormatDate($value) {
if($value == null || $value == '') return 'null';
		$d = date(self::$DBDateFormat, strtotime($value));
		return $this->FormatString($d);
	}
	public function FormatString($value) {
if($value == null || $value == '') return '\'\'';
		$value = self::GetDBObject()->real_escape_string($value);
if(is_numeric($value) && $value != '0') {
return $value;
} else {
		return "'$value'";
}
	}
	public function FormatField($name) {
		return "`$name`";
	}
	public function NewBinaryGuid() {
		$guid = md5(uniqid('',true));
		return $guid;
	}
	public function CompileSelect($fields, $table, $start = 0, $end = 0) {
		$sql = "SELECT $fields FROM ".$this->FormatTableName($table);
		if($start > 0 && $end > 0) {
			$sql .= $this->CompileLimit($start, $end);
		}
		return $sql;
	}
	public function CompileLimit($start, $end) {
		return "LIMIT $start, $end";
	}
	public function CompileOrderBy($Field, $Direction = 'ASC') {
		return 'ORDER BY '.$this->FormatField($Field) . " $Direction";
	}
	public function CompileWhere($PKName, $PKValue) {
		return 'WHERE '.$this->FormatField($PKName)." = $PKValue";
	}
	public function CompileWhereIn($PKName, $PKValues) {
		return 'WHERE '.$this->FormatField($PKName)." IN($PKValues)";
	}
	public function CompileInsert($fields, $values, $table) {
		return 'INSERT INTO '. $this->FormatTableName($table). " ($fields) VALUES($values)";
	}
	public function CompileUpdate($sets, $PKName, $PKValue, $table) {
		return 'UPDATE '.$this->FormatTableName($table). " SET $sets " . $this->CompileWhere($PKName, $PKValue);
	}
	public function CompileDelete($PKName, $PKValue, $table) {
		return 'DELETE FROM '.$this->FormatTableName($table).' '.$this->CompileWhere($PKName, $PKValue);
	}
	public function CompileDeleteIn($PKName, $PKValues, $table) {
		return 'DELETE FROM '.$this->FormatTableName($table).' WHERE ' .$this->FormatField($PKName)." IN ($PKValues)";
	}
	public static function GetDBObject() {
		if(self::$DB == null) {
			self::$DB = new mysqli(Config::$Host, Config::$UserName, Config::$Password, Config::$Database);
			if(mysqli_connect_errno()) {
				die("Connection failed: " . mysqli_connect_error());
			}
		}
		return self::$DB;
	}
	public function Query($sql) {
$res = self::GetDBObject()->query($sql) or die(self::GetDBObject()->error);
		return $res ;
	}
}
?>