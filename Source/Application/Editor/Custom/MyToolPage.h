
#ifndef INSTRENDERER_MYTOOLPAGE_H
#define INSTRENDERER_MYTOOLPAGE_H

#include <QWidget>
#include "QLabel"
#include "QFormLayout"
#include "QVBoxLayout"
#include "QFile"
#include "QPushButton"
#include "iostream"
#include "bitset"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MyToolPage;
}
QT_END_NAMESPACE




class  MyToolPage : public QWidget
{
Q_OBJECT

public:
    explicit MyToolPage(QWidget* content,std::string name,QWidget *parent = nullptr);

    ~MyToolPage() override;

    QWidget* contentWidget;
    QObject* contentParent;
    QPushButton* pushButton;

private:
    Ui::MyToolPage *ui;
    bool isExpanded = true;
    //QLabel* label;

    QVBoxLayout* layout;
public slots:
    void AddWidget(const QString &title, QWidget* widget);
    void Expand();
    void Fold();
private slots:
    void OnPushButtonFoldClicked();
};


#endif //INSTRENDERER_MYTOOLPAGE_H
