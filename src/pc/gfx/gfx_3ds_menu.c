#ifdef TARGET_N3DS

#include "3ds.h"
#include "gfx_3ds_menu.h"

static PrintConsole sConsole;
int selected = 0;

int display_menu(struct gfx_configuration *config)
{
    consoleSelect(consoleInit(GFX_BOTTOM, &sConsole));

    int res = 0;

    consoleClear();
    hidScanInput();

    // draw menu
    printf("\x1b[1;1HSM64 3DS Configuration\n");
    printf("\x1b[3;3H %s Anti-Aliasing [%s]\n", selected == 0 ? ">" : " ", config->useAA ? "ON" : "OFF");
    printf("\x1b[4;3H %s Render Mode   [%s]\n", selected == 1 ? ">" : " ", config->useWide ? "800" : "400");
    printf("\x1b[5;3H %s Resume Game       \n", selected == 2 ? ">" : " ");
    printf("\x1b[6;3H %s Exit Game         \n", selected == 3 ? ">" : " ");

    u32 kDown = hidKeysDown();
    if (kDown & KEY_DOWN)
    {
        selected = (selected + 1) % 4;
    }
    else if (kDown & KEY_UP)
    {
        selected = (selected + 3) % 4;
    }
    else if (kDown & KEY_A)
    {
        if (selected == 0)
        {
            config->useAA = !config->useAA;
        }
        else if (selected == 1)
        {
            config->useWide = !config->useWide;
        }
        else if (selected == 2)
        {
            res = 1;
        } else
        {
            res = -1;
        }
    }

    // Flush and swap framebuffers
    gfxFlushBuffers();
    //gfxSwapBuffers();

    //Wait for VBlank
    //gspWaitForVBlank();

    return res;
}

#endif
