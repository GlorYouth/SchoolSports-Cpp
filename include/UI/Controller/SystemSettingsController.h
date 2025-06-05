//
// Created by GlorYouth on 2025/6/3.
//

#ifndef SYSTEMSETTINGSCONTROLLER_H
#define SYSTEMSETTINGSCONTROLLER_H



#include "../../Components/Core/SystemSettings.h"
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
    // 删除赛程生成管理

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
    
    // 辅助方法
    void displaySessionInfo(); // 显示当前会话时间设置信息
    void handleViewAllVenues(); // 查看所有场地
};




#endif //SYSTEMSETTINGSCONTROLLER_H
