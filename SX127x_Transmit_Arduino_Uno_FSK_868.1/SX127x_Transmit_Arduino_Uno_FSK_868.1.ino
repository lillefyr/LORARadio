// include the library
#include <RadioLib.h>

// SX1276 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
SX1276 fsk = new Module(10, 2, 9, 3);

volatile bool dataTransmitedFlag = false;
volatile bool enableInterrupt = true;
int transmissionState = ERR_NONE;

byte transmitArray[] = { 0xA1, 0xA2, 0xA3, 0xA4, 0xA5, 0xA6, 0xA7, 0xA8,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38,
    0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
    0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
    0x61, 0x62, 0x63, 0x64, 0x65, 0x66, 0x67, 0x68,
    0x71, 0x72, 0x73, 0x74, 0x75, 0x76, 0x77, 0x78,
    0x81, 0x82, 0x83, 0x84, 0x85, 0x86, 0x87 };
size_t arraySize = sizeof(transmitArray);

void IRQ_flag(void) {

    if(!enableInterrupt) {
        return;
    }
    dataTransmitedFlag = true;
}


void setup() {
  Serial.begin(57600);

  Serial.println();
  Serial.println("sx127x_Transmit_Arduino_Uno_FSK_868.1");
  
#define CALLSIGN_DEFAULT                                "OZ1AVU"
#define CARRIER_FREQUENCY       868.7  // MHz
#define BITRATE                 48.0 //kbps
#define FREQUENCY_DEVIATION     50.0 //kHz
#define BANDWIDTH               125.0f  // kHz dual sideband
#define OUTPUT_POWER           13      // dBm
#define CURRENT_LIMIT          100     // mA
#define PREAMBLE_LENGTH        8U
#define SYNC_WORD                                       0x12        /*!< Ensure this sync word is compatable with all devices. */
#define TCXO_VOLTAGE                                    0.0         /*1.6         /*!< Sets the radio's TCX0 voltage. (V) */
#define MAX_NUM_OF_BLOCKS                               3           /*!< maximum number of AES128 blocks that will be accepted */
#define FSK_RECEIVE_WINDOW_LENGTH                       20          /*!< How long to listen out for FSK transmissions for (s) */
#define RESPONSE_DELAY                                  1000        /*!< How long to wait for before responding/processing a transmission (ms) */

// GFSK
#define FSK_BIT_RATE                                   9.6         /*!< kbps nominal */
#define FSK_FREQUENCY_DEVIATION                         5.0         /*!< kHz single-sideband */
#define FSK_RX_BANDWIDTH                                125         /*!< kHz single-sideband */
#define FSK_OUTPUT_POWER                                20          /*!< dBm */
#define FSK_CURRENT_LIMIT                               140.0       /*!< mA */
#define FSK_PREAMBLE_LENGTH                             16          /*!< bits */
#define FSK_DATA_SHAPING                                1.0   /*0.5         /*!< GFSK filter BT product */

  int state = fsk.beginFSK(CARRIER_FREQUENCY,
                             FSK_BIT_RATE,
                             FSK_FREQUENCY_DEVIATION,
                             FSK_RX_BANDWIDTH,
                             FSK_OUTPUT_POWER,
                             FSK_CURRENT_LIMIT,
                             FSK_PREAMBLE_LENGTH,
                             false);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed1, code "));
    Serial.println(state);
    Serial.println(CARRIER_FREQUENCY);
    while (true);
  }
  
    uint8_t syncWord[] = {0x81, 0x46, 0xCD, 0xAE, 0x98, 0x47, 0x54, 0xCF};
    state = fsk.setSyncWord(syncWord, 8);
  
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed2, code "));
    Serial.println(state);
    while (true);
  }

  fsk.setDio1Action(IRQ_flag);
  Serial.print(F("Sending packet ... "));
  transmissionState= fsk.startTransmit(transmitArray, arraySize);
}

void loop() {

//    if(dataTransmitedFlag){

        enableInterrupt = false;
        dataTransmitedFlag = false;

        if (transmissionState == ERR_NONE) {
            // packet was successfully sent
            Serial.print(F("transmission finished!\n"));

        } else {
            Serial.print(F("failed, code "));
            Serial.println(transmissionState);
        }

        Serial.print(F("Sending packet ... "));
        transmissionState= fsk.startTransmit(transmitArray, arraySize);
        
        transmissionState= fsk.startTransmit(transmitArray, arraySize);
        transmissionState= fsk.startTransmit(transmitArray, arraySize);
        transmissionState= fsk.startTransmit(transmitArray, arraySize);
        transmissionState= fsk.startTransmit(transmitArray, arraySize);

        enableInterrupt = true;
        Serial.println("start delay");
        delay(10000);
        Serial.println("end delay");
//    }
}
