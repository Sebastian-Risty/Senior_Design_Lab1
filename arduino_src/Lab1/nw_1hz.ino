#include "nw_1hz.h"

void nw_1hz(){
  if(digitalRead(BT_PIN_STATUS) == LOW){
      pcConnected = false;
      pcCommandDisplayStatus = false;
  }

  //Update connection status
  if(!pcConnected){
    if(digitalRead(BT_PIN_STATUS) == HIGH){
      pcConnected = true;

      //send last 300 seconds
      memcpy(sending.data, last300Temps, sizeof(last300Temps));
      newTempAvailable = false;
      nw_write(true);
    } 
  }

  if (btSerial.available()) {
    nw_read();
  }

  // send new temp value
  if(newTempAvailable && pcConnected){
    sending.singleData = currentTemp;
    newTempAvailable = false;
    nw_write(false);
  }
  
  return;  // TODO: if call has not finished before timer is ready is issue or no?? :D
}
//pcCommandDisplayStatus to turn on display 

void processCommand(const CommandData& cmd){
  switch (cmd.id) {
    case MessageID::NA:
      // Do nothing
      break;
    case MessageID::setDisplay:
      // TODO: update actual value, example for BRAD :D (all data is int16_t, but is just casted from traditional values before being sent over as bytes)

      pcCommandDisplayStatus = static_cast<bool>(cmd.data);
      //Serial.print("value received: ");
      //Serial.println(static_cast<int>(test));
      break;
    
  }
}

int16_t readInt16() {
  byte lowByte = btSerial.read();
  byte highByte = btSerial.read();
  return (highByte << 8) | lowByte;
}

void nw_read(){
    if (btSerial.available() >= sizeof(CommandData) + 2 * sizeof(int16_t)) {  // CommandData + SOM + EOM
    int16_t som = readInt16();
    if (som == SOM_MARKER) {
      CommandData cmd;
      uint8_t* bytePtr = reinterpret_cast<uint8_t*>(&cmd);
      for (size_t i = 0; i < sizeof(CommandData); ++i) {
        bytePtr[i] = btSerial.read();
      }
      int16_t eom = readInt16();
      if (eom == EOM_MARKER) {
        processCommand(cmd);
      }
    } else {
      while(btSerial.available() > 0){  // invalid SOM received, clear the buffer
        btSerial.read();
      }
    }
  }
}

void nw_write(bool all){
  btSerial.write((byte*) &SOM_MARKER, sizeof(int16_t));

  if (all){
      btSerial.write((byte*) sending.data, sizeof(int16_t) * NUM_TEMPS);
      btSerial.write((byte*) &currentTempIndex, sizeof(int16_t));
  } else {
      int16_t dataElement = sending.singleData;
      btSerial.write((byte*) &dataElement, sizeof(int16_t));
  }
  btSerial.write((byte*) &EOM_MARKER, sizeof(int16_t));  
}
