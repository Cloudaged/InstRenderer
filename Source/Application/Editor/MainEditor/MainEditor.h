
#ifndef INSTRENDERER_MAINEDITOR_H
#define INSTRENDERER_MAINEDITOR_H

#include <QMainWindow>
#include "../SceneEditor/SceneEditor.h"
#include "../ComponentEditor/ComponentEditor.h"
#include "../ResourceEditor/ResourceEditor.h"
#include "../RenderEditor/RenderEditor.h"
#include "../ConsoleEditor/ConsoleEditor.h"
#include "../EditorTypes.h"
#include "SDL2/SDL.h"

#include "QMenuBar"
#include "QMenu"
#include "QAction"

#include "QCloseEvent"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainEditor;
}
QT_END_NAMESPACE

struct EditorInitializer
{
    SDL_Window* window;
};


class  MainEditor : public QMainWindow
{
Q_OBJECT

public:
    explicit MainEditor(SDL_Window* window,QWidget *parent = nullptr);


    ~MainEditor() override;


    //SubEditor
    SceneEditor* sceneEditor;
    ComponentEditor* componentEditor;
    ResourceEditor* resourceEditor;
    RenderEditor* renderEditor;
    ConsoleEditor* consoleEditor;
    bool* isClose;


private:
    void DeleteCentralWidget();
    void InitMenuBar();
    void InitSubWidget();
    void InitSubWidgetLayoutAndShow();

    void closeEvent(QCloseEvent *event) override
    {
        *isClose = true;
        event->accept();      // 接受关闭事件
    }
    //ToolBar
    QMenuBar* menuBar;
    QMenu* fileMenu;
    QAction* loadAction;
    SDL_Window* window;

    Ui::MainEditor *ui;

signals:
    void AddNewNode(std::string name, std::string type);
    void DeleteNode();

};


#endif //INSTRENDERER_MAINEDITOR_H
