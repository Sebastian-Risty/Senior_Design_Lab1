#include "tm_1hz.h"

bool skip = true;

void tm_1hz(){
  sensors.requestTemperaturesByIndex(0);
  float tempTemp = sensors.getTempCByIndex(0);
  int16_t fixedPointTemp = round(tempTemp * 10);
  
  if (fixedPointTemp <= NULL_TEMP){
        currentTemp = NULL_TEMP;
        tempSensorActive = false;
        skip = true;
  } else{
        //skip the second that is 85 (when you first plug in)
        if (skip != false) {
            skip = false;
        }else{
           currentTemp = fixedPointTemp;
           tempSensorActive = true;
        }
  }
  //store temp and increment
  last300Temps[currentTempIndex] = currentTemp;
  currentTempIndex = (currentTempIndex+1)%NUM_TEMPS;
  newTempAvailable = true;
  readCount++;
}