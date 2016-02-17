// Port render DIV interface

#include "inter.h"
#include "portrend.h"

#define text_offset mem[7] // Start of text segment (mem[] index)

M3D_info m8map;

tmap *my_map;

static unsigned NumSectors = 0;

DWORD ScrBase;
int ScrWidth, ScrHeight;
int GfxOn=false;

int vpe_inicializada=0;
int mode8_list[10];
int **fpg_grf;
int num_fpg_aux;
int fr=-1,fg=0,fb=0;
extern int num_blocks;


void VPE_Update(void) {
// TODO - VPE UPDATE
//	printf("VPE UPDATE\n");
}

void InitGraph(char *buffer,int ancho,int alto)
{
  GfxOn=true;
  ScrBase=(DWORD)buffer;
  ScrWidth=ancho;
  ScrHeight=alto;
}


void _vpe_inicio(char *fichero,char *buffer,int ancho,int alto)
{
	return;
	/*
  InitGraph(buffer,ancho,alto); // Init gfx system
  VPE_Init(); // Init VPE
  VPE_Config(ancho,alto);       // Configure VPE

  LoadZone(fichero);            // Load zone
  VPE_Start();                  // Start the engine
  * */
}

void load_wld(void) {
	int num_fpg=pila[sp--];
	int nombre=pila[sp];
	int i=0;
	
	char *name = (char *)&mem[text_offset+nombre];
	
	printf("Loading WLD: %s\n",name);

	map_read(&m8map, name);
	
	// convert mymap to portrender map
	
	map2port(&m8map);
	
	
	// return 0;


  _vpe_inicio(buffer,(char *)copia,vga_an,vga_al);

  for (i=0;i<10;i++) {
    mode8_list[i]=-1;
  }

  vpe_inicializada=1;
  free(buffer);
	pila[sp]=0;
	
}


void map_read_old(M3D_info *m3d_aux, char *full) {
	
	printf("old map - not supported yet\n");
	exit(0);
	
}
void map_read(M3D_info *m3d_aux, char *full)
{
  int i;
  char cwork[9];
  int total;
  FILE *fichero, *f;

  char full_path[_MAX_PATH+1];
  char drive[_MAX_DRIVE+1];
  memset(drive,0,_MAX_DRIVE+1);
  
  char dir[_MAX_DIR+1];
  char fname[_MAX_FNAME+1];
  char ext[_MAX_EXT+1];

  my_map=&(m3d_aux->map);

  my_map->num_points=0;
  my_map->num_walls=0;
  my_map->num_regions=0;
  my_map->num_flags=0;

  if((fichero=fopen(full,"rb"))==NULL)
  {
//    v_texto=(char *)texto[43];
//    dialogo(err0);
printf("Failed to load %s\n",full);

    return;
  }

  fread(cwork,8,1,fichero);
  cwork[8]=0;
  if (!strcmp(cwork,"wld\x1a\x0d\x0a\x00\x00"))
  {
    fclose(fichero);
    map_read_old( m3d_aux , full);
    return;
  }
  if (strcmp(cwork,"wld\x1a\x0d\x0a\x01\x00"))
  {
    fclose(fichero);
    return;
  }

  fread(&total,4,1,fichero);
  if (!total)
  {
    fclose(fichero);
    return;
  }

  fread(m3d_aux->m3d_path,256,1,fichero);
  fread(m3d_aux->m3d_name,16,1,fichero);
  fread(&m3d_aux->numero,4,1,fichero);
  fread(m3d_aux->fpg_path,256,1,fichero);
  fread(m3d_aux->fpg_name,16,1,fichero);

  strcpy((char *)m3d_aux->m3d_path,full);
//  strcpy((char *)m3d_aux->m3d_name,input);

  if((f=fopen((char *)m3d_aux->fpg_path,"rb"))!=NULL)
  {
    fclose(f);
  }
  else
  {
    _splitpath((char *)m3d_aux->m3d_path,drive,dir,fname,ext);
    strcpy(full_path, drive);
    strcat(full_path, dir);
    strcat(full_path, (char *)m3d_aux->fpg_name);
    if((f=fopen(full_path,"rb"))!=NULL)
    {
      strcpy((char *)m3d_aux->fpg_path, full_path);
      fclose(f);
    }
    else
    {
      m3d_aux->fpg_path[0]=0;
      m3d_aux->fpg_name[0]=0;
    }
  }

  fread(&my_map->num_points,4,1,fichero);
	printf("Num points: %d\n point size: %d\n",my_map->num_points,sizeof(tpoint));
	
	for (i=0;i<my_map->num_points;i++) {
		my_map->points[i]=(lptpoint)malloc(sizeof(tpoint));
		fread(my_map->points[i],sizeof(tpoint),1,fichero);
//		printf("point[%d]: %d x %d\n",i,my_map->points[i]->x,my_map->points[i]->y);
	}

  fread(&my_map->num_walls,4,1,fichero);
	printf("Num walls: %d wall struct size: %d\n",my_map->num_walls,sizeof(twall));

  for (i=0;i<my_map->num_walls;i++) {
    my_map->walls[i]=(lptwall)malloc(sizeof(twall));
    fread(my_map->walls[i],sizeof(twall),1,fichero);
     printf("walls[%d]= %d <-> %d - fregion =%d bregion = %d\n",i, my_map->walls[i]->p1,my_map->walls[i]->p2,my_map->walls[i]->front_region,my_map->walls[i]->back_region);
  }
  
  fread(&my_map->num_regions,4,1,fichero);
  printf("Num regions: %d region size: %d\n",my_map->num_regions,sizeof(tregion));

  for (i=0;i<my_map->num_regions;i++) {
    my_map->regions[i]=(lptregion)malloc(sizeof(tregion));
    fread(my_map->regions[i],sizeof(tregion),1,fichero);
//	printf("regions[%d]= floor: %d - ceil:%d\n",i, my_map->regions[i]->floor_height,my_map->regions[i]->ceil_height);
  }
  
  fread(&my_map->num_flags,4,1,fichero);
  printf("Num flags: %d flags struct size: %d\n",my_map->num_flags,sizeof(tflag));

  for (i=0;i<my_map->num_flags;i++) {
    my_map->flags[i]=(lptflag)malloc(sizeof(tflag));
    fread(my_map->flags[i],sizeof(tflag),1,fichero);
//   	printf("flags[%d]= x: %d - y:%d\n",i, my_map->flags[i]->x,my_map->flags[i]->y);

  }
  fread(&my_map->fondo,4,1,fichero);

//  map_off();

  fclose(fichero);
}


void start_mode8(void) {
	 int num_region=pila[sp--];
  int num_mode8=pila[sp--];
  int ide_camera=pila[sp];
  int ancho=0,alto=0;
  t_region *my_region;
  pila[sp]=0;

  my_region=(t_region *)((int)region+sizeof(t_region)*num_region);

  ancho=my_region->x1-my_region->x0;
  alto=my_region->y1-my_region->y0;
  vpe_inicializada=1;
  mode8_list[num_mode8]=num_region;
  m8[num_mode8].camera=ide_camera;
  
  printf("Start mode 8 %d %d %d %d %d\n",ide_camera,num_mode8, num_region, ancho, alto);
}

void go_to_flag(void) {
	int flag=pila[sp];

	pila[sp]=0;

	if (!vpe_inicializada)
		return;

	if (id<id_start || id>id_end)
		return;
	
	// check if flag is valid
	
	if(flag>m8map.map.num_points-1 || flag<0)
		return;

	printf("moved to flags %d\n",flag);
	// moveid to flag at x/y

	mem[id+_X]=m8map.map.flags[flag]->x;
	mem[id+_Y]=m8map.map.flags[flag]->y;

}	

void set_sector_height(void) {}

void get_sector_height(void) {}

void set_point_m8(void) {}

void get_point_m8(void) {}

void set_fog(void) {}

void set_sector_texture(void) {}

void get_sector_texture(void) {}

void set_wall_texture(void) {}

void get_wall_texture(void) {}

void set_env_color(void) {}


void loop_mode8(void) {
	int i,j;
	int ancho,alto;
	t_region *my_region;
	struct Object *po;

	if (!vpe_inicializada)
	return;

	VPE_Update();   // Update zone info

	for (i=0;i<10;i++) {
		if (mode8_list[i]!=-1) {
			if (mem[m8[i].camera+_M8_Object]==-1)
				continue;

			if (mem[m8[i].camera+_Height]<16)
				mem[m8[i].camera+_Height]=16;

			if (m8[i].height<8)
				m8[i].height=8;

			if (mem[m8[i].camera+_Height]-m8[i].height<8)
				m8[i].height=mem[m8[i].camera+_Height]-8;

			my_region=(t_region *)((int)region+sizeof(t_region)*mode8_list[i]);
			ancho=my_region->x1-my_region->x0;
			alto=my_region->y1-my_region->y0;

		}

		VPE_Update();
	}
}

int create_object(int ide) { 
#ifdef NOTYET
  struct Object *object;
  struct Region *new_region;
  int num_object;
  int angulo;

  if (!vpe_inicializada)
    return(-1);

  angulo=((DEG360*mem[ide+_Angle])/360000)&(DEG360-1);
  if (angulo==0 || angulo==512 || angulo==1024 || angulo==1024+512) angulo++;

  // Read OBJECT
  object=(struct Object *)AddEntry(&Objects);
  num_object=Objects.Number-1;
  object->Type=50;
  object->pp=(struct Point *)AddEntry(&Points);

  object->pp->Type=0;
  object->pp->nx=-1;
  object->pp->ny=0;
  object->pp->ld=0;
  object->pp->rd=0;
  object->pp->a=0;
  object->pp->Coord=0;
  object->pp->Count=0;
  object->pp->link=0;
  object->pp->Stamp=-1;

  object->Angle=angulo;

  if (mem[ide+_Resolution]<2) {
    object->H=object->RH=INT_FIX(mem[ide+_Z]);
    object->pp->x=INT_FIX(mem[ide+_X]);
    object->pp->y=INT_FIX(FIN_GRID-mem[ide+_Y]);
  }
  else {
    object->H=object->RH=INT_FIX(mem[ide+_Z])/mem[ide+_Resolution];
    object->pp->x=((mem[ide+_X]*256)/mem[ide+_Resolution])*256;
    object->pp->y=(((FIN_GRID*mem[ide+_Resolution]-mem[ide+_Y])*256)/mem[ide+_Resolution])*256;
  }

  object->Height=INT_FIX(mem[ide+_Height]);
  object->Radius=INT_FIX(mem[ide+_Radius]);

  if (mem[ide+_M8_Step]<0)
    mem[ide+_M8_Step]=0;
  if (mem[ide+_M8_Step]>4095)
    mem[ide+_M8_Step]=4095;
  object->Step=INT_FIX(mem[ide+_M8_Step]);

  object->Mass=INT_FIX(100);
  TexAlloc2(&object->TC,mem[ide+_Graph],mem[ide+_File]);

  object->MSpeed.X=object->MSpeed.Y=object->MSpeed.Z=object->MSpeed.T=0;
  object->Acc.X=object->Acc.Y=object->Acc.Z=object->Acc.T=0;
  object->Speed.X=object->Speed.Y=object->Speed.Z=0;
  object->Fade=0;
  object->Event=0;
  object->wall_number=-1;
  object->region_number=-1;
  object->nextregion_number=-1;
  new_region=FindRegion(object->pp->x,object->pp->y,object->H,
                        object->Type&O_STRUCT);

  if (new_region==NULL) {
    error_vpe=156;
    mem[ide+_Ctype]=0;
    mem[ide+_Old_Ctype]=3;
    _object_destroy(num_object);
    return(-1);
  }
  SetObjRegion(object,new_region);
  return(num_object);

#endif
	printf("create object: %d\n",ide);
return 0;

}

void _object_data_input(int ide) {}

void _object_data_output(int ide) {}

void _object_destroy(int num_object) {}
