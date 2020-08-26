#ifdef TARGET_N3DS

#ifndef GFX_3DS_H
#define GFX_3DS_H

#include "gfx_window_manager_api.h"

#ifndef _LANGUAGE_C
#define _LANGUAGE_C
#endif

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

#include <PR/gbi.h>

#include <3ds.h>
#include <citro3d.h>

extern C3D_RenderTarget *gTarget;
extern C3D_RenderTarget *gTargetRight;
extern C3D_RenderTarget *gTargetBottom;

extern float gSliderLevel;

typedef enum
{
    GFX_3DS_MODE_NORMAL,
    GFX_3DS_MODE_AA_22,
    GFX_3DS_MODE_WIDE,
    GFX_3DS_MODE_WIDE_AA_12
} Gfx3DSMode;

extern struct GfxWindowManagerAPI gfx_3ds;
extern Gfx3DSMode gGfx3DSMode;

#endif
#endif
