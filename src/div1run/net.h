//=============================================================================
// Modulo net.h
//=============================================================================

#ifndef __NET_H
#define __NET_H

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef unsigned short WORD;
typedef unsigned char  BYTE;

//-----------------------------------------------------------------------------
// Device definitions
//-----------------------------------------------------------------------------
#define IPX     1
#define SERIAL  2
#define MODEM   3

//-----------------------------------------------------------------------------
// Error definitions
//-----------------------------------------------------------------------------
#define NET_ABORTED -1 // esc
#define NET_BADINIT -2
#define NET_TIMEOUT -3
#define NET_BADSYNC -4

//-----------------------------------------------------------------------------
// Definition of exported variables
//-----------------------------------------------------------------------------
extern int network_initialization; // It must be initialized to zero

//-----------------------------------------------------------------------------
// Definition of external procedures
//-----------------------------------------------------------------------------

// Routine for receiving packets as server
extern void (*SRV_Packet)(WORD User,WORD Command,BYTE *Buffer,WORD Len);

// Routine for receiving packets as Node
extern void (*NOD_Packet)(WORD User,WORD Command,BYTE *Buffer,WORD Len);

//-----------------------------------------------------------------------------
// Definition of own procedures
//-----------------------------------------------------------------------------

// Initialize the ipx protocol (returns error or node number)
int net_init_ipx(int socket,int num_players, int timeout);
// socket=game identifier(int), 2..8, seconds (1..60)

// careful with the abort key(esc)

// Initialize the modem (returns error or node number)
int net_init_modem(int port, int speed, int dial, char *string_start,
                   char *string_call, char *phone, int timeout);

// Initialize the serial cable (returns error or node number)
int net_init_serial(int port, int speed, int timeout);

// Send a packet
void net_send(int direction, char *offset, int size);

// address (in mem), offset (pointer), size in bytes

// Receive a packet
void net_receive();

// End the communication
void net_end();

#ifdef __cplusplus
}
#endif
#endif
