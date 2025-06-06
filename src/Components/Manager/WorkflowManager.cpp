#include "../../../include/Components/Manager/WorkflowManager.h"
#include "../../../include/Components/Core/SystemSettings.h"

WorkflowManager::WorkflowManager(SystemSettings& settings) : settings(settings) {}

WorkflowStage WorkflowManager::getCurrentStage() const {
    return settings._currentWorkflowStage;
}

bool WorkflowManager::setStage(WorkflowStage newStage) {
    settings._currentWorkflowStage = newStage;
    // 同步到DataContainer
    settings.getData().currentWorkflowStage = newStage;
    return true;
}

bool WorkflowManager::enterSetupEventsStage() {
    return setStage(WorkflowStage::SETUP_EVENTS);
}

bool WorkflowManager::enterRegistrationStage() {
    return setStage(WorkflowStage::REGISTRATION_OPEN);
}

bool WorkflowManager::enterCompetitionStage() {
    return setStage(WorkflowStage::COMPETITION_RUNNING);
}

bool WorkflowManager::isInSetupEventsStage() const {
    return getCurrentStage() == WorkflowStage::SETUP_EVENTS;
}

bool WorkflowManager::isInRegistrationStage() const {
    return getCurrentStage() == WorkflowStage::REGISTRATION_OPEN;
}

bool WorkflowManager::isInCompetitionStage() const {
    return getCurrentStage() == WorkflowStage::COMPETITION_RUNNING;
} 