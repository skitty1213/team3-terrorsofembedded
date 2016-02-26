
#include "app1.h"
#include "app1_public.h"
#include "debug.h"


SEND_DATA sendData;




void APP1_Initialize ( void )
{
    sendData.sendQueue = xQueueCreate(80, sizeof(char));
    if (sendData.sendQueue == 0)
        error('c');
    DRV_USART0_Initialize();
}

int addQSnd(char letter)
{
    if (xQueueSend (sendData.sendQueue, &letter, 0) != pdTRUE)
        error('b');
}

void APP1_Tasks ( void )
{
    unsigned char letter;
    while(1)
    {
        xQueueReceive(sendData.sendQueue, &letter, portMAX_DELAY);
        //updateLines(letter);
        DRV_USART0_WriteByte(letter);
        PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    }
}
 

/*******************************************************************************
 End of File
 */
