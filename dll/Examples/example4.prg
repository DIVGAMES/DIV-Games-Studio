program ModeA_object_log_example;

import "dll/modea.so";


global
logo_angle;

texx;
texy;
tex_no;

begin
load_fpg("dll/Examples/example4.fpg");
set_mode(640480);

//write_int(0,0,0,0,&tex_no);
// CREATES SEPERATE TEXTURES FOR EACH SIDE (SO CAN DRAW TO INDIVIDUALLY)

reset_textures();

put(0,1,160,120);
if(grab_texture(320,240,0)!=1)End_Program("Cannot load texture 1");end
if(grab_texture(120,240,1)!=1)End_Program("Cannot load texture 2");end
if(grab_texture(120,240,2)!=1)End_Program("Cannot load texture 3");end

colour_screen(0);
put(0,3,160,120);
if(grab_texture(320,240,3)!=1)End_Program("Cannot load texture 4");end

colour_screen(0);
put(0,2,25,25);
if(grab_texture(50,50,4)!=1)End_Program("Cannot load texture 5");end




// INITIALIZES MODEA AND OBJECT LOGGING AND CHECKS FOR ERRORS
if( Init_ModeA() != 1)End_Program("Cannot Initialize ModeA!");end
if( Init_object_log() != 1)End_Program("Cannot Initialize object Logging!");end


mouse.graph=2;
mouse.size=50;
set_camera(0,0,-2000,-90000,300);
loop

// CLEARS Z BUFFER,OBJECT LOG AND SCREEN
clear_zbuffer();
reset_object_log();
colour_screen(0);

logo_angle+=3000;
if(logo_angle>360000)logo_angle-=360000;end

if(key(_esc))end_Program("ModeA by Antzrhere");end

draw_wall(
rotate_pointx(-500,-200,logo_angle),
rotate_pointy(-500,-200,logo_angle),
rotate_pointx(500,-200,logo_angle),
rotate_pointy(500,-200,logo_angle),
-100,700,-100,700,0);


draw_wall(
rotate_pointx(-500,200,logo_angle),
rotate_pointy(-500,200,logo_angle),
rotate_pointx(500,200,logo_angle),
rotate_pointy(500,200,logo_angle),
-100,700,-100,700,3);

draw_wall(
rotate_pointx(-500,-200,logo_angle),
rotate_pointy(-500,-200,logo_angle),
rotate_pointx(-500,200,logo_angle),
rotate_pointy(-500,200,logo_angle),
-100,700,-100,700,1);

draw_wall(
rotate_pointx(500,-200,logo_angle),
rotate_pointy(500,-200,logo_angle),
rotate_pointx(500,200,logo_angle),
rotate_pointy(500,200,logo_angle),
-100,700,-100,700,2);


texx = object_log_get_texturex(mouse.x,mouse.y);
texy = object_log_get_texturey(mouse.x,mouse.y);
tex_no=object_log_get_texture_id(mouse.x,mouse.y);


if(mouse.left && tex_no>=0)
copy_to_texture(tex_no,4,texx-25,texy-25);
end



frame;
end
end

// JUST FREES UP ALL MODEA MEMORY AND QUITS WITH A MESSAGE

process End_Program(message);
begin
End_ModeA():
exit(message,0);
end