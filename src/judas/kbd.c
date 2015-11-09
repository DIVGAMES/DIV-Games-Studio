/*
 * Keyboard handler
 *
 * Doesn't "officially" belong to JUDAS but feel free to use! This is
 * blasphemy-ware too!
 *
 * Supports the Win95 keyboard extra keys!
 */

//#include <conio.h>
//#include <dos.h>
#include "judasmem.h"
#include "osdep.h"

typedef struct
{
  unsigned char rawcode;
  unsigned char ascii;
  char *name;
} KEY;

int kbd_init(void);
void kbd_uninit(void);
unsigned char waitkey(void);
unsigned char getkey(void);
int checkkey(unsigned char rawcode);
unsigned char getascii(unsigned char rawcode);
char *getname(unsigned char rawcode);
static int kbd_lock(void);
static void kbd_unlock(void);

unsigned short kbd_get_ds(void);
#pragma aux kbd_get_ds = \
"mov ax, ds" \
modify [ax] \
value [ax];

extern int kbd_code_lock_start;
extern int kbd_code_lock_end;
extern unsigned char kt[];
extern unsigned short kbd_ds;
extern void (kbd_handler)();

static unsigned char kbd_initialized = 0;
static void (__interrupt __far *kbd_oldvect)();
static void (__interrupt __far *kbd_newvect)() = &kbd_handler;

static KEY keyname[] = {{0x0e, 8, "BACKSPACE"},
                        {0x3a, 0, "CAPSLOCK"},
                        {0x1c, 13, "ENTER"},
                        {0x01, 27, "ESC"},
                        {0x45, 0, "NUMLOCK"},
                        {0x36, 0, "SHIFT"},
                        {0x46, 0, "SCR. LOCK"},
                        {0x39, ' ', "SPACE"},
                        {0x0f, 0, "TAB"},
                        {0x3b, 0, "F1"},
                        {0x3c, 0, "F2"},
                        {0x3d, 0, "F3"},
                        {0x3e, 0, "F4"},
                        {0x3f, 0, "F5"},
                        {0x40, 0, "F6"},
                        {0x41, 0, "F7"},
                        {0x42, 0, "F8"},
                        {0x43, 0, "F9"},
                        {0x44, 0, "F10"},
                        {0x57, 0, "F11"},
                        {0x58, 0, "F12"},
                        {0x54, 0, "SYSTEM REQ."},
                        {0x4c, '5', "GREY 5"},
                        {0x37, '*', "GREY *"},
                        {0x4a, '-', "GREY -"},
                        {0x4e, '+', "GREY +"},
                        {0x1e, 'A', "A"},
                        {0x31, 'N', "N"},
                        {0x30, 'B', "B"},
                        {0x18, 'O', "O"},
                        {0x2e, 'C', "C"},
                        {0x19, 'P', "P"},
                        {0x20, 'D', "D"},
                        {0x10, 'Q', "Q"},
                        {0x12, 'E', "E"},
                        {0x13, 'R', "R"},
                        {0x21, 'F', "F"},
                        {0x1f, 'S', "S"},
                        {0x22, 'G', "G"},
                        {0x14, 'T', "T"},
                        {0x23, 'H', "H"},
                        {0x16, 'U', "U"},
                        {0x17, 'I', "I"},
                        {0x2f, 'V', "V"},
                        {0x24, 'J', "J"},
                        {0x11, 'W', "W"},
                        {0x25, 'K', "K"},
                        {0x2D, 'X', "X"},
                        {0x26, 'L', "L"},
                        {0x15, 'Y', "Y"},
                        {0x32, 'M', "M"},
                        {0x2c, 'Z', "Z"},
                        {0x02, '1', "1"},
                        {0x03, '2', "2"},
                        {0x04, '3', "3"},
                        {0x05, '4', "4"},
                        {0x06, '5', "5"},
                        {0x07, '6', "6"},
                        {0x08, '7', "7"},
                        {0x09, '8', "8"},
                        {0x0a, '9', "9"},
                        {0x0b, '0', "0"},
                        {0x0c, '-', "-"},
                        {0x0d, '=', "="},
                        {0x1a, '[', "["},
                        {0x1b, ']', "]"},
                        {0x1c, ';', ";"},
                        {0x27, ';', ";"},
                        {0x28, 0x27, "'"},
                        {0x29, '`', "`"},
                        {0x33, ',', ","},
                        {0x34, '.', "."},
                        {0x35, '/', "/"},
                        {0x2b, '\\', "\\"},
                        {0x56, 0, "<>"},
                        {0x37 + 0x80, 0, "PRT. SCR."},
                        {0x38 + 0x80, 0, "ALT"},
                        {0x1d + 0x80, 0, "CTRL"},
                        {0x35 + 0x80, '/', "GREY /"},
                        {0x53 + 0x80, 8, "DELETE"},
                        {0x50 + 0x80, 0, "CRS. DOWN"},
                        {0x4f + 0x80, 0, "END"},
                        {0x47 + 0x80, 0, "HOME"},
                        {0x52 + 0x80, 0, "INSERT"},
                        {0x4b + 0x80, 0, "CRS. LEFT"},
                        {0x51 + 0x80, 0, "PAGE DOWN"},
                        {0x49 + 0x80, 0, "PAGE UP"},
                        {0x4d + 0x80, 0, "CRS. RIGHT"},
                        {0x48 + 0x80, 0, "CRS. UP"},
                        {0x5b + 0x80, 0, "WINDOWS KEY"},
                        {0x5d + 0x80, 0, "MENU KEY"},
                        {0xff, 0, "?"}};

int kbd_init(void)
{
	if (kbd_initialized) return 1;
        if (!kbd_lock()) return 0;
        kbd_ds = kbd_get_ds();
        _dos_setvect(9, kbd_newvect);
        kbd_initialized = 1;
        return 1;
}

void kbd_uninit(void)
{
	if (!kbd_initialized) return;
        _dos_setvect(9, kbd_oldvect);
        kbd_unlock();
        kbd_initialized = 0;
}


static int kbd_lock(void)
{
        if (!judas_memlock(&kbd_code_lock_start, (int)&kbd_code_lock_end - (int)&kbd_code_lock_start)) return 0;
        return 1;
}

static void kbd_unlock(void)
{
        judas_memunlock(&kbd_code_lock_start, (int)&kbd_code_lock_end - (int)&kbd_code_lock_start);
}

unsigned char waitkey(void)
{
        int index;

        for (;;)
        {
                for (index = 0; index < 255; index++)
                {
                        if (kt[index])
                        {
                                kt[index] = 0;
                                return index;
                        }
                }
        }
}

unsigned char getkey(void)
{
        int index;

        for (index = 0; index < 255; index++)
        {
                if (kt[index])
                {
                        kt[index] = 0;
                        return index;
                }
        }
        return 0;
}

int checkkey(unsigned char rawcode)
{
        if (kt[rawcode])
        {
                kt[rawcode] = 0;
                return 1;
        }
        return 0;
}

unsigned char getascii(unsigned char rawcode)
{
  KEY *ptr = &keyname[0];

  while (ptr->rawcode != 255)
  {
    if (ptr->rawcode == rawcode)
    {
      return ptr->ascii;
    }
    ptr++;
  }
  return 0x00;
}

char *getname(unsigned char rawcode)
{
  KEY *ptr = &keyname[0];

  while (ptr->rawcode != 0xff)
  {
    if (ptr->rawcode == rawcode)
    {
      return ptr->name;
    }
    ptr++;
  }
  return ptr->name;
}


