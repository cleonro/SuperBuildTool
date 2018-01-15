#ifndef STAGE_H
#define STAGE_H

#include <QObject>

class Stage : public QObject
{
    Q_OBJECT
public:
    Stage(QObject *parent = nullptr);
    ~Stage();

private:

};

#endif // STAGE_H
