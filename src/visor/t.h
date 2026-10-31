#ifndef __T_H_
#define __T_H_

//─────────────────────────────────────────────────────────────────────────────
//  Assembly funcs (need converting to c)
//─────────────────────────────────────────────────────────────────────────────

//extern "C" {

void kernel8_8(int v, int u, int du, int dv, int width, short *destination);
void kernel8_16(int v, int u, int du, int dv, int width, short *destination);
void kernel8_32(int v, int u, int du, int dv, int width, short *destination);
void kernel8_64(int v, int u, int du, int dv, int width, short *destination);
void kernel8_128(int v, int u, int du, int dv, int width, short *destination);
void kernel8_256(int v, int u, int du, int dv, int width, short *destination);

void mask_kernel8_8(int v, int u, int du, int dv, int width, short *destination);
void mask_kernel8_16(int v, int u, int du, int dv, int width, short *destination);
void mask_kernel8_32(int v, int u, int du, int dv, int width, short *destination);
void mask_kernel8_64(int v, int u, int du, int dv, int width, short *destination);
void mask_kernel8_128(int v, int u, int du, int dv, int width, short *destination);
void mask_kernel8_256(int v, int u, int du, int dv, int width, short *destination);

//};

#endif
