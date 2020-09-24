#if defined(TARGET_PSP)
#include "gfx_window_manager_api.h"
#include "gfx_screen_config.h"
#include "macros.h"

#include <stdio.h>
#include <string.h>
#include <pspsdk.h>
#include <pspkernel.h>
#include <psppower.h>
#include <pspdisplay.h>
#include <pspgu.h>
#include <pspgum.h>
#include <psprtc.h>

#include "../melib.h"
#include "../psp_audio_stack.h"
#include "sceGuDebugPrint.h"

#define GFX_API_NAME "PSP - sceGU"
#define SCR_WIDTH (480)
#define SCR_HEIGHT (272)

static int force_30fps = 1;
static unsigned int last_time = 0;
int audio_manager_thid = 0;

/* I forgot why we need this */
void __assert_func(UNUSED const char *file, UNUSED int line, UNUSED const char *method, UNUSED const char *expression) {
}

/* Minimalist PSP SDK 0.15.0 for Windows whines about missing this, linux built toolchain doesn't care */
char *stpcpy(char *__restrict__ dest, const char *__restrict__ src) {
    while ((*dest++ = *src++) != '\0')
        /* nothing */;
    return --dest;
}

int isspace(int _c) {
    char c = (char) _c;
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

/* Lets rewrite this and add a splash screen donated by z2442 */
void psp_divert_slow_memory_card(void) {
    int ms_to_wait = 3000;
    char buf[64];

    while (ms_to_wait > 0) {
        sprintf(buf, "Launching in%1.2fs", ms_to_wait/1000.f);
        sceGuDebugPrint(32, 32, 0xffffff, buf);
        ms_to_wait -= 16;
        sceGuSync(0, 0);
        sceDisplayWaitVblankStart();
        sceGuSwapBuffers();
    }
}

static int exitCallback(UNUSED int arg1, UNUSED int arg2, UNUSED void *common) {
    J_Cleanup();
    sceKernelTerminateDeleteThread(audio_manager_thid);
    sceKernelExitGame();
    return 0;
}

static int callbackThread(UNUSED SceSize args, UNUSED void *argp) {
    int cbid;

    cbid = sceKernelCreateCallback("Exit Callback", exitCallback, NULL);
    sceKernelRegisterExitCallback(cbid);

    sceKernelSleepThreadCB();

    return 0;
}

void init_mediaengine(void) {
    if (!J_Init(false)) {
        /* Init success, lets enable it! */
        extern int mediaengine_available;
        extern int volatile mediaengine_sound;
        mediaengine_available = 1;
        mediaengine_sound = 1;
    }
}

void init_audiomanager(void) {
    extern int audioOutput(SceSize args, void *argp);
    extern int audio_manager_thid;
    audio_manager_thid = sceKernelCreateThread("AudioOutput", audioOutput, 0x12, 0x20000, THREAD_ATTR_USER | THREAD_ATTR_VFPU, NULL);
    sceKernelStartThread(audio_manager_thid, 0, NULL);
}

void kill_audiomanager(void) {
    J_Cleanup();
    sceKernelTerminateDeleteThread(audio_manager_thid);
    sceKernelDelayThread(250);
}

static void gfx_psp_init(UNUSED const char *game_name, UNUSED bool start_in_fullscreen) {

    int thid = 0;

    thid = sceKernelCreateThread("update_thread", callbackThread, 0x20, 0xFA0, 0, 0);
    if (thid >= 0) {
        sceKernelStartThread(thid, 0, 0);
    }

    scePowerSetClockFrequency(333, 333, 166);
    sceKernelDelayThread(1000);
    init_mediaengine();

    pspDebugScreenInitEx(0, PSP_DISPLAY_PIXEL_FORMAT_8888, 0);
    last_time = sceKernelGetSystemTimeLow();
}

static void gfx_psp_set_fullscreen_changed_callback(UNUSED void (*on_fullscreen_changed)(bool is_now_fullscreen)) {
}

static void gfx_psp_set_fullscreen(UNUSED bool enable) {
}

static void gfx_psp_set_keyboard_callbacks(UNUSED bool (*on_key_down)(int scancode),
                                           UNUSED bool (*on_key_up)(int scancode),
                                           UNUSED void (*on_all_keys_up)(void)) {
}

static void gfx_psp_main_loop(void (*run_one_game_iter)(void)) {
    while (1) {
        run_one_game_iter();
    }
}

static void gfx_psp_get_dimensions(uint32_t *width, uint32_t *height) {
    *width = SCR_WIDTH;
    *height = SCR_HEIGHT;
}

/* What events should we be handling? */
static void gfx_psp_handle_events(void) {
    /* Lets us yield to other threads*/
    sceKernelDelayThread(100);
}

static bool gfx_psp_start_frame(void) {
    return true;
}

static void gfx_psp_swap_buffers_begin(void) {
    // Number of microseconds a frame should take (30 fps)
    const unsigned int FRAME_TIME_US = 33333;
    const unsigned int cur_time = sceKernelGetSystemTimeLow();
    const unsigned int elapsed = cur_time - last_time;
    last_time = cur_time;

    if (force_30fps) {
        if (elapsed < FRAME_TIME_US) {
#ifdef DEBUG
            printf("elapsed %d us fps %f\n", elapsed, (1000.0f * 1000.0f) / elapsed);
#endif
            sceKernelDelayThread(FRAME_TIME_US - elapsed);
            last_time = cur_time + (FRAME_TIME_US - elapsed);
        }
    }
}

static void gfx_psp_swap_buffers_end(void) {
    /* Lets us yield to other threads*/
    sceKernelDelayThread(100);
}

/* Idk what this is for? */
static double gfx_psp_get_time(void) {
    return 0.0;
}

struct GfxWindowManagerAPI gfx_psp = {
    gfx_psp_init,
    gfx_psp_set_keyboard_callbacks,
    gfx_psp_set_fullscreen_changed_callback,
    gfx_psp_set_fullscreen,
    gfx_psp_main_loop,
    gfx_psp_get_dimensions,
    gfx_psp_handle_events,
    gfx_psp_start_frame,
    gfx_psp_swap_buffers_begin,
    gfx_psp_swap_buffers_end,
    gfx_psp_get_time
};
#endif // TARGET_PSP
