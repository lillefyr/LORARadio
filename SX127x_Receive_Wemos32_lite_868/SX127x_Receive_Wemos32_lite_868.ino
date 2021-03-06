/*
   RadioLib SX127x Receive Example

   This example listens for LoRa transmissions using SX127x Lora modules.
   To successfully receive data, the following settings have to be the same
   on both transmitter and receiver:
    - carrier frequency
    - bandwidth
    - spreading factor
    - coding rate
    - sync word
    - preamble length

   Other modules from SX127x/RFM9x family can also be used.

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>
/*#include <radiolibErrorMessages.h>*/

// SX1278 has the following connections here!!!
// NSS pin:   5
// DIO0 pin:  14
// DIO1 pin:  27
// DI02 pin:  26
SX1276 lora = new Module(5,14,12,27);
//SX1268 lora = new Module(5,14,12,27);
/*
RFM95 freq, 862.0, 1020.0      *)
RFM96 freq, 410.0, 525.0
SX1262 freq, 150.0, 960.0
SX1268 freq, 410.0, 810.0
SX1272 freq, 860.0, 1020.0
SX1276 freq, 137.0, 1020.0     *)
SX1277 freq, 137.0, 1020.0
SX1278 freq, 137.0, 525.0
SX1279 freq, 137.0, 960.0
*/

void setup() {
  Serial.begin(115200);
  delay(3000);

  Serial.println(F("\nSX127x_Receive_Wemos32_lite_868"));
#define CARRIER_FREQUENCY       436.7 //868.1  // MHz
#define BANDWIDTH               125.0  // kHz dual sideband
#define SPREADING_FACTOR        7
#define CODING_RATE             8       // 4/8, Extended Hamming
#define OUTPUT_POWER            20      // dBm
#define CURRENT_LIMIT           120     // mA
#define PREAMBLE_LENGTH         8
#define SYNC_WORD               0x12 // make sure you have the matching SyncWord in your Groundstation
#define GAIN                    0

  int state = lora.begin(CARRIER_FREQUENCY, 
                          BANDWIDTH, 
                          SPREADING_FACTOR, 
                          CODING_RATE, 
                          SYNC_WORD, 
                          OUTPUT_POWER, 
                          PREAMBLE_LENGTH, 
                          GAIN);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true) { delay(10); }
  }
}

void loop() {
//  Serial.print(F("[SX1278] Waiting for incoming transmission ... "));

  // you can receive data as an Arduino String
  // NOTE: receive() is a blocking method!
  //       See example ReceiveInterrupt for details
  //       on non-blocking reception method.
  String str;
  int state = lora.receive(str);

  // you can also receive data as byte array
  /*
    byte byteArr[8];
    int state = lora.receive(byteArr, 8);
  */

  if ((state == ERR_NONE) && (str.length() > 0 )) {
    // packet was successfully received
    Serial.println(F("success!"));

    // print the data of the packet
    Serial.print(F("[SX1278] Data:\t\t\t"));
    Serial.println(str);

    // print the RSSI (Received Signal Strength Indicator)
    // of the last received packet
    Serial.print(F("[SX1278] RSSI:\t\t\t"));
    Serial.print(lora.getRSSI());
    Serial.println(F(" dBm"));

    // print the SNR (Signal-to-Noise Ratio)
    // of the last received packet
    Serial.print(F("[SX1278] SNR:\t\t\t"));
    Serial.print(lora.getSNR());
    Serial.println(F(" dB"));

    // print frequency error
    // of the last received packet
    Serial.print(F("[SX1278] Frequency error:\t"));
 //   Serial.print(lora.getFrequencyError());
    Serial.println(F(" Hz"));

  } else if (state == ERR_RX_TIMEOUT) {
    // timeout occurred while waiting for a packet
    Serial.println(F("timeout!"));
    delay(10);

  } else if (state == ERR_CRC_MISMATCH) {
    // packet was received, but is malformed
    Serial.println(F("CRC error!"));
  }
}
