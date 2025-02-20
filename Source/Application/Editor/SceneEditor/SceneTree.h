
#ifndef INSTRENDERER_SCENETREE_H
#define INSTRENDERER_SCENETREE_H

#include "QMouseEvent"

#include <QWidget>
#include "QTreeWidget"
#include "iostream"
#include "QMimeData"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class SceneTree;
}
QT_END_NAMESPACE

class SceneTree : public QTreeWidget
{
Q_OBJECT

public:
    explicit SceneTree(QTreeWidget *parent = nullptr);

    ~SceneTree() override;


    QTreeWidgetItem* curItem= nullptr;
private:
    void dropEvent(QDropEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event);
    void closeEditor(QWidget *editor, QAbstractItemDelegate::EndEditHint hint) override;
    Ui::SceneTree *ui;

signals:
    void ItemRenamed(int id,std::string dstName);
    void DropResource(std::string path);
};


#endif //INSTRENDERER_SCENETREE_H
