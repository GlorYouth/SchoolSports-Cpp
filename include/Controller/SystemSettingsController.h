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
    void manage(); // 系统设置管理主循环
    // 场地管理功能
    void handleVenueManagement();
    // 上午下午/时间段设置
    void handleSessionSettings();
    // 秩序册生成功能
    void handleScheduleGeneration();

private:
    SystemSettings& settings_; // 操作系统设置对象

    // --- 子菜单对应的处理函数 ---
    void handleAddUnit() const;
    void handleViewAllUnits();
    void handleAddEvent();
    void handleViewAllEvents();
    void handleAddAthlete();
    void handleViewAllAthletes();
    void handleSetAthleteMaxEvents();
    
    // 重构后的计分规则统一管理函数
    void handleScoreRuleManagement();
    void handleAddScoreRule(bool isComposite = false);
    void handleViewAllScoreRules();
    void handleManageExistingScoreRule();
};




#endif //SYSTEMSETTINGSCONTROLLER_H
