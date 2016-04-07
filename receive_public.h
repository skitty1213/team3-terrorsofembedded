#ifndef RECEIVE_PUBLIC_H
#define	RECEIVE_PUBLIC_H

#include <xc.h> 

#ifdef	__cplusplus
extern "C" {
#endif /* __cplusplus */

    // Public declaration for adding to receive queue
    int addQRcv(char letter);
    void updateClock();
   
    
#ifdef	__cplusplus
}
#endif /* __cplusplus */

#endif	/* RECEIVE_PUBLIC_H */

