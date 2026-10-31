//=============================================================================
// Modulo net.h
//=============================================================================

#ifndef __NETLIB_H
#define __NETLIB_H

//-----------------------------------------------------------------------------
// Type definitions
//-----------------------------------------------------------------------------
typedef unsigned short WORD;
typedef unsigned char  BYTE;

typedef struct { int device;         // IPX, SERIAL or MODEM
                 int com;                 // 0-3
                 int speed;           // Port speed
                 char phone[32];
                 char string_start[16];  // Default: ATZ
                 int tones;               // 1-tone, 0-pulse
                 int server;            //
                 int num_players;         // Maximum 16
               } net_struct;

//-----------------------------------------------------------------------------
// Device definitions
//-----------------------------------------------------------------------------
#define IPX     1
#define SERIAL  2
#define MODEM   3

//-----------------------------------------------------------------------------
// Error definitions
//-----------------------------------------------------------------------------
#define NET_ABORTED       -1
#define NET_BADINIT       -2
#define NET_BADNUMPLAYERS -3
#define NET_BADSYNC       -4

//-----------------------------------------------------------------------------
// Definition of exported variables
//-----------------------------------------------------------------------------
extern char net_games[10][32];
extern int net_players[16];
extern net_struct *net_lib;
extern int network_initialization;

//-----------------------------------------------------------------------------
// Procedure definitions
//-----------------------------------------------------------------------------

// End the communication
void net_end(void);

// Return the active games on the network
int _net_get_games(int game_id);
void net_get_games(void);

// If the game does not exist, create it, otherwise join it
int _net_join_game(int game_id,char *name,byte *data, int length);
void net_join_game(void);

// Check the network (send and receive packets)
void _net_loop();

#endif
