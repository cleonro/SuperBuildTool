#ifndef PROCESS_H
#define PROCESS_H

#include <QProcess>

class Process : public QProcess
{
    Q_OBJECT
public:
    Process(QObject *parent = nullptr);
    ~Process();

private slots:
    void onReadyReadStandardOutput();
    void onReadyReadStandardError();

private:
    QString m_workingDirectory;
};

#endif // PROCESS_H
