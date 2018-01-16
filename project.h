#ifndef STAGE_H
#define STAGE_H

#include <QObject>

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

private:
    QString m_projectName;
    QString m_projectDirectory;
    QString m_buildType;

};

#endif // STAGE_H
