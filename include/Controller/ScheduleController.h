//
// Created by GlorYouth on 2025/6/3.
//

#ifndef SCHEDULECONTROLLER_H
#define SCHEDULECONTROLLER_H



#include "../Components/Schedule.h"
#include "../../include/Components/SystemSettings.h" // Schedule 可能需要访问 SystemSettings

class ScheduleController {
public:
    ScheduleController(Schedule& schedule, SystemSettings& settings);
    void manage(const SystemSettings& settings);
    void handleLockSchedule();    // 移至 public
    void handleUnlockSchedule();  // 移至 public
    void handleScheduleGeneration();  // 添加新方法用于主菜单中的赛程生成与查看选项

private:
    Schedule& schedule_;
    SystemSettings& settings_; // Schedule::generateSchedule 可能需要 settings

    void handleGenerateSchedule();
    void handleViewSchedule();
    void handleValidateSchedule();
    // 原私有声明移至 public
    // void handleLockSchedule();
    // void handleUnlockSchedule();
};



#endif //SCHEDULECONTROLLER_H
