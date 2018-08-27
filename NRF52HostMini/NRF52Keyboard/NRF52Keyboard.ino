

#include <Arduino.h>
#include <SoftwareSerial.h>

char hidReport[23];
byte inByte;
int byteIndex;
bool skipSeg = false;

#define SW_RXD    A0
#define SW_TXD    A1

// Declare an Software Serial instance
SoftwareSerial mySerial(SW_RXD, SW_TXD);

// Keyboard Keycodes
uint8_t modifier;
uint8_t key0;
uint8_t key1;
uint8_t key2;
uint8_t key3;
uint8_t key4;
uint8_t key5;

void setup() {

  while (!Serial);
  delay(500);

  Serial.begin(115200);
  Serial.println("Ready to rock..");

  mySerial.begin(9600);
  Serial.println("UART ready ready");
}

void loop() {

  while (mySerial.available()) {

    // Read every byte to remove from buffer
    inByte = mySerial.read();

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

        Serial.write(modifier);
        Serial.write(key0);
        Serial.write(key1);
        Serial.write(key2);
        Serial.write(key3);
        Serial.write(key4);
        Serial.write(key5);


        byteIndex = 0;
        memset(hidReport, 0, 23);
        skipSeg = true;
      }
      
    }
    
  }

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


