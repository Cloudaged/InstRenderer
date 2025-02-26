
#include "Presenter.h"
#include "Resource/ResourceManager.h"
#include "Render/VulkanContext.h"
extern RenderSettings globalRenderSettingData;
Presenter::Presenter(std::shared_ptr<GameInstance> gameInstance,std::shared_ptr<MainEditor> mainEditor):instance(gameInstance),editor(mainEditor)
{
    AddGameObject();
    DeleteGameObject();
    RenameGameObject();
    ChangeSelectedItem();
    UpdateComponentData();
    LoadResourceToObj();
    UpdateGraphicSetting();

    editor->sceneEditor->UpdateTree(instance->mainScene->sceneRootGameObject);
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
        auto parentGO = instance->mainScene->GetGameObject(parent);
        auto go = instance->mainScene->CreateObject(name,parentGO,type);
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

void Presenter::UpdateTrans(std::shared_ptr<GameObject> go,entt::registry& reg)
{
    auto& trans = reg.get<Transform>(go->entityID);

    if(go->parent!= nullptr)
    {
        trans.localTransform = EngineMath::GetModelMatrix(trans);
        auto& parentTrans = reg.get<Transform>(go->parent->entityID);
        trans.globalTransform = parentTrans.globalTransform*trans.localTransform;

        if(go->componentBits.test((int)ComponentType::Renderable))
        {
            auto& renderable = reg.get<Renderable>(go->entityID);
            TLAS& tlas = instance->mainScene->rtScene.topAS;
            for (auto& instance:tlas.instances)
            {
                if(instance.instanceCustomIndex == renderable.nodeID)
                {
                    instance.transform = EngineMath::GlmToVkTransform(trans.globalTransform);
                    break;
                }
            }
            memcpy(tlas.instancesHostAddress,
                   tlas.instances.data(),
                   tlas.instances.size()*sizeof(VkAccelerationStructureInstanceKHR));
            RTBuilder::UpdateTransform(tlas);
        }

        trans.isDirty = false;
    }

    for (auto& child : go->child)
    {
        UpdateTrans(child,reg);
    }
}

void Presenter::UpdateComponentData()
{
    editor->connect(editor->componentEditor->transformCompUI,&TransformComponentUI::TransformCompChanged,[&](Transform output)
    {
        auto go = instance->mainScene->GetGameObject((int)editor->componentEditor->curID);

        auto& trans = instance->mainScene->reg.get<Transform>(editor->componentEditor->curID);
        trans = output;

        UpdateTrans(go,instance->mainScene->reg);
        instance->renderSystem.UpdateLightArray();
    });

    editor->connect(editor->componentEditor->lightComponentUI,&LightComponentUI::LightCompChanged,[&](LightComponent output)
    {
        auto& light = instance->mainScene->reg.get<LightComponent>(editor->componentEditor->curID);
        light = output;
        instance->renderSystem.UpdateLightArray();
    });
}

void Presenter::LoadResourceToObj()
{
    editor->connect(editor->sceneEditor->treeWidget,&SceneTree::DropResource,[&](std::string path)
    {
        auto extension= GetExtension(path);
        if(extension=="fbx"||extension=="gltf"||extension=="glb")
        {
            CreateMeshObjectsForRes(path);
        }

    });
}

void Presenter::CreateMeshObjectsForRes(std::string path)
{
    auto resName = ResourceManager::Get().LoadResource(path);
    std::shared_ptr<Res::ResModel> model= std::static_pointer_cast<Res::ResModel>(ResourceManager::Get().resReg[resName]);


   ResourceManager::Get().CompileModel(instance.get(),model);
   editor->sceneEditor->UpdateTree(instance->mainScene->sceneRootGameObject);

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

void Presenter::UpdateGraphicSetting()
{
    editor->connect(editor->graphicSettingWidget,&GraphicSettingWidget::GraphicSettingUpdate,[&](RenderSettingUniform data)
    {
        globalRenderSettingData.uniform = data;
    });}


