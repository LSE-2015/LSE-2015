#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <sys/time.h>
#include "fsm.h"

#define	baliza1_pin     0
#define	baliza2_pin	2
#define	new_game_pin    7



static int baliza1  = 0;
static int baliza2  = 0;
static int new_game = 0;

int delay1,delay2;
struct timeval start,stop1,stop2;

enum nCentral_state {
  IDLE,
  BEACON_1,
  BEACON_2,
};

// funcion que atiende a la interrupcion en el gpio BUTTON_PIN
void baliza1_interrupt (void)
{
  baliza1=1;

}

void baliza2_interrupt (void)
{
  baliza2=1;
}



void new_game_interrupt (void)
{
  new_game=1;
}

	
static int button_pressed (fsm_t* this)
{
  int ret = new_game;
  new_game = 0;
  baliza1 = 0;
  baliza2 = 0;
  return ret;
}

static int beacon_1_detected (fsm_t* this)
{
  int ret =  baliza1;
  new_game = 0;
  baliza1 = 0;
  baliza2 = 0;
  return ret;
}

static int beacon_2_detected (fsm_t* this)
{
  int ret =  baliza2;
  new_game = 0;
  baliza1 = 0;
  baliza2 = 0;
  return ret;
}

static void start_f (fsm_t* this)
{
   printf("\n\nEl juego ha comenzado. El dron puede despegar.\n");
   gettimeofday(&start,NULL);
}

static void beacon_1_f (fsm_t* this)
{
   gettimeofday(&stop1,NULL);
   delay1=(int)((stop1.tv_sec-start.tv_sec)*1000000ULL+(stop1.tv_usec-start.tv_usec));
   printf("\n     El tiempo del 1er \'checkpoint\' ha sido: %d milisegundos\n",delay1/1000);
}

static void beacon_2_f (fsm_t* this)
{
   gettimeofday(&stop2,NULL);
   delay2=(int)((stop2.tv_sec-start.tv_sec)*1000000ULL+(stop2.tv_usec-start.tv_usec));
   printf("     Juego terminado. El tiempo total ha sido: %d milisegundos \n\n",delay2/1000);
   printf("Coloque el dron en la posición de salida y pulse el botón de \'start\' para que de comienzo una nueva carrera.\n");
}

// Explicit FSM description
static fsm_trans_t states[] = {
  { IDLE,       button_pressed,       BEACON_1,     start_f     },
  { BEACON_1,   beacon_1_detected,    BEACON_2,     beacon_1_f  },
  { BEACON_2,   beacon_2_detected,    IDLE,         beacon_2_f  },
  {-1,          NULL,                 -1,           NULL        },
};

//enum estados {idle,  beacon1,  beacon2};

int main(int argc, char **argv)
{
  		

// funciones de inicialización
  if (wiringPiSetup () < 0)
  {
    fprintf (stderr, "Unable to setup wiringPi: %s\n", strerror (errno)) ;
    return 1 ;
  }



// ISR	 
  if (wiringPiISR (baliza1_pin, INT_EDGE_RISING, &baliza1_interrupt) < 0)
  {
    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
    return 1 ;
  }
  
  if (wiringPiISR (baliza2_pin, INT_EDGE_RISING, &baliza2_interrupt) < 0)
  {
    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
    return 1 ;
  }
  
   
  if (wiringPiISR (new_game_pin, INT_EDGE_RISING, &new_game_interrupt) < 0)
  {
    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
    return 1 ;
  }

  pinMode (new_game_pin, INPUT);
  pinMode (baliza1_pin, INPUT);
  pinMode (baliza2_pin, INPUT);


  
        fsm_t* nCentral_fsm = fsm_new (states);
  
        system("clear");
 	printf("¡Bienvenido a la contra-reloj de drones!\n\rPulse el botón de \'start\' para comenzar.\n");
	while(1){
            fsm_fire (nCentral_fsm);
        }

	return 0;
}

