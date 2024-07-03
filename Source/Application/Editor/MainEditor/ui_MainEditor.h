/********************************************************************************
** Form generated from reading UI file 'MainEditor.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINEDITOR_H
#define UI_MAINEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainEditor
{
public:
    QWidget *centralwidget;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainEditor)
    {
        if (MainEditor->objectName().isEmpty())
            MainEditor->setObjectName("MainEditor");
        MainEditor->resize(400, 300);
        centralwidget = new QWidget(MainEditor);
        centralwidget->setObjectName("centralwidget");
        MainEditor->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainEditor);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 400, 17));
        MainEditor->setMenuBar(menubar);
        statusbar = new QStatusBar(MainEditor);
        statusbar->setObjectName("statusbar");
        MainEditor->setStatusBar(statusbar);

        retranslateUi(MainEditor);

        QMetaObject::connectSlotsByName(MainEditor);
    } // setupUi

    void retranslateUi(QMainWindow *MainEditor)
    {
        MainEditor->setWindowTitle(QCoreApplication::translate("MainEditor", "MainEditor", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainEditor: public Ui_MainEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINEDITOR_H
