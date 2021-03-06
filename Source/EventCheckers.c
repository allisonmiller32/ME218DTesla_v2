/****************************************************************************
 Module
   EventCheckers.c

 Revision
   1.0.1 

 Description
   This is the sample for writing event checkers along with the event 
   checkers used in the basic framework test harness.

 Notes
   Note the use of static variables in sample event checker to detect
   ONLY transitions.
   
 History
 When           Who     What/Why
 -------------- ---     --------
 08/06/13 13:36 jec     initial version
****************************************************************************/

// this will pull in the symbolic definitions for events, which we will want
// to post in response to detecting events
#include "ES_Configure.h"
// this will get us the structure definition for events, which we will need
// in order to post events in response to detecting events
#include "ES_Events.h"
// if you want to use distribution lists then you need those function 
// definitions too.
#include "ES_PostList.h"
// This include will pull in all of the headers from the service modules
// providing the prototypes for all of the post functions
#include "ES_ServiceHeaders.h"
// this test harness for the framework references the serial routines that
// are defined in ES_Port.c
#include "ES_Port.h"
#include "ES_Timers.h"

// include our own prototypes to insure consistency between header & 
// actual functionsdefinition
#include "EventCheckers.h"

#include "PWM0_Tiva.h"
#include "Motors.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_gpio.h"
#include "inc/hw_can.h"

// This is the event checking function sample. It is not intended to be 
// included in the module. It is only here as a sample to guide you in writing
// your own event checkers
#if 0
/****************************************************************************
 Function
   Check4Lock
 Parameters
   None
 Returns
   bool: true if a new event was detected
 Description
   Sample event checker grabbed from the simple lock state machine example
 Notes
   will not compile, sample only
 Author
   J. Edward Carryer, 08/06/13, 13:48
****************************************************************************/
bool Check4Lock(void)
{
  static uint8_t LastPinState = 0;
  uint8_t CurrentPinState;
  bool ReturnVal = false;
  
  CurrentPinState =  LOCK_PIN;
  // check for pin high AND different from last time
  // do the check for difference first so that you don't bother with a test
  // of a port/variable that is not going to matter, since it hasn't changed
  if ( (CurrentPinState != LastPinState) &&
       (CurrentPinState == LOCK_PIN_HI) )
  {                     // event detected, so post detected event
    ES_Event ThisEvent;
    ThisEvent.EventType = ES_LOCK;
    ThisEvent.EventParam = 1;
    // this could be any of the service post function, ES_PostListx or 
    // ES_PostAll functions
    ES_PostList01(ThisEvent); 
    ReturnVal = true;
  }
  LastPinState = CurrentPinState; // update the state for next time

  return ReturnVal;
}
#endif

/****************************************************************************
 Function
   Check4Keystroke
 Parameters
   None
 Returns
   bool: true if a new key was detected & posted
 Description
   checks to see if a new key from the keyboard is detected and, if so, 
   retrieves the key and posts an ES_NewKey event to TestHarnessService0
 Notes
   The functions that actually check the serial hardware for characters
   and retrieve them are assumed to be in ES_Port.c
   Since we always retrieve the keystroke when we detect it, thus clearing the
   hardware flag that indicates that a new key is ready this event checker 
   will only generate events on the arrival of new characters, even though we
   do not internally keep track of the last keystroke that we retrieved.
 Author
   J. Edward Carryer, 08/06/13, 13:48
****************************************************************************/
bool Check4Keystroke(void)
{
	static uint32_t LastEncoderCount;
	static uint32_t Frequency = 200;
  if ( IsNewKeyReady() ) // new key waiting?
  {
//    ES_Event ThisEvent;
//    ThisEvent.EventType = ES_NEW_KEY;
//    ThisEvent.EventParam = GetNewKey();
		char key = GetNewKey();
		ES_Event KeyboardEvent;
		/* Events used in MasterHSM */
		if(key == 'c'){
			KeyboardEvent.EventType = ES_CALIBRATING_MODE_BUTTON;
			PostMasterSM(KeyboardEvent);
		} else if(key == 't'){
			KeyboardEvent.EventType = ES_TESTING_MODE_BUTTON;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'r'){
			KeyboardEvent.EventType = ES_CRIMPING_MODE_BUTTON;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'e'){
			KeyboardEvent.EventType = ES_EXIT_BUTTON;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'z'){
			KeyboardEvent.EventType = ES_MOTORS_ZEROED;
			PostMasterSM(KeyboardEvent);
		} else if(key == 's'){
			KeyboardEvent.EventType = ES_SPINNING;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'i'){
			KeyboardEvent.EventType = ES_TIMEOUT;
			KeyboardEvent.EventParam = checkingFailureTimer;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'j'){
			KeyboardEvent.EventType = ES_TIMEOUT;
			KeyboardEvent.EventParam = displayTimer;
			PostMasterSM(KeyboardEvent);
		} 
		
		/* Events used in TestingHSM */
			else if(key == 'a'){
			KeyboardEvent.EventType = ES_LIMIT_SWITCH_HIT;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'b'){
			KeyboardEvent.EventType = ES_MAX_TRAVEL_LIMIT_HIT;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'd'){
			KeyboardEvent.EventType = ES_MIN_TRAVEL_LIMIT_HIT;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'f'){
			KeyboardEvent.EventType = ES_MOTOR_BUTTON_UP;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'g'){
			KeyboardEvent.EventType = ES_MOTOR_FORWARD_DOWN;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'h'){
			KeyboardEvent.EventType = ES_MOTOR_BACKWARD_DOWN;
			PostMasterSM(KeyboardEvent);
		} 
		
		/* Events used in CalibratingHSM */
		else if(key == 'k'){
			KeyboardEvent.EventType = ES_CALIBRATION_DISTANCE;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'l'){
			KeyboardEvent.EventType = ES_CALIBRATION_FORCE;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'm'){
			KeyboardEvent.EventType = ES_BOTH_CALIBRATION_DISTANCE;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'n'){
			KeyboardEvent.EventType = ES_BOTH_CALIBRATION_FORCE;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'o'){
			KeyboardEvent.EventType = ES_CALIBRATION_FAILURE;
			PostMasterSM(KeyboardEvent);
		} 

		/* Events used in CrimpingHSM */
		else if(key == 'p'){
			KeyboardEvent.EventType = ES_CRIMP_BUTTON_DOWN;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'q'){
			KeyboardEvent.EventType = ES_CRIMP_BUTTON_UP;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'u'){
			KeyboardEvent.EventType = ES_CRIMP_DISTANCE;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'v'){
			KeyboardEvent.EventType = ES_CRIMP_FORCE;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'w'){
			KeyboardEvent.EventType = ES_ENCODER_HOMED;
			PostMasterSM(KeyboardEvent);
		} else if(key == 'x'){
			KeyboardEvent.EventType = ES_READY_FOR_NEW_CRIMP;
			PostMasterSM(KeyboardEvent);
		} 

		
		// Ernesto
		else if(key == '1'){
			PWM0_SetDuty(50);
			printf("Motor Duty: 50 percent\n\r");
		}
		else if(key == '2'){
			PWM0_SetDuty(0);
			printf("Motor Duty: 0 percent\n\r");
		}
		else if(key == '3'){
			Frequency +=10;
			PWM0_SetFreq(Frequency);
			printf("Motor Frequency = %d\n\r",Frequency);
		}
		else if(key == '4'){
			Frequency -=10;
			PWM0_SetFreq(Frequency);
			printf("Motor Frequency = %d\n\r",Frequency);
		}
		else if(key == '5'){
			Motor1Direction(MOTOR1_FORWARD);
			printf("Motor0 Direction: Forward\n\r");
		}
		else if(key == '6'){
			Motor1Direction(MOTOR1_REVERSE);
			printf("Motor0 Direction: Reverse\n\r");
		}
		else if(key == '7'){
			Motor1Enable(true);
			printf("Motor1 Enabled\n\r");
		}
		else if(key == '8'){
			Motor1Enable(false);
			printf("Motor1 Disabled\n\r");
		}
		else if(key == '9'){
			PWM1_SetDuty(50);
			printf("Motor1 Duty: 50 percent\n\r");
		}		

		
//    PostMapKeys( ThisEvent );
    return true;
  }
  return false;
}
