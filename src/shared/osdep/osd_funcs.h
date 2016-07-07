
void OSDEP_Init(void);
void OSDEP_Quit(void);

uint32_t OSDEP_GetTicks(void);

void OSDEP_SetCaption(char *title, char *icon);
OSDEP_Surface * OSDEP_SetVideoMode(int width, int height, int bpp, Uint32 flags);

int OSDEP_SetPalette(OSDEP_Surface *surface, OSDEP_Color *colors, int firstcolor, int ncolors);

int32_t OSDEP_NumJoysticks(void);
OSDEP_Joystick *OSDEP_JoystickOpen(int n);
void OSDEP_JoystickClose(OSDEP_Joystick *joy);

int OSDEP_JoystickNumButtons(int n);
int OSDEP_JoystickNumHats(int n);
int OSDEP_JoystickNumAxes(int n);

uint8_t OSDEP_JoystickGetButton(OSDEP_Joystick *joystick, int button);
int16_t OSDEP_JoystickGetAxis(OSDEP_Joystick *joystick, int axis);

char * OSDEP_JoystickName(int n);
