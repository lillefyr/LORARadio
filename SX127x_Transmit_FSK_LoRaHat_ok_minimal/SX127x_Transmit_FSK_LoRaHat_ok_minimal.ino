#include <RadioLib.h>

#define MOSI 11
#define MISO 12
#define SCK 13

#define NSSpin (10)
#define DIO0pin (2)
#define RESETpin (9)
#define DIO1pin (3)
SX1278 fsk = new Module(10, 2, 9, 3);

void setup() {
  Serial.begin(115200);
  int state = fsk.beginFSK( 434.0f, 9.6f, 5.0f, 125.0f, 20, 140, 16, false );
  uint8_t syncWord[] = { 0x2D, 0x01 };  // Hardcoded in RadioLib SX127x.cpp  
  state = fsk.setSyncWord(syncWord, 2);
}

int cnt=0;
char outbuf[12];
void loop() {
  sprintf(outbuf, "Msgno=%d",cnt++);
  Serial.println(outbuf);
  int state = fsk.transmit(outbuf); 
  delay(1000);
}
