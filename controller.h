#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "parser.h"
#include "project.h"
#include "process.h"

enum ControllerPhase
{
    None = 0,
    Checkout,
    Configure,
    Build,
    All
};

class Controller
    : public QObject
{
    Q_OBJECT
public:
    Controller(QObject *parent = nullptr);
    ~Controller();

    void startPhase(const ControllerPhase &phase);

public:
    void open(const QString &filePath);

signals:
    void phaseFinished(int phase);

private slots:
    void onProcessFinished(bool successful, int phase);

private:
    ProcessData::ProcessType processTypeFromControllerPhase(const ControllerPhase &phase);

private:
    Parser m_parser;
    ControllerPhase m_activePhase;
    ControllerPhase m_requestedPhase;
    int m_processCounter;
};

#endif // CONTROLLER_H
