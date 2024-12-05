
#ifndef INSTRENDERER_SCENEOBSERVER_H
#define INSTRENDERER_SCENEOBSERVER_H

class Camera;
class Light;
class SceneSubject;
class SceneObserver
{
    friend class SceneSubject;
protected:
    SceneObserver(){};

    virtual void onCameraUpdate(Camera& camera) = 0;
    virtual void onLightUpdate(Light* light) = 0;
};
#endif //INSTRENDERER_SCENEOBSERVER_H
