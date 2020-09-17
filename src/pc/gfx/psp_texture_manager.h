/*
 * File: psp_texture_manager.h
 * Project: gfx
 * File Created: Friday, 7th August 2020 9:11:56 pm
 * Author: HaydenKow
 * -----
 * Copyright (c) 2020 Hayden Kowalchuk, Hayden Kowalchuk
 * License: BSD 3-clause "New" or "Revised" License, http://www.opensource.org/licenses/BSD-3-Clause
 */

#pragma once
#define TEX_ALIGNMENT (16)

/* 1mb buffer */
#define TEXMAN_BUFFER_SIZE (1 * 1024 * 1024)

struct PSP_Texture {
    unsigned char *location;
    int width, height;
    unsigned int type;
    unsigned int swizzled;
};

/* used for initialization */
int texman_inited(void);
void texman_reset(void *buf, unsigned int size);
void texman_set_buffer(void *buf, unsigned int size);

/* management funcs for clients
Steps:
1. create
2. reserve memory & upload by pointer OR upload_swizzle

note: texture will be bound
*/
unsigned int texman_create(void);
void texman_clear(void);
int gfx_vram_space_available(void);
unsigned char *texman_get_tex_data(unsigned int num);
struct PSP_Texture *texman_reserve_memory(int width, int height, unsigned int type);
void texman_upload_swizzle(int width, int height, unsigned int type, const void *buffer);
void texman_upload(int width, int height, unsigned int type, const void *buffer);
void texman_bind_tex(unsigned int num);
