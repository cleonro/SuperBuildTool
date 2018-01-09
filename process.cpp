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
    qint64 pos = this->pos();
    QString outputText = readAllStandardOutput().toStdString().c_str();
    qInfo() << "standard txt at pos[" << pos << "]: " << outputText;
}

void Process::onReadyReadStandardError()
{
    qint64 pos = this->pos();
    QString errorText = readAllStandardError().toStdString().c_str();
    qInfo() << "error txt at pos[" << pos << "]: " << errorText;
}
