
#ifndef INSTRENDERER_LIGHTCOMPONENTUI_H
#define INSTRENDERER_LIGHTCOMPONENTUI_H

#include <QWidget>
#include "../../Custom/Transform3D.h"
#include "../Engine/Common/Component/Components.h"
#include "../../Custom/TypeSelector.h"
#include "../../Custom/ValueEdit.h"
#include "iostream"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class LightComponentUI;
}
QT_END_NAMESPACE

class LightComponentUI : public QWidget
{
    Q_OBJECT

public:
    explicit LightComponentUI(LightComponent data,QWidget *parent = nullptr);
    void ChangeData(LightComponent data);
    void InitSignal();
    ~LightComponentUI() override;
public:
    LightComponent data;
    Transform3D* colorUI;
    ValueEdit* intensityUI;
    ValueEdit* rangeUI;
    ValueEdit* nearUI;
    ValueEdit* farUI;
    ValueEdit* attenuationUI;
    ValueEdit* outerCutoffUI;
    TypeSelector* lightTypeUI;

private:
    LightComponent UpdateALlData();
    void ChangeToDirectionLight();
    void ChangeToPointLight();
    void ChangeToSpotLight();
    void ClearLayoutItems();
private:
    QVBoxLayout* layout;
    Ui::LightComponentUI *ui;

signals:
    void LightCompChanged(LightComponent data);
};


#endif //INSTRENDERER_LIGHTCOMPONENTUI_H
