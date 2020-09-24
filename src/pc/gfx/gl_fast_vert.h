#pragma once

#define VERTEX_EOL 0xf0000000
#define VERTEX 0xe0000000
#define PACK_ARGB8888(r, g, b, a) ((uint32_t)((uint8_t)(a) << 24) | ((uint8_t)(r) << 16) | ((uint8_t)(g) << 8) | (uint8_t)(b))
#define PACK_BGRA8888(r, g, b, a) ((uint32_t)((uint8_t)(b) << 24) | ((uint8_t)(g) << 16) | ((uint8_t)(r) << 8) | (uint8_t)(a))
#define VTX_COLOR_WHITE .color = {.packed = PACK_BGRA8888(255, 255, 255, 255)}

typedef struct __attribute__((packed, aligned(4))) vec3f_gl {
  float x, y, z;
} vec3f;

typedef struct __attribute__((packed, aligned(4))) uv_float {
  float u, v;
} uv_float;

typedef struct __attribute__((packed)) color_uc_struct {
  unsigned char b, g, r, a;
} color_uc_struct;

typedef union color_uc {
  color_uc_struct array;
  unsigned int packed;
} color_uc;

typedef struct __attribute__((packed, aligned(4))) dc_fast_t {
  uint32_t flags;
  struct vec3f_gl vert;
  uv_float texture;
  color_uc color;  // bgra
  union {
    float pad;
    unsigned int vertindex;
  } pad0;
} dc_fast_t;

/* must be in order:  [weights (0-8)] [texture uv] [color] [normal] [vertex]
  aligned to 32bits (4bytes)
  we are going to use (GU_TEXTURE_32BITF | GU_COLOR_8888 | GU_VERTEX_32BITF  )
*/
typedef struct __attribute__((packed, aligned(4))) psp_fast_t {
  uv_float texture;
  color_uc color;  // bgra
  struct vec3f_gl vert;
} psp_fast_t;
