
#ifndef INSTRENDERER_SCENEEDITOR_H
#define INSTRENDERER_SCENEEDITOR_H

#include <QWidget>
#include "QDockWidget"
#include "QTreeWidget"
#include "string"
#include "QStringList"
#include "iostream"

#include "QMenu"
#include "QContextMenuEvent"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class SceneEditor;
}
QT_END_NAMESPACE

class  SceneEditor : public QDockWidget
{
Q_OBJECT

public:
    explicit SceneEditor(QDockWidget *parent = nullptr);

    ~SceneEditor() override;
    QTreeWidget* treeWidget;


private:
    void InitTreeWidget();
    void AddItem(std::string name,std::string type);

    Ui::SceneEditor *ui;

    void contextMenuEvent(QContextMenuEvent* event) override;
    int count =0;
signals:
    void AddObjAction();
};


#endif //INSTRENDERER_SCENEEDITOR_H
