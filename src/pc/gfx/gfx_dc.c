#if defined(TARGET_DC)
#include "gfx_window_manager_api.h"
#include "gfx_screen_config.h"
#include "macros.h"

#include <stdio.h>
#include <string.h>

#define GFX_API_NAME "Dreamcast GLdc"
#define SCR_WIDTH (640)
#define SCR_HEIGHT (480)

static int force_30fps = 1;
static unsigned int last_time = 0;

extern void glKosSwapBuffers(void);
extern uint64_t timer_ms_gettime64(void);
extern void timer_spin_sleep(unsigned int ms);

unsigned int GetSystemTimeLow(void) {
    uint64_t msec = timer_ms_gettime64();
    return (unsigned int) msec;
}

void DelayThread(unsigned int ms) {
    timer_spin_sleep(ms);
}

static void gfx_dc_init(UNUSED const char *game_name, UNUSED bool start_in_fullscreen) {
    /* init */
    last_time = GetSystemTimeLow();
}

static void gfx_dc_set_fullscreen_changed_callback(UNUSED void (*on_fullscreen_changed)(bool is_now_fullscreen)) {
}

static void gfx_dc_set_fullscreen(UNUSED bool enable) {
}

static void gfx_dc_set_keyboard_callbacks(UNUSED bool (*on_key_down)(int scancode),
                                          UNUSED bool (*on_key_up)(int scancode),
                                          UNUSED void (*on_all_keys_up)(void)) {
}

static void gfx_dc_main_loop(void (*run_one_game_iter)(void)) {
    while (1) {
        run_one_game_iter();
    }
}

static void gfx_dc_get_dimensions(uint32_t *width, uint32_t *height) {
    *width = SCR_WIDTH;
    *height = SCR_HEIGHT;
}

/* What events should we be handling? */
static void gfx_dc_handle_events(void) {
    /* Lets us yield to other threads*/
    // DelayThread(100);
}

float cpu_time = 0.f, gpu_time = 0.f;
uint8_t skip_debounce = 0;
const unsigned int FRAME_TIME_MS = 34; // 33.3_

static bool gfx_dc_start_frame(void) {
    const unsigned int cur_time = GetSystemTimeLow();
    const unsigned int elapsed = cur_time - last_time;

    if (skip_debounce) {
        skip_debounce--;
        return true;
    }
    // skip if frame took longer than 1 / 30 = 33.3 ms
    if (elapsed > FRAME_TIME_MS) {
        skip_debounce = 3; // skip a max of once every 4 frames
        last_time = cur_time;
        return false;
    }
    return true;
}

static void gfx_dc_swap_buffers_begin(void) {
    // Number of microseconds a frame should take (30 fps)
    const unsigned int cur_time = GetSystemTimeLow();
    const unsigned int elapsed = cur_time - last_time;
    last_time = cur_time;

    if (force_30fps && elapsed < FRAME_TIME_MS) {
#ifdef DEBUG
        printf("elapsed %d ms fps %f cur: %d\n", elapsed, 1000.0f / elapsed, (unsigned int)timer_ms_gettime64());
#endif
        DelayThread(FRAME_TIME_MS - elapsed);
    }
}

static void gfx_dc_swap_buffers_end(void) {
    /* Lets us yield to other threads*/
    // DelayThread(100);
    glKosSwapBuffers();
}

/* Idk what this is for? */
static double gfx_dc_get_time(void) {
    return 0.0;
}

struct GfxWindowManagerAPI gfx_dc = { gfx_dc_init,
                                      gfx_dc_set_keyboard_callbacks,
                                      gfx_dc_set_fullscreen_changed_callback,
                                      gfx_dc_set_fullscreen,
                                      gfx_dc_main_loop,
                                      gfx_dc_get_dimensions,
                                      gfx_dc_handle_events,
                                      gfx_dc_start_frame,
                                      gfx_dc_swap_buffers_begin,
                                      gfx_dc_swap_buffers_end,
                                      gfx_dc_get_time };
#endif // TARGET_DC
