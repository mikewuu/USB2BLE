/*********************************************************************
Serial2BLE - Sender Arduino

This takes a serial input for raw HID keyboard reports in HEX and
outputs the "AT+BLEKEYBOARDCODE=" command required for Bluefruit
to send raw key reports.
*********************************************************************/

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"

#include "BluefruitConfig.h"

#if SOFTWARE_SERIAL_AVAILABLE
  #include <SoftwareSerial.h>
#endif

// Bluefruit Module Settings
#define FACTORYRESET_ENABLE         0
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"

/* Uncomment to print to Serial for debugging*/
//#define _DEBUG

#ifdef _DEBUG
   #define DEBUG_PRINT(str) Serial.print (str)
   #define DEBUG_PRINTF(str, format) Serial.print (str, format)
   #define DEBUG_PRINTLN(str)  Serial.println (str)
#else
   #define DEBUG_PRINT(x)
   #define DEBUG_PRINTF(x, format)
   #define DEBUG_PRINTLN(x)
#endif

/* Default for Feather M0 Bluefruit - hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// Track byte
int incomingByte = 0;
int byteCounter = 0;
String cmd = "AT+BLEKEYBOARDCODE=";

// Convert raw hex byte to 2 character string 
String hex_to_str(uint8_t hex) {
  String str = String(hex, HEX);
  if (hex < 16) {
    str = "0" + str;
  }
  return str;
}

// A small helper
void error(const __FlashStringHelper*err) {
  DEBUG_PRINTLN(err);
  while (1);
}


/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{

  /** Uncomment these for debugging to Serial (Micro USB)**/
  //while (!Serial);
  //delay(500);
  //Serial.begin(115200);
   
  while (!Serial1);
  delay(500);
  Serial1.begin(115200);
  DEBUG_PRINTLN(F("Serial2BLE"));
  DEBUG_PRINTLN(F("---------------------------------------"));

  /* Initialise the module */
  DEBUG_PRINT(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in CoMmanD mode & check wiring?"));
  }
  DEBUG_PRINTLN( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    DEBUG_PRINTLN(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  DEBUG_PRINTLN("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  //ble.info();

  /* Change the device name to make it easier to find */
  DEBUG_PRINTLN(F("Setting device name to 'Bluefruit Keyboard': "));
  if (! ble.sendCommandCheckOK(F( "AT+GAPDEVNAME=Kinesis Advantage 2" )) ) {
    error(F("Could not set device name?"));
  }

  /* Enable HID Service */
  DEBUG_PRINTLN(F("Enable HID Service (including Keyboard): "));
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    if ( !ble.sendCommandCheckOK(F( "AT+BleHIDEn=On" ))) {
      error(F("Could not enable Keyboard"));
    }
  }else
  {
    if (! ble.sendCommandCheckOK(F( "AT+BleKeyboardEn=On"  ))) {
      error(F("Could not enable Keyboard"));
    }
  }

  /* Add or remove service requires a reset */
  DEBUG_PRINTLN(F("Performing a SW reset (service changes require a reset): "));
  if (! ble.reset() ) {
    error(F("Couldn't reset??"));
  }

  DEBUG_PRINTLN(F("Setup complete..."));

}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop(void)
{
   if (Serial1.available() > 0) {
                // read the incoming byte:
               incomingByte = Serial1.read();
               if (byteCounter > 0) {
                cmd += "-";
               }
               cmd += hex_to_str(incomingByte);
               byteCounter += 1;
               // Got all 8 bytes
               if (byteCounter == 8) {
                  send_command();
               }
    }
}


void send_command() {

  // Send command to bluefruit module
  ble.println(cmd);

  if( ble.waitForOK() )
  {
    DEBUG_PRINTLN( F("OK!") );
  } else
  {
    DEBUG_PRINTLN( F("FAILED!") );
  }

  //reset cmd
  cmd = "AT+BLEKEYBOARDCODE=";
  //reset byte counter
  byteCounter = 0;
}
