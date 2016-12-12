/****************************************************************************
 
  Header file for CAN service 
  based on the Gen 2 Events and Services Framework

 ****************************************************************************/

#ifndef ServCAN_H
#define ServCAN_H

#include "ES_Types.h"

// Public Function Prototypes

bool InitCANService ( uint8_t Priority );
bool PostCANService( ES_Event ThisEvent );
ES_Event RunCANService( ES_Event ThisEvent );


#endif /* ServCAN_H */

