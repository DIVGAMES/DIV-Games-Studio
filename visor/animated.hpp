#ifndef __ANIMATED_HPP_
#define __ANIMATED_HPP_

#include "complex.hpp"
#include "fileanim.hpp"

typedef struct {
  fileanim *fanm;
  complex complex_struct;
} animated;

void animated_create(animated *animated_struct,fileanim *f_anm,hlrender *hlrnd,char *name,Resource *res);
void animated_destroy(animated *animated_struct);
void animated_setanim(animated *animated_struct,int order);
//void setanim(animated *animated_struct,int order,int frame);

#endif

