#ifndef DEBUG_H
#define	DEBUG_H

#include <xc.h> 
#include <stdio.h>
#include <stdlib.h>
#include "system_config.h"
#include "system_definitions.h"
#include "traces.h"
#include "receive.h"
#include "send.h"
#include "receive_public.h"
#include "send_public.h"
#include "pixy.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    void updateLines(char letter);
    void updateLinesNumber (int8_t number);
    void updateLinesNumber2(int8_t number2); 
    //void updateLinesNumber16(int16_t number2); 
    void updateLinesNumber_int(int number3); 
    void traces(char letter);
    void error(char type);
    /////////////////////////////
    //void debugObstacle1_x(int8_t number)

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* DEBUG_H */

