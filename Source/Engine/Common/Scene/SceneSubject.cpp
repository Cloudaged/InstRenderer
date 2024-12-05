
#include "SceneSubject.h"

void SceneSubject::AddObserver(SceneObserver *observer)
{
    observers.push_back(observer);
}

void SceneSubject::RemoveObserver(SceneObserver *observer)
{

}

void SceneSubject::NotifyCamera(Camera& cam)
{
    for (const auto& obs : observers)
    {
        obs->onCameraUpdate(cam);
    }
}
