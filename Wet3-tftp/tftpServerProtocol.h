#ifndef TFTP_SERVER_PROTOCOL_H_
#define TFTP_SERVER_PROTOCOL_H_

#include "Socket.h"

/***********/
/* defines */
/***********/

/* define sanity checks */
#define WAIT_FOR_PACKET_TIMEOUT 3
#define NUMBER_OF_FAILURES      7

/* define tftp packet fields length */
#define OPCODE_LEN          2
#define BLOCK_NUM_LEN       2
#define DATA_LEN            512

/* define UDP fields length */
#define BUFLEN              DATA_LEN + OPCODE_LEN + BLOCK_NUM_LEN //Max length of buffer
#define ACK_MSG_LEN         OPCODE_LEN + BLOCK_NUM_LEN 




/* define Opcodes */
#define OPCODE_WRQ          2
#define OPCODE_DATA         3
#define OPCODE_ACK          4



/********************/
/* Packet's structs */
/********************/

/* tftp packet struct */
typedef struct _tftpPacket
{
    unsigned char opcode[OPCODE_LEN];
    char *buffer;
    
}__attribute__((packed))tftpPacket, *p_tftpPacket;



/* WRQ packet struct */
typedef struct _wrq
{
    char *filename;
    char *transmissionMode;
}__attribute__((packed))wrq, *p_wrq;



/* ACK packet's body struct */
typedef struct _ack
{
    char blockNumber[BLOCK_NUM_LEN];
}__attribute__((packed))ack, *p_ack;



/* DATA packet's body struct */
typedef struct _data
{
    char blockNumber[BLOCK_NUM_LEN];
    char data[DATA_LEN];
}__attribute__((packed))data, *p_data;


/*************************/
/* Function declarations */
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

int SendAck(unsigned int blockNumber);


/* function name: StartReceivingFile
 * params:
 * ------
 * 
 *  
 * return value 
 * ------------
 * return value is 0 on success
 * */

int ReceiveFile(void);


/* function name: ParseWriteRequest
 * params:
 * ------
 * recvPacket - packet to be parse
 *  
 * return value 
 * ------------
 * return value is 0 on success
 * */
int ParseWriteRequest(tftpPacket *recvPacket);


#endif /* #ifndef TFTP_SERVER_PROTOCOL_H_ */