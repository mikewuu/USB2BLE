#include <Arduino.h>
#include <SoftwareSerial.h>
#include <bluefruit.h>

char hidReport[23];
byte inByte;
int byteIndex;
bool skipSeg = false;

/*
 * Software Serial for Debugging
 * BLE doesn't work with Software Serial.
 */
//#define SW_RXD    A0
//#define SW_TXD    A1

// Declare an Software Serial instance
//SoftwareSerial mySerial(SW_RXD, SW_TXD);

// Keyboard Keycodes
uint8_t modifier;
uint8_t key0;
uint8_t key1;
uint8_t key2;
uint8_t key3;
uint8_t key4;
uint8_t key5;

BLEDis bledis;
BLEHidAdafruit blehid;

void setup() {

  // Serial.begin(115200);
  Serial.begin(9600);   // Baud rate for USB Host
  
  //Serial.println("Ready to rock..");

  //mySerial.begin(9600);
  //Serial.println("UART ready ready");

  Bluefruit.begin();
  Bluefruit.setTxPower(4);
  Bluefruit.setName("Kinesis Advantage 2");

  bledis.setManufacturer("WU Tech");
  bledis.setModel("K2ADV1");
  bledis.begin();

  blehid.begin();

  //blehid.setKeyboardLedCallback(set_keyboard_led);
  
  startAdv();
}

void startAdv(void)
{  
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  
  // Include BLE HID service
  Bluefruit.Advertising.addService(blehid);

  // There is enough room for the dev name in the advertising packet
  Bluefruit.Advertising.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void loop() {

  while (Serial.available()) {

    // Read every byte to remove from buffer
    inByte = Serial.read();

    // New key report, read segment
    if ((int) inByte == 10) {
      skipSeg = false;
    }

    // USB Host sends line breaks which are 10 & 13 in decimal
    if ((int) inByte != 10 && (int) inByte != 13 && skipSeg != true) {

      // Only care about first 23 bytes 
      if (byteIndex < 23) {
        hidReport[byteIndex] = (char) inByte;
      }
  
      byteIndex ++;
  
      if (byteIndex == 23) {

//        Serial.print(hidReport);

        assignKeyCodes(hidReport);

//        Serial.write(modifier);
//        Serial.write(key0);
//        Serial.write(key1);
//        Serial.write(key2);
//        Serial.write(key3);
//        Serial.write(key4);
//        Serial.write(key5);

        blehid.keyboardReport(modifier,key0,key1,key2,key3,key4,key5);


        byteIndex = 0;
        memset(hidReport, 0, 23);
        skipSeg = true;
      }
      
    }
    
  }

  waitForEvent();  

}

uint8_t stringToHex(char* keyCode) {
  uint8_t hexByte = (uint8_t) strtoul(keyCode, NULL ,16);
  return hexByte;
}

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


