#include "project.h"

#include <QDir>

Project::Project(QObject *parent)
    : QObject(parent)
{

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
}
