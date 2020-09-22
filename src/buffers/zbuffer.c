#include <ultra64.h>

#include "config.h"
#include "zbuffer.h"

#if defined(TARGET_N64)
ALIGNED8 u16 gZBuffer[SCREEN_WIDTH * SCREEN_HEIGHT];
#endif