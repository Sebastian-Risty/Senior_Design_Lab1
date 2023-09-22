#pragma once
#include "common.h"

class globals {
public:
	HANDLE hSerial = INVALID_HANDLE_VALUE;

	enum struct MessageID : int16_t {
		NA = 0,
		setDisplay = 1,			// control display on/off
		setDisplayGamma = 2		// control display gamma level 
	};

	// inputted from the arduino
	vector<float> tempData; 

	// tempData but shifted by in order of most to least recent and shifted by tempIndex
	float finalTempData[300] = {-127};

	// the index in tempData that points to the oldest data point
	int tempIndex = 0;

	MessageID currentID = MessageID::NA;	// update this + any other data needed before calling writeData()
	bool enableLED = true;
	bool faren = false;

	bool connected = false;  // true when a BT connection is established
};

inline globals g_globals;