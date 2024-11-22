
#ifndef INSTRENDERER_TRANSFORM3D_H
#define INSTRENDERER_TRANSFORM3D_H

#include <QWidget>
#include "QLabel"
#include "QLineEdit"

#include "QHBoxLayout"
#include "glm/glm.hpp"
#include "DragableLabel.h"
#include "QRegularExpressionValidator"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class Transform3D;
}
QT_END_NAMESPACE


class Transform3D : public QWidget
{
Q_OBJECT

public:
    explicit Transform3D(std::string tagName,glm::vec3 data,QWidget *parent = nullptr);

    ~Transform3D() override;
    QLabel tag;
    QLineEdit* xEdit,*yEdit,*zEdit;
    glm::vec3 data;

    float xWeight=1,yWeight=1,zWeight=1;
    void UpdateData(glm::vec3 data);
private:
    std::string tagName;
    void InitWidget();
    void InitSignal();
    DragableLabel* xLabel,*yLabel,*zLabel;

    Ui::Transform3D *ui;
signals:
    void DragEditFinished(glm::vec3 data);
};


#endif //INSTRENDERER_TRANSFORM3D_H
