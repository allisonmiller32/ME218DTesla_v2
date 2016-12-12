/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"

#include "driverlib/can.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"


#include "utils/uartstdio.h"

#include "BITDEFS.h"

#include "Motors.h"

#define ALL_BITS (0xFF<<2)
/*----------------------------- Module Functions ----------------------------*/
void InitMotors(void)
{
	//Enable Digial Functions for PA2,PA3,PA4,PA5
	HWREG(MOTOR1_PORT+GPIO_O_DEN) |= (MOTOR1_ENABLE_PIN | MOTOR1_DIRECTION_PIN);
	HWREG(MOTOR2_PORT+GPIO_O_DEN) |= (MOTOR1_ENABLE_PIN | MOTOR2_DIRECTION_PIN);
	
	// make pin 4 on Port C into an input
	HWREG(MOTOR1_PORT+GPIO_O_DIR) |= (MOTOR1_ENABLE_PIN | MOTOR1_DIRECTION_PIN);
	HWREG(MOTOR2_PORT+GPIO_O_DIR) |= (MOTOR1_ENABLE_PIN | MOTOR1_DIRECTION_PIN);
}

void Motor1Enable(bool value )
{
	HWREG(MOTOR1_PORT+(GPIO_O_DATA + (ALL_BITS))) |= MOTOR1_ENABLE_PIN;
	if (value == true) 
	{
		HWREG(MOTOR1_PORT+(GPIO_O_DATA + (ALL_BITS))) &= ~MOTOR1_ENABLE_PIN;
	}
}

void Motor1Direction(uint8_t direction)
{
	HWREG(MOTOR1_PORT+(GPIO_O_DATA + (ALL_BITS))) |= MOTOR1_DIRECTION_PIN;
	if (direction == MOTOR1_FORWARD) 
	{
		HWREG(MOTOR1_PORT+(GPIO_O_DATA + (ALL_BITS))) &= ~MOTOR1_DIRECTION_PIN;
	}
}
void EnableMotor2(bool value )
{
	HWREG(MOTOR2_PORT+(GPIO_O_DATA + (ALL_BITS))) |= MOTOR1_ENABLE_PIN;
	if (value == true) 
	{
		HWREG(MOTOR2_PORT+(GPIO_O_DATA + (ALL_BITS))) &= ~MOTOR1_ENABLE_PIN;
	}
}

void Motor2Direction(uint8_t direction)
{
	HWREG(MOTOR2_PORT+(GPIO_O_DATA + (ALL_BITS))) |= MOTOR1_DIRECTION_PIN;
	if (direction == MOTOR2_FORWARD) 
	{
		HWREG(MOTOR2_PORT+(GPIO_O_DATA + (ALL_BITS))) &= ~MOTOR1_DIRECTION_PIN;
	}
}
