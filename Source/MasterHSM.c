/****************************************************************************
 Module
   MasterHSM.c

 Revision
   2.0.1

 Description
   This is the master state machine in the hierarchical architecture for the
	 ME 218D automated crimping machine for Tesla

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 12/11/16	10:31	amm			 converted from template to source code for crimping machine
 02/08/12 01:39 jec      converted from MW_MasterMachine.c
 02/06/12 22:02 jec      converted to Gen 2 Events and Services Framework
 02/13/10 11:54 jec      converted During functions to return Event_t
                         so that they match the template
 02/21/07 17:04 jec      converted to pass Event_t to Start...()
 02/20/07 21:37 jec      converted to use enumerated type for events
 02/21/05 15:03 jec      Began Coding
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"
#include "MasterHSM.h"
#include "WaitingForModeSelect.h"
#include "CalibratingHSM.h"
#include "TestingHSM.h"
#include "CrimpingHSM.h"
#include "ZeroingMotorsHSM.h"


/*----------------------------- Module Defines ----------------------------*/

/*---------------------------- Module Functions ---------------------------*/
static ES_Event DuringWaitingForModeSelect( ES_Event Event);
static ES_Event DuringCalibrating( ES_Event Event);
static ES_Event DuringTesting( ES_Event Event);
static ES_Event DuringCrimping( ES_Event Event);
static ES_Event DuringZeroingMotors( ES_Event Event);


/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, though if the top level state machine
// is just a single state container for orthogonal regions, you could get
// away without it
static MasterState_t CurrentState;
// with the introduction of Gen2, we need a module level Priority var as well
static uint8_t MyPriority;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
     InitMasterSM

 Parameters
     uint8_t : the priorty of this service

 Returns
     boolean, False if error in initialization, True otherwise

 Description
     Saves away the priority,  and starts
     the top level state machine
 Notes

 Author
     J. Edward Carryer, 02/06/12, 22:06
****************************************************************************/
bool InitMasterSM ( uint8_t Priority )
{
  ES_Event ThisEvent;

  MyPriority = Priority;  // save our priority

  ThisEvent.EventType = ES_ENTRY;
  // Start the Master State machine

  StartMasterSM( ThisEvent );

  return true;
}

/****************************************************************************
 Function
     PostMasterSM

 Parameters
     ES_Event ThisEvent , the event to post to the queue

 Returns
     boolean False if the post operation failed, True otherwise

 Description
     Posts an event to this state machine's queue
 Notes

 Author
     J. Edward Carryer, 10/23/11, 19:25
****************************************************************************/
bool PostMasterSM( ES_Event ThisEvent )
{
  return ES_PostToService( MyPriority, ThisEvent);
}

/****************************************************************************
 Function
    RunMasterSM

 Parameters
   ES_Event: the event to process

 Returns
   ES_Event: an event to return

 Description
   the run function for the top level state machine 
 Notes
   uses nested switch/case to implement the machine.
 Author
   J. Edward Carryer, 02/06/12, 22:09
****************************************************************************/
ES_Event RunMasterSM( ES_Event CurrentEvent )
{
   bool MakeTransition = false;/* are we making a state transition? */
   MasterState_t NextState = CurrentState;
   ES_Event EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   ES_Event ReturnEvent = { ES_NO_EVENT, 0 }; // assume no error

    switch ( CurrentState )
   {
		 ///////////////////////////////////////////////////////////////////////////////////////////////////
       case WAITING_FOR_MODE_SELECT :       // If current state is state one
				 puts("In WAITING_FOR_MODE_SELECT in MasterHSM\r\n");
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lower level state machines to re-map
         // or consume the event
         CurrentEvent = DuringWaitingForModeSelect(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case ES_CALIBRATING_MODE_BUTTON : //If event is event one
                  // Execute action function for state one : event one
                  NextState = CALIBRATING;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = true; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent.EventType = ES_NO_EVENT;
                  break;
							 case ES_TESTING_MODE_BUTTON : //If event is event one
                  // Execute action function for state one : event one
                  NextState = TESTING;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = true; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent.EventType = ES_NO_EVENT;
                  break;
							 case ES_CRIMPING_MODE_BUTTON : //If event is event one
                  // Execute action function for state one : event one
                  NextState = CRIMPING;//Decide what the next state will be
                  // for internal transitions, skip changing MakeTransition
                  MakeTransition = true; //mark that we are taking a transition
                  // if transitioning to a state with history change kind of entry
                  EntryEventKind.EventType = ES_ENTRY;
                  // optionally, consume or re-map this event for the upper
                  // level state machine
                  ReturnEvent.EventType = ES_NO_EVENT;
                  break;
                // repeat cases as required for relevant events
            }
         }
         break;
				 
		///////////////////////////////////////////////////////////////////////////////////////////////////
				 case CALIBRATING :       // If current state is state one
					 puts("In CALIBRATING in MasterHSM\r\n");
					 // Execute During function for state one. ES_ENTRY & ES_EXIT are
					 // processed here allow the lowere level state machines to re-map
					 // or consume the event
					 CurrentEvent = DuringCalibrating(CurrentEvent);
//					 puts("Finished DuringCalibrating call in CALIBRATING state of MasterHSM\r\n");
//					 printf("Event was %d\r\n", CurrentEvent); 
				 //process any events
					 if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
					 {
//						 	puts("DuringCalibrating returned some event\r\n");
							switch (CurrentEvent.EventType)
							{
//								 puts("Switching events in CALIBRATING in MasterHSM\r\n");
								 case ES_EXIT_BUTTON : //If event is event one !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
										// Execute action function for state one : event one
										NextState = ZEROING_MOTORS;//Decide what the next state will be
										// for internal transitions, skip changing MakeTransition
										MakeTransition = true; //mark that we are taking a transition
										// if transitioning to a state with history change kind of entry
										EntryEventKind.EventType = ES_ENTRY;
										// optionally, consume or re-map this event for the upper
										// level state machine
										ReturnEvent.EventType = ES_NO_EVENT;
										break;
									// repeat cases as required for relevant events
							}
					 } else {
//						 puts("DuringCalibrating returned ES_NO_EVENT\r\n");
					 }
				break;
				 
				 ///////////////////////////////////////////////////////////////////////////////////////////////////
				 case TESTING :       // If current state is state one
					 puts("In TESTING in MasterHSM\r\n");
					 // Execute During function for state one. ES_ENTRY & ES_EXIT are
					 // processed here allow the lowere level state machines to re-map
					 // or consume the event
					 CurrentEvent = DuringTesting(CurrentEvent);
					 //process any events
					 if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
					 {
							switch (CurrentEvent.EventType)
							{
								 case ES_EXIT_BUTTON : //If event is event one
										// Execute action function for state one : event one
										NextState = ZEROING_MOTORS;//Decide what the next state will be
										// for internal transitions, skip changing MakeTransition
										MakeTransition = true; //mark that we are taking a transition
										// if transitioning to a state with history change kind of entry
										EntryEventKind.EventType = ES_ENTRY;
										// optionally, consume or re-map this event for the upper
										// level state machine
										ReturnEvent.EventType = ES_NO_EVENT;
										break;
									// repeat cases as required for relevant events
							}
					 }
         break;
				 
				 ///////////////////////////////////////////////////////////////////////////////////////////////////
				 case CRIMPING :       // If current state is state one
					 puts("In CRIMPING in MasterHSM\r\n");
					 // Execute During function for state one. ES_ENTRY & ES_EXIT are
					 // processed here allow the lowere level state machines to re-map
					 // or consume the event
					 CurrentEvent = DuringCrimping(CurrentEvent);
					 //process any events
					 if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
					 {
							switch (CurrentEvent.EventType)
							{
								 case ES_EXIT_BUTTON : //If event is event one
										// Execute action function for state one : event one
										NextState = ZEROING_MOTORS;//Decide what the next state will be
										// for internal transitions, skip changing MakeTransition
										MakeTransition = true; //mark that we are taking a transition
										// if transitioning to a state with history change kind of entry
										EntryEventKind.EventType = ES_ENTRY;
										// optionally, consume or re-map this event for the upper
										// level state machine
										ReturnEvent.EventType = ES_NO_EVENT;
										break;
									// repeat cases as required for relevant events
							}
					 }
         break;
				 
				 ///////////////////////////////////////////////////////////////////////////////////////////////////
				 case ZEROING_MOTORS :       // If current state is state one
					 puts("In ZEROING_MOTORS in MasterHSM\r\n");
					 // Execute During function for state one. ES_ENTRY & ES_EXIT are
					 // processed here allow the lowere level state machines to re-map
					 // or consume the event
					 CurrentEvent = DuringZeroingMotors(CurrentEvent);
					 //process any events
					 if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
					 {
							switch (CurrentEvent.EventType)
							{
								 case ES_MOTORS_ZEROED : //If event is event one
										// Execute action function for state one : event one
										NextState = WAITING_FOR_MODE_SELECT;//Decide what the next state will be
										// for internal transitions, skip changing MakeTransition
										MakeTransition = true; //mark that we are taking a transition
										// if transitioning to a state with history change kind of entry
										EntryEventKind.EventType = ES_ENTRY;
										// optionally, consume or re-map this event for the upper
										// level state machine
										ReturnEvent.EventType = ES_NO_EVENT;
										break;
									// repeat cases as required for relevant events
							}
					 }
         break;
				 ///////////////////////////////////////////////////////////////////////////////////////////////////
      // repeat state pattern as required for other states
    }
    //   If we are making a state transition
    if (MakeTransition == true)
    {
       //   Execute exit function for current state
       CurrentEvent.EventType = ES_EXIT;
       RunMasterSM(CurrentEvent);

       CurrentState = NextState; //Modify state variable

       // Execute entry function for new state
       // this defaults to ES_ENTRY
       RunMasterSM(EntryEventKind);
     }
   // in the absence of an error the top level state machine should
   // always return ES_NO_EVENT, which we initialized at the top of func
   return(ReturnEvent);
}
/****************************************************************************
 Function
     StartMasterSM

 Parameters
     ES_Event CurrentEvent

 Returns
     nothing

 Description
     Does any required initialization for this state machine
 Notes

 Author
     J. Edward Carryer, 02/06/12, 22:15
****************************************************************************/
void StartMasterSM ( ES_Event CurrentEvent )
{
  // if there is more than 1 state to the top level machine you will need 
  // to initialize the state variable
  CurrentState = WAITING_FOR_MODE_SELECT;
  // now we need to let the Run function init the lower level state machines
  // use LocalEvent to keep the compiler from complaining about unused var
  RunMasterSM(CurrentEvent);
  return;
}


/***************************************************************************
 PRIVATE FUNCTIONS
 ***************************************************************************/

/***************************************************************************
 private DuringWaitingForModeSelect
 ***************************************************************************/
static ES_Event DuringWaitingForModeSelect( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assme no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        // implement any entry actions required for this state machine
        
        // after that start any lower level machines that run in this state
        //StartLowerLevelSM( Event );
        // repeat the StartxxxSM() functions for concurrent state machines
        // on the lower level
    }
    else if ( Event.EventType == ES_EXIT )
    {
        // on exit, give the lower levels a chance to clean up first
        //RunLowerLevelSM(Event);
        // repeat for any concurrently running state machines
        // now do any local exit functionality
      
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
        // ReturnEvent = RunLowerLevelSM(Event);
      
        // repeat for any concurrent lower level machines
      
        // do any activity that is repeated as long as we are in this state
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}

/***************************************************************************
 private DuringCalibrating
 ***************************************************************************/
static ES_Event DuringCalibrating( ES_Event Event)
{
//	puts("Inside DuringCalibrating in MasterHSM\r\n");
    ES_Event ReturnEvent = Event; // assme no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        // implement any entry actions required for this state machine
        
        // after that start any lower level machines that run in this state
        //StartLowerLevelSM( Event );
//				puts("Instructing MasterHSM to RunCalibratingSM(Event) from if clause\r\n");
        StartCalibratingSM(Event);
        // repeat the StartxxxSM() functions for concurrent state machines
        // on the lower level
    }
    else if ( Event.EventType == ES_EXIT )
    {
        // on exit, give the lower levels a chance to clean up first
        //RunLowerLevelSM(Event);
//				puts("Instructing MasterHSM to RunCalibratingSM(Event) from else if clause\r\n");
        RunCalibratingSM(Event);
        // repeat for any concurrently running state machines
        // now do any local exit functionality
      
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
			 // ReturnEvent = RunLowerLevelSM(Event);
//				puts("Instructing MasterHSM to RunCalibratingSM(Event) from else clause\r\n");
        ReturnEvent = RunCalibratingSM(Event);
      
        // repeat for any concurrent lower level machines
      
        // do any activity that is repeated as long as we are in this state
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}
/***************************************************************************
 private DuringTesting
 ***************************************************************************/
static ES_Event DuringTesting( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assme no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        /* Implement any entry actions required for this state machine */
					//Check that all encoder interrupts are enabled
					//Check that all limit switch interrupts are enabled
        
        /* after that start any lower level machines that run in this state*/
					StartTestingSM(Event);
        
				/* repeat the StartxxxSM() functions for concurrent state machines on the lower level*/
					//None
    }
    else if ( Event.EventType == ES_EXIT )
    {
        /* On exit, give the lower levels a chance to clean up first */
					RunTestingSM(Event);
        /* repeat for any concurrently running state machines */
					//None
        /* now do any local exit functionality */
					//Turn off motors
					//Re-enable encoder interrupts
					//Re-enable limit switch interrupts
      
    }else
    // do the 'during' function for this state
    {
        /* run any lower level state machine */
					ReturnEvent = RunTestingSM(Event);
        /* repeat for any concurrent lower level machines */
					//None
				/* do any activity that is repeated as long as we are in this state */
					//None
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}
/***************************************************************************
 private DuringCrimping
 ***************************************************************************/
static ES_Event DuringCrimping( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assme no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        // implement any entry actions required for this state machine
        
        // after that start any lower level machines that run in this state
        //StartLowerLevelSM( Event );
				StartCrimpingSM(Event);
        // repeat the StartxxxSM() functions for concurrent state machines
        // on the lower level
    }
    else if ( Event.EventType == ES_EXIT )
    {
        // on exit, give the lower levels a chance to clean up first
        //RunLowerLevelSM(Event);
				RunCrimpingSM(Event);
        // repeat for any concurrently running state machines
        // now do any local exit functionality
      
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
        // ReturnEvent = RunLowerLevelSM(Event);
				ReturnEvent = RunCrimpingSM(Event);
        // repeat for any concurrent lower level machines
      
        // do any activity that is repeated as long as we are in this state
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}
/***************************************************************************
 private DuringZeroingMotors
 ***************************************************************************/
static ES_Event DuringZeroingMotors( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assme no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        // implement any entry actions required for this state machine
        
        // after that start any lower level machines that run in this state
        //StartLowerLevelSM( Event );
				StartZeroingMotorsSM(Event);
        // repeat the StartxxxSM() functions for concurrent state machines
        // on the lower level
    }
    else if ( Event.EventType == ES_EXIT )
    {
        // on exit, give the lower levels a chance to clean up first
        //RunLowerLevelSM(Event);
				RunZeroingMotorsSM(Event);
        // repeat for any concurrently running state machines
        // now do any local exit functionality
      
    }else
    // do the 'during' function for this state
    {
        // run any lower level state machine
        // ReturnEvent = RunLowerLevelSM(Event);
				ReturnEvent = RunZeroingMotorsSM(Event);
        // repeat for any concurrent lower level machines
      
        // do any activity that is repeated as long as we are in this state
    }
    // return either Event, if you don't want to allow the lower level machine
    // to remap the current event, or ReturnEvent if you do want to allow it.
    return(ReturnEvent);
}
