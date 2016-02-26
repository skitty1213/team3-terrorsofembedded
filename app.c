
#include "app.h"
#include "app_public.h"
#include "debug.h"

RCV_DATA rcvData;


void APP_Initialize ( void )
{
    rcvData.rcvQueue = xQueueCreate(80, sizeof(char));
    if (rcvData.rcvQueue == 0)
        error('c');
    rcvData.currentIndex = 0;
    timer_initialize(100);
    DRV_USART0_Initialize();
}

void delay(int ms)
{
    const TickType_t xDelay = 500 / ms;
    vTaskDelay( xDelay );
}

// This is the receive queue
int addQRcv(char letter)
{
    if (xQueueSend (rcvData.rcvQueue, &letter, 0) != pdTRUE)
        error('b');
}


// Assembles instructions as char from uart arrive
void checkReceive(char letter)
{
    int i;
    if (letter == START)
    {
        rcvData.currentIndex = 0;
    }
    rcvData.message[rcvData.currentIndex] = letter;
    // updateLines(temp[rcvData.currentIndex]);
    // A complete message
    if (letter == END && rcvData.currentIndex == 7)
    {
        rcvData.currentIndex = 0;
        for (i=0;i<8;i++)
        {
            addQSnd(rcvData.message[i]);
        }
    }
    // Out of bounds
    else if (rcvData.currentIndex > 7)
    {
        rcvData.currentIndex = 0;
    }
    else
        rcvData.currentIndex++;
}


void APP_Tasks ( void )
{
    char letter;
    while(1)
    {
        xQueueReceive(rcvData.rcvQueue, &letter, portMAX_DELAY);
        //updateLines(letter);
        checkReceive(letter);
    }
}
 

