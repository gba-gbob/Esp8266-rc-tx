#include <Adafruit_ADS1015.h>

typedef struct {
  uint16_t lx: 1000;
  uint16_t ly: 1000;
  uint16_t rx: 1000;
  uint16_t ry: 1000;
  uint16_t aux0: 1000;
  uint16_t aux1: 1000;
  uint16_t aux2: 1000;
  uint16_t aux3: 1000;
  bool failed_to_arm;
} TxInput;

Adafruit_ADS1115 ads(0x4A);
Adafruit_ADS1115 ads1(0x48);

//calibrate your analog inputs here
uint16_t center_rx = 7565 , center_ry = 7223, center_lx = 7505;
uint16_t min_rx = 2000 ,  min_ry = 1600,  min_lx = 2000,  min_ly = 2100;
uint16_t max_rx = 12900 , max_ry = 13100, max_lx = 12800, max_ly = 13100;
bool armed = 0;
void readCenters();
#ifdef DEBUG_INPUT
    uint32_t debug_input_interval = 0;
    void printTxInput(TxInput ti);
#endif

void inputInit(){
    Wire.begin(0, 2);
	delay(100);
    readCenters(); //comment out to disable auto centering on start up
}

void readCenters() {
  center_rx = ads.readADC_SingleEnded(0);
  center_ry = ads.readADC_SingleEnded(1);
  uint16_t current_ly = ads.readADC_SingleEnded(2);
  center_lx = ads.readADC_SingleEnded(3);
#ifdef DEBUG_INPUT
    Serial.println("Measured center positions:");
    Serial.print("center_rx: "); Serial.println(center_rx);
    Serial.print("center_ry: "); Serial.println(center_ry);
    Serial.print("current_ly: "); Serial.println(current_ly);
    Serial.print("center_lx: "); Serial.println(center_lx);
#endif
}

int ajustCenter(uint16_t raw_val, uint16_t center, uint16_t min, uint16_t max) {
  uint16_t  val = raw_val;
  if (abs(val - center) < 250 ) {
    return 1500;
  } else {
    uint16_t calc_center = (max - min)/2 + min;
    int16_t offset =  calc_center - center;
    val += offset;
     return   constrain(map(val, min + offset , max + offset, 1000, 2000), 1000, 2000);
  }
}

TxInput updateInput(TxInput ti){
  uint16_t lx_raw, ly_raw, rx_raw, ry_raw;

  rx_raw = ads.readADC_SingleEnded(0);
  ry_raw = ads.readADC_SingleEnded(1);
  ly_raw = ads.readADC_SingleEnded(2);
  lx_raw = ads.readADC_SingleEnded(3);

  ti.rx =  ajustCenter(rx_raw, center_rx, min_rx, max_rx);
  //joysticks ry, lx are inverted on my setup  thus using map to swap
  ti.ry =  map(ajustCenter(ry_raw, center_ry,  min_ry, max_ry), 1000, 2000, 2000, 1000);
  ti.ly =  constrain( map(ly_raw, min_ly, max_ly, 1000, 2000), 1000, 2000);
  ti.lx =  map(ajustCenter(lx_raw, center_lx, min_lx, max_lx), 1000, 2000, 2000, 1000);

  //read 2 way switches
  ti.aux0 = ads1.readADC_SingleEnded(0) < 7500 ?  1000:2000;
  ti.aux1 = ads1.readADC_SingleEnded(1) < 7500 ? 1000:2000;

  //read 3 way switches
  ti.aux2 = ads1.readADC_SingleEnded(2);
  if(ti.aux2 < 4500){ ti.aux2 = 1000; } else if(ti.aux2 > 10500){ ti.aux2 = 2000; } else { ti.aux2 = 1500; }
  ti.aux3 = ads1.readADC_SingleEnded(3);
  if(ti.aux3 < 4500){ ti.aux3 = 1000; } else if(ti.aux3 > 10500){ ti.aux3 = 2000; } else { ti.aux3 = 1500; }
  //aux0 is arm channel only allows to arm if ly(throttle) is below 1010
  if(ti.aux0 == 2000 && !armed && !ti.failed_to_arm){
    if (ti.ly < 1010 ) {
        armed = true;
    }
    else if (ti.ly > 1010) {
        ti.failed_to_arm = true;
    }
  } else if (ti.aux0 == 1000){
    armed = false;
    ti.failed_to_arm = false;
  }
  ti.aux0 = armed? 2000:1000;
  #ifdef DEBUG_INPUT
    if (millis() - debug_input_interval > 2000) {
      Serial.print("rx_raw:"); Serial.println(rx_raw);
      Serial.print("ry_raw:"); Serial.println(ry_raw);
      Serial.print("ly_raw:"); Serial.println(ly_raw);
      Serial.print("lx_raw:"); Serial.println(lx_raw);
      printTxInput(ti);
      debug_input_interval = millis();
    }
  #endif
  return ti;
}

//calibrate your tx battery voltage here remember esp A0 limited at 1V and needs voltage divider
float txBatteryVoltage(){
    return analogRead(A0) * (14.8 / 1023.0);
}

#ifdef DEBUG_INPUT
void printTxInput(TxInput ti){
  Serial.print("rx: "); Serial.println(ti.rx);
  Serial.print("ry: "); Serial.println(ti.ry);
  Serial.print("ly: "); Serial.println(ti.ly);
  Serial.print("lx: "); Serial.println(ti.lx);
  Serial.print("aux0: "); Serial.println(ti.aux0);
  Serial.print("aux1: "); Serial.println(ti.aux1);
  Serial.print("aux2: "); Serial.println(ti.aux2);
  Serial.print("aux3: "); Serial.println(ti.aux3);
  Serial.print("failed_to_arm: "); Serial.println(ti.failed_to_arm);
}
#endif
