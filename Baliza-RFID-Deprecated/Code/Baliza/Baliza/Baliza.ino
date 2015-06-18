/*The original version of DumpInfo example was modified. This code is not the
 * original.
 * ----------------------------------------------------------------------------
 * This is a MFRC522 library example; see https://github.com/miguelbalboa/rfid
 * for further details and other examples.
 * 
 * NOTE: The library file MFRC522.h has a lot of useful info. Please read it.
 * 
 * Released into the public domain.
 * ----------------------------------------------------------------------------
 * Example sketch/program showing how to read data from a PICC (that is: a RFID
 * Tag or Card) using a MFRC522 based RFID Reader on the Arduino SPI interface.
 * 
 * When the Arduino and the MFRC522 module are connected (see the pin layout
 * below), load this sketch into Arduino IDE then verify/compile and upload it.
 * To see the output: use Tools, Serial Monitor of the IDE (hit Ctrl+Shft+M).
 * When you present a PICC (that is: a RFID Tag or Card) at reading distance
 * of the MFRC522 Reader/PCD, the serial output will show the ID/UID, type and
 * any data blocks it can read. Note: you may see "Timeout in communication"
 * messages when removing the PICC from reading distance too early.
 * 
 * If your reader supports it, this sketch/program will read all the PICCs
 * presented (that is: multiple tag reading). So if you stack two or more
 * PICCs on top of each other and present them to the reader, it will first
 * output all details of the first and then the next PICC. Note that this
 * may take some time as all data blocks are dumped, so keep the PICCs at
 * reading distance until complete.
 * 
 * Typical pin layout used:
 * -----------------------------------------------------------------------------------------
 *             MFRC522      Arduino       Arduino   Arduino    Arduino          Arduino
 *             Reader/PCD   Uno           Mega      Nano v3    Leonardo/Micro   Pro Micro
 * Signal      Pin          Pin           Pin       Pin        Pin              Pin
 * -----------------------------------------------------------------------------------------
 * RST/Reset   RST          9             5         D9         RESET/ICSP-5     RST
 * SPI SS      SDA(SS)      10            53        D10        10               10
 * SPI MOSI    MOSI         11 / ICSP-4   51        D11        ICSP-4           16
 * SPI MISO    MISO         12 / ICSP-1   50        D12        ICSP-1           14
 * SPI SCK     SCK          13 / ICSP-3   52        D13        ICSP-3           15
 */

#include <SPI.h>
#include <MFRC522.h>
#include <stdint.h>
#include <stdio.h>

#define RST_PIN		9		// 
#define SS_PIN		10		//

#define TIMEOUT 1000
#define PACKET_RECEIVED 99

// Packet's type.
#define NEW_GAME_REQUEST 0x0000  // Big-endian 
#define NEW_GAME_REPLY   0x0001
#define DATA_REQUEST     0x0002
#define DATA_REPLY       0x0003
#define ALIVE_REQUEST    0x0004
#define ALIVE_REPLY      0x0005

#define NEW_DRONE_REPLY  0xF001  // Packets which no need request are "0xFXXX"

// Default address. At first, the central node don't know the beacon's ID.
// Thus, it send the identification packet (new_game_request) to default address.
//#define DEFAULT_ADDRESS  0x0000

// All packets must start by these 4 bytes.
#define PCK_HEADER       0x404C5345  //"@LSE"

// Reserved packet field
#define RESERV_FIELD     0x0000

#define HEADERS_LENGTH   16  //4 + 2*6 bytes of header
#define MAX_LENGTH       4096

#define MAX_DATA         (MAX_LENGTH - HEADERS_LENGTH) //4096 - 16

#define MAX_DRONES       10
#define MAX_BEACONS      10

#define WAIT_CHAR        10000 //useconds
#define ALIVE_TIME       120 //seconds

typedef struct packet_t 
{
   uint32_t packet_id;
   uint16_t destination;
   uint16_t source;
   uint16_t number;
   uint16_t length;
   uint16_t packet_type;
   uint16_t reserved;
   uint8_t  data[MAX_DATA];
}packet_t;

typedef uint16_t drones_table_t[MAX_DRONES];
typedef uint16_t beacon_ID_t;
typedef uint32_t drone_ID_t;

typedef struct beacon_element_t 
{
   beacon_ID_t beacon;                     //Beacon ID
   drone_ID_t drones_table[MAX_DRONES];   //Beacon's table of drones
   uint16_t reg_drones;                 //Number of drones registered in the table
}beacon_element_t;

typedef struct beacons_table_t 
{
   beacon_element_t elements[MAX_BEACONS];
   uint16_t reg_elements; //How many beacons has beeen registered
}beacons_table_t;

beacons_table_t create_beacons_table(void);
void send_packet(packet_t packet);
//void parse_received(int serial_fd, beacons_table_t *beacons_table);
//void process_packet(packet_t packet, beacons_table_t *beacons_table);
void process_packet(packet_t packet);
void parse_received();

drone_ID_t data_to_drone_ID(packet_t packet);
void drone_ID_to_data(drone_ID_t drone_ID, packet_t *packet);
//void print_beacon_table(beacons_table_t beacons_table);
beacon_element_t* find_beacon (beacon_ID_t beacon_ID,beacons_table_t *beacons_table);

//Global variables
const uint16_t beaconID = 0x0003;
const uint16_t AllNodesID = 0x0000;
//beacons_table_t beacons_table_instance;
//packet_t packet_instance; //No se poque pero si lo defino como variable global no funciona. Tampoco me deja definirlo dentro de la funcion como "static" 
uint16_t readstate = 0;
uint16_t datapos = 0;
uint16_t registered = 0;
enum state_t {DISCOVER, PLAY} state;
uint16_t myCentralNode = 0;

//Auxiliar functions declaration
char hex_to_char (uint16_t remainder);
uint16_t char_to_hex (char received);

MFRC522 mfrc522(SS_PIN, RST_PIN);	// Create MFRC522 instance

void setup() {
	Serial.begin(9600);		// Initialize serial communications with the PC
	while (!Serial);		// Do nothing if no serial port is opened (added for Arduinos based on ATMEGA32U4)
	SPI.begin();			// Init SPI bus
	mfrc522.PCD_Init();		// Init MFRC522
        state = DISCOVER;
}

void loop() {
	uint16_t quotient;
        uint16_t remainder;
        switch(state)
        {
         case DISCOVER:
              parse_received();
              if (myCentralNode != 0)
              {
                 state = PLAY;
              }
              else
              {
                 state = DISCOVER; 
              }
            break;
         case PLAY:
            // Look for new cards
	    if ( ! mfrc522.PICC_IsNewCardPresent()) {
		return;
	    }

	    // Select one of the cards
	   if ( ! mfrc522.PICC_ReadCardSerial()) {
     		return;
	   }

           if (mfrc522.uid.size != 0)
           {
                 Serial.write((char) (PCK_HEADER >> 24));
                 Serial.write((char) (PCK_HEADER >> 16));
                 Serial.write((char) (PCK_HEADER >> 8));
                 Serial.write((char) (PCK_HEADER));
                 Serial.write((char) (myCentralNode >> 8));
                 Serial.write((char) (myCentralNode));
                 Serial.write((char) (beaconID >> 8));
                 Serial.write((char) (beaconID));
                 Serial.write((char) (0x0001 >> 8));
                 Serial.write((char) (0x0001));
                 Serial.write((char) ((2*mfrc522.uid.size)>> 8)); //Each element, two bytes to send
                 Serial.write((char) (2*mfrc522.uid.size));
                 Serial.write((char) (NEW_DRONE_REPLY >> 8 ));
                 Serial.write((char) (NEW_DRONE_REPLY));
                 Serial.write((char) (0x0000 >> 8));
                 Serial.write((char) (0x0000)); 
                 for (byte i = 0; i < mfrc522.uid.size; i++) {
                     remainder = mfrc522.uid.uidByte[i] % 16;
                     Serial.write(hex_to_char(remainder));
                     quotient = quotient / 16;

                     remainder = quotient % 16;
                     Serial.write(hex_to_char(remainder));
                 }
           }
            state = PLAY;
            break;
         default:
            state = DISCOVER;
            break;
      }
}


void send_packet(packet_t packet)
{
   uint16_t pck_length = HEADERS_LENGTH + packet.length; //Number of packet bytes

   //Conversion: packet_t to char array
   char packet_a[MAX_LENGTH];
   
   packet_a[0]  = (char) (packet.packet_id >> 24);
   packet_a[1]  = (char) (packet.packet_id >> 16);
   packet_a[2]  = (char) (packet.packet_id >> 8);
   packet_a[3]  = (char) (packet.packet_id);
   packet_a[4]  = (char) (packet.destination >> 8);
   packet_a[5]  = (char) (packet.destination);
   packet_a[6]  = (char) (packet.source >> 8);
   packet_a[7]  = (char) (packet.source);
   packet_a[8]  = (char) (packet.number >> 8);
   packet_a[9]  = (char) (packet.number);
   packet_a[10] = (char) (packet.length >> 8);
   packet_a[11] = (char) (packet.length);
   packet_a[12] = (char) (packet.packet_type >> 8);
   packet_a[13] = (char) (packet.packet_type);
   packet_a[14] = (char) (packet.reserved >> 8);
   packet_a[15] = (char) (packet.reserved);

   /*int i;
   for (i = 0; i < packet.length; i++)
   {
     packet_a[16 + i] = (char) packet.data[i];
   }*/

  //Serial.write(packet_a); //Casca el programa
}

void parse_received()
{
  /*Existe un problema: Al definir la estructura packet_t como
    una variable global o static dentro de una funcin, el programa
    deja de funcionar, por eso para mantener los valores entre llamadas
    se han definido como static los diferenetes campos por separado.
    Se ha detectado que cuando se define el campo data que es un
    array como static y se intenta acceder a l tmb casca el programa*/
   static uint32_t spacket_id;
   static uint16_t sdestination;
   static uint16_t ssource;
   static uint16_t snumber;
   static uint16_t slength;
   static uint16_t spacket_type;
   static uint16_t sreserved;
   static char  sdata[MAX_DATA];
   
   packet_t packet_instance;
   packet_t packet_to_send;
   char rec;
   uint8_t to_send;
   
   rec=Serial.read(); //Read one character
   if (rec > -1 )
   {
      switch (readstate) 
       {
          case 0: // first sync byte
             if (rec=='@') 
             {
                spacket_id = rec;
                readstate++;
             } else {
                readstate = 0;
             }

	     break;
          case 1:
             if (rec=='L')
             {
                spacket_id = (spacket_id << 8) + rec;
                readstate++;
             } else if (rec=='@') {
                readstate = 1;
             } else {
                readstate = 0;
             }

	     break;
          case 2:
             if (rec=='S')
             {
                spacket_id = (spacket_id << 8) + rec;
                readstate++;
             } else if (rec=='@') {
                readstate = 1;
             } else {
                readstate = 0;
             }
	     break;
          case 3:
             if (rec=='E')
             {
                spacket_id = (spacket_id << 8) + rec;
                readstate++;
             } else if (rec=='@') {
                readstate = 1;
             } else {
                readstate = 0;
             }
	     break;
          case 4:
             sdestination = (uint16_t) rec;
             readstate++;  
	     break;
          case 5:
             sdestination = (sdestination << 8) + (uint16_t) rec;
             readstate++;  
	     break;
          case 6:
             ssource = rec;
             readstate++;  
	     break;
          case 7:
             ssource = (ssource << 8) + rec;
             readstate++;  
	     break;
          case 8:
             snumber = rec;
             readstate++;  
	     break;
          case 9:
             snumber = (snumber << 8) + rec;
             readstate++;  
	     break;
          case 10:
             slength = rec;
             readstate++;  
	     break;
          case 11:
             slength = (slength << 8) + rec;
             readstate++;  
	     break;
          case 12:
             spacket_type = rec;
             readstate++;  
	     break;
          case 13:
             spacket_type = (spacket_type << 8) + rec;
             readstate++;  
	     break;
          case 14:
             sreserved = rec;
             readstate++;  
	     break;
          case 15:
             sreserved = (sreserved << 8) + rec;
             if (slength == 0)
             {
                readstate = PACKET_RECEIVED;
             } 
             else 
             {
                datapos = 0;
                readstate++; 
             }
	     break;
          case 16:
             //sdata[1] = rec; //No se por que pero aqu casca. Como el nodo central no enva datos por ahora no pasa nada quitando esta sentencia
             datapos++;
             if(datapos == slength)
             {
                datapos = 0;
                readstate = PACKET_RECEIVED;
             }  
	     break;
          default: 
             readstate=0;
             break;
      }
      
     if (readstate == PACKET_RECEIVED) 
     {
        packet_instance.packet_id = spacket_id;
        packet_instance.destination = sdestination;
        packet_instance.source = ssource;
        packet_instance.number = snumber;
        packet_instance.length = slength;
        packet_instance.packet_type = spacket_type;
        packet_instance.reserved = sreserved;

           //Serial.println(packet_instance.packet_type);
           //Serial.println(NEW_GAME_REQUEST);
        //Process packet
        if ((packet_instance.destination == beaconID) || (packet_instance.destination == AllNodesID))
        {

           switch (packet_instance.packet_type)
           {
              case NEW_GAME_REQUEST: //Add beacon to the beacons list (table)
                 /*packet_to_send.packet_id = (char) PCK_HEADER;
                 packet_to_send.destination = (char) packet_instance.source;
                 packet_to_send.source = (char) beaconID;
                 packet_to_send.number = (char) 0x0001; //No necessary in this implementation
                 packet_to_send.length = (char) 0x0000;
                 packet_to_send.packet_type = (char) NEW_GAME_REPLY;
                 packet_to_send.reserved = (char) 0x0000;*/
                 
                 Serial.write((char) (PCK_HEADER >> 24));
                 Serial.write((char) (PCK_HEADER >> 16));
                 Serial.write((char) (PCK_HEADER >> 8));
                 Serial.write((char) (PCK_HEADER));
                 Serial.write((char) (packet_instance.source >> 8));
                 Serial.write((char) (packet_instance.source));
                 Serial.write((char) (beaconID >> 8));
                 Serial.write((char) (beaconID));
                 Serial.write((char) (0x0001 >> 8));
                 Serial.write((char) (0x0001));
                 Serial.write((char) (0x0000 >> 8));
                 Serial.write((char) (0x0000));
                 Serial.write((char) (NEW_GAME_REPLY >> 8));
                 Serial.write((char) (NEW_GAME_REPLY));
                 Serial.write((char) (0x0000 >> 8));
                 Serial.write((char) (0x0000));
                 myCentralNode = packet_instance.source;

                 break;
              default:
                 break;
           }
        }
        readstate=0;
     }

   }
}
void process_packet(packet_t packet)
{

   int i;
   drone_ID_t drone_ID;
   beacon_element_t *beacon_element;
   packet_t packet_to_send;
   uint16_t pck_length = HEADERS_LENGTH + packet.length;
   if ((packet.destination != beaconID) && (packet.destination != AllNodesID))
   {
       return; //The packet is not for these device
   }

   switch (packet.packet_type)
   {
      case NEW_GAME_REQUEST: //Add beacon to the beacons list (table)
        packet_to_send.packet_id = (char) PCK_HEADER;
        packet_to_send.destination = (char) packet.source;
        packet_to_send.source = (char) beaconID;
        packet_to_send.number = (char) 0x0001; //No necessary in this implementation
        packet_to_send.length = (char) 0x0000;
        packet_to_send.packet_type = (char) NEW_GAME_REPLY;
        packet_to_send.reserved = (char) 0x0000;
        //send_packet(packet_to_send);
        break;
      default:
        break;
   }
   return;
}

beacons_table_t create_beacons_table(void)
{
   beacons_table_t beacons_table;
   int i;
   for(i = 0;i < MAX_BEACONS; i++)
   {
      //Create empty drones_table for each possible beacon
      beacons_table.elements[i].reg_drones = 0;
   }
   beacons_table.reg_elements = 0;
   return beacons_table;
}

drone_ID_t data_to_drone_ID(packet_t packet)
{
   drone_ID_t drone_ID;
   drone_ID =  ((char_to_hex(packet.data[0])*16 + char_to_hex(packet.data[1]))*16 + char_to_hex(packet.data[2]))*16 + char_to_hex(packet.data[3]);
   return drone_ID;
}

//Hay que pasar packet mediante puntero ya que sino solo se pasa una copia del contenido, por referencia vs por valor
void drone_ID_to_data(drone_ID_t drone_ID, packet_t *packet)
{
   uint16_t quotient;
   uint16_t remainder;

   remainder = drone_ID % 16;
   packet->data[3] = hex_to_char(remainder);
   quotient = drone_ID / 16;

   remainder = quotient % 16;
   packet->data[2] = hex_to_char(remainder);
   quotient = quotient / 16;

   remainder = quotient % 16;
   packet->data[1] = hex_to_char(remainder);
   quotient = quotient / 16;

   remainder = quotient % 16;
   packet->data[0] = hex_to_char(remainder);
}

char hex_to_char (uint16_t remainder)
{
   if (remainder < 10)
   {
      return (remainder + 48);
   }
   else
   {
      return (remainder + 55);  // 65 - 10
   }
}

uint16_t char_to_hex (char received)
{
   if ((uint16_t)received < 58)
   {
      return ((uint16_t)received - 48);
   }
   else
   {
      return ((uint16_t)received - 55);  // 65 - 10
   }
}

//We need the memory address of the original table, not a copy "*beacons_table"
beacon_element_t* find_beacon (beacon_ID_t beacon_ID,beacons_table_t *beacons_table)
{
   int i;
   beacon_element_t *p;
   //printf("Tabla: %p\n\r",beacons_table);
   for (i = 0; i < beacons_table->reg_elements; i++)
   {
      if(beacons_table->elements[i].beacon == beacon_ID)
      {
          p = &(beacons_table->elements[i]);
          //printf("Elemento %d: %p\n\r",i,p);
          return p;
      }
   }

   //Didn't find a beacon with this ID registered, error
   return NULL;
}

/*void print_beacon_table(beacons_table_t beacons_table)
{
   int i;
   int k;
   //printf("************ Beacons table *************\n\r");
   for (i = 0; i < beacons_table.reg_elements; i++)
   {
       printf("****************************************\n\r");
       printf("******* Beacon: %016d *******\n\r",beacons_table.elements[i].beacon);
       printf("****************************************\n\r");
       for(k = 0; k < beacons_table.elements[i].reg_drones; k++)
       {
          printf("----------------------------------------\n\r");
          printf("------- Drone: %016d --------\n\r",beacons_table.elements[i].drones_table[k]);
          printf("----------------------------------------\n\r");
       }
       printf("\n\r");
       printf("\n\r");
   }
}*/

