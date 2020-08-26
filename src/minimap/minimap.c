#ifdef TARGET_N3DS

#include <stdio.h>
#include <string.h>

#include "minimap.h"

static u8 *fb;

static u8 level_index = 0;
static s32 current_level = -1;

// map area is 16384x16384 (-8192 to +8192 in x and y)
static const float MAP_OFFSET = 8192.0f;
// translate 0->16384 into 0->256
static const float MAP_SCALE = 0.015625f;

void minimap_load_level(s32 level_number)
{
    switch (level_number)
    {
        case 1:  /* LEVEL_UNKNOWN_2 */
        case 2:  /* LEVEL_UNKNOWN_3 */
        case 3:  /* LEVEL_UNKNOWN_3 */
        case 25: /* LEVEL_ENDING */
        case 32: /* LEVEL_UNKNOWN_32 */
        case 35: /* LEVEL_UNKNOWN_35 */
        case 37: /* LEVEL_UNKNOWN_37 */
        case 38: /* LEVEL_UNKNOWN_38 */
            level_index = 13; /* no map */
            break;
        case 4:              /* LEVEL_BBH */
            level_index = 1;
            break;
        case 5:              /* LEVEL_CCM */
            level_index = 2;
            break;
        case 6:              /* LEVEL_CASTLE */
            level_index = 3;
            break;
        case 7:              /* LEVEL_HMC */
            level_index = 4;
            break;
        case 8:              /* LEVEL_SSL */
            level_index = 5;
            break;
        case 9:              /* LEVEL_BOB */
            level_index = 6;
            break;
        case 10:              /* LEVEL_SL */
            level_index = 7;
            break;
        case 11:              /* LEVEL_WDW */
            level_index = 8;
            break;
        case 12:              /* LEVEL_JRB */
            level_index = 9;
            break;
        case 13:              /* LEVEL_THI */
            level_index = 10;
            break;
        case 14:              /* LEVEL_TTC */
            level_index = 11;
            break;
        case 15:              /* LEVEL_RR */
            level_index = 12;
            break;
        case 16:              /* LEVEL_CASTLE_GROUNDS */
            level_index = (save_file_get_flags() & SAVE_FLAG_MOAT_DRAINED) ? 31 : 13;
            break;
        case 17:              /* LEVEL_BITDW*/
            level_index = 14;
            break;
        case 18:              /* LEVEL_VCUTM */
            level_index = 15;
            break;
        case 19:              /* LEVEL_BITFS */
            level_index = 16;
            break;
        case 20:              /* LEVEL_SA */
            level_index = 17;
            break;
        case 21:              /* LEVEL_BITS */
            level_index = 18;
            break;
        case 22:              /* LEVEL_LLL */
            level_index = 19;
            break;
        case 23:              /* LEVEL_DDD */
            level_index = 20;
            break;
        case 24:              /* LEVEL_WF */
            level_index = 21;
            break;
        case 26:              /* LEVEL_CASTLE_COURTYARD */
            level_index = 22;
            break;
        case 27:              /* LEVEL_PSS */
            level_index = 23;
            break;
        case 28:              /* LEVEL_COTMC */
            level_index = 24;
            break;
        case 29:              /* LEVEL_TOTWC */
            level_index = 25;
            break;
        case 30:              /* LEVEL_BOWSER_1 */
            level_index = 26;
            break;
        case 31:              /* LEVEL_WMOTR */
            level_index = 27;
            break;
        case 33:              /* LEVEL_BOWSER_2 */
            level_index = 28;
            break;
        case 34:              /* LEVEL_BOWSER_3 */
            level_index = 29;
            break;
        case 36:              /* LEVEL_TTM*/
            level_index = 30;
            break;
        default:
            level_index = 0; // no map
    }
}

void minimap_load_current_texture(uint8_t **texture, size_t *texture_size)
{
    if (gCurrLevelNum != current_level)
    {
        minimap_load_level(gCurrLevelNum);
        current_level = gCurrLevelNum;
    }

#pragma GCC diagnostic ignored "-Wdiscarded-qualifiers"
    *texture = level_info[level_index].texture;
#pragma GCC diagnostic pop
    *texture_size = level_info[level_index].size;
}

bool minimap_get_mario_position(float *mario_x, float *mario_y)
{
    if (level_index == 0)
    {
        // no level, so nothing to do
        return false;
    }

    // get mario's current position
    float cur_mario_x = gMarioState->pos[0]; // x
    float cur_mario_y = gMarioState->pos[2]; // y

    // scale for the mini map and clamp between 0 and 256
    float mario_x_scaled_and_offset = MIN(256.0f, MAX(0.0f, (cur_mario_x + MAP_OFFSET) * MAP_SCALE));
    float mario_y_scaled_and_offset = MIN(256.0f, MAX(0.0f, (cur_mario_y + MAP_OFFSET) * MAP_SCALE));

    *mario_x = mario_x_scaled_and_offset;
    *mario_y = mario_y_scaled_and_offset;

    return true;
}

#endif
