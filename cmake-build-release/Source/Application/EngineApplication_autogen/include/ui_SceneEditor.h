/********************************************************************************
** Form generated from reading UI file 'SceneEditor.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCENEEDITOR_H
#define UI_SCENEEDITOR_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SceneEditor
{
public:

    void setupUi(QWidget *SceneEditor)
    {
        if (SceneEditor->objectName().isEmpty())
            SceneEditor->setObjectName("SceneEditor");
        SceneEditor->resize(400, 300);

        retranslateUi(SceneEditor);

        QMetaObject::connectSlotsByName(SceneEditor);
    } // setupUi

    void retranslateUi(QWidget *SceneEditor)
    {
        SceneEditor->setWindowTitle(QCoreApplication::translate("SceneEditor", "SceneEditor", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SceneEditor: public Ui_SceneEditor {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCENEEDITOR_H
