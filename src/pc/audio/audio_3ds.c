#ifdef TARGET_N3DS

#include <3ds.h>
#include "macros.h"
#include "audio_api.h"

#define N3DS_DSP_DMA_BUFFER_COUNT   4

static int sNextBuffer;
static ndspWaveBuf sDspBuffers[N3DS_DSP_DMA_BUFFER_COUNT];

static bool audio_3ds_init(void)
{
    ndspInit();

    ndspSetOutputMode(NDSP_OUTPUT_STEREO);
    ndspChnReset(0);
    ndspChnWaveBufClear(0);
    ndspChnSetInterp(0, NDSP_INTERP_LINEAR);
    ndspChnSetRate(0, 32000);
    ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);

    float mix[12];
    memset(mix, 0, sizeof(mix));
    mix[0] = 1.0;
    mix[1] = 1.0;
    ndspChnSetMix(0, mix);

    u8* bufferData = linearAlloc(4096 * 4 * N3DS_DSP_DMA_BUFFER_COUNT);
    for (int i = 0; i < N3DS_DSP_DMA_BUFFER_COUNT; i++)
    {
        sDspBuffers[i].data_vaddr = &bufferData[i * 4096 * 4];
        sDspBuffers[i].nsamples = 0;
        sDspBuffers[i].status = NDSP_WBUF_FREE;
    }

    sNextBuffer = 0;
    return true;
}

static int audio_3ds_buffered(void)
{
    int total = 0;
    for (int i = 0; i < N3DS_DSP_DMA_BUFFER_COUNT; i++)
    {
        if (sDspBuffers[i].status == NDSP_WBUF_QUEUED ||
            sDspBuffers[i].status == NDSP_WBUF_PLAYING)
            total += sDspBuffers[i].nsamples;
    }
    return total;
}

static int audio_3ds_get_desired_buffered(void)
{
    return 1100;
}

static void audio_3ds_play(const uint8_t *buf, size_t len)
{
    if (len > 4096 * 4)
        return;
    if (sDspBuffers[sNextBuffer].status != NDSP_WBUF_FREE &&
        sDspBuffers[sNextBuffer].status != NDSP_WBUF_DONE)
        return;
    s16* dst = (s16*)sDspBuffers[sNextBuffer].data_vaddr;
    memcpy(dst, buf, len);
    DSP_FlushDataCache(dst, len);
    sDspBuffers[sNextBuffer].nsamples = len / 4;
    sDspBuffers[sNextBuffer].status = NDSP_WBUF_FREE;
    ndspChnWaveBufAdd(0, &sDspBuffers[sNextBuffer]);
    sNextBuffer = (sNextBuffer + 1) % N3DS_DSP_DMA_BUFFER_COUNT;
}

struct AudioAPI audio_3ds =
{
    audio_3ds_init,
    audio_3ds_buffered,
    audio_3ds_get_desired_buffered,
    audio_3ds_play
};

#endif
