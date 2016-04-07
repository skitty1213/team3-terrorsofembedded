#include "motors.h"
#include "motor_control.h"
#include "command_types.h"


// Instance of struct
MOTORS_DATA motorsData;

void MOTORS_Initialize ( void )
{
    motorsData.motorQueue = xQueueCreate(80, sizeof(char));
    // Verifies queue created
    if (motorsData.motorQueue == 0)
        error('c');
    
    
    PLIB_TMR_Start(TMR_ID_2);
    PLIB_OC_Enable(0);
    PLIB_OC_Enable(1);
    motorsData.sequence = 50;
    //timer_initialize(100000);       
}
// Adds character to received queue
int addQmotors(char letter)
{
    if (xQueueSendFromISR (motorsData.motorQueue, &letter, 0) != pdTRUE)
        error('b');
}


/*
void slowQueue(void)
{
  addQSnd('X');
  addQSnd('O');
  addQSnd('S');
  addQSnd('K');
  addQSnd('I');
  addQSnd('T');
  addQSnd('T');
  addQSnd('Y');
  
  PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);   
} 

void processMotorCommands(void)
{
   char x_out = 0;
  if (motorsData.type == MOVE_FORWARD)
    {
        move_rover_forward();
        x_out = motorsData.message[4];
        motorsData.ready = false;
    }
  else if (motorsData.type == MOVE_BACK)
    {
        move_rover_backwards();
        x_out = motorsData.message[4];
        motorsData.ready = false;
    }
  else if (motorsData.type == TOKEN_FOUND)
    {
        x_out = motorsData.message[4];
    }
  else if (motorsData.type == DISTANCE_MOVED)
    {
        motorsData.delta_x = motorsData.message[4];
    }  
}
void AssembleMotorCommands(char letter)
{
    // Starts new received word when start character arrives
    if (letter == START)
    {
        motorsData.currentIndex = 0;
        // Stores current letter in word
    motorsData.message[motorsData.currentIndex] = letter;
     motorsData.currentIndex++;
    
    }
    
    
    // Indicates word has been properly assembled
    if (letter == END && motorsData.currentIndex == 7)
    {
        if (motorsData.message[1] == MOVE_FORWARD)
        {
            motorsData.type = motorsData.message[1]; 
            motorsData.sequence = motorsData.message[2]*256 + motorsData.message[3]; 
            motorsData.delta_x = motorsData.message[4]; 
            motorsData.delta_angle = motorsData.message[6]; 
            processMotorCommands(); 
        }
    }
    
    // Out of bounds without end character
    else if (motorsData.currentIndex > 7)
    {
        motorsData.currentIndex = 0;
    }
    
    // Increments index
    else
    {
        motorsData.currentIndex++;
    }
}
*/
void MOTORS_Tasks ( void )
{
    //turn_left_degrees(90);
 // char letter; 
  //xQueueReceive(motorsData.motorQueue, &letter, portMAX_DELAY);
  //AssembleMotorCommands(letter); 
 // move_rover_forward();
}
 

/*******************************************************************************
 End of File
 */
