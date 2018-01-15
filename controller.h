#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

class Controller
    : public QObject
{
    Q_OBJECT
public:
    Controller(QObject *parent = nullptr);
    ~Controller();

private:

};

#endif // CONTROLLER_H
