
#include "GameInstance.h"
#include "iostream"



GameInstance::GameInstance(WindowSize size): size(size)
{
    InitCore();
    InitWindow(size);
    controller = new Controller(&event);
    InitVulkanContext();
    InitEntity();
    mainScene->InitGlobalSet();
    mainScene->InitSceneData();
    InitSystem();
}

void GameInstance::InitWindow(WindowSize size)
{
    if(SDL_Init(SDL_INIT_EVERYTHING)<0)
    {
        std::cout<<"Failed to create sdl window\n";
    }

    window = SDL_CreateWindow("GameInstance",
                              SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                              size.width,size.height,
                              SDL_WINDOW_SHOWN|SDL_WINDOW_VULKAN|SDL_WINDOW_RESIZABLE);

    SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);


}

void GameInstance::InitCore()
{
    //Meta
    RegisterMeta();
    //Scene
    mainScene = new Scene;
    //Registry
    entityManager = new EntityManager(&mainScene->reg);
    //Locker
}

void GameInstance::InitVulkanContext()
{
    VulkanContext::Init(window,&event);
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
    SDLEvent();
    if(!isRun)
        return;
    if(!isReady)
    {
        isReady = true;
        return;
    }
    mainScene->UpdateScene();
    renderSystem.Execute();
}

void GameInstance::InitEntity()
{
    auto model = ModelLoader::Load("D:\\code_lib\\AGProject\\InstRenderer\\Asset\\CratesAndBarrels.fbx");


    auto t = Res::ImageLoader::Load("D:\\code_lib\\AGProject\\InstRenderer\\Asset\\Crate_Base_Color.png");
    AllocatedImage img(VK_FORMAT_R8G8B8A8_SRGB,VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_SAMPLED_BIT,
                       {(uint32_t)t->width,(uint32_t)t->height},1,VK_IMAGE_ASPECT_COLOR_BIT);

    img.LoadData(t);


    PBRMaterial material;
    material.AddTexture(new Texture(img));
    material.Setup();

    Mesh mesh(model->meshes[0].verts, model->meshes[0].index);

    glm::vec3 color = {1.0,0.0,0.0};
    entt::entity e1 = mainScene->reg.create();
    mainScene->reg.emplace<Renderable>(e1,mesh,material,color);
    mainScene->reg.emplace<Transform>(e1, glm::vec3{0.0,0.0,0.0},
                                      glm::vec3{0,0.0,0.0},
                                      glm::vec3{1.0,1.0,1.0});
}

void GameInstance::Run(bool* isClose)
{
    while ((!(*isClose)))
    {
        Tick();
    }
}

void GameInstance::SDLEvent()
{
    while (SDL_PollEvent(&event))
    {
        controller->ViewInteract(&mainScene->mainCamera);

        if(event.type==SDL_WINDOWEVENT)
        {
            if(event.window.event==SDL_WINDOWEVENT_RESIZED)
            {
                std::lock_guard<std::mutex> lock(Locker::Get().resizeMtx);

                size.width= event.window.data1;
                size.height = event.window.data2;
                mainScene->mainCamera.UpdateAspect();
            }
            else if(event.window.event ==SDL_WINDOWEVENT_MINIMIZED)
            {
                isRun= false;
            }
            else if(event.window.event==SDL_WINDOWEVENT_RESTORED)
            {
                isRun= true;
            }
        }
    }
}



