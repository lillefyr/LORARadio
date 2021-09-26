#include <RadioLib.h>
#define NSSpin (10)
#define BUSYpin (8)
#define RESETpin (9)
#define DIO1pin (2)
#define TCXO_VOLTAGE  0.0f
SX1268 fsk = new Module(NSSpin, DIO1pin, RESETpin, BUSYpin);

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
  Serial.println(F("SX126x_Receive_atmega328_standalone_ok_minimal"));
  int state = fsk.beginFSK(436.7f, 9.6f, 5.0f, 156.2f, 20, 140, 16, 0.5f, 0.0f, false );
  uint8_t syncWord[] = { 0x2D, 0x01 };  // Hardcoded in RadioLib SX127x.cpp  
  state = fsk.setSyncWord(syncWord, 2);
//  state = fsk.setEncoding(0);
//  fsk.setDio1Action(setFlag);
//  state = fsk.startReceive();
//  Serial.println(state);
//  enableInterrupt = true;
//  receivedFlag = false;
}

void loop() {
/*  
  if(receivedFlag) {
    enableInterrupt = false;
    fsk.clearDio1Action();
    receivedFlag = false;
    String str;
    int state = fsk.readData(str);
    if (state == ERR_NONE) {
      Serial.println(str);
    }
    fsk.setDio1Action(setFlag);
    fsk.startReceive();
    enableInterrupt = true;
  }
  */
  String str;
  int state = fsk.receive(str);
  if (state == ERR_NONE) {
      Serial.println(str);
  }else{
      Serial.println(state);
  }
}
