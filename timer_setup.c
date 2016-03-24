#include "timer_setup.h"
#include "receive_public.h"
#include "send_public.h"
#include "debug.h"
#include "pixy_public.h"

// Initializes timer to specified period in ms
void timer_initialize(int ms)
{
    TimerHandle_t myTimer1;
    myTimer1 = xTimerCreate("test",
                           (ms/portTICK_PERIOD_MS),
                           pdTRUE, 
                           (void *)0,
                           timerCallback);
    if (xTimerStart(myTimer1,10) == pdFAIL)
        error('a');
}

// Increments clock
void timerCallback( TimerHandle_t myTimer )
{
    traces(TIMER_CALLBACK_ENTER);
    updateClock();
    //void sendtoServer(void); // testing 

    traces(TIMER_CALLBACK_EXIT);
}

void timer_initiaize_20(int ms){
        TimerHandle_t myTimer2;
    myTimer2 = xTimerCreate("test2",
                           (ms/portTICK_PERIOD_MS),
                           pdTRUE, 
                           (void *)0,
                           timerCallback_20);
    if (xTimerStart(myTimer2,10) == pdFAIL)
        error('a');
    
} 

void timerCallback_20( TimerHandle_t secondTimer){
    
    
    
}