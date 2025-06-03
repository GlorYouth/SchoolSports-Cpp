//
// Created by GlorYouth on 2025/6/2.
//

#include "../../include/Components/Schedule.h"
#include <iostream> // 用于打印等
#include <ranges>
#include <sstream>

Schedule::Schedule(SystemSettings& sysSettings) : settings(sysSettings) {}

bool Schedule::generateSchedule() {
    std::cout << "正在智能生成秩序册..." << std::endl;
    scheduleEntries.clear();
    // 1. 收集所有未取消项目，按持续时间降序排序
    std::vector<CompetitionEvent> events;
    for (const auto& event : settings.getAllCompetitionEventsConst() | std::views::values) {
        if (!event.get().getIsCancelled()) {
            events.push_back(event);
        }
    }
    std::sort(events.begin(), events.end(), [](utils::RefConst<CompetitionEvent> a, utils::RefConst<CompetitionEvent> b) {
        return a.get().getDurationMinutes() > b.get().getDurationMinutes();
    });
    // 2. 获取所有场地
    const auto& venues = settings.getAllVenues();
    if (venues.empty()) {
        std::cout << "错误：场地表为空，请先添加场地。" << std::endl;
        return false;
    }
    // 3. 获取上午/下午时间段
    auto [morningStart, morningEnd] = settings.getMorningSession();
    auto [afternoonStart, afternoonEnd] = settings.getAfternoonSession();
    // 辅助函数：时间字符串转分钟
    auto timeToMin = [](const std::string& t) -> int {
        if (t.size() != 5 || t[2] != ':') return -1;
        return std::stoi(t.substr(0,2))*60 + std::stoi(t.substr(3,2));
    };
    int morningStartMin = timeToMin(morningStart), morningEndMin = timeToMin(morningEnd);
    int afternoonStartMin = timeToMin(afternoonStart), afternoonEndMin = timeToMin(afternoonEnd);
    if (morningStartMin < 0 || morningEndMin <= morningStartMin || afternoonStartMin < 0 || afternoonEndMin <= afternoonStartMin) {
        std::cout << "错误：上午/下午时间段设置不合法。" << std::endl;
        return false;
    }
    // 4. 维护每个场地每天的上午/下午已排时间表
    struct Slot { int start, end; };
    using VenueDayKey = std::tuple<std::string, int, bool>; // 场地, 天, isMorning
    std::map<VenueDayKey, std::vector<Slot>> venueSchedule;
    int day = 1;
    // 5. 依次为每个项目分配时间
    for (CompetitionEvent event : events) {
        int duration = event.getDurationMinutes();
        bool scheduled = false;
        // 尝试从第1天开始，直到找到可排入的空隙
        for (day = 1; day < 100; ++day) { // 最多排100天
            for (const auto& venue : venues) {
                for (int isMorning = 0; isMorning <= 1; ++isMorning) {
                    int sessionStart = isMorning ? morningStartMin : afternoonStartMin;
                    int sessionEnd = isMorning ? morningEndMin : afternoonEndMin;
                    auto& slots = venueSchedule[{venue, day, isMorning}];
                    // 查找该场地该天该时段的可用空隙
                    int lastEnd = sessionStart;
                    bool found = false;
                    // 按已排slot排序
                    std::sort(slots.begin(), slots.end(), [](const Slot& a, const Slot& b){ return a.start < b.start; });
                    for (const auto& slot : slots) {
                        if (slot.start - lastEnd >= duration) {
                            // 找到空隙
                            int s = lastEnd, e = lastEnd + duration;
                            // 分配
                            event.setVenue(venue);
                            char bufS[6], bufE[6];
                            snprintf(bufS, 6, "%02d:%02d", s/60, s%60);
                            snprintf(bufE, 6, "%02d:%02d", e/60, e%60);
                            event.setStartTime(bufS);
                            event.setEndTime(bufE);
                            slots.push_back({s, e});
                            // 记录到赛程表
                            ScheduleEntry entry;
                            entry.eventId = event.getId();
                            entry.venue = venue;
                            entry.startTime = bufS;
                            entry.endTime = bufE;
                            scheduleEntries.push_back(entry);
                            found = true;
                            scheduled = true;
                            break;
                        }
                        lastEnd = slot.end;
                    }
                    // 检查最后一个空隙
                    if (!found && sessionEnd - lastEnd >= duration) {
                        int s = lastEnd, e = lastEnd + duration;
                        event.setVenue(venue);
                        char bufS[6], bufE[6];
                        snprintf(bufS, 6, "%02d:%02d", s/60, s%60);
                        snprintf(bufE, 6, "%02d:%02d", e/60, e%60);
                        event.setStartTime(bufS);
                        event.setEndTime(bufE);
                        slots.push_back({s, e});
                        ScheduleEntry entry;
                        entry.eventId = event.getId();
                        entry.venue = venue;
                        entry.startTime = bufS;
                        entry.endTime = bufE;
                        scheduleEntries.push_back(entry);
                        scheduled = true;
                        break;
                    }
                    if (scheduled) break;
                }
                if (scheduled) break;
            }
            if (scheduled) break;
        }
        if (!scheduled) {
            std::cout << "错误：项目[" << event.getName() << "]无法排入赛程，请检查场地和时间段设置。" << std::endl;
            return false;
        }
    }
    std::cout << "赛程智能生成成功！共安排" << scheduleEntries.size() << "个项目。" << std::endl;
    settings.lockSchedule();
    return true;
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

// 新增：获取秩序册内容的字符串表示
std::string Schedule::getScheduleContentAsString() const {
    std::ostringstream oss;
    oss << "\n---------- 秩序册 ----------\n"; // 使用中文标点和换行
    if (scheduleEntries.empty()) {
        oss << "秩序册为空或尚未生成。\n"; // 使用中文标点和换行
        return oss.str();
    }
    for (const auto& entry : scheduleEntries) {
        auto eventOpt = settings.getCompetitionEventConst(entry.eventId); // 使用 getCompetitionEventConst
        if (eventOpt) { // 检查 optional 是否有值
            const CompetitionEvent& event_ref = eventOpt.value().get();
            oss << "项目: " << event_ref.getName() << " (ID: " << entry.eventId << ")"
                << "\n  时间: " << entry.startTime << " - " << entry.endTime
                << "\n  地点: " << entry.venue << "\n"; // 使用中文标点和换行
            oss << "  参赛运动员: ";
            if (event_ref.getParticipantCount() > 0) {
                const auto& participantIds = event_ref.getParticipantAthleteIds();
                for (size_t i = 0; i < participantIds.size(); ++i) {
                    auto athOpt = settings.getAthleteConst(participantIds[i]); // 使用 getAthleteConst
                    if (athOpt) { // 检查 optional 是否有值
                        oss << athOpt.value().get().getName() << (i == participantIds.size() - 1 ? "" : ", ");
                    }
                }
            } else {
                oss << "无"; // 使用中文
            }
            oss << "\n\n"; // 两个换行以分隔条目
        }
    }
    oss << "--------------------------\n"; // 使用中文标点和换行
    return oss.str();
}

bool Schedule::validateSchedule() const {
    // 在此实现赛程验证逻辑
    // 例如检查时间冲突、场地冲突等
    std::cout << "正在验证赛程 (当前为占位符)..." << std::endl;
    return true; // 假设总是有效
}
