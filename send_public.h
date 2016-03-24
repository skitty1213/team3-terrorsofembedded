#ifndef SEND_PUBLIC_H
#define	SEND_PUBLIC_H

#include <xc.h> 

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // Public declaration for adding to send queue
    int addQSnd(char letter);
    
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* SEND_PUBLIC_H */

