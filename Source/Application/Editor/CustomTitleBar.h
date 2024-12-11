
#ifndef INSTRENDERER_CUSTOMTITLEBAR_H
#define INSTRENDERER_CUSTOMTITLEBAR_H

#include <QApplication>
#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include "QMenu"
#include "QMenuBar"
#include "QFileDialog"
#include "Common/Core/PathDefine.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class CustomTitleBar;
}
QT_END_NAMESPACE
class MainEditor;
class CustomTitleBar : public QWidget
{
    Q_OBJECT

public:
    explicit CustomTitleBar(QWidget *parent = nullptr);
    QMenuBar* menuBar;
    QMenu* fileMenu;
    QAction* loadAction;
    QPushButton *minimizeButton;
    QPushButton *maximizeButton;
    QPushButton *closeButton;
    ~CustomTitleBar() override;
    MainEditor* mainEditor;
signals:
    void closeWindow();
    void minimizeWindow();
    void toggleMaximizeWindow();

private:
    void ChangeToMax();
    void ChangeToMin();
    void mousePressEvent(QMouseEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;

    void mouseReleaseEvent(QMouseEvent *event) override;

    void mouseDoubleClickEvent(QMouseEvent *event) override;
    bool isMax = false;
    bool isDragging = false;
    QPoint dragStartPos;
    QHBoxLayout* titleLayout;
    QVBoxLayout* mainLayout;
    void InitTitleBar();
    void InitMenuBar();
    Ui::CustomTitleBar *ui;
};


#endif //INSTRENDERER_CUSTOMTITLEBAR_H
