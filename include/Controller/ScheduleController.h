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

private:
    Schedule& schedule_;
    SystemSettings& settings_; // Schedule::generateSchedule 可能需要 settings

    void handleGenerateSchedule();
    void handleViewSchedule();
    void handleValidateSchedule();
};



#endif //SCHEDULECONTROLLER_H
