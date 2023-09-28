#ifndef nw_1hz_h
#define nw_1hz_h


#include "sw_global.h"
#include <SoftwareSerial.h>

enum struct MessageID : int16_t {
	NA = 0,
	setDisplay = 1,			// control display on/off
	setDisplayGamma = 2,		// control display gamma level 
  setBrightness = 3,   //control brightness level
};

struct TempData {
  int16_t data[300];     // message data
  int16_t singleData;
};

struct CommandData {
	MessageID id = MessageID::NA;
	int16_t data = 0;	//  range 0 - 255
};

SoftwareSerial btSerial(BT_PIN_RX, BT_PIN_TX); // RX, TX

TempData sending;
byte* sendingAsBytes = nullptr;

CommandData receiving;
byte* receivingAsBytes = nullptr;

const int16_t SOM_MARKER = -32768;  // 0x8000
const int16_t EOM_MARKER = -32767;  // 0x8001

void nw_read();

void processCommand(const CommandData& cmd);

int16_t readInt16();

void nw_write(bool all);

void nw_1hz();

#endif