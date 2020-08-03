#ifdef TARGET_N3DS

#include "gfx_3ds.h"

#include "macros.h"

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

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

#include "gfx_cc.h"
#include "gfx_rendering_api.h"

static DVLB_s* sVShaderDvlb;
static shaderProgram_s sShaderProgram;
static void* sVboBuffer;

extern const u8 shader_shbin[];
extern const u32 shader_shbin_size;

struct ShaderProgram {
    uint32_t shader_id;
    u32 opengl_program_id;
    uint8_t num_inputs;
    bool used_textures[2];
    uint8_t num_floats;
    u32 attrib_locations[7];
    uint8_t attrib_sizes[7];
    uint8_t num_attribs;
};

static struct ShaderProgram sShaderProgramPool[64];
static uint8_t sShaderProgramPoolSize;

static int sCurShader = 0;

static C3D_Tex sTexturePool[4096];
static float sTexturePoolScaleS[4096];
static float sTexturePoolScaleT[4096];
static int sTextureIndex;
static int sCurTex = 0;

static int sTexUnits[2];

static bool gfx_citro3d_z_is_from_0_to_1(void)
{
    return true;
}

static int sVtxUnitSize = 0;

static bool sDepthTestOn = false;
static bool sDepthUpdateOn = true;
static bool sDepthDecal = false;

static bool sUseBlend;

static int sBufIdx = 0;

static void gfx_citro3d_vertex_array_set_attribs(struct ShaderProgram *prg)
{
    int unitSize = 0;
    for (int i = 0; i < prg->num_attribs; i++)
        unitSize += prg->attrib_sizes[i];
    sVtxUnitSize = unitSize;
}

static void gfx_citro3d_unload_shader(UNUSED struct ShaderProgram *old_prg)
{
}

static GPU_TEVSRC getTevSrc(int input, bool swapInput01)
{
    switch (input)
    {
        case SHADER_0:
            return GPU_CONSTANT;
        case SHADER_INPUT_1:
            return swapInput01 ? GPU_PREVIOUS : GPU_PRIMARY_COLOR;
        case SHADER_INPUT_2:
            return swapInput01 ? GPU_PRIMARY_COLOR : GPU_PREVIOUS;
        case SHADER_INPUT_3:
            return GPU_CONSTANT;
        case SHADER_INPUT_4:
            return GPU_CONSTANT;
        case SHADER_TEXEL0:
        case SHADER_TEXEL0A:
            return GPU_TEXTURE0;
        case SHADER_TEXEL1:
            return GPU_TEXTURE1;
    }
    return GPU_CONSTANT;
}

static void updateShader(bool swapInput01)
{
    struct ShaderProgram* new_prg = &sShaderProgramPool[sCurShader];

    u32 shader_id = new_prg->shader_id;

    uint8_t c[2][4];
    for (int i = 0; i < 4; i++)
    {
        c[0][i] = (shader_id >> (i * 3)) & 7;
        c[1][i] = (shader_id >> (12 + i * 3)) & 7;
    }
    bool opt_alpha = (shader_id & SHADER_OPT_ALPHA) != 0;
    bool opt_fog = (shader_id & SHADER_OPT_FOG) != 0;
    bool opt_texture_edge = (shader_id & SHADER_OPT_TEXTURE_EDGE) != 0;
    bool used_textures[2] = {0, 0};
    int num_inputs = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (c[i][j] >= SHADER_INPUT_1 && c[i][j] <= SHADER_INPUT_4)
            {
                if (c[i][j] > num_inputs)
                {
                    num_inputs = c[i][j];
                }
            }
            if (c[i][j] == SHADER_TEXEL0 || c[i][j] == SHADER_TEXEL0A)
            {
                used_textures[0] = true;
            }
            if (c[i][j] == SHADER_TEXEL1)
            {
                used_textures[1] = true;
            }
        }
    }
    bool do_single[2] = {c[0][2] == 0, c[1][2] == 0};
    bool do_multiply[2] = {c[0][1] == 0 && c[0][3] == 0, c[1][1] == 0 && c[1][3] == 0};
    bool do_mix[2] = {c[0][1] == c[0][3], c[1][1] == c[1][3]};
    bool color_alpha_same = (shader_id & 0xfff) == ((shader_id >> 12) & 0xfff);

    if (num_inputs >= 3)
        printf("more than 2!\n");

    C3D_TexEnv* env = C3D_GetTexEnv(0);
    if (num_inputs >= 2)
    {
        C3D_TexEnvInit(env);
        C3D_TexEnvColor(env, 0);
        C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
        C3D_TexEnvSrc(env, C3D_Both, GPU_CONSTANT, 0, 0);
        C3D_TexEnvOpRgb(env, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR);
        C3D_TexEnvOpAlpha(env, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA);
        env = C3D_GetTexEnv(1);
    }
    else
    {
        C3D_TexEnvInit(C3D_GetTexEnv(1));
    }
    C3D_TexEnvInit(env);
    C3D_TexEnvColor(env, 0);
    if (!color_alpha_same && opt_alpha)
    {
        if (do_single[0])
        {
            C3D_TexEnvFunc(env, C3D_RGB, GPU_REPLACE);
            C3D_TexEnvSrc(env, C3D_RGB, getTevSrc(c[0][3], swapInput01), 0, 0);
            if (c[0][3] == SHADER_TEXEL0A)
                C3D_TexEnvOpRgb(env, GPU_TEVOP_RGB_SRC_ALPHA, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR);
            else
                C3D_TexEnvOpRgb(env, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR);
        }
        else if (do_multiply[0])
        {
            C3D_TexEnvFunc(env, C3D_RGB, GPU_MODULATE);
            C3D_TexEnvSrc(env, C3D_RGB, getTevSrc(c[0][0], swapInput01), getTevSrc(c[0][2], swapInput01), 0);
            C3D_TexEnvOpRgb(env,
                c[0][0] == SHADER_TEXEL0A ? GPU_TEVOP_RGB_SRC_ALPHA : GPU_TEVOP_RGB_SRC_COLOR,
                c[0][2] == SHADER_TEXEL0A ? GPU_TEVOP_RGB_SRC_ALPHA : GPU_TEVOP_RGB_SRC_COLOR,
                GPU_TEVOP_RGB_SRC_COLOR);
        }
        else if (do_mix[0])
        {
            C3D_TexEnvFunc(env, C3D_RGB, GPU_INTERPOLATE);
            C3D_TexEnvSrc(env, C3D_RGB, getTevSrc(c[0][0], swapInput01), getTevSrc(c[0][1], swapInput01), getTevSrc(c[0][2], swapInput01));
            C3D_TexEnvOpRgb(env,
                c[0][0] == SHADER_TEXEL0A ? GPU_TEVOP_RGB_SRC_ALPHA : GPU_TEVOP_RGB_SRC_COLOR,
                c[0][1] == SHADER_TEXEL0A ? GPU_TEVOP_RGB_SRC_ALPHA : GPU_TEVOP_RGB_SRC_COLOR,
                c[0][2] == SHADER_TEXEL0A ? GPU_TEVOP_RGB_SRC_ALPHA : GPU_TEVOP_RGB_SRC_COLOR);
        }
        else
            printf("complex\n");
        C3D_TexEnvOpAlpha(env,  GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA);
        if (do_single[1])
        {
            C3D_TexEnvFunc(env, C3D_Alpha, GPU_REPLACE);
            C3D_TexEnvSrc(env, C3D_Alpha, getTevSrc(c[1][3], swapInput01), 0, 0);
        }
        else if (do_multiply[1])
        {
            C3D_TexEnvFunc(env, C3D_Alpha, GPU_MODULATE);
            C3D_TexEnvSrc(env, C3D_Alpha, getTevSrc(c[1][0], swapInput01), getTevSrc(c[1][2], swapInput01), 0);
        }
        else if (do_mix[1])
        {
            C3D_TexEnvFunc(env, C3D_Alpha, GPU_INTERPOLATE);
            C3D_TexEnvSrc(env, C3D_Alpha, getTevSrc(c[1][0], swapInput01), getTevSrc(c[1][1], swapInput01), getTevSrc(c[1][2], swapInput01));
        }
        else
            printf("complex\n");
    }
    else
    {
        C3D_TexEnvOpAlpha(env, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA);
        if (do_single[0])
        {
            C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
            C3D_TexEnvSrc(env, C3D_Both, getTevSrc(c[0][3], swapInput01), 0, 0);
            if (c[0][3] == SHADER_TEXEL0A)
                C3D_TexEnvOpRgb(env, GPU_TEVOP_RGB_SRC_ALPHA, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR);
            else
                C3D_TexEnvOpRgb(env, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR);
        }
        else if (do_multiply[0])
        {
            C3D_TexEnvFunc(env, C3D_Both, GPU_MODULATE);
            C3D_TexEnvSrc(env, C3D_Both, getTevSrc(c[0][0], swapInput01), getTevSrc(c[0][2], swapInput01), 0);
            C3D_TexEnvOpRgb(env,
                c[0][0] == SHADER_TEXEL0A ? GPU_TEVOP_RGB_SRC_ALPHA : GPU_TEVOP_RGB_SRC_COLOR,
                c[0][2] == SHADER_TEXEL0A ? GPU_TEVOP_RGB_SRC_ALPHA : GPU_TEVOP_RGB_SRC_COLOR,
                GPU_TEVOP_RGB_SRC_COLOR);
        }
        else if (do_mix[0])
        {
            C3D_TexEnvFunc(env, C3D_Both, GPU_INTERPOLATE);
            C3D_TexEnvSrc(env, C3D_Both, getTevSrc(c[0][0], swapInput01), getTevSrc(c[0][1], swapInput01), getTevSrc(c[0][2], swapInput01));
            C3D_TexEnvOpRgb(env,
                c[0][0] == SHADER_TEXEL0A ? GPU_TEVOP_RGB_SRC_ALPHA : GPU_TEVOP_RGB_SRC_COLOR,
                c[0][1] == SHADER_TEXEL0A ? GPU_TEVOP_RGB_SRC_ALPHA : GPU_TEVOP_RGB_SRC_COLOR,
                c[0][2] == SHADER_TEXEL0A ? GPU_TEVOP_RGB_SRC_ALPHA : GPU_TEVOP_RGB_SRC_COLOR);
        }
        else
            printf("complex\n");
    }
    if (!opt_alpha)
    {
        C3D_TexEnvColor(env, 0xFF000000);
        C3D_TexEnvFunc(env, C3D_Alpha, GPU_REPLACE);
        C3D_TexEnvSrc(env, C3D_Alpha, GPU_CONSTANT, 0, 0);
    }
    if (opt_texture_edge && opt_alpha)
        C3D_AlphaTest(true, GPU_GREATER, 77);
    else
        C3D_AlphaTest(true, GPU_GREATER, 0);
}

static void gfx_citro3d_load_shader(struct ShaderProgram *new_prg)
{
    sCurShader = new_prg->opengl_program_id;
    gfx_citro3d_vertex_array_set_attribs(new_prg);

    updateShader(false);
}

static struct ShaderProgram *gfx_citro3d_create_and_load_new_shader(uint32_t shader_id)
{
    uint8_t c[2][4];
    for (int i = 0; i < 4; i++)
    {
        c[0][i] = (shader_id >> (i * 3)) & 7;
        c[1][i] = (shader_id >> (12 + i * 3)) & 7;
    }
    bool opt_alpha = (shader_id & SHADER_OPT_ALPHA) != 0;
    bool opt_fog = (shader_id & SHADER_OPT_FOG) != 0;
    bool opt_texture_edge = (shader_id & SHADER_OPT_TEXTURE_EDGE) != 0;
    bool used_textures[2] = {0, 0};
    int num_inputs = 0;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            if (c[i][j] >= SHADER_INPUT_1 && c[i][j] <= SHADER_INPUT_4)
            {
                if (c[i][j] > num_inputs)
                {
                    num_inputs = c[i][j];
                }
            }
            if (c[i][j] == SHADER_TEXEL0 || c[i][j] == SHADER_TEXEL0A)
            {
                used_textures[0] = true;
            }
            if (c[i][j] == SHADER_TEXEL1)
            {
                used_textures[1] = true;
            }
        }
    }
    bool do_single[2] = {c[0][2] == 0, c[1][2] == 0};
    bool do_multiply[2] = {c[0][1] == 0 && c[0][3] == 0, c[1][1] == 0 && c[1][3] == 0};
    bool do_mix[2] = {c[0][1] == c[0][3], c[1][1] == c[1][3]};
    bool color_alpha_same = (shader_id & 0xfff) == ((shader_id >> 12) & 0xfff);

    size_t vs_len = 0;
    size_t fs_len = 0;
    size_t num_floats = 4;

    size_t cnt = 0;

    int id = sShaderProgramPoolSize;
    struct ShaderProgram *prg = &sShaderProgramPool[sShaderProgramPoolSize++];
    prg->attrib_sizes[cnt] = 4;
    ++cnt;

    if (used_textures[0] || used_textures[1])
    {
        prg->attrib_sizes[cnt] = 2;
        ++cnt;
    }

    if (opt_fog)
    {
        prg->attrib_sizes[cnt] = 4;
        ++cnt;
    }

    for (int i = 0; i < num_inputs; i++)
    {
        prg->attrib_sizes[cnt] = opt_alpha ? 4 : 3;
        ++cnt;
    }

    prg->shader_id = shader_id;
    prg->opengl_program_id = id;
    prg->num_inputs = num_inputs;
    prg->used_textures[0] = used_textures[0];
    prg->used_textures[1] = used_textures[1];
    prg->num_floats = num_floats;
    prg->num_attribs = cnt;

    gfx_citro3d_load_shader(prg);

    return prg;
}

static struct ShaderProgram *gfx_citro3d_lookup_shader(uint32_t shader_id)
{
    for (size_t i = 0; i < sShaderProgramPoolSize; i++)
    {
        if (sShaderProgramPool[i].shader_id == shader_id)
        {
            return &sShaderProgramPool[i];
        }
    }
    return NULL;
}

static void gfx_citro3d_shader_get_info(struct ShaderProgram *prg, uint8_t *num_inputs, bool used_textures[2])
{
    *num_inputs = prg->num_inputs;
    used_textures[0] = prg->used_textures[0];
    used_textures[1] = prg->used_textures[1];
}

static u32 gfx_citro3d_new_texture(void)
{
    if (sTextureIndex == 4096)
    {
        printf("Out of texures!");
        return 0;
    }
    return sTextureIndex++;
}

static void gfx_citro3d_select_texture(int tile, u32 texture_id)
{
    C3D_TexBind(tile, &sTexturePool[texture_id]);
    sCurTex = texture_id;
    sTexUnits[tile] = texture_id;
}

static u32 sTexBuf[16 * 1024] __attribute__((aligned(32)));

static int sTileOrder[] =
{
    0,  1,   4,  5,
    2,  3,   6,  7,

    8,  9,  12, 13,
    10, 11, 14, 15
};

static void performTexSwizzle(const u8* src, u32* dst, u32 w, u32 h)
{
    int offs = 0;
    for (int y = 0; y < h; y += 8)
    {
        for (int x = 0; x < w; x += 8)
        {
            for (int i = 0; i < 64; i++)
            {
                int x2 = i & 7;
                int y2 = i >> 3;
                int pos = sTileOrder[(x2 & 3) + ((y2 & 3) << 2)] + ((x2 >> 2) << 4) + ((y2 >> 2) << 5);
                u32 c = ((const u32*)src)[(y + y2) * w + x + x2];
                dst[offs + pos] = ((c & 0xFF) << 24) | (((c >> 8) & 0xFF) << 16) | (((c >> 16) & 0xFF) << 8) | (c >> 24);
            }
            dst += 64;
        }
    }
}

static void gfx_citro3d_upload_texture(const uint8_t *rgba32_buf, int width, int height)
{
    if (width < 8 || height < 8 || (width & (width - 1)) || (height & (height - 1)))
    {
        int newWidth = width < 8 ? 8 : (1 << (32 - __builtin_clz(width - 1)));
        int newHeight = height < 8 ? 8 : (1 << (32 - __builtin_clz(height - 1)));
        if (newWidth * newHeight * 4 > sizeof(sTexBuf))
        {
            printf("Tex buffer overflow!\n");
            return;
        }
        int offs = 0;
        for (int y = 0; y < newHeight; y += 8)
        {
            for (int x = 0; x < newWidth; x += 8)
            {
                for (int i = 0; i < 64; i++)
                {
                    int x2 = i % 8;
                    int y2 = i / 8;

                    int realX = x + x2;
                    if (realX >= width)
                        realX -= width;

                    int realY = y + y2;
                    if (realY >= height)
                        realY -= height;

                    int pos = sTileOrder[x2 % 4 + y2 % 4 * 4] + 16 * (x2 / 4) + 32 * (y2 / 4);
                    u32 c = ((u32*)rgba32_buf)[realY * width + realX];
                    ((u32*)sTexBuf)[offs + pos] = ((c & 0xFF) << 24) | (((c >> 8) & 0xFF) << 16) | (((c >> 16) & 0xFF) << 8) | (c >> 24);
                }
                offs += 64;
            }
        }
        sTexturePoolScaleS[sCurTex] = width / (float)newWidth;
        sTexturePoolScaleT[sCurTex] = height / (float)newHeight;
        width = newWidth;
        height = newHeight;
    }
    else
    {
        sTexturePoolScaleS[sCurTex] = 1.f;
        sTexturePoolScaleT[sCurTex] = 1.f;
        performTexSwizzle(rgba32_buf, sTexBuf, width, height);
    }
    C3D_TexInit(&sTexturePool[sCurTex], width, height, GPU_RGBA8);
    C3D_TexUpload(&sTexturePool[sCurTex], sTexBuf);
    C3D_TexFlush(&sTexturePool[sCurTex]);
}

static uint32_t gfx_cm_to_opengl(uint32_t val)
{
    if (val & G_TX_CLAMP)
        return GPU_CLAMP_TO_EDGE;
    return (val & G_TX_MIRROR) ? GPU_MIRRORED_REPEAT : GPU_REPEAT;
}

static void gfx_citro3d_set_sampler_parameters(int tile, bool linear_filter, uint32_t cms, uint32_t cmt)
{
    C3D_TexSetFilter(&sTexturePool[sTexUnits[tile]], linear_filter ? GPU_LINEAR : GPU_NEAREST, linear_filter ? GPU_LINEAR : GPU_NEAREST);
    C3D_TexSetWrap(&sTexturePool[sTexUnits[tile]], gfx_cm_to_opengl(cms), gfx_cm_to_opengl(cmt));
}

static void updateDepth()
{
    C3D_DepthTest(sDepthTestOn, GPU_LEQUAL, sDepthUpdateOn ? GPU_WRITE_ALL : GPU_WRITE_COLOR);
    C3D_DepthMap(true, sDepthDecal ? -1 : -1, sDepthDecal ? -0.001f : 0);
}

static void gfx_citro3d_set_depth_test(bool depth_test)
{
    sDepthTestOn = depth_test;
    updateDepth();
}

static void gfx_citro3d_set_depth_mask(bool z_upd)
{
    sDepthUpdateOn = z_upd;
    updateDepth();
}

static void gfx_citro3d_set_zmode_decal(bool zmode_decal)
{
    sDepthDecal = zmode_decal;
    updateDepth();
}

static void gfx_citro3d_set_viewport(int x, int y, int width, int height)
{
    C3D_SetViewport(x, y, width, height);
}

static void gfx_citro3d_set_scissor(int x, int y, int width, int height)
{
    if (gGfx3DSMode == GFX_3DS_MODE_NORMAL)
        C3D_SetScissor(GPU_SCISSOR_NORMAL, y, x, y + height, x + width);
    else if (gGfx3DSMode == GFX_3DS_MODE_AA_22 || gGfx3DSMode == GFX_3DS_MODE_WIDE_AA_12)
        C3D_SetScissor(GPU_SCISSOR_NORMAL, y * 2, x * 2, (y + height) * 2, (x + width) * 2);
    else if (gGfx3DSMode == GFX_3DS_MODE_WIDE)
        C3D_SetScissor(GPU_SCISSOR_NORMAL, y, x * 2, y + height, (x + width) * 2);
}

static void applyBlend()
{
    if (sUseBlend)
        C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA);
    else
        C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_ONE, GPU_ZERO, GPU_ONE, GPU_ZERO);
}

static void gfx_citro3d_set_use_alpha(bool use_alpha)
{
    sUseBlend = use_alpha;
    applyBlend();
}

static u32 vec4ToU32Color(float r, float g, float b, float a)
{
    int r2 = r * 255;
    if (r2 < 0)
        r2 = 0;
    else if (r2 > 255)
        r2 = 255;
    int g2 = g * 255;
    if (g2 < 0)
        g2 = 0;
    else if (g2 > 255)
        g2 = 255;
    int b2 = b * 255;
    if (b2 < 0)
        b2 = 0;
    else if (b2 > 255)
        b2= 255;
    int a2 = a * 255;
    if (a2 < 0)
        a2 = 0;
    else if (a2 > 255)
        a2 = 255;
    return (a2 << 24) | (b2 << 16) | (g2 << 8) | r2;
}

static void renderFog(float buf_vbo[], UNUSED size_t buf_vbo_len, size_t buf_vbo_num_tris)
{
    C3D_TexEnv* env = C3D_GetTexEnv(0);
    C3D_TexEnvInit(env);
    C3D_TexEnvColor(env, 0);
    C3D_TexEnvFunc(env, C3D_Both, GPU_REPLACE);
    C3D_TexEnvSrc(env, C3D_Both, GPU_PRIMARY_COLOR, 0, 0);
    C3D_TexEnvOpRgb(env, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR, GPU_TEVOP_RGB_SRC_COLOR);
    C3D_TexEnvOpAlpha(env, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA, GPU_TEVOP_A_SRC_ALPHA);
    env = C3D_GetTexEnv(1);
    C3D_TexEnvInit(env);
    C3D_AlphaBlend(GPU_BLEND_ADD, GPU_BLEND_ADD, GPU_SRC_ALPHA, GPU_ONE_MINUS_SRC_ALPHA, GPU_ZERO, GPU_DST_ALPHA);
    C3D_DepthTest(sDepthTestOn, GPU_LEQUAL, GPU_WRITE_COLOR);

    int offset = 0;
    float* dst = &((float*)sVboBuffer)[sBufIdx * 10];
    bool hasTex = sShaderProgramPool[sCurShader].used_textures[0] || sShaderProgramPool[sCurShader].used_textures[1];
    bool hasFog = (sShaderProgramPool[sCurShader].shader_id & SHADER_OPT_FOG) != 0;
    for (int i = 0; i < 3 * buf_vbo_num_tris; i++)
    {
        *dst++ = buf_vbo[offset + 1];
        *dst++ = -buf_vbo[offset + 0];
        *dst++ = -buf_vbo[offset + 2];
        *dst++ = buf_vbo[offset + 3];
        int vtxOffs = 4;
        if (hasTex)
            vtxOffs += 2;
        *dst++ = 0;
        *dst++ = 0;
        *dst++ = buf_vbo[offset + vtxOffs++];
        *dst++ = buf_vbo[offset + vtxOffs++];
        *dst++ = buf_vbo[offset + vtxOffs++];
        *dst++ = buf_vbo[offset + vtxOffs++];

        offset += sVtxUnitSize;
    }

    C3D_DrawArrays(GPU_TRIANGLES, sBufIdx, buf_vbo_num_tris * 3);
    sBufIdx += buf_vbo_num_tris * 3;

    updateShader(false);
    applyBlend();
    updateDepth();
}

static void renderTwoColorTris(float buf_vbo[], size_t buf_vbo_len, size_t buf_vbo_num_tris)
{
    int offset = 0;
    float* dst = &((float*)sVboBuffer)[sBufIdx * 10];
    bool hasTex = sShaderProgramPool[sCurShader].used_textures[0] || sShaderProgramPool[sCurShader].used_textures[1];
    bool hasColor = sShaderProgramPool[sCurShader].num_inputs > 0;
    bool hasAlpha = (sShaderProgramPool[sCurShader].shader_id & SHADER_OPT_ALPHA) != 0;
    bool hasFog = (sShaderProgramPool[sCurShader].shader_id & SHADER_OPT_FOG) != 0;
    if (hasFog)
        C3D_TexEnvColor(C3D_GetTexEnv(2), vec4ToU32Color(buf_vbo[hasTex ? 6 : 4], buf_vbo[hasTex ? 7 : 5], buf_vbo[hasTex ? 8 : 6], buf_vbo[hasTex ? 9 : 7]));
    u32 firstColor0, firstColor1;
    bool color0Constant = true;
    bool color1Constant = true;
    //determine which color is constant over all vertices
    for (int i = 0; i < buf_vbo_num_tris * 3 && color0Constant && color1Constant; i++)
    {
        int vtxOffs = 4;
        if (hasTex)
            vtxOffs += 2;
        if (hasFog)
            vtxOffs += 4;
        u32 color0 = vec4ToU32Color(
            buf_vbo[offset + vtxOffs],
            buf_vbo[offset + vtxOffs + 1],
            buf_vbo[offset + vtxOffs + 2],
            hasAlpha ? buf_vbo[offset + vtxOffs + 3] : 1.0f);
        vtxOffs += hasAlpha ? 4 : 3;
        u32 color1 = vec4ToU32Color(
            buf_vbo[offset + vtxOffs],
            buf_vbo[offset + vtxOffs + 1],
            buf_vbo[offset + vtxOffs + 2],
            hasAlpha ? buf_vbo[offset + vtxOffs + 3] : 1.0f);
        if (i == 0)
        {
            firstColor0 = color0;
            firstColor1 = color1;
        }
        else
        {
            if (firstColor0 != color0)
                color0Constant = false;
            if (firstColor1 != color1)
                color1Constant = false;
        }
        offset += sVtxUnitSize;
    }
    offset = 0;
    updateShader(!color1Constant);
    C3D_TexEnvColor(C3D_GetTexEnv(0), color1Constant ? firstColor1 : firstColor0);
    for (int i = 0; i < 3 * buf_vbo_num_tris; i++)
    {
        *dst++ = buf_vbo[offset + 1];
        *dst++ = -buf_vbo[offset + 0];
        *dst++ = -buf_vbo[offset + 2];
        *dst++ = buf_vbo[offset + 3];
        int vtxOffs = 4;
        if (hasTex)
        {
            *dst++ = buf_vbo[offset + vtxOffs++] * sTexturePoolScaleS[sCurTex];
            *dst++ = 1 - (buf_vbo[offset + vtxOffs++] * sTexturePoolScaleT[sCurTex]);
        }
        else
        {
            *dst++ = 0;
            *dst++ = 0;
        }
        if (hasFog)
            vtxOffs += 4;
        if (color0Constant)
            vtxOffs += hasAlpha ? 4 : 3;
        if (hasColor)
        {
            *dst++ = buf_vbo[offset + vtxOffs++];
            *dst++ = buf_vbo[offset + vtxOffs++];
            *dst++ = buf_vbo[offset + vtxOffs++];
            if (hasAlpha)
                *dst++ = buf_vbo[offset + vtxOffs++];
            else
                *dst++ = 1.0f;
        }
        else
        {
            *dst++ = 1.0f;
            *dst++ = 1.0f;
            *dst++ = 1.0f;
            *dst++ = 1.0f;
        }

        offset += sVtxUnitSize;
    }

    C3D_DrawArrays(GPU_TRIANGLES, sBufIdx, buf_vbo_num_tris * 3);
    sBufIdx += buf_vbo_num_tris * 3;

    if (hasFog)
        renderFog(buf_vbo, buf_vbo_len, buf_vbo_num_tris);
}

static void gfx_citro3d_draw_triangles(float buf_vbo[], size_t buf_vbo_len, size_t buf_vbo_num_tris)
{
    if (sBufIdx * 10 > 2 * 1024 * 1024 / 4)
    {
        printf("Poly buf over!\n");
        return;
    }

    if (sShaderProgramPool[sCurShader].num_inputs >= 2)
    {
        renderTwoColorTris(buf_vbo, buf_vbo_len, buf_vbo_num_tris);
        return;
    }

    int offset = 0;
    float* dst = &((float*)sVboBuffer)[sBufIdx * 10];
    bool hasTex = sShaderProgramPool[sCurShader].used_textures[0] || sShaderProgramPool[sCurShader].used_textures[1];
    bool hasColor = sShaderProgramPool[sCurShader].num_inputs > 0;
    bool hasAlpha = (sShaderProgramPool[sCurShader].shader_id & SHADER_OPT_ALPHA) != 0;
    bool hasFog = (sShaderProgramPool[sCurShader].shader_id & SHADER_OPT_FOG) != 0;
    for (int i = 0; i < 3 * buf_vbo_num_tris; i++)
    {
        *dst++ = buf_vbo[offset + 1];
        *dst++ = -buf_vbo[offset + 0];
        *dst++ = -buf_vbo[offset + 2];
        *dst++ = buf_vbo[offset + 3];
        int vtxOffs = 4;
        if (hasTex)
        {
            *dst++ = buf_vbo[offset + vtxOffs++] * sTexturePoolScaleS[sCurTex];
            *dst++ = 1 - (buf_vbo[offset + vtxOffs++] * sTexturePoolScaleT[sCurTex]);
        }
        else
        {
            *dst++ = 0;
            *dst++ = 0;
        }
        if (hasFog)
            vtxOffs += 4;
        if (hasColor)
        {
            *dst++ = buf_vbo[offset + vtxOffs++];
            *dst++ = buf_vbo[offset + vtxOffs++];
            *dst++ = buf_vbo[offset + vtxOffs++];
            if (hasAlpha)
                *dst++ = buf_vbo[offset + vtxOffs++];
            else
                *dst++ = 1.0f;
        }
        else
        {
            *dst++ = 1.0f;
            *dst++ = 1.0f;
            *dst++ = 1.0f;
            *dst++ = 1.0f;
        }

        offset += sVtxUnitSize;
    }

    C3D_DrawArrays(GPU_TRIANGLES, sBufIdx, buf_vbo_num_tris * 3);
    sBufIdx += buf_vbo_num_tris * 3;

    if (hasFog)
        renderFog(buf_vbo, buf_vbo_len, buf_vbo_num_tris);
}

static void gfx_citro3d_init(void)
{
    sVShaderDvlb = DVLB_ParseFile((u32*)shader_shbin, shader_shbin_size);
    shaderProgramInit(&sShaderProgram);
    shaderProgramSetVsh(&sShaderProgram, &sVShaderDvlb->DVLE[0]);
    C3D_BindProgram(&sShaderProgram);

    // Configure attributes for use with the vertex shader
    C3D_AttrInfo* attrInfo = C3D_GetAttrInfo();
    AttrInfo_Init(attrInfo);
    AttrInfo_AddLoader(attrInfo, 0, GPU_FLOAT, 4); // v0=position
    AttrInfo_AddLoader(attrInfo, 1, GPU_FLOAT, 2); // v1=texcoord
    AttrInfo_AddLoader(attrInfo, 2, GPU_FLOAT, 4); // v2=color

    // Create the VBO (vertex buffer object)
    sVboBuffer = linearAlloc(2 * 1024 * 1024);

    // Configure buffers
    C3D_BufInfo* bufInfo = C3D_GetBufInfo();
    BufInfo_Init(bufInfo);
    BufInfo_Add(bufInfo, sVboBuffer, 10 * 4, 3, 0x210);

    C3D_CullFace(GPU_CULL_NONE);
    C3D_DepthMap(true, -1.0f, 0);
    C3D_DepthTest(false, GPU_LEQUAL, GPU_WRITE_ALL);
    C3D_AlphaTest(true, GPU_GREATER, 0x00);
}

static void gfx_citro3d_start_frame(void)
{
    sBufIdx = 0;
}

static void gfx_citro3d_on_resize(void)
{
}

static void gfx_citro3d_end_frame(void)
{
}

static void gfx_citro3d_finish_render(void)
{
}

struct GfxRenderingAPI gfx_citro3d_api = {
    gfx_citro3d_z_is_from_0_to_1,
    gfx_citro3d_unload_shader,
    gfx_citro3d_load_shader,
    gfx_citro3d_create_and_load_new_shader,
    gfx_citro3d_lookup_shader,
    gfx_citro3d_shader_get_info,
    gfx_citro3d_new_texture,
    gfx_citro3d_select_texture,
    gfx_citro3d_upload_texture,
    gfx_citro3d_set_sampler_parameters,
    gfx_citro3d_set_depth_test,
    gfx_citro3d_set_depth_mask,
    gfx_citro3d_set_zmode_decal,
    gfx_citro3d_set_viewport,
    gfx_citro3d_set_scissor,
    gfx_citro3d_set_use_alpha,
    gfx_citro3d_draw_triangles,
    gfx_citro3d_init,
    gfx_citro3d_on_resize,
    gfx_citro3d_start_frame,
    gfx_citro3d_end_frame,
    gfx_citro3d_finish_render
};

#endif
