//
// Created by GlorYouth on 2025/6/3.
//

#ifndef SCHEDULECONTROLLER_H
#define SCHEDULECONTROLLER_H



#include "../../Components/Core/Schedule.h"
#include "../../Components/Core/SystemSettings.h"

class ScheduleController {
public:
    ScheduleController(Schedule& schedule, SystemSettings& settings);
    void manage();
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
    
    // 添加新的辅助方法
    bool validateEventsForScheduling();
};



#endif //SCHEDULECONTROLLER_H
