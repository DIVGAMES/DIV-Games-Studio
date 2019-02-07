// Port render DIV interface

#include "inter.h"
#include "internal.h"
#include "portrend.h"
#include "vpe.h"

#define text_offset mem[7] // Start of text segment (mem[] index)

M3D_info m8map;

tmap *my_map;

static unsigned NumSectors = 0;

DWORD ScrBase;
int ScrWidth, ScrHeight;
int GfxOn = false;

int vpe_inicializada = 0;
int mode8_list[10];
int **fpg_grf;
int num_fpg_aux;
int fr = -1, fg = 0, fb = 0;
extern int num_blocks;

int VPE_Working = FALSE;
int VPE_fog = 0;

void set_fog_table(int intensidad, int r, int g, int b);

extern int **fpg_grf;
extern int num_fpg_aux;

struct ZF_Flag *flags[1000];
int num_flags;
extern int error_vpe;
static char combo_error[128]; // para componer mensajes de error compuestos.

int *tex_pointer;
int tex_size;

#define READ(ptr, type)                                                        \
	{                                                                      \
		ptr = (type *)&Buffer[Pos];                                    \
		Pos += sizeof(type);                                           \
	}
#define COPY(ptr, size)                                                        \
	{                                                                      \
		memcpy(ptr, &Buffer[Pos], size);                               \
		Pos += size;                                                   \
	}
#define PI 3.141592f

#ifdef NOTYET

void ShutMemory(void)
{
	MemFreeAll();
	ClearZone();
}

void VPE_Stop(void)
{
	VPE_Working = FALSE; // VPE is not working
}

void VPE_Shut(void)
{
	ShutMemory(); // Shut down memory manager
}

#endif

void VPE_Update(void)
{
	// TODO - VPE UPDATE
	//	printf("VPE UPDATE\n");
}

void InitGraph(char *buffer, int ancho, int alto)
{
	GfxOn = true;
	ScrBase = (DWORD)buffer;
	ScrWidth = ancho;
	ScrHeight = alto;
}

void _vpe_inicio(char *fichero, char *buffer, int ancho, int alto)
{
	/*
  InitGraph(buffer,ancho,alto); // Init gfx system
  VPE_Init(); // Init VPE
  VPE_Config(ancho,alto);       // Configure VPE
*/
	LoadZone(fichero); // Load zone
			   /*
  VPE_Start();                  // Start the engine
  * */
}

void _vpe_fin(void) {}

void load_wld(void)
{
	int num_fpg = pila[sp--];
	int nombre = pila[sp];
	int i, m;
	FILE *fichero;
	char *buffer;
	int size;

	pila[sp] = 0;

	if (vpe_inicializada) {
		while (Objects.Number) {
			_object_destroy(0);
		}
		_vpe_fin();
	}

	num_blocks = 0;
	fpg_grf = g[num_fpg].grf;
	num_fpg_aux = num_fpg;

	if (npackfiles) {
		m = read_packfile((byte *)&mem[text_offset + nombre]);
		if (m == -1)
			goto wldfuera; // not found
		if (m == -2) {
			pila[sp] = 0;
			e(100);
			return;
		} // not enough memory
		if (m <= 0) {
			pila[sp] = 0;
			e(200);
			return;
		} // bad size
		buffer = packptr;
		size = m;
	} else {
	wldfuera:
		if ((fichero = div_open_file(
			 (byte *)&mem[text_offset + nombre])) == NULL) {
			e(159);
			vpe_inicializada = 0;
			return;
		} else {
			fseek(fichero, 0, SEEK_END);
			size = ftell(fichero);
			if ((buffer = (byte *)malloc(size)) != NULL) {
				fseek(fichero, 0, SEEK_SET);
				fread(buffer, 1, size, fichero);
				fclose(fichero);
			} else {
				fclose(fichero);
				e(100);
				vpe_inicializada = 0;
				return;
			}
		}
	}

	_vpe_inicio(buffer, (char *)copia, vga_an, vga_al);

	for (i = 0; i < 10; i++) {
		mode8_list[i] = -1;
	}

	vpe_inicializada = 1;
	free(buffer);
}

void map_read_old(M3D_info *m3d_aux, char *full)
{

	printf("old map - not supported yet\n");
	exit(0);
}

void map_read(M3D_info *m3d_aux, char *full)
{
	int i;
	char cwork[9];
	int total;
	FILE *fichero, *f;

	char full_path[_MAX_PATH + 1];
	char drive[_MAX_DRIVE + 1];
	memset(drive, 0, _MAX_DRIVE + 1);

	char dir[_MAX_DIR + 1];
	char fname[_MAX_FNAME + 1];
	char ext[_MAX_EXT + 1];

	my_map = &(m3d_aux->map);

	my_map->num_points = 0;
	my_map->num_walls = 0;
	my_map->num_regions = 0;
	my_map->num_flags = 0;

	if ((fichero = fopen(full, "rb")) == NULL) {
		//    v_texto=(char *)texto[43];
		//    dialogo(err0);
		printf("Failed to load %s\n", full);

		return;
	}

	fread(cwork, 8, 1, fichero);
	cwork[8] = 0;
	if (!strcmp(cwork, "wld\x1a\x0d\x0a\x00\x00")) {
		fclose(fichero);
		map_read_old(m3d_aux, full);
		return;
	}
	if (strcmp(cwork, "wld\x1a\x0d\x0a\x01\x00")) {
		fclose(fichero);
		return;
	}

	fread(&total, 4, 1, fichero);
	if (!total) {
		fclose(fichero);
		return;
	}

	fread(m3d_aux->m3d_path, 256, 1, fichero);
	fread(m3d_aux->m3d_name, 16, 1, fichero);
	fread(&m3d_aux->numero, 4, 1, fichero);
	fread(m3d_aux->fpg_path, 256, 1, fichero);
	fread(m3d_aux->fpg_name, 16, 1, fichero);

	strcpy((char *)m3d_aux->m3d_path, full);
	//  strcpy((char *)m3d_aux->m3d_name,input);

	if ((f = fopen((char *)m3d_aux->fpg_path, "rb")) != NULL) {
		fclose(f);
	} else {
		_splitpath((char *)m3d_aux->m3d_path, drive, dir, fname, ext);
		strcpy(full_path, drive);
		strcat(full_path, dir);
		strcat(full_path, (char *)m3d_aux->fpg_name);
		if ((f = fopen(full_path, "rb")) != NULL) {
			strcpy((char *)m3d_aux->fpg_path, full_path);
			fclose(f);
		} else {
			m3d_aux->fpg_path[0] = 0;
			m3d_aux->fpg_name[0] = 0;
		}
	}

	fread(&my_map->num_points, 4, 1, fichero);
	//	printf("Num points: %d\n point size:
	//%d\n",my_map->num_points,sizeof(tpoint));

	for (i = 0; i < my_map->num_points; i++) {
		my_map->points[i] = (lptpoint)malloc(sizeof(tpoint));
		fread(my_map->points[i], sizeof(tpoint), 1, fichero);
		printf("point[%d]: %d x %d\n", i, my_map->points[i]->x,
		       my_map->points[i]->y);
	}

	fread(&my_map->num_walls, 4, 1, fichero);
	//	printf("Num walls: %d wall struct size:
	//%d\n",my_map->num_walls,sizeof(twall));

	for (i = 0; i < my_map->num_walls; i++) {
		my_map->walls[i] = (lptwall)malloc(sizeof(twall));
		fread(my_map->walls[i], sizeof(twall), 1, fichero);
		printf("walls[%d]= %d <-> %d - fregion =%d bregion = %d\n", i,
		       my_map->walls[i]->p1, my_map->walls[i]->p2,
		       my_map->walls[i]->front_region,
		       my_map->walls[i]->back_region);
	}

	fread(&my_map->num_regions, 4, 1, fichero);
	//  printf("Num regions: %d region size:
	//  %d\n",my_map->num_regions,sizeof(tregion));

	for (i = 0; i < my_map->num_regions; i++) {
		my_map->regions[i] = (lptregion)malloc(sizeof(tregion));
		fread(my_map->regions[i], sizeof(tregion), 1, fichero);
		printf("regions[%d]= floor: %d - ceil:%d\n", i,
		       my_map->regions[i]->floor_height,
		       my_map->regions[i]->ceil_height);
	}

	fread(&my_map->num_flags, 4, 1, fichero);
	printf("Num flags: %d flags struct size: %d\n", my_map->num_flags,
	       sizeof(tflag));

	for (i = 0; i < my_map->num_flags; i++) {
		my_map->flags[i] = (lptflag)malloc(sizeof(tflag));
		fread(my_map->flags[i], sizeof(tflag), 1, fichero);
		printf("flags[%d]= x: %d - y:%d\n", i, my_map->flags[i]->x,
		       my_map->flags[i]->y);
	}
	fread(&my_map->fondo, 4, 1, fichero);

	//  map_off();

	fclose(fichero);
}

extern m8camera player;

void start_mode8(void)
{
	int num_region = pila[sp--];
	int num_mode8 = pila[sp--];
	int ide_camera = pila[sp];
	int ancho = 0, alto = 0;
	t_region *my_region;
	pila[sp] = 0;

	my_region = (t_region *)((int)region + sizeof(t_region) * num_region);

	ancho = my_region->x1 - my_region->x0;
	alto = my_region->y1 - my_region->y0;
	vpe_inicializada = 1;
	mode8_list[num_mode8] = num_region;
	m8[num_mode8].camera = ide_camera;

	printf("Start mode 8 %d %d %d %d %d\n", ide_camera, num_mode8,
	       num_region, ancho, alto);
}

void go_to_flag(void)
{
	int flag = pila[sp];

	pila[sp] = 0;

	if (!vpe_inicializada)
		return;

	if (id < id_start || id > id_end)
		return;

	// check if flag is valid

	if (flag > m8map.map.num_points - 1 || flag < 0)
		return;

	printf("moved to flags %d %d %d\n", flag, m8map.map.flags[flag]->x,
	       m8map.map.flags[flag]->y);
	// moveid to flag at x/y
	//	player.where.z=my_map->regions[i]->floor_height;
	mem[id + _X] = m8map.map.flags[flag]->x;
	mem[id + _Y] = m8map.map.flags[flag]->y;
	mem[id + _Z] = 0; // m8map.map.regions[0]->floor_height;
	player.sector = 0;
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

extern SDL_Surface *vga;
extern int ancho, alto;

void loop_mode8(void)
{
	int i, j;
	t_region *my_region;
	struct Object *po;
	int angulo;

	if (!vpe_inicializada)
		return;

	VPE_Update(); // Update zone info

	for (i = 0; i < 10; i++) {
		if (mode8_list[i] != -1) {
			if (mem[m8[i].camera + _M8_Object] == -1)
				continue;

			if (mem[m8[i].camera + _Height] < 16)
				mem[m8[i].camera + _Height] = 16;

			if (m8[i].height < 8)
				m8[i].height = 8;

			if (mem[m8[i].camera + _Height] - m8[i].height < 8)
				m8[i].height = mem[m8[i].camera + _Height] - 8;

			my_region =
			    (t_region *)((int)region +
					 sizeof(t_region) * mode8_list[i]);
			ancho = my_region->x1 - my_region->x0;
			alto = my_region->y1 - my_region->y0;
			// my_map->regions[i]->floor_height;

			//			angulo=((180*mem[m8[i].camera+_Angle])/360000)&(180-1);
			//			 angulo=((DEG360*mem[m8[i].camera+_Angle])/360000)&(DEG360-1);
			//			if (angulo==0 || angulo==512 ||
			//angulo==1024 || angulo==1024+512) angulo++;

			//			if (angulo==0 || angulo==512 ||
			//angulo==1024 || angulo==1024+512) angulo++;

			player.where.x =
			    mem[m8[i].camera +
				_X]; /// mem[m8[i].camera+_Resolution];
			player.where.y =
			    mem[m8[i].camera +
				_Y]; /// mem[m8[i].camera+_Resolution];
			player.where.z = mem[m8[i].camera + _Z];
			player.angle =
			    (float)mem[m8[i].camera + _Angle] / 360000;

			// printf("");

			// fflush(stdout);
			// float x=player.where.x;

			printf("player: %f %f %f\r", player.where.x,
			       player.where.y, player.angle);
			// printf("%f\n",player.angle);
			MovePlayer(0, 0);
			DrawScreen();
		}
		// SDL_Flip(vga);

		VPE_Update();
	}
}

int create_object(int ide)
{
#ifdef NOTYET
	struct Object *object;
	struct Region *new_region;
	int num_object;
	int angulo;

	if (!vpe_inicializada)
		return (-1);

	angulo = ((DEG360 * mem[ide + _Angle]) / 360000) & (DEG360 - 1);
	if (angulo == 0 || angulo == 512 || angulo == 1024 ||
	    angulo == 1024 + 512)
		angulo++;

	// Read OBJECT
	object = (struct Object *)AddEntry(&Objects);
	num_object = Objects.Number - 1;
	object->Type = 50;
	object->pp = (struct Point *)AddEntry(&Points);

	object->pp->Type = 0;
	object->pp->nx = -1;
	object->pp->ny = 0;
	object->pp->ld = 0;
	object->pp->rd = 0;
	object->pp->a = 0;
	object->pp->Coord = 0;
	object->pp->Count = 0;
	object->pp->link = 0;
	object->pp->Stamp = -1;

	object->Angle = angulo;

	if (mem[ide + _Resolution] < 2) {
		object->H = object->RH = INT_FIX(mem[ide + _Z]);
		object->pp->x = INT_FIX(mem[ide + _X]);
		object->pp->y = INT_FIX(FIN_GRID - mem[ide + _Y]);
	} else {
		object->H = object->RH =
		    INT_FIX(mem[ide + _Z]) / mem[ide + _Resolution];
		object->pp->x =
		    ((mem[ide + _X] * 256) / mem[ide + _Resolution]) * 256;
		object->pp->y =
		    (((FIN_GRID * mem[ide + _Resolution] - mem[ide + _Y]) *
		      256) /
		     mem[ide + _Resolution]) *
		    256;
	}

	object->Height = INT_FIX(mem[ide + _Height]);
	object->Radius = INT_FIX(mem[ide + _Radius]);

	if (mem[ide + _M8_Step] < 0)
		mem[ide + _M8_Step] = 0;
	if (mem[ide + _M8_Step] > 4095)
		mem[ide + _M8_Step] = 4095;
	object->Step = INT_FIX(mem[ide + _M8_Step]);

	object->Mass = INT_FIX(100);
	TexAlloc2(&object->TC, mem[ide + _Graph], mem[ide + _File]);

	object->MSpeed.X = object->MSpeed.Y = object->MSpeed.Z =
	    object->MSpeed.T = 0;
	object->Acc.X = object->Acc.Y = object->Acc.Z = object->Acc.T = 0;
	object->Speed.X = object->Speed.Y = object->Speed.Z = 0;
	object->Fade = 0;
	object->Event = 0;
	object->wall_number = -1;
	object->region_number = -1;
	object->nextregion_number = -1;
	new_region = FindRegion(object->pp->x, object->pp->y, object->H,
				object->Type & O_STRUCT);

	if (new_region == NULL) {
		error_vpe = 156;
		mem[ide + _Ctype] = 0;
		mem[ide + _Old_Ctype] = 3;
		_object_destroy(num_object);
		return (-1);
	}
	SetObjRegion(object, new_region);
	return (num_object);

#endif
	printf("create object: %d\n", ide);
	return 0;
}

void _object_data_input(int ide) {}

void _object_data_output(int ide) {}

void _object_destroy(int num_object) {}

void LoadZone(char *Buffer)
{
	struct ZF_Header *Header;
	struct ZF_General *zgen;
	struct ZF_Point *zp;
	struct ZF_Region *zr;
	struct ZF_Wall *zw;
	struct ZF_Flag *zf;
	struct Point *point;
	struct Region *new_region;
	struct Wall *wall;
	struct View *view;
	int n, i, t1, t2;
	LONG Pos;

	// Clear all the zone memory
	ClearZone();
	tex_pointer = NULL;
	tex_size = 0;
	// Set current Buffer position
	Pos = 0;

	//---------------------------------------------------------------------------
	// Lee la cabecera de edicion
	//---------------------------------------------------------------------------
	if (strcmp(&Buffer[Pos], "wld\x1a\x0d\x0a\x01")) {
		error_vpe = 160;
		return;
	}

	Pos += (8 + 4 + *(int *)&Buffer[8]);

	//---------------------------------------------------------------------------
	// Lee la cabecera
	//---------------------------------------------------------------------------
	READ(Header, struct ZF_Header);

	//---------------------------------------------------------------------------
	// Lee los puntos
	//---------------------------------------------------------------------------
	for (n = 0; n < Header->NumPoints; n++) {
		READ(zp, struct ZF_Point);
		point = (struct Point *)AddEntry(&Points);
		point->Type = zp->Type;
		point->x = INT_FIX(zp->x);
		point->y = INT_FIX(zp->y);
		point->nx = zp->link;
		point->Coord = point->Count = 0;
		point->Stamp = -1;
	}
	for (n = 0; n < Header->NumPoints; n++) {
		point = (struct Point *)Points.ptr[n];
		if (point->nx > -1)
			point->link = (struct Point *)Points.ptr[point->nx];
		else
			point->link = NULL;
	}

	//---------------------------------------------------------------------------
	// Lee las regiones
	//---------------------------------------------------------------------------
	for (n = 0; n < Header->NumRegions; n++) {
		READ(zr, struct ZF_Region);
		new_region = (struct Region *)AddEntry(&Regions);
		new_region->Type = zr->Type;
		new_region->FloorH = INT_FIX(zr->FloorH);
		new_region->CeilH = INT_FIX(zr->CeilH);
		TexAlloc(&new_region->FloorTC, zr->FloorTex, num_fpg_aux);
		TexAlloc(&new_region->CeilTC, zr->CeilTex, num_fpg_aux);
		new_region->Below = (struct Region *)zr->Below;
		new_region->Above = (struct Region *)zr->Above;
		new_region->First = NULL;
		new_region->Fade = zr->Fade;
		new_region->Tag = n;
		new_region->Stamp = -1;
		new_region->Idx = n;
	}
	for (n = 0; n < Header->NumRegions; n++) {
		new_region = (struct Region *)Regions.ptr[n];
		t1 = (int)new_region->Above;
		t2 = (int)new_region->Below;
		if (t1 < 0)
			new_region->Above = NULL;
		else
			new_region->Above = (struct Region *)Regions.ptr[t1];
		if (t2 < 0)
			new_region->Below = NULL;
		else
			new_region->Below = (struct Region *)Regions.ptr[t2];
	}

	//---------------------------------------------------------------------------
	// Lee las paredes
	//---------------------------------------------------------------------------
	for (n = 0; n < Header->NumWalls; n++) {
		READ(zw, struct ZF_Wall);
		wall = (struct Wall *)AddEntry(&Walls);
		wall->Type = zw->Type;
		wall->Fade = zw->Fade;
		wall->p1 = (struct Point *)Points.ptr[zw->p1];
		wall->p2 = (struct Point *)Points.ptr[zw->p2];
		wall->Mass = INT_FIX(zw->Mass);
		t1 = FIX_INT(wall->p2->x - wall->p1->x);
		t2 = FIX_INT(wall->p2->y - wall->p1->y);
		wall->Length = INT_FIX(LongSqrt(t1 * t1 + t2 * t2));
		wall->Front = (struct Region *)Regions.ptr[zw->Front];
		printf("Wall: %d Front: %d Back: %d\n", n, zw->Front, zw->Back);
		if (zw->Back > -1)
			wall->Back = (struct Region *)Regions.ptr[zw->Back];
		else
			wall->Back = NULL;
		wall->TexX = INT_FIX(zw->TexX);
		wall->TexY = INT_FIX(zw->TexY);
		TexAlloc(&wall->TopTC, zw->TopTex, num_fpg_aux);
		TexAlloc(&wall->MidTC, zw->MidTex, num_fpg_aux);
		TexAlloc(&wall->BotTC, zw->BotTex, num_fpg_aux);
		wall->Tag = n;
		wall->Stamp = -1;
	}

	//---------------------------------------------------------------------------
	// Lee las banderas
	//---------------------------------------------------------------------------
	num_flags = Header->NumFlags;
	for (n = 0; n < Header->NumFlags; n++) {
		READ(zf, struct ZF_Flag);
		flags[n] = (struct ZF_Flag *)MemAlloc(sizeof(struct ZF_Flag));
		flags[n]->x = INT_FIX(zf->x);
		flags[n]->y = INT_FIX(zf->y);
		flags[n]->number = zf->number;
	}

	//---------------------------------------------------------------------------
	// Inicializa los objetos y la vista
	//---------------------------------------------------------------------------
	view = (struct View *)AddEntry(&Views);
	view->ScrX = 0;
	view->ScrY = 0;
	view->Width = 0;
	view->Height = 0;
	view->pObject = NULL;
	view->ObjHeight = 0;
	view->VAngle = 0;
	view->HAngle = 0;
	view->FIni = INT_FIX(0);
	view->FLen = INT_FIX(100);
	view->Table = 0;
	view->Size = 0;
	view->Buffer = NULL;
	view->BufScan = NULL;
	view->StartClip = NULL;

	//---------------------------------------------------------------------------
	// Lee los datos de caracter general
	//---------------------------------------------------------------------------
	READ(zgen, struct ZF_General);
	strcpy(Gen.Title, zgen->Title);
	//  Cargo el fondo
	TexAlloc2(&Gen.BackTC, zgen->BackTex, num_fpg_aux);
	if (Gen.BackTC.pPic == NULL)
		TexAlloc2(&Gen.BackTC, -1, num_fpg_aux);
	TexCheck2(&Gen.BackTC);

	//  Gen.BackAngle=zgen->BackAngle*DEG90/90;
	Gen.BackAngle = 120 * DEG90 / 90;

	Gen.GlobalForce.X = INT_FIX(zgen->Force.x);
	Gen.GlobalForce.Y = INT_FIX(zgen->Force.y);
	Gen.GlobalForce.Z = INT_FIX(zgen->Force.z);
	t1 = zgen->Force.t * DEG90 / 90;
	Gen.GlobalForce.T = INT_FIX(t1);
	ActView = (struct View *)Views.ptr[0];

	//---------------------------------------------------------------------------
	// Inicializa la paleta y las tablas asociadas
	//---------------------------------------------------------------------------
	LoadPalette(zgen->Palette);

	// Background constant
	Gen.BackConst =
	    FixDiv(INT_FIX(Gen.BackTC.pPic->Height), INT_FIX(Gen.BackAngle));

	// Setup Views
	for (i = 0; i < Views.Number; i++) {
		view = (struct View *)Views.ptr[i];
	}
	// Prepare WallPtrs
	WallPtrs = (struct Wall **)CacheAlloc(Walls.Number * 2 * 8);
	for (n = 0; n < Regions.Number; n++) {
		new_region = (struct Region *)Regions.ptr[n];
		new_region->WallPtrs = &WallPtrs[NumWallPtrs];
		new_region->NumWallPtrs = 0;
		for (i = 0; i < Walls.Number; i++) {
			wall = (struct Wall *)Walls.ptr[i];
			if (wall->Front == new_region ||
			    wall->Back == new_region) {
				WallPtrs[NumWallPtrs] = wall;
				NumWallPtrs++;
				new_region->NumWallPtrs++;
			}
		}
	}
}
