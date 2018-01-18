#include "parser.h"
#include "project.h"
#include "process.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QDebug>

const QString Parser::sWorkingDirectory = "WorkingDirectory";
const QString Parser::sBuildType = "BuildType";
const QString Parser::sProjects = "Projects";
const QString Parser::sCheckout = "Checkout";
const QString Parser::sRepository = "repository";
const QString Parser::sBranch = "branch";
const QString Parser::sConfigure = "Configure";
const QString Parser::sGenerator = "generator";
const QString Parser::sVariable = "variable";
const QString Parser::sName = "name";
const QString Parser::sType = "type";
const QString Parser::sBuild = "Build";

Parser::Parser(QObject *parent)
    : QObject(parent)
{
    m_sectionParsers[sProjects] = &Parser::parseProjectsSection;
    m_sectionParsers[sWorkingDirectory] = &Parser::parseWorkingDirectorySection;
    m_sectionParsers[sBuildType] = &Parser::parseBuildTypeSection;
}

Parser::~Parser()
{

}

bool Parser::open(const QString &filePath)
{
    clear();

    bool r = true;
    QFileInfo fileInfo(filePath);
    QDir parentDir = fileInfo.dir();
    parentDir.cdUp();
    parentDir.cdUp();
    m_workingDirectory = parentDir.absolutePath();
    r = parseDocument(filePath);
    if(!r)
    {
        m_workingDirectory = "";
    }
    return r;
}

QString Parser::workingDirectory()
{
    return m_workingDirectory;
}

QString Parser::buildType()
{
    return m_buildType;
}

bool Parser::parseDocument(const QString &filePath)
{
    bool r = true;

    QDomDocument doc;
    QFile fileDoc(filePath);
    if(!fileDoc.open(QIODevice::ReadOnly))
    {
        return false;
    }
    if(!doc.setContent(&fileDoc))
    {
        fileDoc.close();
        return false;
    }
    fileDoc.close();

    QDomElement docElem = doc.documentElement();
    QDomNode n = docElem.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();

        if(e.isNull())
        {
            n = n.nextSibling();
            continue;
        }

        QString tagName = e.tagName();
        if(m_sectionParsers.contains(tagName))
        {
            r = (this->*m_sectionParsers[tagName])(e);
            if(!r)
            {
                return false;
            }
        }
        n = n.nextSibling();
    }

    return r;
}

bool Parser::parseProjectsSection(const QDomElement &element)
{
    bool r = true;
    QDomNode n = element.firstChild();
    while(!n.isNull())
    {
        QDomElement e = n.toElement();
        if(e.isNull())
        {
            n = n.nextSibling();
            continue;
        }
        QString projectName = e.tagName();
        Project *proj = new Project(this);
        proj->setProjectStructure(projectName, m_workingDirectory, m_buildType);
        QDomNode checkoutElem = e.elementsByTagName(sCheckout).at(0);
        QDomNode configElem = e.elementsByTagName(sConfigure).at(0);
        QDomNode buildElem = e.elementsByTagName(sBuild).at(0);
        createCheckoutProcess(proj, checkoutElem);
        createConfigureProcess(proj, configElem);
        createBuildProcess(proj, buildElem);
        n = n.nextSibling();
        m_projects.push_back(proj);
    }
    return r;
}

bool Parser::parseWorkingDirectorySection(const QDomElement &element)
{
    bool r = true;
    QString value = element.text().trimmed();
    QDir dir(value);
    bool dirExists = dir.exists();
    if(!value.isEmpty() && dirExists)
    {
        m_workingDirectory = value;
    }
    return r;
}

bool Parser::parseBuildTypeSection(const QDomElement &element)
{
    bool r = true;
    static QStringList acceptedValues({"", "Debug", "Release", "debug", "release"});
    QString value = element.text().trimmed();
    if(!acceptedValues.contains(value))
    {
        m_buildType = "";
        r = false;
    }
    m_buildType = value;
    return r;
}

bool Parser::createCheckoutProcess(Project *project, const QDomNode &domNode)
{
    bool r = true;
    if(domNode.isNull())
    {
        return false;
    }
    QDomElement e = domNode.toElement();
    QDomElement repElem = e.elementsByTagName(sRepository).at(0).toElement();
    if(repElem.isNull())
    {
        return false;
    }
    QDomElement branchElem = e.elementsByTagName(sBranch).at(0).toElement();
    if(branchElem.isNull())
    {
        return false;
    }
    QString repository = repElem.text().trimmed();
    QString branch = repElem.text().trimmed();
    Process *process = new Process(project);
    process->setProject(project);
    ProcessData &processData = process->processData();
    processData.type = ProcessData::Checkout;
    processData.repository = repository;
    processData.branch = branch;
    project->addProcess(process);
    return r;
}

bool Parser::createConfigureProcess(Project *project, const QDomNode &domNode)
{
    bool r = true;
    if(domNode.isNull())
    {
        return false;
    }
    QDomElement e = domNode.toElement();
    QDomElement genElem = e.elementsByTagName(sGenerator).at(0).toElement();
    if(genElem.isNull())
    {
        return false;
    }
    QString generator = genElem.text();
    QDomNodeList varNodes = e.elementsByTagName(sVariable);
    Process *process = new Process(project);
    process->setProject(project);
    ProcessData& processData = process->processData();
    processData.type = ProcessData::Configure;
    processData.generator = generator;
    for(int i = 0; i < varNodes.count(); ++i)
    {
        QDomElement varElem = varNodes.at(i).toElement();
        if(varElem.isNull())
        {
            continue;
        }
        ProcessData::CMakeVariable cmakeVar;
        cmakeVar.name = varElem.attribute(sName);
        cmakeVar.type = varElem.attribute(sType);
        cmakeVar.value = varElem.text();
        processData.cmakeVariables.push_back(cmakeVar);
    }
    project->addProcess(process);
    return r;
}

bool Parser::createBuildProcess(Project *project, const QDomNode &domNode)
{
    bool r = true;
    if(domNode.isNull())
    {
        return false;
    }
    Process *process = new Process(project);
    process->setProject(project);
    ProcessData& processData = process->processData();
    processData.type = ProcessData::Build;
    project->addProcess(process);
    return r;
}

void Parser::clear()
{
    m_workingDirectory = "";
    m_buildType = "";
    for(int i = 0; i < m_projects.count(); ++i)
    {
        m_projects[i]->deleteLater();
    }
    m_projects.clear();
}

int Parser::projectsCount()
{
    int count = m_projects.count();
    return count;
}

Project* Parser::project(const int &i)
{
    Project *r = i >=0 && i < m_projects.count() ? m_projects[i] : nullptr;
    return r;
}
