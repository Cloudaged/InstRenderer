
#ifndef INSTRENDERER_RESOURCEEDITOR_H
#define INSTRENDERER_RESOURCEEDITOR_H

#include <QWidget>
#include "QDockWidget"
#include "QListWidget"

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

    Ui::ResourceEditor *ui;
    QListWidget* listWidget;

};


#endif //INSTRENDERER_RESOURCEEDITOR_H
