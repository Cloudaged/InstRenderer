
#ifndef INSTRENDERER_MAINEDITOR_H
#define INSTRENDERER_MAINEDITOR_H

#include <QMainWindow>
#include "../SceneEditor/SceneEditor.h"
#include "../ComponentEditor/ComponentEditor.h"
#include "../ResourceEditor/ResourceEditor.h"
#include "../RenderEditor/RenderEditor.h"
#include "../ConsoleEditor/ConsoleEditor.h"
#include "../EditorTypes.h"
#include "../GameThread.h"
#include "SDL2/SDL.h"
#include "QFile"
#include "QMenuBar"
#include "QMenu"
#include "QAction"
#include "QFileDialog"
#include "Common/Core/PathDefine.h"
#include "Common/Core/WindowContext.h"
#include "Common/GameInstance.h"
#include "QCloseEvent"
#include "memory"
#include "../CustomTitleBar.h"
#include "QVBoxLayout"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainEditor;
}
QT_END_NAMESPACE


class  MainEditor : public QMainWindow
{
Q_OBJECT

public:
    explicit MainEditor(std::shared_ptr<WindowContext> windowContext,QWidget *parent = nullptr);
    void StartGameThread(std::shared_ptr<GameInstance> gameInstance);
    ~MainEditor() override;
public:
    SceneEditor* sceneEditor;
    ComponentEditor* componentEditor;
    ResourceEditor* resourceEditor;
    RenderEditor* renderEditor;
    ConsoleEditor* consoleEditor;
private:
    void DeleteCentralWidget();
    void InitMenuBar();
    void InitSubWidget();
    void InitSubWidgetLayoutAndShow();
    void InitCustomTitleBar();
    void closeEvent(QCloseEvent *event) override;
private:
    CustomTitleBar *titleBar;
    QVBoxLayout* layout;
    std::unique_ptr<GameThread> gameThread;
    std::shared_ptr<WindowContext> windowContext;
    Ui::MainEditor *ui;
signals:
    void AddNewNode(std::string name, std::string type);
    void DeleteNode();
    void LoadAction(std::string path);

};


#endif //INSTRENDERER_MAINEDITOR_H
