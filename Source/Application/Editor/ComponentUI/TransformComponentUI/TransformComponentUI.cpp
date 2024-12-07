
// You may need to build the project (run Qt uic code generator) to get "ui_TransformComponentUI.h" resolved

#include "TransformComponentUI.h"
#include "ui_TransformComponentUI.h"


TransformComponentUI::TransformComponentUI(Transform data,QWidget *parent) :
        QWidget(parent), ui(new Ui::TransformComponentUI), data(data)
{
    ui->setupUi(this);
    Init();
    InitSignal();
}

TransformComponentUI::~TransformComponentUI()
{
    delete ui;
}

void TransformComponentUI::Init()
{
    QVBoxLayout* layout = new QVBoxLayout(this);
    positionEditor = new Transform3D("transform",data.pos);
    rotationEditor = new Transform3D("rotation",data.rotation);
    scaleEditor = new Transform3D("scale", data.scale);

    layout->addWidget(positionEditor);
    layout->addWidget(rotationEditor);
    layout->addWidget(scaleEditor);
    this->setLayout(layout);
}

void TransformComponentUI::InitSignal()
{
    for(auto* child:this->findChildren<QLineEdit*>())
    {
        connect(child,&QLineEdit::editingFinished,this,[&]()
        {
            glm::vec3 pos = {positionEditor->xEdit->text().toFloat(),
                             positionEditor->yEdit->text().toFloat(),
                             positionEditor->zEdit->text().toFloat()};

            glm::vec3 rot = {rotationEditor->xEdit->text().toFloat(),
                             rotationEditor->yEdit->text().toFloat(),
                            rotationEditor->zEdit->text().toFloat()};

            glm::vec3 scl= {scaleEditor->xEdit->text().toFloat(),
                            scaleEditor->yEdit->text().toFloat(),
                            scaleEditor->zEdit->text().toFloat()};

            emit TransformCompChanged({pos,rot,scl});
        });
    }
    connect(positionEditor,&Transform3D::DragEditFinished,[&]()
    {
        glm::vec3 pos = {positionEditor->xEdit->text().toFloat(),
                         positionEditor->yEdit->text().toFloat(),
                         positionEditor->zEdit->text().toFloat()};

        glm::vec3 rot = {rotationEditor->xEdit->text().toFloat(),
                         rotationEditor->yEdit->text().toFloat(),
                         rotationEditor->zEdit->text().toFloat()};

        glm::vec3 scl= {scaleEditor->xEdit->text().toFloat(),
                        scaleEditor->yEdit->text().toFloat(),
                        scaleEditor->zEdit->text().toFloat()};
        emit TransformCompChanged({pos,rot,scl});
    });
    connect(rotationEditor,&Transform3D::DragEditFinished,[&]()
    {
        glm::vec3 pos = {positionEditor->xEdit->text().toFloat(),
                         positionEditor->yEdit->text().toFloat(),
                         positionEditor->zEdit->text().toFloat()};

        glm::vec3 rot = {rotationEditor->xEdit->text().toFloat(),
                         rotationEditor->yEdit->text().toFloat(),
                         rotationEditor->zEdit->text().toFloat()};

        glm::vec3 scl= {scaleEditor->xEdit->text().toFloat(),
                        scaleEditor->yEdit->text().toFloat(),
                        scaleEditor->zEdit->text().toFloat()};
        emit TransformCompChanged({pos,rot,scl});
    });
    connect(scaleEditor,&Transform3D::DragEditFinished,[&]()
    {
        glm::vec3 pos = {positionEditor->xEdit->text().toFloat(),
                         positionEditor->yEdit->text().toFloat(),
                         positionEditor->zEdit->text().toFloat()};

        glm::vec3 rot = {rotationEditor->xEdit->text().toFloat(),
                         rotationEditor->yEdit->text().toFloat(),
                         rotationEditor->zEdit->text().toFloat()};

        glm::vec3 scl= {scaleEditor->xEdit->text().toFloat(),
                        scaleEditor->yEdit->text().toFloat(),
                        scaleEditor->zEdit->text().toFloat()};
        emit TransformCompChanged({pos,rot,scl});
    });


}

void TransformComponentUI::ChangeData(Transform trans)
{
    this->data = trans;
    positionEditor->UpdateData(data.pos);
    rotationEditor->UpdateData(data.rotation);
    scaleEditor->UpdateData(data.scale);
}


