#include "inter.h"
#include "osdep.h"

void EndSound(void) {}

void InitSound(void) {}
void UnloadSound(int n) {}
int PlaySound(int n, int m, int o) {return 0;}
void StopSound(int n) {return 0;}
void ChangeSound(int n, int m, int o) {return 0;}

int StartFLI(char *nombre, char *Buffer, int Buff_anc,int Buff_alt,int cx,int cy) {return 0;}
int Nextframe(void) {return 0;}
void ResetFli(void) {}
void EndFli(void) {}


