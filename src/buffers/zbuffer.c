#include <ultra64.h>

#include "config.h"
#include "zbuffer.h"

#if !(defined(TARGET_DC) || defined(TARGET_PSP))
ALIGNED8 u16 gZBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
#endif