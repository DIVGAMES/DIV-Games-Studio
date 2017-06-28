//=============================================================================
// Modulo net.h
//=============================================================================

#ifndef __NETLIB_H
#define __NETLIB_H

//-----------------------------------------------------------------------------
// Definicion de tipos
//-----------------------------------------------------------------------------
typedef unsigned short WORD;
typedef unsigned char  BYTE;

typedef struct _net_struct {
	int  dispositivo;       // IPX, SERIAL ¢ MODEM
	int  com;               // 0-3
	int  velocidad;         // Velocidad del puerto
	char telefono[32];
	char cadena_inicio[16]; // Defecto: ATZ
	int  tonos;             // 1-tonos, 0-pulsos
	int  servidor;          //
	int  num_players;       // Maximo 16
} t_net_struct;

//-----------------------------------------------------------------------------
// Definicion de Dispositivos
//-----------------------------------------------------------------------------
#define IPX     1
#define SERIAL  2
#define MODEM   3

//-----------------------------------------------------------------------------
// Definicion de errores
//-----------------------------------------------------------------------------
#define NET_ABORTED       -1
#define NET_BADINIT       -2
#define NET_BADNUMPLAYERS -3
#define NET_BADSYNC       -4

//-----------------------------------------------------------------------------
// Definicion de variables exportadas
//-----------------------------------------------------------------------------
extern char net_games[10][32];
extern int net_players[16];
extern t_net_struct *net_lib;
extern int inicializacion_red;

//-----------------------------------------------------------------------------
// Definicion de procedimientos
//-----------------------------------------------------------------------------

// Finaliza la comunicacion
void net_end(void);

// Devuelve las partidas activas en la red
int  net_get_games(int game_id);

// Si la partida no existe la crea en caso contrario se une a ella
int  net_join_game(int game_id, char *nombre, byte *datos, int longitud);

// Chequea la red (manda y recibe paquetes)
void net_loop();

void DIV_net_get_games(void);
void DIV_net_join_game(void);

#endif
