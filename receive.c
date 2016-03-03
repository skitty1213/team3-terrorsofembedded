
#include "receive.h"
#include "receive_public.h"
#include "debug.h"
#include "command_types.h"

// Instance of struct
RCV_DATA rcvData;

// Initialization function
void RECEIVE_Initialize ( void )
{
    traces(RECEIVE_INIT_ENTER);
    // Create queue, 80 slots, each one byte in size
    rcvData.rcvQueue = xQueueCreate(80, sizeof(char));
    // Verifies queue created
    if (rcvData.rcvQueue == 0)
        error('c');
    
    // Initialize parameters
    rcvData.currentIndex = 0;
    timer_initialize(100);
    DRV_USART0_Initialize();
    rcvData.clock = 0;
    rcvData.sequence = 100;
    traces(RECEIVE_INIT_EXIT);
}

// Adds character to received queue
int addQRcv(char letter)
{
    traces(ADD_Q_RCV_ENTER);
    if (xQueueSendFromISR (rcvData.rcvQueue, &letter, 0) != pdTRUE)
        error('b');
    traces(ADD_Q_RCV_EXIT);
}

// Updates clock
void updateClock()
{
    traces(UPDATE_CLOCK);
    rcvData.clock++;
}

void processCommand()
{
    char outType = 255;
    char x_out = 0;
    char y_out = 0;
    char o_out = 0;
    
    // Tracks sequence number of incoming command in case it requires an ack
    rcvData.ack_no = rcvData.message[2]<<8 + rcvData.message[3];
    
    // Determines Message Type
    rcvData.type = rcvData.message[1];
    
    if (rcvData.type == LEAD_POSITION)
    {
        rcvData.xPos = rcvData.message[4];
        rcvData.yPos = rcvData.message[5];
        rcvData.orient = rcvData.message[6];
        rcvData.ready = false;
        return;
    }
    else if (rcvData.type == DISTANCE_MOVED)
    {
        rcvData.delta_x = rcvData.message[4];
        rcvData.ready = false;
        return;
    }
    else if (rcvData.type == AWAITING_INSTRUCTION)
    {
        rcvData.ready = true;
        return;
    }
    else if (rcvData.type == DEBUG_FORWARD)
    {
        outType = MOVE_FORWARD;
        x_out = rcvData.message[4];
    }
    else if (rcvData.type == DEBUG_BACK)
    {
        outType = MOVE_BACK;
        x_out = rcvData.message[4];
    }
    else if (rcvData.type == DEBUG_STOP)
    {
        rcvData.ready = true;
        outType = STOP_LEAD;
    }
    else if (rcvData.type == DEBUG_TURN)
    {
        outType = TURN;
        o_out = rcvData.message[6];
    }
    
    if (rcvData.ready)
    {
        rcvData.sequence++;
        addQSnd(START);
        addQSnd(outType);
        addQSnd(rcvData.sequence/256);
        addQSnd(rcvData.sequence%256);
        addQSnd(x_out);
        addQSnd(0);
        addQSnd(o_out);
        addQSnd(END);
    }
    rcvData.ready = false;
}

// Assembles instructions as char from uart arrive
void checkReceive(char letter)
{
    traces(CHECK_RECEIVE_ENTER);
    
    // Starts new received word when start character arrives
    if (letter == START)
    {
        traces(CHECK_RECEIVE_START_CHAR);
        rcvData.currentIndex = 0;
    }
    // Stores current letter in word
    rcvData.message[rcvData.currentIndex] = letter;
    
    // Indicates word has been properly assembled
    if (letter == END && rcvData.currentIndex == 7)
    {
        traces(CHECK_RECEIVE_WORD_DONE);
        rcvData.currentIndex = 0;
        // Sends message out
        if ((rcvData.message[2]<<8 + rcvData.message[3]) != rcvData.sequence)
        {
            // Send message out
            processCommand();
        }
    }
    
    // Out of bounds without end character
    else if (rcvData.currentIndex > 7)
    {
        traces(CHECK_RECEIVE_WORD_EXCEED);
        rcvData.currentIndex = 0;
    }
    
    // Increments index
    else
    {
        rcvData.currentIndex++;
    }
    traces(CHECK_RECEIVE_WORD_EXIT);
}

// Event loop to read from message received queue
void RECEIVE_Tasks ( void )
{
    traces(RECEIVE_TASKS_ENTER);
    char letter;
    while(1)
    {
        traces(RECEIVE_TASKS_BEFORE_QUEUE);
        xQueueReceive(rcvData.rcvQueue, &letter, portMAX_DELAY);
        traces(RECEIVE_TASKS_AFTER_QUEUE);
        checkReceive(letter);
    }
}
 

