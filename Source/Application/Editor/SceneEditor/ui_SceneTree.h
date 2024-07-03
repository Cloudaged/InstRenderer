/********************************************************************************
** Form generated from reading UI file 'SceneTree.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SCENETREE_H
#define UI_SCENETREE_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SceneTree
{
public:

    void setupUi(QWidget *SceneTree)
    {
        if (SceneTree->objectName().isEmpty())
            SceneTree->setObjectName("SceneTree");
        SceneTree->resize(400, 300);

        retranslateUi(SceneTree);

        QMetaObject::connectSlotsByName(SceneTree);
    } // setupUi

    void retranslateUi(QWidget *SceneTree)
    {
        SceneTree->setWindowTitle(QCoreApplication::translate("SceneTree", "SceneTree", nullptr));
    } // retranslateUi

};

namespace Ui {
    class SceneTree: public Ui_SceneTree {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SCENETREE_H
