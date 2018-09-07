
#ifndef KEYBOARD_H
  #define KEYBOARD_H

  void processSerial();
  uint8_t stringToHex(char* keyCode);
  
  void assignKeyCodes(char* hidReport);

#endif 
