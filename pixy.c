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

////////////////////////////////////////////////////////////////////////////////////////

#include "pixy.h"
#include "pixy_public.h"
#include "timer_setup.h"
#include "traces.h"
#include "receive.h"
#include "send.h"
#include "receive_public.h"
#include "send_public.h"
#include <stdio.h>
#include <stdlib.h>
#include "debug.h"


//#include "Python.h"
//#include "PixyUART.h"

//#include <xc.h>
//#include <stdioh>

PIXY_DATA pixyData;

#define PIXY_START_WORD             0xAA55
#define PIXY_START_WORD_CC          0xAA56
#define PIXY_START_WORDX            0x55AA
#define OBSTACLE_1			0X05
#define OBSTACLE_2			0X06
#define OBSTACLE_3			0X07
#define LEAD_ROVER			0X0A
#define FOLLOWER_ROVER		0X13
#define TOP_LEFT			0X0B
#define TOP_RIGHT			0X0C
#define BOTTOM_LEFT			0X1C
#define BOTTOM_RIGHT 		0X14 
////////////////////////////////////////// server variables
#define leadAngle_float         0X31
#define followerAngle_float     0X32

#define obstacle1_x_float       0X33
#define obstacle1_y_float       0X34

#define frontLead_x_float       0X35
#define frontLead_y_float       0X36

#define frontFollower_x_float   0X37
#define frontFollower_y_float   0X38

#define topLeft_x_float         0X3A
#define topLeft_y_float         0X3B

#define  topRight_x_float       0X3C
#define  topRight_y_float       0X3D

#define bottomLeft_x_float      0X3E
#define bottomLeft_y_float      0X3F

#define bottomRight_x_float     0X41
#define bottomRight_y_float     0X42

#define averagex_Range_float    0X43
#define averagey_Range_float    0X44

#define calibrated_xRatio_float 0X45
#define calibrated_yRatio_float 0X46

#define leadAngleFinal_2byte    0X47
#define followerAngleFinal_2byte 0X48

#define leadPos_xFinal_2byte    0XCC
#define leadPos_yFinal_2byte    0XDD

#define followerPos_xFinal_2byte 0XEE
#define followerPos_yFinal_2byte 0XFF

#define obstacle1_xFinal_2byte 0XAA
#define obstacle1_yFinal_2byte 0XBB

#define obstacle2_xFinal_2byte  0X4A
#define obstacle2_yFinal_2byte 0X4B

#define obstacle3_xFinal_2byte 0X4C
#define obstacle3_yFinal_2byte 0X4D

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
    //updateLinesNumber(0x0a);
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
 // updateLinesNumber(c);/////////////////////////////////////////////////////////////////
  w = getByte();
 // updateLinesNumber(w);/////////////////////////////////////////////////////////////////
  
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
    addQSnd('~');
  addQSnd('A');
  addQSnd('A');
  addQSnd('N');
  addQSnd('N');
  addQSnd('A');
  addQSnd('n');
  addQSnd(')');
  //PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
  //updateLinesNumber(0x0a); 
}

void calculateCoordinates(void)
{
    updateLinesNumber2(0x56);
    pixyData.average_xRange = pixyData.topRight_x - pixyData.topLeft_x; 
    pixyData.average_xRange = ((pixyData.bottomRight_x - pixyData.bottomLeft_x ) + pixyData.average_xRange)/2.0; // range of the x
    
   
    pixyData.average_yRange = pixyData.bottomLeft_y - pixyData.topLeft_y; 
    pixyData.average_yRange = ((pixyData.bottomRight_y - pixyData.topRight_y ) + pixyData.average_yRange)/2.0;// range of the y 
   
    
    pixyData.calibrated_xRatio = (720/ pixyData.average_xRange); // inches for now
    pixyData.calibrated_yRatio = (720/ pixyData.average_yRange); 
    
   
    pixyData.leadPos_xFinal = pixyData.frontLead_x * pixyData.calibrated_xRatio;
    pixyData.leadPos_yFinal = pixyData.frontLead_y * pixyData.calibrated_yRatio; 
    pixyData.followerPos_xFinal = pixyData.frontFollower_x * pixyData.calibrated_xRatio; 
    pixyData.followerPos_yFinal = pixyData.frontFollower_y * pixyData.calibrated_yRatio; 
    pixyData.obstacle1_xFinal = (pixyData.obstacle1_x - pixyData.topLeft_x) * pixyData.calibrated_xRatio; 
    pixyData.obstacle1_yFinal = (pixyData.obstacle1_y - pixyData.topLeft_y) * pixyData.calibrated_yRatio; 
    
    updateLinesNumber2(0x78);///////////////////////////////////////////////////////////////////////////
    updateLinesNumber2(pixyData.obstacle1_xFinal >> 8);
    updateLinesNumber2(pixyData.obstacle1_xFinal);
    
    updateLinesNumber2(pixyData.obstacle1_yFinal >> 8);
    updateLinesNumber2(pixyData.obstacle1_yFinal);
    
    /*pixyData.obstacle2_xFinal; 
    pixyData.obstacle2_yFinal; 
    pixyData.obstacle3_xFinal; 
    pixyData.obstacle3_yFinal; */
    
    updateLinesNumber2(0x78); 
}

void calculateCoordincatesWithAngles(void)
{
    float val;
    val = 180.0/ 3.14159265; 
    
    pixyData.angle_degree = atan( (pixyData.topRight_y - pixyData.topLeft_y)/ (pixyData.topRight_x - pixyData.topLeft_x) ); 
    
   
    pixyData.average_xRange = pixyData.topRight_x - pixyData.topLeft_x; 
    pixyData.average_xRange = ((pixyData.bottomRight_x - pixyData.bottomLeft_x ) + pixyData.average_xRange)/2.0; // range of the x
    
   
    pixyData.average_yRange = pixyData.bottomLeft_y - pixyData.topLeft_y; 
    pixyData.average_yRange = ((pixyData.bottomRight_y - pixyData.topRight_y ) + pixyData.average_yRange)/2.0;// range of the y 
   
    
    pixyData.calibrated_xRatio = (720/ pixyData.average_xRange); // inches for now
    pixyData.calibrated_yRatio = (720/ pixyData.average_yRange); 
    
   
    pixyData.leadPos_xFinal = pixyData.frontLead_x * pixyData.calibrated_xRatio; // change later to complete
    pixyData.leadPos_yFinal = pixyData.frontLead_y * pixyData.calibrated_yRatio; // change later to complete
    pixyData.followerPos_xFinal = pixyData.frontFollower_x * pixyData.calibrated_xRatio; // change later to complete
    pixyData.followerPos_yFinal = pixyData.frontFollower_y * pixyData.calibrated_yRatio; // change later to complete
   // pixyData.obstacle1_xFinal = (pixyData.obstacle1_x - pixyData.topLeft_x) * pixyData.calibrated_xRatio; 
   // pixyData.obstacle1_yFinal = (pixyData.obstacle1_y - pixyData.topLeft_y) * pixyData.calibrated_yRatio; 
    
    //pixyData.obstacle1_xFinal = (pixyData.obstacle1_x * cos(pixyData.angle_degree) - pixyData.obstacle1_y * sin(pixyData.angle_degree) - pixyData.topLeft_x) * pixyData.calibrated_xRatio;
    //pixyData.obstacle1_yFinal = (pixyData.obstacle1_x * cos(pixyData.angle_degree) - pixyData.obstacle1_y * sin(pixyData.angle_degree)- pixyData.topLeft_y) * pixyData.calibrated_yRatio;
    pixyData.obstacle1_xFinal = (pixyData.obstacle1_x * cos(pixyData.angle_degree) - pixyData.obstacle1_y * sin(pixyData.angle_degree) - pixyData.topLeft_x) * pixyData.calibrated_xRatio;
    pixyData.obstacle1_yFinal = (pixyData.obstacle1_x * sin(pixyData.angle_degree) + pixyData.obstacle1_y * cos(pixyData.angle_degree)- pixyData.topLeft_y) * pixyData.calibrated_yRatio;
    
    
}

void calculateAngle(void)
{
    updateLinesNumber2(0x12); 
   if ( pixyData.leadAngle < 0){ // convert the values of negative pixy values
       
        pixyData.leadAngle = 180 + ( 180 - abs(pixyData.leadAngle) ); 
    }
   
   if (pixyData.leadAngle < 45){ // convert to (0,0)
       
       pixyData.leadAngle = 360 + (pixyData.leadAngle - 45); 
   }
   else {
       pixyData.leadAngle = pixyData.leadAngle - 45; 
   }
    
   if (pixyData.leadAngle > 270){// convert to rover location 
       
       pixyData.leadAngle = (pixyData.leadAngle + 90) - 360; 
   } 
   else {// from 0 to 315
       pixyData.leadAngle = pixyData.leadAngle + 90; 
   }
   ////////////////////////////////////////////////////////////////////////////////////////
   
    
      if ( pixyData.followerAngle <= 0){ // convert the values of negative pixy values
        
        pixyData.followerAngle = 180 + ( 180 - abs(pixyData.followerAngle) ); 
    }
   
    /*if (pixyData.followerAngle > 265 && pixyData.followerAngle < 275)
    {
        pixyData.followerAngle = 225; 
    }
    else {*/
   if (pixyData.followerAngle <= 45){
       
       pixyData.followerAngle = 360 + (pixyData.followerAngle - 45); 
   }
   else {
       pixyData.followerAngle = pixyData.followerAngle - 45; 
   }
    
    if (pixyData.followerAngle >= 270){// convert to rover location 
       
       pixyData.followerAngle = (pixyData.followerAngle + 90) - 360; 
   } 
   else {// from 0 to 315
       pixyData.followerAngle = pixyData.followerAngle + 90; 
   }
    //}
    
    uint16_t update =  pixyData.followerAngle; 
     updateLinesNumber2(0x34);////////////////////////////////////////////////////////
    updateLinesNumber2(update >> 8); 
    updateLinesNumber2(update); 
    
    //updateLinesNumber2(pixyData.leadAngle); //gets here fine
   
   pixyData.leadAngleFinal = pixyData.leadAngle/2; 
   pixyData.followerAngleFinal = pixyData.followerAngle/2;
   
   
    
   updateLinesNumber2(0x34); 
}

void resetData(void)
{
        pixyData.leadAngle = 0;
        pixyData.followerAngle = 0; 
        
        pixyData.obstacle1_x = 0; 
        pixyData.obstacle2_x = 0; 
        pixyData.obstacle3_x = 0; 
        pixyData.frontLead_x = 0; 
        pixyData.frontFollower_x = 0;
        pixyData.topLeft_x = 0;
        pixyData.topRight_x = 0; 
        pixyData.bottomLeft_x = 0; 
        pixyData.bottomRight_x = 0; 
        
        pixyData.obstacle1_y = 0; 
        pixyData.obstacle2_y = 0; 
        pixyData.obstacle3_y = 0; 
        pixyData.frontLead_y = 0; 
        pixyData.frontFollower_y = 0;
        pixyData.topLeft_y = 0;
        pixyData.topRight_y = 0; 
        pixyData.bottomLeft_y = 0; 
        pixyData.bottomRight_y = 0;
}

void debuggingQueue(void) // puts directly to the server 
{
    updateLinesNumber2(0xDE);
    float testing_float, testing_float2; 
    testing_float = -1795.9;
    uint16_t testingConversion; 
    unsigned char e;
    
    if (testing_float > 0.0)
        e = '+';
    else{
        e = '-';
        testing_float= abs(testing_float);
    }
    
    testingConversion = testing_float; 
    
    pixyData.sequence_num ++;
    uint8_t a = pixyData.sequence_num >> 8; 
    uint8_t b = pixyData.sequence_num; 
    
    uint8_t c = testingConversion >> 8; 
    uint8_t d = testingConversion; 
     
    /////////////////////////////////////////leadAngle 
    addQSnd('~');
    addQSnd(leadAngle_float); //0x31
    addQSnd(a);
    addQSnd(b);// nothing, just to fill in the space
    addQSnd(c); 
    addQSnd(d);
    addQSnd(e);
    addQSnd(')');
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
    updateLinesNumber2(0xF0);
}

void putinWiflyQueue(int8_t number)
{
    float testing_float; 
    //testing_float = pixyData.obstacle1_x;
    uint16_t testingConversion; 
    unsigned char e;
    
    if ( number == obstacle1_x_float){
        testing_float = pixyData.obstacle1_x;
        testingConversion = testing_float;
    }
    else if ( number == obstacle1_y_float){
        testing_float = pixyData.obstacle1_y; 
        testingConversion = testing_float;
    }
    else if ( number == leadAngle_float){
        testing_float = pixyData.leadAngle;////////////// 
        testingConversion = testing_float;
    }
    else if ( number == followerAngle_float){
        testing_float = pixyData.followerAngle;
        testingConversion = testing_float;
    }
    else if ( number == frontLead_x_float){
        testing_float = pixyData.frontLead_x; 
        testingConversion = testing_float;
    }
    else if ( number == frontLead_y_float){
        testing_float = pixyData.frontLead_y; 
        testingConversion = testing_float;
    }
    else if ( number == frontFollower_x_float){
        testing_float = pixyData.frontFollower_x;
        testingConversion = testing_float;
    }
    else if ( number == frontFollower_y_float){
        testing_float = pixyData.frontFollower_y; 
        testingConversion = testing_float;
    }
    else if ( number == topLeft_x_float){
        testing_float = pixyData.topLeft_x; 
        testingConversion = testing_float;
    }
    else if ( number == topLeft_y_float){
        testing_float = pixyData.topLeft_y;
        testingConversion = testing_float;
    }
    else if ( number == topRight_x_float){
        testing_float = pixyData.topRight_x; 
        testingConversion = testing_float;
    }
    else if ( number == topRight_y_float){
        testing_float = pixyData.topRight_y;
        testingConversion = testing_float;
    }
    else if ( number == bottomLeft_x_float){
        testing_float = pixyData.bottomLeft_x; 
        testingConversion = testing_float;
    }
    else if ( number == bottomLeft_y_float){
        testing_float = pixyData.bottomLeft_y;
        testingConversion = testing_float;
    }
    else if ( number == bottomRight_x_float){
        testing_float = pixyData.bottomRight_x; 
        testingConversion = testing_float;
    }
    else if ( number == bottomRight_y_float){
        testing_float = pixyData.bottomRight_y; 
        testingConversion = testing_float;
    }
    else if ( number ==  averagex_Range_float){
        testing_float = pixyData. average_xRange;
        testingConversion = testing_float;
    }
    else if ( number ==  averagey_Range_float){
        testing_float = pixyData. average_yRange;
        testingConversion = testing_float;
    }
    else if ( number == calibrated_xRatio_float){
        testing_float = pixyData.calibrated_xRatio;
        testingConversion = testing_float;
    }
    else if ( number == calibrated_yRatio_float){
        testing_float = pixyData.calibrated_yRatio;
        testingConversion = testing_float;
    }
    else if ( number == leadAngleFinal_2byte){
        testingConversion = pixyData.leadAngleFinal; 
    }
    else if ( number == followerAngleFinal_2byte){
        testingConversion = pixyData.followerAngleFinal; 
    }
    else if ( number == leadPos_xFinal_2byte){
        testingConversion = pixyData.leadPos_xFinal; 
    }
    else if ( number == leadPos_yFinal_2byte){
        testingConversion = pixyData.leadPos_yFinal; 
    }
    else if ( number == followerPos_xFinal_2byte){
        testingConversion = pixyData.followerPos_xFinal; 
    }
    else if ( number == followerPos_yFinal_2byte){
        testingConversion = pixyData.followerPos_yFinal; 
    }
    else if ( number == obstacle1_xFinal_2byte){
        testingConversion = pixyData.obstacle1_xFinal; 
    }
    else if ( number == obstacle1_yFinal_2byte){
        testingConversion = pixyData.obstacle1_yFinal; 
    }
    else if ( number == obstacle2_xFinal_2byte){
        testingConversion = pixyData.obstacle2_xFinal; 
    }
    else if ( number == obstacle2_yFinal_2byte){
        testingConversion = pixyData.obstacle2_yFinal; 
    }
    else if ( number == obstacle3_xFinal_2byte){
        testingConversion = pixyData.obstacle3_xFinal; 
    }
    else if ( number == obstacle3_yFinal_2byte){
        testingConversion = pixyData.obstacle3_yFinal; 
    }
    
    if (testing_float > 0.0)
        e = '+';
    else{
        e = '-';
        testing_float= abs(testing_float);
    }
    
    //testingConversion = testing_float; 
    
    pixyData.sequence_num ++;
    uint8_t a = pixyData.sequence_num >> 8; 
    uint8_t b = pixyData.sequence_num; 
    
    uint8_t c = testingConversion >> 8; 
    uint8_t d = testingConversion; 
     
    /////////////////////////////////////////leadAngle 
    addQSnd('~');
    addQSnd(number); //0x31
    addQSnd(a);
    addQSnd(b);
    addQSnd(c); 
    addQSnd(d);
    addQSnd(e);
    addQSnd(')');
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
}


void sendObstacle1()
{
    uint16_t conversionx;
    uint16_t conversiony;
    
    pixyData.sequence_num ++;
    uint8_t a = pixyData.sequence_num >> 8; 
    uint8_t b = pixyData.sequence_num; 
    
    conversionx = pixyData.obstacle1_xFinal;// twice as big!!
    conversiony = pixyData.obstacle1_yFinal; 
    
    uint8_t c = conversionx >> 8; 
    uint8_t d = conversionx;
    
    uint8_t e = conversiony >> 8; 
    uint8_t f = conversiony;
    
    /*uint8_t c = pixyData.obstacle1_xFinal >> 8; 
    uint8_t d = pixyData.obstacle1_xFinal;
    
    uint8_t e = pixyData.obstacle1_yFinal >> 8; 
    uint8_t f = pixyData.obstacle1_yFinal;*/
    
     
    /////////////////////////////////////////leadAngle 
    addQSnd('~');
    addQSnd(a);
    addQSnd(b);
    addQSnd(c); 
    addQSnd(d);
    addQSnd(e);
    addQSnd(f);
    addQSnd(')');
    PLIB_INT_SourceEnable(INT_ID_0, INT_SOURCE_USART_1_TRANSMIT);
}

void finishedObstactle1(){
    
    uint16_t conversionx;
    uint16_t conversiony;
    
    pixyData.sequence_num ++;
    uint8_t a = pixyData.sequence_num >> 8; 
    uint8_t b = pixyData.sequence_num; 
    
    conversionx = pixyData.obstacle1_xFinal;// twice as big!!
    conversiony = pixyData.obstacle1_yFinal; 
    
   // uint16_t testingConversion;
    
    addQSnd('~');
    addQSnd(0x11);
    addQSnd(a);// sequence number
    addQSnd(b); // sequence number
    addQSnd(conversionx); // x position 
    addQSnd(conversiony);
    addQSnd(0x00);// orientation none 
    addQSnd(')');
}

void finishedLeadRover(){
    
    uint16_t conversionx;
    uint16_t conversiony;
    
    pixyData.sequence_num ++;
    uint8_t a = pixyData.sequence_num >> 8; 
    uint8_t b = pixyData.sequence_num; 
    
    pixyData.leadPos_xFinal = pixyData.leadPos_xFinal * (2.54/10);
    pixyData.leadPos_yFinal = pixyData.leadPos_yFinal * (2.54/10); 
    
    conversionx = pixyData.leadPos_xFinal;// twice as big!!
    conversiony = pixyData.leadPos_yFinal; 
    
   uint16_t conversionAngle =  pixyData.leadAngleFinal;
    
    addQSnd('~');
    addQSnd(0x10);
    addQSnd(a); // sequence number
    addQSnd(b); // sequence number
    addQSnd(conversionx);
    addQSnd(conversiony);
    addQSnd(conversionAngle);
    addQSnd(')');
} 
            
void finishedFollowerRover(){
    
   /* addQSnd('~');
    addQSnd(0x15);
    addQSnd(b);
    addQSnd(c); 
    addQSnd(d);
    addQSnd(e);
    addQSnd(f);
    addQSnd(')');*/
}


void PIXY_Tasks ( void )
{
    uint16_t data1; 
    uint16_t front; 
    uint16_t back;
    //sendtoServer();///////////////////////////////////////////////////////////////////////////////////
    switch ( pixyData.state )
    {
        case PIXY_STATE_INIT:
        {
            // accept the number of obstacles 
            pixyData.w = 0xffff;
            pixyData.lastw = 0x0000; 
            pixyData.obstacle_num = 1.0;
            pixyData.arraycount = 0; 
            pixyData.roundcount = 0; 
            pixyData.sequence_num = 0; 
            pixyData.state = get_zeros; /////////////////////////////////get_zeros originally 
            pixyData.debug[0] = '~'; 
            pixyData.debug[7] = ')'; 
            //sendtoServer(); 
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
            //resetData(); 
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
            /*else if ((pixyData.w =! 0x0000) || (pixyData.lastw =! 0x0000)){
                pixyData.state = get_nonzeros; // keep checking until get nonzeros which will be 0xaa55
            }*/
            else {
                pixyData.state = get_nonzeros;
                updateLinesNumber2(0x99);
            }

            pixyData.lastw = pixyData.w; // save the last word
            
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
            //sendtoServer(); ////////////////////////////////////////////////////////////////////
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
            ///////////////////////// 4/5 update, try to not use get_zeros copy the getStart
            if ( (pixyData.w == 0x00) && (pixyData.lastw == 0x00))
            {
                pixyData.state = get_nonzeros; 
            }
            else if ((pixyData.w == PIXY_START_WORD) && (pixyData.lastw == PIXY_START_WORD))
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
            /*else if ((pixyData.w == 0x0000) && (pixyData.lastw == 0x0000)){
                pixyData.state = get_zeros;
            }*/
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
            updateLinesNumber2(0x0e);// start data
            
            if (pixyData.obstacle_num == 1.0){
                pixyData.state = obstacle1_read; 
            }
            else if (pixyData.obstacle_num == 2.0){
                pixyData.state = obstacle1_read; 
            }
            else if (pixyData.obstacle_num == 3.0){
                pixyData.state = obstacle1_read; 
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
            if (pixyData.obstacle_num == 1.0){
                pixyData.arraycount = 7.0; //7 arrays
            }
            else if (pixyData.obstacle_num == 2.0){
                pixyData.arraycount = 8.0; // stops at 8
            }
            else if (pixyData.obstacle_num == 3.0){
                pixyData.arraycount = 9.0; // stops at 9
            }

            int counter = 0; 
            pixyData.arraycountTracker = 1;
            
                pixyData.w = getWord(); // checksum get rid of
                /*if (pixyData.w != 0x05){
                    pixyData.w = getWord(); 
                }// new if statement */
                
            for (counter = 0; counter < 5; counter++) {
                pixyData.w = getWord();
                pixyData.array1[counter] = pixyData.w;
                updateLinesNumber2(counter);
            }

            pixyData.w = getWord(); // should be 0xAA56 for A
            pixyData.lastw = pixyData.w;////////////////////////////////////////////////////////////////////////////////??????//
            pixyData.state = notFirstEnter; 
            updateLinesNumber2(0x90);
            updateLinesNumber2(pixyData.arraycountTracker);
            updateLinesNumber2(0x22);// 
        }
            break; /////////////////////////////////////////////////////////////////////////////
         case notFirstEnter:// both 1 , 2  or 3 arrrays
        {
            updateLinesNumber2(0x13);
            int counter = 0; 
            //pixyData.w = getWord(); // should be 0xAA56 for A
            
            if ((pixyData.w == PIXY_START_WORD_CC) && (pixyData.arraycountTracker != pixyData.arraycount)) {// changed the less than to !=
                //counter = 0;
                pixyData.w = getWord(); // checksum
                pixyData.w = getWord(); // have to get the SS or CC
                if (pixyData.w == LEAD_ROVER) {// option1
                    for (counter = 0; counter < 6; counter++) {
                        pixyData.array4[counter] = pixyData.w;
                        pixyData.w = getWord();
                    }// array 4 will be lead rover 
                    pixyData.arraycountTracker++; // increment at the end
                    updateLinesNumber2(pixyData.arraycountTracker);
                    pixyData.state = notFirstEnter;
                }// if statement 
                else if (pixyData.w == FOLLOWER_ROVER) {// option2
                    for (counter = 0; counter < 6; counter++) {
                        pixyData.array5[counter] = pixyData.w;
                        pixyData.w = getWord();
                    }// array 4 will be lead rover 
                    pixyData.arraycountTracker++; // increment at the end
                    updateLinesNumber2(pixyData.arraycountTracker);
                    pixyData.state = notFirstEnter;
                }// if statement 
                else if (pixyData.w == TOP_LEFT) {// option2
                    for (counter = 0; counter < 6; counter++) {
                        pixyData.array6[counter] = pixyData.w;
                        pixyData.w = getWord();
                    }// array 4 will be lead rover 
                    pixyData.arraycountTracker++; // increment at the end
                    updateLinesNumber2(pixyData.arraycountTracker);
                    pixyData.state = notFirstEnter;
                }// if statement 
                else if (pixyData.w == TOP_RIGHT) {// option2
                    for (counter = 0; counter < 6; counter++) {
                        pixyData.array7[counter] = pixyData.w;
                        pixyData.w = getWord();
                    }// array 4 will be lead rover 
                    pixyData.arraycountTracker++; // increment at the end
                    updateLinesNumber2(pixyData.arraycountTracker);
                    pixyData.state = notFirstEnter;
                }// if statement 
                else if (pixyData.w == BOTTOM_LEFT) {// option2
                    for (counter = 0; counter < 6; counter++) {
                        pixyData.array8[counter] = pixyData.w;
                        pixyData.w = getWord();
                    }// array 4 will be lead rover 
                    pixyData.arraycountTracker++; // increment at the end
                    updateLinesNumber2(pixyData.arraycountTracker);
                    pixyData.state = notFirstEnter;
                }// if statement 
                else if (pixyData.w == BOTTOM_RIGHT) {// option2
                    for (counter = 0; counter < 6; counter++) {
                        pixyData.array9[counter] = pixyData.w;
                        pixyData.w = getWord();
                    }// array 4 will be lead rover 
                    pixyData.arraycountTracker++; // increment at the end
                    updateLinesNumber2(pixyData.arraycountTracker);
                    pixyData.state = notFirstEnter;
                }// if statement 
            }// big if statement

            else if (pixyData.w == PIXY_START_WORD && (pixyData.arraycountTracker != pixyData.arraycount)) { // changed from < to !=
                pixyData.w = getWord(); // checksum
                pixyData.w = getWord(); // SS
                if (pixyData.w == OBSTACLE_2) {// option2
                    for (counter = 0; counter < 5; counter++) {
                        pixyData.array2[counter] = pixyData.w;
                        pixyData.w = getWord();
                        updateLinesNumber2(counter);//////////////////////////////
                    }// array 4 will be lead rover 
                    pixyData.arraycountTracker++; // increment at the end
                    //updateLinesNumber2(pixyData.arraycountTracker);
                    pixyData.state = notFirstEnter;
                }// if statement 
                else if (pixyData.w == OBSTACLE_3) {// option2
                    pixyData.w = getWord(); // checksum
                    pixyData.w = getWord(); // SS
                    for (counter = 0; counter < 5; counter++) {
                        pixyData.array3[counter] = pixyData.w;
                        pixyData.w = getWord();
                        updateLinesNumber2(counter);////////////////////////////////
                    }// array 4 will be lead rover 
                    pixyData.arraycountTracker++; // increment at the end
                    //updateLinesNumber2(pixyData.arraycountTracker);
                    pixyData.state = notFirstEnter;
                }// if statement 
            }// else if 

            else {
                pixyData.state = obstacle1_testing;
                updateLinesNumber2(0x90);
                updateLinesNumber2(pixyData.arraycountTracker);//should be 7 or 8 or 9 
                pixyData.arraycountTracker = 0.0; // reset because now done
            }
            //updateLinesNumber2(pixyData.arraycountTracker); 
            updateLinesNumber2(0x57);
        }
  break; ///////////////////////////////////////   
        case obstacle1_testing: // check what is in the arrays 
        {
            updateLinesNumber2(0x33);// 
            //////////////////////////////////////////////////////
            int counter = 0; 
            uint16_t testing; 
            for (counter  = 0; counter <5; counter++){
                testing = pixyData.array1[counter]; 
            updateLinesNumber2(0xA1);//************************** array1
            front = testing >> 8; // added
            updateLinesNumber2(front);
            //updateLinesNumber2(0x05); 
            back = testing & 0x00ff; 
            updateLinesNumber2(back); // displays the same w, trying to get c
            updateLinesNumber2(0xA1);//**************************
            }// array3 will be back of rover 
            
            counter = 0; 
            for (counter = 0; counter <6; counter++){
                testing = pixyData.array4[counter];
            updateLinesNumber2(0xA4);//************************** array 4
            front = testing >> 8; // added
            updateLinesNumber2(front);
            //updateLinesNumber2(0x05); 
            back = testing & 0x00ff; 
            updateLinesNumber2(back); // displays the same w, trying to get c
            updateLinesNumber2(0xA4);//**************************
            }// array3 will be back of rover 
            
            counter = 0; 
            for (counter = 0; counter <6; counter++){
                testing = pixyData.array6[counter];
            updateLinesNumber2(0xA6);//**************************array 6
            front = testing >> 8; // added
            updateLinesNumber2(front);
            //updateLinesNumber2(0x05); 
            back = testing & 0x00ff; 
            updateLinesNumber2(back); // displays the same w, trying to get c
            updateLinesNumber2(0xA6);//**************************
            }// array3 will be back of rover 
              
             counter = 0; 
            for (counter  = 0; counter <6; counter++){
                testing = pixyData.array7[counter];
            updateLinesNumber2(0xA7);//************************** array 7
            front = testing >> 8; // added
            updateLinesNumber2(front);
            //updateLinesNumber2(0x05); 
            back = testing & 0x00ff; 
            updateLinesNumber2(back); // displays the same w, trying to get c
            updateLinesNumber2(0xA7);//**************************
            }// array3 will be back of rover 
             
              counter = 0; 
            for (counter  = 0; counter <6; counter++){
                testing = pixyData.array9[counter];
            updateLinesNumber2(0xA9);//************************** array 9
            front = testing >> 8; // added
            updateLinesNumber2(front);
            //updateLinesNumber2(0x05); 
            back = testing & 0x00ff; 
            updateLinesNumber2(back); // displays the same w, trying to get c
            updateLinesNumber2(0xA9);//**************************
            }// array3 will be back of rover 
             
               counter = 0; 
            for (counter = 0; counter <6; counter++){
                testing = pixyData.array5[counter];
            updateLinesNumber2(0xA5);//************************** array 5 
            front = testing >> 8; // added
            updateLinesNumber2(front);
            //updateLinesNumber2(0x05); 
            back = testing & 0x00ff; 
            updateLinesNumber2(back); // displays the same w, trying to get c
            updateLinesNumber2(0xA5);//**************************
            }// array3 will be back of rover 
               
                counter = 0; 
            for (counter = 0; counter <6; counter++){
                testing = pixyData.array8[counter];
            updateLinesNumber2(0xA8);//************************** array 8 
            front = testing >> 8; // added
            updateLinesNumber2(front);
            //updateLinesNumber2(0x05); 
            back = testing & 0x00ff; 
            updateLinesNumber2(back); // displays the same w, trying to get c
            updateLinesNumber2(0xA8);//**************************
            }// array3 will be back of rover 
                
                if (pixyData.obstacle_num == 2.0) {
                for (counter  = 0; counter < 5; counter++) {
                    testing = pixyData.array2[counter];
                    updateLinesNumber2(0xA2); //************************** array 2
                    front = testing >> 8; // added
                    updateLinesNumber2(front);
                    //updateLinesNumber2(0x05); 
                    back = testing & 0x00ff;
                    updateLinesNumber2(back); // displays the same w, trying to get c
                    updateLinesNumber2(0xA2); //**************************
                }// array3 will be back of rover 
            }
                if (pixyData.obstacle_num == 3.0){
                    for (counter = 0; counter < 5; counter++) {
                    testing = pixyData.array3[counter];
                    updateLinesNumber2(0xA3); //************************** array 3 
                    front = testing >> 8; // added
                    updateLinesNumber2(front);
                    //updateLinesNumber2(0x05); 
                    back = testing & 0x00ff;
                    updateLinesNumber2(back); // displays the same w, trying to get c
                    updateLinesNumber2(0xA3); //**************************
                }// array3 will be back of rover 
            }
                
            //pixyData.state = error_restart; // stop here
            pixyData.state = obstacle1_xyExtract;
            updateLinesNumber2(0x44);
        }
            break; /////////////////////////////////////////////////////////////////////////////
        case obstacle1_xyExtract: // get out the x and y according to the colors
        {
            updateLinesNumber2(0x55);
            if (pixyData.roundcount < 4 ){// will have 4 rounds of data
                
                    pixyData.obstacle1_x = pixyData.array1[1] + pixyData.obstacle1_x; // x coordinate
                    pixyData.obstacle1_y = pixyData.array1[2] + pixyData.obstacle1_y; // y coordinate 
                
                    pixyData.frontLead_x = pixyData.array4[1] + pixyData.frontLead_x; // x coordinate
                    pixyData.frontLead_y = pixyData.array4[2] + pixyData.frontLead_y; // y coordinate 
                    pixyData.leadAngle = pixyData.array4[5] + pixyData.leadAngle; // angle of rotation
                    
                    pixyData.frontFollower_x = pixyData.array5[1] + pixyData.frontFollower_x; // x coordinate
                    pixyData.frontFollower_y = pixyData.array5[2] + pixyData.frontFollower_y; // y coordinate 
                    pixyData.followerAngle = pixyData.array5[5] + pixyData.followerAngle; // angle of rotation 
                    
                    pixyData.topLeft_x = pixyData.array6[1] + pixyData.topLeft_x; // x coordinate 
                    pixyData.topLeft_y = pixyData.array6[2] + pixyData.topLeft_y; // y coordinate 
                    
                    pixyData.topRight_x = pixyData.array7[1] + pixyData.topRight_x; // x coordinate 
                    pixyData.topRight_y = pixyData.array7[2] + pixyData.topRight_y; // y coordinate  
                    
                    pixyData.bottomLeft_x = pixyData.array8[1] + pixyData.bottomLeft_x; // x coordinate 
                    pixyData.bottomLeft_y = pixyData.array8[2] + pixyData.bottomLeft_y; // y coordinate
                    
                    pixyData.bottomRight_x = pixyData.array9[1] + pixyData.bottomRight_x;  // x coordinate 
                    pixyData.bottomRight_y = pixyData.array9[2] + pixyData.bottomRight_y; // y coordinate 
             
                pixyData.roundcount ++; // increment everytime we get data 
                updateLinesNumber2(0x89); // represents round count 
                updateLinesNumber2(pixyData.roundcount);// update how many times it has gone through state 
                pixyData.state =  get_nonzeros; // go back to collecting data 
                // changed the above to get_nonzeros, was originally get_zeros    !!! makes the weird skipping if i chagne it to nonzeros
            }
            else if (pixyData.roundcount == 4){ // 5 values in the arrays 
                
                    pixyData.obstacle1_x = pixyData.array1[1] + pixyData.obstacle1_x; // x coordinate
                    pixyData.obstacle1_y = pixyData.array1[2] + pixyData.obstacle1_y; // y coordinate 
                
                    pixyData.frontLead_x = pixyData.array4[1] + pixyData.frontLead_x; // x coordinate
                    pixyData.frontLead_y = pixyData.array4[2] + pixyData.frontLead_y; // y coordinate 
                    pixyData.leadAngle = pixyData.array4[5] + pixyData.leadAngle; // angle of rotation
                    
                    pixyData.frontFollower_x = pixyData.array5[1] + pixyData.frontFollower_x; // x coordinate
                    pixyData.frontFollower_y = pixyData.array5[2] + pixyData.frontFollower_y; // y coordinate 
                    pixyData.followerAngle = pixyData.array5[5] + pixyData.followerAngle; // angle of rotation 
                    
                    pixyData.topLeft_x = pixyData.array6[1] + pixyData.topLeft_x; // x coordinate 
                    pixyData.topLeft_y = pixyData.array6[2] + pixyData.topLeft_y; // y coordinate 
                    
                    pixyData.topRight_x = pixyData.array7[1] + pixyData.topRight_x; // x coordinate 
                    pixyData.topRight_y = pixyData.array7[2] + pixyData.topRight_y; // y coordinate  
                    
                    pixyData.bottomLeft_x = pixyData.array8[1] + pixyData.bottomLeft_x; // x coordinate 
                    pixyData.bottomLeft_y = pixyData.array8[2] + pixyData.bottomLeft_y; // y coordinate
                    
                    pixyData.bottomRight_x = pixyData.array9[1] + pixyData.bottomRight_x;  // x coordinate 
                    pixyData.bottomRight_y = pixyData.array9[2] + pixyData.bottomRight_y; // y coordinate 
                
                
                         //putinWiflyQueue(followerAngle_float);    
                pixyData.state =  obstacle1_calibrate; 
                pixyData.roundcount = 0; //reset the value 
                //debuggingQueue(); ///////////////////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                
            }
            else {
                pixyData.state = error_output; 
            }
     
            updateLinesNumber2(0x66);
        }
            break; ////////////////////////////////////////////////////////////////////////////////
        case xyCheck: // the totaled data of five readings 
        {
        
            pixyData.state = error_restart;
        }
            break; ///////////////////////////////////////////////////////////////////////////////////
        case obstacle1_calibrate:// finish taking the average and have to calibrate 
        {
             updateLinesNumber2(0x77);
                    //putinWiflyQueue(leadAngle_float); 
                    //putinWiflyQueue(followerAngle_float);
                    //putinWiflyQueue(obstacle1_x_float); 
                    //putinWiflyQueue(obstacle1_y_float);
             
             //debuggingQueue(); ///////////////////////////////////!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            //putinWiflyQueue(obstacle1_x_float); 
        pixyData.topLeft_x /= 5.0; // this is (0,0)
        pixyData.topLeft_y /= 5.0;
        
        pixyData.topRight_x /= 5.0;
        pixyData.topRight_y /= 5.0;
        
        pixyData.bottomLeft_x /= 5.0;
        pixyData.bottomLeft_y /= 5.0;
        
        pixyData.bottomRight_x /= 5.0; 
        pixyData.bottomRight_y /= 5.0; 
        /////////////////////////////////////////////////////////////
        pixyData.leadAngle /= 5.0;
        pixyData.followerAngle /= 5.0; 
        
        pixyData.obstacle1_x /= 5.0; 
        pixyData.obstacle1_y /= 5.0;
        
        pixyData.frontLead_x /= 5.0; 
        pixyData.frontLead_y /= 5.0;
        
        pixyData.frontFollower_x /= 5.0;
        pixyData.frontFollower_y /= 5.0;
        
        calculateAngle(); 
        
        //calculateCoordinates(); 
        calculateCoordincatesWithAngles(); 
        
        //putinWiflyQueue(followerAngle_float);// case1
        //putinWiflyQueue(leadAngle_float);
        sendObstacle1(); // case2
        
          updateLinesNumber2(0x88); //gets here fine 
          pixyData.state = inputQueue;
        }
            break; //////////////////////////////////////////////////////////////////////////////////
        case inputQueue: // for testing stuff
        {
            updateLinesNumber2(0x9A);
            //finishedObstactle1(); 
            //finishedLeadRover(); 
            //finishedFollowerRover(); 
            //debuggingQueue(); 
            resetData(); 
            pixyData.lastw = pixyData.w;
            //pixyData.w = 0x00; 
            updateLinesNumber2(0xBC);
            pixyData.state = get_nonzeros; // do the cycle again?? but have to check 
            //^ put the above to nonxeros instead of get_zeros
            
        }
            break; /////////////////////////////////////////////////////////////////////////////////////////
        case error_restart:
        {
            updateLinesNumber2(0xDD);// 
            //pixyData.state = error_restart;//??
            pixyData.state = get_nonzeros; // restart because not enough objects found
            //^ put the above to nonxeros instead of get_zeros
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
