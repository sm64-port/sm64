#if defined(TARGET_DC)
/*
 * OpenBOR - http://www.LavaLit.com
 * -----------------------------------------------------------------------
 * Licensed under the BSD license, see LICENSE in OpenBOR root for details.
 *
 * Copyright (c) 2004 - 2009 OpenBOR Team
 */

/*
 * This library is used for calculating how much memory is available/used.
 * Certain platforms offer physical memory statistics, we obviously wrap
 * around those functions.  For platforms where we can't retrieve this
 * information we then calculate the estimated sizes based on a few key
 * variables and symbols.  These estimated values should tolerable.......
 */

/////////////////////////////////////////////////////////////////////////////
// Libraries

#include <malloc.h>
#include <string.h>
#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
// Globals

static unsigned long systemRam = 0x00000000;
static unsigned long elfOffset = 0x00000000;
static unsigned long stackSize = 0x00000000;

/////////////////////////////////////////////////////////////////////////////
// Symbols

#if defined(TARGET_DC)
extern unsigned long end;
extern unsigned long start;
#define _END end
#define _START start
#endif

/////////////////////////////////////////////////////////////////////////////
//  Functions

unsigned long getFreeRam(void) {
    struct mallinfo mi = mallinfo();
    return systemRam - (mi.usmblks + stackSize);
}

void setSystemRam(void) {
    // 16 MBytes - ELF Memory Map:
    systemRam = 0x8d000000 - 0x8c000000;
    elfOffset = 0x8c000000;
    stackSize = (int) &_END - (int) &_START + ((int) &_START - elfOffset);
}

unsigned long getSystemRam(void) {
    return systemRam;
}

unsigned long getUsedRam(void) {
    return (systemRam - getFreeRam());
}

void getRamStatus(void) {
    printf("stack: start:%x end:%x\n", (int) &_START, (int) &_END);
    printf("Total Ram: %lu, Free Ram: %lu, Need: %d,  Used Ram: %lu\n",
           getSystemRam(),
           getFreeRam(),
           2200000 - (int) getFreeRam(),
           getUsedRam());
}
#endif
