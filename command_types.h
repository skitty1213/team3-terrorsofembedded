#ifndef _COMMAND_TYPES_H    /* Guard against multiple inclusion */
#define _COMMAND_TYPES_H 

/* Provide C++ Compatibility */
#ifdef __cplusplus
extern "C" {
#endif

#define LEAD_POSITION 0x10
#define OBS_1_POSITION 0x11
#define OBS_2_POSITION 0x12
#define OBS_3_POSITION 0x13
#define ACK_SENSOR 0x14

#define DISTANCE_MOVED 0x20
#define TOKEN_FOUND 0x21
#define AWAITING_INSTRUCTION 0x22
#define ACK_LEAD 0x24

#define TOKEN_RETRIEVED 0x40
#define ACK_FOLLOW 0x44

#define MOVE_FORWARD 0x70
#define TURN 0x71
#define STOP_LEAD 0x72
#define END_SEARCH 0x73
#define ACK_COORD 0x74
#define MOVE_BACK 0x75

#define INITIAL_SETUP 0x00

#define DEBUG_FORWARD 0x60
#define DEBUG_TURN 0x61
#define DEBUG_STOP 0x62
#define DEBUG_BACK 0x65

#define TURN_LEFT 0x42
#define TURN_RIGHT 0x43
#define TURN_RIGHT_DEGREES 0x45
#define MOVE_FORWARDON 0x46
#define MOVE_BACKWARD 0x47    


    /* Provide C++ Compatibility */
#ifdef __cplusplus
}
#endif

#endif /* _COMMAND_TYPES_H */

/* *****************************************************************************
 End of File
 */
