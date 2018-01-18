#include "process.h"
#include "project.h"

#include <QDir>
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

void Process::startProcess()
{
    if(m_project == nullptr)
    {
        return;
    }
    setupProcess();
    QProcess::start();
}

void Process::setupProcess()
{
    QString projectDir = m_project->projectDirectory();
    QString projectName = m_project->projectName();
    QString buildType = m_project->buildType();
    if(m_data.type == ProcessData::Checkout)
    {
        this->setProgram("git");
        QDir gitDir(projectDir + "/" + projectName + "/.git");
        if(gitDir.exists())
        {
            this->setWorkingDirectory(gitDir.absolutePath());
            QStringList args;
            args<< "clone" << "--progress" << m_data.repository << projectName << "--branch" << m_data.branch;
            this->setArguments(args);
        }
        else
        {
            this->setWorkingDirectory(projectDir + "/" + projectName);
            QStringList args;
            args<< "pull" << "--progress";
            this->setArguments(args);
        }
    }
    if(m_data.type == ProcessData::Configure)
    {
        this->setProgram("cmake");
        this->setWorkingDirectory(projectDir + "/build_" + buildType);
        QStringList args;
        //TODO - take into account other CMake generators
        args << "-G" << "\"Ninja\"";
        for(int i = 0; i < m_data.cmakeVariables.count(); ++i)
        {
            QString value = setupCMakeVariable(m_data.cmakeVariables[i]);
            args << "-D" << value;
        }

    }
    if(m_data.type == ProcessData::Build)
    {
        this->setProgram("ninja");
        this->setWorkingDirectory(projectDir + "/build_" + buildType);
    }
}

QString Process::setupCMakeVariable(const ProcessData::CMakeVariable &cmakeVariable)
{
    //types that need to be translated to CMake types
    //      PROJECTBUILDPATH, WORKINGDIRECTORYPATH, WORKINGDIRECTORYFILEPATH
    QString type;
    QString value;
    if(cmakeVariable.type == "PROJECTBUILDPATH")
    {
        type = "PATH";
        QDir dir(m_project->projectDirectory() + "../" + cmakeVariable.value + "/build_" + m_project->buildType());
        value = dir.absolutePath();
    }
    else if(cmakeVariable.type == "WORKINGDIRECTORYPATH")
    {
        type = "PATH";
        QDir dir(m_project->projectDirectory() + "../" + cmakeVariable.value);
        value = dir.absolutePath();
    }
    else if(cmakeVariable.type == "WORKINGDIRECTORYFILEPATH")
    {
        type = "FILEPATH";
        QDir dir(m_project->projectDirectory() + "../" + cmakeVariable.value);
        value = dir.absolutePath();
    }
    else
    {
        type = cmakeVariable.type;
        value = cmakeVariable.value;
    }
    QString r(cmakeVariable.name + ':');
    r += type;
    r += '=';
    r = r + '\"' + value + '\"';
    return r;
}
