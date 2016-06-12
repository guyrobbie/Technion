#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/select.h> 
#include <sys/time.h> 
#include <sys/types.h>
#include <sys/wait.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "Debug.h"
#include "main.h"
#include "Socket.h"
#include "tftpServerProtocol.h"

/***********/
/* Externs */
/***********/

extern int                  g_ListeningSocket;
extern struct sockaddr_in   g_Si_Me, g_Si_Other;
extern int                  g_Slen;


/**************/
/* Main entry */
/**************/

int main(int argc, char **argv)
{
    int             ret, portNumber; 
    tftpPacket      recvPacket;
    
    if(argc != 2)
    {
        printf("usage: ./serverTftp <port number>\r\n");
        exit(1);
    }
 
    // extracting the port number
    portNumber = atoi(argv[1]);
    
    // configure the listening socket
    ret = ConfigureSocket(portNumber);
    if(ret)
    {
        die("ConfigureSocket() failed to open socket");
    }
    
    //keep listening for data
    while(1)
    {
        TRACE("Waiting for data...");
        
        ret = ReceiveFromSocket(&recvPacket);
        if(ret <= 0)
        {
            continue;
        }
        // parse incomming message
        ret = ParseWriteRequest(&recvPacket);
        if(ret != 0)
        {
            continue;
        }
        
        // Write request received, start receiving the file
        ret = ReceiveFile();
        if(ret != 0)
        {
            // Prints message to terminal about unsuccessful receiving of the file
            printf("RECVFAIL\n");
            continue;
        }
        // Prints message to terminal about successful receiving of the file
        printf("RECVOK\n");
        
    }

    close(g_ListeningSocket);
	return 0;
}