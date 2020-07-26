#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QTime>
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

    //test
    QStringList env = QProcess::systemEnvironment();
    foreach (QString str, env)
    {
        if(!str.startsWith("CMAKE_PREFIX_PATH"))
        {
            continue;
        }
        qInfo() << str.toStdString().c_str();
    }
    //

    connect(&m_timer, &QTimer::timeout, this, &MainWindow::onTimer);
    m_timer.setInterval(50);
}

MainWindow::~MainWindow()
{
    delete ui;
    output = nullptr;
}

void MainWindow::on_checkout_clicked()
{
//    m_controller.startPhase(ControllerPhase::Checkout);
    bool checked = ui->checkout->checkState() == Qt::Checked;

    bool all_checked = checked && ui->configure->checkState() == Qt::Checked && ui->build->checkState() == Qt::Checked;
    bool any_checked = checked || ui->configure->checkState() == Qt::Checked || ui->build->checkState() == Qt::Checked;
    ui->all->setChecked(all_checked);
    ui->start->setEnabled(any_checked);

    m_controller.selectPhase(ControllerPhase::Checkout, checked);
}

void MainWindow::on_configure_clicked()
{
//    m_controller.startPhase(ControllerPhase::Configure);
    bool checked = ui->configure->checkState() == Qt::Checked;

    bool all_checked = ui->checkout->checkState() == Qt::Checked && checked && ui->build->checkState() == Qt::Checked;
    bool any_checked = ui->checkout->checkState() == Qt::Checked || checked || ui->build->checkState() == Qt::Checked;
    ui->all->setChecked(all_checked);
    ui->start->setEnabled(any_checked);

    m_controller.selectPhase(ControllerPhase::Configure, checked);
}

void MainWindow::on_build_clicked()
{
//    m_controller.startPhase(ControllerPhase::Build);
    bool checked = ui->build->checkState() == Qt::Checked;

    bool all_checked = ui->checkout->checkState() == Qt::Checked && ui->configure->checkState() == Qt::Checked && checked;
    bool any_checked = ui->checkout->checkState() == Qt::Checked || ui->configure->checkState() == Qt::Checked || checked;
    ui->all->setChecked(all_checked);
    ui->start->setEnabled(any_checked);

    m_controller.selectPhase(ControllerPhase::Build, checked);
}

void MainWindow::on_all_clicked()
{
//    m_controller.startPhase(ControllerPhase::All);
    bool checked = ui->all->checkState() == Qt::Checked;

    {
        ui->checkout->setChecked(checked);
        ui->configure->setChecked(checked);
        ui->build->setChecked(checked);

        ui->start->setEnabled(checked);
    }

    {
        m_controller.selectPhase(ControllerPhase::Checkout, checked);
        m_controller.selectPhase(ControllerPhase::Configure, checked);
        m_controller.selectPhase(ControllerPhase::Build, checked);
    }
}

void MainWindow::on_start_clicked()
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
        ui->time->setText("");
    }
}

void MainWindow::onPhaseStarted(ControllerPhase phase)
{
    Q_UNUSED(phase)
    enableButtons(false);
    ui->time->setText("");
    m_elapsedTime.restart();
    m_timer.start();
}

void MainWindow::onPhaseFinished(ControllerPhase phase)
{
    Q_UNUSED(phase)
    enableButtons();
    m_timer.stop();
}

void MainWindow::enableButtons(bool enabled)
{
    ui->checkout->setEnabled(enabled);
    ui->configure->setEnabled(enabled);
    ui->build->setEnabled(enabled);
    ui->all->setEnabled(enabled);
    ui->clean->setEnabled(enabled);
    ui->eraseBuild->setEnabled(enabled);
    ui->projects->setEnabled(enabled);

    ui->start->setEnabled(enabled);
}

void MainWindow::on_clean_clicked()
{
    m_controller.startClean();
}

void MainWindow::on_eraseBuild_clicked()
{
    m_controller.eraseBuild();
}

void MainWindow::onTimer()
{
    qint64 milliseconds = m_elapsedTime.elapsed();
    QString elapsedTime = QTime::fromMSecsSinceStartOfDay(milliseconds).toString("hh:mm:ss.z");
    ui->time->setText(elapsedTime);
}
