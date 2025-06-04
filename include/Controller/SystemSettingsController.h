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
    // 场馆管理相关
    void handleVenueManagement();
    // 管理上午/下午时段配置
    void handleSessionSettings();
    // 管理赛程生成
    void handleScheduleGeneration();

private:
    SystemSettings& settings_; // 管理系统设置对象

    // --- 子菜单对应的处理函数 ---
    void handleAddUnit() const;
    void handleViewAllUnits();
    void handleAddEvent();
    void handleViewAllEvents();
    void handleAddAthlete();
    void handleViewAllAthletes();
    void handleAddScoreRule();
    void handleViewAllScoreRules();
    void handleSetAthleteMaxEvents();
    
    // 新增：处理复合规则的添加和管理
    void handleAddCompositeScoreRule();
    void handleManageCompositeScoreRule();
};




#endif //SYSTEMSETTINGSCONTROLLER_H
