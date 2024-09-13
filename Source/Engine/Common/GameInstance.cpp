
#include "GameInstance.h"
#include "iostream"

GameInstance::GameInstance(WindowSize size)
{
    InitCore();
    InitWindow(size);
    InitVulkanContext();
    InitEntity();
    mainScene->InitGlobalSet();
    InitSystem();
}

void GameInstance::InitWindow(WindowSize size)
{
    if(SDL_Init(SDL_INIT_EVERYTHING)<0)
    {
        std::cout<<"Failed to create sdl window\n";
    }

    window = SDL_CreateWindow("GameInstance",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,size.width,size.height,SDL_WINDOW_SHOWN|SDL_WINDOW_VULKAN);

}

void GameInstance::InitCore()
{
    //Meta
    RegisterMeta();
    //Scene
    mainScene = new Scene;
    //Registry
    entityManager = new EntityManager(&mainScene->reg);
}

void GameInstance::InitVulkanContext()
{
    VulkanContext::Init(window);
}

void GameInstance::InitSystem()
{
    //Render System
    RenderSysBeginInfo renderSysInfo{};
    renderSysInfo.registry = &mainScene->reg;
    renderSysInfo.globalData = mainScene->globalData;
    renderSystem.BeginSystem(renderSysInfo);

}

void GameInstance::Tick()
{
    mainScene->UpdateScene();
    renderSystem.Execute();
}

void GameInstance::InitEntity()
{
    auto model = ModelLoader::Load("D:\\code_lib\\AGProject\\InstRenderer\\Asset\\CratesAndBarrels.fbx");


    PBRMaterial material;

    std::vector<Vertex> verts =
            {
                    {{0.0,-1.0,1.0},{0.0,0.0,0.0},{0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
                    {{0.5,1.0,1.0},{0.0,0.0,0.0},{0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}},
                    {{1.0,1.0,1.0},{0.0,0.0,0.0},{0.0,0.0},{0.0,0.0,0.0},{0.0,0.0,0.0}}
            };


    Mesh mesh(model->meshes[0].verts, model->meshes[0].index);

    glm::vec3 color = {1.0,0.0,0.0};
    entt::entity e1 = mainScene->reg.create();
    mainScene->reg.emplace<Renderable>(e1,mesh,material,color);
    mainScene->reg.emplace<Transform>(e1, glm::vec3{1.0,1.0,1.0},
                                      glm::vec3{0.0,0.0,0.0},
                                      glm::vec3{0.0,0.0,0.0});

}



