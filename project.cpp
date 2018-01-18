#include "project.h"

#include <QDir>

Project::Project(QObject *parent)
    : QObject(parent)
{
    m_isActive = true;
}

Project::~Project()
{

}

QString Project::projectName()
{
    return m_projectName;
}

QString Project::projectDirectory()
{
    return m_projectDirectory;
}

QString Project::buildType()
{
    return m_buildType;
}

void Project::setActive(const bool &active)
{
    m_isActive = active;
}

bool Project::isActive()
{
    return m_isActive;
}

void Project::setProjectStructure(const QString &projectName, const QString &workingDirectory, const QString &buildType)
{
    QDir workDir(workingDirectory);
    QDir projDirectory(workingDirectory + "/" + projectName);
    //QDir sourceDirectory(workingDirectory + "/" + projectName + "/" + projectName);
    QDir buildDirectory(workingDirectory + "/" + projectName + "/build_" + buildType);

    if(!projDirectory.exists())
    {
        workDir.mkdir(projectName);
    }
    if(!buildDirectory.exists())
    {
        projDirectory.mkdir(buildDirectory.dirName());
    }

    m_projectName = projectName;
    m_projectDirectory = projDirectory.absolutePath();
    m_buildType = buildType;
}

void Project::addProcess(Process *process)
{
    m_processes[process->processData().type] = process;
    connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &Project::onProcessFinished);
}

void Project::startProcess(ProcessData::ProcessType processType)
{
    Process *process = m_processes[processType];
    if(process == nullptr || !m_isActive)
    {
        emit processFinished(true, (int)processType);
        return;
    }
    process->startProcess();
}

void Project::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Process *sender = dynamic_cast<Process*>(this->sender());
    int phase = (int)sender->processData().type;
    bool successful = exitCode == EXIT_SUCCESS && exitStatus == QProcess::NormalExit;
    emit processFinished(successful, phase);
}
