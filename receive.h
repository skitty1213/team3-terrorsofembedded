// UART RECEIVE TASK

#ifndef _RECEIVE_H
#define _RECEIVE_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "queue.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END    
    
#define START '~'
#define END ')'


typedef struct
{
    // Queue to receive from uart isr
    QueueHandle_t rcvQueue;
    // Tracks current place in received word
    uint8_t currentIndex;
    // Assembles message
    char message[8];
    
    // The message type, determines who it applies to
    char type;
    // Sequence number of message
    uint16_t sequence;
    // Position information
    uint8_t xPos;
    uint8_t yPos;
    uint8_t orient;
    
    // Distance traveled
    uint8_t delta_x;
    uint8_t x_goal;
    uint8_t delta_angle;
    uint8_t angle_goal;
    
    // Sequence number of incoming message
    uint16_t ack_no;
        
    // Determines if rover is asking for a command
    bool ready;
    
    // A clock variable to time event
    int clock;
    
    
     ///more
// Info from incoming message
   // uint16_t ack_no;
    char type_in;
    uint8_t x_in;
    uint8_t y_in;
    uint8_t angle_in;
    
} RCV_DATA;

// Initialization of timer, queues and values
void RECEIVE_Initialize ( void );

// Event loop, reads from uart queue
void RECEIVE_Tasks( void );

// Assembles incoming messages to ensure complete
void checkReceive(char letter);

// Processes the command to be sent
void processCommand();
void serverstuff(); 



#endif /* _RECEIVE_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

