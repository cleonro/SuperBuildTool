#include "controller.h"

#include <QDebug>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    m_activePhase = ControllerPhase::None;
    m_requestedPhase = ControllerPhase::None;
    m_processCounter = 0;
}

Controller::~Controller()
{

}

void Controller::open(const QString &filePath)
{
    bool r = m_parser.open(filePath);
    if(r)
    {
        int projCount = m_parser.projectsCount();
        for(int i = 0; i < projCount; ++i)
        {
            Project *proj = m_parser.project(i);
            connect(proj, &Project::processFinished, this, &Controller::onProcessFinished);
        }
    }
}

void Controller::startPhase(const ControllerPhase &phase)
{
    emit phaseStarted(phase);

    int projCount = m_parser.projectsCount();
    if(phase == ControllerPhase::None || m_activePhase != ControllerPhase::None || projCount == 0)
    {
        finishPhase(phase);
        return;
    }
    m_processCounter = 0;
    m_requestedPhase = phase;
    m_activePhase = phase == ControllerPhase::All ? ControllerPhase::Checkout : phase;
    ProcessData::ProcessType processType = processTypeFromControllerPhase(m_activePhase);
    Project* project = m_parser.project(0);
    project->startProcess(processType, m_extraArguments);
}

void Controller::onProcessFinished(bool successful, ProcessData::ProcessType processFinished)
{
    ControllerPhase phaseFinished = controllerPhaseFromProcessType(processFinished);
    if(!successful)
    {
        finishPhase(phaseFinished);
        return;
    }

    //AllPhase - version 1
    //step all projects through one phase (e.g. Checkout), than change phase (e.g. Checkout->Configure) and repeat
//    m_processCounter += 1;
//    int projCount = m_parser.projectsCount();

//    if(m_processCounter == projCount)
//    {
//        if(m_requestedPhase != ControllerPhase::All || m_activePhase == m_requestedPhase)
//        {
//            finishPhase(phase);
//            return;
//        }

//        m_activePhase = m_activePhase == ControllerPhase::Checkout ? ControllerPhase::Configure :
//                       (m_activePhase == ControllerPhase::Configure ? ControllerPhase::Build : ControllerPhase::All);
//        if(m_activePhase == ControllerPhase::All)
//        {
//            finishPhase(phase);
//            return;
//        }
//        m_processCounter = 0;
//        ProcessData::ProcessType processType = processTypeFromControllerPhase(m_activePhase);
//        Project* project = m_parser.project(0);
//        project->startProcess(processType, m_extraArguments);
//        return;
//    }

//    ProcessData::ProcessType processType = processTypeFromControllerPhase(m_activePhase);
//    Project* project = m_parser.project(m_processCounter);
//    project->startProcess(processType, m_extraArguments);

    //AllPhase - version 2
    //step a project through all phases, than change to the next project
    int projCount = m_parser.projectsCount();
    if(m_requestedPhase != ControllerPhase::All)
    {
        m_processCounter += 1;
        if(m_processCounter == projCount)
        {
            finishPhase(phaseFinished);
            return;
        }
    }
    else
    {
        m_activePhase = m_activePhase == ControllerPhase::Checkout ? ControllerPhase::Configure :
                       (m_activePhase == ControllerPhase::Configure ? ControllerPhase::Build : ControllerPhase::All);
        if(m_activePhase == ControllerPhase::All)
        {
            m_processCounter += 1;
            if(m_processCounter == projCount)
            {
                finishPhase(phaseFinished);
                return;
            }
            m_activePhase = ControllerPhase::Checkout;
        }
    }
    ProcessData::ProcessType processToStart = processTypeFromControllerPhase(m_activePhase);
    Project* project = m_parser.project(m_processCounter);
    project->startProcess(processToStart, m_extraArguments);
}

ProcessData::ProcessType Controller::processTypeFromControllerPhase(const ControllerPhase &phase)
{
    ProcessData::ProcessType processType = ProcessData::ProcessType::None;

    switch (phase) {
    case ControllerPhase::None:
        processType = ProcessData::ProcessType::None;
        break;
    case ControllerPhase::Checkout:
        processType = ProcessData::ProcessType::Checkout;
        break;
    case ControllerPhase::Configure:
        processType = ProcessData::ProcessType::Configure;
        break;
    case ControllerPhase::Build:
        processType = ProcessData::ProcessType::Build;
        break;
    case ControllerPhase::All:
        processType = ProcessData::ProcessType::Checkout;
        break;
    }

    return processType;
}

ControllerPhase Controller::controllerPhaseFromProcessType(const ProcessData::ProcessType &processType)
{
    ControllerPhase phase = ControllerPhase::None;
    switch (processType) {
    case ProcessData::ProcessType::None:
        phase = ControllerPhase::None;
        break;
    case ProcessData::ProcessType::Checkout:
        phase = ControllerPhase::Checkout;
        break;
    case ProcessData::ProcessType::Configure:
        phase = ControllerPhase::Configure;
        break;
    case ProcessData::ProcessType::Build:
        phase = ControllerPhase::Build;
        break;
    }
    return phase;
}

void Controller::startClean()
{
    m_extraArguments.clear();
    m_extraArguments << "clean";
    startPhase(ControllerPhase::Build);
}

void Controller::finishPhase(ControllerPhase phase)
{
    emit phaseFinished(phase);
    m_activePhase = ControllerPhase::None;
    m_extraArguments.clear();
}

QStandardItemModel* Controller::standardItemModel()
{
    return m_parser.standardItemModel();
}

Parser* Controller::parser()
{
    return &m_parser;
}

void Controller::eraseBuild()
{
    for (int i = 0; i < m_parser.projectsCount(); ++i)
    {
        Project *p = m_parser.project(i);
        if(p->isActive())
        {
            p->eraseBuild();
        }
    }
}
