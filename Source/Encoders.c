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

#include "Encoders.h"

/*----------------------------- Module Defines ----------------------------*/
#define ALL_BITS (0xFF<<2)
#define BITS0TO7HI 0x00FF
#define BITS8TO15HI 0xFF00
/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/
//Encoder Input Capture Intializations
void InitEncoder( void );

//Encoder ISRs
/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable

static uint32_t Encoder1AEdges;
static uint32_t Encoder1IEdges;

static uint32_t Encoder2AEdges;
static uint32_t Encoder2IEdges;

static uint32_t Motor1Period;
static uint32_t Motor2Period;

void InitEncoders( void )
{
		// start by enabling the clock to the timers (Wide Timer 0, Wide Timer 1, Wide Timer 2)
	HWREG(SYSCTL_RCGCWTIMER) |= SYSCTL_RCGCWTIMER_R0;
	HWREG(SYSCTL_RCGCWTIMER) |= SYSCTL_RCGCWTIMER_R1;
//	HWREG(SYSCTL_RCGCWTIMER) |= SYSCTL_RCGCWTIMER_R2;
	
	// enable the clock to Port C
	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R2;
	// enable the clock to Port D
	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R3;
	
	// since we added this Port C clock init, we can immediately start
	// into configuring the timer, no need for further delay
	// make sure that timers (Timers A & B) is disabled before configuring
	HWREG(WTIMER0_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TAEN & ~TIMER_CTL_TBEN;
	HWREG(WTIMER1_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TAEN & ~TIMER_CTL_TBEN;
//	HWREG(WTIMER2_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TAEN & ~TIMER_CTL_TBEN;

	// set it up in 32bit wide (individual, not concatenated) mode
	// the constant name derives from the 16/32 bit timer, but this is a 32/64
	// bit timer so we are setting the 32bit mode
	HWREG(WTIMER0_BASE+TIMER_O_CFG) = TIMER_CFG_16_BIT;
	HWREG(WTIMER1_BASE+TIMER_O_CFG) = TIMER_CFG_16_BIT;
//	HWREG(WTIMER2_BASE+TIMER_O_CFG) = TIMER_CFG_16_BIT;
	
	// we want to use the full 32 bit count, so initialize the Interval Load
	// register to 0xffff.ffff (its default value :-)
	HWREG(WTIMER0_BASE+TIMER_O_TAILR) = 0xffffffff;
	HWREG(WTIMER0_BASE+TIMER_O_TBILR) = 0xffffffff;
	
	HWREG(WTIMER1_BASE+TIMER_O_TAILR) = 0xffffffff;
	HWREG(WTIMER1_BASE+TIMER_O_TBILR) = 0xffffffff;
	
//	HWREG(WTIMER2_BASE+TIMER_O_TAILR) = 0xffffffff;
//	HWREG(WTIMER2_BASE+TIMER_O_TBILR) = 0xffffffff;
	
	// set up timer A in capture mode (TAMR=3, TAAMS = 0),
	// for edge time (TACMR = 1) and up-counting (TACDIR = 1)
	HWREG(WTIMER0_BASE+TIMER_O_TAMR) =
	(HWREG(WTIMER0_BASE+TIMER_O_TAMR) & ~TIMER_TAMR_TAAMS) |
	(TIMER_TAMR_TACDIR | TIMER_TAMR_TACMR | TIMER_TAMR_TAMR_CAP);

	HWREG(WTIMER0_BASE+TIMER_O_TBMR) = 
	(HWREG(WTIMER0_BASE+TIMER_O_TBMR) & ~TIMER_TBMR_TBAMS) |
	(TIMER_TBMR_TBCDIR | TIMER_TBMR_TBCMR | TIMER_TBMR_TBMR_CAP);
	
	HWREG(WTIMER1_BASE+TIMER_O_TAMR) =
	(HWREG(WTIMER1_BASE+TIMER_O_TAMR) & ~TIMER_TAMR_TAAMS) |
	(TIMER_TAMR_TACDIR | TIMER_TAMR_TACMR | TIMER_TAMR_TAMR_CAP);

	HWREG(WTIMER1_BASE+TIMER_O_TBMR) = 
	(HWREG(WTIMER1_BASE+TIMER_O_TBMR) & ~TIMER_TBMR_TBAMS) |
	(TIMER_TBMR_TBCDIR | TIMER_TBMR_TBCMR | TIMER_TBMR_TBMR_CAP);
	
//	HWREG(WTIMER2_BASE+TIMER_O_TAMR) =
//	(HWREG(WTIMER2_BASE+TIMER_O_TAMR) & ~TIMER_TAMR_TAAMS) |
//	(TIMER_TAMR_TACDIR | TIMER_TAMR_TACMR | TIMER_TAMR_TAMR_CAP);

//	HWREG(WTIMER2_BASE+TIMER_O_TBMR) = 
//	(HWREG(WTIMER2_BASE+TIMER_O_TBMR) & ~TIMER_TBMR_TBAMS) |
//	(TIMER_TBMR_TBCDIR | TIMER_TBMR_TBCMR | TIMER_TBMR_TBMR_CAP);
	
	// To set the event to rising edge, we need to modify the T_EVENT bits
	// in GPTMCTL. Rising edge = 00, so we clear the T_EVENT bits
	HWREG(WTIMER0_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TAEVENT_M & ~TIMER_CTL_TBEVENT_M;
	HWREG(WTIMER1_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TAEVENT_M & ~TIMER_CTL_TBEVENT_M;
	//HWREG(WTIMER2_BASE+TIMER_O_CTL) &= ~TIMER_CTL_TAEVENT_M & ~TIMER_CTL_TBEVENT_M;

	// Now Set up the port to do the capture (clock was enabled earlier)
	// start by setting the alternate function for Port C bit 4 (WT0CCP0)
	// and Port C bit 5 (WT0CCP0)
	HWREG(GPIO_PORTC_BASE+GPIO_O_AFSEL) |= (BIT4HI|BIT5HI|BIT6HI|BIT7HI);
	HWREG(GPIO_PORTD_BASE+GPIO_O_AFSEL) = 0;
	
	// Then, map bit 4's alternate function to WT0CCP0
	// 7 is the mux value to select WT0CCP0, 16 to shift it over to the
	// right nibble for bit 4 (4 bits/nibble * 4 bits)
	HWREG(GPIO_PORTC_BASE+GPIO_O_PCTL) = (HWREG(GPIO_PORTC_BASE+GPIO_O_PCTL) & 0x0000ffff) + (7<<16) + (7<<20) + (7<<24) + (7<<28);
	HWREG(GPIO_PORTD_BASE+GPIO_O_PCTL) = (HWREG(GPIO_PORTD_BASE+GPIO_O_PCTL) | 0xffffffff); // Added to try and fix the PWM0 output on PD0

	// Enable pins on Port C for digital I/O
	HWREG(GPIO_PORTC_BASE+GPIO_O_DEN) |= (BIT4HI | BIT5HI | BIT6HI | BIT7HI);
	HWREG(GPIO_PORTD_BASE+GPIO_O_DEN) |= (BIT0HI|BIT1HI);
	
	// make pin 4 on Port C into an input
	HWREG(GPIO_PORTC_BASE+GPIO_O_DIR) &= (BIT4LO & BIT5LO & BIT6LO & BIT7LO);
	HWREG(GPIO_PORTD_BASE+GPIO_O_DIR) &= (BIT0LO & BIT1LO);
	
	//set priority to be lower than 0
	HWREG(NVIC_PRI23) = (1 << 20) & (1 << 28);//BIT5HI
	
	// back to the timer to enable a local capture interrupt
	HWREG(WTIMER0_BASE+TIMER_O_IMR) |= TIMER_IMR_CAEIM | TIMER_IMR_CBEIM;//***<don't know if this is necessary
	HWREG(WTIMER1_BASE+TIMER_O_IMR) |= TIMER_IMR_CAEIM | TIMER_IMR_CBEIM;
	//HWREG(WTIMER2_BASE+TIMER_O_IMR) |= TIMER_IMR_CAEIM | TIMER_IMR_CBEIM;
	
	// enable the Timer A in Wide Timer 0 interrupt in the NVIC
	// it is interrupt number 94 so appears in EN2 at bit 30
	// Timer B is interrupt number 95-> bit 31
	HWREG(NVIC_EN2) |= (BIT30HI | BIT31HI);
	HWREG(NVIC_EN3) |= (BIT0HI|BIT1HI);
	//HWREG(NVIC_EN3) |= (BIT0HI|BIT1HI|BIT2HI|BIT3HI);
	
	// make sure interrupts are enabled globally
	__enable_irq();
	// now kick the timers off by enabling them and enabling the timer to
	// stall while stopped by the debugger
	HWREG(WTIMER0_BASE+TIMER_O_CTL) |= (TIMER_CTL_TAEN | TIMER_CTL_TASTALL| TIMER_CTL_TBEN | TIMER_CTL_TBSTALL);
	HWREG(WTIMER1_BASE+TIMER_O_CTL) |= (TIMER_CTL_TAEN | TIMER_CTL_TASTALL| TIMER_CTL_TBEN | TIMER_CTL_TBSTALL);
	//HWREG(WTIMER2_BASE+TIMER_O_CTL) |= (TIMER_CTL_TAEN | TIMER_CTL_TASTALL| TIMER_CTL_TBEN | TIMER_CTL_TBSTALL);
	
}

/****************************************************************************
 Function
    Encoder1A_ISR

 Parameters
    void

 Returns
    nothing

 Description
    Interrupt response routine for the Encoder 1 channel A. Posts a new encoder edge
    event to the Encoder service and records the encoder period.
	
 Notes
    Uses Wide Timer 0, Timer A
 Author
   Ernesto Ramirez
****************************************************************************/
void Encoder1A_ISR( void )
{
	uint32_t ThisTime;
	static uint32_t LastTime;
  // start by clearing the source of the interrupt, the input capture event
	HWREG(WTIMER0_BASE+TIMER_O_ICR) = TIMER_ICR_CAECINT;
	
	//Capture current time
	ThisTime = HWREG(WTIMER0_BASE+TIMER_O_TAR);
	
	//Determine Period and store in Period history array
	Motor1Period =  ThisTime - LastTime;
	
	//Increment EncoderEdges variable
	if((HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + (ALL_BITS)))& BIT0HI) == 0)
	{
		Encoder1AEdges++;
	}
	else{
		Encoder1AEdges--;
	}
	
	
	
	//Update LastTime
	LastTime = ThisTime;
}

/****************************************************************************
 Function
    Encoder1B_ISR

 Parameters
    void

 Returns
    nothing

 Description
    Interrupt response routine for the Encoder 1 channel B. Posts a new encoder edge
    event to the Encoder service and records the encoder period.
	
 Notes
    Uses Wide Timer 0, Timer B
 Author
   Ernesto Ramirez
****************************************************************************/
void Encoder1I_ISR( void )
{
	uint32_t ThisTime;
	static uint32_t LastTime;
  // start by clearing the source of the interrupt, the input capture event
	HWREG(WTIMER0_BASE+TIMER_O_ICR) = TIMER_ICR_CBECINT;
	
	//Capture current time
	ThisTime = HWREG(WTIMER0_BASE+TIMER_O_TBR);
	
	//Determine Period and store in Period history array
	//Motor1Period =  ThisTime - LastTime;
	
	//Increment EncoderEdges variable
	Encoder1IEdges++; //Still need to figure out the direction
	
	
	//Update LastTime
	LastTime = ThisTime;
}

/****************************************************************************
 Function
    Encoder1I_ISR

 Parameters
    void

 Returns
    nothing

 Description
    Interrupt response routine for the Encoder 1 channel I. Records the encoder period.
	
 Notes
    Uses Wide Timer 1, Timer A
 Author
   Ernesto Ramirez
****************************************************************************/
void Encoder2A_ISR( void )
{
	uint32_t ThisTime;
	static uint32_t LastTime;
  // start by clearing the source of the interrupt, the input capture event
	HWREG(WTIMER1_BASE+TIMER_O_ICR) = TIMER_ICR_CAECINT;
	
	//Capture current time
	ThisTime = HWREG(WTIMER1_BASE+TIMER_O_TAR);
	
	//Determine Period and store in Period history array
	Motor2Period =  ThisTime - LastTime;
	
	//Increment EncoderEdges variable
	if((HWREG(GPIO_PORTD_BASE+(GPIO_O_DATA + (ALL_BITS)))& BIT1HI) == 0)
	{
		Encoder2AEdges++;
	}
	else{
		Encoder2AEdges--; 
	}
	
	
	//Update LastTime
	LastTime = ThisTime;
}

/****************************************************************************
 Function
    Encoder2I_ISR

 Parameters
    void

 Returns
    nothing

 Description
    Interrupt response routine for the Encoder 2 channel A. Records the encoder period.
	
 Notes
    Uses Wide Timer 1, Timer B
 Author
   Ernesto Ramirez
****************************************************************************/
void Encoder2I_ISR( void )
{
	uint32_t ThisTime;
	static uint32_t LastTime;
  // start by clearing the source of the interrupt, the input capture event
	HWREG(WTIMER1_BASE+TIMER_O_ICR) = TIMER_ICR_CBECINT;
	
	//Capture current time
	ThisTime = HWREG(WTIMER1_BASE+TIMER_O_TBR);
	
	
	//Increment EncoderEdges variable
	Encoder2IEdges++; //Still need to figure out the direction
	
	
	//Update LastTime
	LastTime = ThisTime;
}

uint32_t GetMotor1EdgeCount(void)
{
	return Encoder1AEdges;
}

uint32_t GetMotor2EdgeCount(void)
{
	return Encoder2AEdges;
}
