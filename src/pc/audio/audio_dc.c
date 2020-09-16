#include "macros.h"
#include "audio_api.h"

static bool audio_dc_init(void) {
    return true;
}

static int audio_dc_buffered(void) {
    return 0;
}

static int audio_dc_get_desired_buffered(void) {
    return 0;
}

static void audio_dc_play(UNUSED const uint8_t *buf, UNUSED size_t len) {
}

struct AudioAPI audio_dc = {
    audio_dc_init,
    audio_dc_buffered,
    audio_dc_get_desired_buffered,
    audio_dc_play
};
