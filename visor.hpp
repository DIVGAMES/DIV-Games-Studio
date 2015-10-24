#ifndef __VISOR_HPP_
#define __VISOR_HPP_

typedef struct _buffer_sprite3d {
  int   ancho,alto;
  int   punto_x,punto_y;
  char  *buffer;
} buffer_sprite3d;

typedef struct {
  int porcentaje;       // 100 % es el tama¤o normal
  float alfa;           // Angulo de deriva (0-360)
  float beta;           // Angulo de caida (0-90)
  char *animacion;      // Puntero al nombre de la animacion (incluye path)
  int num_animacion;    // Numero de la animacion
  int frame;            // Frame a renderizar
  int num_frames;       // Frames totales de la animacion
  char *nombre;         // Puntero al nombre del modelo (incluye path)
  char *dac;            // Puntero a la paleta
  char *textura;        // Puntero al nombre de la textura (incluye path)
} tvisor, *lptvisor;

buffer_sprite3d *visor_init(lptvisor datos_render);
void visor_loop(lptvisor datos_render);
void visor_end();

#endif
