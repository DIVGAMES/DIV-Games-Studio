#ifndef __VIEWER_HPP_
#define __VIEWER_HPP_

typedef struct _buffer_sprite3d {
  int   width,height;
  int   point_x,point_y;
  char  *buffer;
} buffer_sprite3d;

typedef struct {
  int percentage;       // 100 % is the normal size
  float alpha;           // Drift angle (0-360)
  float beta;           // Fall angle (0-90)
  char *animation;      // Pointer to the name of the animation (includes path)
  int num_animation;    // Animation number
  int frame;            // Frame to render
  int num_frames;       // Total frames of the animation
  char *name;         // Pointer to the name of the model (includes path)
  char *dac;            // Pointer to the palette
  char *texture;        // Pointer to the name of the texture (includes path)
} tviewer, *lptviewer;

buffer_sprite3d *viewer_init(lptviewer render_data);
void viewer_loop(lptviewer render_data);
void viewer_end();

#endif
