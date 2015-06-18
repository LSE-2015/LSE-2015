#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "comNodoCentral.h"
#include "serial.h"



int main (int argc, char* argv[])
{
  int i;
  int k;
  int num_ports = argc - 1;
  int serial_fd[num_ports];
  beacons_table_t beacons_table;   
  beacons_table = create_beacons_table();
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

  //-- Execute the main loop  
   while(1)
   {
      for(i = 0; i <= 1000; i++)
      {
         for(k = 0; k < num_ports; k++ )
         {
            parse_received(serial_fd[k],&beacons_table);
         }
      }
      print_beacon_table(beacons_table);
   }


  for(i = 0; i < num_ports; i++ )
  {
     //-- Close the serial port
     serial_close(serial_fd[i]);
  }
   return 0;
}
