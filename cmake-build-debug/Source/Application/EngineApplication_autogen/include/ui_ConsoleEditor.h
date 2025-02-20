/********************************************************************************
** Form generated from reading UI file 'ConsoleEditor.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONSOLEEDITOR_H
#define UI_CONSOLEEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConsoleEditor
{
public:

    void setupUi(QWidget *ConsoleEditor)
    {
        if (ConsoleEditor->objectName().isEmpty())
            ConsoleEditor->setObjectName("ConsoleEditor");
        ConsoleEditor->resize(400, 300);

        retranslateUi(ConsoleEditor);

        QMetaObject::connectSlotsByName(ConsoleEditor);
    } // setupUi

    void retranslateUi(QWidget *ConsoleEditor)
    {
        ConsoleEditor->setWindowTitle(QCoreApplication::translate("ConsoleEditor", "ConsoleEditor", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConsoleEditor: public Ui_ConsoleEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONSOLEEDITOR_H
