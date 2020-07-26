#include "controller.h"

#include <QDebug>

Controller::Controller(QObject *parent)
    : QObject(parent)
{
    m_activePhase = ControllerPhase::None;
    m_requestedPhase = ControllerPhase::None;
    m_processCounter = 0;

    {
        m_selectedPhases[ControllerPhase::None] = false;
        m_selectedPhases[ControllerPhase::Checkout] = false;
        m_selectedPhases[ControllerPhase::Configure] = false;
        m_selectedPhases[ControllerPhase::Build] = false;
        m_selectedPhases[ControllerPhase::All] = true;
    }
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

    m_activePhase = phase == ControllerPhase::All ? firstActivePhase() : phase;
    if(m_activePhase == ControllerPhase::None)
    {
        finishPhase(phase);
        return;
    }

    m_processCounter = 0;
    m_requestedPhase = phase;

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
        m_activePhase = nextActivePhase(m_activePhase);
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

ControllerPhase Controller::nextActivePhase(ControllerPhase currentActivePhase)
{
    ControllerPhase result = ControllerPhase::All;
    if(currentActivePhase == ControllerPhase::Checkout)
    {
        if(m_selectedPhases[ControllerPhase::Configure])
        {
            result = ControllerPhase::Configure;
        }
        else if(m_selectedPhases[ControllerPhase::Build])
        {
            result = ControllerPhase::Build;
        }
    }
    if(currentActivePhase == ControllerPhase::Configure)
    {
        if(m_selectedPhases[ControllerPhase::Build])
        {
            result = ControllerPhase::Build;
        }
    }

    return result;
}

ControllerPhase Controller::firstActivePhase()
{
    ControllerPhase result = ControllerPhase::None;
    if(m_selectedPhases[ControllerPhase::Checkout])
    {
        result = ControllerPhase::Checkout;
    }
    else if(m_selectedPhases[ControllerPhase::Configure])
    {
        result = ControllerPhase::Configure;
    }
    else if(m_selectedPhases[ControllerPhase::Build])
    {
        result = ControllerPhase::Build;
    }

    return result;
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

void Controller::selectPhase(ControllerPhase phase, bool selected)
{
    m_selectedPhases[phase] = selected;
}
