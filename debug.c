#include "debug.h"

void error(char type)
{
    //vTaskSuspendAll ();    //suspend tasks
    switch (type)
    {
        // Timer failed to start
        case 'a':   updateLines('[');
                    break;
        // Receive Queue full
        case 'b':   updateLines('{');
                    break;
        // Queue not created
        case 'c':   updateLines('+');
                    break;
        // Send Queue full
        case 'd':   updateLines('}');
                    break;
              
    }
}

void updateLines(char letter)
{
    PLIB_PORTS_DirectionOutputSet (PORTS_ID_0, PORT_CHANNEL_E, 0xFF);
    PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_E, letter);   
}

void updateLinesNumber (int8_t number)
{
    PLIB_PORTS_DirectionOutputSet (PORTS_ID_0, PORT_CHANNEL_E, 0xFF);
    PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_E, number);   
}

void updateLinesNumber2(int8_t number2){
    PLIB_PORTS_DirectionOutputSet (PORTS_ID_0, PORT_CHANNEL_B, 0xFF);
    PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_B, number2);  
    
}

void updateLinesNumber_int(int number3){
    PLIB_PORTS_DirectionOutputSet (PORTS_ID_0, PORT_CHANNEL_B, 0xFF);
    PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_B, number3); 
}

void traces(char letter)
{
    PLIB_PORTS_DirectionOutputSet (PORTS_ID_0, PORT_CHANNEL_A, 0xFF);
    PLIB_PORTS_Write(PORTS_ID_0, PORT_CHANNEL_A, letter);   
}

