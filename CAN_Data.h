/****************************************************************************
  CAN_Data.h
	
	CAN_Data stores all the information received from CAN communications
 ****************************************************************************/

#ifndef CAN_Data_H
#define CAN_Data_H

#include <stdint.h>
/*----------------------------- Module Defines ----------------------------*/
typedef enum { NO_TERMINATION_CONDITION_SET, FORCE_TERMINATION, DISTANCE_TERMINATION } TerminationType_t ;

/*---------------------- Public Function Prototypes -----------------------*/
void setTerminationCondition(TerminationType_t userCondition);

TerminationType_t getTerminationCondition(void);

void setTerminationForce(uint16_t userForce); //force in Newtons

uint16_t getTerminationForce(void); //force in Newtons

void setTerminationDistance(uint16_t userDistance); //distance in millimeters

uint16_t getTerminationDistance(void); //distance in millimeters

#endif /* CAN_Data_H */
