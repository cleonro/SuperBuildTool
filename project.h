#ifndef PROJECT_H
#define PROJECT_H

#include "process.h"

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
    void setActive(const bool &active);
    bool isActive();

    void setProjectStructure(const QString &projectName, const QString &workingDirectory, const QString &buildType);
    void addProcess(Process *process);

    void startProcess(ProcessData::ProcessType processType, const QStringList &extraArguments = QStringList());

    QString processTypeString(const ProcessData::ProcessType &processType);

    void eraseBuild();

signals:
    void processFinished(bool successful, ProcessData::ProcessType phase);

private slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QString m_projectName;
    QString m_projectDirectory;
    QString m_buildType;
    bool m_isActive;

    QMap<ProcessData::ProcessType, Process*> m_processes;
};

#endif // PROJECT_H
