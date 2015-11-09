int timer_init(unsigned short frequency, void (*function)());
void timer_uninit(void);

extern unsigned short timer_frequency;
extern volatile unsigned timer_count;
