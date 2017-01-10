/****************************************************************************
 Module
   TestingHSM.c

 Revision
   1.0

 Description
   -Controls the program flow during the testing state of the crimping machine.
	 -The testing mode (TestingHSM) is activated from the WaitingForModeSelect state
		in the MasterHSM by pressing the testing mode button.
	 -The testing mode (TestingHSM) can be quit by pressing the exit button at any time.
	 -TestingHSM only allows for one motor running at a time. Motor movement is
	 controlled with forward/backward buttons, which must be pressed and held down
	 for a motor to move.

  Notes:
    -Original code structure written by J. Edward Carryer of Stanford University
    -Code modified for crimping machine by Allison Miller

 History
 When           Who     What/Why
 -------------- ---     --------
 12/15/16  		  jec     Template obtained from Stanford/Ed Carryer
 01/05/17 1:40	amm			Template converted to implement testing mode structure

****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
// Basic includes for a program using the Events and Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"

/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "TestingHSM.h"

/*----------------------------- Module Defines ----------------------------*/
// define constants for the states for this machine
// and any other local defines

#define ENTRY_STATE ACCEPTING_USER_INPUT_STATE

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine, things like during
   functions, entry & exit functions.They should be functions relevant to the
   behavior of this state machine
*/
static ES_Event DuringAcceptingUserInputState( ES_Event Event);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well
static TestingState_t CurrentState;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
    RunTestingSM

 Parameters
   ES_Event: the event to process

 Returns
   ES_Event: an event to return

 Description
   Implementation of crimping machine's testing mode
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 2/11/05, 10:45AM
	 Allison Miller,		1/5/17,	 1:44PM
****************************************************************************/
ES_Event RunTestingSM( ES_Event CurrentEvent )
{
   bool MakeTransition = false;/* are we making a state transition? */
   TestingState_t NextState = CurrentState;
   ES_Event EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   ES_Event ReturnEvent = CurrentEvent; // assume we are not consuming event

   switch ( CurrentState )
   {
       case ACCEPTING_USER_INPUT_STATE :       // If current state is state one
				 puts("In ACCEPTING_USER_INPUT_STATE in the run mode of TestingHSM.c (lower level SM)\r\n");
         /* Execute During function for ACCEPTING_USER_INPUT_STATE. ES_ENTRY & ES_EXIT are
          processed here allow the lower level state machines to re-map
          or consume the event
				*/
         CurrentEvent = DuringAcceptingUserInputState(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
							/*The events (ES_LIMIT_SWITCH_HIT, ES_MAX_TRAVEL_LIMIT_HIT, ES_MIN_TRAVEL_LIMIT_HIT)
							   occur if the motors move outside their allowed range (either optical limit switches are hit or encoder count goes out of allowable range).
								 All of these events produce the same actions. Their case statements must be kept next to each other.
							*/
							case ES_LIMIT_SWITCH_HIT: 
							case ES_MAX_TRAVEL_LIMIT_HIT:
							case ES_MIN_TRAVEL_LIMIT_HIT:
									puts("In ES_LIMIT_SWITCH_HIT, || ES_MAX_TRAVEL_LIMIT_HIT || ES_MIN_TRAVEL_LIMIT_HIT in ACCEPTING_USER_INPUT_STATE in TestingHSM.c\r\n");
                  /* Execute action function for this state/event combination */
									//Store event parameter (should be MOTOR_A or MOTOR_B
									//Turn that motor off
									//Disable encoder interrupt for that motor if it was a travel limit event
									//Disable limit switch interrupt for that motor if it was a limit switch event
                  MakeTransition = false; //We're not leaving ACCEPTING_USER_INPUT_STATE
									//Consume the event because the master state machine doesn't need to deal with it
                  ReturnEvent.EventType = ES_NO_EVENT;
                  break;
             	case ES_MOTOR_BUTTON_UP:
									puts("In ES_MOTOR_BUTTON_UP in ACCEPTING_USER_INPUT_STATE in TestingHSM.c\r\n");
                  /* Execute action function for this state/event combination */
									//Store event parameter (should be MOTOR_A or MOTOR_B)
									//Turn that motor off
                  MakeTransition = false; //We're not leaving ACCEPTING_USER_INPUT_STATE
                  //Consume the event because the master state machine doesn't need to deal with it
                  ReturnEvent.EventType = ES_NO_EVENT;
                  break;
							case ES_MOTOR_FORWARD_DOWN:
									puts("In ES_MOTOR_FORWARD_DOWN in ACCEPTING_USER_INPUT_STATE in TestingHSM.c\r\n");
									//Store event parameter (should be MOTOR_A or MOTOR_B)
									//Guard 1: Other motor is off
									//Guard 2: Store CAN motor speed locally and only continue if it's in the allowable range
										/* Execute action function for this state/event combination */
										//If the motor's limit switch interrupt is disabled, then don't turn the motor on
										//Else if the motor's encoder interrupt is off, then turn it on and turn the motor on
										//Else turn the motor on
										MakeTransition = false; //We're not leaving ACCEPTING_USER_INPUT_STATE
										//Consume the event because the master state machine doesn't need to deal with it
										ReturnEvent.EventType = ES_NO_EVENT;
										break;
							case ES_MOTOR_BACKWARD_DOWN:
									puts("In ES_MOTOR_BACKWARD_DOWN in ACCEPTING_USER_INPUT_STATE in TestingHSM.c\r\n");
									//Store event parameter (should be MOTOR_A or MOTOR_B)
									//Guard 1: Other motor is off
									//Guard 2: Store CAN motor speed locally and only continue if it's in the allowable range
										/* Execute action function for this state/event combination */
										//If the encoder interrupt is off, don't turn motor on
										//Else if 1) Turn motor on first 2) Then if that motor's limit switch interrupt is off, turn it on
										//Else turn motor on
										MakeTransition = false; //We're not leaving ACCEPTING_USER_INPUT_STATE
										//Consume the event because the master state machine doesn't need to deal with it
										ReturnEvent.EventType = ES_NO_EVENT;
										break;
							default:
										puts("Switching spurious event in default case in ACCEPTING_USER_INPUT_STATE in TestingHSM.c\r\n");
										//If you get any event that's not handled above, do nothing
									break;
            }
         }
         break; //breaks out of ACCEPTING_USER_INPUT_STATE case
    }
	 
    //   If we are making a state transition
    if (MakeTransition == true)
    {
       //   Execute exit function for current state
       CurrentEvent.EventType = ES_EXIT;
       RunTestingSM(CurrentEvent);

       CurrentState = NextState; //Modify state variable

       //   Execute entry function for new state
       // this defaults to ES_ENTRY
       RunTestingSM(EntryEventKind);
     }
     return(ReturnEvent);
}
/****************************************************************************
 Function
     StartTestingSM

 Parameters
     None

 Returns
     None

 Description
     Does any required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 2/18/99, 10:38AM
		 Allison Miller, 1/5/17, 1:45PM
****************************************************************************/
void StartTestingSM ( ES_Event CurrentEvent )
{
   // to implement entry to a history state or directly to a substate
   // you can modify the initialization of the CurrentState variable
   // otherwise just start in the entry state every time the state machine
   // is started
   if ( ES_ENTRY_HISTORY != CurrentEvent.EventType )
   {
        CurrentState = ENTRY_STATE;
   }
   // call the entry function (if any) for the ENTRY_STATE
   RunTestingSM(CurrentEvent);
}

/****************************************************************************
 Function
     QueryTestingSM

 Parameters
     None

 Returns
     TestingState_t The current state of the Testing state machine

 Description
     returns the current state of the Testing state machine
 Notes

 Author
     J. Edward Carryer, 2/11/05, 10:38AM
****************************************************************************/
TestingState_t QueryTestingSM ( void )
{
   return(CurrentState);
}

/***************************************************************************
 private functions
 ***************************************************************************/

static ES_Event DuringAcceptingUserInputState( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assme no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        /* implement any entry actions required for this state machine */
        //Check that all encoder and limit switch interrupts are enabled
			
        /* after that start any lower level machines that run in this state with StartLowerLevelSM( Event )
		       repeat the StartxxxSM() functions for concurrent state machines
					on the lower level
				*/
        //No lower level state machines
    }
    else if ( Event.EventType == ES_EXIT )
    {
        /* on exit, give the lower levels a chance to clean up first with RunLowerLevelSM(Event);
				   repeat for any concurrently running state machines
				*/
				//No lower level state machines
			
        /* now do any local exit functionality */
				//Turn off motors
				//Re-enable encoder interrupts
				//Re-enable limnit switch interrupts
      
    }else
    // do the 'during' function for this state
    {
				/* Run any lower level state machine with ReturnEvent = RunLowerLevelSM(Event);
					 Repeat for any concurrent lower level machines
				*/
				//No lower level state machines
			
        /*do any activity that is repeated as long as we are in this state */
				//None
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}
