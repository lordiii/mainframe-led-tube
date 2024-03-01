#include <Bluepad32.h>
#include "Wire.h"

#define I2C_DEV_ADDR 0x55

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

// This callback gets called any time a new gamepad is connected.
// Up to 4 gamepads can be connected at the same time.
void onConnectedController(ControllerPtr ctl) {
  bool foundEmptySlot = false;
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
      // Additionally, you can get certain gamepad properties like:
      // Model, VID, PID, BTAddr, flags, etc.
      ControllerProperties properties = ctl->getProperties();
      Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                    properties.product_id);
      myControllers[i] = ctl;
      foundEmptySlot = true;
      break;
    }
  }
  if (!foundEmptySlot) {
    Serial.println("CALLBACK: Controller connected, but could not found empty slot");
  }
}

void onDisconnectedController(ControllerPtr ctl) {
  bool foundController = false;

  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
      myControllers[i] = nullptr;
      foundController = true;
      break;
    }
  }

  if (!foundController) {
    Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
  }
}

uint8_t pushAnalogValue(int value) {
  Wire.write((value >> 24)  & 0xFF);
  Wire.write((value >> 16)  & 0xFF);
  Wire.write((value >> 8 )  & 0xFF);
  Wire.write((value >> 0 )  & 0xFF);
  return 4;
}

void onRequest() {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    ControllerPtr ctl = myControllers[i];
    if (ctl && ctl->isConnected() && ctl->isGamepad()) {
      Wire.write(ctl->dpad());

      uint8_t buttons = 0;
      buttons = buttons | (ctl->y() << 7);
      buttons = buttons | (ctl->b() << 6);
      buttons = buttons | (ctl->a() << 5);
      buttons = buttons | (ctl->x() << 4);
      buttons = buttons | (ctl->l1() << 3);
      buttons = buttons | (ctl->l2() << 2);
      buttons = buttons | (ctl->r1() << 1);
      buttons = buttons | (ctl->r2() << 0);
      Wire.write(buttons);

      buttons = 0;
      buttons = buttons | (ctl->miscHome() << 7);
      buttons = buttons | (ctl->miscStart() << 6);
      buttons = buttons | (ctl->miscSelect() << 5);
      buttons = buttons | (ctl->miscSystem() << 4);
      buttons = buttons | (ctl->miscBack() << 3);
      buttons = buttons | (ctl->miscCapture() << 2);
      buttons = buttons | (ctl->thumbL() << 1);
      buttons = buttons | (ctl->thumbR() << 0);
      Wire.write(buttons);

      pushAnalogValue(ctl->brake());
      pushAnalogValue(ctl->throttle());
      pushAnalogValue(ctl->axisX());
      pushAnalogValue(ctl->axisY());
      pushAnalogValue(ctl->axisRX());
      pushAnalogValue(ctl->axisRY());
    } else {
      Wire.write((uint8_t)0xFF);
      Wire.write((uint8_t)0xFF);
    }
  }
}

void onReceive(int len) {
  if (len == 1) {
    int c = Wire.read();
    switch (c) {
      case 0x00:
        BP32.forgetBluetoothKeys();
        break;
      case 0x01:
        BP32.enableNewBluetoothConnections(true);
        break;
      case 0x02:
        BP32.enableNewBluetoothConnections(false);
        break;
    }
  }
}


void setup() {
  Serial.begin(115200);
  Wire.onReceive(onReceive);
  Wire.onRequest(onRequest);
  Wire.begin((uint8_t)I2C_DEV_ADDR);
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.enableVirtualDevice(false);
  BP32.enableVirtualDevice(false);
}

unsigned long i = 0;
void loop() {
  BP32.update();
}
