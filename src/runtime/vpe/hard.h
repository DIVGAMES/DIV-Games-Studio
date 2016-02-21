/*******************************
  SOME MACHINE DEPENDENT STUFF 
********************************/

#include <stdint.h>

#define FALSE           0
#define TRUE            1

#define DPMI_INT	0x31

//==========================
// General type definitions
//==========================
typedef unsigned char   BYTE;
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
typedef long int        LONG;
typedef short int       SHORT;
typedef long int       FIXED;


//==================
// Math definitions
//==================
#define DEG360		2048		// equals to 360 degrees
#define DEG360_2	11		// DEG360 log2
#define DEG180		1024		// 180 degrees
#define DEG90		512		// 90 degrees
#define ITAN_LEN	1024		// -1 to 1 (1024 steps)
#define F_DEG360	(2048<<16)	// 360 deg FIXED
#define F_DEG180	(1024<<16)	// 180 deg FIXED
#define F_DEG90		(512<<16)	// 90  deg FIXED
#define F_HITAN		(512<<16)	// Half of the ITAN_LEN (FIXED)
#define ITAN_MUL	9		// log2(ITAN_LEN>>1)


//===============
// Useful macros
//===============
#define FIX_INT(x)  (SHORT)((x)>>16)
#define INT_FIX(x)  ((FIXED)(x)<<16)
#define DOUBLE_FIX(x)   ((FIXED)((double)(x)*65536.0))
#define FIX_DOUBLE(x)   ((double)(x)/65536.0)
#define GET_LOW(px)	(WORD)(*px)
#define GET_HI(px)	(*((SHORT *)(px)+1))
#define SET_HI(px,a)    (*((SHORT *)(px)+1)=a
#define SET_LOW(px,a)	(*((WORD *)(px))=a)
#define FixMin(a,b)	((a)<(b)?(a):(b))
#define FixMax(a,b)	((a)>(b)?(a):(b))
#define FixMod(a)	((a)>=0?(a):(-(a)))
#define mod(a)		((a)>0?(a):(-(a)))


//=================
// DPMI structures
//=================
struct MemInfo {
	DWORD	LargestBlockAvail;	// Largest block avalable
	DWORD	MaxUnlockedPage;	// Maximum ulocked page
	DWORD	LargestLockablePage;	// Largest lockable page
	DWORD	LinearAddrSpace;	// Linear address space
	DWORD	NumFreePages;		// Number of free pages
	DWORD	NumPhysicalPages;	// Number of physical pages
	DWORD	TotalPhysicalPages;	// Total number of phys pages
	DWORD	FreeLinearAddrSpace;	// Free linear address space
	DWORD	PageFileSize;		// Size of a page file
	DWORD	reserved[3];		// Reserved stuff
};

void  FatalError(int, char *);		// Fatal error

void *MemAlloc(long int);		// Allocate some memory
void *MemZAlloc(long int);		// Allocate memory zeroing it
void  MemFree(void *);			// Free memory
void  MemFreeAll();			// Free all memory


FIXED FixMul(FIXED, FIXED);	// Fixed math (386 asm)
FIXED FixDiv(FIXED, FIXED);

#pragma aux FixMul =		\
  "imul ebx"            \
  "shrd eax, edx, 16"   \
  parm [eax] [ebx]      \
  value [eax]           \
	modify [edx];

#pragma aux FixDiv =		\
  "xor eax, eax"        \
  "shrd eax, edx, 16"   \
  "sar edx, 16"         \
  "idiv ebx"            \
  parm [edx] [ebx]      \
	value [eax];

