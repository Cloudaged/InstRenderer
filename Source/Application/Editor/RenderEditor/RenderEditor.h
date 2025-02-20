
#ifndef INSTRENDERER_RENDEREDITOR_H
#define INSTRENDERER_RENDEREDITOR_H

#include <QWidget>
#include "QDockWidget"
#include "SDL2/SDL.h"
#include "SDL2/SDL_syswm.h"
#include "Common/Core/Locker.h"
#include "QResizeEvent"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class RenderEditor;
}
QT_END_NAMESPACE

class  RenderEditor : public QDockWidget
{
Q_OBJECT

public:
    explicit RenderEditor(SDL_Window* window,QDockWidget *parent = nullptr);
    SDL_Window* sdlWindow;

    ~RenderEditor() override;

private:
    void resizeEvent(QResizeEvent *event) override;
    Ui::RenderEditor *ui;
};


#endif //INSTRENDERER_RENDEREDITOR_H
