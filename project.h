#ifndef STAGE_H
#define STAGE_H

#include <QObject>

class Project : public QObject
{
    Q_OBJECT
public:
    Project(QObject *parent = nullptr);
    ~Project();

private:

};

#endif // STAGE_H
