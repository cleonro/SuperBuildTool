#ifndef PARSER_H
#define PARSER_H

#include <QObject>

class Parser : public QObject
{
    Q_OBJECT
public:
    Parser(QObject *parent = nullptr);
    ~Parser();

public:

private:

};

#endif // PARSER_H
