#include <hidboot.h>
#include <usbhid.h>
#include <usbhub.h>

#define _DEBUG

#ifdef _DEBUG
   #define DEBUG_PRINT(str) Serial.print (str)
   #define DEBUG_PRINTF(str, format) Serial.print (str, format)
   #define DEBUG_PRINTLN(str)  Serial.println (str)
#else
   #define DEBUG_PRINT(x)
   #define DEBUG_PRINTF(x, format)
   #define DEBUG_PRINTLN(x)
#endif

// Satisfy the IDE, which needs to see the include statment in the ino too.
#ifdef dobogusinclude
#include <spi4teensy3.h>
#include <SPI.h>
#endif

class HIDRelay : public HIDReportParser 
{
    protected:
    virtual void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

void HIDRelay::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
//    DEBUG_PRINT("Relay ");
//    DEBUG_PRINTLN(len);

    for (uint8_t i = 0; i < len; i++) {
           Serial.write((uint8_t)buf[i]);
//         DEBUG_PRINTF((uint8_t)buf[i], HEX);
//         DEBUG_PRINT(",");
    }
    //DEBUG_PRINT("\r\n");
};

class KbdRptParser : public HIDRelay 
{
    void Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf);
};

void KbdRptParser::Parse(USBHID *hid, bool is_rpt_id, uint8_t len, uint8_t *buf) {
    HIDRelay::Parse(hid,is_rpt_id,len,buf);
};


USB     Usb;

HIDBoot<USB_HID_PROTOCOL_KEYBOARD>    HidKeyboard(&Usb);

uint32_t next_time;

KbdRptParser KbdPrs;

void setup()
{
    Serial.begin( 115200 );

#if !defined(__MIPSEL__)
    while (!Serial); // Wait for serial port to connect - used on Leonardo, Teensy and other boards with built-in USB CDC serial connection
#endif

    //DEBUG_PRINTLN("Start");

    if (Usb.Init() == -1)
//        DEBUG_PRINTLN("OSC did not start.");

    delay( 200 );

    next_time = millis() + 5000;

    HidKeyboard.SetReportParser(0, &KbdPrs);

}

void loop()
{
    Usb.Task();
}








