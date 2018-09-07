/**
 * Measure Bat Pin on Power Boost 500
 * - https://www.adafruit.com/product/1944
 */
#ifndef POWER_H
  #define POWER_H
  
  #define MV_PER_LSB         (0.73242188F)        // ADC Range/Resolution = 3000/4096
  
  #define BAT_PIN            (A3)                 // BAT voltage to check %           
  #define FIVE_VOLT_PIN      (A4)                 // Measure 5v Pin from Power Boost to check if the button is ON.
  #define USB_PIN            (A5)                 // USB pin to check if a charging cable isc onnected

  float readBAT();
  float readUSB();

#endif 
