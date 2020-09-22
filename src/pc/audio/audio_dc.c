#if defined(TARGET_AL)
#include "macros.h"
#include "audio_api.h"
#include <stdio.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <assert.h>
#include <string.h>

#define DC_AUDIO_CHANNELS 2
#define DC_STEREO_AUDIO (1)
#define DC_AUDIO_FREQUENCY 32000
#define DC_AUDIO_GIVEN_BUFFERS (2)
#define DC_AUDIO_SAMPLES_DESIRED (544 * DC_AUDIO_GIVEN_BUFFERS)

/* Double Buffer */
static int cur_snd_buf = 0;

/* 2 back to back 1088 sample sound buffers */
uint16_t snd_buffer_internal[DC_AUDIO_SAMPLES_DESIRED * DC_AUDIO_CHANNELS * 2] __attribute__((aligned(64)));
void *snd_buffer[2] = { snd_buffer_internal,
                        snd_buffer_internal + (DC_AUDIO_SAMPLES_DESIRED * DC_AUDIO_CHANNELS) };

ALCdevice *device = NULL;
ALCcontext *context = NULL;
ALuint source = 0;
ALuint buffer = 0;

static bool audio_dc_init(void) {
    const ALCchar *defaultDeviceName = alcGetString(NULL, ALC_DEFAULT_DEVICE_SPECIFIER);
    device = alcOpenDevice(defaultDeviceName);
    assert(device);

    context = alcCreateContext(device, NULL);
    assert(context);
    alcMakeContextCurrent(context);

    alGetError();

    alGenSources((ALuint) 1, &source);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    alSourcef(source, AL_PITCH, 1);
    alSourcef(source, AL_GAIN, 1);
    alSource3f(source, AL_POSITION, 0, 0, 0);
    alSource3f(source, AL_VELOCITY, 0, 0, 0);
    alSourcei(source, AL_LOOPING, AL_FALSE);

    alGenBuffers(1, &buffer);
    //alBufferData(buffer, AL_FORMAT_STEREO16, snd_buffer[cur_snd_buf], DC_AUDIO_SAMPLES_DESIRED * sizeof(short), DC_AUDIO_FREQUENCY);
    //alSourcei(source, AL_BUFFER, buffer);
    //alSourcePlay(source);

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
    /* Copy new sound into buffer, it *might* be played */
    //memcpy(snd_buffer[cur_snd_buf], buf, len);
    memset(snd_buffer[cur_snd_buf], 0xDD, DC_AUDIO_SAMPLES_DESIRED * sizeof(short));

    alBufferData(buffer, AL_FORMAT_MONO16, snd_buffer[cur_snd_buf], DC_AUDIO_SAMPLES_DESIRED * sizeof(short), 44100);
    alSourcei(source, AL_BUFFER, buffer);
    alSourcePlay(source);
    thd_pass(10);

    /* Swap double buffer back */
    cur_snd_buf ^= 1;
}

struct AudioAPI audio_dc = {
    audio_dc_init,
    audio_dc_buffered,
    audio_dc_get_desired_buffered,
    audio_dc_play
};
#endif
