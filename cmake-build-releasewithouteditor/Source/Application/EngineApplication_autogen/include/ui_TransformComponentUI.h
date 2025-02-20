/********************************************************************************
** Form generated from reading UI file 'TransformComponentUI.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRANSFORMCOMPONENTUI_H
#define UI_TRANSFORMCOMPONENTUI_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_TransformComponentUI
{
public:

    void setupUi(QWidget *TransformComponentUI)
    {
        if (TransformComponentUI->objectName().isEmpty())
            TransformComponentUI->setObjectName("TransformComponentUI");
        TransformComponentUI->resize(400, 300);

        retranslateUi(TransformComponentUI);

        QMetaObject::connectSlotsByName(TransformComponentUI);
    } // setupUi

    void retranslateUi(QWidget *TransformComponentUI)
    {
        TransformComponentUI->setWindowTitle(QCoreApplication::translate("TransformComponentUI", "TransformComponentUI", nullptr));
    } // retranslateUi

};

namespace Ui {
    class TransformComponentUI: public Ui_TransformComponentUI {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRANSFORMCOMPONENTUI_H
