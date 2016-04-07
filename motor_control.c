#include "motor_control.h"
#include "timers.h"

#define TOP_SPEED_QUAD_OUTPUT 525  //in one sec, wheel is moving 4cm/sec (basespeed), then the excepted quad output is 525 
#define TOPSPEED 8 //highest speed 8cm/sec 
#define SECSPERDEGREE 15
#define INCHESPERSEC 375 

//Global Variables 
int rightsetpoint = 0;
int leftsetpoint = 0;
int updated_right_PWM = 0;
int updated_left_PWM = 0;
const double kp = 10;
const double ki = .6;
const double kd = .2;
double right_integral = 0;
double left_integral = 0;
double right_previouserror = 0;
double left_previouserror = 0;
double left_error = 0;
double right_error = 0;
double right_derivative = 0;
double left_derivative = 0;
int turningtime = 100; 
int distancetime = 100; 
int stop = 0;



int rR() {
    SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
}

int rF() {
    SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_C, PORTS_BIT_POS_14);
}

int lR() {
    SYS_PORTS_PinSet(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
}

int lF() {
    SYS_PORTS_PinClear(PORTS_ID_0, PORT_CHANNEL_G, PORTS_BIT_POS_1);
}

void Rightquadencoder(void) {
    T5CON = 0x0; // Stop Timer and clear control register
    T5CONSET = 0x0002; // Set prescaler at 1:1, external clock source
    TMR5 = 0x0; // Clear timer register
    PR5 = 0xFFFF; // Load period register
    T5CONSET = 0x8000; // Start Timer 
    //setting external clock pin input, T3CK
    SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_C, PORTS_BIT_POS_4);
}

void Leftquadencoder(void) {
    T4CON = 0x0; // Stop Timer and clear control register
    T4CONSET = 0x0002; // Set prescaler at 1:1, external clock source
    TMR4 = 0x0; // Clear timer register
    PR4 = 0xFFFF; // Load period register
    T4CONSET = 0x8000; // Start Timer 
    //setting external clock pin input, T4CK
    SYS_PORTS_PinDirectionSelect(PORTS_ID_0, SYS_PORTS_DIRECTION_INPUT, PORT_CHANNEL_C, PORTS_BIT_POS_3);
}

double velocitytoquadticks(double velocity) {
    return (TOP_SPEED_QUAD_OUTPUT / 10) * velocity / TOPSPEED; //per 10th of a second 
    //10 is the number of times per second the pid controller is called 
}

void turn_right_encoder(void) {
    stop = 0;
    lF();
    rR();
    int basespeed = 4;
    double inner_ratio;
    double outer_ratio;
    int radius = 10;
    int rover_length = 10; //cm
    inner_ratio = (radius + rover_length / 2.0) / radius;
    outer_ratio = (radius - rover_length / 2.0) / radius;
    rightsetpoint = velocitytoquadticks(inner_ratio * basespeed);
    leftsetpoint = velocitytoquadticks(outer_ratio * basespeed);
}

void turn_left_degrees(int degrees) {
    stop = 0;
    lR();
    rF();
    int basespeed = 4;
    double inner_ratio;
    double outer_ratio;
    int radius = 10;
    int rover_length = 10; //cm 
    //setTurningTime(SECSPERDEGREE * degrees);
    turningtime = (SECSPERDEGREE * degrees); 
    inner_ratio = (radius + rover_length / 2.0) / radius;
    outer_ratio = (radius - rover_length / 2.0) / radius;
    rightsetpoint = velocitytoquadticks(inner_ratio * basespeed);
    leftsetpoint = velocitytoquadticks(outer_ratio * basespeed);
   
}
void turn_right_degrees(int degrees)
{
    stop = 0;
    lF();
    rR();
    int basespeed = 4;
    double inner_ratio;
    double outer_ratio;
    int radius = 10;
    int rover_length = 10; //cm
    turningtime = (SECSPERDEGREE * degrees); 
    inner_ratio = (radius + rover_length / 2.0) / radius;
    outer_ratio = (radius - rover_length / 2.0) / radius;
    rightsetpoint = velocitytoquadticks(inner_ratio * basespeed);
    leftsetpoint = velocitytoquadticks(outer_ratio * basespeed);
     
}
void turn_left_encoder(void) {
    stop = 0;
    lR();
    rF();
    int basespeed = 4;
    double inner_ratio;
    double outer_ratio;
    int radius = 10;
    int rover_length = 10; //cm
    inner_ratio = (radius + rover_length / 2.0) / radius;
    outer_ratio = (radius - rover_length / 2.0) / radius;
    rightsetpoint = velocitytoquadticks(outer_ratio * basespeed);
    leftsetpoint = velocitytoquadticks(inner_ratio * basespeed);
}

void stop_rover(void) {
    stop = 1;
   lF();
   rF();
   rightsetpoint = 0;
   leftsetpoint = 0;
    PLIB_OC_PulseWidth16BitSet(0, 0);
    PLIB_OC_PulseWidth16BitSet(1, 0);
    rightsetpoint = velocitytoquadticks(0);
    leftsetpoint = velocitytoquadticks(0);
}

void move_rover_forward(void) {
    stop = 0;
    lR();
    rR();
    int basespeed = 4;
    double inner_ratio;
    double outer_ratio;
    int radius = 10;
    int rover_length = 10; //cm
    inner_ratio = (radius + rover_length / 2.0) / radius;
    outer_ratio = (radius - rover_length / 2.0) / radius;
    rightsetpoint = velocitytoquadticks(outer_ratio * basespeed);
    leftsetpoint = velocitytoquadticks(outer_ratio * basespeed);
}

void move_rover_forward_distance(int inch) {
    stop = 0;
    lR();
    rR();
    int basespeed = 4;
    double inner_ratio;
    double outer_ratio;
    int radius = 10;
    int rover_length = 10; //cm
    distancetime = (INCHESPERSEC * inch);
    inner_ratio = (radius + rover_length / 2.0) / radius;
    outer_ratio = (radius - rover_length / 2.0) / radius;
    rightsetpoint = velocitytoquadticks(outer_ratio * basespeed);
    leftsetpoint = velocitytoquadticks(outer_ratio * basespeed);
    
}
void move_rover_backward_distance(int inch) {
    stop = 0;
    lF();
    rF();
    int basespeed = 4;
    double inner_ratio;
    double outer_ratio;
    int radius = 10;
    int rover_length = 10; //cm
    distancetime = (INCHESPERSEC * inch);
    inner_ratio = (radius + rover_length / 2.0) / radius;
    outer_ratio = (radius - rover_length / 2.0) / radius;
    rightsetpoint = velocitytoquadticks(outer_ratio * basespeed);
    leftsetpoint = velocitytoquadticks(outer_ratio * basespeed);
    
}

void move_rover_backwards(void) {
    stop = 0;
    lF();
    rF();
    int basespeed = 4;
    double inner_ratio;
    double outer_ratio;
    int radius = 10;
    int rover_length = 10; //cm
    inner_ratio = (radius + rover_length / 2.0) / radius;
    outer_ratio = (radius - rover_length / 2.0) / radius;
    rightsetpoint = velocitytoquadticks(outer_ratio * basespeed);
    leftsetpoint = velocitytoquadticks(outer_ratio * basespeed);

}

void PID_Controller(void) {
    int right_quad = TMR4;
    int left_quad = TMR5;
    int dt = 1;
    double right_output;
    double left_output;

    right_error = rightsetpoint - right_quad;
    left_error = leftsetpoint - left_quad;

    right_integral = right_integral + right_error*dt;
    left_integral = left_integral + left_error*dt;
    right_derivative = (right_error - right_previouserror) / dt;
    left_derivative = (left_error - left_previouserror) / dt;
    right_output = kp * right_error + ki * right_integral + kd*right_derivative;
    left_output = kp * left_error + ki * left_integral + kd*left_derivative;
    right_previouserror = right_error;
    left_previouserror = left_error;
    updated_right_PWM += right_output;
    updated_left_PWM += left_output;
    if (updated_right_PWM < 0) {
        updated_right_PWM = 0;
    }
    if (updated_left_PWM < 0) {
        updated_left_PWM = 0;
    }

    PLIB_OC_PulseWidth16BitSet(0, updated_right_PWM);
    PLIB_OC_PulseWidth16BitSet(1, updated_left_PWM);
    if(stop)
    {
    PLIB_OC_PulseWidth16BitSet(0, 0);
    PLIB_OC_PulseWidth16BitSet(1, 0);    
    }
    TMR4 = 0;
    TMR5 = 0;

}
void Turningtime()
{
    turningtime-=100;
    static int onlyOnce = 1;
    if (turningtime < 0 && onlyOnce)
    {
        addQSnd('H');addQSnd('H');addQSnd('H');addQSnd('H');addQSnd('H');addQSnd('H');addQSnd('H');addQSnd('H'); 
       // move_rover_forward();
        stop_rover(); 
        onlyOnce = 0;
    }else if(turningtime > 0) onlyOnce = 1;
}
void Distancetime()
{
    distancetime-=100; 
    static int onlyOnce = 1;
    if (distancetime < 0 && onlyOnce)
    {
        addQSnd('C');addQSnd('C');addQSnd('C');addQSnd('C');addQSnd('C');addQSnd('C');addQSnd('C');addQSnd('C');
      //turn_right_degrees(90); 
       //turn_left_encoder();
        stop_rover(); 
       onlyOnce = 0;
    }else if(distancetime > 0) onlyOnce = 1;
}