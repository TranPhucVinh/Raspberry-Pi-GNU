var timer = setInterval(function(){loadTime();},1000);
var myTimeout = setTimeout(function(){loadXMLDoc();},100);
var xmlHTTP;
var xmlStr = "";
var xmlStrReady = 0;

function loadTime(){
	var date = Date();
	var date_array = date.split(" ");
	date = date_array[0] + " " + date_array[1] + " " + date_array[2] + " " + date_array[3];
	document.getElementById("localTime").innerHTML = "<b>Local Time: </b>" + date;
}

function loadXMLDoc(){
	if(window.XMLHttpRequest){//code for IE7+, Chrome, Firefox, Opera, Safari
		xmlHTTP = new XMLHttpRequest();
	}
	else {//code for IE6, IE5
		xmlHTTP = new ActiveXObject("Microsoft.xmlHTTP");
	}
	xmlHTTP.onreadystatechange = function(){
		if (xmlHTTP.readyState == 4 && xmlHTTP.status == 200){
			xmlStr = xmlHTTP.responseText;
			xmlStrReady = 1;
			//document.getElementById("debug").innerHTML = xmlStr;
			var data = JSON.parse(xmlStr);
			var table = "<table border=1>\
			<tr><th>SN</th>\
			<th>NAME</th>\
			<th>FRAME</th>\
			<th>DATA</th>\
			<th>DATE TIME</th>\
			<th>STATE</th>\
			<th>COMMAND</th>\
			<th>SET NAME</th>\
			<th>SET DELAY</th>\
			</tr>";
			for (i=0;i<data.DEV.length;i++){
				table = table + "<tr>\
				<td>" + data.DEV[i].SN + "</td>\
				<td>" + data.DEV[i].NAME + "</td>\
				<td>" + data.DEV[i].FRAME + "</td>\
				<td>" + data.DEV[i].DATA + "</td>\
				<td>" + data.DEV[i].DATETIME + "</td>\
				<td>" + data.DEV[i].Command + "</td>\
				<td> <form method=POST role=form action=index.php><button type=submit name=ON value=" + data.DEV[i].SN + "> ON</button></form>\
				<form method=POST role=form action=index.php><button type=submit name=OFF value=" + data.DEV[i].SN + "> OFF</button></form>\
				</td>\
				<td> <form method=POST role=form action=index.php><input type=text name=NAME placeholder ='Name of device'>\
				<button type=submit name=SETNAME value=" + data.DEV[i].SN + "> SAVE</button></form>\
				</td>\
				<td> <form method=POST role=form action=index.php><input type=number name=DELAY placeholder ='Delay for blink'>\
				<button type=submit name=SETDELAY value=" + data.DEV[i].SN + "> SAVE</button></form>\
				</td>\
				</tr>";
			}
			document.getElementById("idDevice").innerHTML = table + "</table>";
			setTimeout(loadXMLDoc,10000); //call loadXMLDoc after 10 second so there won't be recursion
		}
	}
	xmlHTTP.open("POST", "./index.php", true);
	xmlHTTP.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
	xmlHTTP.send("request=1");
}