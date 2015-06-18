#ifndef _COMNODOCENTRAL_H_
#define _COMNODOCENTRAL_H_

#include <stdint.h>
#include "serial.h"

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
#define DEFAULT_ADDRESS  0x0000

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
typedef uint16_t drone_ID_t;

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
void send_packet(int serial_fd, packet_t packet);
void parse_received(void);
void process_packet(packet_t packet, beacons_table_t *beacons_table);

drone_ID_t data_to_drone_ID(packet_t packet);
void drone_ID_to_data(drone_ID_t drone_ID, packet_t *packet);
void print_beacon_table(beacons_table_t beacons_table);
beacon_element_t* find_beacon (beacon_ID_t beacon_ID,beacons_table_t *beacons_table);

#endif
