#ifndef __COMUNICA_H_
#define __COMUNICA_H_

//#include "IPXLIB.H"
//#include "COMLIB.H"

#define MAX_PACK 22
#define MAX_USERS 512

// Dispositivos
#define IPX 1
#define SERIAL 2
#define MODEM 3

// Errores
#define NET_INITOK 0
#define NET_ABORTED -1
#define NET_BADINIT -2
#define NET_BADNUMPLAYERS -3
#define NET_BADSYNC -4

typedef struct {
	int activa;
	int dispositivo;
	int servidor;
	char nombre[32];
	int player_id;
	int game_id;
	int num_players;
	int max_players;
	byte *datos;
	int longitud_datos;
} net_game;

extern char net_games[10][32];
extern int net_players[16];
extern net_game partida_red;
extern int quien_soy;

void net_get_games(void);
void net_join_game(void);

int _net_get_games(int game_id);
int _net_join_game(int game_id, char *nombre, byte *datos, int longitud);

/*- Mensajes publicos internos ----*/
#define MSG_SYNC 10001
#define MSG_OK 10002
#define MSG_ENDSYNC 10003
#define MSG_YOUARELIGHTSERVER 10004
#define MSG_NEWLIGHTSERVER 10005
#define MSG_OLDSERVER 10006
#define MSG_RETRY 10007
#define MSG_DATOS 10008
#define MSG_HANGUP 10009
#define MSG_EXIT 10010
#define MSG_GETCOURT 10012
#define MSG_GETSETS 10013
#define MSG_WANTPLAY 10014
#define MSG_SCORE 10015
#define MSG_STATS 10016
#define MSG_ACK 10017
#define MSG_SETJUGACTIVO 10018
#define MSG_WRONGSYNC 10019
#define MSG_PAUSE 10020
#define MSG_ENDPAUSE 10021
#define MSG_GAME 10022
#define MSG_PAQUETE 10023
#define MSG_TANTEO 10024

// Mensajes internos
#define MSG_OKSYNC 10070

#endif
