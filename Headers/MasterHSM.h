/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts

 ****************************************************************************/

#ifndef MasterHSM_H
#define MasterHSM_H

// State definitions for use with the query function
typedef enum { WAITING_FOR_MODE_SELECT, CALIBRATING, TESTING, CRIMPING, ZEROING_MOTORS } MasterState_t ;

// Public Function Prototypes

ES_Event RunMasterSM( ES_Event CurrentEvent );
void StartMasterSM ( ES_Event CurrentEvent );
bool PostMasterSM( ES_Event ThisEvent );
bool InitMasterSM ( uint8_t Priority );

#endif /*MasterHSM_H */

