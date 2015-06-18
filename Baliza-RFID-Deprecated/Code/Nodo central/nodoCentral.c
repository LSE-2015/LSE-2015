#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "comNodoCentral.h"
#include "serial.h"

extern const uint16_t NodoCentralID;
extern const uint16_t AllNodesID;

int main (int argc, char* argv[])
{
  int i;
  int k;
  int discover_iter;
  int play_iter;
  int num_ports = argc - 1;
  int serial_fd[num_ports];
  beacons_table_t beacons_table;   
  beacons_table = create_beacons_table();
  packet_t packet_to_send;
  enum state_t {DISCOVER, PLAY} state;
  //-- Check if the serial device name is given
  if (argc<2) {
    printf ("No serial device name is given\n");
    exit(0);
  }

  for(i = 0; i < num_ports; i++ )
  {
     //-- Open the serial port
     serial_fd[i]=serial_open(argv[i+1],B9600);
  
     //-- Error checking
     if (serial_fd[i]==-1) {
       printf ("Error opening the serial device: %s\n",argv[i+1]);
       perror("OPEN");
       exit(0);
     }
   }

/* --------------------------------------------------------------------  */
  //-- Execute the main loop  
   state = DISCOVER;
   while(1)
   {
      switch(state)
      {
         case DISCOVER:
              //Build the NEW_GAME_REQUEST packet
              packet_to_send.packet_id = PCK_HEADER;
              packet_to_send.destination = AllNodesID;
              packet_to_send.source = NodoCentralID;
              packet_to_send.number = 0x001; //No necessary in this implementation
              packet_to_send.length = 0x0000;
              packet_to_send.packet_type = NEW_GAME_REQUEST;
              packet_to_send.reserved = 0x0000;
              //It's no necessary to define "data", it'll be ignored
              
              for(k = 0; k < num_ports; k++ ) //Send a packet for each port
              {
                 send_packet(serial_fd[k], packet_to_send);
              }

              /*
              It's necessary to wait for the response and parse it. 
              In each "parse" command waits for receive one character during
              TIMEOUT us (in this case 1ms). Thus, in order to wait 2 sec. aprox
              for a response, the number of iterations are 2000ms/ (1ms X num_ports)
              */
              for(i = 0; i <= (2000 / num_ports); i++) 
              {
                 for(k = 0; k < num_ports; k++ )
                 {
                    parse_received(serial_fd[k],&beacons_table);
                 }
              }


              ////SEND PACKET AGAIN
               for(k = 0; k < num_ports; k++ ) //Send a packet for each port
              {
                 send_packet(serial_fd[k], packet_to_send);
              }

              /*
              It's necessary to wait for the response and parse it. 
              In each "parse" command waits for receive one character during
              TIMEOUT us (in this case 1ms). Thus, in order to wait 2 sec. aprox
              for a response, the number of iterations are 2000ms/ (1ms X num_ports)
              */
              for(i = 0; i <= (2000 / num_ports); i++) 
              {
                 for(k = 0; k < num_ports; k++ )
                 {
                    parse_received(serial_fd[k],&beacons_table);
                 }
              }


              print_beacon_table(beacons_table);
              state = PLAY;
            break;
         case PLAY:
              /*
              It's necessary to wait for the response and parse it. The table with
              the beacons and drones is being printed every 1 sec. In the same way,
              in each "parse" command waits for receive one character during
              TIMEOUT us (in this case 1ms). Thus, in order to wait 1 sec. aprox,
              the number of iterations are 1000ms/ (1ms X num_ports)
              */
              for(i = 0; i <= (1000 / num_ports); i++) 
              {
                 for(k = 0; k < num_ports; k++ )
                 {
                    parse_received(serial_fd[k],&beacons_table);
                 }
              }
              print_beacon_table(beacons_table);
            state = PLAY;
            break;
         default:
            state = DISCOVER;
            break;
      }
   }

/* --------------------------------------------------------------------  */

  for(i = 0; i < num_ports; i++ )
  {
     //-- Close the serial port
     serial_close(serial_fd[i]);
  }
   return 0;
}


