#include "internal.h"

#define MAX_BLOCKS    8192

static void *Blocks[MAX_BLOCKS];
static int   NumBlocks;
static VPEByte  CacheEmpty;

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Inicializa el gestor de memoria
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void InitMemory(void)
{
// Init tables
  Pics.Number=0;
  Pics.Allocated=0;
  Pics.Size=sizeof(struct PicInfo);
  Points.Number=0;
  Points.Allocated=0;
  Points.Size=sizeof(struct Point);
  Regions.Number=0;
  Regions.Allocated=0;
  Regions.Size=sizeof(struct Region);
  Walls.Number=0;
  Walls.Allocated=0;
  Walls.Size=sizeof(struct Wall);
  Objects.Number=0;
  Objects.Allocated=0;
  Objects.Size=sizeof(struct Object);
  Views.Number=0;
  Views.Allocated=0;
  Views.Size=sizeof(struct View);
  // Set number of allocated blocks
  NumBlocks=0;
  // Allocate memory for math tables
  SinTable=(VPEFixed*)MemAlloc((DEG360+16)*sizeof(VPEFixed));
  if (SinTable==NULL)
    FatalError(ER_MEMORY,NULL);
  CosTable=(VPEFixed*)MemAlloc((DEG360+16)*sizeof(VPEFixed));
  if (CosTable==NULL)
    FatalError(ER_MEMORY,NULL);
  // Allocate memory for misc renderer data
  VDraws=(struct VDraw  *)MemZAlloc(MAX_VDRAWS*sizeof(struct VDraw));
  if (VDraws==NULL)
    FatalError(ER_MEMORY,NULL);
  MLines=(struct WLine  *)MemZAlloc(MAX_MLINES*sizeof(struct WLine));
  if (MLines==NULL)
    FatalError(ER_MEMORY,NULL);
  FPSTable=(VPEFixed *)MemZAlloc(FPSTABLE_NUM*FPSTABLE_SIZE*sizeof(VPEFixed));
  if (FPSTable==NULL)
    FatalError(ER_MEMORY,NULL);
  // Set cache flag
  CacheEmpty=TRUE;
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Finaliza el gestor de memoria
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void ShutMemory(void)
{
  MemFreeAll();
  ClearZone();
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Reserva memoria interna
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void *CacheAlloc(VPELong size)
{
  VPEByte *ptr;

  if (size<=0)
    FatalError(ER_CACHE0,NULL);
  ptr=(VPEByte *)MemZAlloc(size);
  if (ptr==NULL) {
    FatalError(ER_MEMORY,NULL);
  }
  return(ptr);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Aคade una entrada al gestor de memoria
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void *AddEntry(struct Table *t)
{
  int i;
  char *ptr;

  if (t->Number>=t->Allocated) {    // Additional allocation
    ptr=(char *)CacheAlloc(RES_INC*t->Size);
    t->Allocated+=RES_INC;
    Blocks[NumBlocks]=ptr;
    NumBlocks++;
    if (NumBlocks>=MAX_BLOCKS)
      FatalError(ER_MEMORY,NULL);
    for(i=t->Number;i<t->Allocated;i++,ptr+=t->Size)
      t->ptr[i]=(void *)ptr;
  }
  t->Number++;
  return(t->ptr[t->Number-1]);
}

//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ
//  Borra una entrada del gestor de memoria
//อออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออออ

void *DelEntry(struct Table *t, int index)
{
  void *ptr;

  t->Number--;
  ptr=t->ptr[index];
  t->ptr[index]=t->ptr[t->Number];
  t->ptr[t->Number]=ptr;

  if (t->Number>0) return(t->ptr[index]);
  else return(NULL);
}

