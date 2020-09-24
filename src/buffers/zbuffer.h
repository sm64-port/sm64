#ifndef ZBUFFER_H
#define ZBUFFER_H

#include <PR/ultratypes.h>

#include "config.h"
#include "macros.h"

#if !(defined(TARGET_DC) || defined(TARGET_PSP))
extern u16 gZBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
#endif

#endif // ZBUFFER_H
