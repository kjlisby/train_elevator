// Start() is called by the HTML page on loading
function Start() {
	InitWS();
	GetStatus();
}

// Initialize the WEB socket for receiving events
var Socket;
function InitWS() {
	Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
	document.getElementById("logConsole").value += 'Listening on ws://' + window.location.hostname + ':81/';
	Socket.onmessage = function(evt) {
		document.getElementById("logConsole").value += evt.data;
		UpdatePage(evt.data);
	};
}

// Enable and refresh the calibration fields and buttons for a new level
function EnableCalibration(actualLevel) {
	for (let i = 1; i <= 12; i++) {
		const elems = document.getElementsByClassName("cal_"+i);
		for (let x of elems) {
			x.style.visibility="hidden";
		}
	}
	const elems = document.getElementsByClassName("cal_"+actualLevel);
	for (let x of elems) {
		x.style.visibility="visible";
	}
	GetCalibration(actualLevel);
}

// Get the stored calibration data for a level
function GetCalibration(level) {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			UpdatePage(this.responseText);
		}
	};
	xhttp.open("GET", "ajax_get_calibration_"+level, true);
	xhttp.send();
}

// Save the values in the calibration input fields
function SaveCalibration(level, leftValue, rightValue) {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			UpdatePage(this.responseText);
		}
	};
	xhttp.open("GET", "ajax_set_calibration_"+level+"_"+leftValue+"_"+rightValue, true);
	xhttp.send();
}

// Move the elevator to a new level using already stored calibration data
function MoveElevator(level) {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			UpdatePage(this.responseText);
		}
	};
	xhttp.open("GET", "ajax_set_level_"+level, true);
	xhttp.send();
}

// Move the elevator to the absolute step values in the calibration input fields
function MoveElevatorDuringCalibration(level, leftValue, rightValue) {
	var xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			UpdatePage(this.responseText);
		}
	};
	xhttp.open("GET", "ajax_set_levelfromcalibration_"+level+"_"+leftValue+"_"+rightValue, true);
	xhttp.send();
}

// Poll the server for status every second to keep up to date - in case an event is missed
function GetStatus() {
	var request = new XMLHttpRequest();
	request.onreadystatechange = function(){
		if(this.readyState == 4 && this.status == 200) {
		  UpdatePage(this.responseText);
		}
	};
	request.open("GET", "ajax_getstatus&random="+Math.random()*1000000,true);
	request.send();
	setTimeout('GetStatus()',1000);
}

var CurrentLevel = 0;
// Handle events as well as responses to various commands
function UpdatePage(jsonString) {
	var i;
	var obj_list = JSON.parse(jsonString);
	for (i in obj_list.objects) {
		if (obj_list.objects[i].item == "STATUS") {
			display = document.getElementById("OLED_copy");
			if (obj_list.objects[i].value == "BLOCKED") {
				display.style.fontsize="5.5vw";
				display.value="&#13;&#10;&#13;&#10;Blocked&#13;&#10;at "+CurrentLevel;
			} else if (obj_list.objects[i].value == "HOMING1") {
				CurrentLevel = 0;
				display.style.fontsize="5.5vw";
				display.value="&#13;&#10;&#13;&#10;Homing&#13;&#10;phase 1";
			} else if (obj_list.objects[i].value == "HOMING2") {
				CurrentLevel = 0;
				display.style.fontsize="5.5vw";
				display.value="&#13;&#10;&#13;&#10;Homing&#13;&#10;phase 2";
			} else if (obj_list.objects[i].value == "HOMING3") {
				CurrentLevel = 0;
				display.style.fontsize="5.5vw";
				display.value="&#13;&#10;&#13;&#10;Homing&#13;&#10;phase 3";
			} else if (obj_list.objects[i].value == "HOMING4") {
				CurrentLevel = 0;
				display.style.fontsize="5.5vw";
				display.value="&#13;&#10;&#13;&#10;Homing&#13;&#10;phase 4";
			} else if (obj_list.objects[i].value == "MOVING") {
				CurrentLevel = 0;
				display.style.fontsize="5.5vw";
				var moveFrom = obj_list.objects[i+1].value;
				var moveTo   = obj_list.objects[i+2].value;
				if (MoveTo > moveFrom) {
					display.value=MoveFrom+"&#13;&#10;&#8648;&#13;&#10;"+MoveTo;
				} else {
					display.value=MoveFrom+"&#13;&#10;&#8650;&#13;&#10;"+MoveTo;
				}
			} else if (obj_list.objects[i].value == "IDLE") {
				display.style.fontsize="8.5vw";
				display.value=obj_list.objects[i+1].value;
				if (obj_list.objects[i+1].value != CurrentLevel) {
					CurrentLevel = obj_list.objects[i+1].value;
					EnableCalibration(CurrentLevel);
				}
			} else {
				// unknown status
			}
		} else if (obj_list.objects[i].item == "CAL_LEVEL") {
			if (obj_list.objects[i+1].value == CurrentLevel) {
				document.getElementById("cali_left_"+CurrentLevel).value = obj_list.objects[i+2].value;
				document.getElementById("cali_rightt_"+CurrentLevel).value = obj_list.objects[i+3].value;
			}
		} else {
			// unknown command
		}
	} /* list iteration */
} /* function UpdatePage */
