#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QDomDocument>
#include <QMap>
#include <QVector>

class Project;

class Parser : public QObject
{
    Q_OBJECT
    typedef bool(Parser::*SectionParserType)(const QDomElement &);
public:
    Parser(QObject *parent = nullptr);
    ~Parser();

public:
    bool open(const QString &filePath);
    QString workingDirectory();
    QString buildType();

private:
    bool parseDocument(const QString &filePath);
    bool parseProjectsSection(const QDomElement &element);
    bool parseWorkingDirectorySection(const QDomElement &element);
    bool parseBuildTypeSection(const QDomElement &element);

    bool createCheckoutProcess(Project *project, const QDomNode &domNode);
    bool createConfigureProcess(Project *project, const QDomNode &domNode);
    bool createBuildProcess(Project *project, const QDomNode &domNode);

    void clear();

private:
    static const QString sWorkingDirectory;
    static const QString sBuildType;
    static const QString sProjects;
    static const QString sCheckout;
    static const QString sRepository;
    static const QString sBranch;
    static const QString sConfigure;
    static const QString sVariable;
    static const QString sName;
    static const QString sType;
    static const QString sBuild;

    QMap<QString, SectionParserType> m_sectionParsers;

    QString m_workingDirectory;
    QString m_buildType;
    QVector<Project*> m_projects;
};

#endif // PARSER_H
