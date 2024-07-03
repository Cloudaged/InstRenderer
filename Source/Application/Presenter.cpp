
#include "Presenter.h"

GameObject* go;

Presenter::Presenter(GameInstance *gameInstance, MainEditor *mainEditor):instance(gameInstance),editor(mainEditor)
{
    AddGameObject();
    DeleteGameObject();
    RenameGameObject();
}

void Presenter::AddGameObject()
{

    editor->connect(editor->sceneEditor,&SceneEditor::AddObjAction,[&](std::string name,std::string type)
    {
        auto ins = instance->mainScene->CreateObject(name,type);
        GameObject go = ins.cast<GameObject>();
        editor->sceneEditor->AddItem(static_cast<int>(go.entityID),name,type);
    });

    editor->connect(editor->sceneEditor,&SceneEditor::AddSubObjAction,[&](std::string name,std::string type,int parent)
    {
        auto ins = instance->mainScene->CreateObject(name,type,parent);
        GameObject go = ins.cast<GameObject>();
        std::cout<<go.parent<<"\n";
        editor->sceneEditor->AddItem(static_cast<int>(go.entityID),name,type,parent);
    });

}

void Presenter::DeleteGameObject()
{
    editor->connect(editor->sceneEditor,&SceneEditor::DeleteObjAction,[&](int id)
    {
        editor->sceneEditor->DeleteItem(id);
        instance->mainScene->DeleteObject(id);
    });
}

void Presenter::RenameGameObject()
{
    editor->connect(editor->sceneEditor->treeWidget,&SceneTree::ItemRenamed,[&](int id,std::string dstName)
    {
        instance->mainScene->RenameObject(id,dstName);
    });
}
