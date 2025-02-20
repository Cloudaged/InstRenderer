/********************************************************************************
** Form generated from reading UI file 'MyToolPage.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MYTOOLPAGE_H
#define UI_MYTOOLPAGE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MyToolPage
{
public:

    void setupUi(QWidget *MyToolPage)
    {
        if (MyToolPage->objectName().isEmpty())
            MyToolPage->setObjectName("MyToolPage");
        MyToolPage->resize(400, 300);

        retranslateUi(MyToolPage);

        QMetaObject::connectSlotsByName(MyToolPage);
    } // setupUi

    void retranslateUi(QWidget *MyToolPage)
    {
        MyToolPage->setWindowTitle(QCoreApplication::translate("MyToolPage", "MyToolPage", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MyToolPage: public Ui_MyToolPage {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MYTOOLPAGE_H
