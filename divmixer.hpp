typedef unsigned short  UWORD;

#define SBMIXADDRESS    4
#define SBMIXDATA       5
#define SBMIXRESET      0
#define SBMASTER        2
#define SBVOC           0x0A
#define SBCD            8
#define SBPMASTER       0x22
#define SBPVOC          0x4
#define SBPCD           0x28

#define MAXMIXADDRESS   0x10C
#define MAXMIXDATA      0x10D
#define GUSMAXVOCL      2
#define GUSMAXVOCR      3
#define GUSMAXCDL       4
#define GUSMAXCDR       5
#define GUSMAXMASTERL   6
#define GUSMAXMASTERR   7

#define GUSMIXADDRESS   0x506
#define GUSMIXDATA      0x106
#define GUSVOCL         (3<<3) | 2
#define GUSVOCR         (3<<3) | 3
#define GUSCDL          (2<<3) | 2
#define GUSCDR          (2<<3) | 3
#define GUSMASTERL      (5<<3) | 2
#define GUSMASTERR      (5<<3) | 3

////////////////////
// Mixer de la SB //

#define MIX_ADR_OFF         4
#define MIX_DATA_OFF        5

#define MIX_VOICE_VOL    0x04 // Mixer register addresses
#define MIX_MICRO_VOL    0x0a
#define MIX_INPUT        0x0c
#define MIX_OUTPUT       0x0e
#define MIX_MASTER_VOL   0x22
#define MIX_FM_VOL       0x26
#define MIX_CD_VOL       0x28
#define MIX_LINE_IN_VOL  0x2e

#define MIX_IN_MICRO        0 // Input register settings
#define MIX_IN_CD           2
#define MIX_IN_MICRO2       4
#define MIX_IN_LINE         6
#define MIX_LOW_FILT        0
#define MIX_HIGH_FILT       8
#define MIX_NO_FILT        20

// Mixer de la SB //
////////////////////

void SetMasterVolume  (UWORD volumen); // de 0 a 15
void SetVocVolume     (UWORD volumen); // de 0 a 15
void SetCDVolume      (UWORD volumen); // de 0 a 15
void InitMixer        (UWORD card, UWORD address, UWORD master, UWORD voc, UWORD cd);
void MIX_Reset        (void);
void MIX_SetInput     (byte opt);
void MIX_GetVolume    (byte reg, byte *left, byte *right);
void MIX_SetVolume    (byte reg, byte left, byte right);
void set_mixer        (void);
void set_init_mixer   (void);

