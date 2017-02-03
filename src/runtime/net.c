//=============================================================================
//  Modulo central de la libreria de red
//  Programado por: Luis F. Fernandez
//=============================================================================

#ifdef NETPLAY
#include <stdio.h>
#include <string.h>
#include <time.h>
#if SDL == 1
#include <SDL/SDL_net.h>
#else
#include <SDL2/SDL2_net.h>
#endif
#include "inter.h"
#include "net.h"

IPaddress ipaddr;
TCPsocket tcpsock,new_tcpsock;

#define NET_TIMEOUT 1
#define NET_NOTINIT -1

char net_games[10][32];
int net_players[16];

int inicializacion_red=0;

net_game partida_red;

extern int reloj;

//=============================================================================
//  Procedimientos internos
//=============================================================================

int net_init(int game_id);
int _net_join_game(int game_id,char *nombre,byte *datos, int longitud);
int join_game(int num_games);
void net_end(void);
void net_exit_game(void);
int net_init_ipx(int game_id);
int net_init_internet(int game_id);

int net_init_serial(int game_id);
void net_send(void);
void net_recv(void);
void net_get_games(void);

int net_create_game(int game_id,char *nombre, int num_jugadores);
int old_net_reloj;

extern void arse(void);

//=============================================================================
// Inicializa la comunicaci¢n
//=============================================================================

int net_init(int game_id)
{
  old_net_reloj=reloj;
  fprintf(stdout,"Starting NETPLAY\n");
  
  return(net_init_internet(game_id));
  
/*  switch (net->dispositivo)
  {
    case IPX:     return(net_init_ipx(game_id));
                  break;
    case SERIAL:  return(net_init_serial(game_id));
                  break;
    case MODEM:   return(net_init_modem(5));
                  break;
  }
  * 
  * */
  e(162);
  return(-1);
}

int net_init_internet(int game_id) {
	inicializacion_red=1;
	partida_red.game_id=game_id;
	partida_red.activa=0;
	partida_red.servidor=0;
	partida_red.player_id=0;
	return SDLNet_Init();
}


//=============================================================================
// Inicializa la comunicaci¢n a traves de ipx
//=============================================================================

#ifdef NOTYET
int net_init_ipx(int game_id)
{
  int socket=27350;

  //---------------------------------------------------------------------------
  // Compruebo si se inici¢ la comunicacion, en caso afirmativo se finaliza
  //---------------------------------------------------------------------------
  if (inicializacion_red)
    net_end();

  //---------------------------------------------------------------------------
  // Inicializa el socket
  //---------------------------------------------------------------------------
  if( !InitIPX(socket) )
  {
    return( NET_BADINIT );
  }

  //---------------------------------------------------------------------------
  // Selecciona el dispositivo
  //---------------------------------------------------------------------------
  InitECBs();
  partida_red.dispositivo=IPX;

  //---------------------------------------------------------------------------
  // Inicializaci¢n correcta
  //---------------------------------------------------------------------------
  inicializacion_red=1;
  partida_red.game_id=game_id;
  partida_red.activa=0;
  partida_red.servidor=0;
  partida_red.player_id=0;
  return(0);
}

//=============================================================================
// Inicializa la comunicaci¢n a traves de cable serie
//=============================================================================

int net_init_serial(int game_id)
{
  //---------------------------------------------------------------------------
  // Compruebo si se inici¢ la comunicacion, en caso afirmativo se finaliza
  //---------------------------------------------------------------------------
  if (inicializacion_red)
    net_end();

  partida_red.num_players=2;

  //---------------------------------------------------------------------------
  // Inicializa el puerto
  //---------------------------------------------------------------------------
  if (InitCOM(net->com,net->velocidad)) {
    return(NET_BADINIT);
  }

  //---------------------------------------------------------------------------
  // Selecciona el dispositivo
  //---------------------------------------------------------------------------
  partida_red.dispositivo=SERIAL;

  //---------------------------------------------------------------------------
  // Inicializaci¢n correcta
  //---------------------------------------------------------------------------
  inicializacion_red=1;
  partida_red.game_id=game_id;
  partida_red.activa=0;
  partida_red.servidor=0;
  COM_Send(1,MSG_SYNC,"SYNC",4);
  return(0);
}

//=============================================================================
// Inicializa la comunicaci¢n a traves de modem
//=============================================================================

int net_init_modem(int timeout)
{
char data_aux[64];
int dial=0;//????????? FERNANDO ?????
/*
  if ( net->telefono[0]==NULL )     // OJO //OKFER
    dial=0;
  else
    dial=1;
*/
  if (net->tonos)
    strcpy(data_aux,"ATDT");
  else
    strcpy(data_aux,"ATDP");

//  strcat(data_aux,net->telefono);

  //---------------------------------------------------------------------------
  // Compruebo si se inici¢ la comunicacion, en caso afirmativo se finaliza
  //---------------------------------------------------------------------------
  if (inicializacion_red)
    net_end();

  partida_red.num_players=2;
  timeout*=CLOCKS_PER_SEC;

  //---------------------------------------------------------------------------
  // Inicializa el puerto
  //---------------------------------------------------------------------------
  if (InitCOM(net->com,net->velocidad)) {
    return(NET_BADINIT);
  }

  //---------------------------------------------------------------------------
  // Inicializa el modem
  //---------------------------------------------------------------------------
  partida_red.servidor=dial;  // El que marca es uno y el que recibe la llamada 0.
                  // (El servidor espera, el nodo llama)
/*
//  InitModem("ATZ", "ATDT5197575");
  if (InitModem(net->cadena_inicio,data_aux)) {
    return(NET_BADINIT);
  }
*/
  //---------------------------------------------------------------------------
  // Selecciona el dispositivo
  //---------------------------------------------------------------------------
  partida_red.dispositivo=MODEM;

  //---------------------------------------------------------------------------
  // Inicializaci¢n correcta
  //---------------------------------------------------------------------------
  inicializacion_red=1;
  return(partida_red.servidor);
}

//=============================================================================
// Control de paquetes (para servidores)
//=============================================================================

#endif

void _net_loop(void)
{
  static int con_reloj=0;

  //---------------------------------------------------------------------------
  // Compruebo si se inici¢ la comunicacion, en caso contrario retorna
  //---------------------------------------------------------------------------
  if (!inicializacion_red)
    return;

  //---------------------------------------------------------------------------
  // Compruebo si hay una partida activa, en caso contrario retorna
  //---------------------------------------------------------------------------
  if (!partida_red.activa)
    return;

  //---------------------------------------------------------------------------
  // Manda un paquete
  //---------------------------------------------------------------------------
  net->servidor=partida_red.servidor;
  net->tonos=partida_red.game_id;
  con_reloj+=(reloj-old_net_reloj);
  old_net_reloj=reloj;
//  if(con_reloj>2)
//  {
//    con_reloj-=2;
    net_send();
  net_recv();

//  }
  
  
#ifdef NOTYET
  //---------------------------------------------------------------------------
  // Chequea los paquetes en la red
  //---------------------------------------------------------------------------
  if (partida_red.dispositivo==IPX)
  {
    do {
    } while (CheckPackets());
//    CheckPackets();
  }
  else
  {
    COM_CheckPacket();
  }
  
#endif
  net->act_players=partida_red.num_players;
  //---------------------------------------------------------------------------
  // Si la partida ya est  completa retorna
  //---------------------------------------------------------------------------
  if (partida_red.num_players==partida_red.max_players) {
    return;
  }
}

//=============================================================================
//  Finaliza la red
//=============================================================================

void net_end()
{
  if (!inicializacion_red)
    return;

  net_exit_game();

  SDLNet_Quit();
#ifdef NOTYET
  if (partida_red.dispositivo==IPX)
    EndIPX();
  else
    EndCOM();
#endif
  inicializacion_red=0;
}

//=============================================================================
// Manda un paquete
//=============================================================================

void net_send()
{
	int len=partida_red.longitud_datos;
//	byte *msg[len];
	int result;
/*
  int i;

  if (partida_red.servidor)
    printf("Jugadores: %d, NumPlayer: %d, Dir: %s, Soy el Servidor\n",partida_red.num_players,partida_red.player_id,ShowInternalAddress(&IPX_MyAddress));
  else
    printf("Jugadores: %d, NumPlayer: %d, Dir: %s, Soy un Nodo\n",partida_red.num_players,partida_red.player_id,ShowInternalAddress(&IPX_MyAddress));
*/
  if (!partida_red.activa || (partida_red.num_players<2 && partida_red.dispositivo==IPX))
    return;
  
//  msg[0]=partida_red.player_id;
//  memcpy(&msg[1],partida_red.datos,partida_red.longitud_datos);
  
  // construct the packet


  if(partida_red.servidor==1 && new_tcpsock)
	result = SDLNet_TCP_Send(new_tcpsock,partida_red.datos,partida_red.longitud_datos);
  else
    result = SDLNet_TCP_Send(tcpsock,partida_red.datos,partida_red.longitud_datos);
  
  if(result<len) {
    //fprintf(stdout,"SDLNet_TCP_Send: %s\n", SDLNet_GetError());
    // It may be good to disconnect sock because it is likely invalid now.
}
#ifdef NOTYET
  if (partida_red.dispositivo==IPX) {
    SendPacketToAll(partida_red.player_id,(void *)((int)partida_red.datos+partida_red.player_id*partida_red.longitud_datos),partida_red.longitud_datos);
  }
  else {
    COM_Send(partida_red.servidor,0,partida_red.datos,partida_red.longitud_datos);
  }
#endif

}

//=============================================================================
// Crea una partida
//=============================================================================

int net_create_game(int game_id,char *nombre, int num_jugadores)
{
	fprintf(stdout,"NET_CREATE_GAME %d %s %d\n",game_id,nombre, num_jugadores);
	
  //---------------------------------------------------------------------------
  // Compruebo si se inici¢ la comunicacion, en caso contrario retorna
  //---------------------------------------------------------------------------
  if (!inicializacion_red)
    net_init(game_id);

  //---------------------------------------------------------------------------
  // Compruebo si hay una partida activa, en caso afirmativo retorna
  //---------------------------------------------------------------------------
  if (partida_red.activa)
    return(-1);

  if (num_jugadores<0 || num_jugadores>=MAX_USERS)
    return(NET_BADNUMPLAYERS);

  //---------------------------------------------------------------------------
  // Compruebo si el n£mero de jugadores es correcto, en caso contrario retorna
  //---------------------------------------------------------------------------
#ifdef NOTYET
  if (partida_red.dispositivo==IPX) {
    if (num_jugadores<0 || num_jugadores>=MAX_USERS)
      return(NET_BADNUMPLAYERS);
  }
  else {
    if (num_jugadores!=2)
      return(NET_BADNUMPLAYERS);
  }
#endif
  strcpy(partida_red.nombre,nombre);
  partida_red.activa=1;
  partida_red.servidor=1;
  net->servidor=1;
  partida_red.max_players=num_jugadores;
  fprintf(stdout,"Starting server\n");
if(SDLNet_ResolveHost(&ipaddr,NULL,9999)==-1) {
    printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
    return 0;
}

tcpsock=SDLNet_TCP_Open(&ipaddr);
if(!tcpsock) {
    printf("SDLNet_TCP_Open: %s\n", SDLNet_GetError());
    return -1;
}

//  if (partida_red.dispositivo==IPX)
//    InsertIn(&IPX_MyAddress,0);

  //---------------------------------------------------------------------------
  // Inicializaci¢n correcta
  //---------------------------------------------------------------------------
  return 1;
}

//=============================================================================
// Destruye una partida
//=============================================================================

void net_exit_game()
{
  //---------------------------------------------------------------------------
  // Compruebo si se inici¢ la comunicacion, en caso contrario retorna
  //---------------------------------------------------------------------------
  if (!inicializacion_red)
    return;
  if (!partida_red.activa)
    return;

  partida_red.activa=0;

  //---------------------------------------------------------------------------
  // Enviar a los otros usuarios el mensaje de salida
  //---------------------------------------------------------------------------
#ifdef NOTYET
  if (partida_red.dispositivo==IPX) {
    ipx_exit_game();
  }
  else {
    if (partida_red.servidor)
      COM_Send(1,MSG_EXIT,"",0);
    else
      COM_Send(0,MSG_EXIT,"",0);
  }
#endif
}

//=============================================================================
// Busca las partidas activas
//=============================================================================

int _net_get_games(int game_id)
{
  int num_games;
  int valor_retorno;

  //---------------------------------------------------------------------------
  // Compruebo si se inicio la comunicacion, en caso contrario la inicia
  //---------------------------------------------------------------------------
  if (!inicializacion_red) {
    if ((valor_retorno=net_init(game_id))<0)
      return(valor_retorno);
  }

#ifdef NOTYET
  if (partida_red.dispositivo==IPX) {
    num_games=looking_for_games(1);
    return(num_games);
  }
#endif
	if(SDLNet_ResolveHost(&ipaddr,"localhost",9999)==-1) {
		printf("SDLNet_ResolveHost: %s\n", SDLNet_GetError());
		exit(1);
	}

	tcpsock=SDLNet_TCP_Open(&ipaddr);

	if(!tcpsock) {
		fprintf(stdout,"SDLNet_TCP_Open: %s\n", SDLNet_GetError());
		return 0;
	}
  fprintf(stdout,"connected to %x\n",tcpsock);
  partida_red.activa=1;
  return(1);
}

//=============================================================================
// Entra en una de las partidas activas
//=============================================================================

int _net_join_game(int game_id,char *nombre,byte *datos, int longitud)
{
int num_games,i;
int valor_retorno;
fprintf(stdout,"Net Join Game\n");
int server=0;
  //---------------------------------------------------------------------------
  // Comprueba el numero de jugadores
  //---------------------------------------------------------------------------
  if (net->num_players<=0 || net->num_players>16)
  {
    e(149);
    return(-5);
  }

  //---------------------------------------------------------------------------
  // Inicializa la tabla de jugadores
  //---------------------------------------------------------------------------
  for (i=0;i<net->num_players;i++) {
    net_players[i]=0;
  }
  partida_red.num_players=0;

  //---------------------------------------------------------------------------
  // Comprueba que le paquete no supere los 200 bytes
  //---------------------------------------------------------------------------
// FER_AQUI
  longitud*=4;
//  longitud/=net->num_players;  
  if (longitud>=200)
  {
    e(163);
    return(-6); // Paquete demasiado grande
  }

  //---------------------------------------------------------------------------
  // Compruebo si se inici¢ la comunicacion, en caso contrario retorna
  //---------------------------------------------------------------------------
  if (!inicializacion_red) {
    if ((valor_retorno=net_init(game_id))<0)
      return(valor_retorno);
  }
  if (partida_red.activa) {
    net_exit_game();
    return(0);
  }

  partida_red.datos=datos;
  partida_red.longitud_datos=longitud;
server = net_create_game(game_id,nombre,net->num_players);

if(server==1)
return 0;

if(_net_get_games(game_id)>0) {
	return(join_game(num_games));
}
  //---------------------------------------------------------------------------
  // Busca si existe la partida
  //---------------------------------------------------------------------------
  if (num_games=_net_get_games(game_id)) {
    for (i=0;i<num_games;i++) {
      if (!stricmp(net_games[i],nombre)) {
#ifdef NOTYET
        if (partida_red.dispositivo==IPX)
          return(join_game(num_games));
        else
#endif
          return 1;
      }
    }
  }
//  return(net_create_game(game_id,nombre,net->num_players));
}



#define text_offset mem[7] // Start of text segment (mem[] index)
int                 net_game_id;

//-----------------------------------------------------------------------------
//  Inicializacion del identificador de juego
//-----------------------------------------------------------------------------

void net_create_game_id(void)
{
  int i;
  net_game_id=0;

  for (i=0;i<9;i++) {
    net_game_id+=mem[i];
  }
  net_game_id=abs(net_game_id);
  if (net_game_id<10500)
    net_game_id+=10500;

}

//-----------------------------------------------------------------------------
//  Entra en una de las partidas activas
//-----------------------------------------------------------------------------

void net_join_game(void)
{
int game_id;
int c;
int longitud=pila[sp--];
int datos   =pila[sp--];
int nombre  =pila[sp];

//fprintf(stdout,"datos: %x\n",datos);

  game_id=net_game_id+nombre+mem[text_offset+nombre];
  	partida_red.game_id=game_id;
	//      partida_red.num_players=game_info.num_players;
//	partida_red.max_players=max_players;
//	partida_red.player_id=player_id;
	partida_red.activa=1;


  c=_net_join_game( game_id, (char *)&mem[text_offset+nombre], (byte *)&mem[datos], longitud );
  pila[sp]=c;
  
}

//-----------------------------------------------------------------------------
//  Entra en una de las partidas activas
//-----------------------------------------------------------------------------

void net_get_games(void)
{
int c;
  c=_net_get_games(net_game_id);
  pila[++sp]=c;
}

int join_game(int num_games) {
	return 1;
}

void net_recv(void) {
	#define MAXLEN partida_red.longitud_datos
	int result=0;
	byte msg[MAXLEN];

	if ( partida_red.servidor) {
		
		if(!new_tcpsock) {
			new_tcpsock=SDLNet_TCP_Accept(tcpsock);
			printf("SDLNet_TCP_Accept: %s\n", SDLNet_GetError());
		} else {
//			if(new_tcpsock!=NULL)
			// && !SDLNet_SocketReady(tcpsock)) 
				result=SDLNet_TCP_Recv(new_tcpsock,msg,MAXLEN);
			
			if(result!=MAXLEN) {
		// An error may have occured, but sometimes you can just ignore it
		// It may be good to disconnect sock because it is likely invalid now.
				return;
			}
		}
	} else {
	//	if(!SDLNet_SocketReady(tcpsock))
			result=SDLNet_TCP_Recv(tcpsock,msg,MAXLEN);
		if(result!=MAXLEN) {
	// An error may have occured, but sometimes you can just ignore it
	// It may be good to disconnect sock because it is likely invalid now.
			return;
		}
	}
//	fprintf(stdout,"Received: %d %d\"%s\"\n",result,MAXLEN,msg);
//	memset(partida_red.datos,1,result);
	memcpy(partida_red.datos,msg,result);
}
#endif

