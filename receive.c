
#include "receive.h"
#include "receive_public.h"
#include "debug.h"

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
    rcvData.match = 1;
    rcvData.clock = 0;
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

char myMess[8] = "~gotit!)";
// Assembles instructions as char from uart arrive
void checkReceive(char letter)
{
    traces(CHECK_RECEIVE_ENTER);
    int i;
    // Starts new received word when start character arrives
    if (letter == START)
    {
        traces(CHECK_RECEIVE_START_CHAR);
        rcvData.currentIndex = 0;
    }
    
    // Stores current letter in word
    rcvData.message[rcvData.currentIndex] = letter;
    
    // Determines if the incoming message matches the previous outgoing command
    if (letter != myMess[rcvData.currentIndex])
    {
        rcvData.match = 0;
    }
    
    // Indicates word has been properly assembled
    if (letter == END && rcvData.currentIndex == 7)
    {
        traces(CHECK_RECEIVE_WORD_DONE);
        rcvData.currentIndex = 0;
        // Sends message out
        if (!rcvData.match)
        {
            for (i=0;i<8;i++)
            {
                //addQSnd(myMess[i]); !!!!!!!!!!!!!!!!!!!!!!!!! changed to stop recieving 
            }
        }
        rcvData.match = 1;
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
 

