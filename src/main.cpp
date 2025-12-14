#include "tigr.h"

int main(int argc, char *argv[])
{
    Tigr *screen = tigrWindow(320, 240, "Hello", TIGR_AUTO);
    while (!tigrClosed(screen))
    {
        tigrClear(screen, tigrRGB(0x80, 0x90, 0xa0));
        tigrPrint(screen, tfont, screen->w / 2, 110, tigrRGB(0xff, 0xff, 0xff), "Hello, world.");
        tigrFillRect(screen, 20, 20, 20, 20, tigrRGB(0xFF, 0xFF, 0xa0));
        tigrUpdate(screen);
    }
    tigrFree(screen);
    return 0;
}