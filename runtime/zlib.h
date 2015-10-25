#ifndef __Z_LIB_
#define __Z_LIB_

// Rutina de compresi¢n, llamar con destLen=sourceLen*0.1+12
// Devuelve 0 si no hay ning£n error.

int compress    (unsigned char *dest,   unsigned long *destLen,
                 unsigned char *source, unsigned long sourceLen);

// Rutina de descompresi¢n, se debe conocer destLen (longitud de los
// datos descomprimidos). Devuelve 0 si no hay ning£n error.
// Mejor no hacer mucho caso al valor de destLen a la salida, ya que
// no se si devuelve la longitud de los datos descomprimidos (lo que val¡a
// en la llamada) o de los comprimidos (es decir, sourceLen).

int uncompress  (unsigned char *dest,   unsigned long *destLen,
                 unsigned char *source, unsigned long sourceLen);

#endif
