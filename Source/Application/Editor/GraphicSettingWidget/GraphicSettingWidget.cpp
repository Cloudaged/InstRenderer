
// You may need to build the project (run Qt uic code generator) to get "ui_GraphicSettingWidget.h" resolved

#include "GraphicSettingWidget.h"
#include "ui_GraphicSettingWidget.h"


GraphicSettingWidget::GraphicSettingWidget(QDockWidget *parent) :
        QDockWidget(parent), ui(new Ui::GraphicSettingWidget)
{
    setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    InitToolBox();
    InitPages();

    for(auto* child:this->findChildren<ValueEdit*>())
    {
        connect(child->edit,&QLineEdit::editingFinished,this,[&]()
        {
            emit GraphicSettingUpdate(UpdateAllData());
        });

        connect(child,&ValueEdit::DragEditFinished,this,[&]()
        {
            emit GraphicSettingUpdate(UpdateAllData());
        });
    }

    for(auto* child:this->findChildren<TriggerBox*>())
    {
        connect(child->trigger,&QCheckBox::stateChanged,this,[&]()
        {
            emit GraphicSettingUpdate(UpdateAllData());
        });
    }

    connect(showCascade->typer,&QComboBox::currentTextChanged,[&](QString text)
    {
        emit GraphicSettingUpdate(UpdateAllData());
    });

    ui->setupUi(this);
}

GraphicSettingWidget::~GraphicSettingWidget()
{
    delete ui;
}

void GraphicSettingWidget::InitToolBox()
{
    scrollArea = new QScrollArea();
    toolBox = new MyToolBox(this);

    scrollArea->setWidget(toolBox);
    this->setWidget(scrollArea);
}

void GraphicSettingWidget::InitPages()
{
    ShadowSettings();
    toolBox->Reconstruct(this->pages);
}

void GraphicSettingWidget::ShadowSettings()
{
    QWidget* w = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(w);
    pcfEdit = new ValueEdit("PCF Sample Count",renderSettingData.shadowDebug.pcfSampleCount);
    bsEdit = new ValueEdit("Blocker Search Sample Count",renderSettingData.shadowDebug.blockerSearchCount);
    showCascade = new TypeSelector("Show Cascade");
    showCascade->AddNewType("Disable");
    showCascade->AddNewType("Color");
    showCascade->AddNewType("0");
    showCascade->AddNewType("1");
    showCascade->AddNewType("2");
    showCascade->AddNewType("3");
    antiShimmer = new TriggerBox("Anti Shimmering",renderSettingData.shadowDebug.antiShimmering);
    enablePCF = new TriggerBox("PCF",renderSettingData.shadowDebug.enablePCF);
    layout->addWidget(pcfEdit);
    layout->addWidget(bsEdit);
    layout->addWidget(showCascade);
    layout->addWidget(antiShimmer);
    layout->addWidget(enablePCF);
    w->setLayout(layout);
    pages.push_back({w,"Shadow"});
}

void GraphicSettingWidget::resizeEvent(QResizeEvent *event)
{
    toolBox->resize(event->size().width(),this->height());
    QWidget::resizeEvent(event);
}

RenderSettingUniform GraphicSettingWidget::UpdateAllData()
{
    this->renderSettingData.shadowDebug.pcfSampleCount = pcfEdit->edit->text().toFloat();
    this->renderSettingData.shadowDebug.blockerSearchCount = bsEdit->edit->text().toFloat();
    if(showCascade->typer->currentText()=="Color")
    {
        this->renderSettingData.shadowDebug.showCascade = -1;
    }else if(showCascade->typer->currentText()=="Disable")
    {
        this->renderSettingData.shadowDebug.showCascade=-2;
    }
    else
    {
        this->renderSettingData.shadowDebug.showCascade = showCascade->typer->currentText().toInt();
    }
    this->renderSettingData.shadowDebug.antiShimmering = antiShimmer->trigger->isChecked();
    this->renderSettingData.shadowDebug.enablePCF = enablePCF->trigger->isChecked();
    return this->renderSettingData;
}
