#ifndef PROJECTSWIDGET_H
#define PROJECTSWIDGET_H

#include <QWidget>

namespace Ui {
class ProjectsWidget;
}

class ProjectsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectsWidget(QWidget *parent = 0);
    ~ProjectsWidget();

private slots:
    void on_workingDirectoryButton_clicked();

    void on_buildType_currentTextChanged(const QString &arg1);

private:
    Ui::ProjectsWidget *ui;
};

#endif // PROJECTSWIDGET_H
