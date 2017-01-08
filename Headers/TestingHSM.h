/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts
 02/08/12 adjsutments for use with the Events and Services Framework Gen2
 3/17/09  Fixed prototpyes to use Event_t
 ****************************************************************************/

#ifndef Testing_H
#define Testing_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { ACCEPTING_USER_INPUT_STATE } TestingState_t ;


// Public Function Prototypes

ES_Event RunTestingSM( ES_Event CurrentEvent );
void StartTestingSM ( ES_Event CurrentEvent );
TestingState_t QueryTestingSM ( void );

#endif /*Testing_H */

