
#include <RadioLib.h>
#define CALLSIGN_DEFAULT                                "OZ1AVU"
#define CARRIER_FREQUENCY                               436.7f       /*!< MHz */
#define SYNC_WORD                                       0x12        /*!< Ensure this sync word is compatable with all devices. */
#define TCXO_VOLTAGE                                    0.0         /*1.6         /*!< Sets the radio's TCX0 voltage. (V) */
#define MAX_NUM_OF_BLOCKS                               3           /*!< maximum number of AES128 blocks that will be accepted */
#define FSK_RECEIVE_WINDOW_LENGTH                       20          /*!< How long to listen out for FSK transmissions for (s) */
#define RESPONSE_DELAY                                  1000        /*!< How long to wait for before responding/processing a transmission (ms) */

// GFSK
#define FSK_BIT_RATE                                    9.6         /*!< kbps nominal */
#define FSK_FREQUENCY_DEVIATION                         5.0         /*!< kHz single-sideband */
#define FSK_RX_BANDWIDTH                                125         /*!< kHz single-sideband */
#define FSK_OUTPUT_POWER                                20          /*!< dBm */
#define FSK_CURRENT_LIMIT                               140.0       /*!< mA */
#define FSK_PREAMBLE_LENGTH                             16          /*!< bits */
#define FSK_DATA_SHAPING                                1.0   /*0.5         /*!< GFSK filter BT product */

#define USE_SX126X

#ifdef USE_SX126X
SX1268 radio = new Module(5, 27, 14, 26);  // nss, DIO1, rst, busy
#endif

// flags
volatile bool interruptEnabled = true;
volatile bool transmissionReceived = false;

// radio ISR
void onInterrupt() {
  if (!interruptEnabled) {
    return;
  }

  transmissionReceived = true;
}


int16_t setGFSK() {
  int state = radio.beginFSK(CARRIER_FREQUENCY,
                             FSK_BIT_RATE,
                             FSK_FREQUENCY_DEVIATION,
                             FSK_RX_BANDWIDTH,
                             FSK_OUTPUT_POWER,
                             FSK_CURRENT_LIMIT,
                             FSK_PREAMBLE_LENGTH);

  radio.setDataShaping(FSK_DATA_SHAPING);
  uint8_t syncWordFSK[2] = {SYNC_WORD, SYNC_WORD};
  radio.setSyncWord(syncWordFSK, 2);
  #ifdef USE_SX126X
    radio.setCRC(2);
  #else
    radio.setCRC(true);
  #endif
  return (state);
}

void setup() {
  Serial.begin(9600);
  Serial.println(F("SX126x_Receive_Interrupt_lolind32_Morse"));
  
  int state = setGFSK();
  
  #ifdef USE_SX126X
    radio.setDio1Action(onInterrupt); 
  #else
    radio.setDio0Action(onInterrupt);
  #endif
  
  // begin listening for packets
  radio.startReceive();
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

void loop3(){
  
    // receive FSK packet
  String str;
  int state = radio.receive(str);

  if (state == ERR_NONE) {
    Serial.println(F("[SX1278] Received packet!"));
    Serial.print(F("[SX1278] Data:\t"));
    Serial.println(str);
  } else if (state == ERR_RX_TIMEOUT) {
    //Serial.println(F("[SX1278] Timed out while waiting for packet!"));
  } else {
    Serial.println(F("[SX1278] Failed to receive packet, code "));
    Serial.println(state);
  }
}

void loop() {
  if(receivedFlag) {
    // disable the interrupt service routine while
    // processing the data
    enableInterrupt = false;

    // reset flag
    receivedFlag = false;

    // you can read received data as an Arduino String
    String str;
    int state = radio.readData(str);

    // you can also read received data as byte array
    /*
      byte byteArr[8];
      int state = radio.receive(byteArr, 8);
    */

    if (state == ERR_NONE) {
      // packet was successfully received
      Serial.println(F("[SX1262] Received packet!"));

      // print data of the packet
      Serial.print(F("[SX1262] Data:\t\t"));
      Serial.println(str);

      // print RSSI (Received Signal Strength Indicator)
      Serial.print(F("[SX1262] RSSI:\t\t"));
      Serial.print(radio.getRSSI());
      Serial.println(F(" dBm"));

      // print SNR (Signal-to-Noise Ratio)
      Serial.print(F("[SX1262] SNR:\t\t"));
      Serial.print(radio.getSNR());
      Serial.println(F(" dB"));

    } else if (state == ERR_CRC_MISMATCH) {
      // packet was received, but is malformed
      Serial.println(F("CRC error!"));

    } else {
      // some other error occurred
      //Serial.print(F("failed, code "));
      //Serial.println(state);
    }

    // put module back to listen mode
    delay(10);
    radio.startReceive();

    // we're ready to receive more packets,
    // enable interrupt service routine
    enableInterrupt = true;
  }
}
