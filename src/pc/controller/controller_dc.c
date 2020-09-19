#if defined(TARGET_DC)

#include <stdbool.h>
#include <PR/ultratypes.h>
//#include <ultra64.h>
/* Dreamcast and Nintendo share these defines... the odds... Copy it here */
/* Buttons */

#define N64_CONT_A 0x8000
#define N64_CONT_B 0x4000
#define N64_CONT_G 0x2000
#define N64_CONT_START 0x1000
#define N64_CONT_UP 0x0800
#define N64_CONT_DOWN 0x0400
#define N64_CONT_LEFT 0x0200
#define N64_CONT_RIGHT 0x0100
#define N64_CONT_L 0x0020
#define N64_CONT_R 0x0010
#define N64_CONT_E 0x0008
#define N64_CONT_D 0x0004
#define N64_CONT_C 0x0002
#define N64_CONT_F 0x0001

/* Nintendo's official button names */

#define A_BUTTON N64_CONT_A
#define B_BUTTON N64_CONT_B
#define L_TRIG N64_CONT_L
#define R_TRIG N64_CONT_R
#define Z_TRIG N64_CONT_G
#define START_BUTTON N64_CONT_START
#define U_JPAD N64_CONT_UP
#define L_JPAD N64_CONT_LEFT
#define R_JPAD N64_CONT_RIGHT
#define D_JPAD N64_CONT_DOWN
#define U_CBUTTONS N64_CONT_E
#define L_CBUTTONS N64_CONT_C
#define R_CBUTTONS N64_CONT_F
#define D_CBUTTONS N64_CONT_D

typedef struct {
    u16 button;
    s8 stick_x; /* -80 <= stick_x <= 80 */
    s8 stick_y; /* -80 <= stick_y <= 80 */
    u8 errnum;
} OSContPad;

/* End Copying this here */

//#include "controller_api.h"
/* Replace this because of Ultra64 Incompatibilities */
struct ControllerAPI {
    void (*init)(void);
    void (*read)(OSContPad *pad);
};
#include <dc/maple.h>
#include <dc/maple/controller.h>

static void controller_dc_init(void) {
}

// n64 controls https://strategywiki.org/wiki/Super_Mario_64/Controls
// DC -> N64 control mapping
//  B, X     -> B
//  A        -> A
//  Y        -> R Trigger
//  L Trig   -> Z Trigger
//  R Trig   -> Z Trigger
//  Analog   -> Move
//  Start    -> Start
//  DPad     -> Camera buttons
#define configDeadzone (0x20)

static void controller_dc_read(OSContPad *pad) {
    maple_device_t *cont;
    cont_state_t *state;

    cont = maple_enum_type(0, MAPLE_FUNC_CONTROLLER);
    if (!cont)
        return;
    state = (cont_state_t *) maple_dev_status(cont);

    const char stickH =state->joyx;
    const char stickV = 0xff-((uint8_t)(state->joyy));
    const uint32_t magnitude_sq = (uint32_t)(stickH * stickH) + (uint32_t)(stickV * stickV);

    if (magnitude_sq > (uint32_t)(configDeadzone * configDeadzone)) {
        pad->stick_x = ((float)stickH/127)*80;
        pad->stick_y = ((float)stickV/127)*80;
    }
    if (state->buttons & CONT_START)
        pad->button |= START_BUTTON;
    if ((state->buttons & CONT_B) || (state->buttons & CONT_X))
        pad->button |= B_BUTTON;
    if (state->buttons & CONT_A)
        pad->button |= A_BUTTON;
    if (state->buttons & CONT_Y)
        pad->button |= R_TRIG;
    if (((uint8_t) state->ltrig & 0x80 /* 128 */))
        pad->button |= Z_TRIG;
    if (((uint8_t) state->rtrig & 0x80 /* 128 */))
        pad->button |= Z_TRIG;
    if (state->buttons & CONT_DPAD_UP)
        pad->button |= U_CBUTTONS;
    if (state->buttons & CONT_DPAD_DOWN)
        pad->button |= D_CBUTTONS;
    if (state->buttons & CONT_DPAD_LEFT)
        pad->button |= L_CBUTTONS;
    if (state->buttons & CONT_DPAD_RIGHT)
        pad->button |= R_CBUTTONS;
}

struct ControllerAPI controller_dc = { controller_dc_init, controller_dc_read };

#endif
