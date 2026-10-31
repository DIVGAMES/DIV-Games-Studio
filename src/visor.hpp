#ifndef __VIEWER_HPP_
#define __VIEWER_HPP_

typedef struct _buffer_sprite3d {
  int   width,height;
  int   point_x,point_y;
  char  *buffer;
} buffer_sprite3d;

typedef struct {
  int percentage;       // 100 % es el tamaño normal
  float alpha;           // Angulo de deriva (0-360)
  float beta;           // Angulo de caida (0-90)
  char *animation;      // Puntero al nombre de la animacion (incluye path)
  int num_animation;    // Numero de la animacion
  int frame;            // Frame a renderizar
  int num_frames;       // Frames totales de la animacion
  char *name;         // Puntero al nombre del modelo (incluye path)
  char *dac;            // Puntero a la paleta
  char *texture;        // Puntero al nombre de la textura (incluye path)
} tviewer, *lptviewer;

buffer_sprite3d *viewer_init(lptviewer render_data);
void viewer_loop(lptviewer render_data);
void viewer_end();

#endif
