#ifndef SOCKET_H_
#define SOCKET_H_

#include "tftpServerProtocol.h"



/*************************/
/* Function declarations */
/*************************/

/* function name: ConfigureSocket
 * params:
 * ------
 * portNumber - the port number to open the UDP socket on for listening
 *  
 * return value 
 * ------------
 * return value is 0 on success
 * */
int ConfigureSocket(int portNumber);

/* function name: ReceiveFromSocket
 * params:
 * ------
 * o_RecvPacket - The function will fill this structure with the lastest data received
 *
 * return value 
 * ------------
 * return value - length of the receive packet, <= 0 on failure
 * */
int ReceiveFromSocket(tftpPacket *o_RecvPacket);



#endif /* #ifndef SOCKET_H_ */