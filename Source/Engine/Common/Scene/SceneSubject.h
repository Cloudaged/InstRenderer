
#ifndef INSTRENDERER_SCENESUBJECT_H
#define INSTRENDERER_SCENESUBJECT_H
#include <functional>
#include <vector>
#include "SceneObserver.h"
class Camera;

class SceneSubject
{
public:
    void AddObserver(SceneObserver* observer);
protected:
    void RemoveObserver(SceneObserver* observer);
    void NotifyLights();
    void NotifyCamera(Camera& cam);
private:
    std::vector<SceneObserver*> observers;
};


#endif //INSTRENDERER_SCENESUBJECT_H
