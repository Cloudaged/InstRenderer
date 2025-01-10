
#ifndef INSTRENDERER_GRAPHICSETTINGWIDGET_H
#define INSTRENDERER_GRAPHICSETTINGWIDGET_H

#include <QWidget>
#include <QDockWidget>
#include <QScrollArea>
#include <QResizeEvent>
#include "QTextEdit"
#include "../Custom/MyToolBox.h"
#include "../Custom/MyToolPage.h"
#include "Render/Uniforms.h"
#include "../Custom/ValueEdit.h"
#include "../Custom/TriggerBox.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class GraphicSettingWidget;
}
QT_END_NAMESPACE

class GraphicSettingWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit GraphicSettingWidget(QDockWidget *parent = nullptr);

    ~GraphicSettingWidget() override;

private:
    void InitToolBox();
    void InitPages();
    void ShadowSettings();
    void resizeEvent(QResizeEvent *event) override;
private:
    RenderSettingUniform renderSettingData;
    std::vector<ContentWithType> pages;
    MyToolBox* toolBox;
    QScrollArea* scrollArea;
    Ui::GraphicSettingWidget *ui;

    ValueEdit* pcfEdit;
    ValueEdit* bsEdit;
    TriggerBox* showCascade;
    RenderSettingUniform UpdateAllData();
signals:
    void GraphicSettingUpdate(RenderSettingUniform data);
};


#endif //INSTRENDERER_GRAPHICSETTINGWIDGET_H
