#include "animated.hpp"

void animated_create(animated *animated_struct,fileanim *f_anm,hlrender *hlrnd,char *name,Resource *res)
{
  complex_create(&animated_struct->complex_struct,hlrnd,name,res);

  	printf("name: %s\n",animated_struct->complex_struct.Objects[0]->name);

  if (animated_struct->complex_struct.nObjects!=f_anm->header.nObjects) {ERROR=(char *)ERR16;exit(1);};
  animated_struct->fanm=f_anm;
  if(animated_struct->complex_struct.nFrames>0)
  {
    lf_free(animated_struct->complex_struct.Anims);
  }
  animated_struct->complex_struct.AIncrustada=0;
  animated_setanim(animated_struct,0);
}

void animated_destroy(animated *animated_struct)
{
  complex_destroy(&animated_struct->complex_struct);
}

void animated_setanim(animated *animated_struct,int order)
{
  animated_struct->complex_struct.Anims=fileanim_getanim(animated_struct->fanm,order,&animated_struct->complex_struct.nFrames);
  animated_struct->complex_struct.curframe=0;
}
/*
void animated_setanim(animated *animated_struct,int order,int frame)
{
  animated_struct->complex_struct.Anims=fileanim_getanim(animated_struct->fanm,order,&animated_struct->complex_struct.nFrames);
  if (frame<0) {
    frame=0;
  }
  if (frame>=animated_struct->complex_struct.nFrames) {
    frame=animated_struct->complex_struct.nFrames-1;
  }
  animated_struct->complex_struct.curframe=frame;
}
*/
