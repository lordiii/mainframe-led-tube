#include <Artnet.h>
#include <NativeEthernet.h>
#include <NativeEthernetUdp.h>
#include <OctoWS2811.h>

Artnet initializeLEDs(byte mac[4], byte ipAddress[4], byte broadcast[4]);
void initTest();
void onDmxFrame(uint16_t universe, uint16_t length, uint8_t sequence, uint8_t* data, IPAddress remoteIP);