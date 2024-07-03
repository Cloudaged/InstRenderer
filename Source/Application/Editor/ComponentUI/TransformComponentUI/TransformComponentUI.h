
#ifndef INSTRENDERER_TRANSFORMCOMPONENTUI_H
#define INSTRENDERER_TRANSFORMCOMPONENTUI_H

#include <QWidget>

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
    explicit TransformComponentUI(QWidget *parent = nullptr);

    ~TransformComponentUI() override;

private:
    Ui::TransformComponentUI *ui;
};


#endif //INSTRENDERER_TRANSFORMCOMPONENTUI_H
