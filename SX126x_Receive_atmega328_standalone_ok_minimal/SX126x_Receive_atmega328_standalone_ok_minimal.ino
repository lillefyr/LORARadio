#include <RadioLib.h> 
#define NSSpin (10)
#define BUSYpin (8)
#define RESETpin (9)
#define DIO1pin (2)
#define TCXO_VOLTAGE  0.0f
SX1268 radio = new Module(NSSpin, DIO1pin, RESETpin, BUSYpin);

volatile bool receivedFlag = false;
volatile bool enableInterrupt = true;
void setFlag(void) {
  if(!enableInterrupt) {
    return;
  }

  receivedFlag = true;
}


void setup() {
  Serial.begin(9600);

  // initialize SX1262 with default settings
  Serial.print(F("SX126x_Receive_atmega328_standalone_ok_minimal"));

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
#define TCXO_VOLTAGE                                    0.0f    // allowed voltages

  int state = radio.begin(LORA_CARRIER_FREQUENCY,
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

  radio.setDio1Action(setFlag);
  state = radio.startReceive();
  Serial.println(state);
  enableInterrupt = true;
  receivedFlag = false;
}

void loop() {
//  Serial.print(F("[SX1262] Waiting for incoming transmission ... "));

  if(receivedFlag) {
    enableInterrupt = false;
    radio.clearDio1Action();
    receivedFlag = false;
    String str;
    int state = radio.readData(str);
    if (state == ERR_NONE) {
      Serial.println(str);
    }
    radio.setDio1Action(setFlag);
    radio.startReceive();
    enableInterrupt = true;
  }
/*
  // you can receive data as an Arduino String
  // NOTE: receive() is a blocking method!
  //       See example ReceiveInterrupt for details
  //       on non-blocking reception method.
  String str;
  int state = radio.receive(str);

  // you can also receive data as byte array
  /*
    byte byteArr[8];
    int state = radio.receive(byteArr, 8);
  */
/*
  if (state == ERR_NONE) {
    // packet was successfully received
    Serial.println(F("success!"));

    // print the data of the packet
    Serial.print(F("[SX1262] Data:\t\t"));
    Serial.println(str);

    // print the RSSI (Received Signal Strength Indicator)
    // of the last received packet
    Serial.print(F("[SX1262] RSSI:\t\t"));
    Serial.print(radio.getRSSI());
    Serial.println(F(" dBm"));

    // print the SNR (Signal-to-Noise Ratio)
    // of the last received packet
    Serial.print(F("[SX1262] SNR:\t\t"));
    Serial.print(radio.getSNR());
    Serial.println(F(" dB"));

  } else if (state == ERR_RX_TIMEOUT) {
    // timeout occurred while waiting for a packet
    Serial.println(F("timeout!"));

  } else if (state == ERR_CRC_MISMATCH) {
    // packet was received, but is malformed
    Serial.println(F("CRC error!"));

  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);

  }
*/
}
