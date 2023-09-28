#ifndef sw_global
#define sw_global

#include <string.h>

#define DISPL_V0_PIN 9
#define DISP_A_PIN 10
#define BUTTON_PIN 8
#define ONE_WIRE_BUS 1

#define BT_PIN_RX 11
#define BT_PIN_TX 12   // RX from BT -> pin 3 (TX on arduino)
#define BT_PIN_STATUS 13

#define RATE_50HZ 20
#define RATE_1HZ 1000

#define NULL_TEMP -1270 // 10's place is decimal point
#define NUM_TEMPS 300

bool pcConnected = false;
bool pcCommandDisplayStatus = false;
bool tempSensorActive = false;
bool newTempAvailable = false;

int16_t last300Temps[NUM_TEMPS];
int16_t currentTempIndex = 0;
int16_t currentTemp = NULL_TEMP;
int16_t readCount = 0;


#endif // sw_global
