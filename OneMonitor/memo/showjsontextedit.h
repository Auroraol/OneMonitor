#ifndef SHOWJSONTEXTEDIT_H
#define SHOWJSONTEXTEDIT_H

#include <QTextEdit>

class showJsonTextEdit : public QWidget
{
    Q_OBJECT
public:
    showJsonTextEdit(QWidget* parent = nullptr);
    void setPlaceholderText(QString str);
private:
    class QTextEdit* lineNumberWidget;
    QTextEdit* jsonTextEdit;
    int lastLine;//上一次的行

    void onTextChange();
    void onCursorPositionChanged();

};

#endif // SHOWJSONTEXTEDIT_H
