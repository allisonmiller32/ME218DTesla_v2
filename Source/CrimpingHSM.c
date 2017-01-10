/****************************************************************************
 Module
   Crimping.c

 Revision
   1.0

 Description
   -This is the lower-level crimping state machine.
   -The purpose of this state machine is to perform the crimping motion and
   report the quality of the crimp to the user

  Notes:
    -Original code structure written by J. Edward Carryer of Stanford University
    -Code modified for crimping machine by Allison Miller

 History
 When           Who     What/Why
 -------------- ---     --------
 12/15/16       jec     Template obtained from Stanford/Ed Carryer
 01/08/17 1:40  amm     Template converted to implement crimping mode structure
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
// Basic includes for a program using the Events and Services Framework
#include "ES_Configure.h"
#include "ES_Framework.h"

/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "CrimpingHSM.h"

/*----------------------------- Module Defines ----------------------------*/
// define constants for the states for this machine
// and any other local defines

#define ENTRY_STATE WAITING_FOR_CRIMP_BUTTON_STATE

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this machine, things like during
   functions, entry & exit functions.They should be functions relevant to the
   behavior of this state machine
*/
static ES_Event DuringWaitingForCrimpButtonState( ES_Event Event);
static ES_Event DuringCrimpingState( ES_Event Event);
static ES_Event DuringReportingCrimpQualityState( ES_Event Event);
static ES_Event DuringPreparingForNextCrimpState( ES_Event Event);

/*---------------------------- Module Variables ---------------------------*/
// everybody needs a state variable, you may need others as well
static CrimpingState_t CurrentState;

/*------------------------------ Module Code ------------------------------*/
/****************************************************************************
 Function
    RunCrimpingSM

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
ES_Event RunCrimpingSM( ES_Event CurrentEvent )
{
   bool MakeTransition = false;/* are we making a state transition? */
   CrimpingState_t NextState = CurrentState;
   ES_Event EntryEventKind = { ES_ENTRY, 0 };// default to normal entry to new state
   ES_Event ReturnEvent = CurrentEvent; // assume we are not consuming event

   switch ( CurrentState )
   {
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       case WAITING_FOR_CRIMP_BUTTON_STATE :
				 puts("In WAITING_FOR_CRIMP_BUTTON_STATE in the run mode of CrimpingHSM.c (lower level SM)\r\n");
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lower level state machines to re-map
         // or consume the event
         CurrentEvent = DuringWaitingForCrimpButtonState(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case ES_CRIMP_BUTTON_DOWN:
                  //Guard: Store CAN speed/distance/forces for this crimp and only run if these are i the allowable range
                  //Set needCANInfo = false
                  NextState = CRIMPING_STATE; //Decide what the next state will be
                  MakeTransition = true; //We're transitioning out of WAITING_FOR_CRIMP_BUTTON_STATE
                  //Consume the event because the upper-level master state machine doesn't need to deal with it
                  ReturnEvent.EventType = ES_NO_EVENT;
                  break;
               case ES_TIMEOUT :
                  //If the timeout came from checkingFailureTimer
                  if(CurrentEvent.EventParam == checkingFailureTimer){
                    //Consume the event because we don't want to leave crimping mode based off an old checkingFailureTimer firing here
                    ReturnEvent.EventType = ES_NO_EVENT;
                  }
                  MakeTransition = false; //We're not leaving WAITING_FOR_CRIMPING_BUTTON_STATE as a result of this event
                  break;

            }
         }
         break; //break out of WAITING_FOR_CRIMP_BUTTON_STATE
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       case CRIMPING_STATE :
         puts("In CRIMPING_STATE in the run mode of CrimpingHSM.c (lower level SM)\r\n");
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lower level state machines to re-map
         // or consume the event
         CurrentEvent = DuringCrimpingState(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case ES_CRIMP_BUTTON_UP :
                  //Set motorCombo to whichever motors are on
                  //Turn motors off
                  MakeTransition = false; //We're not leaving CRIMPING_STATE as a result of this event
                  //Consume the event because the upper-level master state machine doesn't need to deal with it
                  ReturnEvent.EventType = ES_NO_EVENT;
                  break;
               case ES_CRIMP_BUTTON_DOWN :
                  //Turn on motors stored in motorCombo
                  MakeTransition = false; //We're not leaving CRIMPING_STATE as a result of this event
                  //Consume the event because the upper-level master state machine doesn't need to deal with it
                  ReturnEvent.EventType = ES_NO_EVENT;
                  break;
               case ES_CRIMP_DISTANCE :
               case ES_CRIMP_FORCE :
                  //Store event parameter (should be MOTOR_A or MOTOR_B)
                  //If this event != the user's desired termination event
                    //Disable the interrupt that caused the event
                    //Clear crimpResultFlag
                    //Post ES_CRIMP_FAILURE to this state machine
                  //Else if this is the first motor that's triggered a termination condition, for this motor...
                    //Store in lastMotor variable
                    //Disable the motor's encoder interrupt
                    //If event was ES_CRIMP_DISTANCE
                      //Send motor to constant distance feedback loop
                    //Else if event was ES_CRIMP_FORCE
                      //Send motor to constant force feedback loop
                    MakeTransition = false; //We're not leaving CRIMPING_STATE as a result of this event
                    //Consume the event because the upper-level master state machine doesn't need to deal with it
                    ReturnEvent.EventType = ES_NO_EVENT;
                  //Else if this is the second motor that's triggered a termination condition && it's different from lastMotor
                    //Set crimpResultFlag
                    NextState = REPORTING_CRIMP_QUALITY_STATE;
                    MakeTransition = true; //We're transitioning out of CRIMPING_STATE
                    //Consume the event because the upper-level master state machine doesn't need to deal with it
                    ReturnEvent.EventType = ES_NO_EVENT;
                  break;
               case ES_TIMEOUT :
                  //If the timeout came from checkingFailureTimer
                  if(CurrentEvent.EventParam == checkingFailureTimer){
                    //Clear crimpResultFlag
                    //Consume the event because the calibration mode didn't fail. Should exit calibration mode via displayTimer timeout, not checkingFailureTimer timeout (we want to actually report the bad crimp, and not just immediately leave the crimping mode)
                    ReturnEvent.EventType = ES_NO_EVENT;
                  }
                  NextState = REPORTING_CRIMP_QUALITY_STATE;
                  MakeTransition = true; //We're transitioning out of CRIMPING_STATE
                  break;
            }
         }
         break; //break out of WAITING_FOR_CRIMP_BUTTON_STATE
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       case REPORTING_CRIMP_QUALITY_STATE :
         puts("In REPORTING_CRIMP_QUALITY_STATE in the run mode of CrimpingHSM.c (lower level SM)\r\n");
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lower level state machines to re-map
         // or consume the event
         CurrentEvent = DuringReportingCrimpQualityState(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case ES_TIMEOUT:
                  //If the timeout came from checkingFailureTimer
                  if(CurrentEvent.EventParam == checkingFailureTimer){
                    //Consume the event because we don't want to exit the crimping mode while the crimp result is being reported
                    ReturnEvent.EventType = ES_NO_EVENT;
                    MakeTransition = false; //We're not leaving REPORTING_CRIMP_QUALITY_STATE as a result of this event
                  }
                  //If the timeout came from displayTimer
                  if(CurrentEvent.EventParam == displayTimer){
                    //Start both motors running toward their limit switches (home position)
                    NextState = PREPARING_FOR_NEXT_CRIMP_STATE;
                    MakeTransition = true; //We're transitioning out of REPORTING_CRIMP_QUALITY_STATE
                    //Consume the event because we don't want to exit the crimping mode before we go through the process of preparing for the next crimp
                    ReturnEvent.EventType = ES_NO_EVENT;
                  }
                  break;
            }
         }
         break; //break out of REPORTING_CRIMP_QUALITY_STATE
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
       case PREPARING_FOR_NEXT_CRIMP_STATE :
         puts("In PREPARING_FOR_NEXT_CRIMP_STATE in the run mode of CrimpingHSM.c (lower level SM)\r\n");
         // Execute During function for state one. ES_ENTRY & ES_EXIT are
         // processed here allow the lower level state machines to re-map
         // or consume the event
         CurrentEvent = DuringPreparingForNextCrimpState(CurrentEvent);
         //process any events
         if ( CurrentEvent.EventType != ES_NO_EVENT ) //If an event is active
         {
            switch (CurrentEvent.EventType)
            {
               case ES_ENCODER_HOMED :
                  //Store event parameter (should be MOTOR_A or MOTOR_B)
                  //If this is the first motor that's had its encoder homed, for that motor...
                    //Store the motor in the lastMotor variable
                    //Disable motor's encoder interrupt (and the encoder interrupt??)
                    //Stop motor
                  //Else if this is the second motor that's had its encoder homed && it's different from lastMotor
                    //Disable the motor's encoder interrupt
                    //Stop motor
                    //Post ES_READY_FOR_NEW_CRIMP to this state machine
                  MakeTransition = false; //We're not leaving PREPARING_FOR_NEXT_CRIMP_STATE as a result of this event
                  //Consume the event because the upper-level master state machine doesn't need to deal with it
                  ReturnEvent.EventType = ES_NO_EVENT;
                  break;
               case ES_READY_FOR_NEW_CRIMP :
                  NextState = WAITING_FOR_CRIMP_BUTTON_STATE;
                  MakeTransition = true; //We're transitioning out of PREPARING_FOR_NEXT_CRIMP_STATE
                  //Consume the event because the upper-level master state machine doesn't need to deal with it
                  ReturnEvent.EventType = ES_NO_EVENT;
                  break;
            }
         }
         break; //break out of PREPARING_FOR_NEXT_CRIMP_STATE
      //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    }
    //   If we are making a state transition
    if (MakeTransition == true)
    {
       //   Execute exit function for current state
       CurrentEvent.EventType = ES_EXIT;
       RunCrimpingSM(CurrentEvent);

       CurrentState = NextState; //Modify state variable

       //   Execute entry function for new state
       // this defaults to ES_ENTRY
       RunCrimpingSM(EntryEventKind);
     }
     return(ReturnEvent);
}
/****************************************************************************
 Function
     StartCrimpingSM

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
void StartCrimpingSM ( ES_Event CurrentEvent )
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
   RunCrimpingSM(CurrentEvent);
}

/****************************************************************************
 Function
     QueryCrimpingSM

 Parameters
     None

 Returns
     CrimpingState_t The current state of the Crimping state machine

 Description
     returns the current state of the Crimping state machine
 Notes

 Author
     J. Edward Carryer, 2/11/05, 10:38AM
****************************************************************************/
CrimpingState_t QueryCrimpingSM ( void )
{
   return(CurrentState);
}

/***************************************************************************
 private functions
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static ES_Event DuringWaitingForCrimpButtonState( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assume no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        /* implement any entry actions required for this state machine */
          //set needCANInfo = true
        /* after that start any lower level machines that run in this state with StartLowerLevelSM(Event); */
          //None
    }
    else if ( Event.EventType == ES_EXIT )
    {
        /* on exit, give the lower levels a chance to clean up first with RunLowerLevelSM(Event); */
          //None
        /* now do any local exit functionality */
          //None
      
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static ES_Event DuringCrimpingState( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assume no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        /* implement any entry actions required for this state machine */
          //Turn on motors and drive based on desired speed/distance
          //Set inProgressLED
          //Start checkingFailureTimer
        /* after that start any lower level machines that run in this state with StartLowerLevelSM(Event); */
          //None
    }
    else if ( Event.EventType == ES_EXIT )
    {
        /* on exit, give the lower levels a chance to clean up first with RunLowerLevelSM(Event); */
          //None
        /* now do any local exit functionality */
          //Turn off motors
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static ES_Event DuringReportingCrimpQualityState( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assume no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        /* implement any entry actions required for this state machine */
          //Start displayTimer
          //If crimpResultFlag is set (successful crimp)
            //turn on successLED
          //Else
            //turn on failLED
        /* after that start any lower level machines that run in this state with StartLowerLevelSM(Event); */
          //None
    }
    else if ( Event.EventType == ES_EXIT )
    {
        /* on exit, give the lower levels a chance to clean up first with RunLowerLevelSM(Event); */
          //None
        /* now do any local exit functionality */
          //Clear crimpResultFlag
          //Turn off successLED/failLED
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static ES_Event DuringPreparingForNextCrimpState( ES_Event Event)
{
    ES_Event ReturnEvent = Event; // assume no re-mapping or comsumption

    // process ES_ENTRY, ES_ENTRY_HISTORY & ES_EXIT events
    if ( (Event.EventType == ES_ENTRY) ||
         (Event.EventType == ES_ENTRY_HISTORY) )
    {
        /* implement any entry actions required for this state machine */
          //Start checkingFailureTimer
        /* after that start any lower level machines that run in this state with StartLowerLevelSM(Event); */
          //None
    }
    else if ( Event.EventType == ES_EXIT )
    {
        /* on exit, give the lower levels a chance to clean up first with RunLowerLevelSM(Event); */
          //None
        /* now do any local exit functionality */
          //Re-enable motor interrupts
          //Set needCANInfo = true
      
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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
