#include "send.h"
#include "send_public.h"
#include "debug.h"

// Instance of struct
SEND_DATA sendData;

// Initialization function
void SEND_Initialize ( void )
{
    traces(SEND_INIT_ENTER);
    // Create queue, 80 slots, each one byte in size
    sendData.sendQueue = xQueueCreate(80, sizeof(char));
    if (sendData.sendQueue == 0)
        error('c');
    
    // Initialize parameters
    DRV_USART0_Initialize();
    sendData.currentOutIndex = 0;
    traces(SEND_INIT_EXIT);
}

// Adds character to send queue
int addQSnd(char letter)
{
    traces(ADD_Q_SND_ENTER);
    if (xQueueSend (sendData.sendQueue, &letter, 0) != pdTRUE)
        error('d');
    traces(ADD_Q_SND_EXIT);
}

// Assembles a full message before sending out
void assembleSend(char letter)
{
    traces(ASSEMBLE_SEND_ENTER);
    int i;
    sendData.messageOut[sendData.currentOutIndex] = letter;
    sendData.currentOutIndex++;
    if (sendData.currentOutIndex > 7)
    {
        for (i=0;i<8;i++)
        {
            DRV_USART0_WriteByte(sendData.messageOut[i]);
        }
        sendData.currentOutIndex = 0;
        PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    }
    traces(ASSEMBLE_SEND_EXIT);
}

// Event loop reads from send queue and sends messages out
void SEND_Tasks ( void )
{
    traces(SEND_TASKS_ENTER);
    unsigned char letter;
    while(1)
    {
        traces(SEND_TASKS_BEFORE_QUEUE);
        xQueueReceive(sendData.sendQueue, &letter, portMAX_DELAY);
        traces(SEND_TASKS_AFTER_QUEUE);
        assembleSend(letter);
    }
}
 

/*******************************************************************************
 End of File
 */
