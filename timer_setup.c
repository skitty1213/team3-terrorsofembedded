#include "timer_setup.h"
#include "receive_public.h"
#include "send_public.h"
#include "debug.h"
#include "motor_control.h"


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
    PID_Controller(); 
    Turningtime(); 
    Distancetime();
    serverstuff();
    traces(TIMER_CALLBACK_EXIT);
    //slowQueue();  
}



