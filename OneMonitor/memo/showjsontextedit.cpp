#include "showjsontextedit.h"
#include <QTextEdit>
#include <QHBoxLayout>
#include <QDebug>
#include <QScrollBar>
#include <QFontMetrics>

showJsonTextEdit::showJsonTextEdit(QWidget* parent)
    : QWidget(parent)
{
    lineNumberWidget = new QTextEdit;
    lineNumberWidget->setStyleSheet("background-color: #2b366a;");
    lineNumberWidget->verticalScrollBar()->hide();
    lineNumberWidget->horizontalScrollBar()->hide();
    lineNumberWidget->insertPlainText(QStringLiteral("1\n"));
    lineNumberWidget->setFocusPolicy(Qt::NoFocus);
    lineNumberWidget->setContextMenuPolicy(Qt::NoContextMenu);
    jsonTextEdit = new QTextEdit;
    jsonTextEdit->setAcceptRichText(false);//禁用富文本
    jsonTextEdit->setLineWrapMode(QTextEdit::NoWrap);
    connect(jsonTextEdit->verticalScrollBar(), &QScrollBar::valueChanged, [&](int value)
    {
        lineNumberWidget->verticalScrollBar()->setValue(value);
    });
    QHBoxLayout* hb = new QHBoxLayout;
    hb->setMargin(0);
    hb->setSpacing(0);
    hb->addWidget(lineNumberWidget);
    hb->addWidget(jsonTextEdit);
    this->setLayout(hb);
    connect(jsonTextEdit, &QTextEdit::textChanged, this, &showJsonTextEdit::onTextChange);
    connect(jsonTextEdit, &QTextEdit::cursorPositionChanged, this, &showJsonTextEdit::onCursorPositionChanged);
    QFont font;
    font.setPixelSize(25);
    lineNumberWidget->setFont(font);
    jsonTextEdit->setFont(font);
    lineNumberWidget->setFixedWidth(lineNumberWidget->font().pixelSize() + 10);
    lastLine = 1;
    jsonTextEdit->setText("监控员工作事项:\n   1、上岗前要整理仪容，按规定着装，衣冠整洁，带好必要的上岗用品。 \n   2、不得携带通讯工具和与工作无关的物品。\n   3、接班之前应检查监控室设备运行情况。");
//    jsonTextEdit->setText("Work items of the monitor: \n"
//    "1. Tidy up your appearance, dress in accordance with the regulations, and take the necessary articles before going on duty. \n"
//    "2. Do not bring communication tools and articles unrelated to work. \n"
//    "3. Before taking over, check the operation of the equipment in the monitoring room.");
}

void showJsonTextEdit::onTextChange()
{
    int jsonTextEditRow = jsonTextEdit->document()->lineCount();
    if(jsonTextEditRow == lastLine)
    {
        return;
    }
    lineNumberWidget->blockSignals(true);
    jsonTextEdit->blockSignals(true);
    //方法2
    lineNumberWidget->clear();
    QString str;
    ++jsonTextEditRow;
    for (int i = 1; i < jsonTextEditRow; ++i)
    {
        str.append(QString("%1\n").arg(i));
    }
    lineNumberWidget->setPlainText(str);
    //
    lastLine = jsonTextEdit->document()->lineCount();
    lineNumberWidget->verticalScrollBar()->setValue(jsonTextEdit->verticalScrollBar()->value());
    lineNumberWidget->blockSignals(false);
    jsonTextEdit->blockSignals(false);
}

void showJsonTextEdit::onCursorPositionChanged()
{
    lineNumberWidget->blockSignals(true);
    jsonTextEdit->blockSignals(true);
    lineNumberWidget->verticalScrollBar()->setValue(jsonTextEdit->verticalScrollBar()->value());
    lineNumberWidget->blockSignals(false);
    jsonTextEdit->blockSignals(false);
}

void showJsonTextEdit::setPlaceholderText(QString str)
{}
