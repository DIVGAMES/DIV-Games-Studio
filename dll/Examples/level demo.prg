program Level_demo;

import "dll/modea.so";

global

tile_size = 300;
tile_height= 0;
bridge_height=1528;

grid[10*10]=

0,0,1,2,0,2,1,0,0,0,
0,0,0,0,0,0,0,1,0,1,
0,0,1,0,0,2,0,1,0,0,
0,0,0,2,0,0,0,0,2,1,
2,0,0,0,0,0,0,0,0,2,
1,0,2,0,0,0,1,0,0,1,
1,0,1,0,0,0,0,0,0,2,
2,0,2,2,0,0,0,1,1,2,
0,0,0,1,0,1,0,1,0,0,
0,0,0,1,0,1,0,0,0,0;



bridges[10*10]=

0,0,0,0,1,0,0,0,0,0,
0,0,0,0,0,2,0,0,0,0,
0,0,0,0,0,0,1,0,0,2,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,1,0,0,0,0,0,1,1,0,
0,1,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0,0,0,
0,0,0,0,0,0,1,0,0,0,
0,0,0,0,1,0,0,0,0,0;


stage=0;

struct fireball[2];
x;
y;
height;
exists;
end


//import "ModeA.dll";

begin
set_mode(640480);

load_fpg("dll/Examples/ld.fpg");


// LOADS TEXTURES

reset_textures();

put(0,2,32,32);
if(not Grab_texture(64,64,0)==1)end_Program("not enough memory");end

put(0,1,32,32);
if(not Grab_texture(64,64,1)==1)end_Program("not enough memory");end

colour_screen(0);
put(0,3,25,25);
if(not Grab_texture(50,50,2)==1)end_Program("not enough memory");end

colour_screen(0);
put(0,4,43,43);
if(not Grab_texture(87,87,3)==1)end_Program("not enough memory");end




//INITIALIZES MODEA
if(not Init_ModeA()==1)end_Program("not enough memory");end

handle_game();
new_fireball(0);
new_fireball(1);
new_fireball(2);



draw_scene();
end





// DRAWS SCENE

process draw_scene();
private;
c,d;
begin
loop

clear_zbuffer();
shade_screen(28,16);
enable_textured_mode();

for(c=0;c<10;c++)
for(d=0;d<10;d++)

if(grid[c+d*10]>0 && tile_height>0)

// removes hidden walls

if(d!=0)
if(grid[c+(d-1)*10]==0)
draw_wall(c*tile_size,d*tile_size,c*tile_size+tile_size,d*tile_size,0,tile_height,0,tile_height,grid[c+d*10]-1);
end
else
draw_wall(c*tile_size,d*tile_size,c*tile_size+tile_size,d*tile_size,0,tile_height,0,tile_height,grid[c+d*10]-1);
end

if(d!=9)
if(grid[c+(d+1)*10]==0)
draw_wall(c*tile_size,d*tile_size+tile_size,c*tile_size+tile_size,d*tile_size+tile_size,0,tile_height,0,tile_height,grid[c+d*10]-1);
end
else
draw_wall(c*tile_size,d*tile_size+tile_size,c*tile_size+tile_size,d*tile_size+tile_size,0,tile_height,0,tile_height,grid[c+d*10]-1);
end

if(c!=0)
if(grid[(c-1)+d*10]==0)
draw_wall(c*tile_size,d*tile_size,c*tile_size,d*tile_size+tile_size,0,tile_height,0,tile_height,grid[c+d*10]-1);
end
else
draw_wall(c*tile_size,d*tile_size,c*tile_size,d*tile_size+tile_size,0,tile_height,0,tile_height,grid[c+d*10]-1);
end

if(c!=9)
if(grid[(c+1)+d*10]==0)
draw_wall(c*tile_size+tile_size,d*tile_size,c*tile_size+tile_size,d*tile_size+tile_size,0,tile_height,0,tile_height,grid[c+d*10]-1);
end
else
draw_wall(c*tile_size+tile_size,d*tile_size,c*tile_size+tile_size,d*tile_size+tile_size,0,tile_height,0,tile_height,grid[c+d*10]-1);
end

//--





draw_tri_floor(c*tile_size,d*tile_size,c*tile_size+tile_size,d*tile_size,c*tile_size+tile_size,d*tile_size+tile_size,tile_height,7);
draw_tri_floor(c*tile_size,d*tile_size,c*tile_size,d*tile_size+tile_size,c*tile_size+tile_size,d*tile_size+tile_size,tile_height,7);
else;
draw_tri_floor(c*tile_size,d*tile_size,c*tile_size+tile_size,d*tile_size,c*tile_size+tile_size,d*tile_size+tile_size,0,22+rand(0,3));
draw_tri_floor(c*tile_size,d*tile_size,c*tile_size,d*tile_size+tile_size,c*tile_size+tile_size,d*tile_size+tile_size,0,22+rand(0,3));
end;
end
end


enable_coloured_mode();
for(c=0;c<10;c++)
for(d=0;d<10;d++)

if(bridges[c+d*10]==1 && bridge_height<1000)
draw_tri_floor(c*tile_size,d*tile_size+(tile_size/6),c*tile_size+tile_size,d*tile_size+(tile_size/6),c*tile_size+tile_size,d*tile_size+tile_size-(tile_size/6),bridge_height,119);
draw_tri_floor(c*tile_size,d*tile_size+(tile_size/6),c*tile_size,d*tile_size+tile_size-(tile_size/6),c*tile_size+tile_size,d*tile_size+tile_size-(tile_size/6),bridge_height,119);
draw_wall(c*tile_size,d*tile_size+(tile_size/6),c*tile_size+tile_size,d*tile_size+(tile_size/6),bridge_height-70,70,bridge_height-70,70,118);
draw_wall(c*tile_size,d*tile_size+tile_size-(tile_size/6),c*tile_size+tile_size,d*tile_size+tile_size-(tile_size/6),bridge_height-70,70,bridge_height-70,70,118);


end

if(bridges[c+d*10]==2 && bridge_height<1000)


draw_tri_floor(c*tile_size+(tile_size/6),d*tile_size,c*tile_size+tile_size-(tile_size/6),d*tile_size,c*tile_size+tile_size-(tile_size/6),d*tile_size+tile_size,bridge_height,119);
draw_tri_floor(c*tile_size+(tile_size/6),d*tile_size,c*tile_size+(tile_size/6),d*tile_size+tile_size,c*tile_size+tile_size-(tile_size/6),d*tile_size+tile_size,bridge_height,119);
draw_wall(c*tile_size+(tile_size/6),d*tile_size,c*tile_size+(tile_size/6),d*tile_size+tile_size,bridge_height-70,70,bridge_height-70,70,118);
draw_wall(c*tile_size+tile_size-(tile_size/6),d*tile_size,c*tile_size+tile_size-(tile_size/6),d*tile_size+tile_size,bridge_height-70,70,bridge_height-70,70,118);





end;

end
end






for(x=0;x<3;x++)
draw_sprite(fireball[x].x*tile_size+(tile_size/2),fireball[x].height,fireball[x].y*tile_size+(tile_size/2),2,150);
end

//draw_sprite(1000,tile_height+10,1000,3,100);
frame;
end
end









//HANDLES GAME

process handle_game();
private;
horizon=-10;
begin
write(0,10,10,0,"Requesting Area...");
angle=20500;
x=-860;
y=-1120;
z=4000;
loop
if(key(_esc))End_Program("ModeA by Antzrhere");end

if(stage==0)if(angle>-36000)angle-=500;z-=15;else;stage=1;end;end
if(stage==1)if(z>800)delete_text(0);write(0,10,10,0,"Generating Land...");angle-=200;z-=20;advance(20);tile_height+=3;else;stage=2;end;end
if(stage==2)if(x<1000)x+=20;angle-=650;else;stage=3;end;
if(bridge_height>228)delete_text(0);write(0,10,10,0,"Setting Bridges...");bridge_height-=50;end
end

if(stage==3)
if(angle>-90000)angle-=500;delete_text(0);write(0,10,10,0,"Level Complete.")  ;
else;stage=4;end
end



if(stage==4)
if(y>-1100)advance(-25);z+=30;horizon--;x+=15;delete_text(0);
else;stage=5;end
end

set_camera(x,z,y,angle,horizon);
frame;
end
end

// HANDLES FIREBALLS

process new_fireball(no);
private;
accl;
dir=0;
begin
loop
if(fireball[no].exists==0)
x=rand(0,9);
y=rand(0,9);
if(grid[x+y*10]==0 && bridges[x+y*10]==0)
fireball[no].x=x;
fireball[no].y=y;
fireball[no].height=rand(-100,-10);
fireball[no].exists=1;
accl=20;
dir=0;
end
end
if(dir==0)accl+=2;
else;accl-=5;end

fireball[no].height+=accl;

if(fireball[no].height>100)dir=1;end
if(fireball[no].height<-100 && dir==1)fireball[no].exists=0;end



frame;
end
end




// FREE"S UP MEMORY AND QUITS

process End_Program(msg);
begin
End_ModeA();
exit(msg,0);
end