/*******************************
  SOME MACHINE DEPENDENT STUFF
********************************/

#include <stdint.h>

#define FALSE 0
#define TRUE 1

#define DPMI_INT 0x31

//==========================
// General type definitions
//==========================
typedef unsigned char VPEByte;
typedef unsigned short VPEWord;
typedef uint32_t VPEDword;
typedef int32_t VPELong;
typedef int16_t VPEShort;
typedef int32_t VPEFixed;

//==================
// Math definitions
//==================
#define DEG360 2048	   // equals to 360 degrees
#define DEG360_2 11	   // DEG360 log2
#define DEG180 1024	   // 180 degrees
#define DEG90 512	     // 90 degrees
#define ITAN_LEN 1024	 // -1 to 1 (1024 steps)
#define F_DEG360 (2048 << 16) // 360 deg VPEFixed
#define F_DEG180 (1024 << 16) // 180 deg VPEFixed
#define F_DEG90 (512 << 16)   // 90  deg VPEFixed
#define F_HITAN (512 << 16)   // Half of the ITAN_LEN (VPEFixed)
#define ITAN_MUL 9	    // log2(ITAN_LEN>>1)

//===============
// Useful macros
//===============
#define FIX_INT(x) (VPEShort)((x) >> 16)
#define INT_FIX(x) ((VPEFixed)(x) << 16)
#define DOUBLE_FIX(x) ((VPEFixed)((double)(x)*65536.0))
#define FIX_DOUBLE(x) ((double)(x) / 65536.0)
#define GET_LOW(px) (VPEWord)(*px)
#define GET_HI(px) (*((VPEShort *)(px) + 1))
#define SET_HI(px, a)    (*((VPEShort *)(px)+1)=a
#define SET_LOW(px, a) (*((VPEWord *)(px)) = a)
#define FixMin(a, b) ((a) < (b) ? (a) : (b))
#define FixMax(a, b) ((a) > (b) ? (a) : (b))
#define FixMod(a) ((a) >= 0 ? (a) : (-(a)))
#define mod(a) ((a) > 0 ? (a) : (-(a)))

//=================
// DPMI structures
//=================
struct MemInfo {
	VPEDword LargestBlockAvail;   // Largest block avalable
	VPEDword MaxUnlockedPage;     // Maximum ulocked page
	VPEDword LargestLockablePage; // Largest lockable page
	VPEDword LinearAddrSpace;     // Linear address space
	VPEDword NumFreePages;	// Number of free pages
	VPEDword NumPhysicalPages;    // Number of physical pages
	VPEDword TotalPhysicalPages;  // Total number of phys pages
	VPEDword FreeLinearAddrSpace; // Free linear address space
	VPEDword PageFileSize;	// Size of a page file
	VPEDword reserved[3];	 // Reserved stuff
};

void FatalError(int, char *); // Fatal error

void *MemAlloc(long int);  // Allocate some memory
void *MemZAlloc(long int); // Allocate memory zeroing it
void MemFree(void *);      // Free memory
void MemFreeAll();	 // Free all memory

VPEFixed FixMul(VPEFixed, VPEFixed); // Fixed math (386 asm)
VPEFixed FixDiv(VPEFixed, VPEFixed);

#pragma aux FixMul =                                                           \
    "imul ebx"                                                                 \
    "shrd eax, edx, 16" parm[eax][ebx] value[eax] modify[edx];

#pragma aux FixDiv = "xor eax, eax"                                            \
		     "shrd eax, edx, 16"                                       \
		     "sar edx, 16"                                             \
		     "idiv ebx" parm[edx][ebx] value[eax];
