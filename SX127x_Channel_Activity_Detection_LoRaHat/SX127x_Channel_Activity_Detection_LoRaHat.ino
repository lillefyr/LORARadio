// include the library
#include <RadioLib.h>

// SX1278 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
SX1276 lora = new Module(10, 2, 3);


void setup() {
  Serial.begin(115200);
  Serial.print(F("\nSX127x_ChannelActivity_Detection_LoRaHat"));
#define LORA_CARRIER_FREQUENCY                          436.7f  // 413.9f  // MHz
#define LORA_BANDWIDTH                                  125.0f  // kHz dual sideband
#define LORA_SPREADING_FACTOR                           11
#define LORA_CODINGRATE                                  5     // 4/8, Extended Hamming
#define LORA_OUTPUT_POWER                               20     // dBm
#define LORA_CURRENT_LIMIT                              120    // mA
#define SYNC_WORD_6X                                    0x12   // RadioLib uses uint8_t for SX126x
#define LORA_PREAMBLE_LENGTH                             8     // Same for Tx and Rx
#define TCXO_VOLTAGE                                    0.0    // allowed voltages

  int state = lora.begin(LORA_CARRIER_FREQUENCY,
                          LORA_BANDWIDTH,
                          LORA_SPREADING_FACTOR,
                          LORA_CODINGRATE,
                          SYNC_WORD_6X,
                          LORA_OUTPUT_POWER,
                          LORA_PREAMBLE_LENGTH);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

void loop() {
  Serial.print(F("[SX1278] Scanning channel for LoRa preamble ... "));

  // start scanning current channel
  int state = lora.scanChannel();

  if (state == PREAMBLE_DETECTED) {
    // LoRa preamble was detected
    Serial.println(F(" detected preamble!"));

  } else if (state == CHANNEL_FREE) {
    // no preamble was detected, channel is free
    Serial.println(F(" channel is free!"));

  }

  // wait 100 ms before new scan
  delay(100);
}
