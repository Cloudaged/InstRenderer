
#ifndef INSTRENDERER_GRAPHICSETTINGWIDGET_H
#define INSTRENDERER_GRAPHICSETTINGWIDGET_H

#include <QWidget>
#include <QDockWidget>
#include <QScrollArea>
#include <QResizeEvent>
#include "QTextEdit"
#include "../Custom/MyToolBox.h"
#include "../Custom/MyToolPage.h"
#include "Render/RenderSetting.h"
#include "../Custom/ValueEdit.h"
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
    RenderSettingData renderSettingData;
    std::vector<ContentWithType> pages;
    MyToolBox* toolBox;
    QScrollArea* scrollArea;
    Ui::GraphicSettingWidget *ui;

    ValueEdit* pcfEdit;
    ValueEdit* bsEdit;
    RenderSettingData UpdateAllData();
signals:
    void GraphicSettingUpdate(RenderSettingData data);
};


#endif //INSTRENDERER_GRAPHICSETTINGWIDGET_H
