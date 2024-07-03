
#ifndef INSTRENDERER_MYTOOLBOX_H
#define INSTRENDERER_MYTOOLBOX_H

#include <QWidget>
#include "MyToolPage.h"
#include "QScrollArea"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MyToolBox;
}
QT_END_NAMESPACE

class  MyToolBox : public QWidget
{
Q_OBJECT

public:
    explicit MyToolBox(QWidget *parent = nullptr);

    ~MyToolBox() override;

private:

    Ui::MyToolBox *ui;
};


#endif //INSTRENDERER_MYTOOLBOX_H
