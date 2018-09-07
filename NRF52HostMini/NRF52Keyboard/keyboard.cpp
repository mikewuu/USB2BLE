#include <Arduino.h>

#include "keyboard.h"
#include "bluetooth.h"

/**
 * USB Mini Host 
 * - Raw HID mode sends over a string of hex bytes
 * - http://www.hobbytronics.co.uk/usb-host-keyboard
 */
char  hidReport[23];                // 00-00-00-00-00-00-00-00
byte  inByte;
int   byteIndex;
bool  skipSeg = false;

/**
 * Raw Keycodes
 */
uint8_t modifier;
uint8_t key0;
uint8_t key1;
uint8_t key2;
uint8_t key3;
uint8_t key4;
uint8_t key5;

/**
 * Serial data from USB host 
 */
void processSerial() {
      // Read every byte to remove from buffer
    inByte = Serial.read();
    // New key report, read segment
    if ((int) inByte == 10) {
      skipSeg = false;
    }
    // The USB Host sends line breaks which are 10 & 13 in decimal
    if ((int) inByte != 10 && (int) inByte != 13 && skipSeg != true) {
      // Only care about first 23 bytes
      if (byteIndex < 23) {
        hidReport[byteIndex] = (char) inByte;
      }
      
      byteIndex ++;
      
      if (byteIndex == 23) {
        assignKeyCodes(hidReport);
        sendReport(modifier,key0,key1,key2,key3,key4,key5);   // Send key report via bluetooth
        // Reset and move on to next key segment
        byteIndex = 0;
        memset(hidReport, 0, 23);
        skipSeg = true;
      } 
    }
}

/**
 * Takes a hex code in string format 
 * and converts it into a raw byte
 * of the same hex value
 */
uint8_t stringToHex(char* keyCode) {
  uint8_t hexByte = (uint8_t) strtoul(keyCode, NULL ,16);
  return hexByte;
}

/**
 * Parse HID Report array and assigns
 * respective hex bytes to variables
 */
void assignKeyCodes(char* hidReport) {
  
  char keyCode[2];
  
  // Modifier
  keyCode[0] = hidReport[0];
  keyCode[1] = hidReport[1];
  modifier = stringToHex(keyCode);

  //Key0
  keyCode[0] = hidReport[6];
  keyCode[1] = hidReport[7];
  key0 = stringToHex(keyCode);
  
  //Key1
  keyCode[0] = hidReport[9];
  keyCode[1] = hidReport[10];
  key1 = stringToHex(keyCode);

  //Key2
  keyCode[0] = hidReport[12];
  keyCode[1] = hidReport[13];
  key2 = stringToHex(keyCode);  

  //Key3
  keyCode[0] = hidReport[15];
  keyCode[1] = hidReport[16];
  key3 = stringToHex(keyCode);

  //Key4
  keyCode[0] = hidReport[18];
  keyCode[1] = hidReport[19];
  key4 = stringToHex(keyCode);

  //Key5
  keyCode[0] = hidReport[21];
  keyCode[1] = hidReport[22];
  key5 = stringToHex(keyCode);      
}
