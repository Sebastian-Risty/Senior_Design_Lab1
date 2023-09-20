//Third-party headers
#include <arduino-timer.h>

#include "sw_global.h"
#include "ds_50hz.h"
#include "tm_1hz.h"
#include "nw_1hz.h"

///////////////////////////////////////////////////////////////////////////
//////                          SYSTEM TIMING
////////////////////////////////////////////////////////////////////////////
Timer<10> timer;


bool task_50hz(void *){
    //tm_50hz();
    ds_50hz();
    return true;
}

bool task_1hz(void *){
  tm_1hz();
  nw_1hz();  // TODO: rename :D
  return true;
}

/**
Initialization
*/
void setup() {
  ///////////////////////////////////////////////////////////////////////////
  //////                        TIMER SETUP
  ////////////////////////////////////////////////////////////////////////////
  timer.every(RATE_50HZ, task_50hz);
  timer.every(RATE_1HZ, task_1hz);
  timer.every(DISP_CYCLE_TIME, cycleDisp);

  ///////////////////////////////////////////////////////////////////////////
  //////                        DISPLAY SETUP
  ////////////////////////////////////////////////////////////////////////////
  // put your setup code here, to run once:
  //pinMode(0, OUTPUT); //Pin D13 will correspond to V0 pin on the LCD (to control contrast)
  pinMode(DISP_A_PIN, OUTPUT); //Pin will correspond to A on the LCD (to power the LED).
  pinMode(DISPL_V0_PIN, OUTPUT); //Pin Arduino will correspond to V0 on the LCD (contrast setting).
  pinMode(BUTTON_PIN, INPUT); //Pin 13 connects ot the button that controls the display

  lcd.begin(16,2); // Initializes the interface to the LCD screen, and specifies the dimensions (width and height) of the display } 
  lcd.noCursor(); // Hides the LCD cursor

  ///////////////////////////////////////////////////////////////////////////
  //////                        TEMP SENSOR SETUP
  ////////////////////////////////////////////////////////////////////////////
  for (int i = 0; i < NUM_TEMPS; i++) {
    last300Temps[i] = NULL_TEMP;
  }
  
  sensors.begin();
  sensors.setResolution(12);
  sensors.setWaitForConversion(false);
  ///////////////////////////////////////////////////////////////////////////
  //////                        NETWORKING SETUP
  ///////////////////////////////////////////////////////////////////////////
  btSerial.begin(9600);  // Initialize Bluetooth Serial at 9600
  //Serial.begin(9600);
  pinMode(BT_PIN_STATUS, INPUT);
} //end setup


/**
MAIN LOOP
*/
void loop() { 
  //increment timer
  timer.tick();
}