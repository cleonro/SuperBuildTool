#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>

//test-remove
#include "process.h"
#include <iostream>
//

static OutputWidget *output = nullptr;

void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
 {
    Q_UNUSED(context)
    if(output == nullptr)
    {
        return;
    }
    switch (type)
    {
    case QtDebugMsg:
        output->append(msg);
      break;
    case QtInfoMsg:
        output->append(msg);
      break;
    case QtWarningMsg:

      break;
    case QtCriticalMsg:

      break;
    case QtFatalMsg:

      break;
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    qInstallMessageHandler(myMessageOutput);
    ui->setupUi(this);
    ui->mainToolBar->hide();
    output = ui->output;
    connect(ui->action_Quit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->action_Open, &QAction::triggered, this, &MainWindow::onOpen);
    connect(&m_controller, &Controller::phaseStarted, this, &MainWindow::onPhaseStarted);
    connect(&m_controller, &Controller::phaseFinished, this, &MainWindow::onPhaseFinished);
    ui->projects->setController(&m_controller);
}

MainWindow::~MainWindow()
{
    delete ui;
    output = nullptr;
}

void MainWindow::on_checkout_clicked()
{
    m_controller.startPhase(ControllerPhase::Checkout);
}

void MainWindow::on_configure_clicked()
{
    m_controller.startPhase(ControllerPhase::Configure);
}

void MainWindow::on_build_clicked()
{
    m_controller.startPhase(ControllerPhase::Build);
}

void MainWindow::on_all_clicked()
{
    m_controller.startPhase(ControllerPhase::All);
}

void MainWindow::onOpen()
{
    QString filePath = QFileDialog::getOpenFileName(this, "Open Superbuild file",
                                        qApp->applicationDirPath(), "Xml files (*.xml)");
    if(!filePath.isNull())
    {
        m_controller.open(filePath);
    }
}

void MainWindow::onPhaseStarted(int phase)
{
    Q_UNUSED(phase)
    enableButtons(false);
}

void MainWindow::onPhaseFinished(int phase)
{
    Q_UNUSED(phase)
    enableButtons();
}

void MainWindow::enableButtons(bool enabled)
{
    ui->checkout->setEnabled(enabled);
    ui->configure->setEnabled(enabled);
    ui->build->setEnabled(enabled);
    ui->all->setEnabled(enabled);
    ui->clean->setEnabled(enabled);
    ui->projects->setEnabled(enabled);
}

void MainWindow::on_clean_clicked()
{
    m_controller.startClean();
}
