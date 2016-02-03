/*

The purpose of Tbuffers with ModeA is to show you that you can draw
as complex scenes as you want and just have the character moving,
similar to some resident evil games. The reason behind this
is that a game can run on slow pc's just as well as on a top
of the range pc.

Tbuffers work by prerendering the complex scene, saving the pic &
zbuffer and copying it every scene. Then just draw the extra
moving stuff ect.

In this example, no matter how complicated the pre-rendered scene is
it will run at the same speed.
*/
program ModeA_Example1_using_Tbuffer_techinques;

import "dll/modea.so";

global

begin
load_fpg("dll/Examples/example1.fpg");
set_mode(640480);
set_fps(50,0);
write_int(0,0,0,0,&fps);

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

colour_screen(0);
put(0,4,15,34);
if(grab_texture(30,69,3)!=1)End_Program("Cannot load texture 4");end




// INITIALIZES MODEA AND CHECKS FOR ERRORS
if( Init_ModeA() != 1)End_Program("Cannot load texture 1");end

// Initializes Tbuffers
if(Alloc_Tbuffer(0)!=1)End_Program("cannot allocate Tbuffer!");end // allocates Tbuffer for Zbuffer storing
if(Alloc_Tbuffer(1)!=1)End_Program("cannot allocate Tbuffer!");end // allocates Tbuffer for background storing


// CLEARS Z BUFFER AND SCREEN
clear_zbuffer();
Shade_screen(25,31);
set_camera(-1200,500,-600,-43000,320);


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

copy_zbuffer_to_Tbuffer(0);    // copies zbuffer to Tbuffer
copy_background_to_Tbuffer(1); // copies background to Tbuffer

x=-200;
y=-200;



loop // starts loop

if(key(_esc))End_Program("ModeA by Antzrhere");end

copy_Tbuffer_to_Zbuffer(0); // copies  back again every frame
copy_TBuffer_to_background(1); // copies  back again every frame

if(key(_left))x-=20;end
if(key(_right))x+=20;end
if(key(_up))y+=20;end
if(key(_down))y-=20;end

Draw_sprite(x,-100,y,3,900); // redraws moving man every frame

frame;
end
end

// JUST FREES UP ALL MODEA MEMORY AND QUITS WITH A MESSAGE

process End_program(message);
begin
End_ModeA();
exit(message,0);
end