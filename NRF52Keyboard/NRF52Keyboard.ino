#include <Arduino.h>

#include "bluetooth.h"
#include "button.h"
#include "keyboard.h"
#include "power.h"

void setup() {
  Serial.begin(9600);   // Baud rate for USB Host

  pinMode(BUTTON_RED_PIN, OUTPUT);
  pinMode(BUTTON_GREEN_PIN, OUTPUT);
  pinMode(BUTTON_BLUE_PIN, OUTPUT);  

  pinMode(BAT_PIN, INPUT);
  pinMode(FIVE_VOLT_PIN, INPUT);
  pinMode(USB_PIN, INPUT);

  initBluetooth();
  startAdv();
}


void loop() {

  // Read key press on serial
  while (Serial.available()) {
    processSerial();
  }

  // Set ON/OFF Button Color
  if(digitalRead(FIVE_VOLT_PIN) == HIGH) {        // Power ON - Check if it's low battery

    int BatMv = round(readBAT() / 100) * 100;

    if(BatMv > 3900) {
      buttonColor(BLUE);
    } else if (BatMv > 3700) {
      buttonColor(BLUERED);
    } else if (BatMv > 3500) {
      buttonColor(PURPLE);
    } else if (BatMv > 3300) {
      buttonColor(REDBLUE);
    } else {
      buttonColor(RED);
    }
    
  } else {
      int UsbMv = readUSB();
      if(UsbMv > 1000) {                          // USB Connected
        if(UsbMv >= 5000) {
          buttonColor(GREEN);                     // FULL
        } else {
          buttonColor(ORANGE);                    // CHARGING
        }
     } else {
       buttonColor(OFF);
     }
  }

  // Sleep CPU
  __WFI();  

}


