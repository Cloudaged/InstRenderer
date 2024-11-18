
// You may need to build the project (run Qt uic code generator) to get "ui_ComponentEditor.h" resolved

#include "ComponentEditor.h"
#include "ui_ComponentEditor.h"


ComponentEditor::ComponentEditor(QDockWidget *parent) :
        QDockWidget(parent), ui(new Ui::ComponentEditor)
{
    ui->setupUi(this);
    InitToolBox();
    transformCompUI = new TransformComponentUI({});
   // transformCompUI->hide();
}

ComponentEditor::~ComponentEditor()
{
    delete ui;
}

void ComponentEditor::InitToolBox()
{
    scrollArea = new QScrollArea();
    toolBox = new MyToolBox(this);

    scrollArea->setWidget(toolBox);
    this->setWidget(scrollArea);
}

void ComponentEditor::resizeEvent(QResizeEvent *event)
{
    toolBox->resize(event->size().width(),this->height());
    QWidget::resizeEvent(event);
}

void ComponentEditor::ChangeGameObject(GameObject *gameObject, entt::registry *reg)
{
    if(toolBox)
        delete toolBox;
    toolBox = new MyToolBox(this);
    toolBox->resize(this->width(), this->height());
    std::vector<QWidget*> contents;

    curID = gameObject->entityID;
    auto& bits = gameObject->componentBits;

    if(bits.test(static_cast<size_t>(ComponentType::Transform)))
    {
        //Transform
        auto& transformComp = reg->get<Transform>(gameObject->entityID);

        transformCompUI->ChangeData(transformComp);

        contents.push_back(transformCompUI);
    }
    if(bits.test(static_cast<size_t>(ComponentType::Renderable)))
    {
        //Renderable

    }

    toolBox->Reconstruct(contents);
    scrollArea->setWidget(toolBox);
}
