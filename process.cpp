#include "process.h"

#include <QDebug>

Process::Process(QObject *parent)
    : QProcess(parent)
{
    connect(this, &QProcess::readyReadStandardOutput, this, &Process::onReadyReadStandardOutput);
    connect(this, &QProcess::readyReadStandardError, this, &Process::onReadyReadStandardError);
}

Process::~Process()
{

}

void Process::onReadyReadStandardOutput()
{
    QString outputText = readAllStandardOutput();

//    int count = outputText.count() - 1;
//    if(count > 0 && outputText.at(count) == '\"')
//    {
//        outputText.remove(count, 1);
//        if(outputText.at(0) == '\"')
//        {
//            outputText.remove(0, 1);
//        }
//    }

    qInfo() << outputText;
}

void Process::onReadyReadStandardError()
{
    QByteArray errorText = readAllStandardError();

//    int count = errorText.count() - 1;
//    if(count > 0 && errorText.at(count) == '\"')
//    {
//        errorText.remove(count, 1);
//        if(errorText.at(0) == '\"')
//        {
//            errorText.remove(0, 1);
//        }
//    }

    qInfo() << errorText.toStdString().c_str();
}
