#include "../../../include/Components/Manager/ScheduleManager.h"

ScheduleManager::ScheduleManager(DataContainer& dataContainer) : data(dataContainer) {}

bool ScheduleManager::generateSchedule() {
    // 创建临时Schedule对象来生成赛程
    Schedule scheduleGenerator(data);
    bool success = scheduleGenerator.generateSchedule();
    
    // Schedule::generateSchedule()已经将结果保存到data.scheduleEntries中
    return success;
}

const std::vector<ScheduleEntry>& ScheduleManager::getScheduleEntries() const {
    return data.scheduleEntries;
}

void ScheduleManager::printSchedule() const {
    // 创建临时Schedule对象来打印赛程
    Schedule scheduleViewer(data);
    scheduleViewer.printSchedule();
}

std::string ScheduleManager::getScheduleContentAsString() const {
    // 创建临时Schedule对象来获取赛程字符串
    Schedule scheduleViewer(data);
    return scheduleViewer.getScheduleContentAsString();
}

bool ScheduleManager::validateSchedule() const {
    // 创建临时Schedule对象来验证赛程
    Schedule scheduleValidator(data);
    return scheduleValidator.validateSchedule();
} 