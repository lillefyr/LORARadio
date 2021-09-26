#include <RadioLib.h>
#define NSSpin (10)
#define BUSYpin (8)
#define RESETpin (9)
#define DIO1pin (2)

SX1268 lora = new Module(NSSpin, DIO1pin, RESETpin, BUSYpin);

void setup() {
  Serial.begin(9600);
  

  // carrier frequency:           434.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            9
  // coding rate:                 7
  // sync word:                   0x12 (private network)
  // output power:                14 dBm
  // current limit:               60 mA
  // preamble length:             8 symbols
  // TCXO voltage:                1.6 V (set to 0 to not use TCXO)
  // regulator:                   DC-DC (set to true to use LDO)
  // CRC:                         enabled
#define LORA_CARRIER_FREQUENCY                          436.7f  // 413.9f  // MHz
#define LORA_BANDWIDTH                                  125.0f  // kHz dual sideband
#define LORA_SPREADING_FACTOR                           11
#define LORA_CODINGRATE                                  8     // 4/8, Extended Hamming
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
                          (uint8_t)LORA_CURRENT_LIMIT,
                          LORA_PREAMBLE_LENGTH,
                          TCXO_VOLTAGE);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

int cnt=0;
char outbuf[12];
void loop() {
  sprintf(outbuf, "Msgno=%d",cnt++);
  Serial.println(outbuf);
  int state = lora.transmit(outbuf);
  Serial.println(state);
  delay(1000);
}
