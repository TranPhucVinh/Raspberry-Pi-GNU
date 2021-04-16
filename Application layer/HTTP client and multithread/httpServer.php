<?php
$DB_HOSTNAME = "localhost";
$DB_USERNAME = "rpi_test";
$DB_PASSWD = "123456";
$DB_NAME = "rpi_test";

date_default_timezone_set("UTC");
$date_time = gmdate('Y-m-d H:i:s'); //i: minute

try {
	$conn = new PDO("mysql:host=$DB_HOSTNAME;dbname=$DB_NAME",$DB_USERNAME,$DB_PASSWD);
	$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

	if(isset($_POST["serialnumber"]) && isset($_POST["frame"]) && isset($_POST["data"])){
		
		//Send data to HTTP client
		//echo "serialnumber=".$_POST["serialnumber"].";frame =".$_POST["frame"].";data =".$POST["data"];
		$sql_INSERT = "INSERT INTO `device` (";
		$sql_INSERT = $sql_INSERT."`SERIALNUMBER`,`FRAME`, `DATA`, `DATETIME`) VALUES (";
		$sql_INSERT = $sql_INSERT."\"".$_POST["serialnumber"]."\",";
		$sql_INSERT = $sql_INSERT."\"".$_POST["frame"]."\",";
		$sql_INSERT = $sql_INSERT."\"".$_POST["data"]."\",";
		$sql_INSERT = $sql_INSERT."\"".$date_time."\")";

		//Execute SQL, can be execute in one line
		$stmt = $conn->query($sql_INSERT);
		//----------------
		$sql_SELECT = "SELECT * FROM `device_status` WHERE `SERIALNUMBER`= \"";
		$sql_SELECT = $sql_SELECT.$_POST["serialnumber"]."\"";
		echo $sql_SELECT;
		if($stmt = $conn->query($sql_SELECT)->fetch()){
			//update data
			$sql_UPDATE = "UPDATE `device_status` SET ";
			$sql_UPDATE = $sql_UPDATE."`FRAME`=\"".$_POST["frame"]."\",";
			$sql_UPDATE = $sql_UPDATE."`DATA`=\"".$_POST["data"]."\",";
			$sql_UPDATE = $sql_UPDATE."`WHERE `SERIALNUMBER`=\"".$_POST["serialnumber"]."\",";
			//thuc thu lenh SQL
			$stmt = $conn->query($sql_UPDATE);
		}
		else {
			//insert data
			$sql_INSERT = "INSERT INTO `device` (";
			$sql_INSERT = $sql_INSERT."`SERIALNUMBER`,`FRAME`, `DATA`) VALUES (";
			$sql_INSERT = $sql_INSERT."\"".$_POST["serialnumber"]."\",";
			$sql_INSERT = $sql_INSERT."\"".$_POST["frame"]."\",";
			$sql_INSERT = $sql_INSERT."\"".$_POST["data"]."\")";
			//Execute SQL
			$stmt = $conn->query($sql_INSERT);
		}
		echo "OK";
	}
	else {
		echo "ERROR";
	}
}
catch(PDOException $e){
	echo $sql."<br>".$e->getMessage();
}

// echo "OK";
?>
