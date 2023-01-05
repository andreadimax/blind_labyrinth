/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_timer.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    timer.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "timer.h"
#include "../Player/Player.h"
#include "../GLCD/GLCD.h"
#include "../TouchPanel/TouchPanel.h"
#include "../RIT/RIT.h"
#include "../joystick/joystick.h"

extern robot r;

/******************************************************************************
** Function name:		Timer0_IRQHandler
**
** Descriptions:		Timer/Counter 0 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

static unsigned char gameStarted = 0;
extern unsigned char winFlag;


void TIMER0_IRQHandler (void)
{	
	getDisplayPoint(&display, Read_Ads7846(), &matrix ) ;
	#ifdef SIMULATOR
	if(display.x <= 240 && display.x > 0 && display.y < 0x013D){
	#elif
	if(display.x <= 240 && display.x > 0 && display.y != 0){
	#endif
		if(!gameStarted){
			if(display.y >= 60 && display.y <= 242 && display.x >=15 && display.x <= 225){
				gameStarted = 1;
				GUI_Text(65,80, (uint8_t *) "              ", White, White);
				GUI_Text(65,95, (uint8_t *) "              ", White, White);
				
				//init_timer(1,0x1312D0);									/* RIT Initialization 50 msec       	*/
				//enable_timer(1);													/* RIT enabled												*/
				init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
				enable_RIT();													/* RIT enabled												*/
				joystick_init();											/* Joystick Initialization            */
				init_robot();
				//LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
				//reset_RIT();
				
			}
		}
		else{
			//Restart
			if(display.y >= 250 && display.y <= 300 && display.x >=15 && display.x <= 105){
				disable_RIT();
				GUI_Text(65,35, (uint8_t *) " Restarting...", Red, Blue);
				if(winFlag){
					DrawPoint(130,140,White);
					DrawPoint(120,130,White);
					DrawPoint(140,130,White);
					DrawPoint(120,150,White);
					DrawPoint(140,150,White);
					DrawPoint(110,120,White);
					DrawPoint(130,120,White);
					DrawPoint(160,120,White);
					DrawPoint(160,140,White);
					DrawPoint(150,170,White);
					DrawPoint(100,140,White);
					DrawPoint(100,160,White);
					DrawPoint(80,140,White);
					DrawPoint(100,110,White);
					DrawPoint(80,100,White);
					DrawPoint(170,100,White);
					DrawPoint(170,150,White);
					DrawPoint(150,170,White);
					
					GUI_Text(110,210, (uint8_t *) "    ", White, White);
				}
					
				GUI_Text(65,35, (uint8_t *) "              ", Blue, Blue);
				GUI_Text(65,80, (uint8_t *) "Touch to start", Blue2, White);
				GUI_Text(65,95, (uint8_t *) "  a new game  ", Blue2, White);
				gameStarted = 0;
				winFlag=0;
			}
			//Clear
			if(!winFlag){
				if(display.y >= 250 && display.y <= 300 && display.x >=135 && display.x <= 225){
					disable_RIT();
					GUI_Text(65,35, (uint8_t *) " Clearing...  ", Red, Blue);
					clear_obstacles();
					GUI_Text(65,35, (uint8_t *) "              ", Blue, Blue);
					enable_RIT();
				}
			}
				
		}
	}
  LPC_TIM0->IR = 1;			/* clear interrupt flag */
  return;
}


/******************************************************************************
** Function name:		Timer1_IRQHandler
**
** Descriptions:		Timer/Counter 1 interrupt handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void TIMER1_IRQHandler (void)
{
  LPC_TIM1->IR = 1;			/* clear interrupt flag */
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
