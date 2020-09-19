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

#if defined(_WIN32)
#include <windows.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// Globals

static unsigned long systemRam = 0x00000000;
static unsigned long elfOffset = 0x00000000;
static unsigned long stackSize = 0x00000000;

/////////////////////////////////////////////////////////////////////////////
// Symbols

#if defined(_arch_dreamcast)
extern unsigned long end;
extern unsigned long start;
#define _END end
#define _START start
#else
extern unsigned long end;
extern unsigned long _start;
#define _END end
#define _START _executable_start
#endif

/////////////////////////////////////////////////////////////////////////////
//  Functions

unsigned long getFreeRam(void) {
#if defined(_WIN32) || XBOX
    MEMORYSTATUS stat;
    memset(&stat, 0, sizeof(MEMORYSTATUS));
    stat.dwLength = sizeof(MEMORYSTATUS);
    GlobalMemoryStatus(&stat);
    return stat.dwAvailPhys - stackSize;
#elif LINUX
    struct sysinfo info;
    sysinfo(&info);
    return info.freeram - stackSize;
#else
    struct mallinfo mi = mallinfo();
    return systemRam - (mi.usmblks + stackSize);
#endif
}

void setSystemRam(void) {
#if defined(_arch_dreamcast)
    // 16 MBytes - ELF Memory Map:
    systemRam = 0x8d000000 - 0x8c000000;
    elfOffset = 0x8c000000;
#elif PSP
    // 24 MBytes - ELF Memory Map:
    systemRam = 0x01800000 - 0x00000000;
    elfOffset = 0x00000000;
    if (getHardwareModel() == 1)
        systemRam += 32 * 1024 * 1024;
#elif GP2X
    // 32 MBytes - ELF Memory Map:
    systemRam = 0x02000000 - 0x00000000;
    elfOffset = 0x00000000;
    if (gp2x_init() == 2)
        systemRam += 32 * 1024 * 1024;
#else
    systemRam = getFreeRam();
#endif
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
           3145728 - (int) getFreeRam(),
           getUsedRam());
}
#endif
