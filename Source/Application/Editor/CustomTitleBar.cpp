
// You may need to build the project (run Qt uic code generator) to get "ui_CustomTitleBar.h" resolved

#include "CustomTitleBar.h"
#include "ui_CustomTitleBar.h"
#include "MainEditor/MainEditor.h"

CustomTitleBar::CustomTitleBar(QWidget *parent) :
        QWidget(parent), ui(new Ui::CustomTitleBar)
{
    ui->setupUi(this);
    //setStyleSheet("background-color: Transparent; color: white; height: 30px;");
    InitTitleBar();
    InitMenuBar();
}

CustomTitleBar::~CustomTitleBar()
{
    delete ui;
}

void CustomTitleBar::InitMenuBar()
{
    mainLayout = new QVBoxLayout;
    menuBar = new QMenuBar();
    //File Menu
    fileMenu = new QMenu("File",menuBar);
    menuBar->addMenu(fileMenu);
    QAction* loadAction = new QAction("Load Resource",fileMenu);
    fileMenu->addAction(loadAction);
    connect(loadAction,&QAction::triggered,[&]()
    {
        auto filePath = QFileDialog::getOpenFileName(nullptr,"Load File",
                                                     QString::fromStdString(FILE_PATH("Asset")),
                                                     "所有文件(*.*)");
        /*if(!filePath.isEmpty())
        {
            emit LoadAction(filePath.toStdString());
        }*/

    });

    mainLayout->addLayout(titleLayout);
    mainLayout->addWidget(menuBar);
    this->setLayout(mainLayout);
}

void CustomTitleBar::InitTitleBar()
{
    titleLayout = new QHBoxLayout();
    titleLayout->setContentsMargins(5, 0, 5, 0);

    QLabel *title = new QLabel("Editor");
    QString buttonStyle = R"(
    QPushButton {
        border: none;
        background-color: transparent;
        color: white;
    }
    QPushButton:hover {
        background-color: rgba(255, 255, 255, 0.2);
    }
)";
    minimizeButton = new QPushButton("-");
    minimizeButton->setFixedSize(10, 20);
    minimizeButton->setStyleSheet("QPushButton {\n"
                                  "        border: none;\n"
                                  "        background-color: transparent;\n"
                                  "        color: white;\n"
                                  "         font-size: 25px;\n"
                                  "    }"
                                  " QPushButton:hover {\n"
                                  "        background-color: rgba(255, 255, 255, 0.2);\n"
                                  "    }");

    maximizeButton = new QPushButton("▢");
    maximizeButton->setFixedSize(10, 20);
    maximizeButton->setStyleSheet("QPushButton {\n"
                                  "        border: none;\n"
                                  "        background-color: transparent;\n"
                                  "        color: white;\n"
                                  "        font-size:25px;\n"
                                  "    }"
                                  " QPushButton:hover {\n"
                                  "        background-color: rgba(255, 255, 255, 0.2);\n"
                                  "    }");

// 关闭按钮
    closeButton = new QPushButton("x");
    closeButton->setFixedSize(10, 20);
    closeButton->setStyleSheet("QPushButton {\n"
                               "        border: none;\n"
                               "        background-color: transparent;\n"
                               "        color: white;\n"
                               "        font-size: 15px;\n"
                               "    }"
                               " QPushButton:hover {\n"
                               "        background-color: red;\n"
                               "    }");

    titleLayout->addWidget(title);
    titleLayout->addStretch();
    titleLayout->addWidget(minimizeButton);
    titleLayout->addWidget(maximizeButton);
    titleLayout->addWidget(closeButton);
    connect(minimizeButton, &QPushButton::clicked, this, [&]()
    {
        mainEditor->showMinimized();
    });
    connect(maximizeButton, &QPushButton::clicked, this, [&]()
    {
        if(isMax)
        {
            ChangeToMin();
        }else
        {
            ChangeToMax();
        }
    });
    connect(closeButton, &QPushButton::clicked, this, [&]()
    {
        mainEditor->close();
    });
}

void CustomTitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isDragging = true;
        dragStartPos = event->globalPosition().toPoint();
    }

    QWidget::mousePressEvent(event);
}

void CustomTitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (isDragging && mainEditor && !isMax)
    {
        QPoint diff = event->globalPosition().toPoint() - dragStartPos;
        mainEditor->move(mainEditor->pos() + diff);
        dragStartPos = event->globalPosition().toPoint();
    }

    QWidget::mouseMoveEvent(event);
}

void CustomTitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        isDragging = false;
    }
    QWidget::mouseReleaseEvent(event);
}

void CustomTitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (mainEditor)
    {
        if (isMax)
        {
            ChangeToMin();
        } else
        {
           ChangeToMax();
        }
    }
    QWidget::mouseDoubleClickEvent(event);
}

void CustomTitleBar::ChangeToMax()
{
    this->maximizeButton->setText("▫");
    mainEditor->showMaximized();
    isMax = !isMax;
}

void CustomTitleBar::ChangeToMin()
{
    this->maximizeButton->setText("▢");
    mainEditor->showNormal();
    isMax =!isMax;
}
