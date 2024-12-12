/********************************************************************************
** Form generated from reading UI file 'Transform3D.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TRANSFORM3D_H
#define UI_TRANSFORM3D_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Transform3D
{
public:

    void setupUi(QWidget *Transform3D)
    {
        if (Transform3D->objectName().isEmpty())
            Transform3D->setObjectName("Transform3D");
        Transform3D->resize(400, 300);

        retranslateUi(Transform3D);

        QMetaObject::connectSlotsByName(Transform3D);
    } // setupUi

    void retranslateUi(QWidget *Transform3D)
    {
        Transform3D->setWindowTitle(QCoreApplication::translate("Transform3D", "Transform3D", nullptr));
    } // retranslateUi

};

namespace Ui {
    class Transform3D: public Ui_Transform3D {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TRANSFORM3D_H
