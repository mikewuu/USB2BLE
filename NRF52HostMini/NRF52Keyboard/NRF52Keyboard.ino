#include <Arduino.h>
#include <SoftwareSerial.h>
#include <bluefruit.h>

/*
 * Software Serial for debugging keyboard input on 
 * serial. This needs to be turned off as BLE 
 * doesn't function with Software Serial on.
 */
//#define SW_RXD    A0
//#define SW_TXD    A1
//// Declare an Software Serial instance
//SoftwareSerial mySerial(SW_RXD, SW_TXD);

/**
 * USB Mini Host 
 * - Raw HID mode sends over a string of hex bytes
 * - http://www.hobbytronics.co.uk/usb-host-keyboard
 */
char hidReport[23];
byte inByte;
int byteIndex;
bool skipSeg = false;

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
 * Adafruit BLE classes
 */
BLEDis bledis;
BLEHidAdafruit blehid;

/**
 * ON/OFF RGB Button
 */
#define BUTTON_RED_PIN    (A0)
#define BUTTON_GREEN_PIN  (A1)
#define BUTTON_BLUE_PIN   (A2)
#define COMMON_ANODE

/**
 * Color Codes
 * Can't switch on strings so we define as ints
 */
#define OFF 0
#define YELLOW 1
#define GREEN 2
#define RED 3
#define BLUE 5

/**
 * Measure Bat Pin on Power Boost 500
 * - https://www.adafruit.com/product/1944
 */
#define BAT_PIN            (A3)           
#define BAT_MV_PER_LSB     (0.73242188F)   // ADC Range/Resolution = 3000/4096

// Measure 5v Pin from Power Boost to check
// if the button is ON.
#define FIVE_VOLT_PIN   (A4)
// USB pin to check if a charging cable is
// connected
#define USB_PIN         (A5)

// Minimum battery MV before low battery
int lowBatMinMv = 3300;
// Calculate battery running average
const int numBatMvReadings = 100;
int batMvReadings[numBatMvReadings];
int batMvReadIndex = 0;
int batMvTotal = 0;
int batMvAverage = 0;


void setup() {

  // Serial.begin(115200);
  Serial.begin(9600);   // Baud rate for USB Host
  
  //Serial.println("Ready to rock..");

  //mySerial.begin(9600);
  //Serial.println("UART ready ready");

  Bluefruit.begin();
  Bluefruit.setName("Kinesis Advantage 2");
  // Set bluetooth power - Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(4);
  // On-board blue LED: On (true) / Off (false)
  Bluefruit.autoConnLed(true);

  bledis.setManufacturer("Mike Wu");
  bledis.setModel("Kinesis Advantage 2");
  
  bledis.begin();
  blehid.begin();

  //blehid.setKeyboardLedCallback(set_keyboard_led);
  
  startAdv();

  pinMode(BUTTON_RED_PIN, OUTPUT);
  pinMode(BUTTON_GREEN_PIN, OUTPUT);
  pinMode(BUTTON_BLUE_PIN, OUTPUT);  

  pinMode(BAT_PIN, INPUT);
  pinMode(FIVE_VOLT_PIN, INPUT);
  pinMode(USB_PIN, INPUT);
}

// BLE Advertising
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
  
  // Apple recommended advertising settings
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

void loop() {

  // Read key press on serial
  while (Serial.available()) {
    // Read every byte to remove from buffer
    inByte = Serial.read();
    // New key report, read segment
    if ((int) inByte == 10) {
      skipSeg = false;
    }
    // The USB Host sends line breaks which are 10 & 13 in decimal
    if ((int) inByte != 10 && (int) inByte != 13 && skipSeg != true) {
      // Only care about first 23 bytes
      // 00-00-00-00-00-00-00-00
      if (byteIndex < 23) {
        hidReport[byteIndex] = (char) inByte;
      }
      
      byteIndex ++;
      
      if (byteIndex == 23) {
//        Serial.print(hidReport);
        assignKeyCodes(hidReport);
        // Send key report via BLE
        blehid.keyboardReport(modifier,key0,key1,key2,key3,key4,key5);
        // Reset and move on to next key segment
        byteIndex = 0;
        memset(hidReport, 0, 23);
        skipSeg = true;
      } 
    }
  }

  // Set ON/OFF Button Color
  if(digitalRead(FIVE_VOLT_PIN) == HIGH) {
    // Power ON - Check if it's low battery
   if(readBAT() > lowBatMinMv) { 
       // Battery Normal = BLUE
       buttonColor(BLUE);
    } else {
      // Battery level Low = RED
       buttonColor(RED);
    }
  } else {
     // Power OFF - check if USB is connected
    if(digitalRead(USB_PIN) == HIGH) {
      // Check to see if battery is charging or full
      int battMv = readBAT();
      if(battMv > 4140) {
        // Battery Charging = YELLOW
        buttonColor(YELLOW);
      } else if (battMv > 4080) {
        // Battery Full = GREEN
        buttonColor(GREEN);
      } else {
        // Battery Charging = YELLOW
         buttonColor(YELLOW);
      }
    } else {
      buttonColor(OFF);
    }
  }

  // Sleep CPU
  __WFI();  

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

// Set button RGB levels
void buttonRGB(int red, int green, int blue)
{
  #ifdef COMMON_ANODE
    red = 255 - red;
    green = 255 - green;
    blue = 255 - blue;
  #endif
  analogWrite(BUTTON_RED_PIN, red);
  analogWrite(BUTTON_GREEN_PIN, green);
  analogWrite(BUTTON_BLUE_PIN, blue);  
}

// Raw values on BAT Pin
float readBAT(void) {
  // Set the analog reference to 3.0V (default = 3.6V)
  // ie. 0 = 0v and max resolution = 3v
  analogReference(AR_INTERNAL_3_0);
  // Set the resolution to 12-bit (0..4095)
  analogReadResolution(12);

  // Remove old reading at current index
  batMvTotal = batMvTotal - batMvReadings[batMvReadIndex];
  // Read new value and replace at index
  batMvReadings[batMvReadIndex] = analogRead(BAT_PIN);
  // Update our total with new value
  batMvTotal = batMvTotal + batMvReadings[batMvReadIndex];
  // Go to next value in array
  batMvReadIndex = batMvReadIndex + 1;

  // if we're at the end of the array...
  if (batMvReadIndex >= numBatMvReadings) {
    // ...wrap around to the beginning:
    batMvReadIndex = 0;
  }

  // Calculate running average
  batMvAverage = batMvTotal / numBatMvReadings;
  
  // ADC needs to settle - delay for stability
  delay(1);

  // Set ADC back to defaults
  analogReference(AR_DEFAULT);
  analogReadResolution(10);

  // Used 10k resistors to divide voltage in 2.
  return batMvAverage * BAT_MV_PER_LSB * 2;
}

void buttonColor(int color) {
  switch (color) {
    case YELLOW: buttonRGB(255, 255, 0);
      break;
    case GREEN: buttonRGB(0, 255, 0);
      break;
    case RED: buttonRGB(255, 0, 0);
      break;
    case BLUE: buttonRGB(0, 0, 255);
      break;
    default: buttonRGB(0, 0, 0);
      break;      
  }

}

