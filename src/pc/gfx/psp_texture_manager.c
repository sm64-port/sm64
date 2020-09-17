/*
 * File: psp_texture_manager.c
 * Project: gfx
 * File Created: Friday, 7th August 2020 9:11:50 pm
 * Author: HaydenKow
 * -----
 * Copyright (c) 2020 Hayden Kowalchuk, Hayden Kowalchuk
 * License: BSD 3-clause "New" or "Revised" License, http://www.opensource.org/licenses/BSD-3-Clause
 */

#include "psp_texture_manager.h"
#include <string.h>
#include <pspkernel.h>
#include <pspdebug.h>
#include <pspgu.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static struct PSP_Texture textures[512];
static void *psp_tex_buffer = NULL;
static void *psp_tex_buffer_start = NULL;
static void *psp_tex_buffer_max = NULL;
static unsigned int psp_tex_number = 0;
unsigned int psp_tex_bound = 0;

static inline unsigned int getMemorySize(int width, int height, unsigned int psm) {
    switch (psm) {
        case GU_PSM_T4:
            return (width * height) >> 1;

        case GU_PSM_T8:
            return width * height;

        case GU_PSM_5650:
        case GU_PSM_5551:
        case GU_PSM_4444:
        case GU_PSM_T16:
            return 2 * width * height;

        case GU_PSM_8888:
        case GU_PSM_T32:
            return 4 * width * height;

        default:
            return 0;
    }
}

static inline unsigned int getTexWidthBytes(int width, unsigned int psm) {
    switch (psm) {
        case GU_PSM_T4:
            return (width >> 1);

        case GU_PSM_T8:
            return width;

        case GU_PSM_5650:
        case GU_PSM_5551:
        case GU_PSM_4444:
        case GU_PSM_T16:
            return 2 * width;

        case GU_PSM_8888:
        case GU_PSM_T32:
            return 4 * width;

        default:
            return 0;
    }
}

static void swizzle_fast(unsigned char *out, const unsigned char *in, unsigned int width,
                         unsigned int height) {
    unsigned int blockx, blocky;
    unsigned int j;

    unsigned int width_blocks = (width / 16);
    unsigned int height_blocks = (height / 8);

    unsigned int src_pitch = (width - 16) / 4;
    unsigned int src_row = width * 8;

    const unsigned char *ysrc = in;
    unsigned int *dst = (unsigned int *) out;

    for (blocky = 0; blocky < height_blocks; ++blocky) {
        const unsigned char *xsrc = ysrc;
        for (blockx = 0; blockx < width_blocks; ++blockx) {
            const unsigned int *src = (unsigned int *) xsrc;
            for (j = 0; j < 8; ++j) {
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                *(dst++) = *(src++);
                src += src_pitch;
            }
            xsrc += 16;
        }
        ysrc += src_row;
    }
}

int texman_inited(void) {
    return psp_tex_buffer != 0;
}

void texman_reset(void *buf, unsigned int size) {
    memset(textures, 0, sizeof(textures));
    psp_tex_number = 0;
    psp_tex_buffer = psp_tex_buffer_start = buf;
    psp_tex_buffer_max = buf + size;
#ifdef DEBUG
    char msg[64];
    sprintf(msg, "TEXMAN reset @ %p size %d bytes\n", buf, size);
    sceIoWrite(1, msg, strlen(msg));
#endif
}

void texman_clear(void) {
    memset(textures, 0, sizeof(textures));
    psp_tex_number = 0;
    psp_tex_buffer = psp_tex_buffer_start;
#ifdef DEBUG
    char msg[64];
    sprintf(msg, "TEXMAN clear %p size %d bytes!\n", psp_tex_buffer, TEXMAN_BUFFER_SIZE);
    sceIoWrite(1, msg, strlen(msg));
#endif
}

void texman_set_buffer(void *buf, unsigned int size) {
    psp_tex_buffer = buf;
    psp_tex_buffer_max = buf + size;
}

int gfx_vram_space_available(void) {
    return (psp_tex_buffer_max - psp_tex_buffer) > (32 * 1024);
}

unsigned char *texman_get_tex_data(unsigned int num) {
    return textures[num].location;
}

unsigned char texman_get_tex_type(unsigned int num) {
    return textures[num].type;
}

struct PSP_Texture *texman_reserve_memory(int width, int height, unsigned int type) {
    int tex_size = getMemorySize(width, height, type);
    psp_tex_buffer =
        (void *) ((((unsigned int) psp_tex_buffer + tex_size + TEX_ALIGNMENT - 1) / TEX_ALIGNMENT)
                  * TEX_ALIGNMENT);
#ifdef DEBUG
    printf("TEX_MAN tex [%d] reserved %d bytes @ %x left: %d kb\n", psp_tex_number, tex_size,
           (unsigned int) textures[psp_tex_number].location,
           (psp_tex_buffer_max - psp_tex_buffer) / 1024);
#endif
    return &textures[psp_tex_number];
}

unsigned int texman_create(void) {
    psp_tex_number++;
    textures[psp_tex_number] = (struct PSP_Texture){
        location : psp_tex_buffer,
        width : 0,
        height : 0,
        type : 0,
        swizzled : 0
    };
    psp_tex_bound = psp_tex_number;

#ifdef DEBUG
    printf("TEX_MAN new tex [%d] @ %x\n", psp_tex_number, psp_tex_buffer);
#endif
    return psp_tex_number;
}

void texman_upload_swizzle(int width, int height, unsigned int type, const void *buffer) {
    struct PSP_Texture *current = texman_reserve_memory(width, height, type);
    sceKernelDcacheWritebackRange(buffer, getMemorySize(width, height, type));
    current->width = width;
    current->height = height;
    current->type = type;
    /* 32bpp = 4 bytes, width is in bytes */
    swizzle_fast(current->location, buffer, getTexWidthBytes(width, type), height);
    current->swizzled = GU_TRUE;
#ifdef DEBUG
    printf("TEX_MAN upload swizzled [%d]\n", psp_tex_number);
#endif
    sceKernelDcacheWritebackRange(current->location, getMemorySize(width, height, type));
    sceKernelDcacheInvalidateRange(current->location, getMemorySize(width, height, type));
    texman_bind_tex(psp_tex_number);
}

void texman_upload(int width, int height, unsigned int type, const void *buffer) {
    struct PSP_Texture *current = texman_reserve_memory(width, height, type);
    sceKernelDcacheWritebackRange(buffer, getMemorySize(width, height, type));
    current->width = width;
    current->height = height;
    current->type = type;
    current->swizzled = GU_FALSE;
    memcpy(current->location, buffer, getMemorySize(width, height, type));
#ifdef DEBUG
    // printf("TEX_MAN upload plain [%d]\n", psp_tex_number);
#endif
    sceKernelDcacheWritebackRange(current->location, getMemorySize(width, height, type));
    sceKernelDcacheInvalidateRange(current->location, getMemorySize(width, height, type));
    texman_bind_tex(psp_tex_number);
}

void texman_bind_tex(unsigned int num) {
    const struct PSP_Texture *current = &textures[num];
#ifdef DEBUG
    /* Note this will SPAM if you enable */
    // if (psp_tex_bound != num)
    //    printf("TEX_MAN bind tex [%d]\n", num);
#endif
    sceGuTexMode(current->type, 0, 0, current->swizzled);
    sceGuTexImage(0, current->width, current->height, current->width, current->location);
    psp_tex_bound = num;
}

unsigned int texman_get_bound(void) {
    return psp_tex_bound;
}