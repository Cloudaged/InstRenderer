
#ifndef INSTRENDERER_DRAGABLELABEL_H
#define INSTRENDERER_DRAGABLELABEL_H

#include <QWidget>
#include "QEnterEvent"
#include "QLabel"
#include "QMouseEvent"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class DragableLabel;
}
QT_END_NAMESPACE

class DragableLabel : public QLabel
{
    Q_OBJECT

public:
    explicit DragableLabel(QString text,QWidget *parent = nullptr);
    void enterEvent(QEnterEvent* event) override;
    void leaveEvent(QEvent* event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;


    ~DragableLabel() override;

private:
    bool isDrag;
    QPoint lastMousePosition;
    float value =0;
    Ui::DragableLabel *ui;
signals:
    void ChangeData(float value);
};


#endif //INSTRENDERER_DRAGABLELABEL_H
