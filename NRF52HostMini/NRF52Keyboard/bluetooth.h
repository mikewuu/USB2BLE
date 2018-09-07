#ifndef BLUETOOTH_H
#define BLUETOOTH_H

  void initBluetooth(); 
  void startAdv();
  void sendReport(uint8_t modifier,uint8_t key0,uint8_t key1,uint8_t key2,uint8_t key3,uint8_t key4,uint8_t key5);

#endif //BLUETOOTH_H
