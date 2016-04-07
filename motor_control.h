/* 
 * File:   motor_control.h
 * Author: willb
 *
 * Created on February 29, 2016, 4:21 PM
 */
#ifndef MOTOR_CONTROL_H
#define	MOTOR_CONTROL_H


#include "motors.h"
#include "send.h"
#include "receive.h"

#ifdef	__cplusplus
extern "C" {
#endif


    void Rightquadencoder(void);
    void Leftquadencoder(void);
    void PID_Controller(void);
    void Turningtime();
    void Distancetime(); 
  // int move_rover_forward_distance(int inch);
  // void turn_left_degrees(int degrees);
    //void move_rover_forward(void);
    // void turn_right_encoder(void);
    //void turn_left_encoder(void);
    //void stop_rover(void);
    //void move_rover_backwards(void);

#ifdef	__cplusplus
}
#endif

#endif	/* MOTOR_CONTROL_H */

