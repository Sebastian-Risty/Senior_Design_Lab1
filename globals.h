#pragma once
#include "common.h"

class globals {
public:
	HANDLE hSerial = INVALID_HANDLE_VALUE;

	enum struct MessageID : int16_t {
		NA = 0,
		setDisplay = 1,			// control display on/off
		setDisplayGamma = 2,		// control display gamma level 
		setBrightness = 3,   //control brightness level
	};

	// inputted from the arduino
	vector<float> tempData; 

	MessageID currentID = MessageID::NA;	// update this + any other data needed before calling writeData()
	bool enableLED = false;
	bool faren = false;
	int brightness = 200;
	int contrast = 75;

	bool connected = false;  // true when a BT connection is established

	string phoneNumber = "7125420966";
	string message = ":D";
	string carrier = "email.uscc.net";
};

inline globals g_globals;