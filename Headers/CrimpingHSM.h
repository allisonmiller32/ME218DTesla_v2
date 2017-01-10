/****************************************************************************
 Template header file for Hierarchical Sate Machines AKA StateCharts
 02/08/12 adjsutments for use with the Events and Services Framework Gen2
 3/17/09  Fixed prototpyes to use Event_t
 ****************************************************************************/

#ifndef Crimping_H
#define Crimping_H


// typedefs for the states
// State definitions for use with the query function
typedef enum { WAITING_FOR_CRIMP_BUTTON_STATE, CRIMPING_STATE, REPORTING_CRIMP_QUALITY_STATE, PREPARING_FOR_NEXT_CRIMP_STATE } CrimpingState_t ;


// Public Function Prototypes

ES_Event RunCrimpingSM( ES_Event CurrentEvent );
void StartCrimpingSM ( ES_Event CurrentEvent );
CrimpingState_t QueryCrimpingSM ( void );

#endif /*Crimping_H */

