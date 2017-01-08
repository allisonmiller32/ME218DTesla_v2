/****************************************************************************
  Header file for Hardware Service
 ****************************************************************************/

#ifndef Motors_H
#define Motors_H

//Common Headers
#include <stdint.h>
#include <stdbool.h>

/*----------------------------- Module Defines ----------------------------*/

#define MOTOR1_PORT	GPIO_PORTA_BASE
#define MOTOR1_ENABLE_PIN GPIO_PIN_2
#define MOTOR1_DIRECTION_PIN GPIO_PIN_3
#define MOTOR1_FORWARD 0x01
#define MOTOR1_REVERSE 0x02

#define MOTOR2_PORT	GPIO_PORTA_BASE
#define MOTOR2_ENABLE_PIN GPIO_PIN_4
#define MOTOR2_DIRECTION_PIN GPIO_PIN_5
#define MOTOR2_FORWARD 0x01
#define MOTOR2_REVERSE 0x02

#define MOTOR_PORT						GPIO_PORTB_BASE
#define MOTOR_1_PIN						GPIO_PIN_0
#define MOTOR_2_PIN						GPIO_PIN_1
#define MOTOR_1_CHANNEL       PWM0_CHANNEL_0 
#define MOTOR_2_CHANNEL       PWM0_CHANNEL_1

#define LO										0
#define HI										1

#define AD_HOW_MANY           2

/*---------------------- Public Function Prototypes -----------------------*/
//assumes all ports have been enabled
void InitMotors(void);

//takes in a boolean and enables|disables the motor driver
void Motor1Enable(bool value);

//Changes the direction of the motor drivers
//pass in MOTOR1_FORWARD or MOTOR1_REVERSE only
void Motor1Direction(uint8_t direction);

void Motor2Enable(bool value);

//Changes the direction of the motor drivers
//pass in MOTOR2_FORWARD or MOTOR2_REVERSE only
void Motor2Direction(uint8_t direction);


uint32_t GetMotor1EdgeCount(void);
uint32_t GetMotor2EdgeCount(void);

void ResetMotor1EdgeCount(void);
void ResetMotor2EdgeCount(void);

#endif /* Motors_H */
