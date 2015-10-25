#include <dos.h>
#include <io.h>
#include <mem.h>
#include <i86.h>

#define CDROM           0x21
#define EJECT_TRAY      0
#define RESET           2
#define CLOSE_TRAY      5
#define MEDIA_CHANGE    9
#define BUSY            512
#define TRACK_MASK      208

typedef struct playinfo {
  unsigned char control;
  unsigned char adr;
  unsigned char track;
  unsigned char index;
  unsigned char min;
  unsigned char sec;
  unsigned char frame;
  unsigned char zero;
  unsigned char amin;
  unsigned char asec;
  unsigned char aframe;
};


typedef struct volumeinfo {
    unsigned char mode;
    unsigned char input0;
    unsigned char volume0;
    unsigned char input1;
    unsigned char volume1;
    unsigned char input2;
    unsigned char volume2;
    unsigned char input3;
    unsigned char volume3;
};


struct {
  unsigned short drives;
  unsigned char  first_drive;
  unsigned short current_track;
  unsigned int   track_position;
  unsigned char  track_type;
  unsigned char  low_audio;
  unsigned char  high_audio;
  unsigned char  disk_length_min;
  unsigned char  disk_length_sec;
  unsigned char  disk_length_frames;
  unsigned int   endofdisk;
  unsigned char  upc[7];
  unsigned char  diskid[6];
  unsigned int   status;
  unsigned short error;      /* See description below */
} cdrom_data;

/* CDROM_DATA.ERROR Description

  Bit 15         - Error bit
  Bit 14-10      - Reserved
  Bit  9         - Busy
  Bit  8         - Done
  Bit  7-0       - Error code (bit 15 on)

Error codes are the following:

  0  Write-protect violation
  1  Unknown unit
  2  Drive not ready
  3  Unknown command
  4  CRC error
  5  Bad drive request structure length
  6  Seek error
  7  Unknown media
  8  Sector not found
  9  Printer out of paper
  A  Write fault
  B  Read fault
  C  General failure
  D  Reserved
  E  Reserved
  F  Invalid disk change
*/


/*   Multiplex Interrupt routines
     "borrowed" from Ralf Brown's MSDOS Interrupt List v4.1

short 21 - CD-ROM device driver - IOCTL INPUT
	AX = 4402h
	BX = file handle referencing character device for CD-ROM driver
	CX = number of bytes to read
	DS:DX -> control block (see #0563)

Return: CF clear if successful
	    AX = number of bytes actually read
	CF set on error
	    AX = error code (01h,05h,06h,0Dh) (see #0770 at AH=59h)

Note:   the data returned depends on the first byte of the control block; the
	  remainder of the control block is filled by the driver
SeeAlso: AX=4403h"CD-ROM",short 2F/AX=0802h

(Table 0562)
Values for CD-ROM data being requested:
 00h    device driver header address
 01h    drive head location
 02h    reserved
 03h    error statistics
 04h    audio channel info
 05h    raw drive bytes (uninterpreted and device-specific)
 06h    device status
 07h    sector size
 08h    volume size
 09h    media change status
 0Ah    audio disk info
 0Bh    audio track info
 0Ch    audio Q-Channel info
 0Dh    audio sub-channel info
 0Eh    UPC code

Format of CD-ROM control block:
Offset  Size    Description     (Table 0563)
 00h    BYTE    data being requested (see #0562)
---function 00h---
 01h    DWORD   device driver header address (see also AH=52h)
---function 01h---
 01h    BYTE    addressing mode
		00h HSG
		01h Red Book
 02h    DWORD   current location of drive's head
		logical sector number in HSG mode
		frame/second/minute/unused in Red Book mode
		(HSG sector = minute * 4500 + second * 75 + frame - 150)
---function 03h---
 01h  N BYTEs   undefined as of 5 Aug 88 specification
---function 04h---
 01h    BYTE    input channel (0-3) for output channel 0
 02h    BYTE    volume for output channel 0
 03h    BYTE    input channel (0-3) for output channel 1
 04h    BYTE    volume for output channel 1
 05h    BYTE    input channel (0-3) for output channel 2
 06h    BYTE    volume for output channel 2
 07h    BYTE    input channel (0-3) for output channel 3
 08h    BYTE    volume for output channel 3
Notes:  output channels 0 and 1 are left and right, 2 and 3 are left prime and
	  right prime; a volume of 00h is off
	the default setting is for each input channel to be assigned to the
	  same-numbered output channel at full (FFh) volume
---function 05h---
 01h    BYTE    number of bytes read
 02h 128 BYTEs  buffer for drive bytes
---function 06h---
 01h    DWORD   device parameters (see #0564)
---function 07h---
 01h    BYTE    read mode
		00h cooked
		01h raw
 02h    WORD    sector size in bytes
---function 08h---
 01h    DWORD   volume size in sectors
---function 09h---
 01h    BYTE    media change status
		00h don't know
		01h media unchanged
		FFh media has been changed
---function 0Ah---
 01h    BYTE    lowest audio track number
 02h    BYTE    highest audio track number
 03h    DWORD   start address of lead-out track (Red Book format)
--function 0Bh---
 01h    BYTE    track number (set by caller)
 02h    DWORD   starting point of track (Red Book format)
 06h    BYTE    track control info
		bits 15,14,12: track type (notice: bits not contiguous!)
			000 two audio channels, no pre-emphasis
			001 two audio channels with pre-emphasis
			010 data track
			100 four audio channels, no pre-emphasis
			101 four audio channels with pre-emphasis
			other reserved
		bit 13: digital copy permitted
---function 0Ch---
 01h    BYTE    CONTROL and ADR byte (as received from drive)
 02h    BYTE    track number
 03h    BYTE    point or index
 04h    BYTE    minute  \
 05h    BYTE    second   > running time within track
 06h    BYTE    frame   /
 07h    BYTE    zero
 08h    BYTE    "AMIN" or "PMIN"     \
 09h    BYTE    "ASEC" or "PSEC"      > running time on disk
 0Ah    BYTE    "AFRAME" or "PFRAME" /
---function 0Dh---
 01h    DWORD   starting frame address (Red Book format)
 05h    DWORD   transfer address
 09h    DWORD   number of sectors to read
Note:   copies 96 bytes of sub-channel info per sector into buffer
---function 0Eh---
 01h    BYTE    CONTROL and ADR byte
 02h  7 BYTEs   UPC/EAN code (13 BCD digits,low-order nybble of last byte is 0)
 09h    BYTE    zero
 0Ah    BYTE    "AFRAME"

Bitfields for CD-ROM device parameters:
Bit(s)  Description     (Table 0564)
 0      door open
 1      door unlocked
 2      supports raw reading in addition to cooked
 3      writable
 4      can play audio/video tracks
 5      supports interleaving
 6      reserved
 7      supports prefetch requests
 8      supports audio channel control
 9      supports Red Book addressing in addition to HSG
 10     audio is playing
*/
static struct rminfo {
	long EDI;
	long ESI;
	long EBP;
	long reserved_by_system;
	long EBX;
	long EDX;
	long ECX;
	long EAX;
	short flags;
	short ES,DS,FS,GS,IP,CS,SP,SS;
} RMI;

static union REGS inregs, outregs;
static struct SREGS sregs;
static short lowptr;
static int lowptr2;
static void *watptr, *watptr2;

void allocbuffers (void)
{
  memset (&sregs, 0, sizeof(sregs));
  inregs.w.ax = 0x0100;
  inregs.w.bx = 0x20;
  int386x (0x31, &inregs, &outregs, &sregs);
  lowptr = outregs.w.ax;
  watptr = (void *)((outregs.x.eax & 0xffff) << 4);
  int386x (0x31, &inregs, &outregs, &sregs);
  lowptr2 = outregs.w.ax << 16;
  watptr2 = (void *)((outregs.x.eax & 0xffff) << 4);
}


void device_request ()
{
  memset (&sregs, 0, sizeof (sregs));
  memset (&RMI, 0, sizeof(RMI));
  RMI.EAX = 0x00001510;
  RMI.ECX = cdrom_data.first_drive;
  RMI.EBX = 0;
  RMI.ES = lowptr;

  inregs.w.ax = 0x0300;
  inregs.h.bl = 0x2f;
  inregs.h.bh = 0;
  inregs.w.cx = 0;
  sregs.es = FP_SEG (&RMI);
  inregs.x.edi = FP_OFF (&RMI);
  int386x (0x31, &inregs, &outregs, &sregs);
//  if (outregs.x.cflag)
//    printf ("DEVICE REQUEST FAILED!!!\n");
}


void red_book (unsigned int value, unsigned char *min, unsigned char *sec, unsigned char *frame)
{
  *frame = value & 0x000000ff;
  *sec = (value & 0x0000ff00) >> 8;
  *min = (value & 0x00ff0000) >> 16;
}


unsigned int hsg (unsigned int value)
{
  unsigned char min, sec, frame;

  red_book (value, &min, &sec, &frame);
  value = (unsigned int)min * 4500;
  value += (short)sec * 75;
  value += frame;
  value -= 150;
  return value;
}


unsigned int cd_head_position (void)
{
  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    unsigned int address;
    unsigned short bytes;
    unsigned short sector;
    unsigned int  volid;
    unsigned char unused[4];
  } tray_request;

  struct {
    unsigned char mode;
    unsigned char adr_mode;
    unsigned int address;
  } head_data;

  tray_request.length = sizeof (tray_request);
  tray_request.subunit = 0;
  tray_request.comcode = 3;
  tray_request.media = tray_request.sector = tray_request.volid = 0;
  tray_request.address = lowptr2;
  tray_request.bytes = 6;
  head_data.mode = 0x01;
  head_data.adr_mode = 0x00;
  memcpy (watptr, &tray_request, tray_request.length);
  memcpy (watptr2, &head_data, 6);
  device_request ();
  memcpy (&tray_request, watptr, tray_request.length);
  memcpy (&head_data, watptr2, 6);
  cdrom_data.error = tray_request.status;
  return head_data.address;
}


void cd_get_volume (struct volumeinfo *vol)
{
  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    unsigned int address;
    unsigned short bytes;
    unsigned short sector;
    unsigned int  volid;
  } tray_request;

  tray_request.length = sizeof (tray_request);
  tray_request.subunit = 0;
  tray_request.comcode = 3;
  tray_request.media = 0;
  tray_request.media = tray_request.sector = tray_request.volid = 0;
  tray_request.address = lowptr2;
  tray_request.bytes = 9;
  vol->mode = 0x04;
  memcpy (watptr, &tray_request, sizeof (tray_request));
  memcpy (watptr2, vol, sizeof (struct volumeinfo));
  device_request ();
  memcpy (&tray_request, watptr, tray_request.length);
  memcpy (vol, watptr2, sizeof (struct volumeinfo));
  cdrom_data.error = tray_request.status;
}


void cd_set_volume (struct volumeinfo *vol)
{
  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    unsigned int address;
    unsigned short bytes;
    unsigned char unused[4];
  } cd_request;

  vol->mode = 3;
  cd_request.length = sizeof (cd_request);
  cd_request.subunit = 0;
  cd_request.comcode = 12;
  cd_request.media = 0;
  cd_request.address = lowptr2;
  cd_request.bytes = 9;
  memcpy (watptr, &cd_request, sizeof (cd_request));
  memcpy (watptr2, vol, sizeof (struct volumeinfo));
  device_request ();
  memcpy (&cd_request, watptr, cd_request.length);
  cdrom_data.error = cd_request.status;
}


short cd_getupc (void)
{
  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    unsigned int address;
    unsigned short bytes;
    unsigned short sector;
    unsigned int  volid;
  } tray_request;

  struct {
    unsigned char mode;
    unsigned char adr;
    unsigned char upc[7];
    unsigned char zero;
    unsigned char aframe;
  } upc_data;
  
  tray_request.length = sizeof (tray_request);
  tray_request.subunit = 0;
  tray_request.comcode = 3;
  tray_request.media = 0;
  tray_request.media = tray_request.sector = tray_request.volid = 0;
  tray_request.address = lowptr2;
  tray_request.bytes = 11;
  upc_data.mode = 0x0e;
  upc_data.adr = 2;
  memcpy (watptr, &tray_request, sizeof (tray_request));
  memcpy (watptr2, &upc_data, 11);
  device_request ();
  memcpy (&upc_data, watptr2, 11);
  memcpy (&tray_request, watptr, tray_request.length);
  cdrom_data.error = tray_request.status;
  if (upc_data.adr == 0)
    memset (&upc_data.upc, 0, 7);
  memcpy (&cdrom_data.upc[0], &upc_data.upc[0], 7);
  return 1;
}


void cd_get_audio_info (void)
{
  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    int address;
    short bytes;
    short sector;
    int volid;
  } ioctli;

  struct {
    unsigned char mode;
    unsigned char lowest;
    unsigned char highest;
    unsigned int address;
  } track_data;

  ioctli.length = sizeof (ioctli);
  ioctli.subunit = 0;
  ioctli.comcode = 3;
  ioctli.media = 0;
  ioctli.sector = 0;
  ioctli.volid = 0;
  ioctli.address = (unsigned int)lowptr2;
  ioctli.bytes = 7;
  memset (&track_data, 0, 7);
  track_data.mode = 0x0a;
  memcpy (watptr, &ioctli, sizeof (ioctli));
  memcpy (watptr2, &track_data.mode, 7);
  memcpy (&track_data, watptr2, sizeof (track_data));
  device_request ();
  memcpy (&track_data, watptr2, sizeof (track_data));
  memcpy (&ioctli, watptr, sizeof (ioctli));
  memcpy (&cdrom_data.diskid, &track_data.lowest, 6);
  cdrom_data.low_audio = track_data.lowest;
  cdrom_data.high_audio = track_data.highest;
  red_book (track_data.address, &cdrom_data.disk_length_min, &cdrom_data.disk_length_sec, &cdrom_data.disk_length_frames);
  cdrom_data.endofdisk = hsg (track_data.address);
  cdrom_data.error = ioctli.status;
}


void cd_set_track (short tracknum)
{
  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    unsigned int address;
    unsigned short bytes;
    unsigned short sector;
    unsigned int  volid;
  } tray_request;

  struct {
    unsigned char mode;
    unsigned char track;
    unsigned int address;
    unsigned char control;
  } track_data;

  tray_request.length = sizeof (tray_request);
  tray_request.subunit = 0;
  tray_request.comcode = 3;
  tray_request.media = 0;
  tray_request.media = tray_request.sector = tray_request.volid = 0;
  tray_request.address = lowptr2;
  tray_request.bytes = 7;
  track_data.mode = 0x0b;
  track_data.track = tracknum;
  memcpy (watptr, &tray_request, sizeof (tray_request));
  memcpy (watptr2, &track_data, sizeof (track_data));
  device_request ();
  memcpy (&tray_request, watptr, sizeof (tray_request));
  memcpy (&track_data, watptr2, sizeof (track_data));
  cdrom_data.error = tray_request.status;
  cdrom_data.track_position = hsg (track_data.address);
  cdrom_data.current_track = tracknum;
  cdrom_data.track_type = track_data.control & TRACK_MASK;
}


unsigned int get_track_length (short tracknum)
{
  unsigned int start, finish;
  unsigned short ct;

  ct = cdrom_data.current_track;
  cd_set_track (tracknum);
  start = cdrom_data.track_position;
  if (tracknum < cdrom_data.high_audio)
  {
    cd_set_track (tracknum+1);
    finish = cdrom_data.track_position;
  }
  else finish = cdrom_data.endofdisk;

  cd_set_track (ct);

  finish -= start;
  return finish;
}


void cd_track_length (short tracknum, unsigned char *min, unsigned char *sec, unsigned char *frame)
{
  unsigned int value;

  value = get_track_length (tracknum);
  value -= 150;
  *frame = value % 75;
  value -= *frame;
  value /= 75;
  *sec = value % 60;
  value -= *sec;
  value /= 60;
  *min = value;
}


void cd_status (void)
{
  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    unsigned int address;
    unsigned short bytes;
    unsigned short sector;
    unsigned int  volid;
  } tray_request;

  struct {
    unsigned char mode;
    unsigned int status;
  } cd_data;

  tray_request.length = sizeof (tray_request);
  tray_request.subunit = 0;
  tray_request.comcode = 3;
  tray_request.media = 0;
  tray_request.media = tray_request.sector = tray_request.volid = 0;
  tray_request.address = lowptr2;
  tray_request.bytes = 5;

  cd_data.mode = 0x06;

  memcpy (watptr, &tray_request, sizeof (tray_request));
  memcpy (watptr2, &cd_data, sizeof (cd_data));

  device_request ();

  memcpy (&tray_request, watptr, sizeof (tray_request));
  memcpy (&cd_data, watptr2, sizeof (cd_data));

  cdrom_data.status = cd_data.status;
  cdrom_data.error = tray_request.status;
}


void cd_seek (unsigned int location)
{

  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char addressmode;
    unsigned int transfer;
    unsigned short sectors;
    unsigned int seekpos;
  } play_request;

  play_request.length = sizeof (play_request);
  play_request.subunit = 0;
  play_request.comcode = 131;
  play_request.addressmode = 0;
  play_request.transfer = 0;
  play_request.sectors = 0;
  play_request.seekpos = location;
  memcpy (watptr, &play_request, sizeof (play_request));
  device_request ();
  memcpy (&play_request, watptr, sizeof (play_request));
  cdrom_data.error = play_request.status;
}


void cd_play_audio (unsigned int begin, unsigned int end)
{

  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char addressmode;
    unsigned int start;
    unsigned int playlength;
  } play_request;

  play_request.length = sizeof (play_request);
  play_request.subunit = 0;
  play_request.comcode = 132;
  play_request.addressmode = 0;
  play_request.start = begin;
  play_request.playlength = end-begin;
  memcpy (watptr, &play_request, sizeof (play_request));
  device_request ();
  memcpy (&play_request, watptr, sizeof (play_request));
  cdrom_data.error = play_request.status;
}


void cd_stop_audio (void)
{
  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
  } stop_request;

  stop_request.length = sizeof (stop_request);
  stop_request.subunit = 0;
  stop_request.comcode = 133;
  memcpy (watptr, &stop_request, sizeof (stop_request));
  device_request ();
  memcpy (&stop_request, watptr, sizeof (stop_request));
  cdrom_data.error = stop_request.status;
}


void cd_resume_audio (void)
{
  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
  } stop_request;

  stop_request.length = sizeof (stop_request);
  stop_request.subunit = 0;
  stop_request.comcode = 136;
  memcpy (watptr, &stop_request, sizeof (stop_request));
  device_request ();
  memcpy (&stop_request, watptr, sizeof (stop_request));
  cdrom_data.error = stop_request.status;
}


void cd_cmd (unsigned char mode)
{
  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    unsigned int address;
    unsigned short bytes;
    unsigned char unused[4];
  } tray_request;

  unsigned char cd_mode;

  cd_mode = mode;
  tray_request.length = sizeof (tray_request);
  tray_request.subunit = 0;
  tray_request.comcode = 12;
  tray_request.media = 0;
  tray_request.address = lowptr2;
  tray_request.bytes = 1;
  memcpy (watptr, &tray_request, sizeof (tray_request));
  memcpy (watptr2, &cd_mode, 1);
  device_request ();
  memcpy (&tray_request, watptr, sizeof (tray_request));
  cdrom_data.error = tray_request.status;
}


void cd_getpos (struct playinfo *info)
{
  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    unsigned int address;
    unsigned short bytes;
    unsigned short sector;
    unsigned int  volid;
  } tray_request;

  tray_request.length = sizeof (tray_request);
  tray_request.subunit = 0;
  tray_request.comcode = 3;
  tray_request.media = 0;
  tray_request.media = tray_request.sector = tray_request.volid = 0;
  tray_request.address = lowptr2;
  tray_request.bytes = 6;
  info->control = 12;
  memcpy (watptr, &tray_request, sizeof (tray_request));
  memcpy (watptr2, info, sizeof (struct playinfo));
  device_request ();
  memcpy (&tray_request, watptr, sizeof (tray_request));
  memcpy (info, watptr2, sizeof (struct playinfo));
  cdrom_data.error = tray_request.status;
}


short cdrom_installed (void)
{
  inregs.h.ah = 0x15;
  inregs.h.al = 0x00;
  inregs.w.bx = 0;
  int386 (0x2f, &inregs, &outregs);
  if (outregs.w.bx == 0)
    return (0);
  cdrom_data.drives = outregs.w.bx;
  cdrom_data.first_drive = outregs.w.cx;
  if (lowptr == 0)
    allocbuffers ();
  cd_get_audio_info ();
  return (1);
}


short cd_done_play (void)
{
  cd_cmd (CLOSE_TRAY);
  return ((cdrom_data.error & BUSY) == 0);
}


short cd_mediach (void)
{
  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    unsigned int address;
    unsigned short bytes;
    unsigned short sector;
    unsigned int  volid;
  } tray_request;

  struct {
    unsigned char mode;
    unsigned char media;
  } cd_data;

  tray_request.length = sizeof (tray_request);
  tray_request.subunit = 0;
  tray_request.comcode = 3;
  tray_request.media = 0;
  tray_request.media = tray_request.sector = tray_request.volid = 0;
  tray_request.address = lowptr2;
  tray_request.bytes = 2;

  cd_data.mode = 0x09;
  memcpy (watptr, &tray_request, sizeof (tray_request));
  memcpy (watptr2, &cd_data, sizeof (cd_data));
  device_request ();
  memcpy (&tray_request, watptr, sizeof (tray_request));
  memcpy (&cd_data, watptr2, sizeof (cd_data));
  cdrom_data.error = tray_request.status;
  return cd_data.media;
}


void cd_lock (unsigned char doormode)
{
  struct {
    unsigned char length;
    unsigned char subunit;
    unsigned char comcode;
    unsigned short status;
    char ununsed[8];
    unsigned char media;
    unsigned int address;
    unsigned short bytes;
    unsigned char unused[4];
  } tray_request;

  struct {
    unsigned char mode;
    unsigned char media;
  } cd_data;

  tray_request.length = sizeof (tray_request);
  tray_request.subunit = 0;
  tray_request.comcode = 12;
  tray_request.media = 0;
  tray_request.address = lowptr2;
  tray_request.bytes = 2;
  cd_data.mode = 1;
  cd_data.media = doormode;
  memcpy (watptr, &tray_request, sizeof (tray_request));
  memcpy (watptr2, &cd_data, sizeof (cd_data));
  device_request ();
  memcpy (&tray_request, watptr, sizeof (tray_request));
  cdrom_data.error = tray_request.status;
}

void get_musicpos(int *StartPos, int *EndPos)
{
short temp;

  *StartPos = cdrom_data.track_position;
  temp = cdrom_data.current_track;
  if ((temp+1) > cdrom_data.high_audio)
    *EndPos = cdrom_data.endofdisk;
  else 
  {
    cd_set_track (temp+1);
    *EndPos = cdrom_data.track_position;
    cd_set_track (temp);
  }
}

unsigned short get_cd_status()
{
        cd_status();
return(cdrom_data.status);
}


int CD_ROM;
int CDEnd;

void Init_CD()
{
int tracks;
int startpos, endpos;
        CD_ROM=0;
//      return;
        if(!cdrom_installed ())
                return;
        if(get_cd_status()&(1<<11))
                return;
        if(cdrom_data.low_audio+cdrom_data.high_audio==0)
                return;
        CD_ROM=1;
        tracks = cdrom_data.high_audio - cdrom_data.low_audio + 1;
        cd_set_track(tracks+1);
        get_musicpos(&startpos, &endpos);
        CDEnd=endpos;
}

void Play_CD(int pista,int modo)
{
int startpos, endpos;
        if(!CD_ROM)     return;
        cd_set_track(pista);
        get_musicpos(&startpos, &endpos);
        if(modo==0)
                cd_play_audio (startpos, endpos);
        else
                cd_play_audio (startpos, CDEnd);
}
void Stop_CD()
{
        if(!CD_ROM)     return;
        cd_stop_audio ();
}

int IsPaying_CD()
{
        if(!CD_ROM)     return 0;        

        if(get_cd_status()&1)
        {
                Stop_CD();
                return(0);
        }
        if(cdrom_data.error&BUSY)
                return(1);
        else
                return(0);
}

unsigned int get_cd_error()
{
  cd_status();
  return(cdrom_data.error);
}
