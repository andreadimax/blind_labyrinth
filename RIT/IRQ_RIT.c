/*********************************************************************************************************
**--------------File Info---------------------------------------------------------------------------------
** File name:           IRQ_RIT.c
** Last modified Date:  2014-09-25
** Last Version:        V1.00
** Descriptions:        functions to manage T0 and T1 interrupts
** Correlated files:    RIT.h
**--------------------------------------------------------------------------------------------------------
*********************************************************************************************************/
#include "lpc17xx.h"
#include "RIT.h"
#include "../led/led.h"
#include "../Player/Player.h"
#include "../timer/timer.h"
#include "../GLCD/GLCD.h"

/******************************************************************************
** Function name:		RIT_IRQHandler
**
** Descriptions:		REPETITIVE INTERRUPT TIMER handler
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/

extern robot r;
extern unsigned char winFlag;
static int select=0;
static unsigned char J_SELECTED = 1;
static unsigned char f_count = 0;

void fireworks(void){
	switch(f_count){
		case 1:
			DrawPoint(130,140,Red);
			GUI_Text(110,210, (uint8_t *) "WIN!", Blue2, White);
			break;
		case 2:
			DrawPoint(130,140,White);
		
			DrawPoint(120,130,Red);
			DrawPoint(140,130,Red);
			DrawPoint(120,150,Red);
			DrawPoint(140,150,Red);
			GUI_Text(110,210, (uint8_t *) "WIN!", Yellow, White);
			break;
		case 3:
			DrawPoint(120,130,White);
			DrawPoint(140,130,White);
			DrawPoint(120,150,White);
			DrawPoint(140,150,White);
		
			DrawPoint(110,120,Red);
			DrawPoint(130,120,Red);
			DrawPoint(160,120,Red);
			DrawPoint(160,140,Red);
			DrawPoint(150,170,Red);
			DrawPoint(100,140,Red);
			DrawPoint(100,160,Red);
			GUI_Text(110,210, (uint8_t *) "WIN!", Red, White);
			break;
		case 4:
			DrawPoint(110,120,White);
			DrawPoint(130,120,White);
			DrawPoint(160,120,White);
			DrawPoint(160,140,White);
			DrawPoint(150,170,White);
			DrawPoint(100,140,White);
			DrawPoint(100,160,White);
		
		
			
			DrawPoint(80,140,Red);
			DrawPoint(100,110,Red);
			DrawPoint(80,100,Red);
			DrawPoint(170,100,Red);
			DrawPoint(170,150,Red);
			DrawPoint(150,170,Red);
			GUI_Text(110,210, (uint8_t *) "WIN!", Green, White);
			break;
		case 5:
			DrawPoint(80,140,White);
			DrawPoint(100,110,White);
			DrawPoint(80,100,White);
			DrawPoint(170,100,White);
			DrawPoint(170,150,White);
			DrawPoint(150,170,White);
			GUI_Text(110,210, (uint8_t *) "WIN!", Magenta, White);
			f_count = 0;
			break;
	}
	f_count++;
}
void RIT_IRQHandler (void)
{
	static unsigned char joystick_pins;
	
	if(winFlag){
		if(winFlag == 1){
			clear_obstacles();
			DrawObstacle(getX(&r),getY(&r),White);
			GUI_Text(110,210, (uint8_t *) "WIN!", Blue2, White);
			winFlag++;
		}
		fireworks();
		LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */	
		return;
	}
	
	joystick_pins = (LPC_GPIO1->FIOPIN & (0x1F<<25)) >> 25;
	
	if(joystick_pins != 0x1F){	
			if(joystick_pins == J_SEL){
				if(J_SELECTED){
					setStatus(&r);													//Select on joystick was pressed
					DrawR2D2(getX(&r), getY(&r), getStatus(&r));
					J_SELECTED--;
				}
			}
			else{
				if(J_SELECTED == 0) J_SELECTED = 1;
				select++;																//Direction changed
				switch(select){
					case 17:
						select=0;
						switch(joystick_pins){
							case J_NORD:
								setDirection(&r,NORD);
								check_obstacles(&r);
								if(getStatus(&r)){						  //Advance only if in MOVE STATUS
									if(advance(&r) != -1){				//Update figure position on LCD only if no obstacle is present
										//enable_timer(0);
										DrawObstacle(getX(&r),getY(&r)+1, 0xFFFF);
										DrawR2D2(getX(&r), getY(&r),1);
									}
								}
								break;
							case J_EST:
								setDirection(&r,EST);
								check_obstacles(&r);
								if(getStatus(&r)){						  //Advance only if in MOVE STATUS
									if(advance(&r) != -1){				//Update figure position on LCD only if no obstacle is present
										//enable_timer(0);
										DrawObstacle(getX(&r)-1,getY(&r), 0xFFFF);
										DrawR2D2(getX(&r), getY(&r),1);
									}
								}
								break;
							case J_OVEST:
								setDirection(&r,OVEST);
								check_obstacles(&r);
								if(getStatus(&r)){						  //Advance only if in MOVE STATUS
									if(advance(&r) != -1){				//Update figure position on LCD only if no obstacle is present
										//enable_timer(0);
										DrawObstacle(getX(&r)+1,getY(&r), 0xFFFF);
										DrawR2D2(getX(&r), getY(&r),1);
									}
								}
								break;
							case J_SUD:
								setDirection(&r,SUD);
								check_obstacles(&r);
								if(getStatus(&r)){						  //Advance only if in MOVE STATUS
									if(advance(&r) != -1){				//Update figure position on LCD only if no obstacle is present
										//enable_timer(0);
										DrawObstacle(getX(&r),getY(&r)-1, 0xFFFF);
										DrawR2D2(getX(&r), getY(&r),1);
									}
								}
								break;
						}
						break;
				}
			}
	}
	else{
		if(J_SELECTED == 0) J_SELECTED = 1;
		//disable_timer(0);
	}
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
