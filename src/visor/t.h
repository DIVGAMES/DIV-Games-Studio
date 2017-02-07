#ifndef __T_H_
#define __T_H_

//─────────────────────────────────────────────────────────────────────────────
//  Assembly funcs (need converting to c)
//─────────────────────────────────────────────────────────────────────────────

//extern "C" {

void nucleo8_8(int v, int u, int du, int dv, int ancho, short *destino);
void nucleo8_16(int v, int u, int du, int dv, int ancho, short *destino);
void nucleo8_32(int v, int u, int du, int dv, int ancho, short *destino);
void nucleo8_64(int v, int u, int du, int dv, int ancho, short *destino);
void nucleo8_128(int v, int u, int du, int dv, int ancho, short *destino);
void nucleo8_256(int v, int u, int du, int dv, int ancho, short *destino);

void mask_nucleo8_8(int v, int u, int du, int dv, int ancho, short *destino);
void mask_nucleo8_16(int v, int u, int du, int dv, int ancho, short *destino);
void mask_nucleo8_32(int v, int u, int du, int dv, int ancho, short *destino);
void mask_nucleo8_64(int v, int u, int du, int dv, int ancho, short *destino);
void mask_nucleo8_128(int v, int u, int du, int dv, int ancho, short *destino);
void mask_nucleo8_256(int v, int u, int du, int dv, int ancho, short *destino);

//};

#endif
