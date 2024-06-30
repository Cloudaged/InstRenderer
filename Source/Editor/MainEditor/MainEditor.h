
#ifndef INSTRENDERER_MAINEDITOR_H
#define INSTRENDERER_MAINEDITOR_H

#include <QMainWindow>
#include "../SceneEditor/SceneEditor.h"
#include "../ComponentEditor/ComponentEditor.h"
#include "../ResourceEditor/ResourceEditor.h"
#include "../RenderEditor/RenderEditor.h"
#include "../ConsoleEditor/ConsoleEditor.h"

#include "QMenuBar"
#include "QMenu"
#include "QAction"

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
    explicit MainEditor(QWidget *parent = nullptr);


    ~MainEditor() override;


    //SubEditor
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

    //Emit func
    void EmitFunc();

    //ToolBar
    QMenuBar* menuBar;
    QMenu* fileMenu;
    QAction* loadAction;

    Ui::MainEditor *ui;

signals:
    void AddNewNode();

};


#endif //INSTRENDERER_MAINEDITOR_H
