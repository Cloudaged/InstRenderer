
// You may need to build the project (run Qt uic code generator) to get "ui_RenderEditor.h" resolved

#include "RenderEditor.h"
#include "ui_RenderEditor.h"
#include "iostream"

RenderEditor::RenderEditor(SDL_Window* window,QDockWidget *parent) :
        QDockWidget(parent), ui(new Ui::RenderEditor),sdlWindow(window)
{
    ui->setupUi(this);
    setAcceptDrops(true);

    SDL_SysWMinfo systemInfo;
    SDL_VERSION(&systemInfo.version);
    SDL_SetWindowFullscreen(sdlWindow, SDL_WINDOW_FULLSCREEN_DESKTOP);
    SDL_GetWindowWMInfo(sdlWindow, &systemInfo);
    HWND SDLHandle = systemInfo.info.win.window;
    ::SetParent(SDLHandle, (HWND)this->winId());
    SDL_ShowWindow(sdlWindow);

}

RenderEditor::~RenderEditor()
{
    delete ui;
}

void RenderEditor::resizeEvent(QResizeEvent *event)
{
    std::lock_guard<std::mutex> lock(Locker::Get().resizeMtx);
    SDL_RestoreWindow(this->sdlWindow);
    SDL_MaximizeWindow(this->sdlWindow);
}
