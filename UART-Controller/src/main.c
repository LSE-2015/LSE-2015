/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stddef.h>
#include <math.h>
#include "STM32vldiscovery.h"

 /* BUTTON GPIOs define ------------------------------------------------------*/
#define BUTTON_LEFT         GPIO_Pin_0
#define BUTTON_RIGHT        GPIO_Pin_1
#define BUTTON_UP             GPIO_Pin_2
#define BUTTON_DOWN         GPIO_Pin_3

#define BUTTON_SCREEN_PORT     GPIOA
#define BUTTON_DRONE_PORT      GPIOC
#define BUTTONPORTCLK   RCC_APB2Periph_GPIOA

 /* Private macro ------------------------------------------------------------*/
 /* Private variables --------------------------------------------------------*/
 TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
 TIM_OCInitTypeDef  TIM_OCInitStructure;

 /* Private function prototypes ----------------------------------------------*/
 void RCC_Configuration(void);
 void GPIO_Configuration(void);
 void EnableTimerInterrupt(void);
 void USART1_Init(void);

 /* Private functions --------------------------------------------------------*/

 int main(void)
 {

   /* System Clocks Configuration */
   RCC_Configuration();

   /* GPIO Configuration */
   GPIO_Configuration();
   USART1_Init();

   //TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
   TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;

   TIM_ARRPreloadConfig(TIM3, ENABLE);

   /* TIM3 enable counter */
   TIM_Cmd(TIM3, ENABLE);
   TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);

   EnableTimerInterrupt();

   while(1) {}
 }

 /**
   * @brief  Configures the different system clocks.
   * @param  None
   * @retval None
   */
 void RCC_Configuration(void)
 {
   /* TIM3 clock enable */
   RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

   /* GPIOA and GPIOB clock enable */
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB |
                          RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
 }

 void GPIO_Configuration(void)
 {
   GPIO_InitTypeDef GPIO_InitStructure;

   /* GPIOA Configuration: TIM3 Channel 1, 2, 3 and 4 */
   /* as alternate function push-pull */
   GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 | GPIO_Pin_7;
   GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;

   GPIO_Init(GPIOA, &GPIO_InitStructure);

   //using same structure as with TIMER3, we will initialize button pin

   //select pin to initialize button left
   GPIO_InitStructure.GPIO_Pin = BUTTON_LEFT;
   //select input pull-up
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
   GPIO_Init(BUTTON_SCREEN_PORT, &GPIO_InitStructure);

   //select pin to initialize button right
   GPIO_InitStructure.GPIO_Pin = BUTTON_RIGHT;
   //select input pull-up
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
   GPIO_Init(BUTTON_SCREEN_PORT, &GPIO_InitStructure);

   //select pin to initialize button up
   GPIO_InitStructure.GPIO_Pin = BUTTON_UP;
   //select input pull-up
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
   GPIO_Init(BUTTON_SCREEN_PORT, &GPIO_InitStructure);

   //select pin to initialize button down
   GPIO_InitStructure.GPIO_Pin = BUTTON_DOWN;
   //select input pull-up
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
   GPIO_Init(BUTTON_SCREEN_PORT, &GPIO_InitStructure);

   //select pin to initialize button left
   GPIO_InitStructure.GPIO_Pin = BUTTON_LEFT;
   //select input pull-up
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
   GPIO_Init(BUTTON_DRONE_PORT, &GPIO_InitStructure);

   //select pin to initialize button left
   GPIO_InitStructure.GPIO_Pin = BUTTON_RIGHT;
   //select input pull-up
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
   GPIO_Init(BUTTON_DRONE_PORT, &GPIO_InitStructure);

   //select pin to initialize button left
   GPIO_InitStructure.GPIO_Pin = BUTTON_UP;
   //select input pull-up
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
   GPIO_Init(BUTTON_DRONE_PORT, &GPIO_InitStructure);

   //select pin to initialize button left
   GPIO_InitStructure.GPIO_Pin = BUTTON_DOWN;
   //select input pull-up
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
   GPIO_Init(BUTTON_DRONE_PORT, &GPIO_InitStructure);
 }

 /*****************************************************
 * Initialize USART1: enable interrupt on reception
 * of a character
 *****************************************************/
void USART1_Init(void)
{
    /* USART configuration structure for USART1 */
    USART_InitTypeDef usart1_init_struct;
    /* Bit configuration structure for GPIOA PIN9 and PIN10 */
    GPIO_InitTypeDef gpioa_init_struct;

    /* Enalbe clock for USART1, AFIO and GPIOA */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO |
                           RCC_APB2Periph_GPIOA, ENABLE);

    /* GPIOA PIN9 alternative function Tx */
    gpioa_init_struct.GPIO_Pin   = GPIO_Pin_9;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &gpioa_init_struct);
    /* GPIOA PIN9 alternative function Rx */
    gpioa_init_struct.GPIO_Pin   = GPIO_Pin_10;
    gpioa_init_struct.GPIO_Speed = GPIO_Speed_50MHz;
    gpioa_init_struct.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &gpioa_init_struct);

    /* Enable USART1 */
    USART_Cmd(USART1, ENABLE);
    /* Baud rate 9600, 8-bit data, One stop bit
     * No parity, Do both Rx and Tx, No HW flow control
     */
    usart1_init_struct.USART_BaudRate   = 9600;
    usart1_init_struct.USART_WordLength = USART_WordLength_8b;
    usart1_init_struct.USART_StopBits   = USART_StopBits_1;
    usart1_init_struct.USART_Parity     = USART_Parity_No ;
    usart1_init_struct.USART_Mode       = USART_Mode_Rx | USART_Mode_Tx;
    usart1_init_struct.USART_HardwareFlowControl =
                                                 USART_HardwareFlowControl_None;
    /* Configure USART1 */
    USART_Init(USART1, &usart1_init_struct);
    /* Enable RXNE interrupt */
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    /* Enable USART1 global interrupt */
    NVIC_EnableIRQ(USART1_IRQn);
}


 void EnableTimerInterrupt()
  {
      NVIC_InitTypeDef nvicStructure;
      nvicStructure.NVIC_IRQChannel                   = TIM3_IRQn;
      nvicStructure.NVIC_IRQChannelPreemptionPriority = 0;
      nvicStructure.NVIC_IRQChannelSubPriority        = 1;
      nvicStructure.NVIC_IRQChannelCmd                = ENABLE;
      NVIC_Init(&nvicStructure);
  }

 // Se ejecuta cada 20 ms
 void TIM3_IRQHandler()
 {
     int command_send;
     if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET)
     {
         TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

         int left_scr,right_scr,up_scr,down_scr;
         int left_dron,right_dron,up_dron,down_dron;

         // read JOYSTICK every 20 ms (when UPDATE event of TIM3 is generated)

         left_scr   = GPIO_ReadInputDataBit(BUTTON_SCREEN_PORT, BUTTON_LEFT);
         right_scr  = GPIO_ReadInputDataBit(BUTTON_SCREEN_PORT, BUTTON_RIGHT);
         up_scr     = GPIO_ReadInputDataBit(BUTTON_SCREEN_PORT, BUTTON_UP);
         down_scr   = GPIO_ReadInputDataBit(BUTTON_SCREEN_PORT, BUTTON_DOWN);

         left_dron  = GPIO_ReadInputDataBit(BUTTON_DRONE_PORT, BUTTON_LEFT);
         right_dron = GPIO_ReadInputDataBit(BUTTON_DRONE_PORT, BUTTON_RIGHT);
         up_dron    = GPIO_ReadInputDataBit(BUTTON_DRONE_PORT, BUTTON_UP);
         down_dron  = GPIO_ReadInputDataBit(BUTTON_DRONE_PORT, BUTTON_DOWN);


         command_send = (left_dron  * pow (2,7))+
                        (right_dron * pow (2,6))+
                        (up_dron    * pow (2,5))+
                        (down_dron  * pow (2,4))+
                        (left_scr   * pow (2,3))+
                        (right_scr  * pow (2,2))+
                        (up_scr     * pow (2,1))+
                        (down_scr   * pow (2,0));

         USART_SendData(USART1, command_send);
     }
 }
