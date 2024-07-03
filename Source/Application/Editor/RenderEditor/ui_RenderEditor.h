/********************************************************************************
** Form generated from reading UI file 'RenderEditor.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RENDEREDITOR_H
#define UI_RENDEREDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_RenderEditor
{
public:

    void setupUi(QWidget *RenderEditor)
    {
        if (RenderEditor->objectName().isEmpty())
            RenderEditor->setObjectName("RenderEditor");
        RenderEditor->resize(400, 300);

        retranslateUi(RenderEditor);

        QMetaObject::connectSlotsByName(RenderEditor);
    } // setupUi

    void retranslateUi(QWidget *RenderEditor)
    {
        RenderEditor->setWindowTitle(QCoreApplication::translate("RenderEditor", "RenderEditor", nullptr));
    } // retranslateUi

};

namespace Ui {
    class RenderEditor: public Ui_RenderEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RENDEREDITOR_H
