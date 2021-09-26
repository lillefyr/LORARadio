/*
   RadioLib SX127x Transmit Example

   This example transmits packets using SX1278 LoRa radio module.
   Each packet contains up to 256 bytes of data, in the form of:
    - Arduino String
    - null-terminated char array (C-string)
    - arbitrary binary data (byte array)

   Other modules from SX127x/RFM9x family can also be used.

   For full API reference, see the GitHub Pages
   https://jgromes.github.io/RadioLib/
*/

// include the library
#include <RadioLib.h>

// SX1278 has the following connections:
// NSS pin:   10
// DIO0 pin:  2
// RESET pin: 9
// DIO1 pin:  3
SX1278 radio = new Module(10, 2, 9, 3);

MorseClient morse(&radio);

void setup() {
  Serial.begin(9600);

  // initialize SX1278 with default settings
  Serial.print(F("[SX1278] Initializing ... "));
  
#define CALLSIGN_DEFAULT                                "OZ1AVU"
#define CARRIER_FREQUENCY                               436.7f       /*!< MHz */
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

  int state = radio.beginFSK(CARRIER_FREQUENCY,
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
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
  
  uint8_t syncWordFSK[2] = {SYNC_WORD, SYNC_WORD};
  state = radio.setSyncWord(syncWordFSK, 2);
  
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }
  
  state = morse.begin(436.7, 10);
}

/*
const char* MorseTable[] = {
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
        // space, !, ", #, $, %, &, '
        NULL, "-.-.--", ".-..-.", NULL, NULL, NULL, NULL, ".----.",
        // ( ) * + , - . /
        "-.--.", "-.--.-", NULL, ".-.-.", "--..--", "-....-", ".-.-.-", "-..-.",
        // 0 1 2 3 4 5 6 7
        "-----", ".----", "..---", "...--", "....-", ".....", "-....", "--...",
        // 8 9 : ; < = > ?
        "---..", "----.", "---...", "-.-.-.", NULL, "-...-", NULL, "..--..",
        // @ A B C D E F G
        ".--.-.", ".-", "-...", "-.-.", "-..", ".", "..-.", "--.",
        // H I J K L M N O
        "....", "..", ".---", "-.-", ".-..", "--", "-.", "---",
        // P Q R S T U V W
        ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--",
        // X Y Z [ \ ] ^ _
        "-..-", "-.--", "--..", NULL, NULL, NULL, NULL, "..--.-",
        // ' a b c d e f g
        NULL, ".-", "-...", "-.-.", "-..", ".", "..-.", "--.",
        // h i j k l m n o
        "....", "..", ".---", "-.-", ".-..", "--", "-.", "---",
        // p q r s t u v w
        ".--.", "--.-", ".-.", "...", "-", "..-", "...-", ".--",
        // x y z { | } ~ DEL
        "-..-", "-.--", "--..", NULL, NULL, NULL, NULL, NULL,
        };

int dotLength = 100;
int dashLength = dotLength*3;

void setup() {
  // put your setup code here, to run once:
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(57600);
  Serial.println();
  Serial.println("morseTransmitter");
}

void loop() {
  char ch;
  if(Serial.available()){
    ch = Serial.read();
    flashDashDot(MorseTable[ch]);
    Serial.print(ch);
    delay(dotLength*2);
  }
}

void flashDashDot(const char * morseCode)
{
  int i = 0;
  while(morseCode[i] != 0)
  {
    if(morseCode[i] == '.'){
      dot();
    } else if (morseCode[i] == '-'){
      dash();
    }
    i++;
  }
}

void dot()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(dotLength);
  digitalWrite(LED_BUILTIN, LOW);
  delay(dotLength);
}

void dash()
{
  digitalWrite(LED_BUILTIN, HIGH);
  delay(dashLength);
  digitalWrite(LED_BUILTIN, LOW);
  delay(dotLength);
}
*/
// 20 wpm == 100 tegn pr 60 sec
void loop(){
  // send start signal first
//  morse.startSignal();

  Serial.println('-');
  morse.print('t');
  delay(1000);
  Serial.println('.');
  morse.print('e');
  delay(1000);
  
//  Serial.println(F("done!"));
//
  // wait for a second before transmitting again
//  delay(1000);
//  Serial.println("Hello World");
//  int state = radio.transmit("Hello World");
//  delay(1000);
}
