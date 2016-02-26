#ifndef DEBUG_H
#define	DEBUG_H

#include <xc.h> 
#include "system_config.h"
#include "system_definitions.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    void updateLines(char letter);
    void error(char type);

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* DEBUG_H */

