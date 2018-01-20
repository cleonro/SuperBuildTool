#ifndef PARSER_H
#define PARSER_H

#include <QObject>
#include <QDomDocument>
#include <QMap>
#include <QVector>
#include <QStandardItemModel>

class Project;

class Parser : public QObject
{
    Q_OBJECT
    typedef bool(Parser::*SectionParserType)(const QDomElement &);
    typedef void(Parser::*OnItemchangedType)(QStandardItem *);
public:
    Parser(QObject *parent = nullptr);
    ~Parser();

public:
    bool open(const QString &filePath);
    QString workingDirectory();
    QString buildType();
    void setBuildType(const QString &buildType);

    int projectsCount();
    Project* project(const int &i);
    QStandardItemModel *standardItemModel();

signals:
    void parsingStarted(QString filePath);
    void parsingFinished(bool result);

private slots:
    void onStandardItemChanged(QStandardItem *item);

private:
    bool parseDocument(const QString &filePath);
    bool parseProjectsSection(const QDomElement &element);
    bool parseWorkingDirectorySection(const QDomElement &element);
    bool parseBuildTypeSection(const QDomElement &element);

    bool createCheckoutProcess(Project *project, const QDomNode &domNode, QStandardItem* projectItem);
    bool createConfigureProcess(Project *project, const QDomNode &domNode, QStandardItem* projectItem);
    bool createBuildProcess(Project *project, const QDomNode &domNode, QStandardItem* projectItem);

    void onProjectItemChanged(QStandardItem *item);

    void clear();

private:
    static const QString sWorkingDirectory;
    static const QString sBuildType;
    static const QString sProjects;
    static const QString sCheckout;
    static const QString sRepository;
    static const QString sBranch;
    static const QString sConfigure;
    static const QString sGenerator;
    static const QString sVariable;
    static const QString sName;
    static const QString sType;
    static const QString sBuild;

    QMap<QString, SectionParserType> m_sectionParsers;
    QMap<QString, OnItemchangedType> m_itemChangedActions;

    QString m_workingDirectory;
    QString m_buildType;
    QVector<Project*> m_projects;
    QStandardItemModel m_model;
};

#endif // PARSER_H
