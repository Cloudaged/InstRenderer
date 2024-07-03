
#ifndef INSTRENDERER_CONSOLEEDITOR_H
#define INSTRENDERER_CONSOLEEDITOR_H

#include <QWidget>
#include "QDockWidget"
#include "QPlainTextEdit"
QT_BEGIN_NAMESPACE
namespace Ui
{
    class ConsoleEditor;
}
QT_END_NAMESPACE

class  ConsoleEditor : public QDockWidget
{
Q_OBJECT

public:
    explicit ConsoleEditor(QDockWidget *parent = nullptr);
    void AddMessage();
    ~ConsoleEditor() override;

private:
    void InitTextWidget();
    QPlainTextEdit* textWidget;
    Ui::ConsoleEditor *ui;
};


#endif //INSTRENDERER_CONSOLEEDITOR_H
