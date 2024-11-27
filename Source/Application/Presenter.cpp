
#include "Presenter.h"
#include "Resource/ResourceManager.h"

Presenter::Presenter(std::shared_ptr<GameInstance> gameInstance,std::shared_ptr<MainEditor> mainEditor):instance(gameInstance),editor(mainEditor)
{
    AddGameObject();
    DeleteGameObject();
    RenameGameObject();
    ChangeSelectedItem();
    UpdateComponentData();
    LoadResourceToObj();


    editor->sceneEditor->UpdateTree(instance->mainScene->objects);
}

void Presenter::AddGameObject()
{

    editor->connect(editor->sceneEditor,&SceneEditor::AddObjAction,[&](std::string name,std::string type)
    {
        auto go = instance->mainScene->CreateObject(name,type);
        editor->sceneEditor->AddItem(static_cast<int>(go->entityID),name,type);
    });

    editor->connect(editor->sceneEditor,&SceneEditor::AddSubObjAction,[&](std::string name,std::string type,int parent)
    {
        auto go = instance->mainScene->CreateObject(name,parent,type);
        editor->sceneEditor->AddItem(static_cast<int>(go->entityID),name,type,parent);
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
        editor->componentEditor->ChangeGameObject(go.get(),&instance->mainScene->reg);

    });
}

void Presenter::UpdateComponentData()
{
    editor->connect(editor->componentEditor->transformCompUI,&TransformComponentUI::TransformCompChanged,[&](Transform output)
    {
        auto& trans = instance->mainScene->reg.get<Transform>(editor->componentEditor->curID);
        trans = output;
        instance->mainScene->UpdateLightData();
        std::cout<<"Trigger\n";
    });

    editor->connect(editor->componentEditor->lightComponentUI,&LightComponentUI::LightCompChanged,[&](LightComponent output)
    {
        auto& light = instance->mainScene->reg.get<LightComponent>(editor->componentEditor->curID);
        light = output;
        instance->mainScene->UpdateLightData();
    });
}

void Presenter::LoadResourceToObj()
{
    editor->connect(editor->sceneEditor->treeWidget,&SceneTree::DropResource,[&](std::string path)
    {
        auto extension= GetExtension(path);
        if(extension=="fbx"||extension=="gltf")
        {
            CreateMeshObjectsForRes(path);
        }

    });
}

void Presenter::CreateMeshObjectsForRes(std::string path)
{
    auto resName = ResourceManager::Get().LoadResource(path);
    auto model= (Res::ResModel*)ResourceManager::Get().resReg[resName];


   ResourceManager::Get().CompileModel(instance.get(),model);
   editor->sceneEditor->UpdateTree(instance->mainScene->objects);

}

void Presenter::AddComponentsForGo(std::string type, GameObject* go)
{
    if(type=="GameObject")
    {
    }
    else if(type=="Light")
    {

    }
}


