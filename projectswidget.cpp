#include "projectswidget.h"
#include "ui_projectswidget.h"

ProjectsWidget::ProjectsWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ProjectsWidget)
{
    ui->setupUi(this);
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

}
