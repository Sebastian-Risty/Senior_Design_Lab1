#pragma once
#include "common.h"

class globals {
public:
	HANDLE hSerial = INVALID_HANDLE_VALUE;

	enum struct MessageID : uint8_t {
		NA = 0,
		setDisplay = 1,			// control display on/off
		setDisplayGamma = 2		// control display gamma level 
	};

	vector<float> tempData;
	int tempIndex = -1;

	bool enableLED = true;
	bool faren = true;
};

inline globals g_globals;