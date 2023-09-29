#pragma once
#include "common.h"

#ifndef CellCarrier
// enum for carrier options
enum struct CellCarrier : int {
	AT_T = 0,
	Boost_Mobile = 1,
	Consumer_Cellular = 2,
	Cricket = 3,
	C_Spire = 4,
	G_Fi = 5,
	M_Mobile = 6,
	Sprint = 7,
	TMobile = 8,
	US_Cellular = 9,
	US_Mobile = 10,
	Verizon = 11,
	V_Mobile = 12,
	X_Mobile = 13
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

	string phoneNumber = "8154747480";
	string message = "temp sensor warning!";
	string carrier = "txt.att.net";

	CellCarrier selectedCarrier = CellCarrier::AT_T; // TODO don't think this needs to be a global, only see it in gui.cpp. Gonna leave because idk if you want to add it to sms logic
};

inline globals g_globals;