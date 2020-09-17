#include "sceGuDebugPrint.h"

void sceGuDebugPrint(int x, int y, unsigned int color, const char *str) {
    pspDebugScreenSetXY(x / 8, y / 8);
    pspDebugScreenPuts(str);
}
