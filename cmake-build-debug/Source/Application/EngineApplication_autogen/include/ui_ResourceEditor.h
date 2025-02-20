/********************************************************************************
** Form generated from reading UI file 'ResourceEditor.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RESOURCEEDITOR_H
#define UI_RESOURCEEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ResourceEditor
{
public:

    void setupUi(QWidget *ResourceEditor)
    {
        if (ResourceEditor->objectName().isEmpty())
            ResourceEditor->setObjectName("ResourceEditor");
        ResourceEditor->resize(400, 300);

        retranslateUi(ResourceEditor);

        QMetaObject::connectSlotsByName(ResourceEditor);
    } // setupUi

    void retranslateUi(QWidget *ResourceEditor)
    {
        ResourceEditor->setWindowTitle(QCoreApplication::translate("ResourceEditor", "ResourceEditor", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ResourceEditor: public Ui_ResourceEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RESOURCEEDITOR_H
