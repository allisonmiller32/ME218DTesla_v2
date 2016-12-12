/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts
 02/08/12 adjsutments for use with the Events and Services Framework Gen2
 3/17/09  Fixed prototpyes to use Event_t
 ****************************************************************************/

#ifndef WaitingForModeSelect_H
#define WaitingForModeSelect_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { STATE_ONE_WAITING, STATE_TWO_WAITING } WaitingState_t ;


// Public Function Prototypes

ES_Event RunWaitingForModeSelectSM( ES_Event CurrentEvent );
void StartWaitingForModeSelectSM ( ES_Event CurrentEvent );
WaitingState_t QueryWaitingForModeSelectSM ( void );

#endif /*WaitingForModeSelect_H */

