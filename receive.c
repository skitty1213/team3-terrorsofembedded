
#include "receive.h"
#include "receive_public.h"
#include "debug.h"
#include "command_types.h"
#include "motors_public.h"

// Instance of struct
RCV_DATA rcvData;

// Initialization function

void RECEIVE_Initialize(void) {
    traces(RECEIVE_INIT_ENTER);
    // Create queue, 80 slots, each one byte in size
    rcvData.rcvQueue = xQueueCreate(80, sizeof (char));
    // Verifies queue created
    if (rcvData.rcvQueue == 0)
        error('c');

    // Initialize parameters
    rcvData.currentIndex = 0;
    timer_initialize(100);
    DRV_USART0_Initialize();
    rcvData.clock = 0;
    rcvData.sequence = 0;
    rcvData.x_in = 0; 
    traces(RECEIVE_INIT_EXIT);
}

// Adds character to received queue

int addQRcv(char letter) {
    traces(ADD_Q_RCV_ENTER);
    if (xQueueSendFromISR(rcvData.rcvQueue, &letter, 0) != pdTRUE)
        error('b');
    traces(ADD_Q_RCV_EXIT);
}

// Updates clock

void updateClock() {
    traces(UPDATE_CLOCK);
    rcvData.clock++;
}

void processCommand() {
    rcvData.type_in = rcvData.message[1];
    rcvData.ack_no = rcvData.message[2] << 8 + rcvData.message[3];
    rcvData.x_in = rcvData.message[4];
    rcvData.y_in = rcvData.message[5];
    rcvData.angle_in = rcvData.message[6];

    if (rcvData.type_in == 0x46) {
        move_rover_forward();
    } else if (rcvData.type_in == 0x47) {
        move_rover_backwards();
    } else if (rcvData.type_in == 0x42) {
        turn_left_encoder();
    } else if (rcvData.type_in == 0x43) {
        turn_right_encoder();
    } else if (rcvData.type_in == 0x72) {
        stop_rover();
    } else if (rcvData.type_in == 0x71) {
        turn_left_degrees(rcvData.angle_in);
    } else if (rcvData.type_in == 0x45) {
        turn_right_degrees(rcvData.angle_in);
    } else if (rcvData.type_in == 0x70) {
        move_rover_forward_distance(rcvData.x_in);
        rcvData.delta_x += rcvData.x_in; 
    } else if (rcvData.type_in == 0x75) {
        move_rover_backward_distance(rcvData.x_in);
    }
    
}

void serverstuff()
{
   rcvData.sequence++; 
   uint8_t a = rcvData.sequence>>8; 
   uint8_t b = rcvData.sequence;
   addQSnd('~');
    addQSnd(a);
    addQSnd(b);
    addQSnd(rcvData.delta_x); 
    addQSnd('@');
    addQSnd(0);
    addQSnd(0);
    addQSnd(')');
   PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
}


// Assembles instructions as char from uart arrive

void checkReceive(char letter) {

    // Starts new received word when start character arrives
    if (letter == START) {
        rcvData.currentIndex = 0;
        rcvData.message[rcvData.currentIndex] = letter;
        rcvData.currentIndex++;
        return;
    }
    // More than 8 bytes without an end byte
    if (rcvData.currentIndex > 7) {
        rcvData.currentIndex = 0;
        return;
    }

    // Stores current letter in word
    rcvData.message[rcvData.currentIndex] = letter;

    // Indicates word has been properly assembled
    if (letter == END && rcvData.currentIndex == 7) {
        rcvData.currentIndex = 0;
        // Sends message out
        processCommand();
    }

    rcvData.currentIndex++;

}

// Event loop to read from message received queue

void RECEIVE_Tasks(void) {
    char letter;
    while (1) {
        xQueueReceive(rcvData.rcvQueue, &letter, portMAX_DELAY);
        checkReceive(letter);
    }
}



