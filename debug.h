#ifndef DEBUG_H
#define	DEBUG_H

#include <xc.h> 
#include "system_config.h"
#include "system_definitions.h"
#include "traces.h"

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

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* DEBUG_H */

