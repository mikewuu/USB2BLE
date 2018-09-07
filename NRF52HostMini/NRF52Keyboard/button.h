#ifndef BUTTON_H
  #define BUTTON_H
  
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
  #define BLUE 4

  void buttonRGB(int red, int green, int blue);
  void buttonColor(int color);
  
#endif 
