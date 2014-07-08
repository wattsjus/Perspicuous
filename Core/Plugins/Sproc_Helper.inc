<?
class Sproc_Helper {
	function Execute($sproc, $params=null,$return_type=null) {
		$db = DataObject::GetDBObject();
		$prepare_statement = "CALL $sproc(";
		if($params != null) {
		$temp_params = array();
			foreach($params as $key => $val) {
			    $temp_params[] = "@$key:=".$db->FormatString($val);
			}
			$prepare_statement .= implode(',',$temp_params);
		}
		$results = $db->Query($prepare_statement.')');
//echo $prepare_statement.')';
		if($return_type == null) {
			$aret = array();
			for($i = 0; $i < $results->num_rows; $i++) {
				$row = $results->fetch_assoc();
				foreach($row as $key => $val) {
					$ret->$key = $val;
				}
				$aret[] = $ret;
unset($ret);
			}
		} else {
			$aret = new DataObjectArray();
			for($i = 0; $i < $results->num_rows; $i++) {
				$row = $results->fetch_object($return_type);
				$row->NewToOld();
				$row = DataObject::UnpackResult($row);
				$aret[] = $row;
			}
		}
		$results->close();
		$db::$DB->next_result();
		return $aret;
	}
}
?>