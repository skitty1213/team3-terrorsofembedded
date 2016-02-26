#include "timer_setup.h"
#include "app_public.h"

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

int i=0;
char text[8] = "(Team#3;";
void timerCallback( TimerHandle_t myTimer )
{
    // Sends to queue using public function
    addQRcv(text[i%8]);
    //sendValueQ1('-');
    //assembleRcv(text[i%8]);
    i++;
}

