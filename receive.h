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
    // Assembles incoming message
    char message[8];
    
    bool firstPass;
    
    int xGoal;
    int yGoal;
    char board[6][6];
    uint8_t square_x;
    uint8_t square_y;
    uint8_t way_x[3];
    uint8_t way_y[3];
    
    // Outgoing Message details
    char type_out;
    uint16_t sequence_out;
    int xPos;
    int yPos;
    int orient;
    
    // Distance traveled
    uint8_t delta_x;
    //uint8_t x_goal;
    uint8_t delta_angle;
    //uint8_t angle_goal;
    
    // Info from incoming message
    uint16_t ack_no;
    char type_in;
    uint8_t x_in;
    uint8_t y_in;
    uint8_t angle_in;
        
    // Determines is ready for a command
    bool loc_ready;
    bool mov_ready;
    bool obs_ready;
    bool config_ready;
    bool started;
    
    bool turning;
    
    int x_wp[60];
	int y_wp[60];
	uint8_t index;
	uint8_t build_index;
    uint8_t token_index;
    uint8_t retrieved_index;
    bool token_retrieved[3];
    bool up;
    bool done;
    
    int timer_threshold;
    
    uint8_t numObs;
    uint8_t obs_x[3];
    uint8_t obs_y[3];
    uint8_t obs_square_x[3];
	uint8_t obs_square_y[3];
    uint8_t numToks;
    uint8_t tok_x[3];
    uint8_t tok_y[3];
    // A clock variable to time event
    int clock;
    
    int tempx;
	int tempy;
    
} RCV_DATA;

// Initialization of timer, queues and values
void RECEIVE_Initialize ( void );

// Event loop, reads from uart queue
void RECEIVE_Tasks( void );

// Assembles incoming messages to ensure complete
void checkReceive(char letter);

// Processes the command to be sent
void processCommand();

// Moves towards next goal
void goToGoal();

// Sends turn command
void turn(uint8_t degree);

// Sends forward command
void goForward(uint8_t x);

// Sends lead rover stop command
void stop();

// Sends message saying unique token found
void token();

// Determines waypoints
void setPoints();

// Checks if all obstacle positions are known
bool checkObs();

// Corrects course if rover deviating
void maintainLine();

// Adds waypoint to list
void addWaypoint(int x, int y);

// Returns if square is obstacle
bool isObs(int x, int y);



#endif /* _RECEIVE_H */

//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

/*******************************************************************************
 End of File
 */

