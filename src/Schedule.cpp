//
// Created by GlorYouth on 2025/6/2.
//

#include "../include/Schedule.h"
#include <iostream> // 用于打印等

Schedule::Schedule(SystemSettings& sysSettings) : settings(sysSettings) {}

bool Schedule::generateSchedule() {
    // 秩序册自动生成的核心算法将在此实现
    // 这是一个复杂的问题，通常需要启发式算法或约束满足算法
    // 当前版本仅为框架，不实现具体逻辑
    std::cout << "正在尝试生成秩序册 (当前为占位符)..." << std::endl;

    // 示例：简单地将所有未取消的项目添加到赛程中，无时间安排
    scheduleEntries.clear();
    const auto& allEvents = settings.getAllCompetitionEvents();
    int day = 1;
    int count = 0;
    for(const auto& pair : allEvents) {
        const CompetitionEvent& event = pair.second;
        if (!event.getIsCancelled()) {
            ScheduleEntry entry;
            entry.eventId = event.getId();
            // 简陋的时间和场地分配
            entry.startTime = "Day" + std::to_string(day) + " " + (count % 2 == 0 ? "09:00" : "14:00");
            entry.endTime = "Day" + std::to_string(day) + " " + (count % 2 == 0 ? "11:00" : "16:00");
            entry.venue = (event.getEventType() == EventType::TRACK ? "田径场" : "体育馆场地A");
            scheduleEntries.push_back(entry);
            count++;
            if (count % 4 == 0) day++; // 每4个项目换一天（非常粗略）
        }
    }
    std::cout << "秩序册初步条目已生成 (未优化)。" << std::endl;
    return !scheduleEntries.empty();
}

const std::vector<ScheduleEntry>& Schedule::getScheduleEntries() const {
    return scheduleEntries;
}

void Schedule::printSchedule() const {
    std::cout << "\n---------- 秩序册 ----------" << std::endl;
    if (scheduleEntries.empty()) {
        std::cout << "秩序册为空或尚未生成。" << std::endl;
        return;
    }
    for (const auto& entry : scheduleEntries) {
        if (auto event = settings.getCompetitionEvent(entry.eventId); event.has_value()) {
            auto event_ref = event.value().get();
            std::cout << "项目: " << event_ref.getName() << " (ID: " << entry.eventId << ")"
                      << "\n  时间: " << entry.startTime << " - " << entry.endTime
                      << "\n  场地: " << entry.venue << std::endl;
            std::cout << "  参赛人员: ";
            if (event_ref.getParticipantCount() > 0) {
                for(size_t i = 0; i < event_ref.getParticipantAthleteIds().size(); ++i) {
                    auto ath = settings.getAthlete(event_ref.getParticipantAthleteIds()[i]);
                    if (ath.has_value()) std::cout << ath.value().get().getName() << (i == event_ref.getParticipantAthleteIds().size() -1 ? "" : ", ");
                }
            } else {
                std::cout << "无";
            }
            std::cout << std::endl << std::endl;
        }
    }
    std::cout << "--------------------------" << std::endl;
}

bool Schedule::validateSchedule() const {
    // 在此实现赛程验证逻辑
    // 例如检查时间冲突、场地冲突等
    std::cout << "正在验证赛程 (当前为占位符)..." << std::endl;
    return true; // 假设总是有效
}
