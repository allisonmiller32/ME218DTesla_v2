/****************************************************************************
 Module
   CAN_Data.c

 Revision
   1.0

 Description
   -Stores all the information received from CAN communications

 Notes

 History
 When           	Who     What/Why
 -------------- 	---     --------
 01/05/17 2:44PM	amm			Began coding

****************************************************************************/

/*----------------------------- Include Files -----------------------------*/
// Basic includes for a program using the Events and Services Framework
#include <stdint.h>

/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "CAN_Data.h"

/*----------------------------- Module Defines ----------------------------*/
// define constants for the states for this machine and any other local defines

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine, things like during
   functions, entry & exit functions.They should be functions relevant to the
   behavior of this state machine
*/


/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well
static TerminationType_t terminationCondition = NO_TERMINATION_CONDITION_SET;
static uint16_t terminationForce = 0; //In Newtons
static uint16_t terminationDistance = 0; //In millimeters


/*------------------------------ Module Code ------------------------------*/

void setTerminationCondition(TerminationType_t userCondition){
	terminationCondition = userCondition;
}

TerminationType_t getTerminationCondition(void){
	return terminationCondition;
}

void setTerminationForce(uint16_t userForce){ //force in Newtons
	terminationForce = userForce;
}

uint16_t getTerminationForce(void){
	return terminationForce;
}

void setTerminationDistance(uint16_t userDistance){ //distance in millimeters
	terminationDistance = userDistance;
}

uint16_t getTerminationDistance(void){
	return terminationDistance;
}
