#include "ds_50hz.h"

/**
* prints the stuff to the screen
*/
void printLCD(bool printStatus){
  lcd.setCursor(0,0); // back to origin
  lcd.print("                "); //clear row 1
  lcd.setCursor(0,0); // back to origin
  if (tempSensorActive != false) {
    lcd.print("Temp: " + String(currentTemp / 10.f) + (char)223 + "C"); // MAY BE  FAIL BLOGGING THE DISPLAY IDK
  }else{
    lcd.print("- - - - -"); 
  }
  if (printStatus) {
    if (errorMessage.length() <= 16){
      lcd.setCursor(0,1); 
      lcd.print(errorMessage);
    }
    else{ //cycle through
      for (int i =0; i<16; i++) {
        lcd.setCursor(i,1);
        lcd.print(errorMessage[(dispCycle+i)%(errorMessage.length())]);
      }
    }
  } else{
    lcd.setCursor(0,1); // Sets the location at which subsequent text written to the LCD will be displayed 
    lcd.print("                ");
  }
  
}

//Function to increment the display cycle number, called every DISP_CYCLE_TIME miliseconds 
//using a timer
bool cycleDisp(void *){
  dispCycle = ((dispCycle+1)%(errorMessage.length()));
  return true;
} //end cycleDisp

/**
* Handles code for the display task. Run every cycle
*/
void ds_50hz(){
  displayButtonState = (bool)digitalRead(BUTTON_PIN);
  //button pushed for first time after being "off"
  if ((displayButtonState == true) && (displayButtonPrevState == false)){
    lastButtonTime = millis(); //get time button was pressed
  }
  displayButtonPrevState = displayButtonState;
  
  //Form error message
  errorMessage = "";
  if (!pcConnected){
    errorMessage = errorMessage + PC_ERROR;
  }else{
    errorMessage = errorMessage + PC_GOOD;
  }
  if (!tempSensorActive){
    errorMessage = errorMessage + SEPARATOR + TEMP_SENSOR_ERROR;
  }

  if (digitalRead(8) == HIGH || pcCommandDisplayStatus){  //Control whether or not the LCD is on
    //blinking effect only if we don't overflow
    if (errorMessage.length() <= 16){ 
      if (((millis() - lastButtonTime)%1000) >= DISP_BLINK_TIME){
        printLCD(true);
      } else{
        printLCD(false);
      }
    } else{ //If we do overflow, don't blink
        printLCD(true);
    }

     analogWrite(DISPL_V0_PIN, contrast%125);
     analogWrite(DISP_A_PIN, brightness%255); // Generate PWM signal at pin 3, value of 255 (out of 255) ~= 5V (in hte example, they use 5V)
  } else { //Control whether or not the LCD is off
    analogWrite(3,0); //set brightness to 0
    analogWrite(11,0); //set contrast to 0
    lcd.clear();
  }
} //end ds_task