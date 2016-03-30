
#include <xc.h>
#include <sys/attribs.h>
#include "receive.h"
#include "receive_public.h"
#include "send.h"
#include "system_definitions.h"
#include "debug.h"

// UART interrupt handler
void IntHandlerDrvUsartInstance0(void)
{
    char letter;
    letter = DRV_USART0_ReadByte();
    addQRcv(letter);
    
    /*if (!DRV_USART0_ReceiverBufferIsEmpty())
    {
        PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
        letter = DRV_USART0_ReadByte();
        addQRcv(letter);
    }*/
    
    // Disable transmit interrupt
    PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    
    // Clear pending interrupt 

    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_ERROR);
}
 
 
