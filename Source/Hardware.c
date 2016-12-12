/****************************************************************************
 Module
   HW.c

 Revision
   1.0.1

 Description

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/19/16 01:39 ls      added input capture interrupt
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for the framework and this service
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "ES_DeferRecall.h"

#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"	// Define PART_TM4C123GH6PM in project
#include "driverlib/gpio.h"
#include "ES_ShortTimer.h"
#include "inc/hw_nvic.h"
#include "inc/hw_timer.h"

#include "Hardware.h"
#include "Encoders.h"
#include "Motors.h"
#include "CAN.h"
#include "ADMulti.h"
#include "PWM0_Tiva.h"
#include "EnablePA25_PB23_PD7_PF0.h"

/*---------------------------- Module Defines ---------------------------*/
#define ALL_BITS (0xFF<<2)

/*---------------------------- Module Functions ---------------------------*/

/*---------------------------- Module Variables ---------------------------*/

/*------------------------------ Module Code ------------------------------*/
void InitAll(void) {
	PortFunctionInit();
	InitPorts();
	InitMotors();
	InitCAN();
	InitEncoders();
	PWM0_Init();
	PWM1_Init();
	
	ADC_MultiInit(AD_HOW_MANY);
}

void InitPorts(void) {
	// Init PortA
	HWREG(SYSCTL_RCGCGPIO)  |= SYSCTL_RCGCGPIO_R0;
	while((HWREG(SYSCTL_RCGCGPIO) & SYSCTL_RCGCGPIO_R0)  != SYSCTL_RCGCGPIO_R0);
	
	// Init PortB
	HWREG(SYSCTL_RCGCGPIO)  |= SYSCTL_RCGCGPIO_R1;
	while((HWREG(SYSCTL_RCGCGPIO) & SYSCTL_RCGCGPIO_R1)  != SYSCTL_RCGCGPIO_R1);
	
	// Init PortC
	HWREG(SYSCTL_RCGCGPIO)  |= SYSCTL_RCGCGPIO_R2;
	while((HWREG(SYSCTL_RCGCGPIO) & SYSCTL_RCGCGPIO_R2)  != SYSCTL_RCGCGPIO_R2);
	
	// Init PortD
	HWREG(SYSCTL_RCGCGPIO)  |= SYSCTL_RCGCGPIO_R3;
	while((HWREG(SYSCTL_RCGCGPIO) & SYSCTL_RCGCGPIO_R3)  != SYSCTL_RCGCGPIO_R3);
	
	// Init PortE
	HWREG(SYSCTL_RCGCGPIO)  |= SYSCTL_RCGCGPIO_R4;
	while((HWREG(SYSCTL_RCGCGPIO) & SYSCTL_RCGCGPIO_R4)  != SYSCTL_RCGCGPIO_R4);
	
	// Init PortF
	HWREG(SYSCTL_RCGCGPIO)  |= SYSCTL_RCGCGPIO_R5;
	while((HWREG(SYSCTL_RCGCGPIO) & SYSCTL_RCGCGPIO_R5)  != SYSCTL_RCGCGPIO_R5);
}
