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
    int projCount = m_parser.projectsCount();
    if(m_activePhase != ControllerPhase::None || projCount == 0)
    {
        return;
    }
    Project* project = m_parser.project(0);

    //project
}

void Controller::onProcessFinished(bool successful, int phase)
{

}

ProcessData::ProcessType Controller::processTypeFromControllerPhase(const ControllerPhase &phase)
{
    ProcessData::ProcessType processType = ProcessData::None;

    switch (phase) {
    case ControllerPhase::Checkout:
        processType = ProcessData::Checkout;
        break;
    case ControllerPhase::Configure:
        processType = ProcessData::Configure;
        break;
    case ControllerPhase::Build:
        processType = ProcessData::Build;
        break;
    }

    return processType;
}
