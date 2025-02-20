
#ifndef INSTRENDERER_CONTENTWIDGET_H
#define INSTRENDERER_CONTENTWIDGET_H

#include <QWidget>
#include "QListWidget"
#include "QDragEnterEvent"
#include "QDropEvent"
#include "qmimedata.h"
#include "QDrag"
#include "iostream"
#include "QMouseEvent"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class ContentWidget;
}
QT_END_NAMESPACE

class ContentWidget : public QListWidget
{
    Q_OBJECT

public:
    explicit ContentWidget(QWidget *parent = nullptr);

    ~ContentWidget() override;
    static QListWidgetItem* dragItem;

signals:

private:
    void startDrag(Qt::DropActions supportedActions) override;
    void mousePressEvent(QMouseEvent* event);
    void dropEvent(QDropEvent* event) override;
    Ui::ContentWidget *ui;

};


#endif //INSTRENDERER_CONTENTWIDGET_H
