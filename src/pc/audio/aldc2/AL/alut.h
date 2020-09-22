#pragma once

#include "al.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define ALUT_API_MAJOR_VERSION                  1
#define ALUT_API_MINOR_VERSION                  1

#define ALUT_ERROR_NO_ERROR                     0
#define ALUT_ERROR_OUT_OF_MEMORY                0x200
#define ALUT_ERROR_INVALID_ENUM                 0x201
#define ALUT_ERROR_INVALID_VALUE                0x202
#define ALUT_ERROR_INVALID_OPERATION            0x203
#define ALUT_ERROR_NO_CURRENT_CONTEXT           0x204
#define ALUT_ERROR_AL_ERROR_ON_ENTRY            0x205
#define ALUT_ERROR_ALC_ERROR_ON_ENTRY           0x206
#define ALUT_ERROR_OPEN_DEVICE                  0x207
#define ALUT_ERROR_CLOSE_DEVICE                 0x208
#define ALUT_ERROR_CREATE_CONTEXT               0x209
#define ALUT_ERROR_MAKE_CONTEXT_CURRENT         0x20A
#define ALUT_ERROR_DESTROY_CONTEXT              0x20B
#define ALUT_ERROR_GEN_BUFFERS                  0x20C
#define ALUT_ERROR_BUFFER_DATA                  0x20D
#define ALUT_ERROR_IO_ERROR                     0x20E
#define ALUT_ERROR_UNSUPPORTED_FILE_TYPE        0x20F
#define ALUT_ERROR_UNSUPPORTED_FILE_SUBTYPE     0x210
#define ALUT_ERROR_CORRUPT_OR_TRUNCATED_DATA    0x211

AL_API ALboolean AL_APIENTRY alutInit(int *argcp, char **argv);
AL_API ALboolean AL_APIENTRY alutInitWithoutContext(int *argcp, char **argv);
AL_API ALboolean AL_APIENTRY alutExit(void);

AL_API ALenum AL_APIENTRY alutGetError();
AL_API const ALchar* AL_APIENTRY alutGetErrorString(ALenum error);

AL_API void AL_APIENTRY alutLoadWAVFile (
    ALchar *filename,
    ALenum *format,
    ALvoid **data,
    ALsizei *size,
    ALsizei *frequency,
    ALboolean* loop
);

AL_API void AL_APIENTRY alutUnloadWAV (ALenum format, ALvoid *data, ALsizei size, ALsizei frequency);

#if defined(__cplusplus)
}
#endif
