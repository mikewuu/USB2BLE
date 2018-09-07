#include <Arduino.h>
#include <bluefruit.h>

/**
 * Adafruit BLE classes
 */
BLEDis bledis;
BLEHidAdafruit blehid;

void initBluetooth() {
  Bluefruit.begin();
  Bluefruit.setName("Kinesis Advantage 2");
  // Set bluetooth power - Accepted values are: -40, -30, -20, -16, -12, -8, -4, 0, 4
  Bluefruit.setTxPower(-4);
  // On-board blue LED: On (true) / Off (false)
  Bluefruit.autoConnLed(true);

  bledis.setManufacturer("Mike Wu");
  bledis.setModel("Kinesis Advantage 2");
  
  bledis.begin();
  blehid.begin();
}

// BLE Advertising
void startAdv()
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

void sendReport(uint8_t modifier,uint8_t key0,uint8_t key1,uint8_t key2,uint8_t key3,uint8_t key4,uint8_t key5){
  blehid.keyboardReport(modifier,key0,key1,key2,key3,key4,key5);
}

