/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts
 02/08/12 adjsutments for use with the Events and Services Framework Gen2
 3/17/09  Fixed prototpyes to use Event_t
 ****************************************************************************/

#ifndef ZeroingMotors_H
#define ZeroingMotors_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { STATE_ONE_ZEROING, STATE_TWO_ZEROING } ZeroingState_t ;


// Public Function Prototypes

ES_Event RunZeroingMotorsSM( ES_Event CurrentEvent );
void StartZeroingMotorsSM ( ES_Event CurrentEvent );
ZeroingState_t QueryZeroingMotorsSM ( void );

#endif /*ZeroingMotors_H */

