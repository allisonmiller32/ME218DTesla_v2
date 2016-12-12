/****************************************************************************
  Header file for CAN Module
 ****************************************************************************/

#ifndef CAN_H
#define CAN_H

//Common Headers
#include <stdint.h>
#include <stdbool.h>

/*---------------------- Public Function Prototypes -----------------------*/
void InitCAN(void);
void CAN_SEND(uint8_t MsgID, uint32_t timestamp, uint32_t distance, uint32_t force, uint32_t velocity);

#endif /* CAN_H */
