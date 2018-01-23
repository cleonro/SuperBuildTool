#include "outputwidget.h"
#include "ui_outputwidget.h"

OutputWidget::OutputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputWidget),
    m_cr(false)
{
    ui->setupUi(this);
}

OutputWidget::~OutputWidget()
{
    delete ui;
}

void OutputWidget::on_clear_clicked()
{
    ui->output->clear();
}

void OutputWidget::append(const QString &text)
{
    QStringList tokens = text.split("\r\n", QString::SkipEmptyParts);
    int count = tokens.count();
    for(int i = 0; i < count; ++i)
    {
        this->append2(tokens[i]);
    }
}

void OutputWidget::append2(const QString &text)
{
    int crpos = text.indexOf('\r');
    if(crpos == -1)
    {
        insertText(text);
        m_cr = false;
        return;
    }
    int pos = 0;
    m_cr = m_cr || (text.at(pos) == '\r');
    while(crpos != -1)
    {
        QString t = text.mid(pos, crpos - pos);
        insertText(t);
        m_cr = true;
        pos = crpos + 1;
        crpos = text.indexOf('\r', pos);
    }
    if(pos < text.count())
    {
        QString t = text.mid(pos);
        insertText(t);
        m_cr = false;
    }
}

void OutputWidget::insertText(const QString &text)
{
    if(m_cr)
    {
        deleteCurrentLine();
    }
    QStringList tokens = text.split('\n', QString::SkipEmptyParts);
    int count = tokens.count();
    for(int i = 0; i < count; ++i)
    {
        ui->output->appendPlainText(tokens[i]);
    }
}

void OutputWidget::deleteCurrentLine()
{
    //    QTextCursor tc = ui->output->textCursor();
    //    tc.movePosition(QTextCursor::End);
    //    tc.select(QTextCursor::LineUnderCursor);
    //    tc.removeSelectedText();
    //    tc.movePosition(QTextCursor::StartOfLine);
    //    ui->output->setTextCursor(tc);

    //ui->output->setFocus();
    QTextCursor storeCursorPos = ui->output->textCursor();
    ui->output->moveCursor(QTextCursor::End, QTextCursor::MoveAnchor);
    ui->output->moveCursor(QTextCursor::StartOfLine, QTextCursor::MoveAnchor);
    ui->output->moveCursor(QTextCursor::End, QTextCursor::KeepAnchor);
    ui->output->textCursor().removeSelectedText();
    ui->output->textCursor().deletePreviousChar();
    //ui->output->setTextCursor(storeCursorPos);
}
