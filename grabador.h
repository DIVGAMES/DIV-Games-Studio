
#define INIT_FRAME  0
#define CMP_RLE     1
#define CMP_OFF     2
#define END_ANIM    3
#define SET_PALET   4
#define MOUSE_KEY   5

void RegScreen(unsigned char *NewScreen);
void EndGrabador();
void WriteDac(unsigned char *dac);
void WriteMouseKey(char bMouseKey);
