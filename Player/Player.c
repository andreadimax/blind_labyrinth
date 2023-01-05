#include "lpc17xx.h"
#include "../timer/timer.h"
#include "Player.h"
#include "../GLCD/GLCD.h"

/*  ROBOT Direction values:
		3 : NORD;
		2 : EST;
		1 : SUD;
		0 : OVEST;
*/

/* Game map */
unsigned char map[13][15] = {{2,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
														 {0,0,1,1,1,1,0,0,0,0,0,0,0,0,0},
														 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
														 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
														 {0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
														 {1,1,1,1,1,0,0,1,0,1,0,0,0,1,0},
														 {0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
														 {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
														 {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
														 {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
														 {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
														 {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
														 {2,0,1,0,0,0,0,0,0,0,0,0,0,0,2}};

/* R2D2 */
														 
volatile uint16_t R2D2[14][14] = {{White,White,White,White,White,Black, Black, Black, Black, White, White, White,White,White},
												 {White,White,White,Black,Black,Blue,Black,Black,Blue,Black,Black,White,White,White},
												 {White,White,Black,Black,White,Blue,Black,Black,Blue,White,Black,Black,White,White},
												 {White,White,Black,White,White,White,White,White,White,White,White,Black,White,White},
												 {White,White,Black,White,Blue,Red,Blue,Blue,Blue,Blue,White,Black,White,White},
												 {White,White,Black,Grey,White,White,White,White,White,White,Grey,Black,White,White},
												 {Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black,Black},
												 {Black,White,White,White,Grey,White,White,White,White,Grey,White,White,White,Black},
												 {Black,Black,Blue,White,Grey,Grey,Grey,Grey,Grey,Grey,White,Blue,Black,Black},
												 {White,Black,Blue,White,White,White,White,White,White,White,White,Blue,Black,White},
												 {Black,Blue,Grey,White,Black,Black,Black,Black,Black,Black,White,Grey,Blue,Black},
												 {Black,Grey,Grey,Black,White,Black,White,White,Black,White,Black,Grey,Grey,Black},
												 {Black,Black,Black,Black,White,Black,Black,Black,Black,White,Black,Black,Black,Black}};


volatile uint16_t R2D2_red[14][14] = {{White,White,White,White,White,Red, Red, Red, Red, White, White, White,White,White},
												 {White,White,White,Red,Red,Blue,Red,Red,Blue,Red,Red,White,White,White},
												 {White,White,Red,Red,White,Blue,Red,Red,Blue,White,Red,Red,White,White},
												 {White,White,Red,White,White,White,White,White,White,White,White,Red,White,White},
												 {White,White,Red,White,Blue,Black,Blue,Blue,Blue,Blue,White,Red,White,White},
												 {White,White,Red,Grey,White,White,White,White,White,White,Grey,Red,White,White},
												 {Red,Red,Red,Red,Red,Red,Red,Red,Red,Red,Red,Red,Red,Red},
												 {Red,White,White,White,Grey,White,White,White,White,Grey,White,White,White,Red},
												 {Red,Red,Blue,White,Grey,Grey,Grey,Grey,Grey,Grey,White,Blue,Red,Red},
												 {White,Red,Blue,White,White,White,White,White,White,White,White,Blue,Red,White},
												 {Red,Blue,Grey,White,Red,Red,Red,Red,Red,Red,White,Grey,Blue,Red},
												 {Red,Grey,Grey,Red,White,Red,White,White,Red,White,Red,Grey,Grey,Red},
												 {Red,Red,Red,Red,White,Red,Red,Red,Red,White,Red,Red,Red,Red}};


/* Variables and flags */
volatile robot r;
volatile unsigned char startFlag = 0;				//Set by INT0 Button Handler to start the game
volatile unsigned char direction_led = 0;   //Set by function lights_on() to turn on correct led, if == 12 -> win condition
volatile unsigned char winFlag = 0;                   //Set by function advance() if next step is on exit cell		

/* ---- ROBOT OBJECT DEFINITION ---- */		
/* --------------------------------- */															 

struct Robot{
	unsigned char x;
	unsigned char y;
	unsigned char direction;
	unsigned char	MOVE;
};

unsigned char getX(struct Robot* robot){
	return robot->x;
}

unsigned char getY(struct Robot* robot){
	return robot->y;
}

unsigned char getDirection(struct Robot* robot){
	return robot->direction;
}

void setStatus(struct Robot* robot){
	robot->MOVE ^= 1;
}

unsigned char getStatus(struct Robot* robot){
	return robot->MOVE;
}

void setDirection(struct Robot* robot, unsigned char direction){
	robot->direction = direction;
	return;
}

void setPosition(struct Robot* robot, unsigned char x, unsigned char y){
	robot->x = x;
	robot->y = y;
	return;
}

void DrawPoint(uint16_t X, uint16_t Y,uint16_t color){
	LCD_SetPoint(X,Y,color);
	LCD_SetPoint(X+1,Y,color);
	LCD_SetPoint(X,Y+1,color);
	LCD_SetPoint(X+1,Y+1,color);
}

void DrawObstacle(unsigned char X, unsigned char Y,uint16_t color)
{
	uint16_t x = 15 + X*14;
	uint16_t y = 60 + Y*14;
	int mapx=0,mapy=0;
	for(;mapx<14;x++,mapx++)
	{
		for(;mapy<14;y++,mapy++)
		{
			LCD_SetPoint(x,y,color);
		}
		mapy=0;
		y=60+ Y*14;
	}
	return;
}

void DrawR2D2(unsigned char X,unsigned char Y, unsigned char MOVE)
{
	uint16_t x = 15 + X*14;
	uint16_t y = 60 + Y*14;
	int mapx=0,mapy=0;
	if(MOVE)
	{
		for(;mapx<14;x++,mapx++)
		{
			for(;mapy<14;y++,mapy++)
			{
				LCD_SetPoint(x,y,R2D2[mapy][mapx]);
			}
			mapy=0;
			y=60+ Y*14;
		}
	}
	else{
		for(;mapx<14;x++,mapx++)
		{
			for(;mapy<14;y++,mapy++)
			{
				LCD_SetPoint(x,y,R2D2_red[mapy][mapx]);
			}
			mapy=0;
			y=60+ Y*14;
		}
	}
	return;
}

uint16_t getR2D2value(unsigned char x, unsigned char y)
{
	return R2D2[y][x];
}

void clear_all(void){
	int i,j;
	
	for(i=0;i<=LIMIT_Y;i++){
		for(j=0;j<LIMIT_X;j++){
			DrawObstacle(j,i,White);
		}
	}
}

void clear_obstacles(void){
	int i,j;
	
	for(i=0;i<=LIMIT_Y;i++){
		for(j=0;j<LIMIT_X;j++){
			if(map[i][j] == 1){
				DrawObstacle(j,i,White);
			}
		}
	}
}
void init_robot(void)
{	
	// Initializing default values
	r.direction = EST;
	r.x=7;
	r.y=7;
	r.MOVE=1;
	DrawR2D2(r.x,r.y,r.MOVE);
}

/* --------------------------------- */	


/* ---- MOVEMENT AND DIRECTION LEDS MANAGEMENT ---- */
/* ------------------------------------------------ */

/* Function used to move the robot. First is calculated the new
** position seeing the actual direction. If there are no obstacles
** or the robot is not in exit, the position is updated             */

int advance(struct Robot* robot){
	unsigned char temp_X = robot->x;
	unsigned char temp_Y = robot->y;
	
	
	// Calculating temporary new position
	switch (robot->direction){
		case	NORD:
			temp_Y = robot->y - 1;
			break;
		case EST:
			temp_X = robot->x + 1;
			break;
		case SUD:
			temp_Y = robot->y + 1;
			break;
		case OVEST:
			temp_X = robot->x - 1;
			break;
		default:
			break;
	}
	
	/* Checking validity of position in map */
	
	/* Checking out of border */
	if(temp_X > LIMIT_X | temp_Y > LIMIT_Y)
	{
		return -1;
	}
	/* Checking map values */
	//Obstacle
	if(map[temp_Y][temp_X] == 1)
	{
		return -1;
	}
	//Win condition
	if(map[temp_Y][temp_X] == 2)
	{
		robot->x = temp_X;
		robot->y = temp_Y;
		winFlag = 1;
		return 1;
	}
	//Free path - DEFAULT
	robot->x = temp_X;
	robot->y = temp_Y;
	return 0;
};
	
	
/* This function is used to scan the cells in front of robot
** and set the LED blinking frequency */
void check_obstacles(struct Robot* r)
{
	int direction = getDirection(r);
	int x = getX(r);
	int y = getY(r);
	int i;
	
	switch (direction)
	{
		//NORD
		case NORD:		
			for(i=1; i<=5;i++)
			{
				if((y-i) >= 0)                   //Checking out of border
				{
					//Obstacle
					if(map[y-i][x] == 1)
					{
						DrawObstacle(x,y-i, Red);
						break;
					}
					//Exit
					else if(map[y-i][x] == 2)
					{
						break;
					}
				}
			}
			break;
		//SUD
		case SUD:
			for(i=1; i<=5;i++)
			{
				if((y+i) <= 12)                 //Checking out of border
				{
					//Obstacle
					if(map[y+i][x] == 1)
					{
						DrawObstacle(x,y+i, Red);
						break;
					}
					//Exit
					else if(map[y+i][x] == 2)
					{
						break;
					}
				}
			}
			break;
		//EST
		case EST:
			for(i=1; i<=5;i++)
			{
				if((x+i) <= 14)                 //Checking out of border
				{
					//Obstacle
					if(map[y][x+i] == 1)
					{
						DrawObstacle(x+i,y, Red);
						break;
					}
					//Exit
					else if(map[y][x+i] == 2)
					{
						break;
					}
				}
			}
			break;
		//OVEST
		case OVEST:
			for(i=1; i<=5;i++)
			{
				if((x-i) >= 0)                  //Checking out of border
				{
					//Obstacle
					if(map[y][x-i] == 1)
					{
						DrawObstacle(x-i,y, Red);
						break;
					}
					//Exit
					else if(map[y][x-i] == 2)
					{
						break;
					}
				}
			}
			break;
		}															//and previous Led need to be turned off
}
			
/* ------------------------------------------------ */
			
		
	
	
