#pragma once
#include "common.h"

#ifndef CellCarrier
// enum for carrier options
enum struct CellCarrier : int {
	Verizon = 0,
	AT_T = 1,
	TMobile = 2,
	Sprint = 3,
};
#endif


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

	string phoneNumber = "6305203444";
	string message = "Wai Gua 吃一袋鸡巴迟钝";
	string carrier = "txt.att.net";

	CellCarrier selectedCarrier = CellCarrier::AT_T;
};

inline globals g_globals;