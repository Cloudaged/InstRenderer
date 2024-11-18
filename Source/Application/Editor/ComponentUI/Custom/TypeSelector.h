
#ifndef INSTRENDERER_TYPESELECTOR_H
#define INSTRENDERER_TYPESELECTOR_H

#include <QWidget>
#include "QLabel"
#include "QComboBox"
#include "QHBoxLayout"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class TypeSelector;
}
QT_END_NAMESPACE

class TypeSelector : public QWidget
{
    Q_OBJECT

public:
    explicit TypeSelector(std::string name,QWidget *parent = nullptr);
    void AddNewType(std::string name);
    void UpdateData(std::string data);
    QLabel* tag;
    QComboBox* typer;

    std::string currentData;
    ~TypeSelector() override;

private:
    Ui::TypeSelector *ui;
};


#endif //INSTRENDERER_TYPESELECTOR_H
