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
        $sql_SELECT = "SELECT * FROM device WHERE `SERIALNUMBER`<> ''";
        $stmt = $conn->query($sql_SELECT);$stmt->setFetchMode(PDO::FETCH_ASSOC);
        $n = $stmt->rowCount();
        echo "{\"DEV\":[";
        if ($n>0){
            while($row = $stmt->fetch()){
                echo "{\"SN\":\"".$row["SERIALNUMBER"]."\",\"DATA
                \":\"".$row["DATA"]."\",\"NAME\":\"".$row["NAME"]."\",\"FRAME
                \":\"".$row["FRAME"]."\",\"DATETIME\":\"".$row["DATETIME"]."\"}";
                $n--;
                if ($n>0) echo ",";
            }
        }
        else {
            echo "\"\"";
        }
        echo "]}";
    }
    catch(PDOException $e)
    {
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
    catch(PDOException $e)
    {
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
<script type="text/javascript">
    var timer = setInterval(function(){loadTime();},1000);
    function loadTime(){
        var date = Date();var date_array = date.split(" ");
        date = date_array[0] + " " + date_array[1] + " " + date_array[2] + " " + date_array[3];
        document.getElementById("localTime").innerHTML = "<b>Local Time: </b>" + date;
    }
    var xmlHTTP;
    var xmlStr = "";
    var xmlStrReady = 0;
    var myTimeout = setTimeout(function(){loadXMLDoc();},100);
    function loadXMLDoc(){
        if(window.XMLHttpRequest){//code for IE7+, Chrome, Firefox, Opera, Safari
            xmlHTTP = new XMLHttpRequest();
        }
        else {//code for IE6, IE5
            xmlHTTP = new ActiveXObject("Microsoft.xmlHTTP");
        }
        xmlHTTP.onreadystatechange = function(){
            if (xmlHTTP.readyState == 4 && xmlHTTP.status ==200){
                xmlStr = xmlHTTP.responseText;
                xmlStrReady = 1;
                //xu ly du lieu
                //------------
                //document.getElementById("debug").innerHTML= xmlStr;
                var data = JSON.parse(xmlStr);
                var table = "<table border=1>\
                <tr><th>SN</th>\
                <th>NAME</th>\
                <th>FRAME</th>\
                <th>DATA</th>\
                <th>DATE TIME</th>\
                <th>COMMAND</th>\
                </tr>";
                for (i=0;i<data.DEV.length;i++){
                    table = table + "<tr>\
                    <td>" + data.DEV[i].SN + "</td>\
                    <td>" + data.DEV[i].NAME + "</td>\
                    <td>" + data.DEV[i].FRAME + "</td>\
                    <td>" + data.DEV[i].DATA + "</td>\
                    <td>" + data.DEV[i].DATETIME + "</td>\
                    <td> <form method=POST role=form action=index.php><button type=submit name=ON value=" + data.DEV[i].SN+ "> ON</button></form>\
                    </td> </tr>";
                }
                document.getElementById("idDevice").innerHTML=table + "</table>";
                setTimeout(loadXMLDoc,10000); //call loadXMLDoc after 10 second so there won't be recursion
            }
        }
        xmlHTTP.open("POST", "./index.php", true);
        xmlHTTP.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
        xmlHTTP.send("request=1");
    }
</script>
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