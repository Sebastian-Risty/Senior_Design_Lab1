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

	MessageID currentID = MessageID::NA;	// update this + any other data needed before calling writeData()
	bool enableLED = false;
	bool faren = false;

	bool connected = false;  // true when a BT connection is established

	string phoneNumber = "6305203444";
	string message = "Wai Gua 吃一袋鸡巴迟钝";
	string carrier = "txt.att.net";
};

inline globals g_globals;