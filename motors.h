
#ifndef _MOTORS_H
#define _MOTORS_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "motor_control.h"
// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
    
typedef enum
{
MOTORS_STATE_INIT=0,
} MOTORS_STATES;

typedef struct
{
 QueueHandle_t motorQueue;
  // Assembles message
    char message[8];
    // The message type, determines who it applies to
    char type;
    // Sequence number of message
    uint16_t sequence;
      // Tracks current place in received word
    uint8_t currentIndex;
    uint8_t command;
    // Distance traveled
    uint8_t delta_x;
    uint8_t x_goal;
    uint8_t delta_angle;
    uint8_t angle_goal;
    // Determines if rover is asking for a command
    bool ready;
    
    
    
    
   
} MOTORS_DATA;

void MOTORS_Initialize ( void );
void MOTORS_Tasks( void );
void processMotorCommands(void);
void AssembleMotorCommands(char letter);

#endif /* _MOTORS_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

