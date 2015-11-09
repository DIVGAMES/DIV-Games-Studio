#include "internal.h"

void DrawFSpan(struct FLine *)   // Draw hor tex line
{}
void DrawWSpan(struct WLine *)   // Draw ver tex line
{}
void DrawMaskWSpan(struct WLine *) // Draw masked ver tex line (wall)
{}

void DrawTransWSpan(struct WLine *)  // Draw translucent ver tex line (wall)
{}

void DrawOSpan(struct WLine *)   // Draw ver tex line (obj)
{}

void DrawMaskOSpan(struct WLine *) // Draw masked ver tex line (obj)
{}

void DrawTransOSpan(struct WLine *)  // Draw translucent ver tex line (obj)
{}



extern DWORD ScrBase;

