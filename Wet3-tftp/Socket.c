#include <sys/socket.h>
#include <stdbool.h>
#include <string.h>
#include <arpa/inet.h>
#include "Debug.h"
#include "Socket.h"


int                 g_ListeningSocket;
struct sockaddr_in  g_Si_Me, g_Si_Other;
int                 g_Slen = sizeof(g_Si_Other);


/*************************/
/* Function definitions  */
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
int ConfigureSocket(int portNumber)
{
    //create a UDP socket
    if ((g_ListeningSocket=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        return -1;
    }
     
    // zero out the structure
    memset((char *) &g_Si_Me, 0, sizeof(g_Si_Me));
     
    g_Si_Me.sin_family = AF_INET;
    g_Si_Me.sin_port = htons(portNumber);
    g_Si_Me.sin_addr.s_addr = htonl(INADDR_ANY);
     
    //bind socket to port
    if( bind(g_ListeningSocket , (struct sockaddr*)&g_Si_Me, sizeof(g_Si_Me) ) == -1)
    {
        return -1;
    }
    
    return 0;
}


/* function name: ReceiveFromSocket
 * params:
 * ------
 * o_RecvPacket - The function will fill this structure with the lastest data received
 *
 * return value 
 * ------------
 * return value - length of the receive packet, <= 0 on failure
 * */
int ReceiveFromSocket(tftpPacket *o_RecvPacket)
{
    int     recv_len, i;
    char    buf[BUFLEN];

    fflush(stdout);
    
    //try to receive some data, this is a blocking call
    if ((recv_len = recvfrom(g_ListeningSocket, buf, BUFLEN, 0, (struct sockaddr *) &g_Si_Other, (socklen_t *)&g_Slen)) == -1)
    {
        return -1;
        //die("recvfrom()");
    }
    
    //print details of the client/peer and the data received
    TRACE("Received packet from %s:%d", inet_ntoa(g_Si_Other.sin_addr), ntohs(g_Si_Other.sin_port));
    
    if(buf == NULL)
    {
        return -1;
        //die("buf is NULL");
    }
    
    // prints the received buf
    for( i = 0; i < recv_len ; i++ )
        INFO(" buf[%d] = %x %c", i,buf[i], buf[i]);
    
    // copy opcode
    memcpy(o_RecvPacket->opcode, buf, OPCODE_LEN);
    TRACE("o_RecvPacket->opcode = %x %x", o_RecvPacket->opcode[0], o_RecvPacket->opcode[1]);
    
    // Allocate space for buffer to copy the rest of the packet data 
    o_RecvPacket->buffer = (char*)malloc(recv_len - OPCODE_LEN);
    memcpy(o_RecvPacket->buffer, &buf[OPCODE_LEN], recv_len - OPCODE_LEN);
    
    // prints the the rest of the packet data 
    for( i = 0; i < recv_len - OPCODE_LEN ; i++ )
        INFO(" o_RecvPacket->buffer[%d] = %x %c", i, o_RecvPacket->buffer[i], o_RecvPacket->buffer[i]);
        
    return recv_len;
}
