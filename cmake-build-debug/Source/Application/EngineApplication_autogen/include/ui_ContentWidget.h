/********************************************************************************
** Form generated from reading UI file 'ContentWidget.ui'
**
** Created by: Qt User Interface Compiler version 6.5.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONTENTWIDGET_H
#define UI_CONTENTWIDGET_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ContentWidget
{
public:

    void setupUi(QWidget *ContentWidget)
    {
        if (ContentWidget->objectName().isEmpty())
            ContentWidget->setObjectName("ContentWidget");
        ContentWidget->resize(400, 300);

        retranslateUi(ContentWidget);

        QMetaObject::connectSlotsByName(ContentWidget);
    } // setupUi

    void retranslateUi(QWidget *ContentWidget)
    {
        ContentWidget->setWindowTitle(QCoreApplication::translate("ContentWidget", "ContentWidget", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ContentWidget: public Ui_ContentWidget {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONTENTWIDGET_H
