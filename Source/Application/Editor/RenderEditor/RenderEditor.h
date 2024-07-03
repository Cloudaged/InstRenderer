
#ifndef INSTRENDERER_RENDEREDITOR_H
#define INSTRENDERER_RENDEREDITOR_H

#include <QWidget>
#include "QDockWidget"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class RenderEditor;
}
QT_END_NAMESPACE

class  RenderEditor : public QDockWidget
{
Q_OBJECT

public:
    explicit RenderEditor(QDockWidget *parent = nullptr);

    ~RenderEditor() override;

private:
    Ui::RenderEditor *ui;
};


#endif //INSTRENDERER_RENDEREDITOR_H
