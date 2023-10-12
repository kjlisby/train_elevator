// Start() is called by the HTML page on loading
function Start() {
	document.getElementById("logConsole").value = 'HEJ MOR';
	InitWS();
	document.getElementById("logConsole").value += ' efter InitWS';
	GetStatus();
}

// Initialize the WEB socket for receiving events
var Socket;
function InitWS() {
	Socket = new WebSocket('ws://' + window.location.hostname + ':81/');
//	Socket = new WebSocket('ws://192.168.1.201:81/');
	document.getElementById("logConsole").value += 'Listening on ws://' + window.location.hostname + ':81/';
	Socket.onmessage = function(evt) {
		document.getElementById("logConsole").value += evt.data;
		UpdatePage(evt.data);
	};
}

// Enable and refresh the calibration fields and buttons for a new level
function EnableCalibration(actualLevel) {
	document.getElementById("logConsole").value += ' EnableCalibration';
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
	document.getElementById("logConsole").value += ' GetCalibration';
	let xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			UpdatePage(this.responseText);
		}
	};
	document.getElementById("logConsole").value += ' GetCalibration 2';
	xhttp.open("GET", "ajax_get_calibration_"+level, true);
	document.getElementById("logConsole").value += ' GetCalibration 3';
	xhttp.send();
}

// Save the values in the calibration input fields
function SaveCalibration(level) {
	document.getElementById("logConsole").value += ' SaveCalibration';
	let xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			UpdatePage(this.responseText);
		}
	};
	let leftValue  = document.getElementById("cali_left_"+level).value;
	let rightValue = document.getElementById("cali_right_"+level).value;
	xhttp.open("GET", "ajax_set_calibration_"+level+"_"+leftValue+"_"+rightValue, true);
	xhttp.send();
}

// Move the elevator to a new level using already stored calibration data
function MoveElevator(level) {
	document.getElementById("logConsole").value += ' MoveElevator';
	let xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			UpdatePage(this.responseText);
		}
	};
	xhttp.open("GET", "ajax_set_level_"+level, true);
	xhttp.send();
}

// Move the elevator to the absolute step values in the calibration input fields
function MoveElevatorDuringCalibration(level) {
	document.getElementById("logConsole").value += ' MoveElevatorDuringCalibration';
	let xhttp = new XMLHttpRequest();
	xhttp.onreadystatechange = function() {
		if (this.readyState == 4 && this.status == 200) {
			UpdatePage(this.responseText);
		}
	};
	let leftValue  = document.getElementById("cali_left_"+level).value;
	let rightValue = document.getElementById("cali_right_"+level).value;
	xhttp.open("GET", "ajax_set_levelfromcalibration_"+level+"_"+leftValue+"_"+rightValue, true);
	xhttp.send();
}

// Poll the server for status every second to keep up to date - in case an event is missed
function GetStatus() {
	document.getElementById("logConsole").value += ' GetStatus';
	let request = new XMLHttpRequest();
	request.onreadystatechange = function(){
		if(this.readyState == 4 && this.status == 200) {
		  UpdatePage(this.responseText);
		}
	};
	request.open("GET", "ajax_getstatus&random="+Math.random()*1000000,true);
	request.send();
	document.getElementById("logConsole").value += ' GetStatus SENT';
	setTimeout('GetStatus()',20000);
}

var CurrentLevel = 0;
// Handle events as well as responses to various commands
function UpdatePage(jsonString) {
	document.getElementById("logConsole").value += ' UpdatePage';
	let obj_list = JSON.parse(jsonString);
	for (let i = 0; i < obj_list.objects.length; i++) {
		if (obj_list.objects[i].item == "STATUS") {
			document.getElementById("logConsole").value += ' STATUS';
			display = document.getElementById("OLED_copy");
			if (obj_list.objects[i].value == "BLOCKED") {
				document.getElementById("logConsole").value += ' BLOCKED';
				display.style.fontSize="5.5vw";
				display.value='\u000D'+'\u000A'+'\u000D'+'\u000A'+'Blocked'+'\u000D'+'\u000A'+'at '+CurrentLevel;
			} else if (obj_list.objects[i].value == "HOMING1") {
				document.getElementById("logConsole").value += ' HOMING1';
				CurrentLevel = 0;
				display.style.fontSize="5.5vw";
				display.value='\u000D'+'\u000A'+'\u000D'+'\u000A'+'Homing'+'\u000D'+'\u000A'+'phase 1';
			} else if (obj_list.objects[i].value == "HOMING2") {
				document.getElementById("logConsole").value += ' HOMING2';
				CurrentLevel = 0;
				display.style.fontSize="5.5vw";
				display.value='\u000D'+'\u000A'+'\u000D'+'\u000A'+'Homing'+'\u000D'+'\u000A'+'phase 2';
			} else if (obj_list.objects[i].value == "HOMING3") {
				document.getElementById("logConsole").value += ' HOMING3';
				CurrentLevel = 0;
				display.style.fontSize="5.5vw";
				display.value='\u000D'+'\u000A'+'\u000D'+'\u000A'+'Homing'+'\u000D'+'\u000A'+'phase 3';
			} else if (obj_list.objects[i].value == "HOMING4") {
				document.getElementById("logConsole").value += ' HOMING4';
				CurrentLevel = 0;
				display.style.fontSize = "5.5vw";
				display.value='\u000D'+'\u000A'+'\u000D'+'\u000A'+'Homing'+'\u000D'+'\u000A'+'phase 4';
			} else if (obj_list.objects[i].value == "MOVING") {
				document.getElementById("logConsole").value += ' MOVING';
				CurrentLevel = 0;
				document.getElementById("logConsole").value += ' MOVING2';
				display.style.fontSize="5.5vw";
				document.getElementById("logConsole").value += ' MOVING3';
				let moveFrom = obj_list.objects[i+1].value;
				document.getElementById("logConsole").value += ' MOVING4';
				let moveTo   = obj_list.objects[i+2].value;
				document.getElementById("logConsole").value += ' MOVING5';
				if (moveTo > moveFrom) {
					document.getElementById("logConsole").value += ' MOVING6';
					display.value = moveTo+'\u000D'+'\u000A'+'\u21C8'+'\u000D'+'\u000A'+moveFrom;
				} else {
					document.getElementById("logConsole").value += ' MOVING7';
					display.value = moveFrom+'\u000D'+'\u000A'+'\u21CA'+'\u000D'+'\u000A'+moveTo;
				}
				document.getElementById("logConsole").value += ' MOVING8';
			} else if (obj_list.objects[i].value == "IDLE") {
				document.getElementById("logConsole").value += ' IDLE';
				display.style.fontSize = "10vw";
				document.getElementById("logConsole").value += ' IDLE1';
				document.getElementById("logConsole").value += ' '+obj_list.objects[i+1].value;
				display.value = obj_list.objects[i+1].value;
				document.getElementById("logConsole").value += ' IDLE2';
				if (obj_list.objects[i+1].value != CurrentLevel) {
					document.getElementById("logConsole").value += ' IDLE3';
					CurrentLevel = obj_list.objects[i+1].value;
					document.getElementById("logConsole").value += ' IDLE4';
					EnableCalibration(CurrentLevel);
					document.getElementById("logConsole").value += ' IDLE5';
				}
			} else {
				// unknown status
			}
		} else if (obj_list.objects[i].item == "CAL_LEVEL") {
			document.getElementById("logConsole").value += ' CAL_LEVEL';
			document.getElementById("logConsole").value += ' '+obj_list.objects[i].value;
			document.getElementById("logConsole").value += ' '+CurrentLevel;
			if (obj_list.objects[i].value == CurrentLevel) {
				document.getElementById("logConsole").value += ' CAL_LEVEL 2';
				document.getElementById("cali_left_"+CurrentLevel).value = obj_list.objects[i+1].value;
				document.getElementById("logConsole").value += ' CAL_LEVEL 3';
				document.getElementById("cali_right_"+CurrentLevel).value = obj_list.objects[i+2].value;
				document.getElementById("logConsole").value += ' CAL_LEVEL 4';
			}
		} else {
			// unknown command
		}
	} /* list iteration */
	document.getElementById("logConsole").value += ' END OF UpdatePage';
} /* function UpdatePage */
