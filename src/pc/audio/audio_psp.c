#if defined(TARGET_PSP)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "audio_api.h"
#include <pspkernel.h>
#include <pspaudio.h>

#define PSP_AUDIO_CHANNELS 2
#define PSP_AUDIO_FREQUENCY 32000
#define PSP_AUDIO_GIVEN_BUFFERS (2)
#define PSP_AUDIO_SAMPLES_DESIRED (544 * PSP_AUDIO_GIVEN_BUFFERS)

static int chan = -1;
static int samples = 0;

extern void memcpy_vfpu(void *dst, const void *src, size_t size);
extern void init_audiomanager(void);
extern struct Stack *createStack(unsigned capacity);
struct Stack *stack;

/* Double Buffer */
static int cur_snd_buf = 0;
uint16_t snd_buffer_internal[PSP_AUDIO_SAMPLES_DESIRED * PSP_AUDIO_CHANNELS * 2] /* 2 back to back 1088 sample sound buffers */
    __attribute__((aligned(64)));
void *snd_buffer[2] = { snd_buffer_internal,
                        snd_buffer_internal + (PSP_AUDIO_SAMPLES_DESIRED * PSP_AUDIO_CHANNELS) };

static bool audio_psp_init(void) {
    /* Let current audio run out then release */
    while (sceAudioOutput2GetRestSample() > 0)
        sceKernelDelayThread(1000);

    sceAudioSRCChRelease();

    /* Setup new raw audio stream */
    chan = sceAudioSRCChReserve(PSP_AUDIO_SAMPLES_DESIRED, PSP_AUDIO_FREQUENCY, PSP_AUDIO_CHANNELS);
    if (chan < 0) {
        return false;
    }
    samples = PSP_AUDIO_SAMPLES_DESIRED;
    stack = createStack(14);
    init_audiomanager();

    return true;
}

/* unused currently */
static int audio_psp_buffered(void) {
    int ret = PSP_AUDIO_SAMPLES_DESIRED - sceAudioOutput2GetRestSample();
    ret = (ret > 0 ? ret : 0);
    return ret;
}

static int audio_psp_get_desired_buffered(void) {
    /* This is more than 1088 */
    return 1100;
}

void audio_psp_play(const uint8_t *buf, size_t len) {
    int new_samples = len / (sizeof(short) * PSP_AUDIO_CHANNELS);

    sceKernelDcacheInvalidateRange(buf, len);
    memcpy_vfpu(snd_buffer[cur_snd_buf], buf, len);
    sceKernelDcacheWritebackInvalidateRange(snd_buffer[cur_snd_buf], len);

    /* Check if we need to reacquire channel or num samples */
    if (chan < 0) {
        /* Sleep while still playing last sound, 7ms is half a frame at 60fps */
        while (sceAudioOutput2GetRestSample() > 0)
            sceKernelDelayThread(1000);
        if (chan >= 0)
            sceAudioSRCChRelease();
        chan = sceAudioSRCChReserve(new_samples, PSP_AUDIO_FREQUENCY, PSP_AUDIO_CHANNELS);
    }
    if (new_samples != samples) {
        sceAudioOutput2ChangeLength(new_samples);
        samples = new_samples;
    }

    sceAudioOutput2OutputBlocking(PSP_AUDIO_VOLUME_MAX, snd_buffer[cur_snd_buf]);

    /* Swap double buffer back */
    cur_snd_buf ^= 1;
}

struct AudioAPI audio_psp = {
    audio_psp_init,
    audio_psp_buffered,
    audio_psp_get_desired_buffered,
    audio_psp_play
};
#endif
