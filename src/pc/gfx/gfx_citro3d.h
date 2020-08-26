#ifndef GFX_CITRO3D_H
#define GFX_CITRO3D_H

#include <citro3d.h>

#include "gfx_rendering_api.h"

extern struct GfxRenderingAPI gfx_citro3d_api;

// FIXME: include from c3d/texture.h
typedef struct Tex3DS_Texture_s* Tex3DS_Texture;
// FIXME: taken from tex3ds.h
Tex3DS_Texture Tex3DS_TextureImport(const void* input, size_t insize, C3D_Tex* tex, C3D_TexCube* texcube, bool vram);
void Tex3DS_TextureFree(Tex3DS_Texture texture);


#endif
