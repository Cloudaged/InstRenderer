
#ifndef INSTRENDERER_WINDOWCONTEXT_H
#define INSTRENDERER_WINDOWCONTEXT_H
#include "SDL2/SDL.h"
struct WindowSize
{
    int width;
    int height;
};

struct WindowContext
{
    WindowSize windowSize;
    SDL_Window* window;
    bool isClose;
    bool isMinimal = false;
};
#endif //INSTRENDERER_WINDOWCONTEXT_H
