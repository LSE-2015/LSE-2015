
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <wiringPi.h>
#include <stdlib.h>
#include <sys/time.h>

#define	baliza1_pin 0
#define	baliza2_pin	15
#define	insert_coin_pin 7



static int baliza1 = 0 ;
static int baliza2 = 0 ;
static int insert_coin = 0;


// funcion que atiende a la interrupcion en el gpio BUTTON_PIN
void baliza1_interrupt (void)
{
  baliza1=1;

}

void baliza2_interrupt (void)
{
  baliza2=1;
}



void insert_coin_interrupt (void)
{
  insert_coin=1;
}



	enum estados {
  idle,  beacon1,  beacon2	};
	


int main(int argc, char **argv)
{




	int delay1,delay2;
	enum estados estado = idle;	
    struct timeval start,stop1,stop2;
  		

// funciones de inicializacion
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
  
   
		 if (wiringPiISR (insert_coin_pin, INT_EDGE_RISING, &insert_coin_interrupt) < 0)
  {
    fprintf (stderr, "Unable to setup ISR: %s\n", strerror (errno)) ;
    return 1 ;
  }
  
  
    
  
	
 	 printf("Bienvenido\n");


	while(1){
		
                
              
		
		switch (estado) {
		
			case idle:
				
				if (insert_coin==1){
					insert_coin=0;	
					baliza1=0;
					baliza2=0;	
 				printf("Comienze el juego\n ");
				estado=beacon1;
				gettimeofday(&start,NULL);
					}	
				break;
		
	
		
			case beacon1:

		
			
				if (baliza1==1){
					insert_coin=0;	
					baliza1=0;
					baliza2=0;	
				gettimeofday(&stop1,NULL);
				delay1=(int)((stop1.tv_sec-start.tv_sec)*1000000ULL+(stop1.tv_usec-start.tv_usec));
				printf("Hemos recibido la baliza 1 en tiempo:(%d)  milisegundos\n",delay1/1000);
				estado=beacon2;
					}				
				break;
				
			case beacon2:
			
			
				
				if (baliza2==1){
					insert_coin=0;	
					baliza1=0;
					baliza2=0;	
						gettimeofday(&stop2,NULL);
					delay2=(int)((stop2.tv_sec-start.tv_sec)*1000000ULL+(stop2.tv_usec-start.tv_usec));
					printf("Juego terminado en tiempo:(%d) milisegundos \n",delay2/1000);
					estado=idle;
					}	
				break;
				
				
				
			default:
			break;
		
			}	
		}

	return 0;
}

