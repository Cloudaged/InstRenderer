/********************************************************************************
** Form generated from reading UI file 'ValueEdit.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VALUEEDIT_H
#define UI_VALUEEDIT_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ValueEdit
{
public:

    void setupUi(QWidget *ValueEdit)
    {
        if (ValueEdit->objectName().isEmpty())
            ValueEdit->setObjectName("ValueEdit");
        ValueEdit->resize(400, 300);

        retranslateUi(ValueEdit);

        QMetaObject::connectSlotsByName(ValueEdit);
    } // setupUi

    void retranslateUi(QWidget *ValueEdit)
    {
        ValueEdit->setWindowTitle(QCoreApplication::translate("ValueEdit", "ValueEdit", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ValueEdit: public Ui_ValueEdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VALUEEDIT_H
