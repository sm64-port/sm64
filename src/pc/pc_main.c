#include <stdlib.h>

#ifdef TARGET_WEB
#include <emscripten.h>
#include <emscripten/html5.h>
#endif

#include "sm64.h"

#include "game/memory.h"
#include "audio/external.h"

#include "gfx/gfx_pc.h"
#include "gfx/gfx_opengl.h"
#include "gfx/gfx_direct3d11.h"
#include "gfx/gfx_direct3d12.h"
#include "gfx/gfx_dxgi.h"
#include "gfx/gfx_glx.h"
#include "gfx/gfx_psp.h"
#include "gfx/gfx_dc.h"
#include "gfx/gfx_sdl.h"

#include "audio/audio_api.h"
#include "audio/audio_psp.h"
#include "audio/audio_dc.h"
#include "audio/audio_wasapi.h"
#include "audio/audio_pulse.h"
#include "audio/audio_alsa.h"
#include "audio/audio_sdl.h"
#include "audio/audio_null.h"

#include "controller/controller_keyboard.h"

#include "configfile.h"

#include "compat.h"

#if defined(TARGET_PSP)
#include <pspsdk.h>
#include <pspkernel.h>
#define MODULE_NAME "SM64 for PSP"
#ifndef SRC_VER
#define SRC_VER "UNKNOWN"
#endif

PSP_MODULE_INFO(MODULE_NAME, 0, 1, 1);
PSP_HEAP_SIZE_MAX();
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER | THREAD_ATTR_VFPU);

const char _srcver[] __attribute__((section (".version"), used)) = MODULE_NAME " - " SRC_VER ;
#endif

#define CONFIG_FILE "sm64config.txt"

OSMesg D_80339BEC;
OSMesgQueue gSIEventMesgQueue;

s8 gResetTimer;
s8 D_8032C648;
s8 gDebugLevelSelect;
s8 gShowProfiler;
s8 gShowDebugText;

static struct AudioAPI *audio_api;
static struct GfxWindowManagerAPI *wm_api;
static struct GfxRenderingAPI *rendering_api;

extern void gfx_run(Gfx *commands);
extern void thread5_game_loop(void *arg);
extern void create_next_audio_buffer(s16 *samples, u32 num_samples);
void game_loop_one_iteration(void);

void dispatch_audio_sptask(UNUSED struct SPTask *spTask) {
}

void set_vblank_handler(UNUSED s32 index, UNUSED struct VblankHandler *handler, UNUSED OSMesgQueue *queue, UNUSED OSMesg *msg) {
}

static uint8_t inited = 0;

#include "game/game_init.h" // for gGlobalTimer
void send_display_list(struct SPTask *spTask) {
    if (!inited) {
        return;
    }
    gfx_run((Gfx *)spTask->task.t.data_ptr);
}

#ifdef VERSION_EU
#define SAMPLES_HIGH 656
#define SAMPLES_LOW 640
#else
#define SAMPLES_HIGH 544
#define SAMPLES_LOW 528
#endif

#if defined(TARGET_PSP)
/* This flag enables use of the MediaEngine */
#define ME_EXEC

#include "psp_audio_stack.h"
#include "sceGuDebugPrint.h"
#ifdef ME_EXEC
#include "melib.h"
static struct Job j __attribute__((aligned(64)));
#else 
typedef int JobData;
#endif

static s16 audio_buffer[SAMPLES_HIGH * 2 * 2] __attribute__((aligned(64)));
extern struct Stack* stack;
extern void audio_psp_play(const uint8_t *buf, size_t len);

int __attribute__((optimize("O0"))) run_me_audio(JobData data) {
    (void)data;
    create_next_audio_buffer(audio_buffer + 0 * (SAMPLES_HIGH * 2), SAMPLES_HIGH);
    create_next_audio_buffer(audio_buffer + 1 * (SAMPLES_HIGH * 2), SAMPLES_HIGH);
    return 0;
}

int volatile mediaengine_sound = 0;
int volatile *mediaengine_sound_ptr = &mediaengine_sound;
int mediaengine_available = 0;

int audioOutput(SceSize args, void *argp) {
    (void)args;
    (void)argp;
    bool running = true;
#ifdef DEBUG
    char buffer[64];
#endif

#ifdef ME_EXEC
    if(mediaengine_available) {
        /* Job data for MELib */
        j.jobInfo.id = 1;
        j.jobInfo.execMode = MELIB_EXEC_ME;
        j.function = run_me_audio;
        j.data = 0;
        sceKernelDcacheWritebackInvalidateRange(&j, sizeof(j));
        mediaengine_sound = 1;
    }
#endif

    while (running) {
        AudioTask task = stack_pop(stack);

        #ifdef DEBUG
        switch(task) {
            case NOP: sceGuDebugPrint(8,8,0xffffffff, "NOP");break;
            case QUIT: sceGuDebugPrint(8,16,0xffffffff, "QUIT");break;
            case GENERATE: sceGuDebugPrint(8,24,0xffffffff, "GENERATE");break;
            case PLAY: sceGuDebugPrint(8,32,0xffffffff, "PLAY");break;
        }
        sprintf(buffer, "SOUND: %s", (mediaengine_sound ? "ME" : "CPU"));
        sceGuDebugPrint(10,48,0xffffffff, buffer);
        #endif

        switch (task) {
            case NOP:       {; sceKernelDelayThread(1000 + 1000  * (mediaengine_sound)); }break;
            case QUIT:      {; running = false; }break;
            case GENERATE:  {;
#ifdef ME_EXEC
            if(mediaengine_sound){
                J_AddJob(&j);
                J_Update(0.0f);
            } else
#endif
            {
                run_me_audio(0);
                sceKernelDcacheWritebackInvalidateRange(audio_buffer,sizeof(audio_buffer));
            }
            stack_push(stack, PLAY);
            sceKernelDelayThread(250);
            }
            break;
            case PLAY:      {;
                //sceKernelDelayThread(100);
                //stack_clear(stack);
                audio_api->play((u8 *)audio_buffer, 2 /* 2 buffers */ * SAMPLES_HIGH * sizeof(short) * 2 /* stereo */);
            }
            break;
        }
    }
    sceIoWrite(1,"Audio Manager Exit!\n",21);
    SceUID thid = sceKernelGetThreadId();
    sceKernelTerminateDeleteThread(thid);
    return 0;
}
#endif

extern int gProcessAudio;
void produce_one_frame(void) {
#if defined(TARGET_PSP)
    /* Generate sound */
    stack_push(stack, GENERATE);
#endif

    gfx_start_frame();
    game_loop_one_iteration();

#if !(defined(TARGET_DC) || defined(TARGET_PSP))
    int samples_left = audio_api->buffered();
    u32 num_audio_samples = samples_left < audio_api->get_desired_buffered() ? SAMPLES_HIGH : SAMPLES_LOW;
    //printf("Audio samples: %d %u\n", samples_left, num_audio_samples);
    s16 audio_buffer[SAMPLES_HIGH * 2 * 2];
    for (int i = 0; i < 2; i++) {
        /*if (audio_cnt-- == 0) {
            audio_cnt = 2;
        }
        u32 num_audio_samples = audio_cnt < 2 ? 528 : 544;*/
        create_next_audio_buffer(audio_buffer + i * (num_audio_samples * 2), num_audio_samples);
    }
    //printf("Audio samples before submitting: %d\n", audio_api->buffered());
    audio_api->play((u8 *)audio_buffer, 2 * num_audio_samples * 4);
#endif
#if defined(TARGET_DC)
    audio_api->play(NULL, 2 /* 2 buffers */ * SAMPLES_HIGH * sizeof(short) * 2 /* stereo */);
#endif

    gfx_end_frame();
}

#ifdef TARGET_WEB
static void em_main_loop(void) {
}

static void request_anim_frame(void (*func)(double time)) {
    EM_ASM(requestAnimationFrame(function(time) {
        dynCall("vd", $0, [time]);
    }), func);
}

static void on_anim_frame(double time) {
    static double target_time;

    time *= 0.03; // milliseconds to frame count (33.333 ms -> 1)

    if (time >= target_time + 10.0) {
        // We are lagging 10 frames behind, probably due to coming back after inactivity,
        // so reset, with a small margin to avoid potential jitter later.
        target_time = time - 0.010;
    }

    for (int i = 0; i < 2; i++) {
        // If refresh rate is 15 Hz or something we might need to generate two frames
        if (time >= target_time) {
            produce_one_frame();
            target_time = target_time + 1.0;
        }
    }

    request_anim_frame(on_anim_frame);
}
#endif

static void save_config(void) {
    configfile_save(CONFIG_FILE);
}

static void on_fullscreen_changed(bool is_now_fullscreen) {
    configFullscreen = is_now_fullscreen;
}

#if defined(TARGET_DC)
void *main_pc_pool = NULL;
void *main_pc_pool_gd = NULL;
#endif
void main_func(void) {
#if !defined(TARGET_DC)
    static u32 pool[0x165000/8 / 4 * sizeof(void *) * 2];
#else
    static u8 pool[0x165000+0x70800];
    main_pc_pool = &pool;
    main_pc_pool_gd = &pool[0x165000];
#endif
    main_pool_init(pool, pool + sizeof(pool) / sizeof(pool[0]));
    gEffectsMemoryPool = mem_pool_init(0x4000, MEMORY_POOL_LEFT);

#if !defined(TARGET_DC)
    configfile_load(CONFIG_FILE);
    atexit(save_config);
#else
    (void)save_config;
#endif

#ifdef TARGET_WEB
    emscripten_set_main_loop(em_main_loop, 0, 0);
    request_anim_frame(on_anim_frame);
#endif

#if defined(ENABLE_DX12)
    rendering_api = &gfx_direct3d12_api;
    wm_api = &gfx_dxgi_api;
#elif defined(ENABLE_DX11)
    rendering_api = &gfx_direct3d11_api;
    wm_api = &gfx_dxgi_api;
#elif defined(ENABLE_OPENGL)
    rendering_api = &gfx_opengl_api;
    #if defined(__linux__) || defined(__BSD__)
        wm_api = &gfx_glx;
    #elif defined(TARGET_PSP)
        wm_api = &gfx_psp;
    #elif defined(TARGET_DC)
        wm_api = &gfx_dc;
    #else
        wm_api = &gfx_sdl;
    #endif
#endif

    gfx_init(wm_api, rendering_api, "Super Mario 64 PC-Port", configFullscreen);
    
    wm_api->set_fullscreen_changed_callback(on_fullscreen_changed);
    wm_api->set_keyboard_callbacks(keyboard_on_key_down, keyboard_on_key_up, keyboard_on_all_keys_up);
    
#if HAVE_WASAPI
    if (audio_api == NULL && audio_wasapi.init()) {
        audio_api = &audio_wasapi;
    }
#endif
#if defined(TARGET_PSP)
    if (audio_api == NULL && audio_psp.init()) {
        audio_api = &audio_psp;
    }
#endif
#if defined(TARGET_DC)
    if (audio_api == NULL && audio_dc.init()) {
        audio_api = &audio_dc;
    }
#endif
#if HAVE_PULSE_AUDIO
    if (audio_api == NULL && audio_pulse.init()) {
        audio_api = &audio_pulse;
    }
#endif
#if HAVE_ALSA
    if (audio_api == NULL && audio_alsa.init()) {
        audio_api = &audio_alsa;
    }
#endif
#ifdef TARGET_WEB
    if (audio_api == NULL && audio_sdl.init()) {
        audio_api = &audio_sdl;
    }
#endif
    if (audio_api == NULL) {
        audio_api = &audio_null;
    }

    audio_init();
    sound_init();

    thread5_game_loop(NULL);
#ifdef TARGET_WEB
    /*for (int i = 0; i < atoi(argv[1]); i++) {
        game_loop_one_iteration();
    }*/
    inited = 1;
#else
    inited = 1;
    while (1) {
        wm_api->main_loop(produce_one_frame);
    }
#endif
}

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
int WINAPI WinMain(UNUSED HINSTANCE hInstance, UNUSED HINSTANCE hPrevInstance, UNUSED LPSTR pCmdLine, UNUSED int nCmdShow) {
    main_func();
    return 0;
}
#else
int main(UNUSED int argc, UNUSED char *argv[]) {
    main_func();
    return 0;
}
#endif
