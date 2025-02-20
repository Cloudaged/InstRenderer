/********************************************************************************
** Form generated from reading UI file 'LightComponentUI.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_LIGHTCOMPONENTUI_H
#define UI_LIGHTCOMPONENTUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_LightComponentUI
{
public:

    void setupUi(QWidget *LightComponentUI)
    {
        if (LightComponentUI->objectName().isEmpty())
            LightComponentUI->setObjectName("LightComponentUI");
        LightComponentUI->resize(400, 300);

        retranslateUi(LightComponentUI);

        QMetaObject::connectSlotsByName(LightComponentUI);
    } // setupUi

    void retranslateUi(QWidget *LightComponentUI)
    {
        LightComponentUI->setWindowTitle(QCoreApplication::translate("LightComponentUI", "LightComponentUI", nullptr));
    } // retranslateUi

};

namespace Ui {
    class LightComponentUI: public Ui_LightComponentUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_LIGHTCOMPONENTUI_H
