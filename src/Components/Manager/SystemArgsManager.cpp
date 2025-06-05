#include "../../../include/Components/Manager/SystemArgsManager.h"
#include "../../../include/Components/Core/SystemSettings.h"
#include <iostream>

SystemArgsManager::SystemArgsManager(SystemSettings& settings) : settings(settings) {}

void SystemArgsManager::setAthleteMaxEventsAllowed(int maxEvents) {
    if (maxEvents > 0) {
        settings._athleteMaxEventsAllowed = maxEvents;
    } else {
        std::cerr << "错误: 运动员最大参赛项目数必须大于0。" << std::endl;
    }
}

int SystemArgsManager::getAthleteMaxEventsAllowed() const {
    return settings._athleteMaxEventsAllowed;
}

void SystemArgsManager::resetToDefaults() {
    // 设置默认值
    setAthleteMaxEventsAllowed(3); // 默认每位运动员最多可参加3个项目
    
    // 未来可能添加的其他参数默认值设置
    // setMinimumParticipantsForEvent(4);
} 