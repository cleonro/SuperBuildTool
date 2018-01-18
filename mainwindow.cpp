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
    output = ui->output;
    connect(ui->action_Quit, &QAction::triggered, qApp, &QApplication::quit);
    connect(ui->action_Open, &QAction::triggered, this, &MainWindow::onOpen);

    qInfo() << "SuperBuildTool started!";
}

MainWindow::~MainWindow()
{
    delete ui;
    output = nullptr;
}

void MainWindow::on_checkout_clicked()
{
//    static int c = 0;
//    qInfo() << "Checkout started..." << c++;

    qInfo() << "Checkout started...";
    Process *p = new Process(this);
    p->setProgram("git");
    QStringList args;
    args << "clone" << "--progress" << "git://vtk.org/VTK.git" << "VTKd" << "--branch" << "release";
    p->setArguments(args);
    p->start();
}

void MainWindow::on_configure_clicked()
{
    static int c = 0;
    qInfo() << "Configure started..." << c++;
}

void MainWindow::on_build_clicked()
{
    static int c = 0;
    qInfo() << "Build started..." << c++;
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
