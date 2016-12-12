/****************************************************************************
 
  Header file for PWM0_Tiva module

 ****************************************************************************/

#ifndef PWM0_Tiva_H
#define PWM0_Tiva_H

#include <stdint.h>

// Public Function Prototypes
//PWM0 = PB6
void PWM0_Init ( void );
void PWM0_SetDuty (uint8_t Duty);
void PWM0_SetFreq (uint32_t Freq);
 
//PWM1 = PB4
void PWM1_Init ( void );
void PWM1_SetDuty ( uint8_t Duty);
void PWM1_SetFreq (uint32_t Freq);

#endif /* PWM0_Tiva_H */
