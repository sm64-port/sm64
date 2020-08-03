#ifdef __SWITCH__

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <switch.h>

#include "gfx_window_manager_api.h"
#include "gfx_screen_config.h"

static EGLDisplay s_display;
static EGLDisplay s_context;
static EGLDisplay s_surface;

static void gfx_egl_init(void) {
    s_display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(s_display, NULL, NULL);

    EGLConfig config;
    EGLint numConfigs;

    static const EGLint fbAttrList[] = {
        EGL_RED_SIZE,       8,
        EGL_GREEN_SIZE,     8,
        EGL_BLUE_SIZE,      8,
        EGL_ALPHA_SIZE,     8,
        EGL_DEPTH_SIZE,     24,
        EGL_STENCIL_SIZE,   8,
        EGL_NONE
    };

    eglChooseConfig(s_display, fbAttrList, &config, 1, &numConfigs);

    s_surface = eglCreateWindowSurface(s_display, config, nwindowGetDefault(), NULL);

    static const EGLint ctxAttrList[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };

    s_context = eglCreateContext(s_display, config, EGL_NO_CONTEXT, ctxAttrList);

    eglMakeCurrent(s_display, s_surface, s_surface, s_context);

    eglSwapInterval(s_display, 2);
}

static void gfx_egl_set_keyboard_callbacks(void) 
{

}

static void gfx_egl_set_fullscreen_changed_callback(void) 
{

}

static void gfx_egl_set_fullscreen(void) 
{

}

static void gfx_egl_main_loop(void (*run_one_game_iter)(void)) {
    while (appletMainLoop()) {
        run_one_game_iter();
    }
}

static void gfx_egl_get_dimensions(uint32_t *w, uint32_t *h) {
    *w = DESIRED_SCREEN_WIDTH;
    *h = DESIRED_SCREEN_HEIGHT;
}

static void gfx_egl_handle_events(void) {
}

static bool gfx_egl_start_frame(void) {
    return true;
}

static void gfx_egl_swap_buffers_begin(void) {
    eglSwapBuffers(s_display, s_surface);
}

static void gfx_egl_swap_buffers_end(void) {
}


static void gfx_egl_deinit(void) {
    if(s_display) {
        eglMakeCurrent(s_display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if(s_context) { eglDestroyContext(s_display, s_context); s_context = NULL; }
        if(s_surface) { eglDestroySurface(s_display, s_surface); s_surface = NULL; }
        eglTerminate(s_display);
        s_display = NULL;
    }
}

struct GfxWindowManagerAPI gfx_egl_api = {
    gfx_egl_init,
    gfx_egl_set_keyboard_callbacks,
    gfx_egl_set_fullscreen_changed_callback,
    gfx_egl_set_fullscreen,
    gfx_egl_main_loop,
    gfx_egl_get_dimensions,
    gfx_egl_handle_events,
    gfx_egl_start_frame,
    gfx_egl_swap_buffers_begin,
    gfx_egl_swap_buffers_end,
    gfx_egl_deinit
}; 

#endif
