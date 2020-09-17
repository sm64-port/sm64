#pragma once

#include "gl.h"

__BEGIN_DECLS

/*
 * Dreamcast specific compressed + twiddled formats.
 * We use constants from the range 0xEEE0 onwards
 * to avoid trampling any real GL constants (this is in the middle of the
 * any_vendor_future_use range defined in the GL enum.spec file.
*/
#define GL_UNSIGNED_SHORT_5_6_5_TWID_KOS            0xEEE0
#define GL_UNSIGNED_SHORT_1_5_5_5_REV_TWID_KOS      0xEEE2
#define GL_UNSIGNED_SHORT_4_4_4_4_REV_TWID_KOS      0xEEE3

#define GL_COMPRESSED_RGB_565_VQ_KOS                0xEEE4
#define GL_COMPRESSED_ARGB_1555_VQ_KOS              0xEEE6
#define GL_COMPRESSED_ARGB_4444_VQ_KOS              0xEEE7

#define GL_COMPRESSED_RGB_565_VQ_TWID_KOS           0xEEE8
#define GL_COMPRESSED_ARGB_1555_VQ_TWID_KOS         0xEEEA
#define GL_COMPRESSED_ARGB_4444_VQ_TWID_KOS         0xEEEB

#define GL_COMPRESSED_RGB_565_VQ_MIPMAP_KOS                0xEEEC
#define GL_COMPRESSED_ARGB_1555_VQ_MIPMAP_KOS              0xEEED
#define GL_COMPRESSED_ARGB_4444_VQ_MIPMAP_KOS              0xEEEE

#define GL_COMPRESSED_RGB_565_VQ_MIPMAP_TWID_KOS           0xEEEF
#define GL_COMPRESSED_ARGB_1555_VQ_MIPMAP_TWID_KOS         0xEEF0
#define GL_COMPRESSED_ARGB_4444_VQ_MIPMAP_TWID_KOS         0xEEF1

#define GL_NEARZ_CLIPPING_KOS                       0xEEFA

#define GL_UNSIGNED_BYTE_TWID_KOS                   0xEEFB


/* Initialize the GL pipeline. GL will initialize the PVR. */
GLAPI void APIENTRY glKosInit();

typedef struct {
    /* If GL_TRUE, enables pvr autosorting, this *will* break glDepthFunc/glDepthTest */
    GLboolean autosort_enabled;

    /* If GL_TRUE, enables the PVR FSAA */
    GLboolean fsaa_enabled;

    /* The internal format for paletted textures, must be GL_RGBA4 (default) or GL_RGBA8 */
    GLenum internal_palette_format;

    /* Initial capacity of each of the OP, TR and PT lists in vertices */
    GLuint initial_op_capacity;
    GLuint initial_tr_capacity;
    GLuint initial_pt_capacity;
    GLuint initial_immediate_capacity;
} GLdcConfig;


typedef struct {
    GLuint padding0;
    GLfloat x;
    GLfloat y;
    GLfloat z;
    GLfloat u;
    GLfloat v;
    GLubyte bgra[4];
    GLuint padding1;
} GLVertexKOS;

GLAPI void APIENTRY glVertexPackColor3fKOS(GLVertexKOS* vertex, float r, float g, float b);
GLAPI void APIENTRY glVertexPackColor4fKOS(GLVertexKOS* vertex, float r, float g, float b, float a);

GLAPI void APIENTRY glKosInitConfig(GLdcConfig* config);

/* Usage:
 *
 * GLdcConfig config;
 * glKosInitConfig(&config);
 *
 * config.autosort_enabled = GL_TRUE;
 *
 * glKosInitEx(&config);
 */
GLAPI void APIENTRY glKosInitEx(GLdcConfig* config);
GLAPI void APIENTRY glKosSwapBuffers(void);

/*
 * CUSTOM EXTENSION multiple_shared_palette_KOS
 *
 * This extension allows using up to 4 different shared palettes
 * with ColorTableEXT. The following constants are provided
 * to use as targets for ColorTableExt:
 *
 * - SHARED_TEXTURE_PALETTE_0_KOS
 * - SHARED_TEXTURE_PALETTE_1_KOS
 * - SHARED_TEXTURE_PALETTE_2_KOS
 * - SHARED_TEXTURE_PALETTE_3_KOS
 *
 * In this use case SHARED_TEXTURE_PALETTE_0_KOS is interchangable with SHARED_TEXTURE_PALETTE_EXT
 * (both refer to the first shared palette).
 *
 * To select which palette a texture uses, a new pname is accepted by TexParameteri: SHARED_TEXTURE_BANK_KOS
 * by default textures use shared palette 0.
*/

#define GL_SHARED_TEXTURE_PALETTE_0_KOS             0xEEFC
#define GL_SHARED_TEXTURE_PALETTE_1_KOS             0xEEFD
#define GL_SHARED_TEXTURE_PALETTE_2_KOS             0xEEFE
#define GL_SHARED_TEXTURE_PALETTE_3_KOS             0xEEFF

/* Pass to glTexParameteri to set the shared bank */
#define GL_SHARED_TEXTURE_BANK_KOS                  0xEF00

#ifndef GL_EXT_dreamcast_direct_buffer
#define GL_EXT_dreamcast_direct_buffer 1
/* Enable flag */
#define GL_DIRECT_BUFFER_KOS                  0xEF01
/* for glGetIntegerV */
#define GL_DIRECT_BUFFER_ADDRESS              0xEF02
GLAPI void APIENTRY glDirectBufferReserve_INTERNAL_KOS(int count, int* buffer_ptr, GLenum mode);
#endif /* GL_EXT_dreamcast_direct_buffer */

__END_DECLS
extern GLboolean TEXTURES_ENABLED[];
extern GLubyte ACTIVE_TEXTURE;
#define _GetBooleanTextureEnabled(TEXTURE_INDEX) ( TEXTURE_INDEX == GL_TEXTURE0 ? TEXTURES_ENABLED[0] : TEXTURES_ENABLED[1] )