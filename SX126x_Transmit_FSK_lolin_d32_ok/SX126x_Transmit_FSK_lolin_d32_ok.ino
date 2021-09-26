/*
   RadioLib SX126x Transmit Example

   This example transmits packets using SX1262 LoRa radio module.
   Each packet contains up to 256 bytes of data, in the form of:
    - Arduino String
    - null-terminated char array (C-string)
    - arbitrary binary data (byte array)

   Other modules from SX126x family can also be used.

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>

// SX1268 has the following connections:
// NSS pin:   5
// DIO1 pin:  27
// NRST pin:  14
// BUSY pin:  26
SX1268 fsk = new Module(5, 27, 14, 26);

void setup() {
  Serial.begin(9600);

  // initialize SX1262 with default settings
  Serial.print(F("SX126x_Transmit_FSK_lolin_d32_ok"));
#define FREQUENCY                 434.0   // 436.7f  // 413.9f  // MHz
#define BITRATE                   9.6f    // 48.0 Kbit
#define FREQUENCY_DEVIATION       5.0f    //  50.0f
#define RX_BANDWIDTH              156.2f  // kHz dual sideband
#define OUTPUT_POWER              20      // dBm
#define CURRENT_LIMIT             140     // mA
#define PREAMBLE_LENGTH           16      // Same for Tx and Rx
#define DATA_SHAPING              0.5f    // GFSK filter BT product
#define TCXO_VOLTAGE              0.0f    // allowed voltages
  // regulator:                   DC-DC (set to true to use LDO)
  // sync word:                   0x2D  0x01
  // CRC:                         enabled, CRC16 (CCIT)
#define enableOOK                 false

  int state = fsk.beginFSK( FREQUENCY,
                            BITRATE,
                            FREQUENCY_DEVIATION,
                            RX_BANDWIDTH,
                            OUTPUT_POWER,
                            CURRENT_LIMIT,
                            PREAMBLE_LENGTH,
                            DATA_SHAPING,
                            TCXO_VOLTAGE,
                            false );
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

// Hardcoded in RadioLib SX127x.cpp  
  uint8_t syncWord[] = { 0x2D, 0x01 };
  state = fsk.setSyncWord(syncWord, 2);
  if (state != ERR_NONE) {
    Serial.print(F("Unable to set configuration, code "));
    Serial.println(state);
    while (true);
  }
  fsk.setCRC(2);

  state = fsk.disableAddressFiltering();
  if (state != ERR_NONE) {
    Serial.println(F("Unable to remove address filter, code "));
  }
}

int cnt=0;
char outbuf[12];
void loop() {
  Serial.print(F("[SX1268] Transmitting packet ... "));

  sprintf(outbuf, "Msgno=%d",cnt++);
  Serial.print(outbuf);
  // transmit FSK packet
  int state = fsk.transmit(outbuf);

  if (state == ERR_NONE) {
    Serial.println(F("[SX1268] Packet transmitted successfully!"));
  } else if (state == ERR_PACKET_TOO_LONG) {
    Serial.println(F("[SX1268] Packet too long!"));
  } else if (state == ERR_TX_TIMEOUT) {
    Serial.println(F("[SX1268] Timed out while transmitting!"));
  } else {
    Serial.println(F("[SX1268] Failed to transmit packet, code "));
    Serial.println(state);
  }

  // wait for a second before transmitting again
  delay(1000);
}
