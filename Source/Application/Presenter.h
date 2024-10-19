
#ifndef INSTRENDERER_PRESENTER_H
#define INSTRENDERER_PRESENTER_H
#include "Common/GameInstance.h"
#include "Editor/MainEditor/MainEditor.h"
class Presenter
{
public:
    Presenter(GameInstance* gameInstance,MainEditor* mainEditor);

    GameInstance* instance;
    MainEditor* editor;

private:
    void UpdateComponentData();
    void ChangeSelectedItem();
    void AddGameObject();
    void DeleteGameObject();
    void RenameGameObject();
    void LoadResource();

    void CreateMeshObject(std::string path);
};


#endif //INSTRENDERER_PRESENTER_H
