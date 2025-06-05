//
// Created by GlorYouth on 2025/6/2.
//

#include "../../../include/Components/Core/Schedule.h"
#include <iostream> // 用于打印等
#include <ranges>
#include <sstream>
#include <algorithm> // 为 std::sort 添加此头文件

Schedule::Schedule(SystemSettings& sysSettings) : settings(sysSettings) {}

bool Schedule::generateSchedule() {
    std::cout << "正在智能生成秩序册..." << std::endl;
    scheduleEntries.clear();
    
    // 1. 收集所有未取消项目的ID，而不是创建副本
    std::vector<int> eventIds;
    for (const auto& event : settings.events.getAllConst() | std::views::values) {
        if (!event.get().getIsCancelled()) {
            eventIds.push_back(event.get().getId());
        }
    }
    
    // 按持续时间降序排序项目ID
    std::ranges::sort(eventIds, [&](int a, int b) {
        auto eventA = settings.events.getConst(a);
        auto eventB = settings.events.getConst(b);
        if (eventA.has_value() && eventB.has_value()) {
            return eventA.value().get().getDurationMinutes() > eventB.value().get().getDurationMinutes();
        }
        return false; // 如果ID无效，则保持原顺序
    });
    
    // 2. 获取所有场地
    const auto& venues = settings.venues.getAll();
    if (venues.empty()) {
        std::cout << "错误：场地表为空，请先添加场地。" << std::endl;
        return false;
    }
    
    // 将场地转换为可排序的vector，便于轮询分配
    std::vector<std::string> venueList(venues.begin(), venues.end());
    
    // 3. 获取上午/下午时间段
    auto [morningStart, morningEnd] = settings.sessions.getMorningSession();
    auto [afternoonStart, afternoonEnd] = settings.sessions.getAfternoonSession();
    
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
    
    // 新增：记录每个场地的使用次数，用于均衡分配
    std::map<std::string, int> venueUsageCount;
    for (const auto& venue : venueList) {
        venueUsageCount[venue] = 0;
    }
    
    // 新增：记录每个运动员的赛程安排
    // 格式：运动员ID -> {(天, 开始时间, 结束时间, 项目ID)}
    struct AthleteTimeSlot {
        int day;
        int startMin;
        int endMin;
        int eventId;
    };
    std::map<int, std::vector<AthleteTimeSlot>> athleteSchedules;

    // 5. 依次为每个项目分配时间
    for (int eventId : eventIds) {
        // 获取项目的可修改引用
        auto eventOpt = settings.events.get(eventId);
        if (!eventOpt.has_value()) {
            std::cout << "错误：项目ID " << eventId << " 无效，跳过此项目。" << std::endl;
            continue;
        }
        
        auto& event = eventOpt.value().get(); // 获取原始项目的引用
        int duration = event.getDurationMinutes();
        bool scheduled = false;
        
        // 获取该项目的所有参与运动员
        const std::vector<int>& participantIds = event.getParticipantAthleteIds();
        
        // 尝试从第1天开始，直到找到可排入的空隙
        for (day = 1; day < 100; ++day) { // 最多排100天
            // 对场地进行排序，使用次数少的优先
            std::ranges::sort(venueList, [&](const std::string& a, const std::string& b) {
                return venueUsageCount[a] < venueUsageCount[b];
            });
            
            for (const auto& venue : venueList) {
                for (int isMorning = 0; isMorning <= 1; ++isMorning) {
                    int sessionStart = isMorning ? morningStartMin : afternoonStartMin;
                    int sessionEnd = isMorning ? morningEndMin : afternoonEndMin;
                    auto& slots = venueSchedule[{venue, day, isMorning}];
                    
                    // 查找该场地该天该时段的可用空隙
                    int lastEnd = sessionStart;
                    bool found = false;
                    
                    // 按已排slot排序
                    std::ranges::sort(slots, [](const Slot& a, const Slot& b){ return a.start < b.start; });
                    
                    for (const auto& [start, end] : slots) {
                        if (start - lastEnd >= duration) {
                            // 找到空隙，尝试分配
                            int s = lastEnd, e = lastEnd + duration;
                            
                            // 新增：检查运动员时间冲突
                            bool athleteConflict = false;
                            for (int athleteId : participantIds) {
                                // 检查该运动员在该天是否有冲突的比赛安排
                                if (athleteSchedules.find(athleteId) != athleteSchedules.end()) {
                                    for (const auto& slot : athleteSchedules[athleteId]) {
                                        if (slot.day == day) {
                                            // 检查时间是否重叠
                                            if (!(e <= slot.startMin || s >= slot.endMin)) {
                                                athleteConflict = true;
                                                std::cout << "运动员ID " << athleteId << " 在项目 " 
                                                          << event.getName() << " 和项目ID " << slot.eventId 
                                                          << " 之间存在时间冲突，尝试其他时间段。" << std::endl;
                                                break;
                                            }
                                        }
                                    }
                                    if (athleteConflict) break;
                                }
                            }
                            
                            if (athleteConflict) {
                                // 如果有运动员冲突，继续寻找下一个空隙
                                lastEnd = end;
                                continue;
                            }
                            
                            // 分配 - 现在修改的是原始对象
                            event.setVenue(venue);
                            char bufS[6], bufE[6];
                            snprintf(bufS, 6, "%02d:%02d", s/60, s%60);
                            snprintf(bufE, 6, "%02d:%02d", e/60, e%60);
                            
                            // 设置时间并验证是否成功应用
                            std::string originalStartTime = event.getStartTime();
                            std::string originalEndTime = event.getEndTime();
                            
                            event.setStartTime(bufS);
                            event.setEndTime(bufE);
                            
                            // 检查时间是否成功设置
                            if (event.getStartTime() != bufS || event.getEndTime() != bufE) {
                                std::cerr << "警告：项目 \"" << event.getName() << "\" 的时间设置未成功应用。"
                                          << "可能是由于时间格式验证失败。" << std::endl;
                                continue; // 尝试下一个空隙
                            }
                            
                            slots.push_back({s, e});
                            
                            // 更新场地使用计数
                            venueUsageCount[venue]++;
                            
                            // 记录到赛程表
                            ScheduleEntry entry;
                            entry.eventId = eventId;
                            entry.venue = venue;
                            entry.startTime = bufS;
                            entry.endTime = bufE;
                            scheduleEntries.push_back(entry);
                            
                            // 新增：记录该项目所有运动员的时间安排
                            for (int athleteId : participantIds) {
                                athleteSchedules[athleteId].push_back({day, s, e, eventId});
                            }
                            
                            std::cout << "项目[" << event.getName() << "] 安排在 场地[" << venue 
                                      << "] 时间[" << bufS << "-" << bufE << "]" << std::endl;
                            
                            found = true;
                            scheduled = true;
                            break;
                        }
                        lastEnd = end;
                    }
                    
                    // 检查最后一个空隙
                    if (!found && sessionEnd - lastEnd >= duration) {
                        int s = lastEnd, e = lastEnd + duration;
                        
                        // 新增：检查运动员时间冲突
                        bool athleteConflict = false;
                        for (int athleteId : participantIds) {
                            // 检查该运动员在该天是否有冲突的比赛安排
                            if (athleteSchedules.find(athleteId) != athleteSchedules.end()) {
                                for (const auto& slot : athleteSchedules[athleteId]) {
                                    if (slot.day == day) {
                                        // 检查时间是否重叠
                                        if (!(e <= slot.startMin || s >= slot.endMin)) {
                                            athleteConflict = true;
                                            std::cout << "运动员ID " << athleteId << " 在项目 " 
                                                      << event.getName() << " 和项目ID " << slot.eventId 
                                                      << " 之间存在时间冲突，尝试其他时间段。" << std::endl;
                                            break;
                                        }
                                    }
                                }
                                if (athleteConflict) break;
                            }
                        }
                        
                        if (athleteConflict) {
                            // 如果有运动员冲突，跳过此空隙
                            continue;
                        }
                        
                        // 修改原始对象
                        event.setVenue(venue);
                        char bufS[6], bufE[6];
                        snprintf(bufS, 6, "%02d:%02d", s/60, s%60);
                        snprintf(bufE, 6, "%02d:%02d", e/60, e%60);
                        
                        // 设置时间并验证是否成功应用
                        std::string originalStartTime = event.getStartTime();
                        std::string originalEndTime = event.getEndTime();
                        
                        event.setStartTime(bufS);
                        event.setEndTime(bufE);
                        
                        // 检查时间是否成功设置
                        if (event.getStartTime() != bufS || event.getEndTime() != bufE) {
                            std::cerr << "警告：项目 \"" << event.getName() << "\" 的时间设置未成功应用。"
                                      << "可能是由于时间格式验证失败。" << std::endl;
                            continue; // 尝试其他时间段
                        }
                        
                        slots.push_back({s, e});
                        
                        // 更新场地使用计数
                        venueUsageCount[venue]++;
                        
                        ScheduleEntry entry;
                        entry.eventId = eventId;
                        entry.venue = venue;
                        entry.startTime = bufS;
                        entry.endTime = bufE;
                        scheduleEntries.push_back(entry);
                        
                        // 新增：记录该项目所有运动员的时间安排
                        for (int athleteId : participantIds) {
                            athleteSchedules[athleteId].push_back({day, s, e, eventId});
                        }
                        
                        std::cout << "项目[" << event.getName() << "] 安排在 场地[" << venue 
                                  << "] 时间[" << bufS << "-" << bufE << "]" << std::endl;
                        
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
            std::cout << "错误：项目[" << event.getName() << "]无法排入赛程，请检查场地和时间段设置或运动员参赛项目重叠情况。" << std::endl;
            return false;
        }
    }
    
    // 新增：检查和报告运动员参与多个项目的情况
    std::map<int, int> athleteEventCount;
    for (const auto& [athleteId, slots] : athleteSchedules) {
        athleteEventCount[athleteId] = static_cast<int>(slots.size());
    }
    
    std::vector<std::pair<int, int>> athletesWithMultipleEvents;
    for (const auto& [athleteId, count] : athleteEventCount) {
        if (count > 1) {
            athletesWithMultipleEvents.push_back({athleteId, count});
        }
    }
    
    if (!athletesWithMultipleEvents.empty()) {
        std::cout << "\n多项目参赛运动员情况：" << std::endl;
        for (const auto& [athleteId, count] : athletesWithMultipleEvents) {
            auto athleteOpt = settings.athletes.getConst(athleteId);
            if (athleteOpt.has_value()) {
                std::cout << "运动员 " << athleteOpt.value().get().getName() 
                          << " (ID: " << athleteId << ") 参加 " << count << " 个项目" << std::endl;
            }
        }
    }
    
    // 打印场地使用情况统计
    std::cout << "\n场地使用统计：" << std::endl;
    for (const auto& [venue, count] : venueUsageCount) {
        std::cout << "场地[" << venue << "] 使用 " << count << " 次" << std::endl;
    }
    
    std::cout << "赛程智能生成成功！共安排" << scheduleEntries.size() << "个项目。" << std::endl;
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
        if (auto event = settings.events.get(entry.eventId); event.has_value()) {
            auto event_ref = event.value().get();
            std::cout << "项目: " << event_ref.getName() << " (ID: " << entry.eventId << ")"
                      << "\n  时间: " << entry.startTime << " - " << entry.endTime
                      << "\n  场地: " << entry.venue << std::endl;
            std::cout << "  参赛人员: ";
            if (event_ref.getParticipantCount() > 0) {
                for(size_t i = 0; i < event_ref.getParticipantAthleteIds().size(); ++i) {
                    auto ath = settings.athletes.get(event_ref.getParticipantAthleteIds()[i]);
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
        auto eventOpt = settings.events.getConst(entry.eventId); // 使用 getCompetitionEventConst
        if (eventOpt) { // 检查 optional 是否有值
            const CompetitionEvent& event_ref = eventOpt.value().get();
            oss << "项目: " << event_ref.getName() << " (ID: " << entry.eventId << ")"
                << "\n  时间: " << entry.startTime << " - " << entry.endTime
                << "\n  地点: " << entry.venue << "\n"; // 使用中文标点和换行
            oss << "  参赛运动员: ";
            if (event_ref.getParticipantCount() > 0) {
                const auto& participantIds = event_ref.getParticipantAthleteIds();
                for (size_t i = 0; i < participantIds.size(); ++i) {
                    auto athOpt = settings.athletes.getConst(participantIds[i]); // 使用 getAthleteConst
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
