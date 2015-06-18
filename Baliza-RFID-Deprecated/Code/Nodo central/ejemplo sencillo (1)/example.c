#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "serial.h"
#include "comNodoCentral.h"


//------------------
//-- CONSTANTS
//------------------

//--TIMEOUT in micro-sec 
#define TIMEOUT 1

//--------------------
//-- Global variables
//--------------------
//-- Serial port descriptor
int serial_fd;


/**************************************************************************/
/* Main loop. When the user press a key, it is sent to the serial port.   */
/* When a character is received from the serial port, it is printed on    */
/* the screen                                                             */
/**************************************************************************/
void term(void)
{
  uint8_t length = 4 + 2*6 + 4; //Se va a enviar HOLA
  char rec[length];
  int n;
  packet_t test_packet;
  test_packet.packet_id = PCK_HEADER;
  test_packet.destination = 0x1023;
  test_packet.source = 0x3211;
  test_packet.number = 0x0001;
  test_packet.length = 0x0004;
  test_packet.packet_type = 0x0012;
  test_packet.reserved = 0x0000;
  drone_ID_to_data(0x01FF,&test_packet);
  printf("%d\r\n",data_to_drone_ID(test_packet));

  send_packet(serial_fd,  test_packet);
  //This code shows that a no read character, remains in the buffer and will be processesed later
  n=serial_read(serial_fd,rec,length,TIMEOUT);
  int i;
  for (i = 0; i < n; i++)  
  {
      printf ("|%02x|", rec[i]);
        
  }
  printf ("\n\r");


  for (i = 0; i < n; i++)  
  {
      printf ("|%c|", rec[i]);
        
  }
  printf ("\n\r");
  fflush(stdout);

  sleep(1);

  n=serial_read(serial_fd,rec,length,TIMEOUT);
    
  for (i = 0; i < n; i++)  
  {
      printf ("|%02x|", rec[i]);
        
  }
  printf ("\n\r");


  for (i = 0; i < n; i++)  
  {
      printf ("|%c|", rec[i]);
        
  }
  printf ("\n\r");
  fflush(stdout);
}

/**********************/
/*  MAIN PROGRAM      */
/**********************/
int main (int argc, char* argv[])
{
  
  //-- Check if the serial device name is given
  if (argc<2) {
    printf ("No serial device name is given\n");
    exit(0);
  }

  //-- Open the serial port
  serial_fd=serial_open(argv[1],B9600);
  
  //-- Error checking
  if (serial_fd==-1) {
    printf ("Error opening the serial device: %s\n",argv[1]);
    perror("OPEN");
    exit(0);
  }
 
  //-- Execute the main loop
  term();
  
  //-- Close the serial port
  serial_close(serial_fd);

  return 0;
}
