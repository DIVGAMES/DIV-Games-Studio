
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//#define PEDEBUG

extern char *dll_error;

typedef char BYTE;
typedef short WORD;
typedef long LONG;

typedef unsigned char UBYTE;
typedef unsigned short UWORD;
typedef unsigned long ULONG;

//
// Image Format
//

#define IMAGE_DOS_SIGNATURE 0x5A4D    // MZ
#define IMAGE_OS2_SIGNATURE 0x454E    // NE
#define IMAGE_OS2_SIGNATURE_LE 0x454C // LE
#define IMAGE_NT_SIGNATURE 0x00004550 // PE00
#pragma align 1
typedef struct _IMAGE_DOS_HEADER { // DOS .EXE header
	UWORD e_magic;		   // Magic number
	UWORD e_cblp;		   // Bytes on last page of file
	UWORD e_cp;		   // Pages in file
	UWORD e_crlc;		   // Relocations
	UWORD e_cparhdr;	   // Size of header in paragraphs
	UWORD e_minalloc;	  // Minimum extra paragraphs needed
	UWORD e_maxalloc;	  // Maximum extra paragraphs needed
	UWORD e_ss;		   // Initial (relative) SS value
	UWORD e_sp;		   // Initial SP value
	UWORD e_csum;		   // Checksum
	UWORD e_ip;		   // Initial IP value
	UWORD e_cs;		   // Initial (relative) CS value
	UWORD e_lfarlc;		   // File address of relocation table
	UWORD e_ovno;		   // Overlay number
	UWORD e_res[4];		   // Reserved words
	UWORD e_oemid;		   // OEM identifier (for e_oeminfo)
	UWORD e_oeminfo;	   // OEM information; e_oemid specific
	UWORD e_res2[10];	  // Reserved words
	LONG e_lfanew;		   // File address of new exe header
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

//
// File header format.
//

typedef struct _IMAGE_FILE_HEADER {
	UWORD Machine;
	UWORD NumberOfSections;
	ULONG TimeDateStamp;
	ULONG PointerToSymbolTable;
	ULONG NumberOfSymbols;
	UWORD SizeOfOptionalHeader;
	UWORD Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;

#define IMAGE_SIZEOF_FILE_HEADER 20

#define IMAGE_FILE_RELOCS_STRIPPED 0x0001 // Relocation info stripped from file.
#define IMAGE_FILE_EXECUTABLE_IMAGE                                            \
	0x0002 // File is executable  (i.e. no unresolved externel references).
#define IMAGE_FILE_LINE_NUMS_STRIPPED 0x0004 // Line nunbers stripped from file.
#define IMAGE_FILE_LOCAL_SYMS_STRIPPED                                         \
	0x0008				 // Local symbols stripped from file.
#define IMAGE_FILE_MINIMAL_OBJECT 0x0010 // Reserved.
#define IMAGE_FILE_UPDATE_OBJECT 0x0020  // Reserved.
#define IMAGE_FILE_16BIT_MACHINE 0x0040  // 16 bit word machine.
#define IMAGE_FILE_BYTES_REVERSED_LO                                           \
	0x0080				// Bytes of machine word are reversed.
#define IMAGE_FILE_32BIT_MACHINE 0x0100 // 32 bit word machine.
#define IMAGE_FILE_DEBUG_STRIPPED                                              \
	0x0200 // Debugging info stripped from file in .DBG file
#define IMAGE_FILE_PATCH 0x0400  // Reserved.
#define IMAGE_FILE_SYSTEM 0x1000 // System File.
#define IMAGE_FILE_DLL 0x2000    // File is a DLL.
#define IMAGE_FILE_BYTES_REVERSED_HI                                           \
	0x8000 // Bytes of machine word are reversed.

#define IMAGE_FILE_MACHINE_UNKNOWN 0
#define IMAGE_FILE_MACHINE_I860 0x14d  // Intel 860.
#define IMAGE_FILE_MACHINE_I386 0x14c  // Intel 386.
#define IMAGE_FILE_MACHINE_R3000 0x162 // MIPS little-endian, 0540 big-endian
#define IMAGE_FILE_MACHINE_R4000 0x166 // MIPS little-endian
#define IMAGE_FILE_MACHINE_ALPHA 0x184 // Alpha_AXP

//
// Directory format.
//

typedef struct _IMAGE_DATA_DIRECTORY {
	ULONG VirtualAddress;
	ULONG Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;

#define IMAGE_NUMBEROF_DIRECTORY_ENTRIES 16

//
// Optional header format.
//

typedef struct _IMAGE_OPTIONAL_HEADER {
	//
	// Standard fields.
	//

	UWORD Magic;
	UBYTE MajorLinkerVersion;
	UBYTE MinorLinkerVersion;
	ULONG SizeOfCode;
	ULONG SizeOfInitializedData;
	ULONG SizeOfUninitializedData;
	ULONG AddressOfEntryPoint;
	ULONG BaseOfCode;
	ULONG BaseOfData;

	//
	// NT additional fields.
	//

	ULONG ImageBase;
	ULONG SectionAlignment;
	ULONG FileAlignment;
	UWORD MajorOperatingSystemVersion;
	UWORD MinorOperatingSystemVersion;
	UWORD MajorImageVersion;
	UWORD MinorImageVersion;
	UWORD MajorSubsystemVersion;
	UWORD MinorSubsystemVersion;
	ULONG Reserved1;
	ULONG SizeOfImage;
	ULONG SizeOfHeaders;
	ULONG CheckSum;
	UWORD Subsystem;
	UWORD DllCharacteristics;
	ULONG SizeOfStackReserve;
	ULONG SizeOfStackCommit;
	ULONG SizeOfHeapReserve;
	ULONG SizeOfHeapCommit;
	ULONG LoaderFlags;
	ULONG NumberOfRvaAndSizes;
	IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} IMAGE_OPTIONAL_HEADER, *PIMAGE_OPTIONAL_HEADER;

#define IMAGE_SIZEOF_STD_OPTIONAL_HEADER 28
#define IMAGE_SIZEOF_NT_OPTIONAL_HEADER 224

typedef struct _IMAGE_NT_HEADERS {
	ULONG Signature;
	IMAGE_FILE_HEADER FileHeader;
	IMAGE_OPTIONAL_HEADER OptionalHeader;
} IMAGE_NT_HEADERS, *PIMAGE_NT_HEADERS;

// Subsystem Values

#define IMAGE_SUBSYSTEM_UNKNOWN 0 // Unknown subsystem.
#define IMAGE_SUBSYSTEM_NATIVE 1  // Image doesn't require a subsystem.
#define IMAGE_SUBSYSTEM_WINDOWS_GUI                                            \
	2 // Image runs in the Windows GUI subsystem.
#define IMAGE_SUBSYSTEM_WINDOWS_CUI                                            \
	3 // Image runs in the Windows character subsystem.
#define IMAGE_SUBSYSTEM_OS2_CUI 5 // image runs in the OS/2 character subsystem.
#define IMAGE_SUBSYSTEM_POSIX_CUI                                              \
	7 // image run  in the Posix character subsystem.

// Dll Characteristics

#define IMAGE_LIBRARY_PROCESS_INIT                                             \
	1 // Dll has a process initialization routine.
#define IMAGE_LIBRARY_PROCESS_TERM 2 // Dll has a thread termination routine.
#define IMAGE_LIBRARY_THREAD_INIT 4  // Dll has a thread initialization routine.
#define IMAGE_LIBRARY_THREAD_TERM 8  // Dll has a thread termination routine.

//
// Loader Flags
//

#define IMAGE_LOADER_FLAGS_BREAK_ON_LOAD 0x00000001
#define IMAGE_LOADER_FLAGS_DEBUG_ON_LOAD 0x00000002

// Directory Entries

#define IMAGE_DIRECTORY_ENTRY_EXPORT 0       // Export Directory
#define IMAGE_DIRECTORY_ENTRY_IMPORT 1       // Import Directory
#define IMAGE_DIRECTORY_ENTRY_RESOURCE 2     // Resource Directory
#define IMAGE_DIRECTORY_ENTRY_EXCEPTION 3    // Exception Directory
#define IMAGE_DIRECTORY_ENTRY_SECURITY 4     // Security Directory
#define IMAGE_DIRECTORY_ENTRY_BASERELOC 5    // Base Relocation Table
#define IMAGE_DIRECTORY_ENTRY_DEBUG 6	// Debug Directory
#define IMAGE_DIRECTORY_ENTRY_COPYRIGHT 7    // Description String
#define IMAGE_DIRECTORY_ENTRY_GLOBALPTR 8    // Machine Value (MIPS GP)
#define IMAGE_DIRECTORY_ENTRY_TLS 9	  // TLS Directory
#define IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG 10 // Load Configuration Directory

//
// Section header format.
//

#define IMAGE_SIZEOF_SHORT_NAME 8

typedef struct _IMAGE_SECTION_HEADER {
	UBYTE Name[IMAGE_SIZEOF_SHORT_NAME];
	char *PhysicalAddress;
	/*      union {
			void far *PhysicalAddress;
			ULONG   VirtualSize;
	} Misc;
*/
	ULONG VirtualAddress;
	ULONG SizeOfRawData;
	ULONG PointerToRawData;
	ULONG PointerToRelocations;
	ULONG PointerToLinenumbers;
	UWORD NumberOfRelocations;
	UWORD NumberOfLinenumbers;
	ULONG Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

#define IMAGE_SIZEOF_SECTION_HEADER 40

#define IMAGE_SCN_TYPE_REGULAR 0x00000000 //
#define IMAGE_SCN_TYPE_DUMMY 0x00000001   // Reserved.
#define IMAGE_SCN_TYPE_NO_LOAD 0x00000002 // Reserved.
#define IMAGE_SCN_TYPE_GROUPED 0x00000004 // Used for 16-bit offset code.
#define IMAGE_SCN_TYPE_NO_PAD 0x00000008  // Reserved.
#define IMAGE_SCN_TYPE_COPY 0x00000010    // Reserved.

#define IMAGE_SCN_CNT_CODE 0x00000020 // Section contains code.
#define IMAGE_SCN_CNT_INITIALIZED_DATA                                         \
	0x00000040 // Section contains initialized data.
#define IMAGE_SCN_CNT_UNINITIALIZED_DATA                                       \
	0x00000080 // Section contains uninitialized data.

#define IMAGE_SCN_LNK_OTHER 0x00000100 // Reserved.
#define IMAGE_SCN_LNK_INFO                                                     \
	0x00000200 // Section contains comments or some other type of
		   // information.
#define IMAGE_SCN_LNK_OVERLAY 0x00000400 // Section contains an overlay.
#define IMAGE_SCN_LNK_REMOVE                                                   \
	0x00000800 // Section contents will not become part of image.
#define IMAGE_SCN_LNK_COMDAT 0x00001000 // Section contents comdat.

#define IMAGE_SCN_ALIGN_1UBYTES 0x00100000 //
#define IMAGE_SCN_ALIGN_2UBYTES 0x00200000 //
#define IMAGE_SCN_ALIGN_4UBYTES 0x00300000 //
#define IMAGE_SCN_ALIGN_8UBYTES 0x00400000 //
#define IMAGE_SCN_ALIGN_16UBYTES                                               \
	0x00500000 // Default alignment if no others are specified.
#define IMAGE_SCN_ALIGN_32UBYTES 0x00600000 //
#define IMAGE_SCN_ALIGN_64UBYTES 0x00700000 //

#define IMAGE_SCN_MEM_DISCARDABLE 0x02000000 // Section can be discarded.
#define IMAGE_SCN_MEM_NOT_CACHED 0x04000000  // Section is not cachable.
#define IMAGE_SCN_MEM_NOT_PAGED 0x08000000   // Section is not pageable.
#define IMAGE_SCN_MEM_SHARED 0x10000000      // Section is shareable.
#define IMAGE_SCN_MEM_EXECUTE 0x20000000     // Section is executable.
#define IMAGE_SCN_MEM_READ 0x40000000	// Section is readable.
#define IMAGE_SCN_MEM_WRITE 0x80000000       // Section is writeable.

//
// Export Format
//

typedef struct _IMAGE_EXPORT_DIRECTORY {
	ULONG Characteristics;
	ULONG TimeDateStamp;
	UWORD MajorVersion;
	UWORD MinorVersion;
	ULONG Name;
	ULONG Base;
	ULONG NumberOfFunctions;
	ULONG NumberOfNames;
	ULONG AddressOfFunctions;
	ULONG AddressOfNames;
	ULONG AddressOfNameOrdinals;
} IMAGE_EXPORT_DIRECTORY, *PIMAGE_EXPORT_DIRECTORY;

//
// Based relocation format.
//

typedef struct _IMAGE_BASE_RELOCATION {
	ULONG VirtualAddress;
	ULONG SizeOfBlock;
	//  UWORD   TypeOffset[1];
} IMAGE_BASE_RELOCATION, *PIMAGE_BASE_RELOCATION;

#define IMAGE_SIZEOF_BASE_RELOCATION 8

//
// Based relocation types.
//

#define IMAGE_REL_BASED_ABSOLUTE 0
#define IMAGE_REL_BASED_HIGH 1
#define IMAGE_REL_BASED_LOW 2
#define IMAGE_REL_BASED_HIGHLOW 3
#define IMAGE_REL_BASED_HIGHADJ 4
#define IMAGE_REL_BASED_MIPS_JMPADDR 5
#define IMAGE_REL_BASED_I860_BRADDR 6
#define IMAGE_REL_BASED_I860_SPLIT 7

typedef struct PE {
	IMAGE_NT_HEADERS nh;
	IMAGE_SECTION_HEADER *sh;
} PE;

char *dll_error = "No error";
#pragma align 4

static void *PE_GetLinearAddress(PE *p, ULONG va)
/*
	Translates a virtual address into the PE file to the normal memory
	address.
*/
{
	int t;
	IMAGE_SECTION_HEADER *s;

	for (t = 0, s = p->sh; t < p->nh.FileHeader.NumberOfSections;
	     t++, s++) {

		if (va >= s->VirtualAddress &&
		    va < (s->VirtualAddress + s->SizeOfRawData)) {
			return (s->PhysicalAddress + va - s->VirtualAddress);
		}
	}
	return NULL;
}

static IMAGE_DATA_DIRECTORY *PE_ImageDirectory(PE *p, int idir)
{
	if (idir >= p->nh.OptionalHeader.NumberOfRvaAndSizes) {
		return NULL;
	}
	return (&p->nh.OptionalHeader.DataDirectory[idir]);
}

static void *PE_ImageDirectoryOffset(PE *p, int idir)
{
	IMAGE_DATA_DIRECTORY *dd;
	if ((dd = PE_ImageDirectory(p, idir)) == NULL)
		return NULL;
	return (PE_GetLinearAddress(p, dd->VirtualAddress));
}

static int PE_LoadSection(FILE *in, IMAGE_SECTION_HEADER *h)
{
	// no data in this section? -> skip

	if (h->SizeOfRawData == 0)
		return 1;

	// allocate enough memory to hold section

	if ((h->PhysicalAddress = malloc(h->SizeOfRawData)) == 0) {
		dll_error = "Error allocating section space";
		return 0;
	};

	// BSS section ? -> don't try to load it.

	if (h->Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA)
		return 1;

	// load every other type of section

	fseek(in, h->PointerToRawData, SEEK_SET);
	fread(h->PhysicalAddress, h->SizeOfRawData, 1, in);

#ifdef PEDEBUG
	printf("Section: '%s'\n"
	       "Charactr: %lx\n"
	       "Physical address:   %p\n"
	       "Virtual address:    %lx\n"
	       "Size of raw data:   %ld\n"
	       "pointer to raw data: %ld\n",
	       h->Name, h->Characteristics, h->PhysicalAddress,
	       h->VirtualAddress, h->SizeOfRawData, h->PointerToRawData);
#endif

	return 1;
}

static int PE_ApplyReloc(PE *p, IMAGE_BASE_RELOCATION *br)
/*
	Apply the fixups in a single fixup block
*/
{
	UWORD *reloc;
	char *pagebase, *relocadr;
	int numrelocs, t;
	ULONG relocval;

#ifdef PEDEBUG
	printf("Page rva %ld, block size %ld\n", br->VirtualAddress,
	       br->SizeOfBlock);
#endif

	// get base address of section to be relocated:

	pagebase = PE_GetLinearAddress(p, br->VirtualAddress);

	// find reloc word table address & number of entries:

	reloc = (UWORD *)((char *)br + sizeof(IMAGE_BASE_RELOCATION));
	numrelocs =
	    (br->SizeOfBlock - sizeof(IMAGE_BASE_RELOCATION)) / sizeof(UWORD);

#ifdef PEDEBUG
	printf("%d relocs at %p\n", numrelocs, reloc);
#endif

	for (t = 0; t < numrelocs; t++) {

#ifdef PEDEBUG
		printf("Type %d, Offset %u\n", reloc[t] >> 12,
		       reloc[t] & 0xfff);
#endif
		// determine address where to apply relocation:

		relocadr = pagebase + (reloc[t] & 0xfff);

		switch (reloc[t] >> 12) {

		case IMAGE_REL_BASED_ABSOLUTE:
			break;

		case IMAGE_REL_BASED_HIGH:

			// fetch word value at that place:
			relocval = *((UWORD *)relocadr);
			relocval <<= 16;

			// make it into a RVA
			relocval -= p->nh.OptionalHeader.ImageBase;

			// find the physical address it belongs to:
			relocval = (ULONG)PE_GetLinearAddress(p, relocval);

			// write back that value:
			*((UWORD *)relocadr) = relocval >> 16;
			break;

		case IMAGE_REL_BASED_HIGHLOW:

			// fetch long value at that place:
			relocval = *((ULONG *)relocadr);

			// make it into a RVA
			relocval -= p->nh.OptionalHeader.ImageBase;

			// find the physical address it belongs to:
			relocval = (ULONG)PE_GetLinearAddress(p, relocval);

			// write back that value:
			*((ULONG *)relocadr) = relocval;
			break;

		default:
			dll_error = "Unsupported relocation type";
			return 0;
		}
	}
	return 1;
}

static int PE_Relocate(PE *p)
{
	IMAGE_DATA_DIRECTORY *dd;
	IMAGE_BASE_RELOCATION *br;
	ULONG t;

	/* find relocation data directory, if any:
	   p.s.: no relocation data is NoT an error */

	if ((dd = PE_ImageDirectory(p, IMAGE_DIRECTORY_ENTRY_BASERELOC)) ==
		NULL ||
	    dd->Size == 0 ||
	    (br = PE_GetLinearAddress(p, dd->VirtualAddress)) == NULL)
		return 1;

	// fetch total number of bytes in relocation tables

	t = dd->Size;

	// do all relocation tables until we're done

	while (t > 0) {
		if (!PE_ApplyReloc(p, br))
			return 0;
		t -= br->SizeOfBlock;
		br = (IMAGE_BASE_RELOCATION *)((char *)br + br->SizeOfBlock);
	}

	return 1;
}

/*************************************************

	Public Functions:

		- PE_ReadFP()
		- PE_ReadFN()
		- PE_Free()
		- PE_ImportFnc()

*************************************************/

PE *PE_ReadFP(FILE *in)
{
	int t;
	UWORD id = 0;
	IMAGE_DOS_HEADER dh;
	PE *p;

	// allocate PE structure

	if ((p = calloc(1, sizeof(PE))) == NULL) {
		dll_error = "Error allocating PE structure";
		return NULL;
	}

	// load dos header

	if (!fread(&dh, sizeof(IMAGE_DOS_HEADER), 1, in) ||
	    dh.e_magic != IMAGE_DOS_SIGNATURE) {
		dll_error = "Not a dos executable header";
		PE_Free(p);
		return NULL;
	}

	// seek to the PE header

#ifdef PEDEBUG
	printf("NT header offset at 0x%lx\n", dh.e_lfanew);
#endif

	fseek(in, dh.e_lfanew, SEEK_SET);

	// read the magic ID

	if (!fread(&id, 2, 1, in) || id != IMAGE_NT_SIGNATURE) {
		dll_error = "Not a portable executable format file.";
		PE_Free(p);
		return NULL;
	}

	// reset the file position to the PE header.. again

	fseek(in, dh.e_lfanew, SEEK_SET);

	// read the PE Windows NT headers

	if (!fread(&p->nh, sizeof(IMAGE_NT_HEADERS), 1, in) ||
	    p->nh.Signature != IMAGE_NT_SIGNATURE ||
	    p->nh.FileHeader.SizeOfOptionalHeader !=
		IMAGE_SIZEOF_NT_OPTIONAL_HEADER ||
	    p->nh.FileHeader.Machine != IMAGE_FILE_MACHINE_I386) {
		dll_error = "Unknown portable executable format.";
		PE_Free(p);
		return NULL;
	}

#ifdef PEDEBUG
	printf("Number of sections: %d\n", p->nh.FileHeader.NumberOfSections);
#endif

	// read section headers

	if ((p->sh = calloc(p->nh.FileHeader.NumberOfSections,
			    sizeof(IMAGE_SECTION_HEADER))) == NULL ||
	    !fread(p->sh, sizeof(IMAGE_SECTION_HEADER),
		   p->nh.FileHeader.NumberOfSections, in)) {
		dll_error = "Failed allocating or loading section headers";
		PE_Free(p);
		return NULL;
	}

	// read section bodys

	for (t = 0; t < p->nh.FileHeader.NumberOfSections; t++) {
		p->sh[t].PhysicalAddress = NULL;
		if (!PE_LoadSection(in, &p->sh[t])) {
			PE_Free(p);
			return NULL;
		}
	}

	// relocate image.

	if (!PE_Relocate(p)) {
		PE_Free(p);
		return NULL;
	}

	return p;
}

void PE_Free(PE *p)
{
	int t;

	// free sections, if any:

	if (p->sh != NULL) {
		for (t = 0; t < p->nh.FileHeader.NumberOfSections; t++) {

			// sections body present? -> remove it:

			if (p->sh[t].PhysicalAddress != NULL) {
				free(p->sh[t].PhysicalAddress);
			}
		}
		free(p->sh);
	}
	free(p);
}

FILE *div_open_file(char *file);

PE *PE_ReadFN(char *filename)
{
	FILE *in;
	PE *p;

	if ((in = div_open_file(filename)) == NULL) {
		dll_error = "error opening file";
		return NULL;
	}

	p = PE_ReadFP(in);

	fclose(in);
	return p;
}

void *PE_ImportFnc(PE *p, char *funcname)
{
	IMAGE_EXPORT_DIRECTORY *e;
	ULONG *nametable, *rvatable;
	UWORD *ordtable;
	// char *s;
	int t, u;
	char cwork[256];

	// find export directory address:

	e = PE_ImageDirectoryOffset(p, IMAGE_DIRECTORY_ENTRY_EXPORT);

	// not present, return NULL.

	if (e == NULL)
		return NULL;

	// find lookup table addresses

	nametable = PE_GetLinearAddress(p, e->AddressOfNames);
	ordtable = PE_GetLinearAddress(p, e->AddressOfNameOrdinals);
	rvatable = PE_GetLinearAddress(p, e->AddressOfFunctions);

	// if any one of them isn't present, return NULL

	if (nametable == NULL || ordtable == NULL || rvatable == NULL)
		return NULL;

	// try to find the function we are looking for

	for (t = 0; t < e->NumberOfNames; t++) {

		strcpy(cwork, (char *)PE_GetLinearAddress(p, nametable[t]));
		if (cwork[1] == '?') {
			strcpy(cwork, cwork + 2);
			u = 0;
			while (cwork[u] != '$') {
				u++;
			}
			cwork[u] = '_';
			cwork[u + 1] = 0;
		}
#ifdef PEDEBUG
		printf("Seek Function Name %s\n", funcname);
		printf("Function Name %s\n", cwork);
#endif
		if (!strcmp(funcname, cwork)) {

			//		if(!strcmp(funcname,(char
			//*)PE_GetLinearAddress(p,nametable[t]))){

			// found it, so return the address

			return (PE_GetLinearAddress(p, rvatable[ordtable[t]]));
		}
	}
	return NULL;
}
