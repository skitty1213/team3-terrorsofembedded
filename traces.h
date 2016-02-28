#ifndef TRACES_H
#define	TRACES_H

#ifdef	__cplusplus
extern "C" {
#endif

#define RECEIVE_INIT_ENTER              0
#define RECEIVE_INIT_EXIT               1
#define RECEIVE_TASKS_ENTER             2
#define RECEIVE_TASKS_BEFORE_QUEUE      3
#define RECEIVE_TASKS_AFTER_QUEUE       4
#define CHECK_RECEIVE_ENTER             5
#define CHECK_RECEIVE_START_CHAR        6
#define CHECK_RECEIVE_WORD_DONE         7
#define CHECK_RECEIVE_WORD_EXCEED       8
#define CHECK_RECEIVE_WORD_EXIT         9
#define UPDATE_CLOCK                    10
#define ADD_Q_RCV_ENTER                 11
#define ADD_Q_RCV_EXIT                  12
#define SEND_INIT_ENTER                 13
#define SEND_INIT_EXIT                  14
#define ADD_Q_SND_ENTER                 15
#define ADD_Q_SND_EXIT                  16
#define ASSEMBLE_SEND_ENTER             17
#define ASSEMBLE_SEND_EXIT              18
#define SEND_TASKS_ENTER                19
#define SEND_TASKS_BEFORE_QUEUE         20
#define SEND_TASKS_AFTER_QUEUE          21
#define TIMER_CALLBACK_ENTER            22
#define TIMER_CALLBACK_EXIT             23
#define ISR_ENTER                       24
#define ISR_EXIT                        25

#ifdef	__cplusplus
}
#endif

#endif	/* TRACES_H */