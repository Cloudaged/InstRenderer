
#ifndef INSTRENDERER_TRANSFORMCOMPONENTUI_H
#define INSTRENDERER_TRANSFORMCOMPONENTUI_H

#include <QWidget>

#include "QLabel"
#include "QLineEdit"
#include "QLayout"
#include "glm/glm.hpp"
#include "../../Custom/Transform3D.h"
#include "iostream"

#include "../Engine/Common/Component/Components.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class TransformComponentUI;
}
QT_END_NAMESPACE



class  TransformComponentUI : public QWidget
{
Q_OBJECT

public:
    explicit TransformComponentUI(Transform data,QWidget *parent = nullptr);
    void Init();
    void ChangeData(Transform trans);

    ~TransformComponentUI() override;
    Transform3D* positionEditor;
    Transform3D* rotationEditor;
    Transform3D* scaleEditor;
private:
    Transform data;
    void InitSignal();
    Ui::TransformComponentUI *ui;
signals:
    void TransformCompChanged(Transform output);
};


#endif //INSTRENDERER_TRANSFORMCOMPONENTUI_H
