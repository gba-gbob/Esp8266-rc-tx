#### Esp 8266 tx 2.4G 8 channel transmitter
Uses EEE 802.11 Probe Response Packets to transmit Multiwii Serial Protocol http://www.multiwii.com/wiki/index.php?title=Multiwii_Serial_Protocol 
MSP_SET_RAW_RC commands. Wifi network work in promiscuous mode allowing unassociated low latency transmission for best possible delivery and range.
See Esp 8266 rx for receiver part.  
##### Hardware
1. esp8266 Esp-07 module with external antenna connection
2. ads1115 x2 i2c ADC - for analog inputs
3. sh1106 OLED i2c display
4. analog controls joysticks/switches/buttons, easiest way is to use donor transmitter  
##### Confirmed range
So far tested with 1/2 dipole antenna
~600m in clear sight with no packet loss
~400m in flight.
  
 
 