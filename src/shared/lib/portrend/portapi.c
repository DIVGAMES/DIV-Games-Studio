// Port render DIV interface

#include "inter.h"
#include "portrend.h"

#define text_offset mem[7] // Start of text segment (mem[] index)

tmap *my_map;

static unsigned NumSectors = 0;

void load_wld(void) {
	int num_fpg=pila[sp--];
	int nombre=pila[sp];
	M3D_info mymap;
	
	char *name = (char *)&mem[text_offset+nombre];
	
	printf("Loading WLD: %s\n",name);

	map_read(&mymap, name);
	
	// convert mymap to portrender map
	
	map2port(&mymap);
	
	
	// return 0;
		
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
//     printf("walls[%d]= %d <-> %d - fregion =%d bregion = %d\n",i, my_map->walls[i]->p1,my_map->walls[i]->p2,my_map->walls[i]->front_region,my_map->walls[i]->back_region);
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


void start_mode8(void) {}

void go_to_flag(void) {}

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

