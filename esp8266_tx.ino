//import order is important to recognise custom structures
#include "wifi.h"
#include "ads1115_input.h"
#include "sh1106_display.h"
#include "multiwii_msp.h"

TxInput txInput;
Packet wifiPacket;
long oneSecondUpdates = 0;
long bindDuration = 5000;

void setup() {
  Serial.begin(115200);
  displayInit();
  inputInit();
  wifiPacket = wifiInitBind(wifiPacket);
  bindDuration += millis();
  while(bindDuration > millis()){
    wifiSendPacket(wifiPacket);
    delay(3);
  }
  wifiPacket = wifiInitData(wifiPacket);
}

void loop() {
    txInput = updateInput(txInput);
	updateMspRawRc((uint8_t*) &wifiPacket.data, txInput);
   	wifiSendPacket(wifiPacket);
    if (millis() - oneSecondUpdates > 1000) {
        displayUpdates(remoteRssi(), txInput, txBatteryVoltage(), remoteBatteryVoltage());
        oneSecondUpdates = millis();
    }
    delay(3);
}
