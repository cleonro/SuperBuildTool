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
    bool open(const QString &filePath);

private:
    bool parseDocument(const QString &filePath);

private:
    static const QString sWorkingDirectory;
    static const QString sProjects;
    static const QString sCheckout;
    static const QString sRepository;
    static const QString sBranch;
    static const QString sConfigure;
    static const QString sVariable;
    static const QString sName;
    static const QString sType;
    static const QString sBuild;
};

#endif // PARSER_H
