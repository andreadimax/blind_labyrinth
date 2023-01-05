/*----------------------------------------------------------------------------
 * Name:    sample.c
 * Purpose: to control led through debounced buttons and Joystick
 *        	- key1 switches on the led at the left of the current led on, 
 *					- it implements a circular led effect,
 * 					- joystick select function returns to initial configuration (led11 on) .
 * Note(s): this version supports the LANDTIGER Emulator
 * Author: 	Paolo BERNARDI - PoliTO - last modified 15/12/2020
 *----------------------------------------------------------------------------
 *
 * This software is supplied "AS IS" without warranties of any kind.
 *
 * Copyright (c) 2017 Politecnico di Torino. All rights reserved.
 *----------------------------------------------------------------------------*/
                  
#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "button_EXINT/button.h"
#include "timer/timer.h"
#include "RIT/RIT.h"
#include "GLCD/GLCD.h" 
#include "TouchPanel/TouchPanel.h"
#include "joystick/joystick.h"
#include "Player/Player.h"

extern robot r;

#ifdef SIMULATOR
extern uint8_t ScaleFlag; // <- ScaleFlag needs to visible in order for the emulator to find the symbol (can be placed also inside system_LPC17xx.h but since it is RO, it needs more work)
#endif
/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  
	SystemInit();  												/* System Initialization (i.e., PLL)  */ 
	joystick_init();											/* Joystick Initialization            */
//	init_RIT(0x004C4B40);									/* RIT Initialization 50 msec       	*/
//	enable_RIT();													/* RIT enabled												*/
	init_timer(0,0x1B3F72);
	
	LCD_Initialization();  
	TP_Init();
	TouchPanel_Calibrate();	
	LCD_Clear(Blue);
	GUI_Text(55,20, (uint8_t *) "Blind labirinth", Red, Blue);
	//Restart
	LCD_DrawBox(15,250,105,250,15,300,105,300,White);
	GUI_Text(33,270, (uint8_t *) "Restart", Blue,White);
	//Clear
	LCD_DrawBox(135,250,225,250,135,300,225,300,Yellow);
	GUI_Text(160,270, (uint8_t *) "Clear", Black, Yellow);
	//Labirinth
	LCD_DrawBox(15,60,225,60,15,242,225,242,White);
	
//	for(i=0;i<14;i++){
//		DrawObstacle(i,0);
//	}
//	for(i=0;i<14;i++){
//		DrawObstacle(0,i);
//	}
	//init_robot();
	//DrawObstacle(8,7, Red);
	GUI_Text(65,80, (uint8_t *) "Touch to start", Blue2, White);
	GUI_Text(65,95, (uint8_t *) "  a new game  ", Blue2, White);
	init_timer(0, 0x4E2); 						    /* 500us * 25MHz = 1.25*10^3 = 0x4E2 */
	enable_timer(0);	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
		
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }

}
