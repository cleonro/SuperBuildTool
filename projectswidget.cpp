#include "projectswidget.h"
#include "ui_projectswidget.h"
#include "controller.h"

#include <QStandardItemModel>

ProjectsWidget::ProjectsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectsWidget)
{
    ui->setupUi(this);
    ui->projects->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_controller = nullptr;
}

ProjectsWidget::~ProjectsWidget()
{
    delete ui;
}

void ProjectsWidget::on_workingDirectoryButton_clicked()
{

}

void ProjectsWidget::on_buildType_currentTextChanged(const QString &arg1)
{
    if(m_controller == nullptr)
    {
        return;
    }
    Parser *parser = m_controller->parser();
    parser->setBuildType(arg1);
}

void ProjectsWidget::setController(Controller *controller)
{
    m_controller = controller;
    if(m_controller != nullptr)
    {
        QStandardItemModel *model = m_controller->standardItemModel();
        ui->projects->setModel(model);
        Parser *parser = m_controller->parser();
        connect(parser, &Parser::parsingStarted, this, &ProjectsWidget::onParsingStarted);
        connect(parser, &Parser::parsingFinished, this, &ProjectsWidget::onParsingFinished);
    }
}

void ProjectsWidget::onParsingStarted(QString filePath)
{
    ui->buildType->setCurrentText("Debug");
    ui->workingDirectory->setText("");
    ui->filePath->setText(filePath);
}

void ProjectsWidget::onParsingFinished(bool result)
{
    if(!result)
    {
        return;
    }
    Parser *parser = m_controller->parser();
    ui->buildType->setCurrentText(parser->buildType());
    ui->workingDirectory->setText(parser->workingDirectory());
}
