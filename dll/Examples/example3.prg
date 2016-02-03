program ModeA_zbuffer_stencil_example;
import "dll/modea.so";
global
logo_angle=-90000;

begin
load_fpg("dll/Examples/example3.fpg");
set_mode(640480);


// LOADS TEXTURE AND CHECKS FOR ERRORS

reset_textures();

put(0,1,160,120);
if(grab_texture(320,240,0)!=1)End_Program("Cannot load texture 1");end

// LOADS STENCIL AS A TEXTURE AND CHECKS FOR ERRORS

colour_screen(0);
put(0,2,320,240);
if(grab_texture(640,480,1)!=1)End_Program("Cannot load stencil");end


// INITIALIZES MODEA AND CHECKS FOR ERRORS
if( Init_ModeA() != 1)End_Program("Cannot Initialize ModeA!");end

set_camera(0,0,-2500,-90000,300);
loop

// STENCILS Z BUFFER AND CLEARS SCREEN
stencil_zbuffer(1);

colour_screen(0);

logo_angle+=5000;
if(logo_angle>360000)logo_angle-=360000;end

if(key(_esc))end_Program("ModeA by Antzrhere");end


draw_wall(
rotate_pointx(-1000,-200,logo_angle),
rotate_pointy(-1000,-200,logo_angle),
rotate_pointx(1000,-200,logo_angle),
rotate_pointy(1000,-200,logo_angle),
-100,700,-100,700,0);


draw_wall(
rotate_pointx(-1000,200,logo_angle),
rotate_pointy(-1000,200,logo_angle),
rotate_pointx(1000,200,logo_angle),
rotate_pointy(1000,200,logo_angle),
-100,700,-100,700,0);

draw_wall(
rotate_pointx(-1000,-200,logo_angle),
rotate_pointy(-1000,-200,logo_angle),
rotate_pointx(-1000,200,logo_angle),
rotate_pointy(-1000,200,logo_angle),
-100,700,-100,700,0);

draw_wall(
rotate_pointx(1000,-200,logo_angle),
rotate_pointy(1000,-200,logo_angle),
rotate_pointx(1000,200,logo_angle),
rotate_pointy(1000,200,logo_angle),
-100,700,-100,700,0);


frame;
end
end

// JUST FREES UP ALL MODEA MEMORY AND QUITS WITH A MESSAGE

process End_Program(message);
begin
End_ModeA();
exit(message,0);
end