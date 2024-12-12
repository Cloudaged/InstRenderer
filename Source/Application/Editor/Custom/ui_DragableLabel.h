/********************************************************************************
** Form generated from reading UI file 'DragableLabel.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DRAGABLELABEL_H
#define UI_DRAGABLELABEL_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DragableLabel
{
public:

    void setupUi(QWidget *DragableLabel)
    {
        if (DragableLabel->objectName().isEmpty())
            DragableLabel->setObjectName("DragableLabel");
        DragableLabel->resize(400, 300);

        retranslateUi(DragableLabel);

        QMetaObject::connectSlotsByName(DragableLabel);
    } // setupUi

    void retranslateUi(QWidget *DragableLabel)
    {
        DragableLabel->setWindowTitle(QCoreApplication::translate("DragableLabel", "DragableLabel", nullptr));
    } // retranslateUi

};

namespace Ui {
    class DragableLabel: public Ui_DragableLabel {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DRAGABLELABEL_H
