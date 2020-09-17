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

/* Minimalist PSP SDK 0.15.0 for Windows whines about missing this, linux built toolchain doesn't care
 */
char *stpcpy(char *__restrict__ dest, const char *__restrict__ src) {
    while ((*dest++ = *src++) != '\0')
        /* nothing */;
    return --dest;
}

int isspace(int _c) {
    char c = (char) _c;
    return c == ' ' || c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r';
}

extern uint64_t timer_ms_gettime64(void);
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

static void
gfx_dc_set_fullscreen_changed_callback(UNUSED void (*on_fullscreen_changed)(bool is_now_fullscreen)) {
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

static bool gfx_dc_start_frame(void) {
    return true;
}

static void gfx_dc_swap_buffers_begin(void) {
    // Number of microseconds a frame should take (30 fps)
    const unsigned int FRAME_TIME_US = 33333;
    const unsigned int cur_time = GetSystemTimeLow();
    const unsigned int elapsed = cur_time - last_time;
    last_time = cur_time;

    if (force_30fps) {
        if (elapsed < FRAME_TIME_US) {
#ifdef DEBUG
            printf("elapsed %d us fps %f\n", elapsed, (1000.0f * 1000.0f) / elapsed);
#endif
            DelayThread((FRAME_TIME_US - elapsed) / 1000);
            last_time = cur_time + (FRAME_TIME_US - elapsed);
        }
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
