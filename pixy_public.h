/* ************************************************************************** */
/** Descriptive File Name

  @Company
    Company Name

  @File Name
    filename.h

  @Summary
    Brief description of the file.

  @Description
    Describe the purpose of this file.
 */
/* ************************************************************************** */

#ifndef PIXY_PUBLIC_H    /* Guard against multiple inclusion */
#define PIXY_PUBLIC_H

#include "pixy.h"

/* This section lists the other files that are included in this file.
 */

/* TODO:  Include other files here if needed. */


/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

    /*typedef struct _example_struct_t {
       
        int some_number;

        bool some_flag;

    } example_struct_t;*/


   int addQ_fromPxy(int16_t coord);
   uint16_t getWord(void); 
   uint8_t getByte(void); 
   void sendtoServer(void); 
   void calculateAngle(void); 
   void calculateCoordinates (void); 
   void debuggingQueue(void);
   void resetData(void);
   void putinWiflyQueue(int8_t number);
   //void outputVariableData(void);
   //void getByte(void); 
   //uint8_t word; 


    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _EXAMPLE_FILE_NAME_H */

/* *****************************************************************************
 End of File
 */
