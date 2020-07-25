#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "controller.h"

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_checkout_clicked();

    void on_configure_clicked();

    void on_build_clicked();

    void onOpen();

    void onPhaseStarted(ControllerPhase phase);
    void onPhaseFinished(ControllerPhase phase);

    void on_all_clicked();

    void on_clean_clicked();

    void on_eraseBuild_clicked();

private:
    void enableButtons(bool enabled = true);

private:
    Ui::MainWindow *ui;
    Controller m_controller;
};

#endif // MAINWINDOW_H
