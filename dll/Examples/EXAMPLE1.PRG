program ModeA_Fire_engine_example;
//global

import "dll/modea.so";

//global
begin
load_fpg("dll/Examples/example1.fpg");
set_mode(640480);


// LOADS TEXTURES AND CHECKS FOR ERRORS

reset_textures();

put(0,1,202,60);
if(grab_texture(404,120,0)!=1)End_Program("Cannot load texture 1");end

colour_screen(0);
put(0,2,60,60);
if(grab_texture(120,120,1)!=1)End_Program("Cannot load texture 2");end

colour_screen(0);
put(0,3,60,60);
if(grab_texture(120,120,2)!=1)End_Program("Cannot load texture 3");end


// INITIALIZES MODEA AND CHECKS FOR ERRORS
if( Init_ModeA() != 1)End_Program("Cannot Init ModeA");end
angle=-43000;
y=-1380;
x=-2160;

loop

// CLEARS Z BUFFER AND SCREEN

clear_zbuffer();
Shade_screen(25,31);




// USER CONTROL

if(key(_esc))End_Program("ModeA by Antzrhere");end
if(key(_right))angle+=2000;end
if(key(_left))angle-=2000;end
if(key(_up))advance(60);end
if(key(_down))advance(-60);end

set_camera(x,300,y,angle,mouse.y);

//DRAWS SCENE

enable_textured_mode();
draw_wall(480,1490,480,1010,-12,512,-12,512,2);       // interior of model
draw_wall(-100,1000,2000,1000,-100,600,-100,600,0); // side of model
draw_wall(-100,1500,2000,1500,-100,600,-100,600,0);

draw_wall(-100,1500,-100,1000,-100,600,-100,600,1); // front of model

enable_coloured_mode();

draw_wall(2000,1500,2000,1000,-12,512,-12,512,23);     // back of model

draw_tri_floor(-100,1000,-100,1500,2000,1000,500,10); //top of model
draw_tri_floor(-100,1500,2000,1000,2000,1500,500,10);

draw_tri_floor(-100,1000,-100,1500,2000,1000,-100,6);  // road
draw_tri_floor(-100,1500,2000,1000,2000,1500,-100,6);
draw_tri_floor(-100,1000,-100,1500,-2000,1000,-100,7);
draw_tri_floor(-100,1500,-2000,1000,-2000,1500,-100,7);

draw_tri_floor(-100,1000,-100,-1000,2000,1000,-100,35);   //grass
draw_tri_floor(-100,-1000,2000,1000,2000,-1000,-100,35);
draw_tri_floor(-100,1500,-100,3500,2000,1500,-100,35);
draw_tri_floor(-100,3500,2000,1500,2000,3500,-100,35);

draw_tri_floor(-100,1000,-100,-1000,-2000,1000,-100,35);
draw_tri_floor(-100,-1000,-2000,1000,-2000,-1000,-100,35);
draw_tri_floor(-100,1500,-100,3500,-2000,1500,-100,35);
draw_tri_floor(-100,3500,-2000,1500,-2000,3500,-100,35);






frame;
end
end

// JUST FREES UP ALL MODEA MEMORY AND QUITS WITH A MESSAGE

process End_program(message);
begin
End_ModeA();
exit(message,0);
end