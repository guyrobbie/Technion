#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/socket.h>
#include "main.h"
#include "Socket.h"
#include "Debug.h"
#include "tftpServerProtocol.h"

/***********/
/* Externs */
/***********/

extern int                  g_ListeningSocket;
extern struct sockaddr_in   g_Si_Other;
extern int                  g_Slen;


/***********/
/* Globals */
/***********/

int                         fileDescriptor;
char                        *filename = NULL;
unsigned int                lastReceivedBlockNumber = 0;
unsigned int                currentBlockNumber = 0;



/*************************/
/* Function definitions  */
/*************************/

/* function name: SendAck
 * params:
 * ------
 * blockNumber - the block number to be filled in the Ack message
 *  
 * return value 
 * ------------
 * return value is 0 on success
 * */

int SendAck(unsigned int blockNumber)
{
    int i;
    char sendBuffer[ACK_MSG_LEN] = {0};
    
    // Prepare the ACK 0 message to be sent
    sendBuffer[0] = 0;
    sendBuffer[1] = OPCODE_ACK;
    sendBuffer[2] = (blockNumber & 0xFF00)>>8;
    sendBuffer[3] = blockNumber & 0xFF;
    
    for( i = 0; i < 4 ; i++ )
        INFO(" sendBuffer[%d] = %x %c", i, sendBuffer[i], sendBuffer[i]);
    
    // Sends the Ack message
    if (sendto(g_ListeningSocket, &sendBuffer, sizeof(sendBuffer), 0, (struct sockaddr*) &g_Si_Other, g_Slen) == -1)
    {
        return -1;
    }
    printf("OUT:ACK, %d\r\n", blockNumber);
    return 0;
}


/* function name: ReceiveFile
 * params:
 * ------
 * 
 *  
 * return value 
 * ------------
 * return value is 0 on success
 * */

int ReceiveFile(void)
{
    int             ret, recv_len;
    bool            lastDataPacketArrived = false, timeOutExpired = false;
    unsigned int    dataChunkLen = 0;
    unsigned int    timeoutExpiredCount = 0;
    tftpPacket      recvPacket;
    fd_set          rfds;
    struct timeval  tv;
    
    /* Initialize the file descriptor set */
    FD_ZERO(&rfds);
    FD_SET(g_ListeningSocket, &rfds);
    /* Wait up to timeout seconds for packet to arrive. */
    tv.tv_sec = WAIT_FOR_PACKET_TIMEOUT;
    tv.tv_usec = 0;    

    do
    {
        
        /* description  : block number |  data   */
        /* content      :    bytes     |  bytes  */
        /* length(bytes):      2       | 512 max */
        
        do
        {
            // Wait WAIT_FOR_PACKET_TIMEOUT to see if something appears
            // for us at the socket (we are waiting for DATA)
            if((ret = select(g_ListeningSocket + 1, &rfds, NULL, NULL, &tv)) == -1)
            {
                die("select()");
            }

            if(ret)                     /* FD_ISSET(0, &rfds) will be true. */
            {
                timeOutExpired = false;
            }
            else                        /* No data within 3 seconds */
            {
                timeOutExpired = true;
            }

            if (!timeOutExpired)     // TODO: if there was something at the socket and
            {                        // we are here not because of a timeout

                // TODO: Read the DATA packet from the socket (at
                // least we hope this is a DATA packet)
                recv_len = ReceiveFromSocket(&recvPacket);
            }
            else    //Time out expired while waiting for data to appear at the socket
	    {
             	//Send another ACK for the last packet   
                timeoutExpiredCount++;                
                
                // TODO: send ACK packet to the client
                ret = SendAck(currentBlockNumber);
                if(ret == -1)
                {
                    die("Failed in SendAck: when calling sendto()");
                }
            }
            
            if (timeoutExpiredCount>= NUMBER_OF_FAILURES)
            {
                // FATAL ERROR BAIL OUT
                ERROR("failed to receive file due to timeout");
                
                // Prints FLOWERROR message
                printf("FLOWERROR: timeout expired %d times, while waiting for the file\n", NUMBER_OF_FAILURES);

		// Delete the file
	        ret = close(fileDescriptor);
		if(ret != 0)
                {
                    die("Failed to close the file, close():");
                }
		ret = remove(filename);
		if(ret != 0)
                {
                    die("Failed to remove the file, remove():");
                }

                return -1;
            }
	// Continue while some socket was ready but recvfrom somehow failed to read the data
        }while ((recv_len <= 0) && !timeOutExpired); 
        
	if(timeOutExpired)
	{
	    continue;
	}

        // TODO: We got something else but DATA
        if (recvPacket.opcode[1] != OPCODE_DATA) 
        {
            // FATAL ERROR BAIL OUT
            ERROR("not a DATA packet");

            // Prints FLOWERROR message
            printf("FLOWERROR: received message is not DATA message, received opcode = 0x%x\n",recvPacket.opcode[1]);

      	    // Delete the file
	        ret = close(fileDescriptor);
	        if(ret != 0)
	        {
	            die("Failed to close the file, close():");
	        }
	        ret = remove(filename);
	        if(ret != 0)
	        {
	            die("Failed to remove the file, remove():");
	        }

            return -1;
        }

        TRACE("Received OPCODE_DATA packet");
    
        currentBlockNumber = (recvPacket.buffer[0]<<8) | recvPacket.buffer[1];
        
        dataChunkLen = recv_len - (OPCODE_LEN + BLOCK_NUM_LEN);
        
        printf("IN:DATA, %d, %d\r\n", currentBlockNumber, dataChunkLen);
        
        // Check is the current block number is equal to lastReceivedBlockNumber + 1
        if (currentBlockNumber != ( lastReceivedBlockNumber + 1)) 	// TODO: The incoming block number is not what we have
                    // expected, i.e. this is a DATA pkt but the block number
                    // in DATA was wrong (not last ACKs block number + 1)
        {
            // FATAL ERROR BAIL OUT
            ERROR("currentBlockNumber is different than lastReceivedBlockNumber + 1");
            
            // Prints FLOWERROR message
            printf("FLOWERROR: current block number is different than last block number + 1\n");

	        // Delete the file
	        ret = close(fileDescriptor);
	        if(ret != 0)
	        {
	            die("Failed to close the file, close():");
	        }
	        ret = remove(filename);
	        if(ret != 0)
	        {
	            die("Failed to remove the file, remove():");
	        }

            return -1;
        }
        
        // Update receiveBlockCounter
        lastReceivedBlockNumber = currentBlockNumber;
        
        // Reset timeout expired counter
        timeoutExpiredCount = 0;

        // Write to file only if there is actual data to write, in case it is empty DATA message we don't write
        if(dataChunkLen > 0)
        {
            ret = write(fileDescriptor, &recvPacket.buffer[BLOCK_NUM_LEN], dataChunkLen);
            if(ret == -1)
            {
                die("write()");
            }
            
            // Prints message to terminal about writing to the file
            printf("WRITING: %d\n", dataChunkLen);
        }

        // TODO: send ACK packet to the client
        ret = SendAck(currentBlockNumber);
        if(ret == -1)
        {
            die("Failed in SendAck: when calling sendto()");
        }

        // Deallocate buffer
        free(recvPacket.buffer);
        
        // Check if it is the last data buffer (of current received file)
        if(dataChunkLen < DATA_LEN )
        {
            // close file
            ret = close(fileDescriptor);
            if(ret == -1)
            {
                die("write()");
            }
            TRACE("Last data packet arrived, len of data = %d", dataChunkLen);
            lastDataPacketArrived = true;
        }


    // Have blocks left to be read from client (not end of transmission)        
    }while (!lastDataPacketArrived); 
    
    return 0;
}


/* function name: ParseWriteRequest
 * params:
 * ------
 * recvPacket - packet to be parse
 *  
 * return value 
 * ------------
 * return value is 0 on success
 * */
int ParseWriteRequest(tftpPacket *recvPacket)
{
    int             ret;
    unsigned int    filenameLen = 0;
    unsigned int    transmissionModeLen = 0;
    wrq             wrq_packet;
    
    if(recvPacket->opcode[0] != 0)
    {
        return -1;
    }
        
    if( recvPacket->opcode[1] != OPCODE_WRQ)
    {
        return -1;
    }    
    
    /* description  : filename | str terminator | trans mode | str terminator */
    /* content      :  string  |         0      |   string   |        0       */
    /* length(bytes):  unknown |         1      |   unknown  |        1       */
    
    TRACE("Received OPCODE_WRQ packet");

    filenameLen = strlen(recvPacket->buffer) + 1;
    
    // Allocate and copy the file name to create
    filename = (char*)malloc(filenameLen);
    strcpy(filename, recvPacket->buffer);
    wrq_packet.filename = filename;
    
    INFO(" %s ", wrq_packet.filename);
    
    // Allocate and copy the transmission mode string
    transmissionModeLen = strlen(&recvPacket->buffer[filenameLen]) + 1;
    wrq_packet.transmissionMode = (char*)malloc(transmissionModeLen);
    strcpy(wrq_packet.transmissionMode, &recvPacket->buffer[filenameLen]);
    
    INFO(" %s ", wrq_packet.transmissionMode);
    
    // Prints terminal message for the incomming WRQ packet
    printf("IN:WRQ, %s, %s\r\n", wrq_packet.filename, wrq_packet.transmissionMode);
    
    // Reseting block number counting
    currentBlockNumber = 0;
    
    // Creates file with the file name
    fileDescriptor = open(filename, FILE_PERMISSIONS, FILE_MODES);
    if(fileDescriptor == -1)
    {
        die("open()");
    }

    ret = SendAck(currentBlockNumber);
    if(ret == -1)
    {
        die("Failed in SendAck: when calling sendto()");
    }
    
    // Deallocate buffers
    free(recvPacket->buffer);
    free(wrq_packet.transmissionMode);
    return 0;
}
