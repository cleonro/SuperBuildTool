#ifndef STAGE_H
#define STAGE_H

#include <process.h>

#include <QObject>
#include <QMap>

class Project : public QObject
{
    Q_OBJECT
public:
    Project(QObject *parent = nullptr);
    ~Project();

    QString projectName();
    QString projectDirectory();
    QString buildType();

    void setProjectStructure(const QString &projectName, const QString &workingDirectory, const QString &buildType);
    void addProcess(Process *process);

private:
    QString m_projectName;
    QString m_projectDirectory;
    QString m_buildType;

    QMap<ProcessData::ProcessType, Process*> m_processes;
};

#endif // STAGE_H
