#ifndef OUTPUTWIDGET_H
#define OUTPUTWIDGET_H

#include <QWidget>

namespace Ui {
class OutputWidget;
}

class OutputWidget : public QWidget
{
    Q_OBJECT

public:
    explicit OutputWidget(QWidget *parent = 0);
    ~OutputWidget();

public:
    void append(const QString &text);

private slots:
    void on_clear_clicked();

private:
    void append2(const QString &text);
    void deleteCurrentLine();
    void insertText(const QString &text);

private:
    Ui::OutputWidget *ui;
    bool m_cr;
};

#endif // OUTPUTWIDGET_H
