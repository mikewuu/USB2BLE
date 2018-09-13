#include <Arduino.h>

#include "power.h"


int batLastReported = 0;
int batLastReportTime = -10001;                                // Check battery every 10 seconds


// Calculate battery running average
const int numBatMvReadings = 30;
int batMvReadings[numBatMvReadings];
int batMvReadIndex = 0;
int batMvTotal = 0;
int batMvAverage = 0;


// Raw values on BAT Pin
float readBAT() {
  // Set the analog reference to 3.0V (default = 3.6V)
  // ie. 0 = 0v and max resolution = 3v
  analogReference(AR_INTERNAL_3_0);
  // Set the resolution to 12-bit (0..4095)
  analogReadResolution(12);

  int runtime = millis();
  if(runtime - batLastReportTime > 10000){
   int rawBatMv = analogRead(BAT_PIN) * MV_PER_LSB * 2;       // Used 10k resistors to divide voltage in 2.
   batLastReported = round(rawBatMv / 100) * 100;
   batLastReportTime = runtime;
  } 

  // ADC needs to settle - delay for stability
  delay(1);

  // Set ADC back to defaults
  analogReference(AR_DEFAULT);
  analogReadResolution(10);

  return batLastReported;
}

// Raw values on USB Pin
float readUSB() {
  // Set the analog reference to 3.0V (default = 3.6V)
  // ie. 0 = 0v and max resolution = 3v
  analogReference(AR_INTERNAL_3_0);
  // Set the resolution to 12-bit (0..4095)
  analogReadResolution(12);

  int UsbRawMv = analogRead(USB_PIN) * MV_PER_LSB * 2;      // Used 10k resistors to divide voltage in 2.
  
  // ADC needs to settle - delay for stability
  delay(1);

  // Set ADC back to defaults
  analogReference(AR_DEFAULT);
  analogReadResolution(10);

  return round(UsbRawMv / 10) * 10;
}
