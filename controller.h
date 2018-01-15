#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "parser.h"

class Controller
    : public QObject
{
    Q_OBJECT
public:
    Controller(QObject *parent = nullptr);
    ~Controller();

public:
    void open(const QString &filePath);

private:
    Parser m_parser;
};

#endif // CONTROLLER_H
