//=============================================================================
//  Modulo central de la libreria de red
//  Programado por: Luis F. Fernandez
//=============================================================================

#ifdef NETPLAY
#include <stdio.h>
#include <string.h>
#include <time.h>
#include "SDL_net.h"
#include "inter.h"
#include "net.h"

IPaddress ipaddr;
TCPsocket tcpsock,new_tcpsock;

#define NET_TIMEOUT 1
#define NET_NOTINIT -1

char net_games[10][32];
int net_players[16];

int network_initialization=0;

net_game network_game;

extern int clock_ticks;

//=============================================================================
//  Procedimientos internos
//=============================================================================

int net_init(int game_id);
int _net_join_game(int game_id,char *name,byte *data, int length);
int join_game(int num_games);
void net_end(void);
void net_exit_game(void);
int net_init_ipx(int game_id);
int net_init_internet(int game_id);

int net_init_serial(int game_id);
void net_send(void);
void net_recv(void);
void net_get_games(void);

int net_create_game(int game_id,char *name, int num_players);
int old_net_clock;

extern void arse(void);

//=============================================================================
// Inicializa la comunicación
//=============================================================================

int net_init(int game_id)
{
  old_net_clock=clock_ticks;
  fprintf(stdout,"Starting NETPLAY\n");
  
  return(net_init_internet(game_id));
  
/*  switch (net->device)
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
	network_initialization=1;
	network_game.game_id=game_id;
	network_game.active=0;
	network_game.server=0;
	network_game.player_id=0;
	return SDLNet_Init();
}


//=============================================================================
// Inicializa la comunicación a traves de ipx
//=============================================================================

#ifdef NOTYET
int net_init_ipx(int game_id)
{
  int socket=27350;

  //---------------------------------------------------------------------------
  // Compruebo si se inició la comunicacion, en caso afirmativo se finaliza
  //---------------------------------------------------------------------------
  if (network_initialization)
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
  network_game.device=IPX;

  //---------------------------------------------------------------------------
  // Inicialización correcta
  //---------------------------------------------------------------------------
  network_initialization=1;
  network_game.game_id=game_id;
  network_game.active=0;
  network_game.server=0;
  network_game.player_id=0;
  return(0);
}

//=============================================================================
// Inicializa la comunicación a traves de cable serie
//=============================================================================

int net_init_serial(int game_id)
{
  //---------------------------------------------------------------------------
  // Compruebo si se inició la comunicacion, en caso afirmativo se finaliza
  //---------------------------------------------------------------------------
  if (network_initialization)
    net_end();

  network_game.num_players=2;

  //---------------------------------------------------------------------------
  // Inicializa el puerto
  //---------------------------------------------------------------------------
  if (InitCOM(net->com,net->speed)) {
    return(NET_BADINIT);
  }

  //---------------------------------------------------------------------------
  // Selecciona el dispositivo
  //---------------------------------------------------------------------------
  network_game.device=SERIAL;

  //---------------------------------------------------------------------------
  // Inicialización correcta
  //---------------------------------------------------------------------------
  network_initialization=1;
  network_game.game_id=game_id;
  network_game.active=0;
  network_game.server=0;
  COM_Send(1,MSG_SYNC,"SYNC",4);
  return(0);
}

//=============================================================================
// Inicializa la comunicación a traves de modem
//=============================================================================

int net_init_modem(int timeout)
{
char data_aux[64];
int dial=0;//????????? FERNANDO ?????
/*
  if ( net->phone[0]==NULL )     // OJO //OKFER
    dial=0;
  else
    dial=1;
*/
  if (net->tones)
    strcpy(data_aux,"ATDT");
  else
    strcpy(data_aux,"ATDP");

//  strcat(data_aux,net->phone);

  //---------------------------------------------------------------------------
  // Compruebo si se inició la comunicacion, en caso afirmativo se finaliza
  //---------------------------------------------------------------------------
  if (network_initialization)
    net_end();

  network_game.num_players=2;
  timeout*=CLOCKS_PER_SEC;

  //---------------------------------------------------------------------------
  // Inicializa el puerto
  //---------------------------------------------------------------------------
  if (InitCOM(net->com,net->speed)) {
    return(NET_BADINIT);
  }

  //---------------------------------------------------------------------------
  // Inicializa el modem
  //---------------------------------------------------------------------------
  network_game.server=dial;  // El que marca es uno y el que recibe la llamada 0.
                  // (El servidor espera, el nodo llama)
/*
//  InitModem("ATZ", "ATDT5197575");
  if (InitModem(net->string_start,data_aux)) {
    return(NET_BADINIT);
  }
*/
  //---------------------------------------------------------------------------
  // Selecciona el dispositivo
  //---------------------------------------------------------------------------
  network_game.device=MODEM;

  //---------------------------------------------------------------------------
  // Inicialización correcta
  //---------------------------------------------------------------------------
  network_initialization=1;
  return(network_game.server);
}

//=============================================================================
// Control de paquetes (para servidores)
//=============================================================================

#endif

void _net_loop(void)
{
  static int with_clock=0;

  //---------------------------------------------------------------------------
  // Compruebo si se inició la comunicacion, en caso contrario retorna
  //---------------------------------------------------------------------------
  if (!network_initialization)
    return;

  //---------------------------------------------------------------------------
  // Compruebo si hay una partida activa, en caso contrario retorna
  //---------------------------------------------------------------------------
  if (!network_game.active)
    return;

  //---------------------------------------------------------------------------
  // Manda un paquete
  //---------------------------------------------------------------------------
  net->server=network_game.server;
  net->tones=network_game.game_id;
  with_clock+=(clock_ticks-old_net_clock);
  old_net_clock=clock_ticks;
//  if(with_clock>2)
//  {
//    with_clock-=2;
    net_send();
  net_recv();

//  }
  
  
#ifdef NOTYET
  //---------------------------------------------------------------------------
  // Chequea los paquetes en la red
  //---------------------------------------------------------------------------
  if (network_game.device==IPX)
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
  net->act_players=network_game.num_players;
  //---------------------------------------------------------------------------
  // Si la partida ya está completa retorna
  //---------------------------------------------------------------------------
  if (network_game.num_players==network_game.max_players) {
    return;
  }
}

//=============================================================================
//  Finaliza la red
//=============================================================================

void net_end()
{
  if (!network_initialization)
    return;

  net_exit_game();

  SDLNet_Quit();
#ifdef NOTYET
  if (network_game.device==IPX)
    EndIPX();
  else
    EndCOM();
#endif
  network_initialization=0;
}

//=============================================================================
// Manda un paquete
//=============================================================================

void net_send()
{
	int len=network_game.data_length;
//	byte *msg[len];
	int result;
/*
  int i;

  if (network_game.server)
    printf("Jugadores: %d, NumPlayer: %d, Dir: %s, Soy el Servidor\n",network_game.num_players,network_game.player_id,ShowInternalAddress(&IPX_MyAddress));
  else
    printf("Jugadores: %d, NumPlayer: %d, Dir: %s, Soy un Nodo\n",network_game.num_players,network_game.player_id,ShowInternalAddress(&IPX_MyAddress));
*/
  if (!network_game.active || (network_game.num_players<2 && network_game.device==IPX))
    return;
  
//  msg[0]=network_game.player_id;
//  memcpy(&msg[1],network_game.data,network_game.data_length);
  
  // construct the packet


  if(network_game.server==1 && new_tcpsock)
	result = SDLNet_TCP_Send(new_tcpsock,network_game.data,network_game.data_length);
  else
    result = SDLNet_TCP_Send(tcpsock,network_game.data,network_game.data_length);
  
  if(result<len) {
    //fprintf(stdout,"SDLNet_TCP_Send: %s\n", SDLNet_GetError());
    // It may be good to disconnect sock because it is likely invalid now.
}
#ifdef NOTYET
  if (network_game.device==IPX) {
    SendPacketToAll(network_game.player_id,(void *)((int)network_game.data+network_game.player_id*network_game.data_length),network_game.data_length);
  }
  else {
    COM_Send(network_game.server,0,network_game.data,network_game.data_length);
  }
#endif

}

//=============================================================================
// Crea una partida
//=============================================================================

int net_create_game(int game_id,char *name, int num_players)
{
	fprintf(stdout,"NET_CREATE_GAME %d %s %d\n",game_id,name, num_players);
	
  //---------------------------------------------------------------------------
  // Compruebo si se inició la comunicacion, en caso contrario retorna
  //---------------------------------------------------------------------------
  if (!network_initialization)
    net_init(game_id);

  //---------------------------------------------------------------------------
  // Compruebo si hay una partida activa, en caso afirmativo retorna
  //---------------------------------------------------------------------------
  if (network_game.active)
    return(-1);

  if (num_players<0 || num_players>=MAX_USERS)
    return(NET_BADNUMPLAYERS);

  //---------------------------------------------------------------------------
  // Compruebo si el número de jugadores es correcto, en caso contrario retorna
  //---------------------------------------------------------------------------
#ifdef NOTYET
  if (network_game.device==IPX) {
    if (num_players<0 || num_players>=MAX_USERS)
      return(NET_BADNUMPLAYERS);
  }
  else {
    if (num_players!=2)
      return(NET_BADNUMPLAYERS);
  }
#endif
  strcpy(network_game.name,name);
  network_game.active=1;
  network_game.server=1;
  net->server=1;
  network_game.max_players=num_players;
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

//  if (network_game.device==IPX)
//    InsertIn(&IPX_MyAddress,0);

  //---------------------------------------------------------------------------
  // Inicialización correcta
  //---------------------------------------------------------------------------
  return 1;
}

//=============================================================================
// Destruye una partida
//=============================================================================

void net_exit_game()
{
  //---------------------------------------------------------------------------
  // Compruebo si se inició la comunicacion, en caso contrario retorna
  //---------------------------------------------------------------------------
  if (!network_initialization)
    return;
  if (!network_game.active)
    return;

  network_game.active=0;

  //---------------------------------------------------------------------------
  // Enviar a los otros usuarios el mensaje de salida
  //---------------------------------------------------------------------------
#ifdef NOTYET
  if (network_game.device==IPX) {
    ipx_exit_game();
  }
  else {
    if (network_game.server)
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
  int return_value;

  //---------------------------------------------------------------------------
  // Compruebo si se inicio la comunicacion, en caso contrario la inicia
  //---------------------------------------------------------------------------
  if (!network_initialization) {
    if ((return_value=net_init(game_id))<0)
      return(return_value);
  }

#ifdef NOTYET
  if (network_game.device==IPX) {
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
  network_game.active=1;
  return(1);
}

//=============================================================================
// Entra en una de las partidas activas
//=============================================================================

int _net_join_game(int game_id,char *name,byte *data, int length)
{
int num_games,i;
int return_value;
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
  network_game.num_players=0;

  //---------------------------------------------------------------------------
  // Comprueba que le paquete no supere los 200 bytes
  //---------------------------------------------------------------------------
// FER_AQUI
  length*=4;
//  length/=net->num_players;  
  if (length>=200)
  {
    e(163);
    return(-6); // Paquete demasiado grande
  }

  //---------------------------------------------------------------------------
  // Compruebo si se inició la comunicacion, en caso contrario retorna
  //---------------------------------------------------------------------------
  if (!network_initialization) {
    if ((return_value=net_init(game_id))<0)
      return(return_value);
  }
  if (network_game.active) {
    net_exit_game();
    return(0);
  }

  network_game.data=data;
  network_game.data_length=length;
server = net_create_game(game_id,name,net->num_players);

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
      if (!stricmp(net_games[i],name)) {
#ifdef NOTYET
        if (network_game.device==IPX)
          return(join_game(num_games));
        else
#endif
          return 1;
      }
    }
  }
//  return(net_create_game(game_id,name,net->num_players));
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
int length=opstack[sp--];
int data   =opstack[sp--];
int name  =opstack[sp];

//fprintf(stdout,"data: %x\n",data);

  game_id=net_game_id+name+mem[text_offset+name];
  	network_game.game_id=game_id;
	//      network_game.num_players=game_info.num_players;
//	network_game.max_players=max_players;
//	network_game.player_id=player_id;
	network_game.active=1;


  c=_net_join_game( game_id, (char *)&mem[text_offset+name], (byte *)&mem[data], length );
  opstack[sp]=c;
  
}

//-----------------------------------------------------------------------------
//  Entra en una de las partidas activas
//-----------------------------------------------------------------------------

void net_get_games(void)
{
int c;
  c=_net_get_games(net_game_id);
  opstack[++sp]=c;
}

int join_game(int num_games) {
	return 1;
}

void net_recv(void) {
	#define MAXLEN network_game.data_length
	int result=0;
	byte msg[MAXLEN];

	if ( network_game.server) {
		
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
//	memset(network_game.data,1,result);
	memcpy(network_game.data,msg,result);
}
#endif

