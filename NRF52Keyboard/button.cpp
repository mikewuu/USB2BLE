#include <Arduino.h>

#include "button.h"

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
