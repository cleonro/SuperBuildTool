#include "project.h"

#include <QDir>
#include <QDebug>

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

void Project::eraseBuild()
{
    QDir projectDir(m_projectDirectory);
    QDir buildDir(m_projectDirectory + "/build_" + m_buildType);
    buildDir.removeRecursively();
    projectDir.mkdir(buildDir.dirName());

    qInfo() << m_projectName << " build " << m_buildType << " was erased.";
}

void Project::addProcess(Process *process)
{
    m_processes[process->processData().type] = process;
    connect(process, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
            this, &Project::onProcessFinished);
}

void Project::startProcess(ProcessData::ProcessType processType, const QStringList &extraArguments)
{
    Process *process = m_processes[processType];
    if(process == nullptr || !m_isActive)
    {
//        QString msg = "----------- " + m_projectName + " - " + processTypeString(processType)
//                + " no program started -----";
//        qInfo() << msg.toStdString().c_str();
        emit processFinished(true, processType);
        return;
    }
    QString msg = "----------- " + m_projectName + " - " + processTypeString(processType)
            + " started -----";
    qInfo() << msg.toStdString().c_str();
    process->startProcess(extraArguments);
}

void Project::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Process *sender = dynamic_cast<Process*>(this->sender());
    ProcessData::ProcessType processType = sender->processData().type;
    bool successful = exitCode == EXIT_SUCCESS && exitStatus == QProcess::NormalExit;
    QString successString = successful ? " succeeded -----" : " failed -----";
    QString msg = "----------- " + m_projectName + " - " + processTypeString(processType)
            + successString;
    qInfo() << msg.toStdString().c_str();
    emit processFinished(successful, processType);
}

QString Project::processTypeString(const ProcessData::ProcessType &processType)
{
    QString str;
    switch (processType)
    {
    case ProcessData::ProcessType::None:
        str = "None";
        break;
    case ProcessData::ProcessType::Checkout:
        str = "Checkout";
        break;
    case ProcessData::ProcessType::Configure:
        str = "Configure";
        break;
    case ProcessData::ProcessType::Build:
        str = "Build";
        break;
    default:
        break;
    }
    return str;
}
