/*
 This project is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Multiprotocol is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with Multiprotocol.  If not, see <http://www.gnu.org/licenses/>.
 */

#define MSP_SET_RAW_RC  200   //in message          8 rc chan
#define TX_BUFFER_SIZE 23
//#define DEBUG_MSP

uint8_t checksum;
volatile uint8_t serialHeadTX, serialTailTX = 0;
uint8_t serialBufferTX[TX_BUFFER_SIZE];

void SerialSerialize(uint8_t a) {
  uint8_t t = serialHeadTX;
  if (++t >= TX_BUFFER_SIZE) t = 0; serialBufferTX[t] = a;
  serialHeadTX = t;
}

void serialize8(uint8_t a) {
  SerialSerialize(a);
  checksum ^= a;
}

void serialize16(uint16_t a) {
  serialize8((a   ) & 0xFF);
  serialize8((a >> 8) & 0xFF);
}

void headSerialResponse( uint8_t payloadSize, uint8_t cmd) {
  serialize8('$');
  serialize8('M');
  serialize8('<');
  checksum = 0; // start calculating a new checksum
  serialize8(payloadSize);
  serialize8(cmd);
}

void tailSerialReply() {
  serialize8(checksum);
}

void readBuffer(uint8_t *msp_packet) {
	uint8_t idx = 0;
  while (serialHeadTX != serialTailTX ) {
    if (++serialTailTX >= TX_BUFFER_SIZE) serialTailTX = 0;
    msp_packet[idx] = serialBufferTX[serialTailTX];
    idx++;
  }
}

void updateMspRawRc(uint8_t *msp, struct TxInput in){
   headSerialResponse(16, MSP_SET_RAW_RC);
   serialize16(in.lx);
   serialize16(in.ly);
   serialize16(in.ry);
   serialize16(in.rx);
   serialize16(in.aux0);
   serialize16(in.aux1);
   serialize16(in.aux2);
   serialize16(in.aux3);
   tailSerialReply();
   readBuffer(msp);
#ifdef DEBUG_MSP
   for(int i = 0; i < 22; i++){
     Serial.print(msp[i]); Serial.print("_");
   }
   Serial.println();
#endif
}


