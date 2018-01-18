#include "parser.h"
#include "project.h"

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
    return r;
}

bool Parser::createConfigureProcess(Project *project, const QDomNode &domNode)
{
    bool r = true;
    if(domNode.isNull())
    {
        return false;
    }
    return r;
}

bool Parser::createBuildProcess(Project *project, const QDomNode &domNode)
{
    bool r = true;
    if(domNode.isNull())
    {
        return false;
    }
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
