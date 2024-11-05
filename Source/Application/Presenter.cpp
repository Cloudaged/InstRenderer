
#include "Presenter.h"
#include "Resource/ResourceManager.h"

Presenter::Presenter(GameInstance *gameInstance, MainEditor *mainEditor):instance(gameInstance),editor(mainEditor)
{
    AddGameObject();
    DeleteGameObject();
    RenameGameObject();
    ChangeSelectedItem();
    UpdateComponentData();
    LoadResourceToObj();
}

void Presenter::AddGameObject()
{

    editor->connect(editor->sceneEditor,&SceneEditor::AddObjAction,[&](std::string name,std::string type)
    {
        auto go = instance->mainScene->CreateObject(name);
        editor->sceneEditor->AddItem(static_cast<int>(go->entityID),name,type);

        glm::vec3 pos = {10.0,20.0,0.0};
        instance->mainScene->reg.emplace<Transform>(go->entityID,pos,pos,pos);
        go->componentBits.set(0);
    });

    editor->connect(editor->sceneEditor,&SceneEditor::AddSubObjAction,[&](std::string name,std::string type,int parent)
    {
        auto go = instance->mainScene->CreateObject(name,parent);
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
    auto modelGo =instance->mainScene->CreateObject(resName);
    auto item= editor->sceneEditor->AddItem(static_cast<int>(modelGo->entityID),resName,"GameObject");
    item->setSelected(true);
    editor->sceneEditor->treeWidget->setCurrentItem(item);
   /* for (auto& mesh:model->meshes)
    {
        //Backend
        auto meshGo =instance->mainScene->CreateObject(mesh->name);
        glm::vec3 pos = {10.0,20.0,0.0};
        instance->mainScene->reg.emplace<Transform>(meshGo->entityID,pos,pos,pos);
        meshGo->componentBits.set(0);

        auto meshData= ResourceManager::Get().TransMesh(mesh);
        auto materialData = ResourceManager::Get().TransMaterial(mesh->material);
        instance->mainScene->reg.emplace<Renderable>(meshGo->entityID,meshData,materialData);

        //Frontend
        editor->sceneEditor->AddItem(static_cast<int>(meshGo->entityID),mesh->name,"GameObject",(static_cast<int>(modelGo->entityID)));
    }
    instance->renderSystem.materialManager.AllocateDescriptorSets();*/

   ResourceManager::Get().CompileModel(instance,model);
   editor->sceneEditor->UpdateTree(instance->mainScene->objects);

}
