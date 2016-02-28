#ifndef _SEND_H
#define _SEND_H

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

typedef struct
{
    // Queue to send to uart isr
    QueueHandle_t sendQueue;
    // Tracks current place in word to be sent
    uint8_t currentOutIndex;
    // Message to be sent
    char messageOut[8];
} SEND_DATA;

// Initialization of timer, queues and values
void SEND_Initialize ( void );

// Event loop, reads from send queue
void SEND_Tasks( void );

// Assembles outbound message, then sends characters
void assembleSend(char letter);


#endif /* _SEND_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

