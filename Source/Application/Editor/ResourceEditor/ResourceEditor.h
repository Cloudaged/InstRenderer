
#ifndef INSTRENDERER_RESOURCEEDITOR_H
#define INSTRENDERER_RESOURCEEDITOR_H

#include <QWidget>
#include "QDockWidget"
#include "QListWidget"
#include "QTreeWidget"
#include "../Engine/Common/Core/PathDefine.h"
#include "QHBoxLayout"
#include "QSplitter"
#include "QDir"
#include "iostream"
#include "../IconManager.h"
#include "ContentWidget.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class ResourceEditor;
}
QT_END_NAMESPACE

class  ResourceEditor : public QDockWidget
{
Q_OBJECT

public:
    explicit ResourceEditor(QDockWidget *parent = nullptr);

    ~ResourceEditor() override;

private:
    void InitListWidget();
    void InitFolderWidget();
    void ShowFileOfPath(std::string path);
    std::string selectedPath;

    Ui::ResourceEditor *ui;
    ContentWidget* contentWidget;
    QTreeWidget* folderWidget;
    void SetIcon(QListWidgetItem* item,QString extension);

};


#endif //INSTRENDERER_RESOURCEEDITOR_H
