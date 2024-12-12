/********************************************************************************
** Form generated from reading UI file 'TypeSelector.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TYPESELECTOR_H
#define UI_TYPESELECTOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TypeSelector
{
public:

    void setupUi(QWidget *TypeSelector)
    {
        if (TypeSelector->objectName().isEmpty())
            TypeSelector->setObjectName("TypeSelector");
        TypeSelector->resize(400, 300);

        retranslateUi(TypeSelector);

        QMetaObject::connectSlotsByName(TypeSelector);
    } // setupUi

    void retranslateUi(QWidget *TypeSelector)
    {
        TypeSelector->setWindowTitle(QCoreApplication::translate("TypeSelector", "TypeSelector", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TypeSelector: public Ui_TypeSelector {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TYPESELECTOR_H
