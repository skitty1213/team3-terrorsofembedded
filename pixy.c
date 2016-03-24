/*******************************************************************************
  MPLAB Harmony Application Source File
  
  Company:
    Microchip Technology Inc.
  
  File Name:
    pixy.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:

 *******************************************************************************/

#include "pixy.h"
#include "pixy_public.h"
#include "timer_setup.h"
#include "traces.h"
//#include "Python.h"
//#include "PixyUART.h"

//#include <xc.h>
//#include <stdioh>

PIXY_DATA pixyData;
//PixyUART pixy; 

#define PIXY_START_WORD             0xAA55
#define PIXY_START_WORD_CC          0xAA56
#define PIXY_START_WORDX            0x55AA


void PIXY_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    pixyData.state = PIXY_STATE_INIT;
    pixyData.pixyQueue = xQueueCreate(80, sizeof(int16_t));
    timer_initiaize_20(20);
    DRV_USART1_Initialize();
    //Py_Initialize(); 
    //updateLines('c');
}

int addQ_fromPxy(int16_t coord)
{
    traces(PIXY_ENTER_QUEUE);
    if (xQueueSendFromISR (pixyData.pixyQueue, &coord, 0) != pdTRUE)
        error('b');
    traces(PIXY_LEAVE_QUEUE);
    //updateLines('c');
}

uint8_t getByte(void){
    uint8_t word;
    xQueueReceive(pixyData.pixyQueue, &word, portMAX_DELAY);
    return word; 
     
}

uint16_t getWord(void)
{
  // this routine assumes little endian 
  uint16_t w; 
  uint16_t c;
  uint16_t front; 
  uint16_t back;
  c = getByte();
  updateLinesNumber(c);
  w = getByte();
  updateLinesNumber(w);
  
  updateLinesNumber2(0xaa);
  w = w << 8;// original
  front = w >> 8; // added
  updateLinesNumber2(front);
  w = w|c; // original 
  updateLinesNumber2(0xbb); 
  back = w & 0x00ff; 
  updateLinesNumber2(back); // displays the same w, trying to get c
  updateLinesNumber2(0xcc); 
 
  //updateLinesNumber2(0x00);// is fine have to tweck in between 
  //addQSnd(w);
  //PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);

  return w;
}

void sendtoServer(void)
{
  addQSnd('A');
  addQSnd('N');
  addQSnd('N');
  addQSnd('A');
  addQSnd('\n');
  PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
}


//static Block *g_blocks; // an array

void PIXY_Tasks ( void )
{
    
    uint16_t data1; 
  
    uint16_t front; 
    uint16_t back;

    //FILE *fp; //////////////////////////////////////////////////////////////////////////////

   // Block *block;
  //g_blocks = (Block *)malloc(sizeof(Block)*100);//an array of blocks 

    switch ( pixyData.state )
    {
        case PIXY_STATE_INIT:
        {
            // accept the number of obstacles 
            pixyData.w = 0xffff;
            pixyData.lastw = 0x0000; 
            pixyData.obstacle_num = 1;
            pixyData.arraycount = 0; 
            pixyData.roundcount = 0; 
            pixyData.state = get_byte; /////////////////////////////////get_zeros originally 
        }
            break; //////////////////////////////////////////////////////////////////////////////
        case get_byte:// get byte
        { 
            updateLinesNumber2(0xff);
            data1 = getByte(); 
            updateLinesNumber2(data1); //get every byte okay
            pixyData.state = get_byte;
            updateLinesNumber2(0xff);
        }
            break;////////////////////////////////////////////////////////////////////////////////
        case get_zeros:
        {
            updateLinesNumber2(0x0a);
            pixyData.w = getWord();
  
            updateLinesNumber2(0x01);//**************************
            front = pixyData.w >> 8; // added
            updateLinesNumber2(front);
            updateLinesNumber2(0x02); 
            back = pixyData.w & 0x00ff; 
            updateLinesNumber2(back); // displays the same w, trying to get c
            updateLinesNumber2(0x03);//**************************
            
            if ((pixyData.w == 0x0000) && (pixyData.lastw == 0x0000)){
                pixyData.state = get_zeros;
            }
            else if ((pixyData.w =! 0x0000) && (pixyData.lastw =! 0x0000)){
                pixyData.state = get_nonzeros; // keep checking until get nonzeros which will be 0xaa55
            }
            pixyData.lastw = pixyData.w; 
            
            updateLinesNumber2(0x04);//**************************
            front = pixyData.lastw >> 8; // added
            updateLinesNumber2(front);
            updateLinesNumber2(0x05); 
            back = pixyData.lastw & 0x00ff; 
            updateLinesNumber2(back); // displays the same w, trying to get c
            updateLinesNumber2(0x06);//**************************
            
            updateLinesNumber2(0x0b);
             
        }// check if there is data 
            break; //////////////////////////////////////////////////////////////////////////////
        case get_nonzeros:
        {
            updateLinesNumber2(0x0c); 
            pixyData.w = getWord();
            
            updateLinesNumber2(0x01);//**************************new word
            front = pixyData.w >> 8; // added
            updateLinesNumber2(front);
            updateLinesNumber2(0x02); 
            back = pixyData.w & 0x00ff; 
            updateLinesNumber2(back); // 
            updateLinesNumber2(0x03);//**************************
            
            updateLinesNumber2(0x04);//**************************old word
            front = pixyData.lastw >> 8; // added
            updateLinesNumber2(front);
            updateLinesNumber2(0x05); 
            back = pixyData.lastw & 0x00ff; 
            updateLinesNumber2(back); // 
            updateLinesNumber2(0x06);//**************************
            
            if ((pixyData.w == PIXY_START_WORD) && (pixyData.lastw == PIXY_START_WORD))
            {
                pixyData.state = start_data; 
                updateLinesNumber2(0x08); // goes into here 
            }
            else if (pixyData.w == 0x55AA)
            {
                data1 = getByte();  // out of sync backwards
                pixyData.state = get_nonzeros ;
                updateLinesNumber2(0xa0); // recallibrate
            }
            else if ((pixyData.w == 0x0000) && (pixyData.lastw == 0x0000)){
                pixyData.state = get_zeros;
            }
            else {
                pixyData.state = get_nonzeros; 
                updateLinesNumber2(0x07); // goes into here 
            }
            pixyData.lastw = pixyData.w; // save the last word
            
            updateLinesNumber2(0x04);//**************************
            front = pixyData.lastw >> 8; // added
            updateLinesNumber2(front);
            updateLinesNumber2(0x05); 
            back = pixyData.lastw & 0x00ff; 
            updateLinesNumber2(back); // displays the same w, trying to get c
            updateLinesNumber2(0x06);//**************************
            
            updateLinesNumber2(0x0d);// get word
        }
            break; ////////////////////////////////////////////////////////////////////////////////////
        case start_data:
        {
            updateLinesNumber2(0x0e);// get word
            
            if (pixyData.obstacle_num == 1.0){
                pixyData.state = obstacle1_read; 
            }
            else if (pixyData.obstacle_num == 2.0){
                pixyData.state = obstacle2_read; 
            }
            else if (pixyData.obstacle_num == 3.0){
                pixyData.state = obstacle3_read; 
            }
            else {
                pixyData.state = error_output;
            }
            updateLinesNumber2(0x0f);
        }
            break; ////////////////////////////////////////////////////////////////////////////////
        case obstacle1_read:// get the data into arrays
        {
            updateLinesNumber2(0x11);
            int counter = 0; 
            for (counter; counter <6; counter++){
                pixyData.w = getWord(); 
                pixyData.array1[counter] = pixyData.w;
                updateLinesNumber2(counter);
            }// array1 will be obstacle 1
            
            pixyData.w = getWord();// should be 0xAA55
            if (pixyData.w == PIXY_START_WORD){
                int counter = 0; 
            for (counter; counter <6; counter++){
                pixyData.w = getWord(); 
                pixyData.array2[counter] = pixyData.w;
                updateLinesNumber2(counter);
            }// array2 will be front of rover 
                }// if statement 
            else {
                pixyData.state = error_restart; 
                }
            
            pixyData.w = getWord(); // should be 0xAA55 
            if (pixyData.w == PIXY_START_WORD){
                int counter = 0; 
            for (counter; counter <6; counter++){
                pixyData.w = getWord(); 
                pixyData.array3[counter] = pixyData.w;
                updateLinesNumber2(counter);
            }// array3 will be back of rover 
                }// if statement
            else {
                pixyData.state = error_restart; 
                }
            
            pixyData.state = obstacle1_testing; 
            updateLinesNumber2(0x22);// 
        }
            break; /////////////////////////////////////////////////////////////////////////////
        case obstacle1_testing: // check what is in the arrays 
        {
            updateLinesNumber2(0x33);// 
            //////////////////////////////////////////////////////
            int counter = 0; 
            uint16_t testing; 
            for (counter; counter <6; counter++){
                testing = pixyData.array1[counter]; 
                
            updateLinesNumber2(0xA1);//**************************
            front = testing >> 8; // added
            updateLinesNumber2(front);
            //updateLinesNumber2(0x05); 
            back = testing & 0x00ff; 
            updateLinesNumber2(back); // displays the same w, trying to get c
            updateLinesNumber2(0xA1);//**************************
                
            }// array3 will be back of rover 
            counter = 0; 
            for (counter; counter <6; counter++){
                testing = pixyData.array2[counter];
            
            updateLinesNumber2(0xA2);//**************************
            front = testing >> 8; // added
            updateLinesNumber2(front);
            //updateLinesNumber2(0x05); 
            back = testing & 0x00ff; 
            updateLinesNumber2(back); // displays the same w, trying to get c
            updateLinesNumber2(0xA2);//**************************
                
            }// array3 will be back of rover 
            counter = 0; 
            for (counter; counter <6; counter++){
                testing = pixyData.array3[counter];
            
            updateLinesNumber2(0xA3);//**************************
            front = testing >> 8; // added
            updateLinesNumber2(front);
            //updateLinesNumber2(0x05); 
            back = testing & 0x00ff; 
            updateLinesNumber2(back); // displays the same w, trying to get c
            updateLinesNumber2(0xA3);//**************************
                
            }// array3 will be back of rover 
              
            //pixyData.state = error_restart; // stop here
            pixyData.state = obstacle1_xyExtract;
            updateLinesNumber2(0x44);
        }
            break; /////////////////////////////////////////////////////////////////////////////
        case obstacle1_xyExtract: // get out the x and y according to the colors
        {
            updateLinesNumber2(0x55);
            if (pixyData.roundcount < 4 ){// will have 4 rounds of data
                
                    pixyData.obstacle1_x = pixyData.array1[2] + pixyData.obstacle1_x; // x coordinate
                    pixyData.obstacle1_y = pixyData.array1[3] + pixyData.obstacle1_y; // y coordinate 
                
                    pixyData.frontLead_x = pixyData.array2[2] + pixyData.frontLead_x; // x coordinate
                    pixyData.frontLead_y = pixyData.array2[3] + pixyData.frontLead_y; // y coordinate 
                    
                    pixyData.backLead_x = pixyData.array3[2] + pixyData.backLead_x; // x coordinate
                    pixyData.backLead_y = pixyData.array3[3] + pixyData.backLead_y; // y coordinate 
             
                pixyData.roundcount ++; // increment everytime we get data 
                updateLinesNumber2(pixyData.roundcount);// update how many times it has gone through state 
                pixyData.state =  get_zeros; // go back to collecting data 
            }
            else if (pixyData.roundcount == 4){ // 5 values in the arrays 
                //pixyData.state = obstacle1_calibrate;
                    pixyData.obstacle1_x = pixyData.array1[2] + pixyData.obstacle1_x; // x coordinate
                    pixyData.obstacle1_y = pixyData.array1[3] + pixyData.obstacle1_y; // y coordinate 
                
                    pixyData.frontLead_x = pixyData.array2[2] + pixyData.frontLead_x; // x coordinate
                    pixyData.frontLead_y = pixyData.array2[3] + pixyData.frontLead_y; // y coordinate 
                    
                    pixyData.backLead_x = pixyData.array3[2] + pixyData.backLead_x; // x coordinate
                    pixyData.backLead_y = pixyData.array3[3] + pixyData.backLead_y; // y coordinate 
                    
                pixyData.state =  obstacle1_calibrate; 
                pixyData.roundcount = 0; //reset the value 
            }
            else {
                pixyData.state = error_output; 
            }
     
            updateLinesNumber2(0x66);
        }
            break; ////////////////////////////////////////////////////////////////////////////////
        case xyCheck: // the totaled data of five readings 
        {
            updateLinesNumber2(0x33);

         /*addQSnd(0x33);
         addQSnd(pixyData.obstacle1_x);
         addQSnd(pixyData.frontLead_x);
         addQSnd(pixyData.backLead_x);
         
         addQSnd(pixyData.obstacle1_y);
         addQSnd(pixyData.frontLead_y);
         addQSnd(pixyData.backLead_y);
          addQSnd(0x33);
             
        PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT); */
        
            pixyData.state = error_restart;
             updateLinesNumber2(0x44);
        }
            break; ///////////////////////////////////////////////////////////////////////////////////
        case obstacle1_calibrate:// finish taking the average and have to calibrate 
        {
             updateLinesNumber2(0x77);
         pixyData.obstacle1_x /= 5; 
         pixyData.frontLead_x /= 5; 
         pixyData.backLead_x /= 5;
         
         pixyData.obstacle1_y /= 5; 
         pixyData.frontLead_y /= 5; 
         pixyData.backLead_y /= 5;
         
         //fp = fopen( "/Users/lanna1/Documents/temp/test.txt", "w+");
         //fprintf(fp, "This is testing for fprintf...\n");
         //fputs("This is testing for fputs...\n", fp);
         //fclose(fp);
         
          updateLinesNumber2(0x88); //gets here fine 
          pixyData.state = xyCheck;
        }
            break; //////////////////////////////////////////////////////////////////////////////////
        case error_restart:
        {
            updateLinesNumber2(0xDD);// 
            //pixyData.state = error_restart;//??
            pixyData.state = get_zeros; // restart because not enough objects found
        }
            break; /////////////////////////////////////////////////////////////////////////////
        case error_output:
        {
            updateLinesNumber2(0xEE);// 
            //pixyData.state =error_output;//??
            pixyData.state = error_output; // restart because not enough objects found
        }
            break; //////////////////////////////////////////////////////////////////////////////
    }
}
 

/*******************************************************************************
 End of File
 */
