#ifndef PLAYER_H
#define PLAYER_H
#define LIMIT_X 14
#define LIMIT_Y 12
#include "lpc17xx.h"

enum direction{
	NORD= 3,
	EST=	2,
	SUD=	1,
	OVEST=0
};

enum joystick{
	J_NORD=0xF,
	J_EST= 0x17,
	J_OVEST= 0x1B,
	J_SUD=0x1D,
	J_SEL=0x1E
};

enum LEDS{
	RUN_LED=1UL<<5
};

typedef struct Robot robot;
void           init_robot(void);
void 					 check_obstacles(struct Robot* r);
void 					 setDirection(struct Robot* robot, unsigned char direction);
void 					 setPosition(struct Robot* robot, unsigned char x, unsigned char y);
void 					 setStatus(struct Robot* robot);
unsigned char  getX(struct Robot* robot);
unsigned char  getY(struct Robot* robot);
unsigned char  getDirection(struct Robot* robot);
unsigned char  getStatus(struct Robot* robot);
int					   advance(struct Robot* robot);

void DrawR2D2(unsigned char X,unsigned char Y, unsigned char MOVE);
void DrawObstacle(unsigned char X, unsigned char Y,uint16_t color);
void clear_obstacles(void);
void clear_all(void);
void DrawPoint(uint16_t X, uint16_t Y,uint16_t color);
uint16_t getR2D2value(unsigned char x, unsigned char y);
#endif
