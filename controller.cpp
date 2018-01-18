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
        emit phaseFinished(phase);
        m_activePhase = ControllerPhase::None;
        return;
    }
    m_processCounter = 0;
    m_requestedPhase = phase;
    m_activePhase = phase == ControllerPhase::All ? ControllerPhase::Checkout : phase;
    ProcessData::ProcessType processType = processTypeFromControllerPhase(m_activePhase);
    Project* project = m_parser.project(0);
    project->startProcess(processType);
}

void Controller::onProcessFinished(bool successful, int phase)
{
    if(!successful)
    {
        emit phaseFinished(phase);
        m_activePhase = ControllerPhase::None;
        return;
    }

    m_processCounter += 1;
    int projCount = m_parser.projectsCount();

    if(m_processCounter == projCount)
    {
        if(m_requestedPhase != ControllerPhase::All || m_activePhase == m_requestedPhase)
        {
            emit phaseFinished(phase);
            m_activePhase = ControllerPhase::None;
            return;
        }

        m_activePhase = m_activePhase == ControllerPhase::Checkout ? ControllerPhase::Configure :
                       (m_activePhase == ControllerPhase::Configure ? ControllerPhase::Build : ControllerPhase::All);
        m_processCounter = 0;
        ProcessData::ProcessType processType = processTypeFromControllerPhase(m_activePhase);
        Project* project = m_parser.project(0);
        project->startProcess(processType);
        return;
    }

    ProcessData::ProcessType processType = processTypeFromControllerPhase(m_activePhase);
    Project* project = m_parser.project(m_processCounter);
    project->startProcess(processType);
}

ProcessData::ProcessType Controller::processTypeFromControllerPhase(const ControllerPhase &phase)
{
    ProcessData::ProcessType processType = ProcessData::None;

    switch (phase) {
    case ControllerPhase::None:
        processType = ProcessData::None;
        break;
    case ControllerPhase::Checkout:
        processType = ProcessData::Checkout;
        break;
    case ControllerPhase::Configure:
        processType = ProcessData::Configure;
        break;
    case ControllerPhase::Build:
        processType = ProcessData::Build;
        break;
    case ControllerPhase::All:
        processType = ProcessData::Checkout;
        break;
    }

    return processType;
}
