#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <QObject>

#include "parser.h"
#include "project.h"
#include "process.h"

class QStandardItemModel;

enum class ControllerPhase
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
    void startClean();

    QStandardItemModel* standardItemModel();
    Parser* parser();

    void eraseBuild();

public:
    void open(const QString &filePath);

signals:
    void phaseStarted(ControllerPhase phase);
    void phaseFinished(ControllerPhase phase);

private slots:
    void onProcessFinished(bool successful, ProcessData::ProcessType processFinished);

private:
    ProcessData::ProcessType processTypeFromControllerPhase(const ControllerPhase &phase);
    ControllerPhase controllerPhaseFromProcessType(const ProcessData::ProcessType &processType);
    void finishPhase(ControllerPhase phase);

private:
    Parser m_parser;
    ControllerPhase m_activePhase;
    ControllerPhase m_requestedPhase;
    int m_processCounter;
    QStringList m_extraArguments;
};

#endif // CONTROLLER_H
