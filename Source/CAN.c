/****************************************************************************
 Module
   CANService.c

 Revision
   1.0.1

 Description
   This is a CAN file for implementing a simple service under the 
   Gen2 Events and Services Framework.

 Notes

 History
 When           Who     What/Why
 -------------- ---     --------
 01/16/12 09:58 jec      began conversion from CANFSM.c
****************************************************************************/
/*----------------------------- Include Files -----------------------------*/
/* include header files for this state machine as well as any machines at the
   next lower level in the hierarchy that are sub-machines to this machine
*/
#include "ES_Configure.h"
#include "ES_Framework.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>


#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_nvic.h"
#include "inc/hw_sysctl.h"
#include "inc/hw_gpio.h"
#include "inc/hw_timer.h"

#include "driverlib/can.h"
#include "driverlib/interrupt.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"
#include "driverlib/uart.h"
#include "driverlib/pin_map.h"


#include "utils/uartstdio.h"

#include "BITDEFS.h"

#include "CAN.h"

/*----------------------------- Module Defines ----------------------------*/
#define ALL_BITS (0xFF<<2)
#define BITS0TO7HI 0x00FF
#define BITS8TO15HI 0xFF00


#define CAN_PORT GPIO_PORTE_BASE

#define CAN_RX_PIN GPIO_PIN_4
#define CAN_RX_PIN_LO ~GPIO_PIN_4
#define CAN_RX_PIN_HI GPIO_PIN_4

#define CAN_TX_PIN GPIO_PIN_5
#define CAN_TX_PIN_LO ~GPIO_PIN_5
#define CAN_TX_PIN_HI GPIO_PIN_5

#define CAN_BITRATE 500000
//#define CAN_TEST

#define CAN_MESSAGE_LENGTH 8

/*---------------------------- Module Functions ---------------------------*/
/* prototypes for private functions for this service.They should be functions
   relevant to the behavior of this service
*/
void InitCAN(void);
void CAN_SEND(uint8_t MsgID, uint32_t timestamp, uint32_t distance, uint32_t force, uint32_t velocity);

//CAN ISR
void CANIntHandler(void);

/*---------------------------- Module Variables ---------------------------*/
// with the introduction of Gen2, we need a module level Priority variable
//CAN Flags
volatile bool rxFlag = 0; // msg recieved flag
volatile bool errFlag = 0; // error flag

//Create CAN message
tCANMsgObject msg; // the CAN message object
uint8_t msgData[CAN_MESSAGE_LENGTH]; // the message data is four bytes long which we can allocate as an int32
uint8_t *msgDataPtr = (uint8_t *) &msgData; // make a pointer to msgData so we can access individual bytes


void InitCAN(void)
{
	uint32_t ui32Msg;

		HWREG(SYSCTL_RCGCGPIO) |= BIT4HI;
    while ((HWREG(SYSCTL_RCGCGPIO) & BIT4HI) != BIT4HI)
    ;
    HWREG(GPIO_PORTE_BASE+GPIO_O_DEN) |= (BIT4HI|BIT5HI);
    HWREG(GPIO_PORTE_BASE+GPIO_O_DIR) &= (BIT4LO);
		HWREG(GPIO_PORTE_BASE+GPIO_O_DIR) |= (BIT5HI);
    
    //Select the Alternate Functions for PE4 and PE5
		HWREG(GPIO_PORTE_BASE+GPIO_O_AFSEL) |= BIT4HI|BIT5HI;
		HWREG(GPIO_PORTE_BASE+GPIO_O_PCTL) = (HWREG(GPIO_PORTE_BASE+GPIO_O_PCTL) & 0xff00ffff) + (8<<(5*4)) + (8<<(4*4));
		
		// Initialize the CAN peripheral
		HWREG(SYSCTL_RCGC0) |= SYSCTL_RCGC0_CAN0;
	
    //Set Controller in Intialization mode and clear out all of the messages objects
    HWREG(CAN0_BASE + CAN_O_CTL) = CAN_CTL_INIT;

    // Wait for busy bit to clear  
    while(HWREG(CAN0_BASE + CAN_O_IF1CRQ) & CAN_IF1CRQ_BUSY)
    {
    }
    // Clear the message value bit in the arbitration register.  This indicates
    // the message is not valid and is a "safe" condition to leave the message
    // object.  The same arb reg is used to program all the message objects.
    HWREG(CAN0_BASE + CAN_O_IF1CMSK) = (CAN_IF1CMSK_WRNRD | CAN_IF1CMSK_ARB |
                                       CAN_IF1CMSK_CONTROL);
    HWREG(CAN0_BASE + CAN_O_IF1ARB2) = 0;
    HWREG(CAN0_BASE + CAN_O_IF1MCTL) = 0;

    // Loop through to program all 32 message objects
    for(ui32Msg = 1; ui32Msg <= 32; ui32Msg++)
    {
        // Wait for busy bit to clear
        while(HWREG(CAN0_BASE + CAN_O_IF1CRQ) & CAN_IF1CRQ_BUSY)
        {
        }

        // Initiate programming the message object
        HWREG(CAN0_BASE + CAN_O_IF1CRQ) = ui32Msg;
    }

    // Make sure that the interrupt and new data flags are updated for the
    // message objects.
    HWREG(CAN0_BASE + CAN_O_IF1CMSK) = (CAN_IF1CMSK_NEWDAT |
                                       CAN_IF1CMSK_CLRINTPND);

    // Loop through to program all 32 message objects
    for(ui32Msg = 1; ui32Msg <= 32; ui32Msg++)
    {
        // Wait for busy bit to clear.
		while(HWREG(CAN0_BASE + CAN_O_IF1CRQ) & CAN_IF1CRQ_BUSY)
		{
		}
		// Initiate programming the message object
		HWREG(CAN0_BASE + CAN_O_IF1CRQ) = ui32Msg;
    }
    // Acknowledge any pending status interrupts.
    HWREG(CAN0_BASE + CAN_O_STS);
	
    //Set the CAN Bitrate
		CANBitRateSet(CAN0_BASE, SysCtlClockGet(), CAN_BITRATE);
	
		//Enable Interrupts
		HWREG(CAN0_BASE + CAN_O_CTL) |= CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS;
	
    //Interrupt Number 39 -> ENVIC1 #7
		HWREG(NVIC_EN1) |= BIT7HI;
	
    //Enable Interrupts globally
    __enable_irq();
	
    //Set the CAN Controller in Normal mode
	HWREG(CAN0_BASE + CAN_O_CTL) &= ~CAN_CTL_INIT;
}
void CAN_SEND(uint8_t MsgID, uint32_t timestamp, uint32_t distance, uint32_t force, uint32_t velocity)
{
    
		msg.ui32MsgID = MsgID;
		msg.ui32MsgIDMask = 0;
		msg.ui32Flags = MSG_OBJ_TX_INT_ENABLE;
		msg.ui32MsgLen = sizeof(msgData);
		printf("Size of %d\n\r",sizeof(msgData));
		msg.pui8MsgData = msgDataPtr;
		
    msgDataPtr[0] = timestamp&BITS0TO7HI;
    msgDataPtr[1] = timestamp&BITS8TO15HI>>8;
    msgDataPtr[2] = distance&BITS0TO7HI;
    msgDataPtr[3] = distance&BITS8TO15HI>>8;
    msgDataPtr[4] = force&BITS0TO7HI;
    msgDataPtr[5] = force&BITS8TO15HI>>8;
    msgDataPtr[6] = velocity&BITS0TO7HI;
    msgDataPtr[7] = velocity&BITS8TO15HI>>8;
    
    CANMessageSet(CAN0_BASE, 1, &msg, MSG_OBJ_TYPE_TX);
}

void CANIntHandler(void) {
	uint32_t status = CANIntStatus(CAN0_BASE, CAN_INT_STS_CAUSE); // read interrupt status

	if(status == CAN_INT_INTID_STATUS) { // controller status interrupt
		status = CANStatusGet(CAN0_BASE, CAN_STS_CONTROL); // read back error bits, do something with them?
		errFlag = 1;
		#ifdef CAN_TEST
		status&=(BIT2HI|BIT1HI|BIT0HI);
		if(status==0x0)
		{
			printf("No Error\n\r");
		}
		else if(status==0x1)
		{
			printf("Stuff Error\n\r");
		}
		else if(status==0x2)
		{
			printf("Format Error\n\r");
		}
		else if(status==0x3)
		{
			printf("Ack Error\n\r");
		}
		else if(status==0x4)
		{
			printf("Bit1 Error\n\r");
		}
		else if(status==0x5)
		{
			printf("Bit0 Error\n\r");
		}
		else if(status==0x6)
		{
			printf("CRC Error\n\r");
		}
		else if(status==0x7)
		{
			printf("No Event\n\r");
		}
		#endif
	} 
	else if(status == 1) { // message object 1
		CANIntClear(CAN0_BASE, 1); // clear interrupt
		errFlag = 0; // clear any error flags
        
	} else { // should never happen
		printf("Unexpected CAN bus interrupt\n");
	}
}
