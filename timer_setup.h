#ifndef TIMER_SETUP_H
#define	TIMER_SETUP_H

#include <xc.h> // include processor files - each processor file is guarded.  
#include "system_config.h"
#include "system_definitions.h"
#include "timers.h"
#include "queue.h"

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    void timer_initialize(int ms);
    void timerCallback( TimerHandle_t myTimer );
    
    void timer_initiaize_20(int ms); 
    void timerCallback_20( TimerHandle_t secondTimer); 

#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* TIMER_SETUP_H */

