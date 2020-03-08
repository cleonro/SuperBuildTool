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
    , m_intermediaryStepsCount(0)
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
    QProcess *process = qobject_cast<QProcess*>(sender());
    QString outputText = process->readAllStandardOutput();
    qInfo() << outputText.toStdString().c_str();
}

void Process::onReadyReadStandardError()
{
    QProcess *process = qobject_cast<QProcess*>(sender());
    QByteArray errorText = process->readAllStandardError();
    qInfo() << errorText.toStdString().c_str();
}

void Process::startProcess(const QStringList &extraArguments)
{
    if(m_project == nullptr)
    {
        return;
    }
    setupProcess();
    m_intermediaryStepsCount = 0;

    if(extraArguments.count() > 0)
    {
        QStringList args = this->arguments();
        args.append(extraArguments);
        this->setArguments(args);
    }

    if(m_intermediarySteps.count() > 0)
    {
        m_intermediarySteps[0]->start();
        return;
    }

    this->start();
}

void Process::setupProcess()
{
    m_intermediarySteps.clear();
    QString projectDir = m_project->projectDirectory();
    QString projectName = m_project->projectName();
    QString buildType = m_project->buildType();
    if(m_data.type == ProcessData::Checkout)
    {
        QDir gitDir(projectDir + "/" + projectName + "/.git");
        if(!gitDir.exists())
        {
            bool intermediaryStep = !m_data.commit.isEmpty();
            QProcess *cloneProcess = intermediaryStep ? new QProcess(this) : this;
            cloneProcess->setProgram("git");
            cloneProcess->setWorkingDirectory(projectDir);
            QStringList cloneArgs;
            cloneArgs<< "clone" << "--progress" << m_data.repository << projectName << "--branch" << m_data.branch;
            cloneProcess->setArguments(cloneArgs);
            if(intermediaryStep)
            {
                m_intermediarySteps.push_back(cloneProcess);
                connect(cloneProcess, static_cast<void(QProcess::*)(int, QProcess::ExitStatus)>(&QProcess::finished),
                        this, &Process::onProcessFinished);
                connect(cloneProcess, &QProcess::readyReadStandardOutput, this, &Process::onReadyReadStandardOutput);
                connect(cloneProcess, &QProcess::readyReadStandardError, this, &Process::onReadyReadStandardError);

                QProcess *checkoutProcess = this;
                checkoutProcess->setProgram("git");
                checkoutProcess->setWorkingDirectory(projectDir + "/" + projectName);
                QStringList checkoutArgs;
                checkoutArgs << "checkout" << "--progress" << m_data.commit;
                checkoutProcess->setArguments(checkoutArgs);
            }
        }
        else
        {
            this->setWorkingDirectory(projectDir + "/" + projectName);
            QStringList args;

            if(!m_data.commit.isEmpty())
            {
                args << "checkout" << "--progress" << m_data.commit;
            }
            else
            {
                args<< "pull" << "--progress";
            }
            this->setArguments(args);
        }
    }
    if(m_data.type == ProcessData::Configure)
    {
        this->setProgram("cmake");
        this->setWorkingDirectory(projectDir + "/build_" + buildType);
        QStringList args;
        //TODO - take into account other CMake generators
        args << "-G" << "Ninja";
        for(int i = 0; i < m_data.cmakeVariables.count(); ++i)
        {
            QString value = setupCMakeVariable(m_data.cmakeVariables[i]);
            args << "-D" << value;
        }
        QString cmakeBuildType = "CMAKE_BUILD_TYPE=" + buildType;
        args << "-D" << cmakeBuildType;
        QDir sourceDir(projectDir + "/" + projectName);
        args << sourceDir.absolutePath();
        this->setArguments(args);
    }
    if(m_data.type == ProcessData::Build)
    {
        this->setProgram("ninja");
        this->setWorkingDirectory(projectDir + "/build_" + buildType);
        QStringList args;
        this->setArguments(args);
    }
}

void Process::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_intermediaryStepsCount += 1;

    if(exitStatus != QProcess::NormalExit)
    {
        emit this->finished(exitCode, exitStatus);
        return;
    }

    if(m_intermediaryStepsCount == m_intermediarySteps.count())
    {
        m_intermediaryStepsCount = 0;
        this->start();
    }
}

QString Process::setupCMakeVariable(const ProcessData::CMakeVariable &cmakeVariable)
{
    //types that need to be translated to CMake types
    //      PROJECTBUILDPATH, WORKINGDIRECTORYPATH, WORKINGDIRECTORYFILEPATH, WORKINGDIRECTORYBUILDPATH
    QString type;
    QString value;
    if(cmakeVariable.type == "PROJECTBUILDPATH")
    {
        type = "PATH";
        int sepPos = cmakeVariable.value.indexOf('/');
        int strCount = cmakeVariable.value.count();
        QString projName = cmakeVariable.value.left(sepPos);
        QString projPath = cmakeVariable.value.right(strCount - projName.count());
        QDir dir(m_project->projectDirectory() + "/../" + projName + "/build_" + m_project->buildType() + projPath);
        value = dir.absolutePath();
    }
    else if(cmakeVariable.type == "WORKINGDIRECTORYPATH")
    {
        type = "PATH";
        QDir dir(m_project->projectDirectory() + "/../" + cmakeVariable.value);
        value = dir.absolutePath();
    }
    else if(cmakeVariable.type == "WORKINGDIRECTORYFILEPATH")
    {
        type = "FILEPATH";
        QDir dir(m_project->projectDirectory() + "/../" + cmakeVariable.value);
        value = dir.absolutePath();
    }
    else if(cmakeVariable.type == "WORKINGDIRECTORYBUILDPATH")
    {
        type = "PATH";
        QDir dir(m_project->projectDirectory() + "/../" + cmakeVariable.value + "/build_" + m_project->buildType());
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
    r += value;
    return r;
}
