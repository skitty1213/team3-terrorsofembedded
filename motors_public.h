/* 
 * File:   motors_public.h
 * Author: Pooja
 *
 * Created on March 24, 2016, 10:41 PM
 */

#ifndef MOTORS_PUBLIC_H
#define	MOTORS_PUBLIC_H

#ifdef	__cplusplus
extern "C" {
#endif

 // Public declaration for adding to motors queue
    int addQmotors(char letter);
    void move_rover_forward(void);
    void move_rover_backwards(void);
    void turn_right_encoder(void); 
    void turn_left_encoder(void); 
    void stop_rover(void);
    void turn_left_degrees(int degrees);
    void turn_right_degrees(int degrees);
    void move_rover_forward_distance(int inch);
    void move_rover_backward_distance(int inch);

#ifdef	__cplusplus
}
#endif

#endif	/* MOTORS_PUBLIC_H */

