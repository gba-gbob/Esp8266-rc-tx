#include "SH1106Wire.h"


char buffer[64];
SH1106Wire display(0x3c, 0, 2);
void drawVoltage(int pos, float voltage, char* msg);

void displayInit(){
  display.init();
  //reduce wire clock set by display from 400khz to 100khz ads1115 readings get messed up
  //might be due to week 10k pull ups used on i2c
  Wire.setClock(100000);
  display.setFont(ArialMT_Plain_10);
  //TODO add initial message
}

void showAroundCenter(int y, int xoffset, char* msg ) {
  display.setTextAlignment(TEXT_ALIGN_CENTER);
  display.drawString(64 + xoffset, y, msg);
}

void showAroundCenter(int y, char* msg ) {
  showAroundCenter(y, 0, msg);
}

void displayUpdates(int8_t rssi, TxInput txi, float voltage,  float remoteVoltage ) {
  display.setColor(BLACK); display.fillRect(0, 0, 128, 12); display.setColor(WHITE);
  sprintf(buffer, "%s%i%s", "RSSI:", rssi, " dBm ");
  display.setColor(BLACK); display.fillRect(0, 12, 128, 52); display.setColor(WHITE);
  showAroundCenter(0, buffer);
  if (txi.failed_to_arm) {
    sprintf(buffer, "%s%i", "Failed to arm throttle:", txi.ly  );
  }
  else {
    sprintf(buffer, "%s%i", txi.aux0 == 2000 ? "Armed throttle:" : "Unarmed throttle:", txi.ly  );
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


