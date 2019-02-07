#define NOTPRESSED 0
#define PRESSED 128
#define KEY_BACKSPACE 0x0E
#define KEY_CAPSLOCK 0x3A
#define KEY_ENTER 0x1C
#define KEY_ESC 0x01
#define KEY_ALT 0xb8
#define KEY_CTRL 0x9D
#define KEY_SHIFT 0x36
#define KEY_NUMLOCK 0x45
#define KEY_SCROLLLOCK 0x46
#define KEY_SPACE 0x39
#define KEY_TAB 0x0F
#define KEY_F1 0x3B
#define KEY_F2 0x3C
#define KEY_F3 0x3D
#define KEY_F4 0x3E
#define KEY_F5 0x3F
#define KEY_F6 0x40
#define KEY_F7 0x41
#define KEY_F8 0x42
#define KEY_F9 0x43
#define KEY_F10 0x44
#define KEY_F11 0x57
#define KEY_F12 0x58
#define KEY_GREY0 0x52
#define KEY_GREY1 0x4F
#define KEY_GREY2 0x50
#define KEY_GREY3 0x51
#define KEY_GREY4 0x4B
#define KEY_GREY5 0x4C
#define KEY_GREY6 0x4D
#define KEY_GREY7 0x47
#define KEY_GREY8 0x48
#define KEY_GREY9 0x49
#define KEY_GREYCOLON 0x53
#define KEY_GREYASTERSK 0x37
#define KEY_GREYMINUS 0x4A
#define KEY_GREYPLUS 0x4E
#define KEY_A 0x1E
#define KEY_N 0x31
#define KEY_B 0x30
#define KEY_O 0x18
#define KEY_C 0x2E
#define KEY_P 0x19
#define KEY_D 0x20
#define KEY_Q 0x10
#define KEY_E 0x12
#define KEY_R 0x13
#define KEY_F 0x21
#define KEY_S 0x1F
#define KEY_G 0x22
#define KEY_T 0x14
#define KEY_H 0x23
#define KEY_U 0x16
#define KEY_I 0x17
#define KEY_V 0x2F
#define KEY_J 0x24
#define KEY_W 0x11
#define KEY_K 0x25
#define KEY_X 0x2D
#define KEY_L 0x26
#define KEY_Y 0x15
#define KEY_M 0x32
#define KEY_Z 0x2C
#define KEY_1 0x02
#define KEY_2 0x03
#define KEY_3 0x04
#define KEY_4 0x05
#define KEY_5 0x06
#define KEY_6 0x07
#define KEY_7 0x08
#define KEY_8 0x09
#define KEY_9 0x0A
#define KEY_0 0x0B
#define KEY_MINUS 0x0C
#define KEY_EQUAL 0x0D
#define KEY_BRACKETL 0x1A
#define KEY_BRACKETR 0x1B
#define KEY_SEMICOLON 0x1C
#define KEY_APOST1 0x28
#define KEY_APOST2 0x29
#define KEY_COMMA 0x33
#define KEY_COLON 0x34
#define KEY_SLASH 0x35
#define KEY_BACKSLASH 0x2B
#define KEY_PRTSC 0x37 + 0x80
#define KEY_SLASH2 0x35 + 0x80
#define KEY_DEL 0x53 + 0x80
#define KEY_DOWN 0x50 + 0x80
#define KEY_END 0x4F + 0x80
#define KEY_HOME 0x47 + 0x80
#define KEY_INS 0x52 + 0x80
#define KEY_LEFT 0x4B + 0x80
#define KEY_PGDN 0x51 + 0x80
#define KEY_PGUP 0x49 + 0x80
#define KEY_RIGHT 0x4D + 0x80
#define KEY_UP 0x48 + 0x80
#define KEY_WINDOWS 0x5B + 0x80
#define KEY_MENU 0x5D + 0x80
#define KEY_PAUSE 0xFF

int kbd_init(void);
void kbd_uninit(void);
int waitkey(void);
int getkey(void);
int checkkey(int rawcode);
unsigned char getascii(int rawcode);
char *getname(int rawcode);

extern unsigned char kt[];
