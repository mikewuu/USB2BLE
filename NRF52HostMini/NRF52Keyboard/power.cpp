#include <Arduino.h>

#include "power.h"

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
  return batMvAverage * MV_PER_LSB * 2;
}

// Raw values on USB Pin
float readUSB() {
  // Set the analog reference to 3.0V (default = 3.6V)
  // ie. 0 = 0v and max resolution = 3v
  analogReference(AR_INTERNAL_3_0);
  // Set the resolution to 12-bit (0..4095)
  analogReadResolution(12);

  int UsbRawMv = analogRead(USB_PIN);
  
  // ADC needs to settle - delay for stability
  delay(1);

  // Set ADC back to defaults
  analogReference(AR_DEFAULT);
  analogReadResolution(10);

  // Used 10k resistors to divide voltage in 2.
  return UsbRawMv * MV_PER_LSB * 2;
}
