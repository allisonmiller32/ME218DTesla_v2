/****************************************************************************
  Header file for Hardware Service
 ****************************************************************************/

#ifndef Hardware_H
#define Hardware_H

//Common Headers
#include <stdint.h>
#include <stdbool.h>

/*----------------------------- Module Defines ----------------------------*/
#define AD_HOW_MANY           2

/*---------------------- Public Function Prototypes -----------------------*/
void InitAll(void);
void InitPorts(void);
void InitInput(int Port, uint8_t Pin);
uint8_t ReadInput(int Port, uint8_t Pin);
void InitOutput(int Port, uint8_t Pin);
void SetOutput(int Port, uint8_t Pin, bool SetHi);

#endif /* Hardware_H */

