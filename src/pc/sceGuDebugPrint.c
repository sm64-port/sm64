#if defined(TARGET_PSP)
extern void pspDebugScreenSetXY(int x, int y);
extern int pspDebugScreenPuts(const char *str);

void sceGuDebugPrint(int x, int y, __attribute__((unused)) unsigned int color, const char *str) {
    pspDebugScreenSetXY(x / 8, y / 8);
    pspDebugScreenPuts(str);
}
#endif
