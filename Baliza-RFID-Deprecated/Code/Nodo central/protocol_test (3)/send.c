#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "comNodoCentral.h"
#include "serial.h"

/*
Ejemplo de uso (los datos se escriben en hexadecimal):
./send (puerto USB) (direc destino,tamaño 4 car) (direc fuente,tamaño 4 car) (tipo de paquete, 0/1) (datos,tamaño 4)

./send /dev/ttyUSB0 0001 0007 1 0012

*/

uint16_t char_to_hex2 (char received);
uint16_t int_data;
int main (int argc, char* argv[])
{
  int n;
  int serial_fd;
  packet_t test_packet;
  
  //-- Check if the serial device name is given
  if (argc<6) {
    printf ("No serial device name is given or beacon ID is forgotten\n");
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


  test_packet.packet_id = PCK_HEADER;
  test_packet.destination = ((char_to_hex2(argv[2][0])*16 + char_to_hex2(argv[2][1]))*16 + char_to_hex2(argv[2][2]))*16 + char_to_hex2(argv[2][3]);
  test_packet.source = ((char_to_hex2(argv[3][0])*16 + char_to_hex2(argv[3][1]))*16 + char_to_hex2(argv[3][2]))*16 + char_to_hex2(argv[3][3]);
  test_packet.number = 0x0001;
  test_packet.length = 0x0004;
  if (argv[4][0] == '0')
  {
     //test_packet.packet_type = NEW_GAME_REPLY;
     test_packet.packet_type = NEW_GAME_REQUEST;
  }
  else
  {
    test_packet.packet_type = NEW_DRONE_REPLY;
  }
  test_packet.reserved = 0x0000;
  int_data = ((char_to_hex2(argv[5][0])*16 + char_to_hex2(argv[5][1]))*16 + char_to_hex2(argv[5][2]))*16 + char_to_hex2(argv[5][3]);
  drone_ID_to_data(int_data,&test_packet);
  send_packet(serial_fd,  test_packet);

   return 0;
}

uint16_t char_to_hex2 (char received)
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
