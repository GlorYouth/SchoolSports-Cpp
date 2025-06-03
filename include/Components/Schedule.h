//
// Created by GlorYouth on 2025/6/2.
//

#ifndef SCHEDULE_H
#define SCHEDULE_H

#include <vector>
#include <string>
#include "CompetitionEvent.h"
#include "SystemSettings.h"

// 单个赛程条目
struct ScheduleEntry {
    int eventId;
    std::string startTime; // 格式可以定义为 "YYYY-MM-DD HH:MM" 或更简单如 "Day1 09:00"
    std::string endTime;
    std::string venue;     // 比赛场地
    // 可以添加其他信息，如裁判等
};

// 赛程管理类 (秩序册生成)
class Schedule {
private:
    std::vector<ScheduleEntry> scheduleEntries;
    SystemSettings& settings; // 引用系统设置

public:
    explicit Schedule(SystemSettings& sysSettings);

    // 自动生成秩序册 (核心算法)
    // 需要考虑：场地冲突、运动员时间冲突、运动员连续项目、总时长、上下午均衡等
    bool generateSchedule();

    // 获取生成的赛程
    [[nodiscard]] const std::vector<ScheduleEntry>& getScheduleEntries() const;

    // 手动调整赛程 (可选)
    // bool adjustScheduleEntry(int entryIndex, const ScheduleEntry& newEntry);

    // 打印或导出秩序册
    void printSchedule() const; // 或者 exportToFormat()
    std::string getScheduleContentAsString() const; // 新增：获取秩序册内容的字符串表示

    // 验证赛程的有效性
    [[nodiscard]] bool validateSchedule() const;
};

#endif //SCHEDULE_H
