#ifndef PROJECTSWIDGET_H
#define PROJECTSWIDGET_H

#include <QWidget>

namespace Ui {
class ProjectsWidget;
}

class Controller;

class ProjectsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ProjectsWidget(QWidget *parent = 0);
    ~ProjectsWidget();

    void setController(Controller *controller);

private slots:
    void on_workingDirectoryButton_clicked();

    void on_buildType_currentTextChanged(const QString &arg1);

    void onParsingStarted(QString filePath);
    void onParsingFinished(bool result);

private:
    Ui::ProjectsWidget *ui;
    Controller *m_controller;
};

#endif // PROJECTSWIDGET_H
