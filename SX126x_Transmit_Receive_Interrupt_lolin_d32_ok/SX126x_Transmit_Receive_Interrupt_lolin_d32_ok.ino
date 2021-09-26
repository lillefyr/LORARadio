
// include the library
#include <RadioLib.h>

SX1268 radio = new Module(5, 27, 14, 26);
/*
SDA 4
SCL 22
BUILDIN_LED 5
NSS 5
DIO0 0
DIO1 27
BUSY 26
RST 14
SCK 18
MISO 19
MOSI 18
*/
float RSSI = 0.0;
float SNR = 0.0;
float FreqErr = 0.0;
int state;
unsigned long lasttime = 0;
int cnt=0;
String str;
char outbuf[50];

void setup() {
  Serial.begin(115200); 

  // initialize SX1268
  Serial.println(F("SX126x_Receive_Interrupt_lolin_d32_ok"));
#define LORA_CARRIER_FREQUENCY                          436.7f  // 413.9f  // MHz
#define LORA_BANDWIDTH                                  125.0f  // kHz dual sideband
#define LORA_SPREADING_FACTOR                           11
#define LORA_CODINGRATE                                  5     // 4/8, Extended Hamming
#define LORA_OUTPUT_POWER                               20     // dBm
#define LORA_CURRENT_LIMIT                              120    // mA
#define SYNC_WORD_6X                                    0x12   // RadioLib uses uint8_t for SX126x
#define LORA_PREAMBLE_LENGTH                             8     // Same for Tx and Rx
#define TCXO_VOLTAGE                                    0.0    // allowed voltages

  state = radio.begin(LORA_CARRIER_FREQUENCY,
                          LORA_BANDWIDTH,
                          LORA_SPREADING_FACTOR,
                          LORA_CODINGRATE,
                          SYNC_WORD_6X,
                          LORA_OUTPUT_POWER,
                          LORA_PREAMBLE_LENGTH,
                          TCXO_VOLTAGE);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  Serial.println("Send startmessage");
  state = radio.transmit("start");
  if (state == ERR_NONE) {
    // print measured data rate
    Serial.print(F("[SX1268] Datarate:\t"));
    Serial.print(radio.getDataRate());
    Serial.println(F(" bps"));
  } else if (state == ERR_PACKET_TOO_LONG) {
    // the supplied packet was longer than 256 bytes
    Serial.println(F(" too long!"));
  } else if (state == ERR_TX_TIMEOUT) {
    // timeout occured while transmitting packet
    Serial.println(F(" timeout!"));
  } else {
    // some other error occurred
    Serial.print(F("failed, code "));
    Serial.println(state);
  }

  // start listening for LoRa packets
  // set the function that will be called
  // when new packet is received
  radio.setDio1Action(setFlag);
  Serial.print(F("[SX1268] Starting to listen ... "));
  state = radio.startReceive();
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
}

// flag to indicate that a packet was received
volatile bool receivedFlag = false;

// disable interrupt when it's not needed
volatile bool enableInterrupt = true;

// this function is called when a complete packet
// is received by the module
// IMPORTANT: this function MUST be 'void' type
//            and MUST NOT have any arguments!
void setFlag(void) {
  // check if the interrupt is enabled
  if(!enableInterrupt) {
    return;
  }

  // we got a packet, set the flag
  receivedFlag = true;
}

void loop() {
  // check if the flag is set
  if(receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    String str;
    state = radio.readData(str);

    if (state == ERR_NONE) {
      // packet was successfully received
//      Serial.println(F("[SX1268] Received packet!"));

      // print data of the packet

      Serial.print(F("Data: "));
      Serial.println(str);

      // print RSSI (Received Signal Strength Indicator)
//      Serial.print(F("[SX1268] RSSI:\t\t"));
//      Serial.print(radio.getRSSI());
//      Serial.println(F(" dBm"));

      // print SNR (Signal-to-Noise Ratio)
//      Serial.print(F("[SX1268] SNR:\t\t"));
//      Serial.print(radio.getSNR());
//      Serial.println(F(" dB"));
    } else if (state == ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("CRC error!"));
    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);
    }
    
    // put module back to listen mode
    radio.setDio1Action(setFlag);
    radio.startReceive();
    enableInterrupt = true;
  }

  if ( (lasttime + 30000) < millis()) {
    // wait for 5 seconds before transmitting again

    lasttime = millis();

    for (int i=0; i< sizeof(outbuf); i++) { outbuf[i] = 0x00; }
    sprintf(outbuf, "Msgno=%d\0",cnt++);
//    Serial.println(outbuf);
    state = radio.transmit(outbuf);
  
    if (state == ERR_NONE) {
      // print measured data rate
      //Serial.print(F("[SX1278] Datarate:\t"));
      //Serial.print(radio.getDataRate());
      //Serial.println(F(" bps"));
    } else if (state == ERR_PACKET_TOO_LONG) {
      // the supplied packet was longer than 256 bytes
      Serial.println(F(" too long!"));
    } else if (state == ERR_TX_TIMEOUT) {
      // timeout occured while transmitting packet
      Serial.println(F(" timeout!"));
  
    } else {
      // some other error occurred
      Serial.print(F("failed, code "));
      Serial.println(state);
    }
  }

}
