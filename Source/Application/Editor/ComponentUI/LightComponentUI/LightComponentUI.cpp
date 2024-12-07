
// You may need to build the project (run Qt uic code generator) to get "ui_LightComponentUI.h" resolved

#include "LightComponentUI.h"
#include "ui_LightComponentUI.h"


LightComponentUI::LightComponentUI(LightComponent data,QWidget *parent) :
        QWidget(parent), ui(new Ui::LightComponentUI), data(data)
{
    ui->setupUi(this);

    layout = new QVBoxLayout(this);
    this->lightTypeUI = new TypeSelector("Type");
    lightTypeUI->AddNewType("Directional");
    lightTypeUI->AddNewType("Point");
    lightTypeUI->AddNewType("Spot");

    this->colorUI = new Transform3D("Color",{1,1,1} );
    this->intensityUI = new ValueEdit("Intensity",0);
    this->rangeUI = new ValueEdit("Range",0 );
    this->nearUI = new ValueEdit("Near offset",1);
    this->farUI = new ValueEdit("Far offset",1000);
    this->outerCutoffUI = new ValueEdit("Outer CutOff",0);
    this->attenuationUI = new ValueEdit("Attenuation",0);
    layout->addWidget(this->lightTypeUI);
    layout->addWidget(this->colorUI);
    layout->addWidget(intensityUI);
    layout->addWidget(rangeUI);
    layout->addWidget(nearUI);
    layout->addWidget(farUI);
    layout->addWidget(outerCutoffUI);
    layout->addWidget(attenuationUI);
    this->setLayout(layout);

    InitSignal();
}

LightComponentUI::~LightComponentUI()
{
    delete ui;
}

void LightComponentUI::ChangeData(LightComponent data)
{
    this->data = data;
    auto c = data.color;
    colorUI->UpdateData({c.x,c.y,c.z});
    intensityUI->UpdateData(data.Intensity);
    rangeUI->UpdateData(data.range);
    nearUI->UpdateData(data.shadowCamera.near);
    farUI->UpdateData(data.shadowCamera.far);
    attenuationUI->UpdateData(data.attenuation);
    outerCutoffUI->UpdateData(data.outerCutoff);
    if(data.type==LightType::Directional)
    {
        ChangeToDirectionLight();
        lightTypeUI->UpdateData("Directional");

    } else if(data.type==LightType::Point)
    {
        ChangeToPointLight();
        lightTypeUI->UpdateData("Point");
    }else if(data.type == LightType::Spot)
    {
        ChangeToSpotLight();
        lightTypeUI->UpdateData("Spot");
    }
}

void LightComponentUI::InitSignal()
{
    for(auto* child:this->findChildren<QLineEdit*>())
    {
        connect(child,&QLineEdit::editingFinished,this,[&]()
        {

            emit LightCompChanged(UpdateALlData());
        });
    }

    connect(lightTypeUI->typer,&QComboBox::currentTextChanged,[&](QString text)
    {
        emit LightCompChanged(UpdateALlData());
    });

    connect(nearUI,&ValueEdit::DragEditFinished,[&]()
    {
        emit LightCompChanged(UpdateALlData());
    });
    connect(farUI,&ValueEdit::DragEditFinished,[&]()
    {
        emit LightCompChanged(UpdateALlData());
    });
}

LightComponent LightComponentUI::UpdateALlData()
{
    glm::vec3 color= {colorUI->xEdit->text().toFloat(),
                      colorUI->yEdit->text().toFloat(),
                      colorUI->zEdit->text().toFloat()};
    float intensity = intensityUI->edit->text().toFloat();
    float range = rangeUI->edit->text().toFloat();
    float attenuation = attenuationUI->edit->text().toFloat();
    float outerCutOff = outerCutoffUI->edit->text().toFloat();
    std::string type =lightTypeUI->typer->currentText().toStdString();
    ShadowCamera shadowCamera = {nearUI->edit->text().toFloat(),farUI->edit->text().toFloat()};

    LightType lightType;
    if(type=="Directional")
    {
        lightType=LightType::Directional;
        ChangeToDirectionLight();
    } else if(type=="Point")
    {
        lightType=LightType::Point;
        ChangeToPointLight();
    } else if(type=="Spot")
    {
        lightType = LightType::Spot;
        ChangeToSpotLight();
    }
    return {lightType,color,intensity,range,outerCutOff,attenuation,shadowCamera};
}

void LightComponentUI::ChangeToDirectionLight()
{
    ClearLayoutItems();
    //layout = new QVBoxLayout(this);
    layout->addWidget(this->lightTypeUI);
    layout->addWidget(this->colorUI);
    layout->addWidget(intensityUI);
    layout->addWidget(nearUI);
    layout->addWidget(farUI);
    //this->setLayout(layout);
}

void LightComponentUI::ChangeToPointLight()
{
    ClearLayoutItems();

    layout->addWidget(this->lightTypeUI);
    layout->addWidget(this->colorUI);
    layout->addWidget(intensityUI);
    layout->addWidget(rangeUI);
    layout->addWidget(attenuationUI);
    //this->setLayout(layout);
}

void LightComponentUI::ChangeToSpotLight()
{
    ClearLayoutItems();

    layout->addWidget(this->lightTypeUI);
    layout->addWidget(this->colorUI);
    layout->addWidget(intensityUI);
    layout->addWidget(attenuationUI);
    layout->addWidget(rangeUI);
    layout->addWidget(outerCutoffUI);
    //this->setLayout(layout);
}

void LightComponentUI::ClearLayoutItems()
{
    if (layout) {
        QLayoutItem *child;
        while ((child = layout->takeAt(0)) != nullptr) {
            if(child->widget()) {
                child->widget()->setParent(nullptr);//setParent(nullptr),防止删除之后界面不消失
            }
            delete child;
        }
    }
}

