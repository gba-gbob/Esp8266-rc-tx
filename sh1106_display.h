#include <Wire.h>
#include "SH1106Wire.h"

//#define DEBUG_DISPLAY
char buffer[64];
SH1106Wire display(0x3c, 0, 2);
void drawVoltage(int pos, float voltage, char* msg);

void displayInit(){
  display.init();
  display.setFont(ArialMT_Plain_10);
}

void showAroundCenter(int y, int xoffset, char* msg ) {
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64 + xoffset, y, msg);
}

void showAroundCenter(int y, char* msg ) {
  showAroundCenter(y, 0, msg);
}

void displayUpdates(int8_t rssi, struct TxInput txi, float voltage,  float remoteVoltage ) {
  display.setColor(BLACK); display.fillRect(0, 12, 128, 52); display.setColor(WHITE);
  sprintf(buffer, "%s%i%s", "RSSI:", rssi, " dBm ");
  showAroundCenter(0, buffer);
  if (txi.failed_to_arm) {
    sprintf(buffer, "%s%i", "Failed to arm throttle:", txi.ry  );
  }
  else {
    sprintf(buffer, "%s%i", txi.aux0 == 2000 ? "Armed throttle:" : "Unarmed throttle:", txi.ry  );
  }
  showAroundCenter(13, buffer);
  drawVoltage(26, voltage, "Battery:");
  drawVoltage(39, remoteVoltage, "Remote Battery:");
  display.display();
}

void drawVoltage(int pos, float voltage, char* msg) {
  char v[sizeof(voltage)];
  dtostrf(voltage, 3, 1, v);
  sprintf(buffer, "%s%s %s", msg, v, "V");
  display.setColor(BLACK);
  display.fillRect(0, pos, 128, 12);
  display.setColor(WHITE);
  showAroundCenter(pos, buffer);
}


