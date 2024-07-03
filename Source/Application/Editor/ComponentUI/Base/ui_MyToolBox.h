/********************************************************************************
** Form generated from reading UI file 'MyToolBox.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYTOOLBOX_H
#define UI_MYTOOLBOX_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyToolBox
{
public:

    void setupUi(QWidget *MyToolBox)
    {
        if (MyToolBox->objectName().isEmpty())
            MyToolBox->setObjectName("MyToolBox");
        MyToolBox->resize(400, 300);

        retranslateUi(MyToolBox);

        QMetaObject::connectSlotsByName(MyToolBox);
    } // setupUi

    void retranslateUi(QWidget *MyToolBox)
    {
        MyToolBox->setWindowTitle(QCoreApplication::translate("MyToolBox", "MyToolBox", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MyToolBox: public Ui_MyToolBox {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYTOOLBOX_H
