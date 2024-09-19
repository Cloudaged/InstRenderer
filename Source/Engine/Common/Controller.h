
#ifndef INSTRENDERER_CONTROLLER_H
#define INSTRENDERER_CONTROLLER_H
#include "SDL2/SDL.h"
#include "Camera.h"
class Controller
{
public:
    Controller(SDL_Event* event);
    void ViewInteract(Camera* cam);
    struct InteractInfo
    {
        bool isSpace;
        float distance;
        bool isDrag;
        glm::vec2 dragStartPos;
    }interactInfo;

    SDL_Event* event;
};


#endif //INSTRENDERER_CONTROLLER_H
