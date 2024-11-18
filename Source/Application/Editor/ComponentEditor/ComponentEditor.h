
#ifndef INSTRENDERER_COMPONENTEDITOR_H
#define INSTRENDERER_COMPONENTEDITOR_H

#include <QWidget>
#include "QDockWidget"
#include "QToolBox"
#include "QLabel"
#include "QResizeEvent"
#include "QPushButton"
#include "../ComponentUI/Base/MyToolPage.h"
#include "../ComponentUI/Base/MyToolBox.h"

#include "entt.hpp"
#include "../Engine/Common/GameObject.h"

#include "../Engine/Common/Component/Components.h"
#include "../ComponentUI/TransformComponentUI/TransformComponentUI.h"
#include "../ComponentUI/LightComponentUI/LightComponentUI.h"
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

    void ChangeGameObject(GameObject* gameObject,entt::registry* reg);

    ~ComponentEditor() override;

    entt::entity curID;
    TransformComponentUI* transformCompUI;
    LightComponentUI* lightComponentUI;
private:
    void InitToolBox();
    void resizeEvent(QResizeEvent *event) override;
    Ui::ComponentEditor *ui;
    MyToolBox* toolBox;
    QScrollArea* scrollArea;
};


#endif //INSTRENDERER_COMPONENTEDITOR_H
