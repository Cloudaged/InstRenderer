
#ifndef INSTRENDERER_TRIGGERBOX_H
#define INSTRENDERER_TRIGGERBOX_H

#include <QWidget>
#include "QLabel"
#include "QCheckBox"
#include "QHBoxLayout"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class TriggerBox;
}
QT_END_NAMESPACE

class TriggerBox : public QWidget
{
    Q_OBJECT

public:
    explicit TriggerBox(std::string name,bool data,QWidget *parent = nullptr);
    void UpdateData(bool value);
    QLabel* tag;
    QCheckBox* trigger;
    bool value;
    ~TriggerBox() override;

private:
    Ui::TriggerBox *ui;
};


#endif //INSTRENDERER_TRIGGERBOX_H
