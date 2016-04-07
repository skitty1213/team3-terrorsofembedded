/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    pixy.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_Initialize" and "APP_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
Copyright (c) 2013-2014 released Microchip Technology Inc.  All rights reserved.

Microchip licenses to you the right to use, modify, copy and distribute
Software only when embedded on a Microchip microcontroller or digital signal
controller that is integrated into your product or third party product
(pursuant to the sublicense terms in the accompanying license agreement).

You should refer to the license agreement accompanying this Software for
additional information regarding your rights and obligations.

SOFTWARE AND DOCUMENTATION ARE PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF
MERCHANTABILITY, TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE.
IN NO EVENT SHALL MICROCHIP OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER
CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, BREACH OF WARRANTY, OR
OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR
CONSEQUENTIAL DAMAGES, LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF
SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY CLAIMS BY THIRD PARTIES
(INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
 *******************************************************************************/
//DOM-IGNORE-END

#ifndef _PIXY_H
#define _PIXY_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END 

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
	/* Application's state machine's initial state. */
	PIXY_STATE_INIT=0,
            get_byte =1,
            get_zeros = 2, 
            get_nonzeros =3 ,
            start_data = 4,
            
            obstacle1_read = 5,
            obstacle2_read = 6,
            obstacle3_read = 7, 
            
            error_restart = 8,// for repeating purposes when not enough objects are detected
            obstacle1_testing = 9,
            obstacle1_xyExtract = 10,
            error_output = 11, // for knowing something when wrong 
            
            xyCheck = 12,// check what is in the x and y vectors 
            obstacle1_calibrate = 13, // average the values and calibrate to correct lense
            inputQueue = 14, 
            notFirstEnter = 15, 
            
            
            
            /*data_send = 100, 
            which_array = 101,
            read_data_fill,
            sync1,
            sync2,
            array1_fill,
            array2_fill,
            array3_fill,
            full_Arrays,
            breakdown_color,
            breakdown_x,
            breakdown_y,
            send2UART,*/
            end = 30,
	/* TODO: Define states used by the application state machine. */

} PIXY_STATES;


typedef struct
{
    /* The application's current state */
    PIXY_STATES state;

   QueueHandle_t pixyQueue;
   uint16_t w;
   uint16_t lastw; 
   uint16_t sequence_num; 
   int obstacle_num; 
   int arraycount; // number of arrays expected based on obstacle number
   int arraycountTracker;
   int roundcount; // number of rounds 5

        int16_t array1[6]; // obstacle1
        int16_t array2[6]; // obstacle 2
        int16_t array3[6]; // obstacle 3
        int16_t array4[7]; // Lead Rover
        int16_t array5[7]; // Follower Rover
        int16_t array6[7]; // Top Left
        int16_t array7[7];   // Top Right
        int16_t array8[7]; // Bottom Left
        int16_t array9[7]; // Bottom Right
        
        int16_t arrayObstacle1[8];
        int16_t arrayFrontL[8];// front of leader
        int16_t arrayBackL[8];// back of leader
        int16_t arrayFrontF[8];// front of follower
        int16_t arrayBackF[8];// back of follower
        
        unsigned char debug[8];
        float leadAngle;
        float followerAngle; 
        
        float obstacle1_x; 
        float obstacle2_x; 
        float obstacle3_x; 
        float frontLead_x; 
       // float backLead_x;
        float frontFollower_x;
       // float backFollower_x; 
        float topLeft_x;
        float topRight_x; 
        float bottomLeft_x; 
        float bottomRight_x; 
        
        float obstacle1_y; 
        float obstacle2_y; 
        float obstacle3_y; 
        float frontLead_y; 
       // float backLead_y;
        float frontFollower_y;
       // float backFollower_y; 
        float topLeft_y;
        float topRight_y; 
        float bottomLeft_y; 
        float bottomRight_y; 
        
        float average_xRange;
        float average_yRange; 
        
        float calibrated_xRatio; 
        float calibrated_yRatio; 
        
         uint16_t leadAngleFinal; 
    uint16_t followerAngleFinal; 
    uint16_t leadPos_xFinal;
    uint16_t leadPos_yFinal; 
    uint16_t followerPos_xFinal; 
    uint16_t followerPos_yFinal; 
    
    uint16_t obstacle1_xFinal; 
    
    uint16_t obstacle1_yFinal; 
    uint16_t obstacle2_xFinal; 
    uint16_t obstacle2_yFinal; 
    uint16_t obstacle3_xFinal; 
    uint16_t obstacle3_yFinal; 
  
} PIXY_DATA;



typedef struct
{
    uint16_t signature; 
    uint16_t x;
    uint16_t y;
    uint16_t width;
    uint16_t height;
    
}Block;


void PIXY_Initialize ( void );

void PIXY_Tasks( void );




#endif /* _PIXY_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

