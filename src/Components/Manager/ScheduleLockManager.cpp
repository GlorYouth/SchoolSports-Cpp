#include "../../../include/Components/Manager/ScheduleLockManager.h"
#include "../../../include/Components/Core/SystemSettings.h"
#include "../../../include/Components/Core/Workflow.h"

ScheduleLockManager::ScheduleLockManager(SystemSettings& settings) : settings(settings) {}

void ScheduleLockManager::lock() {
    settings._scheduleLocked = true;
    // 成功锁定赛程后，自动进入运动员报名阶段
    settings.workflow.enterRegistrationStage();
}

void ScheduleLockManager::unlock() {
    settings._scheduleLocked = false;
    // 解锁赛程后，自动退回项目设置阶段
    settings.workflow.enterSetupEventsStage();
}

bool ScheduleLockManager::isLocked() const {
    return settings._scheduleLocked;
} 