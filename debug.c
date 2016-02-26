#include "debug.h"

void error(char type)
{
    vTaskSuspendAll ();    //suspend tasks
    switch (type)
    {
        // Timer failed to start
        case 'a':   updateLines('[');
                    break;
        // Queue full
        case 'b':   updateLines('{');
                    break;
        // Queue not created
        case 'c':   updateLines('+');
                    break;
    }
}

void updateLines(char letter)
{
    PLIB_PORTS_DirectionOutputSet (PORTS_ID_0, PORT_CHANNEL_E, 0xFF);
    PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_E, letter);
}