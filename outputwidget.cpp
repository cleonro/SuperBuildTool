#include "outputwidget.h"
#include "ui_outputwidget.h"

OutputWidget::OutputWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OutputWidget)
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
    ui->output->appendPlainText(text);
}
