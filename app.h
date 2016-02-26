// UART RECEIVE TASK

#ifndef _APP_H
#define _APP_H

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
    
#define START '('
#define END ';'


typedef struct
{
    QueueHandle_t rcvQueue;
    uint8_t currentIndex;
    char type;
    uint16_t sequence;
    uint8_t xPos;
    uint8_t yPos;
    uint8_t orient;
    char message[8];
    
} RCV_DATA;


void APP_Initialize ( void );

void APP_Tasks( void );

void checkReceive(char letter);


#endif /* _APP_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

