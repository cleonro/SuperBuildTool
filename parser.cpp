#include "parser.h"

#include <QFile>
#include <QDebug>

const QString Parser::sWorkingDirectory = "WorkingDirectory";
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
}

Parser::~Parser()
{

}

bool Parser::open(const QString &filePath)
{
    bool r = true;

    r = parseDocument(filePath);
    return r;
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
    qDebug() << Q_FUNC_INFO << element.tagName();
    return r;
}

bool Parser::parseWorkingDirectorySection(const QDomElement &element)
{
    bool r = true;
    qDebug() << Q_FUNC_INFO << element.tagName();
    return r;
}
