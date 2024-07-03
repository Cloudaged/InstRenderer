/********************************************************************************
** Form generated from reading UI file 'ComponentEditor.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_COMPONENTEDITOR_H
#define UI_COMPONENTEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ComponentEditor
{
public:

    void setupUi(QWidget *ComponentEditor)
    {
        if (ComponentEditor->objectName().isEmpty())
            ComponentEditor->setObjectName("ComponentEditor");
        ComponentEditor->resize(400, 300);

        retranslateUi(ComponentEditor);

        QMetaObject::connectSlotsByName(ComponentEditor);
    } // setupUi

    void retranslateUi(QWidget *ComponentEditor)
    {
        ComponentEditor->setWindowTitle(QCoreApplication::translate("ComponentEditor", "ComponentEditor", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ComponentEditor: public Ui_ComponentEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_COMPONENTEDITOR_H
