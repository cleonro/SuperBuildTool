#ifndef PROCESS_H
#define PROCESS_H

#include <QProcess>
#include <QVector>

class Project;

struct ProcessData
{
    enum ProcessType
    {
        None = 0,
        Checkout,
        Configure,
        Build
    };

    struct CMakeVariable
    {
        CMakeVariable();
        ~CMakeVariable();

        QString name;
        QString type;
        QString value;
    };

    ProcessData();
    ~ProcessData();

    ProcessType type;
    QString repository;
    QString branch;
    QString generator;
    QVector<CMakeVariable> cmakeVariables;
};

class Process : public QProcess
{
    Q_OBJECT
public:
    Process(QObject *parent = nullptr);
    ~Process();

    void setProject(Project *project);
    Project* project();
    ProcessData& processData();

    void startProcess(const QStringList &extraArguments = QStringList());

private slots:
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();

private:
    void setupProcess();
    QString setupCMakeVariable(const ProcessData::CMakeVariable &cmakeVariable);

private:
    QString m_workingDirectory;
    Project *m_project;
    ProcessData m_data;
};

#endif // PROCESS_H
