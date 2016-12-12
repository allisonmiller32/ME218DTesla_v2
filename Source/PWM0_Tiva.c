/****************************************************************************
 Module
   PWM0_Tiva.c

 Revision
   1.0.1

 Description
   Module for initializing and using the Tiva PWM0 generator

 Notes
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for the framework and this service
*/
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_pwm.h"
#include "driverlib/sysctl.h"
#include "driverlib/pin_map.h"	// Define PART_TM4C123GH6PM in project
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "BITDEFS.H"
#include "termio.h"

#include "PWM0_Tiva.h"

 /*----------------------------- Module Defines ----------------------------*/
 #define ClockDiv 32
 #define TicksPerMS 40000
 #define PeriodInMS 5 //200 Hz
 #define PWMTicksPerMS TicksPerMS/ClockDiv
 #define BitsPerNibble 4
 #define uS_PER_SEC 1000000 //Microseconds per second

void PWM0_Init(void)
{
	volatile uint32_t Dummy; // use volatile to avoid over-optimization
	// start by enabling the clock to the PWM Module (PWM0)
	HWREG(SYSCTL_RCGCPWM) |= SYSCTL_RCGCPWM_R0;
	// enable the clock to Port B
	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R1;
	// Select the PWM clock as System Clock/32
	HWREG(SYSCTL_RCC) = (HWREG(SYSCTL_RCC) & ~SYSCTL_RCC_PWMDIV_M) |
	(SYSCTL_RCC_USEPWMDIV | SYSCTL_RCC_PWMDIV_32);
	// make sure that the PWM module clock has gotten going
	while ((HWREG(SYSCTL_PRPWM) & SYSCTL_PRPWM_R0) != SYSCTL_PRPWM_R0)
	;
	// disable the PWM while initializing
	HWREG( PWM0_BASE+PWM_O_0_CTL ) = 0;
	// program generator A to go to 1 at rising compare A, 0 on falling compare A
	HWREG( PWM0_BASE+PWM_O_0_GENA) =
	(PWM_0_GENA_ACTCMPAU_ONE | PWM_0_GENA_ACTCMPAD_ZERO );
	// Set the PWM period. Since we are counting both up & down, we initialize
	// the load register to 1/2 the desired total period. We will also program
	// the match compare registers to 1/2 the desired high time
	HWREG( PWM0_BASE+PWM_O_0_LOAD) = ((PeriodInMS * PWMTicksPerMS)-1)>>1;
	// Set the initial Duty cycle on A to 50% by programming the compare value
	// to 1/2 the period to count up (or down). Technically, the value to program
	// should be Period/2 - DesiredHighTime/2, but since the desired high time is 1/2
	// the period, we can skip the subtract
	HWREG( PWM0_BASE+PWM_O_0_CMPA) = HWREG( PWM0_BASE+PWM_O_0_LOAD)>>1;
	// enable the PWM outputs
	HWREG( PWM0_BASE+PWM_O_ENABLE) |= (PWM_ENABLE_PWM0EN);
	// now configure the Port B pins to be PWM outputs
	// start by selecting the alternate function for PB6 & 7
	HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) |= ( BIT6HI);
	// now choose to map PWM to those pins, this is a mux value of 4 that we
	// want to use for specifying the function on bits 6 & 7
	HWREG(GPIO_PORTB_BASE+GPIO_O_PCTL) =
	(HWREG(GPIO_PORTB_BASE+GPIO_O_PCTL) & 0xf0fffff) +
	(4<<(6*BitsPerNibble));
	// Enable pins 6 on Port B for digital I/O
	HWREG(GPIO_PORTB_BASE+GPIO_O_DEN) |= (BIT6HI);
	// make pins 6 on Port B into outputs
	HWREG(GPIO_PORTB_BASE+GPIO_O_DIR) |= (BIT6HI);
	// set the up/down count mode, enable the PWM generator and make
	// both generator updates locally synchronized to zero count
	HWREG(PWM0_BASE+ PWM_O_0_CTL) = (PWM_0_CTL_MODE | PWM_0_CTL_ENABLE |
	PWM_0_CTL_GENAUPD_LS | PWM_0_CTL_GENBUPD_LS);
}

void PWM1_Init(void)
{
	volatile uint32_t Dummy; // use volatile to avoid over-optimization
	// start by enabling the clock to the PWM Module (PWM0)
	HWREG(SYSCTL_RCGCPWM) |= SYSCTL_RCGCPWM_R0;
	// enable the clock to Port B
	HWREG(SYSCTL_RCGCGPIO) |= SYSCTL_RCGCGPIO_R1;
	// Select the PWM clock as System Clock/32
	HWREG(SYSCTL_RCC) = (HWREG(SYSCTL_RCC) & ~SYSCTL_RCC_PWMDIV_M) |
	(SYSCTL_RCC_USEPWMDIV | SYSCTL_RCC_PWMDIV_32);
	// make sure that the PWM module clock has gotten going
	while ((HWREG(SYSCTL_PRPWM) & SYSCTL_PRPWM_R0) != SYSCTL_PRPWM_R0)
	;
	// disable the PWM while initializing
	HWREG( PWM0_BASE+PWM_O_1_CTL ) = 0;
	// program generator A to go to 1 at rising compare A, 0 on falling compare A
	HWREG( PWM0_BASE+PWM_O_1_GENA) =
	(PWM_1_GENA_ACTCMPAU_ONE | PWM_1_GENA_ACTCMPAD_ZERO );
	// Set the PWM period. Since we are counting both up & down, we initialize
	// the load register to 1/2 the desired total period. We will also program
	// the match compare registers to 1/2 the desired high time
	HWREG( PWM0_BASE+PWM_O_1_LOAD) = ((PeriodInMS * PWMTicksPerMS)-1)>>1;
	// Set the initial Duty cycle on A to 0%
	PWM1_SetDuty(0);
	HWREG( PWM0_BASE+PWM_O_1_CMPB) = HWREG( PWM0_BASE+PWM_O_1_LOAD)>>1;
	// enable the PWM outputs
	HWREG( PWM0_BASE+PWM_O_ENABLE) |= (PWM_ENABLE_PWM3EN | PWM_ENABLE_PWM2EN);
	// now configure the Port B pins to be PWM outputs
	// start by selecting the alternate function for PB4 & 5
	HWREG(GPIO_PORTB_BASE+GPIO_O_AFSEL) |= (BIT4HI);
	// now choose to map PWM to those pins, this is a mux value of 4 that we
	// want to use for specifying the function on bits 4 & 5
	HWREG(GPIO_PORTB_BASE+GPIO_O_PCTL) =
	(HWREG(GPIO_PORTB_BASE+GPIO_O_PCTL) & 0xff0ffff) + (4<<(4*BitsPerNibble));
	// Enable pins 4 on Port B for digital I/O
	HWREG(GPIO_PORTB_BASE+GPIO_O_DEN) |= (BIT4HI);
	// make pins 4 on Port B into outputs
	HWREG(GPIO_PORTB_BASE+GPIO_O_DIR) |= (BIT4HI);
	// set the up/down count mode, enable the PWM generator and make
	// both generator updates locally synchronized to zero count
	HWREG(PWM0_BASE+ PWM_O_1_CTL) = (PWM_1_CTL_MODE | PWM_1_CTL_ENABLE |
	PWM_1_CTL_GENAUPD_LS | PWM_1_CTL_GENBUPD_LS);
}

void PWM0_SetDuty(uint8_t Duty)
{
	static uint8_t LastCH0Duty;
	if (Duty > 100)
	{
		Duty = 0;
		printf("\r\nWARNING!: Duty Cycle out of range. Set to 0. [PWM0_Tiva_SetDuty]\r\n");
	}

	if (Duty == 100) /*Set duty on PB6*/
	{
		// Set the load value to 0 since the desired high time is Period/2
		// Period/2 - Period/2 = 0
		HWREG( PWM0_BASE+PWM_O_0_CMPA) = 0;
		// Since the CmpADn action (was set to zero) wins, we need to change
		// the generator events to always force high
		HWREG( PWM0_BASE+PWM_O_0_GENA) = PWM_0_GENA_ACTZERO_ONE;
	}
	else
	{
		if(LastCH0Duty == 100 || LastCH0Duty == 0){
			//Set action to force opposite
			HWREG( PWM0_BASE+PWM_O_0_GENA) = (PWM_0_GENA_ACTCMPAU_ONE | PWM_0_GENA_ACTCMPAD_ZERO );
		}
		if (Duty == 0) 
		{
			// to program 0% DC, simply set the action on zero to set the output to zero
			HWREG( PWM0_BASE+PWM_O_0_GENA) = PWM_0_GENA_ACTZERO_ZERO;
			printf("made it to the zero case\n\r");
		}
		else 
		{
			//Write new PWM to the PWM register
			HWREG( PWM0_BASE+PWM_O_0_CMPA) = HWREG( PWM0_BASE+PWM_O_0_LOAD ) - HWREG( PWM0_BASE+PWM_O_0_LOAD )*Duty/100;
		}
	}
	LastCH0Duty = Duty;
}
void PWM1_SetDuty(uint8_t Duty)
{
	static uint8_t LastPWM1Duty;
	if (Duty > 100)
	{
		Duty = 0;
		printf("\r\nWARNING: Duty Cycle out of range. [PWM_Shooter_SetDuty]\r\n");
	}
	if (Duty == 100)
	{
		// Set the load value to 0 since the desired high time is Period/2
		// Period/2 - Period/2 = 0
		HWREG( PWM0_BASE+PWM_O_1_CMPA) = 0;
		// Since the CmpADn action (was set to zero) wins, we need to change
		// the generator events to always force high
		HWREG( PWM0_BASE+PWM_O_1_GENA) = PWM_1_GENA_ACTZERO_ONE;
	}
	else
	{
		if(LastPWM1Duty == 100 || LastPWM1Duty == 0){
			//Set action to force opposite
			HWREG( PWM0_BASE+PWM_O_1_GENA) = (PWM_1_GENA_ACTCMPAU_ONE | PWM_1_GENA_ACTCMPAD_ZERO );
		}
		if (Duty == 0) 
		{
			// to program 0% DC, simply set the action on zero to set the output to zero
			HWREG( PWM0_BASE+PWM_O_1_GENA) = PWM_1_GENA_ACTZERO_ZERO;
		}
		else 
		{
			//Write new PWM to the PWM register
			HWREG( PWM0_BASE+PWM_O_1_CMPA) = HWREG( PWM0_BASE+PWM_O_1_LOAD ) - HWREG( PWM0_BASE+PWM_O_1_LOAD )*Duty/100;
		}
	}
	LastPWM1Duty = Duty;
}

void PWM0_SetFreq(uint32_t Frequency)
{
	//Calculate Period (in microseconds) from Frequency
	uint32_t Period = uS_PER_SEC/Frequency;
	uint32_t PeriodInTicks = (Period * PWMTicksPerMS)/1000;
	
	// write new Period
	HWREG( PWM0_BASE+PWM_O_0_LOAD) = (PeriodInTicks-1)>>1;
}

void PWM1_SetFreq(uint32_t Frequency)
{
	//Calculate Period (in microseconds) from Frequency
	uint32_t Period = uS_PER_SEC/Frequency;
	uint32_t PeriodInTicks = (Period * PWMTicksPerMS)/1000;
	
	// write new Period
	HWREG( PWM0_BASE+PWM_O_1_LOAD) = (PeriodInTicks-1)>>1;
}


/*------------------------------- Footnotes -------------------------------*/
/*------------------------------ End of file ------------------------------*/
