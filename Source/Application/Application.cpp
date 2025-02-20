
#include "Application.h"

Application::Application(std::string title, WindowSize size,int argc,char** argv):
windowContext(std::make_shared<WindowContext>(WindowContext{size, nullptr, false})),
title(title)
{
    InitWindow();
    gameInstance = std::make_shared<GameInstance>(windowContext);

#ifdef WITH_EDITOR
    qApplication = std::make_unique<QApplication>(argc,argv);
    editor = std::make_unique<MainEditor>(windowContext);
    presenter = std::make_shared<Presenter>(gameInstance,editor);

    QFile file("D:\\code_lib\\AGProject\\InstRenderer\\Source\\Application\\Editor\\qss\\Combinear.qss"); // ":/" 表示资源文件路径
    if (file.open(QFile::ReadOnly)) {
        // 读取文件内容
        QString styleSheet = file.readAll();
        // 应用 QSS 样式表到整个应用程序
        qApplication->setStyleSheet(styleSheet);
        file.close();
    } else {
        qWarning("Could not open QSS file.");
    }
#endif

}

void Application::Run()
{
#ifdef WITH_EDITOR
    editor->StartGameThread(gameInstance);
#else
    SDL_ShowWindow(windowContext->window);
    gameInstance->Run();
#endif
}

int Application::Quit()
{
#ifdef WITH_EDITOR
    int qAppExecCode = qApplication->exec();
    SDL_DestroyWindow(windowContext->window);
    return qAppExecCode;
#else
    SDL_DestroyWindow(windowContext->window);
    return 0;
#endif
}

void Application::InitWindow()
{
    if(SDL_Init(SDL_INIT_EVERYTHING)<0)
    {
        std::cout<<"Failed to create sdl window\n";
    }
    windowContext->window = SDL_CreateWindow(title.c_str(),SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,
                              windowContext->windowSize.width,windowContext->windowSize.height,
                              SDL_WINDOW_HIDDEN|SDL_WINDOW_VULKAN|SDL_WINDOW_RESIZABLE);
}

Application::~Application()
{

}





