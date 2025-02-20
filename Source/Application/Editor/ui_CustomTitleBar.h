/********************************************************************************
** Form generated from reading UI file 'CustomTitleBar.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CUSTOMTITLEBAR_H
#define UI_CUSTOMTITLEBAR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CustomTitleBar
{
public:

    void setupUi(QWidget *CustomTitleBar)
    {
        if (CustomTitleBar->objectName().isEmpty())
            CustomTitleBar->setObjectName("CustomTitleBar");
        CustomTitleBar->resize(400, 300);

        retranslateUi(CustomTitleBar);

        QMetaObject::connectSlotsByName(CustomTitleBar);
    } // setupUi

    void retranslateUi(QWidget *CustomTitleBar)
    {
        CustomTitleBar->setWindowTitle(QCoreApplication::translate("CustomTitleBar", "CustomTitleBar", nullptr));
    } // retranslateUi

};

namespace Ui {
    class CustomTitleBar: public Ui_CustomTitleBar {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CUSTOMTITLEBAR_H
