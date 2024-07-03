
#ifndef INSTRENDERER_COMPONENTEDITOR_H
#define INSTRENDERER_COMPONENTEDITOR_H

#include <QWidget>
#include "QDockWidget"
#include "QToolBox"
#include "QLabel"
#include "QPushButton"
#include "../ComponentUI/Base/MyToolPage.h"
#include "../ComponentUI/Base/MyToolBox.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class ComponentEditor;
}
QT_END_NAMESPACE

class  ComponentEditor : public QDockWidget
{
Q_OBJECT

public:
    explicit ComponentEditor(QDockWidget *parent = nullptr);

    ~ComponentEditor() override;

private:
    void InitToolBox();
    Ui::ComponentEditor *ui;
    MyToolBox* toolBox;
    QScrollArea* scrollArea;
};


#endif //INSTRENDERER_COMPONENTEDITOR_H
