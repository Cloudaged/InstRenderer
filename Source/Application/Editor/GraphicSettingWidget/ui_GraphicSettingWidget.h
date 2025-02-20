/********************************************************************************
** Form generated from reading UI file 'GraphicSettingWidget.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_GRAPHICSETTINGWIDGET_H
#define UI_GRAPHICSETTINGWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_GraphicSettingWidget
{
public:

    void setupUi(QWidget *GraphicSettingWidget)
    {
        if (GraphicSettingWidget->objectName().isEmpty())
            GraphicSettingWidget->setObjectName("GraphicSettingWidget");
        GraphicSettingWidget->resize(400, 300);

        retranslateUi(GraphicSettingWidget);

        QMetaObject::connectSlotsByName(GraphicSettingWidget);
    } // setupUi

    void retranslateUi(QWidget *GraphicSettingWidget)
    {
        GraphicSettingWidget->setWindowTitle(QCoreApplication::translate("GraphicSettingWidget", "GraphicSettingWidget", nullptr));
    } // retranslateUi

};

namespace Ui {
    class GraphicSettingWidget: public Ui_GraphicSettingWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_GRAPHICSETTINGWIDGET_H
