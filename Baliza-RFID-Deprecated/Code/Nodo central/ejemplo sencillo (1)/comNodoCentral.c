//These implementation assumes that medium interface 
//is a USB implementing serial communication
#include <stdint.h>
#include <stdio.h>
#include "comNodoCentral.h"
#include "serial.h"

#define TIMEOUT 1000
#define PACKET_RECEIVED 99

//Global variables
const uint16_t NodoCentralID = 0x0001;
const uint16_t AllNodesID = 0x0000;
//beacons_table_t beacons_table_instance;
packet_t packet_instance; 
uint16_t readstate = 0;
uint16_t datapos = 0;
uint16_t registered = 0;

//Auxiliar functions declaration
char hex_to_char (uint16_t remainder);
uint16_t char_to_hex (char received);

void send_packet(int serial_fd, packet_t packet)
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

   int i;
   for (i = 0; i < packet.length; i++)
   {
     packet_a[16 + i] = (char) packet.data[i];
   }

   serial_send(serial_fd,packet_a,pck_length);
}

void parse_received(int serial_fd, beacons_table_t *beacons_table)
{
   int n;
   char rec;
   n=serial_read(serial_fd,&rec,1,TIMEOUT); //Read one character
   if (n == 1)
   {
       printf("%02x\r\n",rec);
       switch (readstate) 
       {
          case 0: // first sync byte
             if (rec=='@') 
             {
                packet_instance.packet_id = rec;
                readstate++;
             } else {
                readstate = 0;
             }

	     break;
          case 1:
             if (rec=='L')
             {
                packet_instance.packet_id = (packet_instance.packet_id << 8) + rec;
                readstate++;
             } else {
                readstate = 0;
             }
	     break;
          case 2:
             if (rec=='S')
             {
                packet_instance.packet_id = (packet_instance.packet_id << 8) + rec;
                readstate++;
             } else {
                readstate = 0;
             }
	     break;
          case 3:
             if (rec=='E')
             {
                packet_instance.packet_id = (packet_instance.packet_id << 8) + rec;
                readstate++;
             } else {
                readstate = 0;
             }
	     break;
          case 4:
             packet_instance.destination = rec;
             readstate++;  
	     break;
          case 5:
             packet_instance.destination = (packet_instance.destination << 8) + rec;
             readstate++;  
	     break;
          case 6:
             packet_instance.source = rec;
             readstate++;  
	     break;
          case 7:
             packet_instance.source = (packet_instance.source << 8) + rec;
             readstate++;  
	     break;
          case 8:
             packet_instance.number = rec;
             readstate++;  
	     break;
          case 9:
             packet_instance.number = (packet_instance.number << 8) + rec;
             readstate++;  
	     break;
          case 10:
             packet_instance.length = rec;
             readstate++;  
	     break;
          case 11:
             packet_instance.length = (packet_instance.length << 8) + rec;
             readstate++;  
	     break;
          case 12:
             packet_instance.packet_type = rec;
             readstate++;  
	     break;
          case 13:
             packet_instance.packet_type = (packet_instance.packet_type << 8) + rec;
             readstate++;  
	     break;
          case 14:
             packet_instance.reserved = rec;
             readstate++;  
	     break;
          case 15:
             packet_instance.reserved = (packet_instance.reserved << 8) + rec;
             readstate++;  
	     break;
          case 16:
             packet_instance.data[datapos] = rec;
             datapos++;
             if(datapos == packet_instance.length)
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
        process_packet(packet_instance, beacons_table);
        readstate=0;
     }
      
   }
}
void process_packet(packet_t packet, beacons_table_t *beacons_table)
{
   int i;
   drone_ID_t drone_ID;
   beacon_element_t *beacon_element;
   if (packet.destination != NodoCentralID)
   {
       return; //The packet is not for these device
   }

   switch (packet.packet_type)
   {
      case NEW_GAME_REPLY: //Add beacon to the beacons list (table)
        //Is the beacon already in the table?
        registered = 0;
        for (i = 0; i <= beacons_table->reg_elements; i++)
        {
           if(beacons_table->elements[i].beacon == packet.source)
           {
              registered = 1;
           }
        }

        if ((beacons_table->reg_elements < MAX_BEACONS) && (registered == 0))
        {
           //Register new beacon in the table
           //The drones tables must be created at the begining of the game,
           //although now are empty
           beacons_table->elements[beacons_table->reg_elements].beacon = packet.source;
           beacons_table->reg_elements++;
        }
        /*if (beacons_table->reg_elements < MAX_BEACON)
        {
           //Error (too many beacons) => log error
        }*/
        //If the packet contains data, it will be ignored.
        break;
      case DATA_REPLY: //Only for checking that beacons have sent all information about drones
        //No se va a implementar ahora
        //Este paquete es por si se pierde algún new_drone_reply. Aunque también se podrían hacer un ACK
        break;
      case ALIVE_REPLY: //No se va a implementar por ahora
        break;
      case NEW_DRONE_REPLY: //Add drone to the corresponding beacon drone list (table)
        drone_ID = data_to_drone_ID(packet);
        beacon_element = find_beacon (packet.source, beacons_table);
        if (beacon_element != NULL) //The beacon was registered
        {
           registered = 0;
           for (i = 0; i <= beacon_element->reg_drones; i++)
           {
              if(beacon_element->drones_table[i] == drone_ID)
              {
                 registered = 1;
              }
           }

           if (registered == 0)
           {
              beacon_element->drones_table[beacon_element->reg_drones] = drone_ID;
              beacon_element->reg_drones++;
           }
        }
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

void print_beacon_table(beacons_table_t beacons_table)
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
}
