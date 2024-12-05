
#include "Controller.h"

Controller::Controller(SDL_Event *event):event(event)
{

}

void Controller::ViewInteract(Camera* cam)
{
    if(event->type == SDL_KEYUP)
    {
        switch (event->key.keysym.sym)
        {
            case SDLK_SPACE:
                interactInfo.isSpace = false;
                break;
        }
    }
    else if(event->type == SDL_KEYDOWN)
    {
        switch (event->key.keysym.sym)
        {
            case SDLK_SPACE:
                interactInfo.isSpace = true;
                break;
        }
    }
    else if(event->type == SDL_MOUSEWHEEL)
    {
        if(event->wheel.y>0)
            cam->SetCameraPos((cam->GetCameraPos()+10.0f*cam->GetCameraTarget()));
        if(event->wheel.y<0)
            cam->SetCameraPos((cam->GetCameraPos()-10.0f*cam->GetCameraTarget()));
    }
    else if(event->type == SDL_MOUSEBUTTONDOWN)
    {
        if(event->button.button == SDL_BUTTON_LEFT)
        {
            glm::vec3 pos= cam->GetCameraPos();
            float xSquare = pow((pos.x-0),2);
            float ySquare = pow((pos.y-0),2);
            float zSquare = pow((pos.z-0),2);
            interactInfo.distance = 0.001*sqrt(xSquare+ySquare+zSquare);

            interactInfo.isDrag = true;
            interactInfo.dragStartPos.x = event->button.x;
            interactInfo.dragStartPos.y = event->button.y;

        }
    }
    else if (event->type == SDL_MOUSEMOTION && interactInfo.isDrag&&interactInfo.isSpace)
    {

        float deltaX = event->motion.x - interactInfo.dragStartPos.x;
        float deltaY = event->motion.y - interactInfo.dragStartPos.y;
        cam->pitch += 0.05*deltaY;
        cam->yaw -= 0.05*deltaX;
        interactInfo.dragStartPos.x = event->motion.x;
        interactInfo.dragStartPos.y = event->motion.y;
        if(cam->pitch>89.0f)
            cam->pitch =89.0f;
        if(cam->pitch<-89.0f)
            cam->pitch =-89.0f;
        glm::vec3 front =cam->GetCameraTarget();

        front.x += cos(glm::radians(cam->yaw))* cos(glm::radians(cam->pitch)) ;
        front.y += sin(glm::radians(cam->pitch));
        front.z += sin(glm::radians(cam->yaw))* cos(glm::radians(cam->pitch)) ;

        front = glm::normalize(front);
        cam->SetCameraTarget(front);
    }
    else if (event->type == SDL_MOUSEMOTION && interactInfo.isDrag)
    {
        float deltaX = event->motion.x - interactInfo.dragStartPos.x;
        float deltaY = event->motion.y - interactInfo.dragStartPos.y;

        interactInfo.dragStartPos.x = event->motion.x;
        interactInfo.dragStartPos.y = event->motion.y;

        deltaX *=interactInfo.distance;
        deltaY *= interactInfo.distance;


        cam->SetCameraPos(cam->GetCameraPos()+cam->xAxis*deltaX);
        cam->SetCameraPos(cam->GetCameraPos()+cam->yAxis*deltaY);


    }
    else if (event->type == SDL_MOUSEBUTTONUP)
    {
        if (event->button.button == SDL_BUTTON_LEFT)
        {
            SDL_ShowCursor(1);
            interactInfo.isDrag = false;
        }
    }
}
