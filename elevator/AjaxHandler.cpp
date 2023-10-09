#include "AjaxHandler.h"
#include "PosLogic.h"
#include "Calibrator.h"

// The AjaxHandler is responding to commands sent via HTTP requests by the JavaScript code that is executed in a browser somewhere.
// There is no event mechanism. Just simple commands:
//   - read the status of everything
//   - set a turnout
//   - adjust power
//   - etc.

extern PosLogic   *PL;
extern Calibrator *CA;

#include "JSONArray.h"

class InternalAjaxHandler : public RequestHandler {
	bool canHandle(HTTPMethod method, String uri) {
		return uri.startsWith("/ajax_");
	}

	String GetCalibration(int level) {
		return JSON_ArrayStart()+JSON_item("CAL_LEVEL",String(level))+
		       JSON_ArrayDivider()+JSON_item("LEFT", String(CA->GetOffset(true, level)))+
		       JSON_ArrayDivider()+JSON_item("RIGHT",String(CA->GetOffset(false,level)))+
		       JSON_ArrayEnd();
	}

	String SetCalibration(int level, long leftOffset, long rightOffset) {
		CA->SetOffset(level,leftOffset,rightOffset);
		CA->WriteToSD();
		return JSON_ArrayStart()+JSON_item("CAL_SET","OK")+JSON_ArrayEnd();
	}

	String MoveToLevel(int level) {
		PL->MoveTo(level,0);
		return PL->GetStatus();
	}

	String MoveToLevelFromCalibration(int level, long leftSteps, long rightSteps) {
		PL->MoveToSteps(level,leftSteps,rightSteps);
		return PL->GetStatus();
	}

	bool handle(WebServer& server, HTTPMethod requestMethod, String requestUri) {   
		Serial.println("AJAX GETINPUT");
		Serial.println(requestUri);

		String cmdarray[10];
		int index = 0;
		int startindex = 0;
		int cmd_length = 0;
		while (index < requestUri.length()) {
			if (requestUri[index] == '_') {
				cmdarray[cmd_length] = requestUri.substring(startindex, index);
				startindex = index+1;
				cmd_length++;
			}
			index++;
		}
		cmdarray[cmd_length] = requestUri.substring(startindex);
		index++;
		cmd_length++;

		for (int i=0; i<cmd_length; i++) {
			Serial.print(i); Serial.print(": ");
			Serial.println(cmdarray[i]);
		}

		if (cmdarray[1].startsWith("getstatus")) {
			server.send(200, "text/plain", PL->GetStatus());
		} else if (cmdarray[1].equals("get")) {
				if (cmdarray[2].equals("calibration")) {
					server.send(200, "text/plain", GetCalibration(cmdarray[3].toInt()));
				} else {
					server.send(404, "text/plain", "Unknown HTLM element for get command");
				}
		} else if (cmdarray[1].equals("set")) {
			if (cmdarray[2].equals("calibration")) {
				server.send(200, "text/plain", SetCalibration(cmdarray[3].toInt(), cmdarray[4].toInt(), cmdarray[5].toInt()));
			} else if (cmdarray[2].equals("level")) {
				server.send(200, "text/plain", MoveToLevel(cmdarray[3].toInt()));
			} else if (cmdarray[2].equals("levelfromcalibration")) {
				server.send(200, "text/plain", MoveToLevelFromCalibration(cmdarray[3].toInt(), cmdarray[4].toInt(), cmdarray[5].toInt()));
			} else {
				server.send(404, "text/plain", "Unknown HTLM element for set command");
			}
		} else {
			server.send(404, "text/plain", "Unknown AJAX command");
		}
	}
} internalAjaxHandler;


void AjaxHandler::Loop() {
}

bool AJAXINITIALIZED = false;
void AjaxHandler::Init(WebServer *server) {
	if (AJAXINITIALIZED) return;
	AJAXINITIALIZED = true;
	server->addHandler(&internalAjaxHandler);
}
