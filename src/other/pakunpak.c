////// PAKFILE UNPACKER
////// By MikeDX
////// For Dom Cook

#ifdef EMSCRIPTEN
#include <emscripten.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlib.h>

typedef unsigned char byte;
char full[255];
int res;

char packfile[128];
int npackfiles;

struct _packdir {
	char filename[16];
	int offset;
	int len;
	int len_desc;
};

struct _packdir *packdir = NULL;

byte *packptr;

int npackfiles = 0;

void read_packfile(char *filepath)
{
	FILE *f, *fout;
	char *ptr;
	int n;
	unsigned long len_desc;

	f = fopen(packfile, "rb");
	if (f == NULL) {
		printf("Cannot open %s for reading\n", packfile);
		return;
	}
	for (n = 0; n < npackfiles; n++) {
		len_desc = packdir[n].len_desc;

		packptr = (byte *)malloc(len_desc);

		if (packptr == NULL) {
			printf("cannot allocate %lu bytes\n", len_desc);
			fclose(f);
			return;
		}

		ptr = (char *)malloc(packdir[n].len);

		if (packptr == NULL) {
			printf("cannot allocate %lu bytes\n", len_desc);
			fclose(f);
			return;
		}

		fseek(f, packdir[n].offset, SEEK_SET);
		res = fread(ptr, 1, packdir[n].len, f);

		if (!uncompress(packptr, &len_desc, (byte *)ptr,
				packdir[n].len)) {
			fout = fopen(packdir[n].filename, "rb");

			if (fout != NULL) {
				printf("Skipping existing file %s\n",
				       packdir[n].filename);
				fclose(fout);
			} else {
				printf("Extracting %s\n", packdir[n].filename);

				fout = fopen(packdir[n].filename, "wb");

				if (fout != NULL) {
					fwrite(packptr, 1, packdir[n].len_desc,
					       fout);
					fclose(fout);
				}
			}
		}

		free(ptr);
		free(packptr);
	}

	fclose(f);
}

void busca_packfile(char *path)
{
	int prg_id = 0;
	char head[8];
	int id[3];
	FILE *f;
	int n, nfiles;
	char buf[1024];

	printf("Trying to open PAKfile %s\n", path);

	f = fopen(path, "rb");

	if (f == NULL) {
		printf("Could not open %s\n", path);
		return;
	}

	res = fread(head, 1, 8, f);
	res = fread(id, 4, 3, f);
	res = fread(&nfiles, 4, 1, f);

	if (strcmp(head, "dat\x1a\x0d\x0a")) {
		printf("Does not look like a PAKfile\n");
		fclose(f);
		return;
	}
	if (nfiles <= 0) {
		fclose(f);
		return;
	}

	if (id[0] != prg_id && id[1] != prg_id && id[2] != prg_id) {
		printf("prg_id mismatch\n");
		fclose(f);
		return;
	}

	packdir = (struct _packdir *)malloc(nfiles * sizeof(struct _packdir));

	if (packdir == NULL) {
		printf("Could not allocate %zu bytes for read\n",
		       nfiles * sizeof(struct _packdir));
		fclose(f);
		return;
	}

	if ((res = fread(packdir, sizeof(struct _packdir), nfiles, f)) ==
	    nfiles) {
		for (n = 0; n < nfiles; n++) {
			strcpy(buf, packdir[n].filename);
		}
		strcpy(packfile, path);
		npackfiles = nfiles;
	}

	fclose(f);
	return;
}

int main(int argc, char *argv[])
{

	if (argc < 2) {
		printf("DIV2 PAKfile unpacker - (c)2016 MikeDX\n");
		printf("Usage: %s PAKFILE\n\n", argv[0]);
		return 0;
	}
#ifdef EMSCRIPTEN
	EM_ASM(FS.mkdir('/working');
	       FS.mount(NODEFS, {root : '.'}, '/working'););
	chdir("/working");
#endif

	busca_packfile(argv[1]);

	//	printf("%x\n",packdir);

	if (packdir == NULL) {
		printf("Failed to load PAKfile\n");
		return -1;
	}
	printf("Found %d files in %s\n", npackfiles, argv[1]);

	if (npackfiles > 0) {
		read_packfile("all");
	}

	free(packdir);

	printf("Finished.\n\n");

	return 0;
}