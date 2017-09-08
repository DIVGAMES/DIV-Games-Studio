//=============================================================================
// Modulo net.h
//=============================================================================

#ifndef __NET_H
#define __NET_H

#ifdef __cplusplus
extern "C" {
#endif

//-----------------------------------------------------------------------------
// Definicion de tipos
//-----------------------------------------------------------------------------
typedef unsigned short WORD;
typedef unsigned char  BYTE;

//-----------------------------------------------------------------------------
// Definicion de Dispositivos
//-----------------------------------------------------------------------------
#define IPX     1
#define SERIAL  2
#define MODEM   3

//-----------------------------------------------------------------------------
// Definicion de errores
//-----------------------------------------------------------------------------
#define NET_ABORTED -1 // esc
#define NET_BADINIT -2
#define NET_TIMEOUT -3
#define NET_BADSYNC -4

//-----------------------------------------------------------------------------
// Definicion de variables exportadas
//-----------------------------------------------------------------------------
extern int inicializacion_red; // Hay que inicializarla a cero

//-----------------------------------------------------------------------------
// Definicion de procedimientos externos
//-----------------------------------------------------------------------------
// Rutina para la recepcion de paquetes como servidor
extern void (*SRV_Packet)(WORD Usuario, WORD Comando, BYTE *Buffer, WORD Len);

// Rutina para la recepcion de paquetes como Nodo
extern void (*NOD_Packet)(WORD Usuario, WORD Comando, BYTE *Buffer, WORD Len);

//-----------------------------------------------------------------------------
// Definicion de procedimientos propios
//-----------------------------------------------------------------------------
// Inicializa el protocolo ipx (devuelve error o número de nodo)
int net_init_ipx(int socket, int num_jugadores, int timeout);
// socket=identificador de partida(int), 2..8, segundos (1..60)

// ojo con el key(esc) de aborción
// Inicializa el modem (devuelve error o número de nodo)
int net_init_modem(int puerto, int velocidad, int dial, char *cadena_inicio, char *cadena_llamada, char *telefono, int timeout);

// Inicializa el cable serie (devuelve error o número de nodo)
int net_init_serial(int puerto, int velocidad, int timeout);

// Manda un paquete
void net_send(int direccion, char *offset, int size);

// direccion (de mem), offset (puntero), size en bytes

// Recibe un paquete
void net_receive();

// Finaliza la comunicacion
void net_end();

#ifdef __cplusplus
}
#endif
#endif
