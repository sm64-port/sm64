#ifndef GFX_MINIMAP_H
#define GFX_MINIMAP_H

#define u64 __u64
#define s64 __s64
#define u32 __u32
#define vu32 __vu32
#define vs32 __vs32
#define s32 __s32
#define u16 __u16
#define s16 __s16
#define u8 __u8
#define s8 __s8
#include <3ds/types.h>
#undef u64
#undef s64
#undef u32
#undef vu32
#undef vs32
#undef s32
#undef u16
#undef s16
#undef u8
#undef s8

#include "game/level_update.h"
#include "game/area.h"
#include "game/save_file.h"

#include <3ds.h>

#include "minimap_levels.h"

void minimap_load_current_texture(uint8_t **texture, size_t *texture_size);
bool minimap_get_mario_position(float *mario_x, float *mario_y);

#endif
