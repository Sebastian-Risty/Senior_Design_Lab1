#ifndef ds_50hz_h
#define ds_50hz_h

#include "sw_global.h"
#include <LiquidCrystal.h>


////////////////////////////////////////////
// DEFINES
///////////////////////////////////////////
#define PC_ERROR "PC NOT CONNECTED"
#define PC_GOOD "PC CONNECTED!"
#define TEMP_SENSOR_ERROR "TEMP SENSOR ERROR  !!! "
#define SEPARATOR " & "
#define DISP_CYCLE_TIME 400
#define DISP_BLINK_TIME 400

////////////////////////////////////////////
// GLOBALS
///////////////////////////////////////////
uint8_t brightness = 200; // max 255 for uint8
uint8_t contrast = 75;
bool displayButtonState = false;
bool displayButtonPrevState = false;
unsigned long lastButtonTime = 0;
int dispCycle = 0;  //used to cycle status when it overflows
//ERROR MESSAGE-RELATED
String errorMessage = "";

LiquidCrystal lcd(2, 1, 4, 5, 6, 7); // Creates an LCD object. Parameters: (rs, enable, d4, d5, d6, d7) 


////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////
void ds_50hz(void *);

bool cycleDisp(void *);



#endif // ds_50hz_h
