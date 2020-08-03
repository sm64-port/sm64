#ifdef __SWITCH__

#include <switch.h>
#include <ultra64.h>

#include "controller_api.h"

HidControllerID conId;

static void joycon_init(void) {
    hidScanInput();

    conId = CONTROLLER_P1_AUTO;
}

static void joycon_read(OSContPad *pad) {
    hidScanInput();

    u64 pressed = hidKeysDown(conId);
    u64 held = hidKeysHeld(conId);

    JoystickPosition sticks[2];
    hidJoystickRead(&sticks[0], conId, JOYSTICK_LEFT);
    hidJoystickRead(&sticks[1], conId, JOYSTICK_RIGHT);

    if(pressed & KEY_PLUS || held & KEY_PLUS)       pad->button |= START_BUTTON;
    if(pressed & KEY_MINUS || held & KEY_MINUS)     pad->button |= START_BUTTON;

    if(pressed & KEY_B || held & KEY_B)             pad->button |= A_BUTTON;
    if(pressed & KEY_A || held & KEY_A)             pad->button |= A_BUTTON;
    if(pressed & KEY_Y || held & KEY_Y)             pad->button |= B_BUTTON;
    if(pressed & KEY_X || held & KEY_X)             pad->button |= B_BUTTON;

    if(pressed & KEY_L  || held & KEY_L)            pad->button |= L_TRIG;
    if(pressed & KEY_ZL || held & KEY_ZL)           pad->button |= Z_TRIG;
    if(pressed & KEY_R  || held & KEY_R)            pad->button |= R_TRIG;
    if(pressed & KEY_ZR || held & KEY_ZR)           pad->button |= R_TRIG;

    if(pressed & KEY_DUP    || held & KEY_DUP)      pad->button |= U_JPAD;
    if(pressed & KEY_DLEFT  || held & KEY_DLEFT)    pad->button |= L_JPAD;
    if(pressed & KEY_DDOWN  || held & KEY_DDOWN)    pad->button |= D_JPAD;
    if(pressed & KEY_DRIGHT || held & KEY_DRIGHT)   pad->button |= R_JPAD;

    if(sticks[1].dx > 0x4000)   pad->button |= R_CBUTTONS;
    if(sticks[1].dx < -0x4000)  pad->button |= L_CBUTTONS;
    if(sticks[1].dy > 0x4000)   pad->button |= U_CBUTTONS;
    if(sticks[1].dy < -0x4000)  pad->button |= D_CBUTTONS;

    //TODO: Deadzone
    pad->stick_x = sticks[0].dx / 0x100;
    pad->stick_y = sticks[0].dy / 0x100;
}

struct ControllerAPI controller_switch = {
    joycon_init,
    joycon_read
};

#endif
