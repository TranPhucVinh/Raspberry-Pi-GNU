<?php
$servername = "localhost";
$username = "rpi_test";
$password = "123456";


if(isset($_POST["request"])){
	try {
		$conn = new PDO("mysql:host=$servername;dbname=rpi_test", $username, $password);
		// set the PDO error mode to exception
		$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
		//echo "Connection initiated";
		$sql_SELECT = "SELECT * FROM device WHERE `SERIALNUMBER` <> ''";
		$stmt = $conn->query($sql_SELECT);
		$stmt->setFetchMode(PDO::FETCH_ASSOC);
		$n = $stmt->rowCount();
		echo "{\"DEV\":[";
		if ($n>0){
			while($row = $stmt->fetch()){
			// send JSON data-type to client
				echo "{\"SN\":\"".$row["SERIALNUMBER"]."\",\"DATA\":\"".$row["DATA"]."\",\"NAME\":\"".$row["NAME"]."\",\"FRAME\":\"".$row["FRAME"]."\",\"DATETIME\":\"".$row["DATETIME"]."\",\"Command\":\"".$row["Command"]."\"}";
				$n--;
				if ($n>0) echo ",";
			}
		}
		else {
			echo "\"\"";
		}
		echo "]}";
	}
	catch(PDOException $e){
		echo "Connection failed: " . $e->getMessage();
	}
	exit(); // them exit() de khong tra ve trang HTML
}

if (isset($_POST["ON"])){
	$dev_SN = $_POST["ON"];
	try{
		$conn = new PDO("mysql:host=$servername;dbname=rpi_test", $username, $password);
    	// set the PDO error mode to exception
    	$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    	$sql_UPDATE = "UPDATE `device` SET `Command`='1' WHERE `SERIALNUMBER` = '".$dev_SN."'";
    	$stmt = $conn->query($sql_UPDATE);
	}
	catch(PDOException $e){
	    echo "Connection failed: " . $e->getMessage();
    }
}

if (isset($_POST["OFF"])){
	$dev_SN = $_POST["OFF"];
	try{
		$conn = new PDO("mysql:host=$servername;dbname=rpi_test", $username, $password);
    	// set the PDO error mode to exception
    	$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    	$sql_UPDATE = "UPDATE `device` SET `Command`='0' WHERE `SERIALNUMBER` = '".$dev_SN."'";
    	$stmt = $conn->query($sql_UPDATE);
	}
	catch(PDOException $e){
	    echo "Connection failed: " . $e->getMessage();
    }
}

if (isset($_POST["SETNAME"])){
	$dev_SN = $_POST["SETNAME"];
	$dev_NAME = $_POST["NAME"];
	try{
		$conn = new PDO("mysql:host=$servername;dbname=rpi_test", $username, $password);
    	// set the PDO error mode to exception
    	$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    	$sql_UPDATE = "UPDATE `device` SET `NAME`='".$dev_NAME."WHERE `SERIALNUMBER`='".$dev_SN."'";
    	$stmt = $conn->query($sql_UPDATE);
	}
	catch(PDOException $e){
    	echo "Connection failed: " . $e->getMessage();
    }
}

if (isset($_POST["SETDELAY"])){
	$dev_SN = $_POST["SETDELAY"];
	$dev_DELAY = $_POST["DELAY"];
	echo $dev_DELAY;
	try{
		$conn = new PDO("mysql:host=$servername;dbname=rpi_test", $username, $password);
    	// set the PDO error mode to exception
    	$conn->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    	$sql_UPDATE = "UPDATE `device` SET `Command`='".$dev_DELAY."' WHERE `SERIALNUMBER`='".$dev_SN."'";
    	$stmt = $conn->query($sql_UPDATE);
	}
	catch(PDOException $e){
	    echo "Connection failed: " . $e->getMessage();
    }
}
?>

<!DOCTYPE html>
<html>
	<style>
	h1 {
		text-align: center;
	}
	</style>
	<script src="script.js"></script>

	<head>
		<title>Raspberry</title>
		<meta charset="utf-8">

	</head>
	<body> <!--This is comment -->
		<h1>Header 1</h1>
		<h2>Header 2</h2>
	<font color= "blue" size = "14"> Dashboard </font>
	<hr>
	<div id = "localTime"></div>
	<b>
	<div id = "idDevice"></div>
	<div id = "debug"></div>
	</body>
</html>