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

	vector<float> tempData;
	float finalTempData[300] = { 0 };
	int tempIndex = -1;

	MessageID currentID = MessageID::NA;	// update this + any other data needed before calling writeData()
	bool enableLED = true;
	bool faren = false;
};

inline globals g_globals;