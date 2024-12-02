
#ifndef INSTRENDERER_VALUEEDIT_H
#define INSTRENDERER_VALUEEDIT_H

#include <QWidget>
#include "string"
#include "QLabel"
#include "QLineEdit"

#include "QHBoxLayout"
#include "glm/glm.hpp"
#include "DragableLabel.h"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class ValueEdit;
}
QT_END_NAMESPACE

class ValueEdit : public QWidget
{
    Q_OBJECT

public:
    explicit ValueEdit(std::string name,float data,QWidget *parent = nullptr);
    void UpdateData(float data);
    ~ValueEdit() override;
public:
    DragableLabel* tag;
    float data;
    QLineEdit* edit;
private:
    Ui::ValueEdit *ui;
signals:
    void DragEditFinished(float data);
};


#endif //INSTRENDERER_VALUEEDIT_H
