#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>

//test-remove
#include <QProcess>
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

    qDebug() << "SuperBuildTool started!";
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_checkout_clicked()
{

}
