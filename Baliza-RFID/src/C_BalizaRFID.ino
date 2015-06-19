/*------------------------------------------------------------
--------------------------------------------------------------
	LABORATORIO DE SISTEMAS ELECTRONICOS
--------------------------------------------------------------
	DATE: 18/06/2015
--------------------------------------------------------------
	AUTHORS: Rosalia Ramos & Borja Colubi
--------------------------------------------------------------
	FILE: C_BalizaRFID
--------------------------------------------------------------
------------------------------------------------------------*/
#include <SPI.h>
#include <MFRC522.h>
#include <stdint.h>
#include <stdio.h>

//-- DEFINES ------------------------------------------------

#define SS_PIN         10 
#define RST_PIN         9  
#define NR_KNOWN_KEYS   8

#define p_interrupt    6

//-- VARIABLES -----------------------------------------------
MFRC522 mfrc522(SS_PIN, RST_PIN);
boolean detect_RFID(void);

byte knownKeys[NR_KNOWN_KEYS][MFRC522::MF_KEY_SIZE] =  {
    {0xff, 0xff, 0xff, 0xff, 0xff, 0xff}, // FF FF FF FF FF FF 
    {0xa0, 0xa1, 0xa2, 0xa3, 0xa4, 0xa5}, // A0 A1 A2 A3 A4 A5
    {0xb0, 0xb1, 0xb2, 0xb3, 0xb4, 0xb5}, // B0 B1 B2 B3 B4 B5
    {0x4d, 0x3a, 0x99, 0xc3, 0x51, 0xdd}, // 4D 3A 99 C3 51 DD
    {0x1a, 0x98, 0x2c, 0x7e, 0x45, 0x9a}, // 1A 98 2C 7E 45 9A
    {0xd3, 0xf7, 0xd3, 0xf7, 0xd3, 0xf7}, // D3 F7 D3 F7 D3 F7
    {0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff}, // AA BB CC DD EE FF
    {0x00, 0x00, 0x00, 0x00, 0x00, 0x00}  // 00 00 00 00 00 00
};

//------------------------------------------------------------
//                   C_BalizaRFID FUNCTIONS
//------------------------------------------------------------

//-- SETUP ---------------------------------------------------
void setup() {
  pinMode(p_interrupt, OUTPUT);
  digitalWrite(p_interrupt, LOW); 
  
  SPI.begin();
  mfrc522.PCD_Init();

}

//-- LOOP ----------------------------------------------------
void loop() {
    if(detect_RFID()){        
        if(mfrc522.uid.size > 0){
            digitalWrite(p_interrupt, 1);
            mfrc522.uid.size = 0;
        }
    }
  else{
    digitalWrite(p_interrupt, 0);
  }

}

//-- RFID FUNCTIONS -------------------------------------------
boolean detect_RFID(void){
      if ( ! mfrc522.PICC_IsNewCardPresent())
        return false;

      if ( ! mfrc522.PICC_ReadCardSerial())
        return false;
        
     dump_byte_array(mfrc522.uid.uidByte, mfrc522.uid.size);
     return true;  
}

//-------------------------------------------------------------
void dump_byte_array(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
    Serial.println();
}

//-------------------------------------------------------------
// END C_BalizaRFID IMPLEMENTATION
//-------------------------------------------------------------

