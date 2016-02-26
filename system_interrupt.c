
#include <xc.h>
#include <sys/attribs.h>
#include "app.h"
#include "app1.h"
#include "system_definitions.h"

void IntHandlerDrvUsartInstance0(void)
{
    unsigned char letter;
 
    if (!DRV_USART0_ReceiverBufferIsEmpty())
    {
        PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
        letter = DRV_USART0_ReadByte();
        // Adds letter to read queue
        addQRcv(letter);
    }

    PLIB_INT_SourceDisable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    /* Clear pending interrupt */

    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_RECEIVE);
    PLIB_INT_SourceFlagClear(INT_ID_0, INT_SOURCE_USART_1_ERROR);
}
 
 
