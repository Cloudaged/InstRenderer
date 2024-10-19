
#include "Presenter.h"
#include "Resource/ResourceManager.h"
GameObject* go;

Presenter::Presenter(GameInstance *gameInstance, MainEditor *mainEditor):instance(gameInstance),editor(mainEditor)
{
    AddGameObject();
    DeleteGameObject();
    RenameGameObject();
    ChangeSelectedItem();
    UpdateComponentData();
    LoadResource();
}

void Presenter::AddGameObject()
{

    editor->connect(editor->sceneEditor,&SceneEditor::AddObjAction,[&](std::string name,std::string type)
    {
        auto& ins = instance->mainScene->CreateObject(name,type);
        GameObject& go = ins.cast<GameObject&>();
        editor->sceneEditor->AddItem(static_cast<int>(go.entityID),name,type);

        glm::vec3 pos = {10.0,20.0,0.0};
        instance->mainScene->reg.emplace<Transform>(go.entityID,pos,pos,pos);
        go.componentBits.set(0);
    });

    editor->connect(editor->sceneEditor,&SceneEditor::AddSubObjAction,[&](std::string name,std::string type,int parent)
    {
        auto ins = instance->mainScene->CreateObject(name,type,parent);
        GameObject go = ins.cast<GameObject>();
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

void Presenter::ChangeSelectedItem()
{
    editor->connect(editor->sceneEditor,&SceneEditor::SelectedItemChanged,[&](int id)
    {
        auto go = instance->mainScene->GetGameObject(id);
        editor->componentEditor->ChangeGameObject(go,&instance->mainScene->reg);

    });
}

void Presenter::UpdateComponentData()
{
    editor->connect(editor->componentEditor->transformCompUI,&TransformComponentUI::TransformCompChanged,[&](Transform output)
    {
        auto& trans = instance->mainScene->reg.get<Transform>(editor->componentEditor->curID);
        trans = output;
        auto& tran2 = instance->mainScene->reg.get<Transform>(editor->componentEditor->curID);

        std::cout<<tran2.pos.x<<"\n";
    });
}

void Presenter::LoadResource()
{
    editor->connect(editor->sceneEditor->treeWidget,&SceneTree::DropResource,[&](std::string path)
    {
        auto extension= GetExtension(path);
        if(extension=="fbx")
        {
            CreateMeshObject(path);
        }

    });
}

void Presenter::CreateMeshObject(std::string path)
{
    /*auto name = GetNameFromPath(path);
    ResourceManager::Get().LoadResource(path);
    auto model = (Res::ResModel*)ResourceManager::Get().resReg[name];

    auto& ins = instance->mainScene->CreateObject(name,"MeshObject");
    GameObject& go = ins.cast<GameObject&>();
    editor->sceneEditor->AddItem(static_cast<int>(go.entityID),name,type);

    glm::vec3 pos = {10.0,20.0,0.0};
    instance->mainScene->reg.emplace<Transform>(go.entityID,pos,pos,pos);
    go.componentBits.set(0);*/
}
