#ifndef _CDROM_
#define _CDROM_

#define EJECT_TRAY 0
#define RESET 2
#define CLOSE_TRAY 5
#define DATA_TRACK 64
#define LOCK 1
#define UNLOCK 0

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
} _playinfo;

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
} _volumeinfo;

extern struct {
	unsigned short drives;
	unsigned char first_drive;
	unsigned short current_track;
	unsigned int track_position;
	unsigned char track_type;
	unsigned char low_audio;
	unsigned char high_audio;
	unsigned char disk_length_min;
	unsigned char disk_length_sec;
	unsigned char disk_length_frames;
	unsigned int endofdisk;
	unsigned char upc[7];
	unsigned char diskid[6];
	unsigned int status;
	unsigned short error;
} cdrom_data;

extern unsigned int cd_head_position(void);
extern void cd_get_volume(struct volumeinfo *vol);
extern void cd_set_volume(struct volumeinfo *vol);
extern short cd_getupc(void);
extern void cd_get_audio_info(void);
extern void cd_set_track(short tracknum);
extern void cd_track_length(short tracknum, unsigned char *min,
			    unsigned char *sec, unsigned char *frame);
extern void cd_status(void);
extern void cd_seek(unsigned int location);
extern void cd_play_audio(unsigned int begin, unsigned int end);
extern void cd_stop_audio(void);
extern void cd_resume_audio(void);
extern void cd_cmd(unsigned char mode);
extern void cd_getpos(struct playinfo *info);
extern short cdrom_installed(void);
extern short cd_done_play(void);
extern short cd_mediach(void);
extern void cd_lock(unsigned char doormode);
extern void get_musicpos(int *StartPos, int *EndPos);
extern unsigned int get_cd_status();
#endif
