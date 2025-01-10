/********************************************************************************
** Form generated from reading UI file 'TriggerBox.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRIGGERBOX_H
#define UI_TRIGGERBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TriggerBox
{
public:

    void setupUi(QWidget *TriggerBox)
    {
        if (TriggerBox->objectName().isEmpty())
            TriggerBox->setObjectName("TriggerBox");
        TriggerBox->resize(400, 300);

        retranslateUi(TriggerBox);

        QMetaObject::connectSlotsByName(TriggerBox);
    } // setupUi

    void retranslateUi(QWidget *TriggerBox)
    {
        TriggerBox->setWindowTitle(QCoreApplication::translate("TriggerBox", "TriggerBox", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TriggerBox: public Ui_TriggerBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRIGGERBOX_H
