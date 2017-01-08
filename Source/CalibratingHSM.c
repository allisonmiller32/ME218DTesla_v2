/****************************************************************************
 Module
   CalibratingHSM.c

 Revision
   2.0.1

 Description
   This is the lower-level calibrating state machine.
	 The purpose of this state machine is to calibrate the motors on the crimping machine.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 02/07/13 21:00 jec      corrections to return variable (should have been
                         ReturnEvent, not CurrentEvent) and several EV_xxx
                         event names that were left over from the old version
 02/08/12 09:56 jec      revisions for the Events and Services Framework Gen2
 02/13/10 14:29 jec      revised Start and run to add new kind of entry function
                         to make implementing history entry cleaner
 02/13/10 12:29 jec      added NewEvent local variable to During function and
                         comments about using either it or Event as the return
 02/11/10 15:54 jec      more revised comments, removing last comment in during
                         function that belongs in the run function
 02/09/10 17:21 jec      updated comments about internal transitions on During funtion
 02/18/09 10:14 jec      removed redundant call to RunLowerlevelSM in EV_Entry
                         processing in During function
 02/20/07 21:37 jec      converted to use enumerated type for events & states
 02/13/05 19:38 jec      added support for self-transitions, reworked
                         to eliminate repeated transition code
 02/11/05 16:54 jec      converted to implment hierarchy explicitly
 02/25/03 10:32 jec      converted to take a passed event parameter
 02/18/99 10:19 jec      built template from MasterMachine.c
 02/14/99 10:34 jec      Began Coding
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
// Basic includes for a program using the Events and Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"

/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "CalibratingHSM.h"

/*----------------------------- Module Defines ----------------------------*/
// define constants for the states for this machine
// and any other local defines

#define ENTRY_STATE DRIVING_MOTORS_STATE

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine, things like during
   functions, entry & exit functions.They should be functions relevant to the
   behavior of this state machine
*/
static ES_Event DuringDrivingMotorsState( ES_Event Event);
static ES_Event DuringDisplayingResultState( ES_Event Event);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well
static CalibratingState_t CurrentState;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
    RunCalibratingSM

 Parameters
   ES_Event: the event to process

 Returns
   ES_Event: an event to return

 Description
   add your description here
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 2/11/05, 10:45AM
****************************************************************************/
ES_Event RunCalibratingSM( ES_Event CurrentEvent )
{
	puts("In RunCalibratingSM in CalibratingHSM\r\n");
   bool MakeTransition = false;/* are we making a state transition? */
   CalibratingState_t NextState = CurrentState;
   ES_Event EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   ES_Event ReturnEvent = CurrentEvent; // assume we are not consuming event
//   printf("Event was %d\r\n", CurrentEvent); 
   switch ( CurrentState )
   {
			puts("In switch statement in RunCalibratingSM in CalibratingHSM\r\n");
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       case DRIVING_MOTORS_STATE :       // If current state is state one
				 puts("In DRIVING_MOTORS_STATE in the run mode of CalbratingHSM.c (lower level SM)\r\n");
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lower level state machines to re-map
         // or consume the event
         CurrentEvent = DuringDrivingMotorsState(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
					 puts("Switching event in DRIVING_MOTORS_STATE of RunCalibratingSM in CalibratingHSM\r\n");
            switch (CurrentEvent.EventType)
            {
               case ES_CALIBRATION_DISTANCE:
							 case ES_CALIBRATION_FORCE:
                  //Store event parameter (should be MOTOR_A or MOTOR_B)
									//If this event != user's desired termination condition
										//disable the interrupt that caused the event
										//clear calibrationResultFlag
										//Post ES_CALIBRATION_FAILURE to this state machine
									//Else if this is the first motor that's triggered a termination condition
										//Store in lastMotor variable
										//Set motor's calibrating tick count value for encoder
										//If event was ES_CALIBRATION_DISTANCE
											//Send motor to constant distance feedback loop
										//Else
											//Send motor to constant force feedback loop
										MakeTransition = false; //We're not leaving DRIVING_MOTORS_STATE as a result of this event
										//Consume the event because the master state machine doesn't need to deal with it
										ReturnEvent.EventType = ES_NO_EVENT;
									//Else if this is the second motor that's triggered a termination condition && it's different from lastMotor
										//Set motor's calibrating tick count value for the encoder
										//Set calibrationResultFlag
										NextState = DISPLAYING_RESULT_STATE;
										MakeTransition = true; //We're transitioning out of DRIVING_MOTORS_STATE
										//Consume the event because the master state machine doesn't need to deal with it
										ReturnEvent.EventType = ES_NO_EVENT;
                  break;
							 case ES_BOTH_CALIBRATION_DISTANCE:
							 case ES_BOTH_CALIBRATION_FORCE:
								 //If this event != user's desired termination condition
									//Clear calibrationResultFlag to indicate failure
								 //Else
									//Diable bothg encoder interrupts
									//Set crimping encoder tick value for both motors
									//Set calibrationResultFlag
								 break;
							 case ES_CALIBRATION_FAILURE:
                  //Clear calibrationResultFlag
                  NextState = DISPLAYING_RESULT_STATE;
									MakeTransition = true; //We're transitioning out of DRIVING_MOTORS_STATE
									//Consume the event because the master state machine doesn't need to deal with it
									ReturnEvent.EventType = ES_NO_EVENT;
                  break;
							 default:
									puts("Switching spurious event in default case\r\n");
									//If you get any event that's not handled above, do nothing
									break;
            }
         }
         break; //breaking out of DRIVING_MOTOR_STATE
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
      case DISPLAYING_RESULT_STATE:
				 puts("In DISPLAYING_RESULT_STATE in the run mode of CalbratingHSM.c (lower level SM)\r\n");
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lower level state machines to re-map
         // or consume the event
         CurrentEvent = DuringDisplayingResultState(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case ES_TIMEOUT:
									//If the timeout came from checkingFailureTimer
									if(CurrentEvent.EventParam == checkingFailureTimer){
										//Consume the event because the calibration mode didn't fail. Should exit calibration mode via displayTimer timeout, not checkingFailureTimer timeout
										ReturnEvent.EventType = ES_NO_EVENT;
									}
									//If the timeout came from the displayTimer
									if(CurrentEvent.EventParam == displayTimer){
										//Turn off successLED only
										//Do not consume the event. Upper state machine needs the ES_TIMEOUT w/ displayTimer event to exit the calibration state
										ReturnEvent = CurrentEvent;
									}
									MakeTransition = false; //We're not leaving DISPLAYING_RESULT_STATE as a result of this event
                  break;
							 default:
									puts("Switching spurious event in default case\r\n");
									//If you get any event that's not handled above, do nothing
									break;
            }
         }
         break; //breaking out of DISPLAY_RESULT_STATE
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
    //   If we are making a state transition
    if (MakeTransition == true)
    {
       //   Execute exit function for current state
       CurrentEvent.EventType = ES_EXIT;
       RunCalibratingSM(CurrentEvent);

       CurrentState = NextState; //Modify state variable

       //   Execute entry function for new state
       // this defaults to ES_ENTRY
       RunCalibratingSM(EntryEventKind);
     }
     return(ReturnEvent);
}
/****************************************************************************
 Function
     StartCalibratingSM

 Parameters
     None

 Returns
     None

 Description
     Does any required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 2/18/99, 10:38AM
****************************************************************************/
void StartCalibratingSM ( ES_Event CurrentEvent )
{
   // to implement entry to a history state or directly to a substate
   // you can modify the initialization of the CurrentState variable
   // otherwise just start in the entry state every time the state machine
   // is started
	puts("In StartCalibratingSM\r\n");
   if ( ES_ENTRY_HISTORY != CurrentEvent.EventType )
   {
        CurrentState = ENTRY_STATE;
   }
   // call the entry function (if any) for the ENTRY_STATE
   RunCalibratingSM(CurrentEvent);
}

/****************************************************************************
 Function
     QueryCalibratingSM

 Parameters
     None

 Returns
     CalibratingState_t The current state of the Calibrating state machine

 Description
     returns the current state of the Calibrating state machine
 Notes

 Author
     J. Edward Carryer, 2/11/05, 10:38AM
****************************************************************************/
CalibratingState_t QueryCalibratingSM ( void )
{
   return(CurrentState);
}

/***************************************************************************
 private functions
 ***************************************************************************/

static ES_Event DuringDrivingMotorsState( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assume no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        /* implement any entry actions required for this state machine */
					//Turn both motors on
        /* after that start any lower level machines that run in this state with StartLowerLevelSM(Event); */
					//None
    }
    else if ( Event.EventType == ES_EXIT )
    {
        /* on exit, give the lower levels a chance to clean up first with RunLowerLevelSM(Event); */
					//None
        /* now do any local exit functionality */
					//Stop motors
					//Turn off inProgressLED
      
    }else
    // do the 'during' function for this state
    {
        /* run any lower level state machines with ReturnEvent = RunLowerLevelSM(Event) */
					//None
        /* do any activity that is repeated as long as we are in this state */
					//None
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}

static ES_Event DuringDisplayingResultState( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assume no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        /* implement any entry actions required for this state machine */
					//Start displayTimer
					//If calibrationResultFlag is set (aka successful calibraiton occurred)
						//Turn on okToCrimpLED
						//Set okToCrimpFlag
					//Else
						//Turn on failLED
        /* after that start any lower level machines that run in this state with StartLowerLevelSM(Event); */
					//None
    }
    else if ( Event.EventType == ES_EXIT )
    {
        /* on exit, give the lower levels a chance to clean up first with RunLowerLevelSM(Event); */
					//None
        /* now do any local exit functionality */
					//Clear calibrationResultFlag
					//Re-enable encoder interrupts
      
    }else
    // do the 'during' function for this state
    {
        /* run any lower level state machines with ReturnEvent = RunLowerLevelSM(Event) */
					//None
        /* do any activity that is repeated as long as we are in this state */
					//None
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}
