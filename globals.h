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
	int16_t brightness = 200;
	int16_t contrast = 75;

	bool connected = false;  // true when a BT connection is established
};

inline globals g_globals;