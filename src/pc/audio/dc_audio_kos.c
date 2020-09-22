#if defined(TARGET_DC)
#include "macros.h"
#include "audio_api.h"
#include <kos.h>
#include <dc/sound/stream.h>
#include <stdio.h>

#define DC_AUDIO_CHANNELS 2
#define DC_STEREO_AUDIO (1)
#define DC_AUDIO_FREQUENCY 32000
#define DC_AUDIO_GIVEN_BUFFERS (2)
#define SAMPLES_HIGH (544)
//#define DC_AUDIO_SAMPLES_DESIRED (SAMPLES_HIGH * DC_AUDIO_GIVEN_BUFFERS * 2 /* to help pad space */)
#define DC_AUDIO_SAMPLES_DESIRED (0x10000 / 4)

/* Double Buffer */
snd_stream_hnd_t shnd = -1;

/* 0x10000 bytes */
uint16_t snd_buffer_internal[DC_AUDIO_SAMPLES_DESIRED * DC_AUDIO_CHANNELS] __attribute__((aligned(64)));


void *audio_callback(UNUSED snd_stream_hnd_t hnd, int samples_requested, int *samples_returned) {
    int backup_req = samples_requested;
    void *snd_buf = snd_buffer_internal;
    do {
        //printf("left to generate %d/%d bytes\n", samples_requested, backup_req);
        create_next_audio_buffer(snd_buf, SAMPLES_HIGH);
        snd_buf += (SAMPLES_HIGH * DC_AUDIO_CHANNELS * sizeof(short));
        samples_requested -= (SAMPLES_HIGH * DC_AUDIO_CHANNELS * sizeof(short));
    } while (samples_requested >= (SAMPLES_HIGH * DC_AUDIO_CHANNELS * sizeof(short)));
    create_next_audio_buffer(snd_buf, SAMPLES_HIGH);
    snd_buf += (SAMPLES_HIGH * DC_AUDIO_CHANNELS * sizeof(short));

    //int ret = backup_req-samples_requested;
    int ret = backup_req;
    *samples_returned = ret;
    void *buf = snd_buffer_internal;
    //printf("%s:%d asked for %d and gave %d @ %x with %d left \n", __func__, __LINE__, backup_req, ret, (unsigned int) buf, samples_requested);
    //fflush(stdout);
    return snd_buffer_internal;
}

static bool audio_dc_init(void) {
    if (snd_stream_init()) {
        printf("AICA INIT FAILURE!\n");
        fflush(stdout);
        return false;
    }

    shnd = snd_stream_alloc(audio_callback, SND_STREAM_BUFFER_MAX / 2 /* 0xFFFF */);
    if (shnd == SND_STREAM_INVALID) {
        printf("SND: creation failure!");
        fflush(stdout);
        return false;
    }

    //snd_stream_start(shnd, DC_AUDIO_FREQUENCY, DC_STEREO_AUDIO);
    memset(snd_buffer_internal, '\0', DC_AUDIO_SAMPLES_DESIRED * DC_AUDIO_CHANNELS * 2);
    return true;
}

static int audio_dc_buffered(void) {
    return 0;
}

static int audio_dc_get_desired_buffered(void) {
    /* This is more than 1088 */
    return 1100;
}

static void audio_dc_play(const uint8_t *buf, size_t len) {
    int ret = snd_stream_poll(shnd);
    if (ret) {
        printf("SND: %d\n", ret);
        fflush(stdout);
    }
    thd_sleep(1);
    //thd_pass();
}

struct AudioAPI audio_dc = {
    audio_dc_init,
    audio_dc_buffered,
    audio_dc_get_desired_buffered,
    audio_dc_play
};
#endif
