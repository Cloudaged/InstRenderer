
#ifndef INSTRENDERER_SCENEEDITOR_H
#define INSTRENDERER_SCENEEDITOR_H

#include "entt.hpp"

#include <QWidget>
#include "QDockWidget"
#include "QTreeWidget"
#include "string"
#include "QStringList"
#include "iostream"

#include "QMenu"
#include "QContextMenuEvent"

#include "SceneTree.h"
#include "QDropEvent"

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

    void AddItem(int id,std::string name,std::string type);
    void AddItem(int id,std::string name,std::string type,int parent);

    void DeleteItem(int id);


    ~SceneEditor() override;
    SceneTree* treeWidget;


private:
    void InitTreeWidget();
    void ConnectFuncs();

    void CheckItem();
    void ItemChanged();
    std::string GetUniqueName(std::string name);
    QTreeWidgetItem* FindItems(QTreeWidgetItem* topItem,int id);

    Ui::SceneEditor *ui;
    int count =0;

    //Events
    void contextMenuEvent(QContextMenuEvent* event) override;

signals:
    void AddObjAction(std::string name,std::string type);
    void AddSubObjAction(std::string name,std::string type,int parent);
    void DeleteObjAction(int id);
    void SelectedItemChanged(int id);

};


#endif //INSTRENDERER_SCENEEDITOR_H
