#include <RadioLib.h>
#define NSSpin (10)
#define BUSYpin (8)
#define RESETpin (9)
#define DIO1pin (2)
#define TCXO_VOLTAGE  0.0f
SX1268 fsk = new Module(NSSpin, DIO1pin, RESETpin, BUSYpin);

void setup() {
  Serial.begin(9600);
  int state = fsk.beginFSK(436.7f, 9.6f, 5.0f, 156.2f, 20, 140, 16, 0.5f, 0.0f, false );
  uint8_t syncWord[] = { 0x2D, 0x01 };  // Hardcoded in RadioLib SX127x.cpp  
  state = fsk.setSyncWord(syncWord, 2);
//  state = fsk.setEncoding(0);
}

int cnt=0;
char outbuf[12];
void loop() {
  sprintf(outbuf, "Msgno=%d",cnt++);
  Serial.println(outbuf);
  int state = fsk.transmit(outbuf);
  Serial.println(state);
  delay(1000);
}
