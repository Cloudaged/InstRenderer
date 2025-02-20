
#ifndef INSTRENDERER_TRANSFORM3D_H
#define INSTRENDERER_TRANSFORM3D_H

#include <QWidget>
#include "QLabel"
#include "QLineEdit"

#include "QHBoxLayout"
#include "Common/Core/glmConfig.h"
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
    void UpdateData(glm::vec3 data);
    ~Transform3D() override;
public:
    QLabel tag;
    QLineEdit* xEdit,*yEdit,*zEdit;
    glm::vec3 data;
    float xWeight=1,yWeight=1,zWeight=1;
private:
    void InitWidget();
    void InitSignal();
private:
    DragableLabel* xLabel,*yLabel,*zLabel;
    std::string tagName;
    Ui::Transform3D *ui;
signals:
    void DragEditFinished(glm::vec3 data);
};


#endif //INSTRENDERER_TRANSFORM3D_H
