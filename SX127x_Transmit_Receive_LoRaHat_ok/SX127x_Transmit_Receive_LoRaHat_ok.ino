
#include <RadioLib.h>

// SX1276 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
//SX1276 radio = new Module(10, 2, 9, 3);
//SX1276 radio = new Module(10, 2, 6);


// MISO=12
// MOSI=11
// SCL =19
// NSS =10
// SYNC_WORD =12
// 
// Carrier Frequency:436.70
// Spreading factor: 10
// Coding rate:      5

SX1276 radio = new Module(10, 2, 9, 3);  // Arduino with LoraHat

float RSSI = 0.0;
float SNR = 0.0;
float FreqErr = 0.0;
int state;
unsigned long lasttime = 0;

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


void setup() {
  Serial.begin(115200);

  // initialize SX1278
  Serial.println(F("\nSX127x_Transmit_Receive_LoRaHat_ok"));

#define LORA_CARRIER_FREQUENCY                          436.7f  // 413.9f  // MHz
#define LORA_BANDWIDTH                                  125.0f  // kHz dual sideband
#define LORA_SPREADING_FACTOR                           11
#define LORA_CODINGRATE                                  5     // 4/8, Extended Hamming
#define LORA_OUTPUT_POWER                                1     // dBm
#define LORA_CURRENT_LIMIT                              120    // mA
#define SYNC_WORD_6X                                    0x12   // RadioLib uses uint8_t for SX126x
#define LORA_PREAMBLE_LENGTH                             8     // Same for Tx and Rx


  state = radio.begin(LORA_CARRIER_FREQUENCY,
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
  
  // put module back to listen mode
  radio.setDio0Action(setFlag);
  radio.startReceive();
  
  // we're ready to receive more packets,
  // enable interrupt service routine
  enableInterrupt = true;
}

int cnt=0;
char outbuf[50];
String str;

void loop() {

  if(receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;
    state = radio.readData(str);

    if (state == ERR_NONE) {
      RSSI = radio.getRSSI();
      SNR = radio.getSNR();
      FreqErr = radio.getFrequencyError();

      Serial.println("data received");
      Serial.println(str);
      sprintf(outbuf, "RSSI=%f SNR=%f FreqErr=%f\0", RSSI, SNR, FreqErr);
      Serial.println(outbuf);
    }
  }

  if ( (lasttime + 3000) < millis()) {
    // wait for 5 seconds before transmitting again

    lasttime = millis();
      
    sprintf(outbuf, "Msgno=%d",cnt++);
    Serial.println(outbuf);
    RSSI = 0.0;
    state = radio.transmit(outbuf);
  
    if (state == ERR_NONE) {
      // print measured data rate
      Serial.print(F("[SX1278] Datarate:\t"));
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
  }

  // put module back to listen mode
  radio.setDio0Action(setFlag);
  radio.startReceive();
  
  // we're ready to receive more packets,
  // enable interrupt service routine
  enableInterrupt = true;
  delay(10);
}
