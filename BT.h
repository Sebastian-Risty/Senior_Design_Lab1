#pragma once
#include "globals.h"

const int16_t SOM_MARKER = -32768;
const int16_t EOM_MARKER = -32767;

enum MessageID {  // TODO: see if we can use enum name directly otherwise numbers will do for now :D
	setDisplay      = 0,
	setDisplayGamma = 1
};

struct CommandData {

};

bool readData();

bool initPair(bool isReconnect = false);

void writeData();