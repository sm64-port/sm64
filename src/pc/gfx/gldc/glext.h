/* KallistiGL for KallistiOS ##version##

   libgl/glext.h
   Copyright (C) 2014 Josh Pearson
   Copyright (c) 2007-2013 The Khronos Group Inc.
*/

#ifndef __GL_GLEXT_H
#define __GL_GLEXT_H

#include <sys/cdefs.h>
__BEGIN_DECLS

#define GL_TEXTURE0_ARB                   0x84C0
#define GL_TEXTURE1_ARB                   0x84C1
#define GL_TEXTURE2_ARB                   0x84C2
#define GL_TEXTURE3_ARB                   0x84C3
#define GL_TEXTURE4_ARB                   0x84C4
#define GL_TEXTURE5_ARB                   0x84C5
#define GL_TEXTURE6_ARB                   0x84C6
#define GL_TEXTURE7_ARB                   0x84C7
#define GL_TEXTURE8_ARB                   0x84C8
#define GL_TEXTURE9_ARB                   0x84C9
#define GL_TEXTURE10_ARB                  0x84CA
#define GL_TEXTURE11_ARB                  0x84CB
#define GL_TEXTURE12_ARB                  0x84CC
#define GL_TEXTURE13_ARB                  0x84CD
#define GL_TEXTURE14_ARB                  0x84CE
#define GL_TEXTURE15_ARB                  0x84CF
#define GL_TEXTURE16_ARB                  0x84D0
#define GL_TEXTURE17_ARB                  0x84D1
#define GL_TEXTURE18_ARB                  0x84D2
#define GL_TEXTURE19_ARB                  0x84D3
#define GL_TEXTURE20_ARB                  0x84D4
#define GL_TEXTURE21_ARB                  0x84D5
#define GL_TEXTURE22_ARB                  0x84D6
#define GL_TEXTURE23_ARB                  0x84D7
#define GL_TEXTURE24_ARB                  0x84D8
#define GL_TEXTURE25_ARB                  0x84D9
#define GL_TEXTURE26_ARB                  0x84DA
#define GL_TEXTURE27_ARB                  0x84DB
#define GL_TEXTURE28_ARB                  0x84DC
#define GL_TEXTURE29_ARB                  0x84DD
#define GL_TEXTURE30_ARB                  0x84DE
#define GL_TEXTURE31_ARB                  0x84DF
#define GL_ACTIVE_TEXTURE_ARB             0x84E0
#define GL_CLIENT_ACTIVE_TEXTURE_ARB      0x84E1
#define GL_MAX_TEXTURE_UNITS_ARB          0x84E2

#define GL_CLAMP_TO_EDGE                  0x812F

#define GL_TRANSPOSE_MODELVIEW_MATRIX_ARB 0x84E3
#define GL_TRANSPOSE_PROJECTION_MATRIX_ARB 0x84E4
#define GL_TRANSPOSE_TEXTURE_MATRIX_ARB   0x84E5
#define GL_TRANSPOSE_COLOR_MATRIX_ARB     0x84E6

#define GL_MULTISAMPLE_ARB                0x809D
#define GL_SAMPLE_ALPHA_TO_COVERAGE_ARB   0x809E
#define GL_SAMPLE_ALPHA_TO_ONE_ARB        0x809F
#define GL_SAMPLE_COVERAGE_ARB            0x80A0
#define GL_SAMPLE_BUFFERS_ARB             0x80A8
#define GL_SAMPLES_ARB                    0x80A9
#define GL_SAMPLE_COVERAGE_VALUE_ARB      0x80AA
#define GL_SAMPLE_COVERAGE_INVERT_ARB     0x80AB
#define GL_MULTISAMPLE_BIT_ARB            0x20000000

#define GL_NORMAL_MAP_ARB                 0x8511
#define GL_REFLECTION_MAP_ARB             0x8512
#define GL_TEXTURE_CUBE_MAP_ARB           0x8513
#define GL_TEXTURE_BINDING_CUBE_MAP_ARB   0x8514
#define GL_TEXTURE_CUBE_MAP_POSITIVE_X_ARB 0x8515
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_X_ARB 0x8516
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Y_ARB 0x8517
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_ARB 0x8518
#define GL_TEXTURE_CUBE_MAP_POSITIVE_Z_ARB 0x8519
#define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_ARB 0x851A
#define GL_PROXY_TEXTURE_CUBE_MAP_ARB     0x851B
#define GL_MAX_CUBE_MAP_TEXTURE_SIZE_ARB  0x851C

#define GL_COMPRESSED_ALPHA_ARB           0x84E9
#define GL_COMPRESSED_LUMINANCE_ARB       0x84EA
#define GL_COMPRESSED_LUMINANCE_ALPHA_ARB 0x84EB
#define GL_COMPRESSED_INTENSITY_ARB       0x84EC
#define GL_COMPRESSED_RGB_ARB             0x84ED
#define GL_COMPRESSED_RGBA_ARB            0x84EE
#define GL_TEXTURE_COMPRESSION_HINT_ARB   0x84EF
#define GL_TEXTURE_COMPRESSED_IMAGE_SIZE_ARB 0x86A0
#define GL_TEXTURE_COMPRESSED_ARB         0x86A1
#define GL_NUM_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A2
#define GL_COMPRESSED_TEXTURE_FORMATS_ARB 0x86A3

#define GL_COLOR_ATTACHMENT0_EXT              0x8CE0
#define GL_COLOR_ATTACHMENT1_EXT              0x8CE1
#define GL_COLOR_ATTACHMENT2_EXT              0x8CE2
#define GL_COLOR_ATTACHMENT3_EXT              0x8CE3
#define GL_COLOR_ATTACHMENT4_EXT              0x8CE4
#define GL_COLOR_ATTACHMENT5_EXT              0x8CE5
#define GL_COLOR_ATTACHMENT6_EXT              0x8CE6
#define GL_COLOR_ATTACHMENT7_EXT              0x8CE7
#define GL_COLOR_ATTACHMENT8_EXT              0x8CE8
#define GL_COLOR_ATTACHMENT9_EXT              0x8CE9
#define GL_COLOR_ATTACHMENT10_EXT             0x8CEA
#define GL_COLOR_ATTACHMENT11_EXT             0x8CEB
#define GL_COLOR_ATTACHMENT12_EXT             0x8CEC
#define GL_COLOR_ATTACHMENT13_EXT             0x8CED
#define GL_COLOR_ATTACHMENT14_EXT             0x8CEE
#define GL_COLOR_ATTACHMENT15_EXT             0x8CEF
#define GL_DEPTH_ATTACHMENT_EXT               0x8D00
#define GL_STENCIL_ATTACHMENT_EXT             0x8D20
#define GL_FRAMEBUFFER_EXT                    0x8D40
#define GL_RENDERBUFFER_EXT                   0x8D41
#define GL_RENDERBUFFER_WIDTH_EXT            0x8D42
#define GL_RENDERBUFFER_HEIGHT_EXT            0x8D43
#define GL_RENDERBUFFER_INTERNAL_FORMAT_EXT   0x8D44

#define GL_FRAMEBUFFER_COMPLETE_EXT                      0x8CD5
#define GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT         0x8CD6
#define GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT 0x8CD7
#define GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT        0x8CDB
#define GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT        0x8CDC
#define GL_FRAMEBUFFER_UNSUPPORTED_EXT                   0x8CDD
#define GL_INVALID_FRAMEBUFFER_OPERATION_EXT             0x0506

/* Multitexture extensions */
GLAPI void APIENTRY glActiveTextureARB(GLenum texture);
GLAPI void APIENTRY glClientActiveTextureARB(GLenum texture);
GLAPI void APIENTRY glMultiTexCoord2fARB(GLenum target, GLfloat s, GLfloat t);

GLAPI void APIENTRY glGenFramebuffersEXT(GLsizei n, GLuint* framebuffers);
GLAPI void APIENTRY glDeleteFramebuffersEXT(GLsizei n, const GLuint* framebuffers);
GLAPI void APIENTRY glBindFramebufferEXT(GLenum target, GLuint framebuffer);
GLAPI void APIENTRY glFramebufferTexture2DEXT(GLenum target, GLenum attachment, GLenum textarget, GLuint texture, GLint level);
GLAPI void APIENTRY glGenerateMipmapEXT(GLenum target);
GLAPI GLenum APIENTRY glCheckFramebufferStatusEXT(GLenum target);
GLAPI GLboolean APIENTRY glIsFramebufferEXT(GLuint framebuffer);

/* ext_paletted_texture */
#define GL_COLOR_INDEX1_EXT                0x80E2
#define GL_COLOR_INDEX2_EXT                0x80E3
#define GL_COLOR_INDEX4_EXT                0x80E4
#define GL_COLOR_INDEX8_EXT                0x80E5
#define GL_COLOR_INDEX12_EXT               0x80E6
#define GL_COLOR_INDEX16_EXT               0x80E7

#define GL_COLOR_TABLE_FORMAT_EXT          0x80D8
#define GL_COLOR_TABLE_WIDTH_EXT           0x80D9
#define GL_COLOR_TABLE_RED_SIZE_EXT        0x80DA
#define GL_COLOR_TABLE_GREEN_SIZE_EXT      0x80DB
#define GL_COLOR_TABLE_BLUE_SIZE_EXT       0x80DC
#define GL_COLOR_TABLE_ALPHA_SIZE_EXT      0x80DD
#define GL_COLOR_TABLE_LUMINANCE_SIZE_EXT  0x80DE
#define GL_COLOR_TABLE_INTENSITY_SIZE_EXT  0x80DF

#define GL_TEXTURE_INDEX_SIZE_EXT          0x80ED

#define GL_SHARED_TEXTURE_PALETTE_EXT      0x81FB

GLAPI void APIENTRY glColorTableEXT(GLenum target, GLenum internalFormat, GLsizei width, GLenum format, GLenum type, const GLvoid *data);
GLAPI void APIENTRY glColorSubTableEXT(GLenum target, GLsizei start, GLsizei count, GLenum format, GLenum type, const GLvoid *data);
GLAPI void APIENTRY glGetColorTableEXT(GLenum target, GLenum format, GLenum type, GLvoid *data);
GLAPI void APIENTRY glGetColorTableParameterivEXT(GLenum target, GLenum pname, GLint *params);
GLAPI void APIENTRY glGetColorTableParameterfvEXT(GLenum target, GLenum pname, GLfloat *params);

/* Loads VQ compressed texture from SH4 RAM into PVR VRAM */
/* internalformat must be one of the following constants:
    GL_UNSIGNED_SHORT_5_6_5_VQ
    GL_UNSIGNED_SHORT_5_6_5_VQ_TWID
    GL_UNSIGNED_SHORT_4_4_4_4_VQ
    GL_UNSIGNED_SHORT_4_4_4_4_VQ_TWID
    GL_UNSIGNED_SHORT_1_5_5_5_VQ
    GL_UNSIGNED_SHORT_1_5_5_5_VQ_TWID
 */
GLAPI void APIENTRY glCompressedTexImage2DARB(GLenum target,
        GLint level,
        GLenum internalformat,
        GLsizei width,
        GLsizei height,
        GLint border,
        GLsizei imageSize,
        const GLvoid *data);


/* Core aliases */
#define GL_INVALID_FRAMEBUFFER_OPERATION GL_INVALID_FRAMEBUFFER_OPERATION_EXT

#define glActiveTexture glActiveTextureARB
#define glClientActiveTexture glClientActiveTextureARB
#define glMultiTexCoord2f glMultiTexCoord2fARB

#define glGenerateMipmap glGenerateMipmapEXT
#define glCompressedTexImage2D glCompressedTexImage2DARB

#ifndef GL_VERSION_1_4
#define GL_VERSION_1_4 1
#define GL_MAX_TEXTURE_LOD_BIAS           0x84FD
#define GL_TEXTURE_LOD_BIAS               0x8501
#define GL_MAX_TEXTURE_LOD_BIAS_DEFAULT 7
#define GL_KOS_INTERNAL_DEFAULT_MIPMAP_LOD_BIAS -4
#endif

#ifndef GL_EXT_texture_lod_bias
#define GL_EXT_texture_lod_bias 1
#define GL_MAX_TEXTURE_LOD_BIAS_EXT       0x84FD
#define GL_TEXTURE_FILTER_CONTROL_EXT     0x8500
#define GL_TEXTURE_LOD_BIAS_EXT           0x8501
#endif /* GL_EXT_texture_lod_bias */

__END_DECLS

#endif /* !__GL_GLEXT_H */
