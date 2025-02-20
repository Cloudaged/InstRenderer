
#ifndef INSTRENDERER_PRESENTER_H
#define INSTRENDERER_PRESENTER_H
#include "Common/GameInstance.h"
#include "Editor/MainEditor/MainEditor.h"


class Presenter
{
public:
    Presenter(std::shared_ptr<GameInstance> gameInstance,std::shared_ptr<MainEditor> mainEditor);
public:
    std::shared_ptr<GameInstance> instance;
    std::shared_ptr<MainEditor> editor;
private:
    void UpdateTrans(std::shared_ptr<GameObject> go,entt::registry& reg);
    void UpdateComponentData();
    void ChangeSelectedItem();
    void AddGameObject();
    void DeleteGameObject();
    void RenameGameObject();
    void LoadResourceToObj();
    void UpdateGraphicSetting();
    void AddComponentsForGo(std::string type,GameObject* go);
    void CreateMeshObjectsForRes(std::string path);
};


#endif //INSTRENDERER_PRESENTER_H
