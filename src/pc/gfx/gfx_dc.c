#if defined(TARGET_DC)
#include "gfx_window_manager_api.h"
#include "gfx_screen_config.h"
#include "macros.h"

#include <stdio.h>
#include <string.h>
#include <dc/video.h>
#include <assert.h>

#define GFX_API_NAME "Dreamcast GLdc"
#define SCR_WIDTH (640)
#define SCR_HEIGHT (480)

static int force_30fps = 1;
static unsigned int last_time = 0;

extern void glKosSwapBuffers(void);
extern uint64_t timer_ms_gettime64(void);

unsigned int GetSystemTimeLow(void) {
    uint64_t msec = timer_ms_gettime64();
    return (unsigned int) msec;
}

void DelayThread(unsigned int ms) {
    thd_sleep(ms);
}

//-----------------------------------------------------------------------------
#if 0
typedef void (*assert_handler_t)(const char * file, int line, const char * expr,
                                 const char * msg, const char * func);
assert_handler_t assert_set_handler(assert_handler_t hnd);
extern void bfont_draw_str(uint16_t *buffer, int bufwidth, int opaque, char *str);
static void drawtext(int x, int y, char *string) {
    extern uint16_t *vram_s;
    printf("%s\n", string);
    int offset = ((y * 640) + x);
    bfont_draw_str(vram_s + offset, 640, 1, string);
}

static void assert_hnd(const char *file, int line, const char *expr, const char *msg, const char *func) {
    char strbuffer[1024];

    /* Reset video mode, clear screen */
    vid_set_mode(DM_640x480,  1 /*PM_RGB565*/);
    vid_empty();

    /* Display the error message on screen */
    drawtext(32, 64, "nuQuake - Assertion failure");

    sprintf(strbuffer, " Location: %s, line %d (%s)", file, line, func);
    drawtext(32, 96, strbuffer);

    sprintf(strbuffer, "Assertion: %s", expr);
    drawtext(32, 128, strbuffer);

    sprintf(strbuffer, "  Message: %s", msg);
    drawtext(32, 160, strbuffer);
    arch_exit();
}
#endif

//=============================================================================
extern void setSystemRam(void);
static void gfx_dc_init(UNUSED const char *game_name, UNUSED bool start_in_fullscreen) {
    //assert_set_handler(assert_hnd);
    setSystemRam();

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
const unsigned int FRAME_TIME_MS = 30; // hopefully get right on target @ 33.3

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
}

static void gfx_dc_swap_buffers_end(void) {
    // Number of microseconds a frame should take (30 fps)
    const unsigned int cur_time = GetSystemTimeLow();
    const unsigned int elapsed = cur_time - last_time;
    last_time = cur_time;

    if (force_30fps && elapsed < FRAME_TIME_MS) {
#ifdef DEBUG
        printf("elapsed %d ms fps %f delay %d \n", elapsed, 1000.0f / elapsed, FRAME_TIME_MS - elapsed);
#endif
        DelayThread(FRAME_TIME_MS - elapsed);
        last_time += (FRAME_TIME_MS - elapsed);
    }

    /* Lets us yield to other threads*/
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


/* Last ditch 118kb saver, less after VMU */
#if 0
#include <stdio.h>
#include <assert.h>
#include <arch/rtc.h>
#include <kos/fs.h>
#include <kos/thread.h>
#include <kos/fs_pty.h>
#include <kos/fs_ramdisk.h>
#include <kos/library.h>
#include <kos/net.h>
#include <kos/dbgio.h>
#include <dc/fs_iso9660.h>
#include <dc/cdrom.h>
#include <dc/fs_vmu.h>
#include <dc/vmufs.h>
#include <dc/cdrom.h>
#include <dc/spu.h>
#include <dc/pvr.h>
#include <dc/maple.h>
#include <dc/sound/sound.h>
#include <dc/scif.h>
#include <dc/fs_dcload.h>
#include <dc/fs_dclsocket.h>
#include <arch/irq.h>
#include <arch/timer.h>
#include <dc/fb_console.h>
int arch_auto_init() {

  /* Initialize memory management */
  mm_init();

  /* Do this immediately so we can receive exceptions for init code
     and use ints for dbgio receive. */
  irq_init();         /* IRQs */
  irq_disable();          /* Turn on exceptions */

  fs_dcload_init_console();   /* Init dc-load console, if applicable */

  // Init SCIF for debug stuff (maybe)
  scif_init();

  /* Init debug IO */
  dbgio_init();

  timer_init();           /* Timers */
  hardware_sys_init();        /* DC low-level hardware init */

  /* Initialize our timer */
  timer_ms_enable();
  rtc_init();

  /* Threads */
  thd_init(THD_MODE_PREEMPT);

  nmmgr_init();

  fs_init();          /* VFS */
  fs_pty_init();          /* Pty */
  fs_ramdisk_init();      /* Ramdisk */

  /* DC peripheral init */
	/* Init sound */
	spu_init();
	spu_dma_init();

	/* Init CD-ROM.. NOTE: NO GD-ROM SUPPORT. ONLY CDs/CDRs. */
	//cdrom_init();

	/* Setup maple bus */
	maple_init();

	/* Init video */
	vid_init(DEFAULT_VID_MODE, DEFAULT_PIXEL_MODE);

  if(!(__kos_init_flags & INIT_NO_DCLOAD) && *DCLOADMAGICADDR == DCLOADMAGICVALUE) {
      dbglog(DBG_INFO, "dc-load console support enabled\n");
      fs_dcload_init();
  }

  //fs_iso9660_init();
  //vmufs_init();
  //fs_vmu_init();

  // Initialize library handling
  library_init();

  /* Now comes the optional stuff */
  if(__kos_init_flags & INIT_IRQ) {
      irq_enable();       /* Turn on IRQs */
  }

  return 0;
}

void  arch_auto_shutdown() {
    fs_dclsocket_shutdown();
    net_shutdown();

    irq_disable();
    snd_shutdown();
    timer_shutdown();
    hardware_shutdown();
    pvr_shutdown();
    library_shutdown();
    fs_dcload_shutdown();
    //fs_vmu_shutdown();
    //vmufs_shutdown();
    //fs_iso9660_shutdown();
    //fs_ramdisk_shutdown();
    fs_pty_shutdown();
    fs_shutdown();
    thd_shutdown();
    rtc_shutdown();
}
#endif

#endif // TARGET_DC
