#include "process.h"
#include "project.h"

#include <QDebug>

////////////////////////////////////////////////////////////////////////////////

ProcessData::ProcessData()
    : type(None)
{

}

ProcessData::~ProcessData()
{

}

ProcessData::CMakeVariable::CMakeVariable()
{

}

ProcessData::CMakeVariable::~CMakeVariable()
{

}

////////////////////////////////////////////////////////////////////////////////


Process::Process(QObject *parent)
    : QProcess(parent)
{
    m_project = nullptr;
    connect(this, &QProcess::readyReadStandardOutput, this, &Process::onReadyReadStandardOutput);
    connect(this, &QProcess::readyReadStandardError, this, &Process::onReadyReadStandardError);
}

Process::~Process()
{

}

void Process::setProject(Project *project)
{
    m_project = project;
}

Project* Process::project()
{
    return m_project;
}

ProcessData& Process::processData()
{
    return m_data;
}

void Process::onReadyReadStandardOutput()
{
    QString outputText = readAllStandardOutput();
    qInfo() << outputText.toStdString().c_str();
}

void Process::onReadyReadStandardError()
{
    QByteArray errorText = readAllStandardError();
    qInfo() << errorText.toStdString().c_str();
}
