//
// Created by GlorYouth on 2025/6/3.
//

#ifndef SYSTEMSETTINGSCONTROLLER_H
#define SYSTEMSETTINGSCONTROLLER_H



#include "../Components/SystemSettings.h"
#include <string> // For std::string
#include <vector> // For std::vector
#include <map>    // For std::map for scores

class SystemSettingsController {
public:
    SystemSettingsController(SystemSettings& settings);
    void manage(); // 系统设置管理的主循环

private:
    SystemSettings& settings_; // 引用系统设置对象

    // --- 各子菜单项对应的处理函数 ---
    void handleAddUnit() const;
    void handleViewAllUnits();
    void handleAddEvent();
    void handleViewAllEvents();
    void handleAddAthlete();
    void handleViewAllAthletes();
    void handleAddScoreRule();
    void handleViewAllScoreRules();
    void handleSetAthleteMaxEvents();
    void handleSetMinParticipantsToHoldEvent();
};




#endif //SYSTEMSETTINGSCONTROLLER_H
