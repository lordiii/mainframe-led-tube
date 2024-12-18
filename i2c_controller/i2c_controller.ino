#include "i2c_controller.h"
#include <Bluepad32.h>
#include "Wire.h"

#define I2C_DEV_ADDR 0x55

#define GP_CMD_CLEAR 0x00
#define GP_CMD_ENABLE_PAIRING 0x01
#define GP_CMD_DISABLE_PAIRING 0x02

#define GP_NO_CONTROLLER 0x02
#define GP_HAS_CHANGES 0x01
#define GP_NO_CHANGES 0x00

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

GP_Status lastStatus;
GP_Status newStatus;

bool hasChanged;

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

void onRequest() {
  ControllerPtr ctl = myControllers[0];

  if (ctl && ctl->isConnected() && ctl->isGamepad()) {
    if(hasChanged) {
      Wire.write((uint8_t) GP_HAS_CHANGES);
      Wire.write((uint8_t*) &lastStatus, sizeof(GP_Status));
      hasChanged = false;
    } else {
      Wire.write((uint8_t) GP_NO_CHANGES);
    }
  } else {
    Wire.write((uint8_t) GP_NO_CONTROLLER);
    hasChanged = true;
  }
}

void onReceive(int len) {
  if (len == 1) {
    int c = Wire.read();
    switch (c) {
      case GP_CMD_CLEAR:
        BP32.forgetBluetoothKeys();
        hasChanged = true;
        break;
      case GP_CMD_ENABLE_PAIRING:
        BP32.enableNewBluetoothConnections(true);
        hasChanged = true;
        break;
      case GP_CMD_DISABLE_PAIRING:
        BP32.enableNewBluetoothConnections(false);
        hasChanged = true;
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
}

unsigned long i = 0;
void loop() {
  BP32.update();

  ControllerPtr ctl = myControllers[0];

  if (!ctl || !ctl->isConnected() || !ctl->isGamepad()) {
    Serial.println("NO CONTROLLER; Skipping status check");
    delay(250);
    return;
  }
  
  unsigned char dpad = ctl->dpad();

  newStatus.dpadLeft = dpad & DPAD_LEFT;
  newStatus.dpadRight = dpad & DPAD_RIGHT;
  newStatus.dpadDown = dpad & DPAD_DOWN;
  newStatus.dpadUp = dpad & DPAD_UP;

  newStatus.buttonY = ctl->y();
  newStatus.buttonB = ctl->b();
  newStatus.buttonA = ctl->a();
  newStatus.buttonX = ctl->x();
  newStatus.shoulderL1 = ctl->l1();
  newStatus.shoulderL2 = ctl->l2();
  newStatus.shoulderR1 = ctl->r1();
  newStatus.shoulderR2 = ctl->r2();

  newStatus.miscHome = ctl->miscHome();
  newStatus.miscStart = ctl->miscStart();
  newStatus.miscSelect = ctl->miscSelect();
  newStatus.miscSystem = ctl->miscSystem();
  newStatus.miscBack = ctl->miscBack();
  newStatus.miscCapture = ctl->miscCapture();
  newStatus.buttonTL = ctl->thumbL();
  newStatus.buttonTR = ctl->thumbR();

  newStatus.breakForce = ctl->brake();
  newStatus.throttleForce = ctl->throttle();
  newStatus.stickLX = ctl->axisX();
  newStatus.stickRX = ctl->axisRX();

  newStatus.stickLY = ctl->axisY();
  newStatus.stickRY = ctl->axisRY();

  if(memcmp(&newStatus, &lastStatus, sizeof(GP_Status)) != 0) {
    Serial.println("GP CHANGE DETECTED");
    memcpy(&lastStatus, &newStatus, sizeof(GP_Status));
    hasChanged = true;
  }

  delay(1);
}
