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
#define FEEDBACK_SAMPLE_RATE 10
#define TICKS_PER_MS 40000
#define PE0 0
#define PE1 1


/*---------------------------- Module Functions ---------------------------*/
static void InitCANPeriodicInt(void);
/*---------------------------- Module Variables ---------------------------*/
static uint32_t Analog_Array[AD_HOW_MANY];
static uint32_t LoadCell1; 
static uint32_t LoadCell2;
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
	InitCANPeriodicInt();
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

static void InitCANPeriodicInt( void )
{
  volatile uint32_t Dummy; // use volatile to avoid over-optimization
  // start by enabling the clock to the timer (Wide Timer 2)
  HWREG(SYSCTL_RCGCWTIMER) |= SYSCTL_RCGCWTIMER_R2; // kill a few cycles to let the clock get going
  Dummy = HWREG(SYSCTL_RCGCGPIO);
  // make sure that timer (Timer A) is disabled before configuring
  HWREG(WTIMER2_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TAEN;
  // set it up in 32bit wide (individual, not concatenated) mode
  HWREG(WTIMER2_BASE+TIMER_O_CFG) = TIMER_CFG_16_BIT;
  // set up timer B in periodic mode so that it repeats the time-outs
  HWREG(WTIMER2_BASE+TIMER_O_TAMR) =
  (HWREG(WTIMER2_BASE+TIMER_O_TAMR)& ~TIMER_TAMR_TAMR_M)|
  TIMER_TAMR_TAMR_PERIOD;
  // set timeout to 2mS
  HWREG(WTIMER2_BASE+TIMER_O_TAILR) = TICKS_PER_MS * FEEDBACK_SAMPLE_RATE;
  //set priority to be lower than 0
  // enable a local timeout interrupt
  HWREG(WTIMER2_BASE+TIMER_O_IMR) |= TIMER_IMR_TATOIM;
  // enable the Timer A in Wide Timer 2 interrupt in the NVIC
  // it is interrupt number 98 so appears in EN3 at bit 2
  HWREG(NVIC_EN3) = BIT2HI;
  // make sure interrupts are enabled globally
  __enable_irq();
  // now kick the timer off by enabling it and enabling the timer to
  // stall while stopped by the debugger
  HWREG(WTIMER2_BASE+TIMER_O_CTL) |= (TIMER_CTL_TAEN |
  TIMER_CTL_TASTALL);
}

void CAN_SENDING_ISR( void ){
  // start by clearing the source of the interrupt
	//printf("Sent a message \n\r");
	static uint16_t time = 0;
  HWREG(WTIMER2_BASE+TIMER_O_ICR) = TIMER_ICR_TATOCINT;
	ADC_MultiRead(Analog_Array);
  LoadCell1 = Analog_Array[PE0];
	//printf("Analog Value = %d\n\r",LoadCell1);
  LoadCell2 = Analog_Array[PE1];
	CAN_SEND(1,time,GetMotor1EdgeCount(),LoadCell1,0);
	time++;
}
